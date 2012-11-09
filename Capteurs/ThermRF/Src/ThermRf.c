/*
    This file is part of ThermRF.

    ThermRF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ThermRF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2011  Alexandre Coffignal <alexandre.coffignal[@]gmail.com>

*/
#include <pic16f628a.h>
#include <string.h>
#include "serial.h"
#include "sw_i2c.h"
/* ----------------------------------------------------------------------- */
/* Bits de configuration: adapté à votre dispositif et à vos besoins */
typedef unsigned int word;
word at 0x2007 CONFIG = _WDT_ON & _PWRTE_OFF & _INTOSC_OSC_NOCLKOUT & _MCLRE_ON & _BOREN_ON & _LVP_ON & _DATA_CP_OFF & _CP_OFF;


#define RFID	0

#define TRIS_ALIM_DS	TRISA1
#define ALIM_DS		RA1

#define TRIS_ALIM_RF	TRISA2
#define ALIM_RF		RA2

#define NB_TRANSMIT	5


#define sleep() __asm sleep __endasm




void isr() interrupt 0 {                                                                                                  /* routine de service des interruptions */
    /* << insérer le code d'interruption >*/
	serial_vPrintln("it");


}

void setup_wdt(void);
void sleep5min(void);
void setup(void);
void run(unsigned char temp);
void rfTransmit(char * buf);
char* itoa (unsigned int n,char u[10]);
void mystrcat(char * s1, const char * s2);

/**
@author: Mathias Van Malderen (tux4life)
*/

void mystrcat(char * s1, const char * s2)
{
	while(*s1) s1++;
	for(; *s1 = *s2; s1++, s2++);

}

char* itoa (unsigned int n,char u[10]){
  int i=0,j;
  char s[10];
  

  do{
    s[i++]=(char)( n%10 + 48 );
    n-=n%10;
  }
  while((n/=10)>0);
  for (j=0;j<i;j++)
  u[i-1-j]=s[j];

  u[j]='\0';
  return u;
}



void rfTransmit(char * buf)
{
	int i;
	unsigned char checksum=0;
	char temp[10];
	pingWdt();
	setup_wdt();	// clrwdt resets WDT parameters to default
	ALIM_RF=1;
	for(i=0;buf[i]!='\0';i++)
	{
		checksum+=buf[i];
	}
	
	itoa (checksum,temp);
	
	for(i=0;i<NB_TRANSMIT;i++)
	{
		serial_vPrint("TH ");
		serial_vPrint(buf);
		serial_vPrint(" ");
		serial_vPrint(temp);
		serial_vPrint("\n\r");
	}

	ALIM_RF=0;
}






void run(unsigned char temp)
{
		unsigned char ms;
		unsigned char ls;
		unsigned int msls;
		long i;
		char buf[10];
		char buf2[10];
#ifdef DEBUG
		unsigned long frac;
#endif
		ALIM_DS=1;
	pingWdt();
	setup_wdt();	// clrwdt resets WDT parameters to default
		for(i=0;i<20;i++)
			I2cDelay10us();
		I2cStart();
		I2cTxByte(0x91+temp);
		I2cTxByte(0xAC);
		I2cTxByte(0x06);	//mode 12 bit + continious
		I2cStop();

	pingWdt();
	setup_wdt();	// clrwdt resets WDT parameters to default	
		for(i=0;i<100;i++)
			I2cDelay10us();
		I2cStart();
		I2cTxByte(0x90+temp);
		I2cTxByte(0x51);	//start conv
		I2cStop();
#ifdef DEBUG
		serial_vPrintln("Start conv");
#endif
	pingWdt();
	setup_wdt();	// clrwdt resets WDT parameters to default
		for(i=0;i<2000;i++)
			I2cDelay50us();
	pingWdt();
	setup_wdt();	// clrwdt resets WDT parameters to default
#ifdef DEBUG
		serial_vPrintln("Read conv");
#endif
		I2cStart();
		I2cTxByte(0x90+temp);
		I2cTxByte(0xAA);
		I2cStart();
		I2cTxByte(0x91+temp);
		ms=I2cRxByte(0);
		ls=I2cRxByte(1);
		I2cStop();
		ALIM_DS=0;

#ifdef DEBUG
		serial_vPrint(itoa (ms,buf));
		serial_vPrint(",");
		frac=(ls/16)*625;
		if(frac<1000)
		serial_vPrint("0");
		serial_vPrint(itoa (frac,buf));
		serial_vPrintln(" ");
#endif
		msls=(unsigned int) ms*256+ls;
		
		itoa (RFID,buf);		
		mystrcat(buf," ");
		itoa (msls,buf2);
		mystrcat(buf,buf2);
		rfTransmit(buf);	

}




void setup_wdt(void)
{

	/**
	 * This could done in one line with OPTION_REG=0x0d, but
	 * this is an example, so we show you exactly what bits we're 
	 * setting to what why.
	 */
	PSA=1;	// WDT uses the prescaler, not the timer
	/**
	 *	PS2 PS1 PS0
	 *	0   0   0	=	1:1
	 *	0   0   1	=	1:2
	 *	0   1   0	=	1:4
	 *	0   1   1	=	1:8
	 *	1   0   0	=	1:16
	 *	1   0   1	=	1:32
	 *	1   1   0	=	1:64
	 *	1   1   1	=	1:128
	 */
	PS2=1;	
	PS1=1;
	PS0=1;
}

void sleep5min(void)
{
	unsigned char i;
	for(i=0;i<130;i++)
	{
		sleep();
	}
}

void setup(void)
{
	setup_wdt();
	serial_vInit();
	CMCON=0x07;			//Turn comparators off and enable pins for I/O functions
	TRIS_ALIM_DS=0;			//Turn ALIM DS pin output
	TRIS_ALIM_RF=0;			//Turn ALIM DS pin output
	ALIM_DS=0;			//Disable thermo chip
	ALIM_RF=0;			//Disable RF transceiver
	
}
void main(void)
{
	static unsigned char i;
	static  char ms,ls;
	setup();
	serial_vPrintln("start");
	for(i=0;i<20;i++)
		I2cDelay10us();

	while(1)
	{
		run(14);		//Measure and send temperature
		sleep5min();		//Idle Pic
	}
}


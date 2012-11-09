
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
#include <pic16f88.h>
#include "serial.h"
#include "io.h"

#define MAX_BUF_RCV	8
#if	(RX_INTERRUPT == 1)
unsigned char ucIdxRead=0;
unsigned char ucIdxWrite=0;
char cFull=0;
char cEmpty=1;
char acDataRing[MAX_BUF_RCV];
unsigned char ucRing_pointer = 0;



void serial_intr() interrupt 0 
{
	unsigned char ucRst;
	if(RCIF) 
	{ 
		// serial received
		if(!cFull)
		{	//buffer pas plein 
			acDataRing[ucIdxWrite] = RCSTA;
			ucIdxWrite++;
			if(ucIdxWrite>=MAX_BUF_RCV)
			{
				ucIdxWrite=0;	//on a fait un tour
			}
			if(ucIdxWrite==ucIdxRead)
			{
				cFull=1;	//buffer plein on arrete la reception
			}
			cEmpty=0;
		}

		// check for errors here?
		// RCREG;
		// if we got an error we should clear CREN
		CREN = 0;
		// re-enable receiveing
		CREN = 1;
	}
}
#endif
#if	(RX_INTERRUPT == 1)
char serial_getData(void)
{
	char cRet=-1;
	if(!cEmpty)
	{
		cRet=acDataRing[ucIdxRead++]; 
		if(ucIdxRead>=MAX_BUF_RCV)
		{
			ucIdxRead=0;	//on a fait un tour
		}

		if(ucIdxWrite==ucIdxRead)
		{
			cEmpty=1;	//buffer vide 
		}
		cFull=0;
	}
	return cRet;
}
#else
char serial_getData(void)
{
	char cRet=-1;
	if(RCIF)	// Wait until data recieved
	{
		cRet=RCREG;	// Store for later
		// if we got an error we should clear CREN
		CREN = 0;
		// re-enable receiveing
		CREN = 1;
	}
	return cRet;
}
#endif
void serial_vInit()
{
	CONFIG_RX;		// These need to be 1 for USART to work
	CONFIG_TX;	
	
	SPBRG=SPBRG_VALUE;	// Baud Rate register, calculated by macro
	BRGH=BAUD_HI;

	SYNC=0;			// Disable Synchronous/Enable Asynchronous
	SPEN=1;			// Enable serial port
	
	TXEN=1;			// Enable transmission mode
	CREN=1;			// Enable reception mode
#if	(RX_INTERRUPT == 1)	
	RCIE=1;			// Enable Rcx interrupt
	//todo if using other interrupt remove that line
	GIE=1;			//Enable global IT 
 	PEIE = 1;		//Enable Peripheral Int. 
#endif
}
//#ifdef DEBUG

void serial_vPrintUint(unsigned int n, unsigned char base)
{
	unsigned char buf[5]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) 
	{
		serial_vPut('0');
		return;
	} 

	while (n > 0) 
	{
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
	{
		serial_vPut((char) (buf[i - 1] < 10 ?
				'0' + buf[i - 1] :
				'A' + buf[i - 1] - 10));
	}
}

void serial_vPrintInt(int n, unsigned char base)
{
	if (base == 0) 
	{
		serial_vPut(n);
	} 
	else 
	{
		if (base == 10)
		{
			if (n < 0) 
			{
				serial_vPut('-');
				n = -n;
			}
		} 
		serial_vPrintUint(n, base);
	}
}

//#endif

void serial_vPrint(char * buf)
{
	char i=0;
	while(buf[i]!='\0')
	{
		serial_vPut(buf[i++]);
	}
}
void serial_vPrintln(char * buf)
{
	serial_vPrint(buf);
	serial_vPut('\n');
	serial_vPut('\r');
}



void serial_vPut(char c)
{
	TXREG = c;
	TXEN = 1;    // Envoie le caractere
	while(!TRMT);
}


char* itoa (unsigned int n,char u[10])
{
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
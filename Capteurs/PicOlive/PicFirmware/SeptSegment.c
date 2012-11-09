/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: SeptSegment.c
 * Description:  display message on the 7segment LEDs through
 * GPIOs of 18F4520. The 8 bits data output of PORTD were connected 
 * to a-h of 7segment LEDs, and PORTB4-7 were connected to base of ULN2003, 
 * then drive 4 cathode of those LEDs.
 * Base on DB-DP113_o.pdf from Sure Electronics
 * @author Alexandre Coffignal, Copyright (C) 2010.
 * @see The GNU Public License (GPL)
 */
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include <p18f4520.h>
#include <delays.h>


#define LED0 PORTDbits.RD0
#define LED1 PORTDbits.RD1
#define LED2 PORTDbits.RD2
#define LED3 PORTDbits.RD3
#define LED4 PORTDbits.RD4
#define LED5 PORTDbits.RD5
#define LED6 PORTDbits.RD6
#define LED7 PORTDbits.RD7

#define SEPTSEG	PORTD
#define SELSEG	PORTB

#define LEDbuf0 PORTBbits.RB4
#define LEDbuf1 PORTBbits.RB5
#define LEDbuf2 PORTBbits.RB6
#define LEDbuf3 PORTBbits.RB7

unsigned char acNum[] ={ 0x3F,0x06,0x5B,0x4f,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

// Initial
void seg_init(void)
{
	CMCON=0b00000111; // Close Comparator
	//TRISA=0b00010000;
	TRISB&=~(0b11110000);
	//TRISB=0b00000001;
	TRISC=0b11000000;
	TRISD=0b00000000;
	//TRISE=0b00001000;
	//ADCON1=0b00001111; // Configure Digital Channel
}


void selSeg(char num)
{
	
	unsigned char mask=1<<(num+4);
	SELSEG&=0x0F;
	SELSEG|=mask;
}

void seg_set_num(unsigned char ucNum, unsigned char ucDigit,char cDecPoint)
{
	selSeg(ucNum);
	if(cDecPoint)
		SEPTSEG=acNum[ucDigit]|0x80;
	else
		SEPTSEG=acNum[ucDigit];
	
	
	Delay100TCYx(2);
}

void seg_run(unsigned int uiNum)
{
	seg_set_num(0,(uiNum&0x000F)>>0,0);
	seg_set_num(1,(uiNum&0x00F0)>>4,0);
	seg_set_num(2,(uiNum&0x0F00)>>8,0);
	seg_set_num(3,(uiNum&0xF000)>>12,0);
}

void seg_off()
{
	SELSEG&=0x0F;
}



void seg_time(unsigned int uiH,unsigned int uiM,char cDecPoint)
{

	seg_set_num(0,uiM%10,0);
	seg_set_num(1,uiM/10,0);
	seg_set_num(2,uiH%10,cDecPoint%2);
	seg_set_num(3,uiH/10,0);
}
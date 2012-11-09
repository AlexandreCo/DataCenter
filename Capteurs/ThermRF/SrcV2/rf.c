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

    Copyright (C) 2012  Alexandre Coffignal <alexandre.coffignal[@]gmail.com>

*/
#include <pic16f88.h>
#include "rf.h"
#include "io.h"
#include "serial.h"
#include "wdt.h"

void mystrcat(char * s1, const char * s2);

/**
@author: Mathias Van Malderen (tux4life)
*/

void mystrcat(char * s1, const char * s2)
{
	while(*s1) s1++;
	for(; *s1 = *s2; s1++, s2++);

}
/**
******************************************
*/



void rfTransmit(char * acBuf)
{
	int i;
	unsigned char ucChecksum=0;
	char acChkSum[4];
	pingWdt();
	
	for(i=0;acBuf[i]!='\0';i++)
	{
		ucChecksum+=acBuf[i];
	}
	
	itoa (ucChecksum,acChkSum);
	
	for(i=0;i<NB_TRANSMIT;i++)
	{
		serial_vPrint("TH ");
		serial_vPrint(acBuf);
		serial_vPrint(" ");
		serial_vPrint(acChkSum);
		serial_vPrint("\n\r");
	}

}


void addUint(char * buf,unsigned int uiNumber)
{
	char acBufTemp[5];
	itoa (uiNumber,acBufTemp);
	mystrcat(buf,acBufTemp);
}


void rf_vTransmit(unsigned int uiTemp,unsigned int uiBat)
{
	char buf[17];
²	char i=0;
	unsigned int uiRfId=RFID;
	for(i=0;i<17;i++)
	{
		buf[i]='\0';
	}
	
	if(INPUT_TEST)
	{
		uiRfId=666;	
	}
	addUint(buf,uiRfId);
	mystrcat(buf," ");

	addUint(buf,uiTemp);
	mystrcat(buf," ");

	addUint(buf,uiBat);
	rfTransmit(buf);
}

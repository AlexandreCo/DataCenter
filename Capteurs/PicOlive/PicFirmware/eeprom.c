/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: eeprom.c
 * Description:  write / read data in i2c eeprom
 *   
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
#include <delays.h>
#include <sw_i2c.h>
#include <stdio.h>
/**************************************************************************/
// Write data
void EEPROM_vByteWrite(unsigned char adr,unsigned char data)
{
#ifndef EEPROM_READ_ONLY
	SWStartI2C();
	SWPutcI2C(0xA0); 		// Control byte
	SWAckI2C();
	SWPutcI2C(adr);		// Word address
	SWAckI2C();
	SWPutcI2C(data); 		// Data
	SWAckI2C();
	SWStopI2C();
#endif
}




// Read data
unsigned char EEPROM_ucByteRead(unsigned char adr)
{
	unsigned char ucRet;
	SWStartI2C();
	SWPutcI2C( 0xA0 ); 		// Control byte
	SWAckI2C();
	SWPutcI2C(adr); 		// Word address
	SWAckI2C();
	SWRestartI2C();
	SWPutcI2C( 0xA1 );		// Control byte
	SWAckI2C();
	ucRet = SWGetcI2C();//data
	SWStopI2C();
	return ucRet;

}

// Write string
void EEPROM_vPageWrite(unsigned char adr,unsigned char wdata[])
{
#ifndef EEPROM_READ_ONLY
	SWStartI2C();
	SWPutcI2C(0xA0); 		// Control byte
	SWAckI2C();
	SWPutcI2C(adr); 		// Word address
	SWAckI2C();
	SWPutsI2C(wdata);		// Data
	SWStopI2C();
#endif

}

// Read string

void EEPROM_vSequentialRead(unsigned char adr,unsigned char rdata[],unsigned char len)
{
	SWStartI2C();
	SWPutcI2C( 0xA0 );		// Control byte
	SWAckI2C();
	SWPutcI2C(adr); 		// Word address
	SWAckI2C();
	SWRestartI2C();
	SWPutcI2C( 0xA1 );		// Control byte
	SWAckI2C();
	SWGetsI2C(rdata,len);		// Data
	SWStopI2C();
}

// Inquiries confirmed 

void EEPROM_vAckPoll( void )
{
	SWStartI2C();
	SWPutcI2C( 0xA0 ); 		// Control byte
	while( SWAckI2C() )
	{
		SWRestartI2C();
		SWPutcI2C(0xA0); 	// Data
	}
	SWStopI2C();
}

//return 2xTemp
int EEPROM_iRead(unsigned char ucIndex)
{
	int iTemp;
	unsigned int  uiTempH,uiTempL;
	Delay10KTCYx(5);
	uiTempH=	EEPROM_ucByteRead(ucIndex);Delay10KTCYx(5);
	uiTempL=	EEPROM_ucByteRead(ucIndex+1);Delay10KTCYx(5);
	iTemp=(uiTempH<<8)|uiTempL;
#ifdef DEBUG
	printf("EERPROM read %d %d\n\r",iTemp,ucIndex);
#endif
	return iTemp;
}

//arg :  2xTemp

void EEPROM_vWrite(int iTemp,unsigned char ucIndex)
{
#ifndef EEPROM_READ_ONLY

	Delay10KTCYx(5);
	EEPROM_vByteWrite(ucIndex,iTemp>>8);	Delay10KTCYx(5);
	EEPROM_vByteWrite(ucIndex+1,iTemp&0xFF);	Delay10KTCYx(5);
#endif

#ifdef DEBUG
	printf("EERPROM write %d %d\n\r",iTemp,ucIndex);
#endif
}

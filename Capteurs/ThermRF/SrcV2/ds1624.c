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
#include "io.h"
#include "sw_i2c.h"
#include "serial.h"


void vSetAccess(char cNum)
{
	I2cStart();
	I2cTxByte(0x91+cNum);
	I2cTxByte(0xAC);
	I2cTxByte(0x4A);	
	I2cStop();
}

void vStartConv(char cNum)
{
	I2cStart();
	I2cTxByte(0x90+cNum);
	I2cTxByte(0xEE);	//start conv
	I2cStop();
}

unsigned int uiReadConv(char cNum)
{
	unsigned char ucMs=0;	
	unsigned char ucLs=0;
	unsigned int uiRes;	
	I2cStart();
	I2cTxByte(0x90+cNum);
	I2cTxByte(0xAA);
	I2cStart();
	I2cTxByte(0x91+cNum);
	ucMs=I2cRxByte(0);
	ucLs=I2cRxByte(1);
	I2cStop();
	uiRes=(unsigned int) ucMs*256+ucLs;
	return uiRes;
}


unsigned char ucReadStat(char cNum)
{
	unsigned char ucRes=0;	
	I2cStart();
	I2cTxByte(0x90+cNum);
	I2cTxByte(0xAC);

	I2cStart();
	I2cTxByte(0x91+cNum);
	ucRes=I2cRxByte(1);
	I2cStop();
	return ucRes;
}

void ds1624_vInit(char cNum)
{
	ALIM_DS_ON;
	vSetAccess(cNum);
	I2cDelay10ms();
	ALIM_DS_OFF;
}

unsigned int ds1624_uiRead(char cNum)
{
	unsigned int uiRes=0;
	unsigned char ucStat=0;;

	ALIM_DS_ON;
	vStartConv(cNum);
	I2cDelay1s();
	uiRes=uiReadConv(cNum);
	ALIM_DS_OFF;
	return uiRes;
}

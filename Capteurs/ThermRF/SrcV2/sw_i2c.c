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
#include "sw_i2c.h"
#include "wdt.h"
#include "io.h"
void I2cDelay20us()
{
	long j;
	for(j=0;j<1;j++)
	{
		
	}
	pingWdt();
}

void I2cDelay40us()
{
	I2cDelay20us();
	I2cDelay20us();

}

void I2cDelay1ms()
{
	char i;
	for(i=0;i<60;i++)
		I2cDelay20us();
}

void I2cDelay10ms()
{
	char i;
	for(i=0;i<10;i++)
		I2cDelay1ms();
}

void I2cDelay1s()
{
	char i;
	for(i=0;i<100;i++)
		I2cDelay10ms();

}
/*
 Set the SCL line high by making it an input pin and
 then waiting 50us.
*/
void I2cSclHi()
{
	SCL_HIGH;
	I2cDelay40us();
}

void I2cSclLo()
{
	SCL_LOW;
	I2cDelay40us();
}

/*
 Set the SDA line high by making it an input pin and
 then waiting 50us.
*/
void I2cSdaHi()
{
	I2C_SDA_HIGH;
	I2cDelay40us();
}

void I2cSdaLo()
{
	I2C_SDA_LOW;
	I2cDelay40us();
}

void I2cStart()
{
	I2cSclLo();
        I2cSdaHi();
        I2cSclHi();
        I2cSdaLo();
        I2cSclLo();
}

void I2cStop()
{
	I2cSclLo();
	I2cSdaLo();
	I2cSclHi();
	I2cSdaHi();
}

void I2cAck()
{
	I2cSdaLo();
	I2cSclHi();
	I2cSclLo();
        I2cSdaHi();
}

void I2cNack()
{
	I2cSdaHi();
        I2cSclHi();
        I2cSclLo();
}


char I2cRxBit()
{
	char c;
	I2cSclHi();
        c=I2C_SDA;
	I2cSclLo();

	return c;
}


unsigned char I2cRxByte(char ack)
{
	unsigned char byte =0,i;
	I2cSdaHi();
        for(i=0; i<8; i++)
	{
		byte <<= 1;
		if(I2cRxBit())
			byte |= 1;
	}
	
	if(ack)
		I2cNack();
	else
		I2cAck();
	return byte;	
}


void I2cTxBit(char bit)
{
	if(bit)
		I2cSdaHi();
	else
		I2cSdaLo();
	I2cSclHi();
	I2cSclLo();
}
void I2cTxByte (unsigned char byte)
{
	unsigned char i;
	for(i=0; i<8; i++) 
	{
		if(byte&0x80) 
			I2cTxBit(1);
		else 
			I2cTxBit(0);
		byte <<= 1;
	}
	I2cSdaHi();	//ack du composant
	I2cSclHi();
	I2cSclLo();
}


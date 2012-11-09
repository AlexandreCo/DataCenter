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
#include "serial.h"
#include "sw_i2c.h"
void I2cDelay10us()
{
	long i,j;
	for(j=0;j<10;j++)
	{
		
	}
	pingWdt();
}

void I2cDelay50us()
{
	I2cDelay10us();
	I2cDelay10us();
	I2cDelay10us();
	I2cDelay10us();
	I2cDelay10us();
}



/*
 Set the SCL line high by making it an input pin and
 then waiting 50us.
*/
void I2cSclHi()
{
	TRIS_SCL=ENTREE;	//Entree
	I2cDelay50us();
}

void I2cSclLo()
{
	I2C_SCL=0;
	TRIS_SCL=SORTIE;	//Sortie
	I2cDelay50us();
}

/*
 Set the SDA line high by making it an input pin and
 then waiting 50us.
*/
void I2cSdaHi()
{
	TRIS_SDA=ENTREE;	//Entree
	//serial_vPrint("1");
	I2cDelay50us();
}

void I2cSdaLo()
{
	I2C_SDA=0;
	TRIS_SDA=SORTIE;	//Sortie
	//serial_vPrint("0");
	I2cDelay50us();
}

void I2cStart()
{
	//serial_vPrint("S");
	I2cSclLo();
        I2cSdaHi();
        I2cSclHi();
        I2cSdaLo();
        I2cSclLo();
	//serial_vPrint("s");
}

void I2cStop()
{
	//serial_vPrint("P");
	I2cSclLo();
	I2cSdaLo();
	I2cSclHi();
	I2cSdaHi();
	//serial_vPrintln("p");
}

void I2cAck()
{
	//serial_vPrint("A");
	I2cSdaLo();
	I2cSclHi();
	I2cSclLo();
        I2cSdaHi();
	//serial_vPrint("a");
}

void I2cNack()
{
	//serial_vPrint("N");
	I2cSdaHi();
        I2cSclHi();
        I2cSclLo();
	//serial_vPrint("n");
}


char I2cRxBit()
{
	char c;
	I2cSclHi();
        c=I2C_SDA;
	I2cSclLo();
/*	if(c)
		serial_vPrint("I");
	else
		serial_vPrint("O");
*/
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


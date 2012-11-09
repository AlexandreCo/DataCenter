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
#ifndef _SWI2C
#define _SWI2C

void I2cStart();
void I2cStop();
unsigned char I2cRxByte(char ack);
void I2cTxByte(unsigned char byte);
void I2cDelay10us();
void I2cDelay50us();

#define TRIS_SCL	TRISA7
#define TRIS_SDA	TRISA4
#define I2C_SCL		RA7
#define I2C_SDA		RA4

#define ENTREE		1
#define SORTIE		0

#define pingWdt() __asm clrwdt __endasm;	// It has its own special instruction
#endif
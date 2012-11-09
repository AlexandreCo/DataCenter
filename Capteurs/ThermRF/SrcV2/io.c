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
void io_vInit(void)
{
	CMCON=0x07;				//Turn comparators off and enable pins for I/O functions

	CONFIG_ALIM_RF;			//Turn ALIM RF pins output
	CONFIG_ALIM_DS;			//Turn ALIM DS pin output
	CONFIG_LED;			//Turn LED pin output
	CONFIG_INPUT_BAT;		//Turn BAT pin input
	CONFIG_INPUT_TEST;		//Turn TEST pin input
	
	ALIM_DS_OFF;			//Disable thermo chip
	ALIM_RF_OFF;			//Disable RF transceiver
	LED_OFF;			//clear LED

	ANSEL=0x01;			//RA0 analog input
	ADCON1=0x80;			//justifié a droite ref=vcc/gnd
	ADCON0=0x81;			//12 TAD = 48uS PA0 ADC ON
	
}

unsigned int io_startConv(char channel)
{
	unsigned int res=0;
	unsigned char mask = channel <<5;
	ADCON0|=(mask+1);			//start conv
	I2cDelay20us();				//wait acqui
	GO=1;				//start conv
	while(GO);			//wait fin conv
	res=ADRESH*256+ADRESL;
	return res;
}
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
#include <string.h>

#include "serial.h"
#include "sw_i2c.h"
#include "io.h"
#include "wdt.h"
#include "ds1624.h"
#include "rf.h"
/* ----------------------------------------------------------------------- */
/* Bits de configuration: adapté à votre dispositif et à vos besoins */
typedef unsigned int word;

#ifdef _NO_WDT
word at _CONFIG1 CONFIG1 = _WDT_OFF & _PWRTE_OFF & _INTRC_IO & _MCLR_OFF & _BODEN_OFF & _LVP_OFF & _CPD_OFF &  _WRT_PROTECT_OFF & _DEBUG_OFF & _CCP1_RB0 & _CP_OFF;
#else
word at _CONFIG1 CONFIG1 = _WDT_ON & _PWRTE_OFF & _INTRC_IO & _MCLR_OFF & _BODEN_OFF & _LVP_OFF & _CPD_OFF &  _WRT_PROTECT_OFF & _DEBUG_OFF & _CCP1_RB0 & _CP_OFF;
#endif
//_INTRC_CLKOUT permet de sortir fck/4 sinon _INTRC_IO
word at _CONFIG2 CONFIG2 = _FCMEN_ON & _IESO_ON;



void setup(void);
void run(void);

void run(void)
{
	unsigned int uiData=0;
	unsigned int uiBat=0;

	uiBat= io_startConv(0);		//~U bat/5 (10bits)
	uiData=ds1624_uiRead(DS_TH);	//(13bits)
	ALIM_RF_ON;
	rf_vTransmit(uiData,uiBat);
	ALIM_RF_OFF;
	LED_ON;
	wdt_vSleep5min();		//Idle Pic*/
}

void setup(void)
{
	//osc
	OSCCON=0x70; //oscilateur interne a 8Mhz
	wdt_vInit();
	serial_vInit();
	io_vInit();
	ds1624_vInit(DS_TH);
}

void main(void)
{

	setup();
	
	while(1)
	{
		run();
	}
}


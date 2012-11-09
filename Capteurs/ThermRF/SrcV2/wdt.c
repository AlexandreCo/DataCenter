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
#include "wdt.h"
#include "serial.h"
#include "io.h"


void wdt_vInit(void)
{
#ifndef _NO_WDT
	/**
	 * This could done in one line with OPTION_REG=0x0d, but
	 * this is an example, so we show you exactly what bits we're 
	 * setting to what why.
	 */
	PSA=1;	// WDT uses the prescaler, not the timer
	/**
	 *	PS2 PS1 PS0
	 *	0   0   0	=	1:1
	 *	0   0   1	=	1:2
	 *	0   1   0	=	1:4
	 *	0   1   1	=	1:8
	 *	1   0   0	=	1:16
	 *	1   0   1	=	1:32
	 *	1   1   0	=	1:64
	 *	1   1   1	=	1:128
	 */
	PS2=1;	
	PS1=1;
	PS0=1;
#endif
}

void wdt_vSleep5min(void)
{
	unsigned char i;
	for(i=0;i<130;i++)
	{
#ifndef _NO_WDT

		sleep();
		LED_OFF;
		if(INPUT_TEST)
		{
			//mode test actif
			break;
		}
#endif
	}
}

void wdt_vSleep2s(void)
{
#ifndef _NO_WDT
	sleep();
#endif
}
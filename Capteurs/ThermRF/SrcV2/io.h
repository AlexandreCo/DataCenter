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
#ifndef IO
#define IO
#include "time.h"

#define ENTREE		1
#define SORTIE		0

/***********************************
 *	         PORTA
 ***********************************/

//RA0 (MESURE BAT)
#define CONFIG_INPUT_BAT	(TRISA0=1)
#define INPUT_BAT		(RA0)

//RA1 (ALIM DS)
#define CONFIG_ALIM_DS		(TRISA1=0)
#define ALIM_DS_ON		(RA1=1)
#define ALIM_DS_OFF		(RA1=0)


#define NB_ALIM_RF	2
#if	(NB_ALIM_RF == 1)
//RA2 (ALIM RF 1)
#define CONFIG_ALIM_RF	(TRISA2=0)
#define ALIM_RF_ON	(RA2=1)
#define ALIM_RF_OFF	(RA2=0)
#else
	#if	(NB_ALIM_RF == 2)
//RA3 (ALIM RF 2)
	#define CONFIG_ALIM_RF	(TRISA=TRISA & ~(0x0C))
	#define ALIM_RF_ON	(PORTA=PORTA |   0x0C)
	#define ALIM_RF_OFF	(PORTA=PORTA & ~(0x0C))
	#else
//RA4 (ALIM RF 3)
	#define CONFIG_ALIM_RF	(TRISA=TRISA & ~(0x1C))
	#define ALIM_RF_ON	(PORTA=PORTA |   0x1C)
	#define ALIM_RF_OFF	(PORTA=PORTA & ~(0x1C))	
	#endif
#endif

//RA5 NC (MCLR)

//RA6 (BP1)
#define CONFIG_INPUT_TEST	(TRISA6=1)
#define INPUT_TEST		(RA6)

//RA7 (CLKI)

/***********************************
 *	         PORTB
 ***********************************/

//RB0 NC (INT0)

//RB1 I2C 
#define I2C_SDA_HIGH	(TRISB1=ENTREE)
#define I2C_SDA_LOW	(RB1=0);(TRISB1=SORTIE)
#define I2C_SDA		(RB1)

//RB2 RX
#define CONFIG_RX	(TRISB2=1)

//RB3 LED
#define CONFIG_LED	(TRISB3=0)
#define LED_ON		RB3=1
#define LED_OFF		RB3=0

//RB4 I2C
#define SCL_HIGH	(TRISB4=ENTREE)
#define SCL_LOW		(RB4=0);(TRISB4=SORTIE)

//RB5 TX
#define CONFIG_TX	(TRISB5=0)

//RB6 NC (PGC)

//RB7 NC (PGD)



void io_vInit(void);
unsigned int io_startConv(char channel);


#endif
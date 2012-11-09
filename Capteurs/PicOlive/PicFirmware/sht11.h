/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: sht11.h
 * Description:  SHT11 on PIC18F4580 Get Temperature and humidity from SHT11 sensor
 *   
 * @author Alexandre Coffignal, Copyright (C) 2009.
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

#ifndef	__SHT11_H__
#define	__SHT11_H__

#define D1 400				//Datasheet SHT11
#define D2 10				//Datasheet SHT11
#define TEMP_BASE 25			//SHT11 datasheet T base in C
#define DELAY_SHT 200			//SHT11 CLK delay
#define DELAY_1MS	225
#define TRIS_data TRISAbits.TRISA1			//Sensor SHT11 data
#define SHT_DATA PORTAbits.RA1				//Sensor SHT11 data
#define TRIS_clock TRISAbits.TRISA3			//Sensor SHT11 clock
#define SHT_CLOCK PORTAbits.RA3				//Sensor SHT11 clock


#define MIN_TEMP				-50
#define MIN_HUMI				0
#define MIN_ERROR				-100
#define ERROR_TIMEOUT_MEASURE	(MIN_ERROR)

#define TIMEOUT_500MS	500	//~55ms for humidity

//
//	PROTOTYPES
//

int SHT11_iGetTemp(void);
char SHT11_cGetHumi(int iT_c);
void SHT11_vSetup(void);

#endif

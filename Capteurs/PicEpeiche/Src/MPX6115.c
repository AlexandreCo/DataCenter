/**
 * Project: PicEpeiche 
 * File name: MPX6115.c
 * Description:  get presure sample on MPX6115 sensor
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

#include <adc.h>
#include <delays.h>
unsigned int MPX6115_uiGetPression(void)
{
	int iResult; 
	unsigned long ulPression;
	OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_12_TAD, ADC_CH0 & ADC_INT_OFF, 0); //open adc port for reading
	ADCON1 =0x00; //set VREF+ to VDD and VREF- to GND (VSS)
	SetChanADC(ADC_CH5); //Set ADC to Pin 5
	Delay1KTCYx(5);
	ConvertADC();
	while( BusyADC() );
	iResult = ReadADC();
	CloseADC();
	ulPression=((unsigned long)iResult*5000/1024 + 475)*10 /45;
#ifdef DEBUG
	printf("Pression %ld %x\n\r",ulPression,iResult);
#endif
	
	ADCON1=0b00001111;			// Digital Channel Allocation 
	return (unsigned int)ulPression;
}
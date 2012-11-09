/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: lm75.c
 * Description:  get tempeature sample on LM75 sensor
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

#include <sw_i2c.h>
#include "LM75.h"
void LM75_vInit(void)			// Temperature Sensor Initializtion
{
	SWStartI2C();
	SWPutcI2C(ADDRESS_BYTE+WRITE);		// Control byte
	SWAckI2C();
	SWPutcI2C(PTR_CONFIG);		// Configure register
	SWAckI2C();
	SWPutcI2C(CFG_6FAULT);		// Configure byte
	SWAckI2C();
	SWStopI2C();
}


//return 2xTemp
int LM75_iGetTemp(void)
{
	int iTemp;
	unsigned char aucTptr[2];
	unsigned int  uiTempH,uiTempL;
	SWStartI2C();			
	SWPutcI2C(ADDRESS_BYTE+WRITE);
	SWAckI2C();
	SWPutcI2C(PTR_TEMP);
	SWAckI2C();
	SWRestartI2C();
	SWPutcI2C(ADDRESS_BYTE+READ);		// Control byte
	SWAckI2C();
	SWGetsI2C(aucTptr, 2);		// Read Temperature
	SWStopI2C();

	uiTempH=aucTptr[0];				// High bits
	uiTempL=aucTptr[1];				// Low bits
	
	// Compute Centigrade 
	iTemp=(uiTempH<<8)|uiTempL;

	return (iTemp/128);
}
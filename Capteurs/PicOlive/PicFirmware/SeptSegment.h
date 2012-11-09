/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: flash.h
 * Description:  FLASH on PIC18F4580 
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

#ifndef	__SEPT_SEG_H__
#define	__SEPT_SEG_H__

//prototypes
void seg_init(void);
void seg_run(unsigned int iNum);
void seg_time(unsigned int uiH,unsigned int uiM,char cDecPoint);
void seg_set_num(unsigned char ucNum, unsigned char ucDigit,char cDecPoint);
void seg_off(void);




#endif
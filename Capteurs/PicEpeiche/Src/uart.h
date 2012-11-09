/**
 * Project: PicEpeiche 
 * File name: uart.h
 * Description:  write / read data in serial line
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
#include <utils.h>
#ifndef __UART_H
#define __UART_H

#ifdef OLD_VERSION
#define FOSC	12000000
#define HIGH_SPEED 1
#else
#define FOSC	20000000
#define HIGH_SPEED 0
#endif

#define BAUD 	4800


#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))
#else
#define SPEED 0
#define DIVIDER ((int)(FOSC/(64UL * BAUD) -1))
#endif


void Uart_vInit(void);
#endif
/**
 * Project: PicEpeiche 
 * File name: uart.c
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

#include <p18f4520.h>
#include <usart.h>
#include <uart.h>
#include <stdio.h>
void Uart_vInit(void)
{
	SPBRG =DIVIDER; 				//Set BAUD
#ifdef HIGH_SPEED
	TXSTA = 0x24;					// setup USART transmit high speed
#else
	TXSTA = 0x20;					// setup USART transmit low speed
#endif
	RCSTA = 0x90;					// setup USART receive
	TXSTA = (SPEED|NINE_BITS|0x20);
	RCSTA = (NINE_BITS|0x90);		// enable USART


	stdout = _H_USART ;
}
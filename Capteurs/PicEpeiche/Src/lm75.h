/**
 * Project: PicEpeiche 
 * File name: lm75.h
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

#ifndef __LM75_H
#define __LM75_H


#define ADDRESS_BYTE	0x90
#define READ		0x01
#define WRITE		0x00


/*POINTER REGISTER (Selects which registers will be read from or written to):
0 0 0 Temperature (Read only) (Power-up default)
0 0 1 Configuration (Read/Write)
0 1 0 THYST (Read/Write)
0 1 1 TOS (Read/Write)
1 1 1 Product ID Register (LM75A Only)*/

//Temperature (Read only) (Power-up default)
#define PTR_TEMP	0x00
//CONFIGURATION REGISTER (Read/Write):
#define PTR_CONFIG	0x01
// D7 D6 D5    D4D3           D2         D1     D0
// 0  0  0 Fault Queue O.S. Polarity Cmp/Int Shutdown

#define CFG_SHUTDOWN	0x01
#define CFG_INT_MODE	0x02
#define CFG_COMP_MODE	0x00
#define CFG_OS_POL	0x04
#define CFG_1FAULT	0x00
#define CFG_2FAULT	0x08
#define CFG_4FAULT	0x10
#define CFG_6FAULT	0x18
#define Shutdown	0x01

//THYST (Read/Write)
#define PTR_THYST	0x02

//TOS (Read/Write)
#define PTR_TOS		0x03

//Product ID Register (LM75A Only)
#define PTR_PID		0x03


void LM75_vInit(void);
int LM75_iGetTemp(void);
#endif
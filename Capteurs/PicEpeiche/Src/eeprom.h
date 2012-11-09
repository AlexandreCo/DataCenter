/**
 * Project: PicEpeiche 
 * File name: eeprom.h
 * Description:  write / read data in i2c eeprom
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
#ifndef __EEPROM_H
#define __EEPROM_H

#define MAX_T	10
#define MIN_T	12
#define MAX_P	14
#define MIN_P	16

#define INDEX_HISTO_JOUR	20
#define HISTO_PRESS		22

#define HISTO_TEMP		60
//#define EEPROM_READ_ONLY	1
void EEPROM_vByteWrite(unsigned char adr,unsigned char data);
unsigned char EEPROM_ucByteRead(unsigned char adr);
void EEPROM_vPageWrite(unsigned char adr,unsigned char wdata[]);
void EEPROM_vSequentialRead(unsigned char adr,unsigned char rdata[],unsigned char len);

void EEPROM_vAckPoll( void );
int EEPROM_iRead(unsigned char ucIndex);
void EEPROM_vWrite(int iTemp,unsigned char ucIndex);
#endif
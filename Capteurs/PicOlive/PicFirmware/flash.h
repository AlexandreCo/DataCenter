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

#ifndef	__FLASH_H__
#define	__FLASH_H__

//prototypes

char * acGetArchive(unsigned int uiNum);
int Archive_iSave(void);
int Archive_iRead(unsigned int uiMin ,unsigned int uiMax );
int Archive_iGet(unsigned int uiNum );
int Archive_iErase(void);
#define ARCHIVE_MIN	0x4000
#define ARCHIVE_MAX	0x8000

#define FLASH_BLOCK_SIZE	64
#define NB_BLOCK_ARCHIVE	((ARCHIVE_MAX-ARCHIVE_MIN)/FLASH_BLOCK_SIZE)

#define INDEX_FREE_PATERN	TIME/2	//l'humidité ne peut pas etre egale à -1
#define INDEX_FULL_PATERN	TIME/2	//
#define FREE_PARTERN 	0xFFFF	//FF -1
#define FULL_PARTERN 	0xAAAA	//l'humidité et la temp ne peuvent pas etre egale à -61
#endif

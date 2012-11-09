/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: flash.c
 * Description:  FLASH on PIC18F4580  read/write flash memory.
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

//prototypes
#include <p18f4580.h>
#include <stdio.h>
#include "eeprom.h"
#include "flash.h"
void memcpyram2flash(unsigned addr, char * mem);
void FlashPage_Erase(unsigned int FlashPageBaseAdd);

void memcpyram2flash(unsigned uiFlash, char * acRam)
{
	unsigned char chunkSize = 16;
	unsigned char i;

	printf("Archive %X\n\r",uiFlash);
	if ((uiFlash >= ARCHIVE_MIN) && (uiFlash < ARCHIVE_MAX))
	{
		uiFlash = uiFlash & 0xFFFFC0;	// Address filter (robustness: to ensure pointer passed in paacRameter is correct)

		TBLPTR = uiFlash;
		EECON1bits.EEPGD = 1;
		EECON1bits.CFGS = 0;
		EECON1bits.WREN = 1;
		EECON1bits.FREE = 1;
		INTCONbits.GIE = 0;
		EECON2 = 0x55;
		EECON2 = 0xAA;
		EECON1bits.WR = 1;
		Nop();
		Nop();
		Nop();
		Nop();
		EECON1bits.WREN = 0;

		TBLPTR = uiFlash;

		for(i=0 ; i<FLASH_BLOCK_SIZE ; i++)
		{
			TABLAT = acRam[i];        // put a char into the table latch register

			_asm
			TBLWTPOSTINC          // write to holding register and post-increment
			_endasm

			if(((i + 1) % chunkSize) == 0)
			{ // write to uiFlash after each chunkSize bytes
				TBLPTR --;    //put TBLPTR in holding register address range
				EECON1bits.EEPGD = 1; // ...write the holding registers to flash
				EECON1bits.CFGS = 0;
				EECON1bits.WREN = 1;
				EECON1bits.FREE = 0;
				INTCONbits.GIE = 0;
				EECON2 = 0x55;
				EECON2 = 0xAA;
				EECON1bits.WR = 1;
				Nop();
				Nop();
				Nop();
				Nop();
				INTCONbits.GIE = 1;
				EECON1bits.WREN = 0;
				TBLPTR ++;
			}
		}
	}
	else
	{
		printf("error location : %X\n\r",uiFlash);
	}
}



int Archive_iErase(void)
{
	unsigned int uiBlock;
	unsigned int uiFreeBlock;
	int i;
	far rom int * acArchive;

	//Find free block
	for(uiFreeBlock=0;uiFreeBlock<NB_BLOCK_ARCHIVE-1;uiFreeBlock++)
	{
		acArchive=(int*)acGetArchive(uiFreeBlock);
		FlashPage_Erase(acArchive);
		
	}
	printf("Archive RAZ ok\n\r");
	return uiFreeBlock;
}


void FlashPage_Erase(unsigned int FlashPageBaseAdd)
{
        unsigned char cTblWrCount;

        TBLPTR = FlashPageBaseAdd;
       
        EECON1bits.EEPGD = 1;           // Point to Flash memory
        EECON1bits.CFGS = 0;            // Acess Flash/EPROM
        EECON1bits.WREN = 1;            // Enable writes
        EECON1bits.FREE = 1;            //Enable Row Erase operation
        INTCONbits.GIE = 0;         //Disable interrupt        
        EECON2 = 0x55;                          //Required
        EECON2 = 0xAA;                          //Required
        EECON1bits.WR = 1;                      //Set WR bit to begin write
        INTCONbits.GIE = 1;                     //Enable interrupt
}


char * acGetArchive(unsigned int uiNum)
{
	return ARCHIVE_MIN+FLASH_BLOCK_SIZE*uiNum;
}
void swap(unsigned int *x)
{
    *x = (*x>>8) | 
        (*x<<8);
}
unsigned char auiEepromBuf[FLASH_BLOCK_SIZE];

void EEPROM_vGetContent(char * auiBuf,int iIndex,int size)
{
	int i;
	for(i=0;i<size/2;i++)
	{
		auiBuf[i*2]=EEPROM_ucByteRead(i*2+iIndex+1);
		auiBuf[i*2+1]=EEPROM_ucByteRead(i*2+iIndex);
		
	}
}
int Archive_iSave(void)
{
	unsigned int uiBlock;
	unsigned int uiFreeBlock;
	int uiTemp,i;
	far rom char * acArchive;
	//Find free block (patern 0xFF)
	for(uiFreeBlock=0;uiFreeBlock<NB_BLOCK_ARCHIVE;uiFreeBlock++)
	{
		acArchive=acGetArchive(uiFreeBlock);
		if(acArchive[INDEX_FREE_PATERN]==FREE_PARTERN)
		{
			printf("free block = %d\n\r",uiFreeBlock);
			break;
		}
	}
	if(uiFreeBlock==NB_BLOCK_ARCHIVE)
	{
		printf("Memoire Pleine TODO...\n\r");
		return -1;
	}
	//Save Eeprom in flash memory
	EEPROM_vGetContent((char *)auiEepromBuf,HISTO_JOUR+0*FLASH_BLOCK_SIZE,FLASH_BLOCK_SIZE);
	memcpyram2flash(ARCHIVE_MIN+(uiFreeBlock+0)*FLASH_BLOCK_SIZE,(char *)auiEepromBuf);

	EEPROM_vGetContent((char *)auiEepromBuf,HISTO_JOUR+1*FLASH_BLOCK_SIZE,FLASH_BLOCK_SIZE);
	memcpyram2flash(ARCHIVE_MIN+(uiFreeBlock+1)*FLASH_BLOCK_SIZE,(char *)auiEepromBuf);


	return uiFreeBlock+2;
}

int Archive_iGetFreeblock( )
{
	unsigned int uiFreeBlock;
	far rom int * acArchive;
	
	//Find free block
	for(uiFreeBlock=0;uiFreeBlock<NB_BLOCK_ARCHIVE-1;uiFreeBlock++)
	{
		acArchive=(int*)acGetArchive(uiFreeBlock);
		if(acArchive[INDEX_FREE_PATERN]==FREE_PARTERN)
		{
			//printf("Block Vide\n\r");
			break;
		}
	}
	return uiFreeBlock;
}

int Archive_iRead(unsigned int uiMin ,unsigned int uiMax )
{
	unsigned int uiBlock;
	unsigned int uiFreeBlock;
	int i;
	unsigned long ulNbIndex=uiMin*FLASH_BLOCK_SIZE/2;
	far rom int * acArchive;
	if(uiMax>NB_BLOCK_ARCHIVE-1)
	{
		printf("Max %d\n\r",NB_BLOCK_ARCHIVE-1);
		uiMax=NB_BLOCK_ARCHIVE-1;
	}

	//Find free block
	for(uiFreeBlock=uiMin;uiFreeBlock<uiMax;uiFreeBlock++)
	{
		acArchive=(int*)acGetArchive(uiFreeBlock);
		if(acArchive[INDEX_FREE_PATERN]==FREE_PARTERN)
		{
			//printf("Block Vide\n\r");
			break;
		}
		for(i=0;i<((FLASH_BLOCK_SIZE)/SAMPLE_SIZE);i++)
		{
			//if(acArchive[i*SAMPLE_SIZE/2+HMOY/2]!=FULL_PARTERN)
			{
				ulNbIndex++;
				printf("%ld;%04d;%d;%d;%d;%d\n\r",ulNbIndex
						,acArchive[i*SAMPLE_SIZE/2+TIME/2]
						,acArchive[i*SAMPLE_SIZE/2+PMOY/2]
						,(char)(acArchive[i*SAMPLE_SIZE/2+T1_HMOY/2]>>8)
						,(int)acArchive[i*SAMPLE_SIZE/2+T2MOY/2]
						,acArchive[i*SAMPLE_SIZE/2+T1_HMOY/2]&0xFF);
				
			}
		}
	}
	return uiFreeBlock;
}

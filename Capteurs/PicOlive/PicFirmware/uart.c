/**
 * Project: PicOlive : presure and temperature sensor data logger
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
#include <string.h>
#include <stdlib.h>
#include <flash.h>
#define IDX_MAX_CMD	20
static char _acCmd[IDX_MAX_CMD];
static char _cIndexCmd;
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

extern void vFuncDayHisto(void);
extern void vFuncRaz(void);
extern void vFuncSetTime(void);
extern void vFuncGetTime(void);
extern void vFuncGetSample(void);
extern int Archive_iGetFreeblock(void);
extern void vSetPeriode(void);
extern unsigned int _uiNbSample;
char acCmdHisto[]="histo";
char acCmdRaz[]="raz";

char acCmdTime[]="settime";
char acCmdGTime[]="gettime";

char acCmdget[]="get";

char acCmdread[]="read";
char acCmdErase[]="erase";

char acCmdread2[]="R";
char acCmdperiode[]="periode";
char acCmdsave[]="save";
char acCmdfree[]="free";
char acCmdHelp[]="help";
char acBuf[16];
void vRunCmd(void)
{
	int iFreeBlock,iNbBlockFree;


	int i;

	if(strcmp(_acCmd,acCmdHisto)==0)
	{
		vFuncDayHisto();
	}


	if(strcmp(_acCmd,acCmdRaz)==0)
	{
		vFuncRaz();
	}

	if(strcmp(_acCmd,acCmdTime)==0)
	{
		vFuncSetTime();
	}
	if(strcmp(_acCmd,acCmdGTime)==0)
	{
		vFuncGetTime();
	}
	if(strcmp(_acCmd,acCmdget)==0)
	{
		vFuncGetSample();
	}
	if(strcmp(_acCmd,acCmdsave)==0)
	{
		Archive_iSave();
	}
	if(strcmp(_acCmd,acCmdread)==0)
	{
		int ret;
		ret=Archive_iRead(0 ,255 );
		printf("free %d blocks used %d blocks\n\r",256-ret,ret);
	}

	if(strcmp(_acCmd,acCmdErase)==0)
	{
		Archive_iErase();
	}

	if(strcmp(_acCmd,acCmdread2)==0)
	{
		int ret;
		int arg=UART_iGetInt();
		
		ret=Archive_iRead(arg ,arg+1 );
		printf("free %d blocks used %d blocks\n\r",256-ret,ret);
	}

	if(strcmp(_acCmd,acCmdperiode)==0)
	{
		vSetPeriode();
	}
	if(strcmp(_acCmd,acCmdfree)==0)
	{


		iFreeBlock=Archive_iGetFreeblock();
		iNbBlockFree=256-iFreeBlock;
		printf("free :%d blocks used :%d blocks\n\r",iNbBlockFree,iFreeBlock);
		printf("periode %d min \n\r",_uiNbSample);
	}


	if(strcmp(_acCmd,acCmdHelp)==0)
	{
		printf("%s %s %s %s %s %s %s %s %s %s %s\n\r",
		acCmdHisto,
		acCmdRaz,
		acCmdget,
		acCmdTime,
		acCmdGTime,
		acCmdsave,
		acCmdread,
		acCmdErase,
  		acCmdperiode,
    		acCmdfree,
		acCmdHelp);
	}
}


void vSetupCmd(void)
{
	int i;
	for(i=0;i<IDX_MAX_CMD;i++)
	{
		_acCmd[i]=0;
	}
	_cIndexCmd=0;
}

void UART_vProcessCmd()
{
	char cData;

	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN=0;
		RCSTAbits.CREN=1;
	}

	if(PIR1bits.RCIF)				// Check for USART interrupt
   	{
		cData=RCREG;
		if(cData!=0x0d)
		{
			_acCmd[_cIndexCmd]=cData;	
		}			
		_cIndexCmd++;
		if((_cIndexCmd>=(IDX_MAX_CMD-2))||(cData==0x0d))
		{
			//Buffer overflow
			TXREG = cData;
			//printf("%s\n\r",_acCmd);
			vRunCmd();
			vSetupCmd();
		}
		else
		{
			TXREG=cData;
		}
   	}
}



int UART_iGetInt(void)
{
	char cData;
	vSetupCmd();

	do
	{
		if(RCSTAbits.OERR)
		{
			RCSTAbits.CREN=0;
			RCSTAbits.CREN=1;
		}

		if(PIR1bits.RCIF)				// Check for USART interrupt
		{
			cData=RCREG;
			if(cData!=0x0d)
				_acCmd[_cIndexCmd]=cData;
			_cIndexCmd++;
			if((_cIndexCmd>=(IDX_MAX_CMD-2))||(cData==0x0d))
			{
				//Buffer overflow
				TXREG = cData;
				//printf("%s\n\r",_acCmd);
				break;
			}
			else
			{
				TXREG=cData;
			}
		}
	}while(1);
	return atoi(_acCmd);
}
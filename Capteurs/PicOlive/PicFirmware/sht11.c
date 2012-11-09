/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: sht11.c
 * Description:  SHT11 on PIC18F4580 Get Temperature and humidity from SHT11 sensor
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
#include <p18f4520.h>
#include <stdio.h>
//#include "utils.h"
#include "sht11.h"



//#define DEBUG_ENABLED




#ifdef DEBUG_ENABLED
#define DEBUG1(x) x
#else
#define DEBUG1(x)
#endif

#define SUCCESS	0

#define SORTIE	0
#define ENTREE	1


static char cTestAck(void);
static void vSendBit(char cBit);
static char cReadBit(void);
static void vSendStart(void);
static void vSendReset(void);
static char cWaitEndMesurement(void);
static char cShtRead(char cAck);
static char cShtWrite(unsigned char ucVal);

void vDelay(unsigned int uiTick)
{
	int i;
	for(i=0;i<uiTick;i++)
	{
		Nop();
	}
}


int SHT11_iGetTemp(void)
{
	unsigned char aucRead[3];
	int iTemp;
	int iT_c;
	char cError=0;
	vSendReset(); //Wake sensor
	DEBUG1(printf("\n\rH : "));
	vSendStart();
	if(!cShtWrite(0x03))
	{
		cError=cWaitEndMesurement();
		if(cError!=SUCCESS)
		{
			return cError;
		}
		aucRead[0]=cShtRead(0);
		aucRead[1]=cShtRead(0);
		aucRead[2]=cShtRead(1);
		DEBUG1(printf("\n\rbrute  0x%02x 0x%02x 0x%02x\n\r",aucRead[0],aucRead[1],aucRead[2]));
	
		iTemp=(aucRead[0]*256)+aucRead[1];
		iTemp/=D2;
		
		if (iTemp<D1)		//Check for minus temperatures
		{
			DEBUG1(printf("adjust back to a positive number\n"));
			iT_c=D1-iTemp;	//adjust back to a positive number
			iT_c=-iT_c;
		}
		else
		{
			iT_c=iTemp-D1;
		}

		DEBUG1(printf("iTemp=%d D1=%d iT_c=%d\n\r",iTemp,D1,(char)iT_c));
	}
	else
	{
		DEBUG1(printf("\n\r"));
		vSendReset(); //Wake sensor
	}
	return iT_c;
}


char SHT11_cGetHumi(int iT_c)
{
	unsigned char aucRead[3];
	unsigned long ulHumidity,ulHlin,ulHadj;
	unsigned int uiTemp;
	char error=0;
	vSendReset(); //Wake sensor
	DEBUG1(printf("\n\rH : "));
	vSendStart();
	if(!cShtWrite(0x05))
	{
		error=cWaitEndMesurement();
		if(error!=SUCCESS)
		{
			return error;
		}
		aucRead[0]=cShtRead(0);
		aucRead[1]=cShtRead(0);
		aucRead[2]=cShtRead(1);
		DEBUG1(printf("\n\r 0x%02x 0x%02x 0x%02x\n\r",aucRead[0],aucRead[1],aucRead[2]));
		ulHumidity=(aucRead[0]*256)+aucRead[1];

		if(ulHumidity <1801)	//1800 ticks= approx 60% humidity unadjusted
		{
			ulHlin=((ulHumidity*1000)/30960)+2; //multiply by 1000 to avoid fractions
		}
		else
		{
			ulHlin=((ulHumidity*1000)/31046);
		}

		if(iT_c>TEMP_BASE)
		{
			uiTemp=(iT_c-TEMP_BASE)*(ulHlin*26/100);
			uiTemp/=100;
			ulHadj= uiTemp + ulHlin;
		}
		else
		{
			ulHadj=ulHlin;
		}
	    if (ulHadj>99)
		{
	    	ulHadj=100;
		}
	}
	else
	{
		DEBUG1(printf("\n\r"));
		vSendReset(); //Wake sensor
	}
	DEBUG1(printf("H=%ld t_c=%dC\n\r",ulHadj,ulHlin,ulHumidity,iT_c));
	return (char)ulHadj;
}

void SHT11_vSetup(void)
{
	SHT_DATA=1;				//SHT Data line idle high, clock idle low
	SHT_CLOCK=0;
 	TRIS_data=SORTIE;
 	TRIS_clock=SORTIE;
}



//vSendReset function
//This function wakes the sensor from sleep.
//The sequence is with data line high, clock the SCL line 9 times then call
//transmission start sequence.
static void vSendReset(void)
{
	char cCount=0;
	DEBUG1(printf("\n\rReset "));
	for(cCount=0;cCount<10;cCount++)
	{
		vSendBit(1);
	}
}

//Function vSendStart
//This in the sensor document is the transmission start sequence
//With the data line high, the clock is brought high and the data line brought low
//the SCL line is clocked again and the data line is then brought high ready for
//data transmission or reception
static void vSendStart(void)
{
	//Make pin an input
	TRIS_data=SORTIE;
	SHT_DATA=1;
	vDelay(DELAY_SHT);
	SHT_CLOCK=1;
	vDelay(DELAY_SHT);
	SHT_DATA=0;
	vDelay(DELAY_SHT);
	SHT_CLOCK=0;
	vDelay(DELAY_SHT);
	SHT_CLOCK=1;
	vDelay(DELAY_SHT);
	SHT_DATA=1;
	vDelay(DELAY_SHT);
	SHT_CLOCK=0;
	vDelay(DELAY_SHT);
}

static char cShtWrite(unsigned char ucVal)
{
	unsigned char ucLoop;
	DEBUG1(printf("SEND : "));
	for(ucLoop=0x80;ucLoop>0;ucLoop>>=1) 			//data out MSB first
	{

	 	if (ucLoop & ucVal)
	 	{
	 		vSendBit(1);
		}
	 	else
	 	{
	 		vSendBit(0);
	 	}
	}

	if(cTestAck()==1)
	{
		return -1;
	}
	return 0;
}

static char cShtRead(char cAck)
{
	char cData=0;
	unsigned char ucLoop;
	char cBit;
	int i;
	//Get data in MSB first
	for(ucLoop=0x80;ucLoop>0;ucLoop>>=1)
	{
		cBit=cReadBit();
		if(cBit)
		{
			cData|=ucLoop;
		}
	}

	if(cAck)
	{
		DEBUG1(printf("a"));
		vSendBit(1);
		DEBUG1(printf(" "));
	}
	else
	{
		DEBUG1(printf("a"));
		vSendBit(0);
		DEBUG1(printf(" "));
	}

	return cData;
}




static char cReadBit(void)
{
	char cBit;
	//Make pin an input
	TRIS_data=ENTREE;
	SHT_CLOCK=1;
	vDelay(DELAY_SHT);
	if(SHT_DATA==1)
	{
		DEBUG1(printf("1"));
		cBit=1;
	}
	else
	{
		DEBUG1(printf("0"));
		cBit=0;
	}
	SHT_CLOCK=0;
	vDelay(DELAY_SHT);
	return cBit;

}


static void vSendBit(char cBit)
{
	//Make pin an output
	TRIS_data=SORTIE;
	SHT_DATA=cBit;
	DEBUG1(printf("%d",cBit));
	vDelay(DELAY_SHT);
	SHT_CLOCK=1;
	vDelay(DELAY_SHT);
	SHT_CLOCK=0;
	vDelay(DELAY_SHT);
}


static char cTestAck(void)
{
	char ack;
	//Make pin an input
	TRIS_data=ENTREE;
	vDelay(DELAY_SHT);
	SHT_CLOCK=1;
	vDelay(DELAY_SHT);
	if(SHT_DATA==1)
	{
		DEBUG1(printf("A"));
		ack=1;
	}
	else
	{
		DEBUG1(printf("nA"));
		ack=0;
	}
	SHT_CLOCK=0;
	vDelay(DELAY_SHT);
	return ack;
}


static char cWaitEndMesurement(void)
{
	unsigned int uiTimeout=TIMEOUT_500MS;
	TRIS_data=ENTREE;

	while(SHT_DATA==1)
	{
		vDelay(DELAY_1MS);
		uiTimeout--;
		if(!uiTimeout)
		{
			DEBUG1(printf("cWaitEndMesurement error %d : TIMEOUT MEASURE\n\r",ERROR_TIMEOUT_MEASURE));
			return ERROR_TIMEOUT_MEASURE;
		}
	}
	vDelay(DELAY_SHT);
	return SUCCESS;
}


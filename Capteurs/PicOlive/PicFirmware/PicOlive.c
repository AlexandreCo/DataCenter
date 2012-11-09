/**
 * Project: PicOlive : presure and temperature sensor data logger
 * File name: flash.c
 * Description:  Log tempererature, presure, humidity information flash memory.
 *
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
#include <lm75.h>
#include <MPX6115.h>
#include <sw_i2c.h>
#include <eeprom.h>
#include <sht11.h>
#include <flash.h>
//#include <timer.h>

#include <uart.h>
#include <usart.h>
#include <delays.h>
#include <timers.h>
#include <stdio.h>
#include <string.h>
#include <cron.h>
#include <SeptSegment.h>


#pragma config OSC = HS
#pragma config PWRT = OFF
#pragma config BOREN = OFF
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = PORTC

#define DownKey 	PORTAbits.RA4		// Change key
#define RightKey 	PORTBbits.RB0		// Function key


#define LED_SAMPLE 	PORTBbits.RB1	
#define LED_MAX 	PORTBbits.RB2
#define LED_MIN 	PORTBbits.RB3	

#define ALIM_RF 	PORTEbits.RE1
#define NB_TRANSMIT	5	//nb de transmission de la trame rf
#define SAMPLE_EVERY	300							// Periode sample en s
#define RF_ID		1

#define TIME_TIMEOUT	(65535-15000)
#define MINUTE		1000

#define TRUE 	1
#define FALSE 	0			

#define PRESSION_MAX	1150 	// PNM maximum : 1086 hPa, à Tosontsengel (Mongolie), le 19 décembre 2001.
#define PRESSION_MIN	800 	//PNM minimum : 870 hPa, au large des Philippines, près du centre du typhon Tip, le 12 octobre 1979



typedef struct
{
	unsigned int uiTime;
	unsigned int uiPMoy;
	int iT2Moy;
	char cT1Moy;
	unsigned char ucHMoy;
}stSample;	

stSample _stSampleTabJour[NB_SAMPLE_MOY];
//#define DEBUG
unsigned char _ucSampleIndex=0;
unsigned char _ucIndexJour=0;
unsigned int _uiNbSample;		//nombre de sample a moyenné (1 sample par min)
typedef struct
{
	unsigned long ulPMoy;
	long lT1Moy;
	long lT2Moy;
	unsigned long ulHMoy;
}stSampleMoy;	
stSampleMoy _stSampleMoy;


unsigned int _uiPression;
int _iShT2;
unsigned char _ucShHumi;
char _cLMT1;			// Memory Centigrade
#define NO_VEILLE	0
#define SET_TIME 	1 
#define VEILLE		2
#define TEMPS_VEILLE	5	//en min
char _bVeille=NO_VEILLE;
char _cTempsVeille;

char bRun;


void SetTime(void);
void vInit(void);						// Initialize Control Microchip

void vDelayInit(void);
void Appli_vInit(void);
void vTimerInit(void);

void vGetSamples(void);
void vFuncRaz(void);
void vFuncMain(int iTemp,unsigned int uiPression,int iShTemp,int iShHumi);
void vFuncDayHisto(void);
void vFuncSetTime(void);
void vFuncGetSample(void);

void vSetPeriode(void)
{
	unsigned int uiNbSample;
	printf("Periode (1 90) en minute\n\r");
	uiNbSample=UART_iGetInt();
	if((uiNbSample>=1)&(uiNbSample<=90))
	{
		EEPROM_vWrite(uiNbSample,EEPROM_NB_SAMPLE);
		_uiNbSample=uiNbSample;
		Appli_vInit();
	}
	else
	{
		printf("la periode doit etre comprise en tre 1 et 90\n\r");
	}
}
		
void vFuncSetTime(void)
{
	unsigned int uiTemp;
	printf("Heure\n\r");
	uiTemp=UART_iGetInt();
	cron_vSetHeure(uiTemp);

	printf("minute\n\r");
	uiTemp=UART_iGetInt();
	cron_vSetMin(uiTemp);

	printf("seconde\n\r");
	uiTemp=UART_iGetInt();
	cron_vSetSec(uiTemp);
}

void vFuncGetTime(void)
{
	printf("%02d:%02d:%02d\r",cron_uiHeure(),cron_uiMinute(),cron_uiSeconde());
}

void vFuncGetSample(void)
{
	vFuncMain(_cLMT1,_uiPression,_iShT2,_ucShHumi);
}
/**************************************************************************/
void vInit(void)
{			
	CMCON=0b00000111;	// Close Comparator
	TRISA=0b00010000;
	TRISB=0b00000001;
	TRISC=0b11000000;	
	PORTC=0b00000000;
	TRISD=0b00000000;
	PORTD=0x00;	
	TRISE=0b00001000;	
	ADCON1=0b00001111;	// Digital Channel Allocation 
	Uart_vInit();
	SHT11_vSetup();
	LM75_vInit();		// Temperature Sensor Initializtion
	seg_init();		// Sept Seg Initialization
	LED_SAMPLE=1;		// Led Initializtion
	Appli_vInit();
	vTimerInit();
}

void Appli_vInit(void)
{
	int i;
	unsigned int uiTemp;
	_stSampleMoy.ulPMoy=0;
	_stSampleMoy.lT1Moy=0;
	_stSampleMoy.lT2Moy=0;
	_stSampleMoy.ulHMoy=0;
	for(i=0;i<NB_SAMPLE_MOY;i++)
	{
		_stSampleTabJour[i].uiTime=EEPROM_iRead(HISTO_JOUR+NB_CHAR*i+TIME);
		_stSampleTabJour[i].uiPMoy=EEPROM_iRead(HISTO_JOUR+NB_CHAR*i+PMOY);
		_stSampleTabJour[i].iT2Moy=EEPROM_iRead(HISTO_JOUR+NB_CHAR*i+T2MOY);
		uiTemp=EEPROM_iRead(HISTO_JOUR+NB_CHAR*i+T1_HMOY);
		_stSampleTabJour[i].cT1Moy=uiTemp>>8;
		_stSampleTabJour[i].ucHMoy=uiTemp&0xFF;
		
	}
	_uiNbSample=EEPROM_iRead(EEPROM_NB_SAMPLE);
	if((_uiNbSample>=1)&(_uiNbSample<=90))
	{
		printf("Periode %d minutes\n\r",_uiNbSample);
	}
	else
	{
		_uiNbSample=NB_SAMPLE_HEURE;
		printf("Periode %d minutes (default)\n\r",_uiNbSample);
	}
	_ucIndexJour=EEPROM_iRead(INDEX_HISTO_JOUR);
	
	_bVeille=NO_VEILLE;
	_cTempsVeille=0;

#ifdef DEBUG
	printf("IndexJour %d\n\r",_ucIndexJour);
#endif
}




void vTimerInit(void)
{
	OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT & T1_PS_1_8);
	WriteTimer1(TIME_TIMEOUT);
	INTCON=0;    //make sure interrupts are disable
	INTCONbits.GIE=1;  //enable global interrupts
	INTCONbits.PEIE=1;  //enable peripheral interrupts
	PIE1bits.TMR1IE=1; //enable TMR0 overflow interrupt enable bit

}

void vGetSamples(void)
{
	stSample stSpl;
	unsigned char i=0;
	unsigned int uiTemp;
	//Get Samples
	_cLMT1=LM75_iGetTemp();
	_iShT2=SHT11_iGetTemp();
	_ucShHumi=SHT11_cGetHumi((char)(_iShT2/10));
	_uiPression=MPX6115_uiGetPression();
	
	//process Moy
	_stSampleMoy.ulPMoy +=_uiPression;
	_stSampleMoy.lT1Moy +=_cLMT1;
	_stSampleMoy.lT2Moy +=_iShT2;
	_stSampleMoy.ulHMoy +=_ucShHumi;
	
	_ucSampleIndex++;
#ifdef DEBUG
	printf("_ucSampleIndex %d %d %d %d %d\n\r",_ucSampleIndex,_stSampleMoy.ulPMoy,_stSampleMoy.lT1Moy,_stSampleMoy.lT2Moy,_stSampleMoy.ulHMoy);
#endif
	if(_ucSampleIndex>=_uiNbSample)
	{
		
		_ucSampleIndex=0;
		stSpl.uiPMoy=_stSampleMoy.ulPMoy/_uiNbSample;
		stSpl.iT2Moy=_stSampleMoy.lT2Moy/_uiNbSample;
		stSpl.cT1Moy=_stSampleMoy.lT1Moy/_uiNbSample;
		stSpl.ucHMoy=_stSampleMoy.ulHMoy/_uiNbSample;
		
#ifdef DEBUG
		printf("uiPressMoy=%d\n\r",stSpl.uiPMoy);
#endif
		_stSampleTabJour[_ucIndexJour%NB_SAMPLE_MOY].uiPMoy=stSpl.uiPMoy;
		_stSampleTabJour[_ucIndexJour%NB_SAMPLE_MOY].cT1Moy=stSpl.cT1Moy;
		_stSampleTabJour[_ucIndexJour%NB_SAMPLE_MOY].ucHMoy=stSpl.ucHMoy;
		_stSampleTabJour[_ucIndexJour%NB_SAMPLE_MOY].iT2Moy=stSpl.iT2Moy;
		_stSampleTabJour[_ucIndexJour%NB_SAMPLE_MOY].uiTime=cron_uiHeure()*100+cron_uiMinute();

		EEPROM_vWrite(cron_uiHeure()*100+cron_uiMinute(),HISTO_JOUR+NB_CHAR*(_ucIndexJour%NB_SAMPLE_MOY)+TIME);
		EEPROM_vWrite(stSpl.iT2Moy,HISTO_JOUR+NB_CHAR*(_ucIndexJour%NB_SAMPLE_MOY)+T2MOY);
		EEPROM_vWrite(stSpl.uiPMoy,HISTO_JOUR+NB_CHAR*(_ucIndexJour%NB_SAMPLE_MOY)+PMOY);
		uiTemp=(unsigned char)stSpl.cT1Moy*256+stSpl.ucHMoy;
		EEPROM_vWrite(uiTemp,HISTO_JOUR+NB_CHAR*(_ucIndexJour%NB_SAMPLE_MOY)+T1_HMOY);
		
		
		_ucIndexJour++;
		if(_ucIndexJour>=NB_SAMPLE_MOY)
		{	//save in flash
			Archive_iSave();
			//1 tour d'eeprom
			_ucIndexJour=0;
		}
		EEPROM_vWrite(_ucIndexJour,INDEX_HISTO_JOUR);
		_stSampleMoy.ulPMoy=0;
		_stSampleMoy.lT1Moy=0;
		_stSampleMoy.lT2Moy=0;
		_stSampleMoy.ulHMoy=0;
	}
}

void vFuncMain(int iTemp,unsigned int uiPression,int iShTemp,int iShHumi)
{
	printf("T=%d.%d °C, P=%d hPa, T=%d.%d °C, %d%%\r",(iTemp)/2,((iTemp)*5)%10,uiPression,(iShTemp-19)/10,(iShTemp-19)%10,iShHumi);	
}

void vFuncDayHisto()
{
	char i;
	char acBuf[20];	
	unsigned int uiMax,uiMin,uiQuantum,uiEqui,uiValue;
	unsigned char ucIndex;
	printf("histo\n\r");
	printf("Date;P;T1*2;T2*10;H;\n\r");
	for(i=0;i<NB_SAMPLE_MOY;i++,ucIndex++)
	{
		printf("%d;%04d;%d;%d;%d;%d\n\r",i,_stSampleTabJour[i%NB_SAMPLE_MOY].uiTime,
						_stSampleTabJour[i%NB_SAMPLE_MOY].uiPMoy,
						_stSampleTabJour[i%NB_SAMPLE_MOY].cT1Moy,
						_stSampleTabJour[i%NB_SAMPLE_MOY].iT2Moy,
						_stSampleTabJour[i%NB_SAMPLE_MOY].ucHMoy
		      );
	}
	printf("fin histo\n\r");
}

void vFuncRaz(void)
{
	char acBuf[16];	
	int i;

	//raz histo
	for(i=0;i<NB_SAMPLE_MOY;i++)
	{
		EEPROM_vWrite(FULL_PARTERN,HISTO_JOUR+NB_CHAR*(i%NB_SAMPLE_MOY)+TIME);
		EEPROM_vWrite(FULL_PARTERN,HISTO_JOUR+NB_CHAR*(i%NB_SAMPLE_MOY)+PMOY);
		EEPROM_vWrite(FULL_PARTERN,HISTO_JOUR+NB_CHAR*(i%NB_SAMPLE_MOY)+T1_HMOY);
		EEPROM_vWrite(FULL_PARTERN,HISTO_JOUR+NB_CHAR*(i%NB_SAMPLE_MOY)+T2MOY);
		
	}
	Appli_vInit();
	EEPROM_vWrite(0,INDEX_HISTO_JOUR);
	_ucIndexJour=0;
	EEPROM_vWrite(NB_SAMPLE_HEURE,EEPROM_NB_SAMPLE);
	_uiNbSample=NB_SAMPLE_HEURE;

	printf("ok\n\r");
	for(i=0;i<1000;i++)
		Delay1KTCYx(5); // Delay of 1ms


}

void SetTime(void)
{
	//static unsigned char ucDigit = 0;
	static char cCurrentDigit = -1;
	static char tabDigit[4]={0};
	int i;
	if(_bVeille==VEILLE)
	{
		_bVeille=NO_VEILLE;
	}
	else
	{
		if(!RightKey)
		{
			
			cCurrentDigit++;
		}
	
		if(!cCurrentDigit)
		{
			_bVeille=SET_TIME;
		}
		
		if(cCurrentDigit>3)
		{
			unsigned int uiTemp;

			cCurrentDigit=-1;
			uiTemp=tabDigit[1]*10+tabDigit[0];
			cron_vSetMin(uiTemp);
			uiTemp=tabDigit[3]*10+tabDigit[2];
			cron_vSetHeure(uiTemp);
			_bVeille=NO_VEILLE;
		}
		
		if(!DownKey)
		{
			
			tabDigit[cCurrentDigit]++;
			if(tabDigit[cCurrentDigit]>9)
			{
				tabDigit[cCurrentDigit]=0;
			}
			
		}
		if(_bVeille==SET_TIME)
		{
			seg_set_num(cCurrentDigit,tabDigit[cCurrentDigit],0);
		}
	}
		
	for(i=0;i<250;i++)
		Delay1KTCYx(5); // Delay of 1ms
}

//gestion du mode veille
void IHM_run(void)
{
	if((!RightKey)||(!DownKey))
	{
		SetTime();
		_cTempsVeille=0;
	}
	
	if(_bVeille==NO_VEILLE)
	{			
		seg_time(cron_uiHeure(),cron_uiMinute(),cron_uiSeconde());
	}

	if(_cTempsVeille>TEMPS_VEILLE)
	{
		_bVeille=VEILLE;
	}

	if(_bVeille==VEILLE)
	{
		seg_off();
	}
}
void rfTransmit(char * buf,int len)
{
	int i;
	unsigned char checksum=0;
	char strChk[4];
	ALIM_RF=1;
	for(i=0;i<len;i++)
	{
		checksum+=buf[i];
	}
	
	for(i=0;i<NB_TRANSMIT;i++)
		printf("PO %s %d\n\r",buf,checksum);

	ALIM_RF=0;
}

void main (void)
{
	unsigned int i;
	char cGetSample=TRUE;
	char buf[25]; 
	vInit();							// Initialize Control Microchip
	_uiPression=MPX6115_uiGetPression();
	_cLMT1=LM75_iGetTemp();
	_iShT2=SHT11_iGetTemp();
	_ucShHumi=SHT11_cGetHumi((char)(_iShT2/10));

	while(1)
	{
		if(bRun==TRUE)
		{	
			bRun=FALSE;
#ifdef AFFICHE_HEURE
			printf("%02d:%02d:%02d\r",_uiHeure,_uiMinute,_uiSeconde);
#else
	#ifdef AFFICHE_HEURE_SAMPLE
			printf("%02d:%02d:%02d\t",_uiHeure,_uiMinute,_uiSeconde);
			vFuncGetSample();
	#else

	#endif
#endif
		}
		if(cron_bActionSec(SAMPLE_EVERY)==TRUE)
		{
				LED_SAMPLE=0;
				if(_bVeille!=VEILLE)
					_cTempsVeille++;
				else
					_cTempsVeille=0;
				seg_off();
				vGetSamples();
				sprintf(buf,"%d %d %d %d %d",RF_ID,_cLMT1,_iShT2,_ucShHumi,_uiPression);
				rfTransmit(buf,strlen(buf));
				cGetSample=FALSE;
				LED_SAMPLE=1;
		}
		UART_vProcessCmd();
		IHM_run();
	};
}


// protoype pour la fonction qui sera appelé lors d'une interruption
// Le timer1 est par défaut en interruption haute priorité
void timer1_isr(void);

#pragma code high_vector=0x08
void interruption_haute(void)
{
	_asm GOTO timer1_isr _endasm
}



#pragma code

#pragma interrupt timer1_isr
void timer1_isr(void)
{
						// On part de 3065 pour aller à 65535 soit 0.1s avec le quartz à 20M
	WriteTimer1(65535-(((FOSC/4)/8)/10));   // On part de 28035 pour aller à 65535 soit 0.1s avec le quartz à 12M
	PIR1bits.TMR1IF = 0;	// Remise à zero du drapeau d'interruption
	cron_addDixieme();
}

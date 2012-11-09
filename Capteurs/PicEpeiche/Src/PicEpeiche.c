/*
Display tempererature and pressure, minima and  maxima information on the HD44780 compatible LCD Module.
Display last 24 hours data histo on the HD44780 compatible LCD Module

(Hardare use  : sure electronics DP-113 eval board, with some few modifications

The LCD module was connected to 
	PORTD (as data port), 
	PORTA1 (E signal of LCD module), 
	PORTA2 (RW signal of LCD module), 
	PORTA3 (RS signal of LCD module), 
	PORTC1 (VEE : CCP2 as PWM controlled DC voltage generator).

LM75 (I2C addr. = 0xA0 )  module was connected to
	PORTC4	(SDA)
	PORTC5	(SCL)

24C04 EEPROM (4K bit serial I2C eeprom)
	PORTC4	(SDA)
	PORTC5	(SCL)

MPX6115 module was connected to
	PORTE0	(AN5)

LEDs were connected to 
	PORTB1 : Led new sample
	PORTB2 : Led new max
	PORTB3 : Led new min

Switches were connected to  
	PORTA4	: key down
	PORTB0	: key Right
	PORTE3	: reset

RF module was connected to
	PORTC6	(TX)
	PORTE0	(ALIM)

USB to serial (CP2102) converter was connected to
	PORTC6	(PIC TX USB RX)
	PORTC7	(PIC RX USB TX)

12 or 20 Mhz Oscillator 
	PORTA6
	PORTA7

FREE : 	PORTA0
	PORTA5
	PORTB4	(7 Segments on DP113 not used)
	PORTB5	(7 Segments on DP113 not used)
	PORTB6	(7 Segments on DP113 not used)
	PORTB7	(7 Segments on DP113 not used)
	PORTC0
	PORTC1
	PORTC2	(SPEAKER on DP113 not used)
	PORTC3
 */
#include <p18f4520.h>
#include <lm75.h>
#include <delays.h>
#include <MPX6115.h>
#include <pwm.h>
#include <cron.h>
#include <eeprom.h>
#include <LCD.h>
#include <uart.h>
#include <usart.h>
#include <timers.h>
#include <stdio.h>
#include <string.h>
#include <key.h>
#include <utils.h>

//#define DEBUG

#pragma config OSC = HS
#pragma config PWRT = OFF
#pragma config BOREN = OFF
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = PORTC


#define LED_SAMPLE 	PORTBbits.RB1	
#define LED_MAX 	PORTBbits.RB2
#define LED_MIN 	PORTBbits.RB3	
#define ALIM_RF 	PORTEbits.RE1

#define TIME_TIMEOUT	(65535-15000)

#define NB_CONTRAST	11	

#define NB_TRANSMIT	5	//nb de transmission de la trame rf


//contante pour la generation des histos
// un sample tous les 1 min
#define SAMPLE_EVERY	300			// Periode sample en s
#define NEW_CHART_EVERY	90			//((24*60)/NB_COL_LCD)	 Periode nouveau histo en min
#define NB_SAMPLE_BUFFER_MOY 	18		// (NEW_CHART_EVERY/(SAMPLE_EVERY/60))	Taille du buffer de calcule de la moyenne


// min max absolu
#define PRESSION_MAX	1150 	// PNM maximum : 1086 hPa, à Tosontsengel (Mongolie), le 19 décembre 2001.
#define PRESSION_MIN	800 	//PNM minimum : 870 hPa, au large des Philippines, près du centre du typhon Tip, le 12 octobre 1979

#define HPA 	0
#define dC	1
//#define DEBUG
unsigned int _auiTabMoyPress[NB_COL_LCD];	//tab histo de la pression des dernieres 24h 
int _aiTabMoyTemp[NB_COL_LCD];			//tab histo de la temp des dernieres 24h 

unsigned char _ucNbSampleMoy=0;	//Nb sample to compute moy
unsigned char _ucHistoIndex=0;	//current index in hito table 


unsigned char _ucKeyFlag=0;	// Sub menu flag

unsigned long _ulPressMoy=0;	// Moy pressure 
unsigned int _uiPression;	// Current pressure sample 

unsigned long _lTempMoy=0;	// Moy pressure 
int  _iTemp;			// Current Temp Centigrade sample

//Init Function		
void vInit(void);		
void Appli_vInit(void);
void vTimerInit(void);

//Periodic samples acquisition Task
void vGetSamples(void);
//Main Task
void vRun (void);

//Function Menu
void vFuncRaz(void);
void vFuncContrast(unsigned char ucContrast);
void vFuncTMinMax(char cMin);
void vFuncPMinMax(char cMin);
void vFuncMain(int iTemp,unsigned int uiPression);
void vFuncDayHisto(char cFlag,unsigned int auiValHisto[NB_COL_LCD],char cUnit);
/**************************************************************************/
void vInit(void)
{			
	CMCON=0b00000111;	// Close Comparator
	TRISA=0b00010000;	// inutile sans doute
	TRISB=0b00000001;	// inutile sans doute
	TRISC=0b11000000;	
	TRISD=0b00000000;	
	TRISE=0b00001000;	
	ADCON1=0b00001111;	// Digital Channel Allocation 
	Uart_vInit();
	Key_vInit();
	LM75_vInit();		// Temperature Sensor Initializtion
	LCD_vInit();		// Open LCD and display
	LED_SAMPLE=1;		//Led Initializtion
	LED_MAX=1;
	LED_MIN=1;
	ALIM_RF=0;
	Appli_vInit();
	vTimerInit();
}
void Appli_vInit(void)
{
	int i;

	for(i=0;i<NB_COL_LCD;i++)
	{
		_auiTabMoyPress[i]=EEPROM_iRead(HISTO_PRESS+2*i);
		_aiTabMoyTemp[i]=EEPROM_iRead(HISTO_TEMP+2*i);
	}
	_ucHistoIndex=EEPROM_iRead(INDEX_HISTO_JOUR);
#ifdef DEBUG
	printf("_ucHistoIndex %d\n\r",_ucHistoIndex);
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
/**************************************************************************/


/**************************************************************************/

void vRun (void)
{	
	unsigned char ucDownKey,ucRightKey;
	char acBuf[16];			// min max Centigrade Value Array

	ucRightKey=Key_ucRight();
	ucDownKey=Key_ucDown();
	if(_ucKeyFlag==0)
	{
		switch(ucRightKey)
		{	
		case(FUNC_INT):				//Display "Centigrade"
				{
			vFuncMain(_iTemp,_uiPression);
				}break;

		case(FUNC_HISTO_PRESS):
				{
			//affichage d'un histo de type pression
			vFuncDayHisto(ucDownKey%2,_auiTabMoyPress,HPA);
				}
		break;
		case(FUNC_HISTO_TEMP):
				{
			//affichage d'un histo de type Temp
			vFuncDayHisto(ucDownKey%2,_aiTabMoyTemp,dC);
				}
		break;
		case(FUNC_T):
				{
			vFuncTMinMax(ucDownKey%2);
				}
		break;

		case(FUNC_P):
				{
			vFuncPMinMax(ucDownKey%2);
				}
		break;
		case(FUNC_CONT):				//Display Contrast information
				{
			char Cont[]="Contrast        ";
			LCD_vDisplay(Cont);
				}break;
		case(FUNC_RAZ):				//Display Contrast information
				{	char Raz[]="RAZ             ";
				char acLcdName[]="Pic Epeiche     ";
				LCD_vWrite(0,0,acLcdName);
				LCD_vDisplay(Raz);
				}break;
		}
		if(Key_bDownPress()==TRUE)
		{
			_ucKeyFlag=1;
		}
	}
	else
	{

		switch(ucRightKey)
		{
		case FUNC_CONT :
			//sous menu contrast
			vFuncContrast(ucDownKey%NB_CONTRAST);
			break;
		case FUNC_RAZ :
			//sous menu raz
			vFuncRaz();

			_ucKeyFlag=0;
			break;
		case FUNC_P :
			//sous menu P min /max
			_ucKeyFlag=0;
			break;
		case FUNC_T :
			//sous menu T min /max
			_ucKeyFlag=0;
			break;
		default :
			//pas de sous menu
			_ucKeyFlag=0;
			break;	
		}
		if(Key_bRightPress()==TRUE)
			_ucKeyFlag=0;
	}

}

void vGetSamples(void)
{
	int iTmax,iTmin,iPmax,iPmin;
	unsigned int uiPressMoy;
	int iTempMoy;
	unsigned char i=0;
	
	iTmax=EEPROM_iRead(MAX_T);
	iTmin=EEPROM_iRead(MIN_T);
	iPmax=EEPROM_iRead(MAX_P);
	iPmin=EEPROM_iRead(MIN_P);

	_uiPression=MPX6115_uiGetPression();
	_iTemp=LM75_iGetTemp();

	_ulPressMoy+=_uiPression;
	_lTempMoy+=_iTemp;

	_ucNbSampleMoy++;

	if(_ucNbSampleMoy>=NB_SAMPLE_BUFFER_MOY)
	{
		_ucNbSampleMoy=0;

		uiPressMoy=_ulPressMoy/NB_SAMPLE_BUFFER_MOY;
		iTempMoy=_lTempMoy/NB_SAMPLE_BUFFER_MOY;
#ifdef DEBUG
		printf("uiPressMoy=%d iTempMoy=%d uiPressMoy=%ld iTempMoy=%ld nb=%d\n\r",uiPressMoy,iTempMoy,_ulPressMoy,_lTempMoy,NB_SAMPLE_BUFFER_MOY);
#endif
		
		_auiTabMoyPress[_ucHistoIndex%NB_COL_LCD]=uiPressMoy;
		_aiTabMoyTemp[_ucHistoIndex%NB_COL_LCD]=iTempMoy;

		EEPROM_vWrite(uiPressMoy,HISTO_PRESS+2*(_ucHistoIndex%NB_COL_LCD));
		EEPROM_vWrite(iTempMoy,HISTO_TEMP+2*(_ucHistoIndex%NB_COL_LCD));

		_ucHistoIndex++;
		EEPROM_vWrite(_ucHistoIndex,INDEX_HISTO_JOUR);
		_ulPressMoy=0;
	}


	if(_iTemp>iTmax)
	{
		LED_MAX=0;
		EEPROM_vWrite(_iTemp,MAX_T);
	}

	if(_iTemp<iTmin)
	{
		LED_MIN=0;
		EEPROM_vWrite(_iTemp,MIN_T);
	}

	if(_uiPression>iPmax)
	{
		if( _uiPression<PRESSION_MAX)
		{
			LED_MAX=0;
			EEPROM_vWrite(_uiPression,MAX_P);
		}
	}

	if(_uiPression<iPmin)
	{
		if( _uiPression>PRESSION_MIN)
		{
			LED_MIN=0;
			EEPROM_vWrite(_uiPression,MIN_P);
		}
	}
#ifdef DEBUG
	printf("%d %d %d %d %d %d\n\r",_iTemp,iTmax,iTmin,_uiPression,iPmax,iPmin);
#endif
	LED_MAX=1;
	LED_MIN=1;	
}

void vFuncMain(int iTemp,unsigned int uiPression)
{
	char acBuf[16];	
	sprintf(acBuf,"%d.%d C %d hPa  ",iTemp/2,(iTemp%2)*5,uiPression);
	LCD_vWrite(1,0,acBuf);

}

void vFuncPMinMax(char cMin)
{
	char acBuf[16];	
	int iP;
	if(cMin==TRUE)
	{
		iP=EEPROM_iRead(MIN_P);
		sprintf(acBuf,"Min %d hPa     ",iP);	
	}
	else
	{
		iP=EEPROM_iRead(MAX_P);
		sprintf(acBuf,"Max %d hPa     ",iP);	
	}
	LCD_vWrite(1,0,acBuf);
}

void vFuncTMinMax(char cMin)
{
	char acBuf[16];	
	int iT;

	if(cMin==TRUE)
	{
		iT=EEPROM_iRead(MIN_T);
		sprintf(acBuf,"Min %d.%d C     ",iT/2,(iT%2)*5);
	}
	else
	{
		iT=EEPROM_iRead(MAX_T);
		sprintf(acBuf,"Max %d.%d C     ",iT/2,(iT%2)*5);
	}
	LCD_vWrite(1,0,acBuf);
}

void vFuncContrast(unsigned char ucContrast)
{
	char acBuf[16];	
	vPwm(NB_CONTRAST-ucContrast-1);
	sprintf(acBuf,"%d%%  ",ucContrast*10);
	LCD_vWrite(1,11,acBuf);
}

void vFuncDayHisto(char cFlag,unsigned int auiValHisto[NB_COL_LCD],char cUnit)
{
	char i;
	char acBuf[20];	
	unsigned int uiMax,uiMin;

	if(cFlag==FALSE)
	{
		//affichage de l'histo
		LCD_vHisto(auiValHisto,_ucHistoIndex%NB_COL_LCD);
	}
	else
	{
		//Calcul des max /min
		uiMax=auiValHisto[0];
		uiMin=auiValHisto[0];
		for(i=0;i<NB_COL_LCD;i++)
		{
			if(auiValHisto[i]>uiMax)
				uiMax=auiValHisto[i];

			if(auiValHisto[i]<uiMin)
				uiMin=auiValHisto[i];
#ifdef DEBUG
			printf("(%d %d %d %d)\n\r",i,auiValHisto[i],uiMax,uiMin);
#endif
		}
#ifdef DEBUG
		printf("Max=%d Min=%d\n\r",uiMax,uiMin);
#endif		
		if(cUnit!=HPA)
		{
			//histo des °C diviser par 2
			//affichage des min/max
			sprintf(acBuf,"Max %d.%d C       ",uiMax/2,uiMax%2);	
			LCD_vWrite(0,0,acBuf);
	
			sprintf(acBuf,"Min %d.%d C       ",uiMin/2,uiMin%2);	
			LCD_vWrite(1,0,acBuf);

		}
		else
		{

			//affichage des min/max
			sprintf(acBuf,"Max %d hPa       ",uiMax);	
			LCD_vWrite(0,0,acBuf);
	
			sprintf(acBuf,"Min %d hPa       ",uiMin);	
			LCD_vWrite(1,0,acBuf);
		}
#ifdef DEBUG
		printf("Max=%d Min=%d\n\r",uiMax,uiMin);
#endif	
	}

}

void vFuncRaz(void)
{
	char acBuf[16];	
	int i;
	//raz temp
	EEPROM_vWrite(31*2,MAX_T);
	EEPROM_vWrite(29,MIN_T);

	//raz press
	EEPROM_vWrite(1035,MAX_P);
	EEPROM_vWrite(969,MIN_P);

	//raz histo
	for(i=0;i<NB_COL_LCD;i++)
	{
		EEPROM_vWrite(1005+i,HISTO_PRESS+2*i);
		EEPROM_vWrite(0+i,HISTO_TEMP+2*i);

	}
	Appli_vInit();
	EEPROM_vWrite(0,INDEX_HISTO_JOUR);

	sprintf(acBuf,"ok  ");
	LCD_vWrite(1,11,acBuf);
	for(i=0;i<1000;i++)
		Delay1KTCYx(5); // Delay of 1ms


}

/**************************************************************************/

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
		printf("PE %s %d\n\r",buf,checksum);

	ALIM_RF=0;
}

void main (void)
{
	unsigned int i;
	char buf[20];
	vInit();							// Initialize Control Microchip
	_uiPression=MPX6115_uiGetPression();
	_iTemp=LM75_iGetTemp();


	while(1)
	{
		if(cron_bActionSec(SAMPLE_EVERY)==TRUE)
		{
			LED_SAMPLE=0;
			vGetSamples();
			sprintf(buf,"%d %d %d",RF_ID,_uiPression,_iTemp);
			rfTransmit(buf,strlen(buf));
			LED_SAMPLE=1;
		}
		vRun();
		
	}
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

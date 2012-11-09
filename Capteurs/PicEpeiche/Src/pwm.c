#include <p18f4520.h>
//pwm par pas de 25
void vPwm(unsigned char i)
{
	CCP2CON=0b00001100;
	T2CONbits.TMR2ON = 0;
	T2CONbits.T2OUTPS3 = 1;
	T2CONbits.T2OUTPS2 = 1;
	T2CONbits.T2OUTPS1 = 1;
	T2CONbits.T2OUTPS0 = 1;
	T2CONbits.T2CKPS1 = 1;
	T2CONbits.T2CKPS1 = 1;
	PR2 = 255;
	TRISCbits.TRISC2=0;
	T2CONbits.TMR2ON = 1;
	CCPR2L = 25*i;
}

#include <p18f4520.h>
#include <key.h>
#include <delays.h>
#include <utils.h>
#define KEY_PRESS 	0
#define KEY_NO_PRESS 	1


unsigned char _ucLastKeyRightPressed;
unsigned char _ucLastKeyDownPressed;
unsigned char _ucNumRight=0;
unsigned char _ucNumDown=0;

#define DownKey 	PORTAbits.RA4		// Change key
#define RightKey 	PORTBbits.RB0		// Function key

void Key_vInit(void)
{
	TRISAbits.TRISA4=1;	//Down == input
	TRISBbits.TRISB0=1;	//RIGHT == input
}


unsigned char Key_ucDown(void)
{
	unsigned char ucTemp;
	unsigned char ucKeyDownPressed=DownKey;

	if((ucKeyDownPressed==KEY_PRESS)&(_ucLastKeyDownPressed==KEY_NO_PRESS))
	{
		Delay100TCYx(1);			//Delay to avoid buffeting
		if(ucKeyDownPressed==KEY_PRESS)
			_ucNumDown++;
	}
	_ucLastKeyDownPressed=ucKeyDownPressed;

	ucTemp=_ucNumDown;
	return ucTemp;
}

unsigned char Key_ucRight(void)
{
	unsigned char ucTemp;
	unsigned char ucKeyRightPressed=RightKey;
	if((ucKeyRightPressed==KEY_PRESS)&(_ucLastKeyRightPressed==KEY_NO_PRESS))
	{
		Delay100TCYx(1);			//Delay to avoid buffeting
		if(ucKeyRightPressed==KEY_PRESS)
		{
			_ucNumRight++;
			_ucNumDown=0;
		}
	}
	_ucLastKeyRightPressed=ucKeyRightPressed;

	ucTemp=_ucNumRight%NB_FUNC_RIGHT;
	return ucTemp;
}

char Key_bDownPress(void)
{
	if(DownKey==KEY_PRESS)
		return TRUE;
	else
		return FALSE;
}

char Key_bRightPress(void)
{
	if(RightKey==KEY_PRESS)
		return TRUE;
	else
		return FALSE;
}


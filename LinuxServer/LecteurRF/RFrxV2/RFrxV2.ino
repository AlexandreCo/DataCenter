#include <SoftwareSerial.h>

#define TRUE	1
#define FALSE	0



// set up a new serial port
#define rxPin 6
#define txPin 7

SoftwareSerial wireless(rxPin, txPin);

void setup ()
{
	int i;
	Serial.begin(57600);
	wireless.begin(4800);
	Serial.println("start");
}
unsigned long time;


int isAlphaNum(char c)
{
	if ((c>='a')&& (c<='z'))
	{
		return TRUE;
	}
	if ((c>='A')&& (c<='Z'))
	{
		return TRUE;
	}

	if ((c>='0')&&(c<='9'))
	{
		return TRUE;
	}

	switch(c)
	{
	case ' ':
	case '\n':
	case '\r':
		return TRUE;
		break;
	default : 
		return FALSE;
		break;
	}
	return FALSE;
}

void loop () 
{
	char cRead;
	if (wireless.available())
	{
		cRead=wireless.read();
		if(isAlphaNum(cRead)==TRUE)
		{
			Serial.write(cRead);
		}
	}
}


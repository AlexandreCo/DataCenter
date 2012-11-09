#include <NewSoftSerial.h>
#include <Messenger.h>

#define TRUE	1
#define FALSE	0

Messenger msgWireless = Messenger();

// set up a new serial port
#define rxPin 6
#define txPin 7

NewSoftSerial wireless(rxPin, txPin);

void setup () {
	int i;
	Serial.begin(57600);
	//msgWireless.attach(WirelessCompleted);
	// set the data rate for the SoftwareSerial port
	wireless.begin(4800);

	Serial.println("start");
}
unsigned long time;

void loop () {
	int i;

	while (wireless.available( )  )
	{
		uint8_t alpha=msgWireless.processAlphaNum(wireless.read());
		if(alpha)
			Serial.print(alpha);
	}
}


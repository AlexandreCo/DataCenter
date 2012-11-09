// Demo remote relay control
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
// 2012-10-28 <alexandre.github@gmail.com> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x45 };

//#define DHCP
#ifndef DHCP
// ethernet interface ip address
static byte myip[] = { 192, 168, 0, 15 };
// gateway ip address
static byte gwip[] = { 192, 168, 0, 254 };
#endif
byte Ethernet::buffer[700];
int relayPin = 4;
BufferFiller bfill;

// called when the client request is complete
static void my_callback(byte status, word off, word len) {
	Serial.println(">>>");
	Ethernet
	::buffer[off+300] = 0;
	Serial.print((const char*) Ethernet::buffer + off);
	Serial.println("...");
}



void setup() {
	pinMode(relayPin, OUTPUT);
	Serial.begin(57600);
	Serial.println("\n[webClient]");

	if (ether.begin(sizeof Ethernet::buffer, mymac,10) == 0)
		Serial.println("Failed to access Ethernet controller");
#ifdef DHCP
	if (!ether.dhcpSetup())
	Serial.println("DHCP failed");
#else
	ether.staticSetup(myip, gwip);
#endif    
	ether.printIp("IP:  ", ether.myip);
	ether.printIp("GW:  ", ether.gwip);
	ether.printIp("DNS: ", ether.dnsip);

}

static word homePage() {
	long t = millis() / 1000;
	word h = t / 3600;
	byte m = (t / 60) % 60;
	byte s = t % 60;
	bfill = ether.tcpOffset();
	bfill.emit_p(
			PSTR(
					"HTTP/1.0 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Pragma: no-cache\r\n"
							"\r\n"
							"<meta http-equiv='refresh' content='1'/>"
							"<title>RBBB server</title>"
							"<h1>uptime : $D$D:$D$D:$D$D</h1><h1><a href='/?rel=off'>off</a> <a href='/?rel=on'>on</a></h1>"),
			h / 10, h % 10, m / 10, m % 10, s / 10, s % 10);
	return bfill.position();
}

void loop() {

	word len = ether.packetReceive();
	word pos = ether.packetLoop(len);
	if (pos)  // check if valid tcp data is received
	{
		bfill = ether.tcpOffset();
		char* data = (char *) Ethernet
		::buffer + pos;
		//Serial.println(data);
		if (strncmp("GET /?rel=on ", data, 13) == 0) {
			//Serial.println("relay on");
			digitalWrite(relayPin, HIGH);   // sets the LED on
		}

		if (strncmp("GET /?rel=off ", data, 14) == 0) {
			//Serial.println("relay off");
			digitalWrite(relayPin, LOW);   // sets the LED on
		}

		ether.httpServerReply(homePage()); // send web page data

	}
}


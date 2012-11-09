/*
    This file is part of ThermRF.

    ThermRF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ThermRF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2011  Alexandre Coffignal <alexandre.coffignal[@]gmail.com>

*/
#include <pic16f628a.h>
#include "serial.h"


void serial_vInit()
{
	TRISB=TX_BIT|RX_BIT;	// These need to be 1 for USART to work

	SPBRG=SPBRG_VALUE;	// Baud Rate register, calculated by macro
	BRGH=BAUD_HI;

	SYNC=0;			// Disable Synchronous/Enable Asynchronous
	SPEN=1;			// Enable serial port
	TXEN=1;			// Enable transmission mode
	CREN=1;			// Enable reception mode

}
#ifdef DEBUG

void serial_vPrintNumber(unsigned long n, unsigned char base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    serial_vPut('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    serial_vPut((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void serial_vPrintBase(long n, int base)
{
  if (base == 0) {
    serial_vPut(n);
  } else if (base == 10) {
    if (n < 0) {
      serial_vPut('-');
      n = -n;
    }
    serial_vPrintNumber(n, 10);
  } else {
    serial_vPrintNumber(n, base);
  }
}

unsigned char serial_ucGet()
{
		unsigned char ucRet=0;
		while(!RCIF);	// Wait until data recieved
		ucRet=RCREG;	// Store for later
		return ucRet;
}

#endif

void serial_vPrint(char * buf)
{
	char i=0;
	while(buf[i]!='\0')
	{
		serial_vPut(buf[i++]);
	}
}
void serial_vPrintln(char * buf)
{
	serial_vPrint(buf);
	serial_vPut('\n');
	serial_vPut('\r');
}



void serial_vPut(char c)
{
		while(!TRMT);	// Wait until we're free to transmit
		TXREG=c;	// Transmit

}

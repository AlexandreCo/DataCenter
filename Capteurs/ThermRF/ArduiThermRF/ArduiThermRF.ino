/* thermrf version arduino
  2012-11-14 <alexandre.github@gmail.com> http://opensource.org/licenses/mit-license.php
 */

#include <DS1624.h> //from https://github.com/AlexandreCo/Arduino-i2c-temperature-sensor-DS1624
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define LED_PIN (13)
#define NB_TRANSMIT  3
volatile int f_timer=0;

/* 0x48 is the address of the DS1624 with three pin grounded. */
DS1624 temperature(0x48);

ISR(TIMER1_OVF_vect)
{
	/* update timer.*/
	f_timer ++;
}


void enterSleep(void)
{
	set_sleep_mode(SLEEP_MODE_IDLE);

	sleep_enable();


	/* Disable all of the unused peripherals. This will reduce power
	 * consumption further and, more importantly, some of these
	 * peripherals may generate interrupts that will wake our Arduino from
	 * sleep!
	 * via http://donalmorrissey.blogspot.fr/2011/11/sleeping-arduino-part-4-wake-up-via.html
	 */
	power_adc_disable();
	power_spi_disable();
	power_timer0_disable();
	power_timer2_disable();
	power_twi_disable();

	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the timer timeout*/
	sleep_disable(); /* First thing to do is disable sleep. */

	/* Re-enable the peripherals. */
	power_all_enable();
}

void setup() {  
	//Begin serial connection at 4800 baud
	Serial.begin(4800);
        temperature.start();
	/*** Configure the timer.***/
	/* Normal timer operation.*/
	TCCR1A = 0x00;

	/* Clear the timer counter register.
	 * You can pre-load this register with a value in order to
	 * reduce the timeout period, say if you wanted to wake up
	 * ever 4.0 seconds exactly.
	 */
	TCNT1=0x0000;

	/* Configure the prescaler for 1:1024, giving us a
	 * timeout of 4.09 seconds.
	 */
	TCCR1B = 0x05;

	/* Enable the timer overlow interrupt. */
	TIMSK1=0x01;
} 

#define RFID  2
void rf_vTransmit(float fTemp, unsigned int uiBat)
{
        unsigned char ucChecksum;
        char acBuf[25];
        sprintf(acBuf, "%d %d %d", RFID, (int)(fTemp*100),uiBat);
      
        for(int i=0;acBuf[i]!='\0';i++)
	{
		ucChecksum+=acBuf[i];
	}

        Serial.println(acBuf);
        
        sprintf(acBuf, "TH%d %d %d %d", RFID, (int)(fTemp*100),uiBat,ucChecksum);
        Serial.println(acBuf);
}

void loop() 
{
	if(f_timer>=1)
	{
		f_timer = 0;
		/* Toggle the LED */
		digitalWrite(LED_PIN, !digitalRead(LED_PIN));
		//Get the current temperature
                float fTemp=temperature.getTemp();
		Serial.println(fTemp);
                rf_vTransmit(fTemp,0);
		/* Re-enter sleep mode. */
		enterSleep();
	}
}

#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
//include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
#include <MAX17048.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


#define RTC_INT_Pin 2
#define Charge_EN A3
#define LED1 6
#define LED2 7
volatile boolean alarmIsrWasCalled = false;

void printDateTime(time_t t);
void RTC_init();
void alarmIsr();
void led_init();

MAX17048 pwr_mgmt;

void wake()
{
	// cancel sleep as a precaution
	sleep_disable();
	// precautionary while we do other stuff
	detachInterrupt(digitalPinToInterrupt(RTC_INT_Pin));
	power_usart0_enable();
	power_twi_enable();
	power_timer0_enable();// Timer 0

	digitalWrite(LED2, LOW);
	Serial.begin(115200);
	Serial.println("-------I woke----------");
}  // end of wake


void setup(void)
{


    Serial.begin(115200);
	RTC_init();
	pinMode(Charge_EN, OUTPUT);
	digitalWrite(Charge_EN, LOW);

	led_init();

	pwr_mgmt.attatch(Wire);
	pwr_mgmt.quickStart();

	pwr_mgmt.enSleep(true);

	Serial.println("------Setup Done------");

	// disable ADC
	/*
	power_adc_disable(); // ADC converter
	power_spi_disable(); // SPI
	power_timer0_disable();// Timer 0
	power_timer1_disable();// Timer 1
	power_timer2_disable();// Timer 2
	power_usart0_disable();
	power_twi_disable();
	wdt_disable();  // disable watchdog
	*/

}



void loop(void)
{
	digitalWrite(LED1, LOW);
	RTC_check();

	Serial.print("VCELL ADC : ");
	Serial.println(pwr_mgmt.adc());
	Serial.print("VCELL V   : ");
	Serial.println(pwr_mgmt.voltage());
	Serial.print("VCELL SOC : ");
	Serial.print(pwr_mgmt.percent());
	Serial.println(" \%");
	Serial.print("VCELL SOC : ");
	Serial.print(pwr_mgmt.accuratePercent());
	Serial.println(" \%");
	Serial.print("VCELL RATE : ");
	Serial.print(pwr_mgmt.crate());
	Serial.println(" %/hr");
	Serial.println();

	delay(100);
	ADCSRA = 0;
	power_adc_disable(); // ADC converter
	power_spi_disable(); // SPI
	power_timer0_disable();// Timer 0
	power_timer1_disable();// Timer 1
	power_timer2_disable();// Timer 2
	power_usart0_disable();
	power_twi_disable();
	wdt_disable();  // disable watchdog
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	// Do not interrupt before we go to sleep, or the
  // ISR will detach interrupts and we won't wake.
	noInterrupts();
	// will be called when pin D2 goes low  
	attachInterrupt(digitalPinToInterrupt(RTC_INT_Pin), wake, FALLING);
	EIFR = bit(INTF0);  // clear flag for interrupt 0

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above
	digitalWrite(LED1, HIGH);

	interrupts();             // guarantees next instruction executed
	sleep_cpu();              // sleep within 3 clock cycles of above


	delay(5000);
	digitalWrite(LED2, HIGH);
	Serial.println("=====Did delay++++");
	RTC.alarm(ALARM_1);
	time_t t = RTC.get();
	RTC.setAlarm(ALM1_MATCH_SECONDS, (second(t) + 10) % 60, 0, 0, 0);
}



/*-----------------------*/
/*--------RTC------------*/
/*-----------------------*/
void alarmIsr()
{
	alarmIsrWasCalled = true;
	
}


void printDateTime(time_t t)
{
    Serial.print( monthShortStr(month(t)));
    Serial.print(" ");
    Serial.print( day(t));
    Serial.print(", ");
    Serial.print((year(t)));
    Serial.print(" ");
    Serial.print(  (hour(t))) ;
    Serial.print(":");
    Serial.print(  (minute(t)));
    Serial.print(":");
    Serial.println(  (second(t)));
}

void RTC_init()
{
	
	Serial.println("RTC Sync");

	RTC.set(0, 54, 22, 2, 22, 1, 19);
	printDateTime(RTC.get());
	Serial.println(" --> Current RTC time.");

	//Disable the default square wave of the SQW pin.
	RTC.squareWave(SQWAVE_NONE);

	//Attach an interrupt on the falling of the SQW pin.
	//digitalWrite(SQW_PIN, HIGH);    //redundant with the following line
	pinMode(RTC_INT_Pin, INPUT_PULLUP);
	//attachInterrupt(digitalPinToInterrupt(RTC_INT_Pin), alarmIsr, FALLING);

	//Set an alarm at every 20th second of every minute. 
	//                               s, m , h, daydate
	RTC.setAlarm(ALM1_MATCH_SECONDS, 10, 0, 0, 0);    //daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_1, true);

	//Set an alarm every minute.
	//RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 1);    //daydate parameter should be between 1 and 7
	//RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
	//RTC.alarmInterrupt(ALARM_2, true);
	   
}

void RTC_check()
{
	if (alarmIsrWasCalled) {
		if (RTC.alarm(ALARM_1)) {
			time_t t = RTC.get();
			printDateTime(t);
			Serial.println(" --> Alarm 1!");
			RTC.setAlarm(ALM1_MATCH_SECONDS, (second(t) + 10) % 60, 0, 0, 0);    //daydate parameter should be between 1 and 7
		}
		alarmIsrWasCalled = false;
	}
}


void led_init()
{
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);


	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, LOW);

	delay(500);

	digitalWrite(LED1, LOW);
	digitalWrite(LED2, HIGH);

	delay(500);

	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, LOW);

	delay(500);

	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);

	delay(500);

	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);

}
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
//include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire

#define RTC_INT_Pin 2
#define Charge_EN A3
volatile boolean alarmIsrWasCalled = false;

void printDateTime(time_t t);
void RTC_init();
void alarmIsr();

void setup(void)
{
    Serial.begin(115200);
	RTC_init();
	pinMode(Charge_EN, INPUT_PULLUP);
	digitalWrite(Charge_EN, LOW);
}


void loop(void)
{
	RTC_check();

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
	attachInterrupt(digitalPinToInterrupt(RTC_INT_Pin), alarmIsr, FALLING);

	//Set an alarm at every 20th second of every minute. 
	//                               s, m , h, daydate
	RTC.setAlarm(ALM1_MATCH_SECONDS, 20, 0, 0, 0);    //daydate parameter should be between 1 and 7
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
			printDateTime(RTC.get());
			Serial.println(" --> Alarm 1!");
		}
		alarmIsrWasCalled = false;
	}
}
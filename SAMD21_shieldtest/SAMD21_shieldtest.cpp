/*
 * SAMD21_shieldtest.cpp
 *
 * Created: 3/20/2019 1:38:12 AM
 *  Author: mosta
 */ 
// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       SAMD21_shieldtest.ino
	Created:	3/15/2019 10:55:07 AM
	Author:     DESKTOP-4LJLPCB\mosta
*/

// Define User Types below here or use a .h file
//

#include <RTCZero.h>
#include "GlobalTypes.h"
#include "RTCLib.h"
#include "General.h"
#include <MKRGSM.h>
#include "SAMD21_shieldtest.h"


GSMClient client;
GPRS gprs;
GSM gsmAccess;

#include "Arduino.h"
// Define Function Prototypes that use User Types below here or use a .h file
//

volatile int flag = 0;
volatile int state = LOW;
volatile int printFlag = 0;
volatile unsigned long timeNow = 0;
volatile unsigned long timeLast = 0;
volatile unsigned long timeDiff = 0;
// Define Functions below here or use other .ino or cpp files
//


bool tcIsSyncing();
void tcDisable();




// The setup() function runs once each time the micro-controller starts
unsigned long baud = 115200;
void setup()
{

	Serial1.begin(115200);
	while (!Serial1);
	SerialGSM.begin(baud);
	while (!SerialGSM);
	GPIOSetup();
	GPIODefaults();
	GPIO_dance();
	Serial1.println("Starting...");
	Anemometer_Enable();


	
	delay(100);
	SerialGSM.println("AT+UPSV");
	SerialGSM.println("AT+CPWROFF");



	delay(400);

	setup_BME280();
	setup_battMang();
	setup_fram();


	Serial1.println(BME.readTempC());
	Serial1.println(pwr_mgmt.voltage());



	rtcTime.currTime.sec = 0;
	rtcTime.currTime.min = 0;
	rtcTime.currTime.hour = 5;
	rtcTime.currTime.day = 17;
	rtcTime.currTime.month = 3;
	rtcTime.currTime.year = 2019;
	rtcTime.alarmTime = rtcTime.currTime;
	rtcTime.alarmTime.sec = rtcTime.alarmTime.sec + 5;

	rtcSetup();
	rtcSetAlarm();
	//rtcEnable();

	Serial1.println("Entering Loop...");

	digitalWrite(GPLED2, LED_OFF);
	delay(100);
//	SCB->SCR.SLEEPDEEP=1;



	SYSCTRL->OSC32K.bit.ENABLE = 0;
	SYSCTRL->OSC32K.bit.RUNSTDBY = 0;
	SYSCTRL->BOD33.bit.ENABLE = 0;

	SYSCTRL->BOD33.bit.RUNSTDBY = 0;

	SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;
	SYSCTRL->XOSC.bit.RUNSTDBY = 0;
	
	//attachInterrupt(ANE_PULSE, pulseISR, LOW);


	//GCLK->GENDIV.reg = (GCLK_GENDIV_ID(4) | GCLK_GENDIV_DIV(0));
	//while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
	//	;                                                         /*XOSC32K*/
	//GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(4));
	//GCLK->GENCTRL.bit.DIVSEL = 0;
	//while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
	//	;
	////GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
	//GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4));
	//GCLK->CLKCTRL.bit.ID = 0x1A;
	//while (GCLK->STATUS.bit.SYNCBUSY)
	//	;





//	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_EIC) |
//		GCLK_CLKCTRL_GEN_GCLK1 |
//		GCLK_CLKCTRL_CLKEN;


//	GCLK->CLKCTRL.bit.ID = 5;

//	EIC->NMICTRL.bit.NMISENSE = 2;
//	EIC->NMICTRL.bit.NMIFILTEN = 1;
//	EIC->EVCTRL.bit.EXTINTEO0 = 0;
//	EIC->WAKEUP.bit.WAKEUPEN0 = 1;
//	EIC->CTRL.bit.ENABLE = 1;
	
	digitalWrite(GPLED2, LED_ON);

	Serial1.println(micros());
	delay(300);
	Serial1.println(micros());

	timeNow = micros();
	timeLast = timeNow;

	

	delay(100);
	rtc.standbyMode();

}

// Add the main program code into the continuous loop() function
void loop()
{
	//Serial1.begin(9600);

	//Serial1.print("Got woke");
	//delay(500);
	if (printFlag)
	{
		timeLast = micros();
		timeDiff = timeLast - timeNow;
		timeNow = timeLast;
		Serial1.println(timeDiff);
		printFlag = 0;
		digitalWrite(GPLED2, state);
		delay(10);
		rtc.standbyMode();
	}

//	rtc.standbyMode();

	


}

void printWind()
{
	float tempSpeed = 0.0;
	int tempDir = 0;
	int error = 0;

//	GPIO_dance();
//	Serial1.begin(9600);
//	Serial.begin(115200);
//	while (!SerialUSB);

	digitalWrite(GPLED2, LED_ON);
	rtcTime.alarmTime.sec = rtc.getSeconds()  +3;
	rtcSetAlarm();
	rtcEnable();
	//error = anemometer.read(tempSpeed, tempDir);
	//Serial1.print("speed is:");
	//Serial1.println(tempSpeed);
	//Serial1.print("dir is:");
//	Serial1.println(tempDir);
//	Serial1.print("error is:");
//	Serial1.println(error);
	//delay(2000);
}



//disable TC5
void tcDisable()
{
	TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

bool tcIsSyncing()
{
	return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}



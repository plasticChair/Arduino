// setupFunctions.h

#ifndef _SETUPFUNCTIONS_h
#define _SETUPFUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "pinMapGSM.h"
#include "Wire.h"

#define LED_ON 0
#define LED_OFF 1

extern BME280 BME;
extern MAX17048 pwr_mgmt;
extern Adafruit_FRAM_I2C fram;

void GPIOSetup();
void GPIODefaults();
void GPIO_dance();

void setup_BME280();
void setup_battMang();
void setup_fram();


void GPIOSetup()
{
	pinMode(GPLED1, OUTPUT);
	pinMode(GPLED2, OUTPUT);
	pinMode(ANE_Vcc_EN, OUTPUT);
	pinMode(ANE_TX, INPUT);
	pinMode(ANE_PULSE, INPUT);
	pinMode(DONE, OUTPUT);
	pinMode(WAKE, INPUT);
	pinMode(BATT_ALRT, INPUT);

}

void GPIODefaults()
{
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	  digitalWrite(GSM_RESETN, HIGH);
	  delay(100);
	  digitalWrite(GSM_RESETN, LOW);
	  

	digitalWrite(ANE_Vcc_EN, true);
	digitalWrite(DONE, false);

}


void GPIO_dance()
{

	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_ON);


}


void setup_BME280()
{
	BME.settings.I2CAddress = 0x76;

	BME.settings.runMode = 1;
	BME.settings.filter = 1;
	BME.settings.tempOverSample = 1;
	BME.settings.pressOverSample = 1;
	BME.settings.humidOverSample = 1;

	BME.begin();
	//delay();

}



void setup_fram()
{
	fram.begin();
	// Read the first byte
	uint8_t test = fram.read8(0x0);
	// Test write ++
	fram.write8(0x0, test + 1);

	Serial.print("Fram test = ");
	Serial.println(test);
}

void setup_battMang()
{
	pwr_mgmt.attatch(Wire);
	pwr_mgmt.quickStart();
	pwr_mgmt.vAlertMinThreshold(3.1);
	pwr_mgmt.enSleep(true);

}

#endif


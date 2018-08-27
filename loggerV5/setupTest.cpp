// 
// 
// 

#include "setupTest.h"

void basicSetup() {

	pinMode(SD_CS, OUTPUT);
	pinMode(Pin_GPS_pwr, OUTPUT);
	pinMode(Pin_Batt_mon_en, OUTPUT);
	pinMode(Pin_Batt_mon, INPUT);
	pinMode(Pin_BLE_pwr, OUTPUT);

	pinMode(Pin_fun1, INPUT);
	pinMode(Pin_fun2, INPUT);
	pinMode(Pin_fun3, INPUT);

	pinMode(Pin_led1, OUTPUT);
	pinMode(Pin_led2, OUTPUT);
	pinMode(Pin_led3, OUTPUT);
	pinMode(Pin_led4, OUTPUT);
	pinMode(sysLED, OUTPUT);

	digitalWrite(SD_CS, LOW);
	digitalWrite(Pin_GPS_pwr, (GPS_Enable));
	digitalWrite(Pin_BLE_pwr, (BLE_Enable));
	digitalWrite(Pin_Batt_mon_en, HIGH);

	digitalWrite(Pin_led1, HIGH);

	digitalWrite(Pin_led2, HIGH);
	digitalWrite(Pin_led3, HIGH);
	digitalWrite(Pin_led4, HIGH);
	digitalWrite(sysLED, LOW);
}
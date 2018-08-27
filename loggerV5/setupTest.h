// setupTest.h

#ifndef _SETUPTEST_h
#define _SETUPTEST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//#include "Logger_start.h"
#include "Options.h"



extern int sysLED;
extern int SD_CS;
extern int Pin_GPS_pwr;
extern int Pin_Batt_mon_en;
extern int Pin_Batt_mon;
extern int Pin_BLE_pwr;

extern int Pin_fun1;
extern int Pin_fun2;
extern int Pin_fun3;
 
extern int Pin_led1;
extern int Pin_led2;
extern int Pin_led3;
extern int Pin_led4;




void basicSetup();


#endif


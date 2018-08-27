// BLE.h

#ifndef _BLE_h
#define _BLE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Options.h"
extern int Pin_BLE_pwr;
extern HardwareSerial4 BLE_PORT;

void BLESetup();

#endif


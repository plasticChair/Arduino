// SAMD21_shieldtest.h

#ifndef _SAMD21_SHIELDTEST_h
#define _SAMD21_SHIELDTEST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Wire.h"
#include "Adafruit_FRAM_I2C.h"
#include "LaCrosse_TX23.h"
#include <MAX17048.h>
#include "SparkFunBME280_I2C.h"
#include "setupFunctions.h"
#include <RTCZero.h>
#include "RTCLib.h"
#include "pinMapGSM.h"


Adafruit_FRAM_I2C       fram = Adafruit_FRAM_I2C();
LaCrosse_TX23     anemometer = LaCrosse_TX23(ANE_TX);
BME280                   BME;
MAX17048            pwr_mgmt;
RTCZero                  rtc;
RTCStruct            rtcTime;

#define FRAM_RESET_CNT_Addr  1
#define FRAM_MEM_POINTER_Addr  1
#define FRAM_NUM_P0INTS_Addr  3
#define FRAM_MEAS_INT_Addr 4
#define FRAM_LP_MODE_Addr  5
#define FRAM_DATA_OFFSET 0x20
#define FRAM_CNTRL_OFFSET 0x10



#endif


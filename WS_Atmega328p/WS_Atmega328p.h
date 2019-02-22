#pragma once
#include "GlobalTypes.h"
#include "Wire.h"
#include "Adafruit_FRAM_I2C.h"
#include "LaCrosse_TX23.h"
#include <MAX17048.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "Arduino.h" // for digitalWrite
#include "SparkFunBME280_I2C.h"

#include "GlobalTypes.h"
#include "RTCCntrl.h"
#include "Init.h"
#include "General.h"


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define maxGustLimMPH 114
#define minGustLimMPH 1.5

#define maxGustLimuS 24 //round((1/(maxGustLimMPH/60))/1e-6/22)
#define minGustLimuS 1818 //round((1/(minGustLimMPH/60))/1e-6/22)



// Initilizations
Adafruit_FRAM_I2C       fram = Adafruit_FRAM_I2C();
LaCrosse_TX23     anemometer = LaCrosse_TX23(mANE_TX);
BME280                   BME;
CntrlStruct        MstrCntrl;
MstrDataStruct      MstrData;
storeDataUnion     storeData;
MAX17048            pwr_mgmt;
wxSFStruct              wxSF;
nativeData       sampledData;
IOStruct             IOCntrl;
//DS3232RTC                RTC;// = DS3232RTC(false);
volatile PulseStruct MstrISR;

volatile unsigned long Tmr2Cnt;
volatile unsigned long *Tmr2CntPtr;
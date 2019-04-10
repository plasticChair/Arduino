// SAMD21_shieldtest.h

#ifndef _MKR1400_WX_h
#define _MKR1400_WX_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <MKRGSM.h>
#include "GlobalTypes.h"
#include "Wire.h"
#include "Adafruit_FRAM_I2C.h"
#include "LaCrosse_TX23.h"
#include <MAX17048.h>
#include "SparkFunBME280_I2C.h"
#include "General.h"
#include "setupFunctions.h"
#include <RTCZero.h>
#include "RTCLib.h"
#include "pinMapGSM.h"
#include "UserParams.h"
#include "GSMFun.h"


// peripherals init
Adafruit_FRAM_I2C       fram = Adafruit_FRAM_I2C();
LaCrosse_TX23     anemometer = LaCrosse_TX23(ANE_TX);
BME280                   BME;
MAX17048            pwr_mgmt;
RTCZero                  rtc;

// Data structures setup
wxSFStruct               wxSF;
nativeData        sampledData;
serverStruct       serverData;
storeDataUnion       MstrData;
volatile RTCStruct    rtcTime;
volatile pulseStruct pulseData;
volatile CntrlStruct MstrCntrl;

volatile bool PulseISR_flag  = false;
volatile bool rtcISR_flag    = false;
volatile bool sendData_flag  = false;
volatile bool PulseLed_state = false;

// Server vars
char preJSONArray[200][(wxPackageSize+2)*2];
char JSONArray[2+(10+wxPackageSize*2)*30-1 +  30*2];
uint32_t measTime_SeverTx = ServerTx_INTERVAL;

void pulseISR();
void lowPowerISR();
void watchdogISR();


#endif


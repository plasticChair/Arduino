// GPS.h

#ifndef _GPS_h
#define _GPS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <NMEAGPS.h>
#include "Options.h"

static NMEAGPS  gps;

extern NMEAGPS gps;
extern HardwareSerial5 DEBUG_PORT;

extern void trace_header(Print & outs);
extern void doSomeWork();
extern void basicSetup();
extern int Pin_GPS_pwr;
extern int Pin_led3;

static gps_fix  fix_data;
static gps_fix SdFixInfo;


static void waitForFix();
void gpsSetup();
static void GPSloop();


#endif


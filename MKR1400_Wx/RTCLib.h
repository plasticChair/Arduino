// RTCLib.h

#ifndef _RTCLIB_h
#define _RTCLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

extern RTCZero rtc;
extern RTCStruct rtcTime;
extern void printWind();

void rtcSetup()
{

	rtc.begin(); //Start RTC library, this is where the clock source is initialized

	rtc.setTime(rtcTime.currTime.hour, rtcTime.currTime.min, rtcTime.currTime.sec); //set time
	rtc.setDate(rtcTime.currTime.day, rtcTime.currTime.month, rtcTime.currTime.year); //set date

	//rtc.attachInterrupt(printWind);

}

void rtcSetAlarm()
{
	rtc.setAlarmTime(rtcTime.alarmTime.hour, rtcTime.alarmTime.min, rtcTime.alarmTime.sec);
}


void rtcEnable()
{
	rtc.enableAlarm(rtc.MATCH_HHMMSS);
}


void rtcSleep()
{
	rtc.standbyMode(); //library call

}
#endif


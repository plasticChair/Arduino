// RTCLib.h

#ifndef _RTCLIB_h
#define _RTCLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <TimeLib.h>
extern RTCZero rtc;
extern volatile RTCStruct rtcTime;
extern void rtcISR();
extern int hour(time_t t);
extern int minute(time_t t);
extern int second(time_t t);
extern int day(time_t t);
extern int month(time_t t);
extern int year(time_t t);

void rtcSetup()
{

	rtc.begin(); //Start RTC library, this is where the clock source is initialized

	rtc.setTime(rtcTime.currTime.hour, rtcTime.currTime.min, rtcTime.currTime.sec); //set time
	rtc.setDate(rtcTime.currTime.day, rtcTime.currTime.month, rtcTime.currTime.year); //set date


}

void rtcSetGSMTime(time_t now)
{
		rtc.setTime(hour(now), minute(now), second(now)); //set time
		rtc.setDate(day(now), month(now), year(now)-34); //set date
	
}

void rtcSetAlarm(int minutesIN, int secondsIN)
{
	uint32_t tempSeconds = (rtc.getSeconds() + secondsIN);
	uint32_t tempMinutes = (rtc.getMinutes() + minutesIN);
	uint32_t tempHours   = 0;

	tempSeconds = (rtc.getSeconds() + secondsIN);
	tempMinutes = ((tempSeconds / 60) + tempMinutes);
	tempSeconds = tempSeconds % 60;
	tempHours = (rtc.getHours() + ( tempMinutes / 60)) % 60;
	tempMinutes = tempMinutes % 60;
	
	
	rtcTime.alarmTime.hour = tempHours;
	rtcTime.alarmTime.min = tempMinutes;
	rtcTime.alarmTime.sec = tempSeconds;
	
	rtc.setAlarmTime(rtcTime.alarmTime.hour, rtcTime.alarmTime.min, rtcTime.alarmTime.sec);
}

void rtcUpdateTime()
{
	rtcTime.currTime.hour  =  rtc.getHours();
	rtcTime.currTime.min   =  rtc.getMinutes();
	rtcTime.currTime.sec   =  rtc.getSeconds();
	rtcTime.currTime.day   =  rtc.getDay();
	rtcTime.currTime.month =  rtc.getMonth();
	rtcTime.currTime.year  =  rtc.getYear();
}

void rtcEnable()
{
	rtc.enableAlarm(rtc.MATCH_HHMMSS);
}

void rtcDisable()
{
	rtc.disableAlarm();
}


void rtcSleep()
{
	rtc.standbyMode(); //library call

}
#endif


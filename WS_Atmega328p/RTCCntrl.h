#pragma once
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include "GlobalTypes.h"
#include "WS_Atmega328p.h"


extern CntrlStruct MstrCntrl;

void RTC_SetAlarm_MeasInterval();
void RTC_SetAlarm_AneInterval();
//void RTC_ClearAlarm_2sInterval();
//void RTC_init();
void RTC_SetAlarm_Interval_15min();
void RTC_ClearAlarm_Interval2();
void RTC_ClearAlarm_Interval1();

void setup_RTC()
{
	RTC.squareWave(SQWAVE_NONE);
}

void RTC_SetAlarm_Interval_15min()
{
	//MstrCntrl.t = RTC.get();
	tmElements_t temp;
	byte temp2;
	breakTime(RTC.get(), MstrCntrl.time);

	RTC.setAlarm(ALM2_EVERY_MINUTE, 0, (byte)((MstrCntrl.time.Minute + 15) % 60), 0, 1);    //daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_2, true);
}

void RTC_SetAlarm_MeasInterval()  // input in minutes
{
	//MstrCntrl.t = RTC.get();
	tmElements_t temp;
	byte tempmin = 0;
	byte tempsecond = 0;
	breakTime(RTC.get(), MstrCntrl.time);

	RTC.squareWave(SQWAVE_NONE);


	if (MstrCntrl.Meas_Int < 1.0) {
		Serial.println("Setting Alarm");
		//setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate)

		Serial.println((byte)(MstrCntrl.time.Second + (uint8_t)((int)(MstrCntrl.Meas_Int * 60) % 60)));

		uint8_t temp = (uint8_t)((int)(MstrCntrl.Meas_Int * 60));
		RTC.setAlarm(ALM1_MATCH_SECONDS, (byte)((MstrCntrl.time.Second + temp) % 60), 0, 0, 1);    //daydate parameter should be between 1 and 7
		//RTC.setAlarm(ALM1_MATCH_SECONDS, (byte)((MstrCntrl.time.Second + 5)%60), 0, 0, 1);    //daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_1, true);
	}
	else {
		tempsecond = (byte)((round(((int)MstrCntrl.Meas_Int - MstrCntrl.Meas_Int) * 60)) % 60);

		RTC.setAlarm(ALM1_MATCH_MINUTES, tempsecond, (byte)((MstrCntrl.time.Minute + (int)floor(MstrCntrl.Meas_Int)) % 60), 0, 1);    //daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_1, true);
	}
}

void RTC_SetAlarm_AneInterval()
{
	//MstrCntrl.t = RTC.get();
	tmElements_t temp;
	byte temp2;
	breakTime(RTC.get(), MstrCntrl.time);

	RTC.setAlarm(ALM1_MATCH_SECONDS, (byte)((MstrCntrl.time.Second + MstrCntrl.Ane_Int) % 60), 0, 0, 1);    //daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_1, true);


}

void RTC_ClearAlarm_Interval1()
{
	RTC.alarmInterrupt(ALARM_1, false);
	//Restore
	RTC_SetAlarm_MeasInterval();
}

void RTC_ClearAlarm_Interval2()
{
	RTC.alarmInterrupt(ALARM_2, false);

}

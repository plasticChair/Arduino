/*
* SAMD21_shieldtest.cpp
*
* Created: 3/20/2019 1:38:12 AM
*  Author: mosta
*/
// Visual Micro is in vMicro>General>Tutorial Mode
//
/*
Name:       SAMD21_shieldtest.ino
Created:	3/15/2019 10:55:07 AM
Author:     DESKTOP-4LJLPCB\mosta
*/

// Define User Types below here or use a .h file
//


#include "MKR1400_Wx.h"

#define debug_GSM_EN 0

GSMBand band;
GSMClient client;
GPRS gprs;
GSM gsmAccess = 1;

// temp vars
int specialRTC_flag = 0;
int tempCount = 0;


// Define Function Prototypes that use User Types below here or use a .h file
#define IS_MTB_ENABLED \
REG_MTB_MASTER & MTB_MASTER_EN
#define DISABLE_MTB \
REG_MTB_MASTER = REG_MTB_MASTER & ~MTB_MASTER_EN
#define ENABLE_MTB \
REG_MTB_MASTER = REG_MTB_MASTER | MTB_MASTER_EN

__attribute__((aligned(1024)))
volatile char __tracebuffer__[1024];
volatile int __tracebuffersize__ = sizeof(__tracebuffer__);
void InitTraceBuffer()
{
	int index = 0;
	uint32_t mtbEnabled = IS_MTB_ENABLED;
	DISABLE_MTB;
	for(index =0; index<1024; index++)
	{
		__tracebuffer__[index];
		__tracebuffersize__;
	}
	if(mtbEnabled)
	ENABLE_MTB;
}
	


void setup()
{
	/* ----- Init vals ----- */
	init_vars();

	/* ----- GPIOs ----- */
	GPIOSetup();
	GPIODefaults();
	digitalWrite(GPLED2, LED_ON);
	GPIO_dance();

	/* ----- Anemometer ----- */
	Anemometer_Enable();

	/* ----- Power Mode ----- */
	powerSave();

	/* ----- Timer ----- */
	timerSetup();

	/* ----- GSM ----- */
	GSM_Disable();
	delay(100);

	#if debug_GSM_EN
		GSM_Enable();
		delay(8000);
		//GSM_PowerSave();
	#endif

	/* ----- Peripherals ----- */
	setup_BME280();
	setup_fram();
	setup_battMang();


	/* ----- RTC ----- */
	rtcTime.currTime.sec = 5;
	rtcTime.currTime.min = 55;
	rtcTime.currTime.hour = 0;
	rtcTime.currTime.day = 3;
	rtcTime.currTime.month = 4;
	rtcTime.currTime.year = 5;
	rtcSetup();
	delay(10);
	rtcSetAlarm(0, MstrCntrl.measInterval);
	rtcEnable();
	rtcUpdateTime();

	/* ----- ISR ----- */
	rtc.attachInterrupt(rtcISR);
	attachInterrupt(digitalPinToInterrupt(ANE_PULSE), pulseISR, FALLING);
	EIC_Setup();
	measTime_SeverTx = (rtcTime.currTime.min + MstrCntrl.serverInterval) % 60;

	/* ----- User ----- */
	Serial1.println("Entering Loop...");
	delay(10);
	digitalWrite(GPLED2, LED_OFF);
	//enterSleep();

	//--------- Check battery voltage and sleep if low
	LPModeCheck();
	
	Serial1.println(serverData.server);
	Serial1.println(serverData.serverIP);
	Serial1.println("Connecting....");


	// reset mem pointer, debug only
	fram.write8(FRAM_NUM_P0INTS_Addr, 0);
	fram.write8(FRAM_MEM_POINTER_Addr, 0);
	Serial1.println("Starting ********************************************");

	InitTraceBuffer();
	MstrCntrl.sleepSingle = true;
}

void pulseISR()
{
	detachInterrupt(digitalPinToInterrupt(ANE_PULSE));

	EIC->EVCTRL.bit.EXTINTEO2 = 0;
	pulseData.countVal = TC4->COUNT32.COUNT.bit.COUNT;
	if ((TC4->COUNT32.COUNT.bit.COUNT >700) && (pulseData.RTCFlag == false)) {  // min time between pulses,  )
		// Reset counter
		tcReset();

		// Set flags
		PulseISR_flag = true;
		PulseLed_state = !PulseLed_state;

		// Median filter
		medGustData();

		// Store in circular buffer
		pulseData.buffer[pulseData.buffIdx] = pulseData.countVal;
		pulseData.buffIdx++;
		if (pulseData.buffIdx >= PULSEDT_BUFFLENGTH) {
			pulseData.buffIdx = 0;
			findRunningPeaks();
		}
	}
	else {
		// Handle RTC
		pulseData.RTCFlag = false;
		tcReset();
	}
	EIC->EVCTRL.bit.EXTINTEO2 = 1;
	attachInterrupt(digitalPinToInterrupt(ANE_PULSE), pulseISR, FALLING);
	
}



// Add the main program code into the continuous loop() function
void loop()
{

	/*---------------------------------------*/
	//  Pulse ISR Flag
	/*---------------------------------------*/
	if (PulseISR_flag) {
		//Serial1.print("Pulse is: ");
		//Serial1.println(pulseData.countVal);
		//	Serial1.println(1.0F/(pulseData.countVal/32768.0F) * 60.0F/54.0F);
		PulseISR_flag = false;
		//	delay(10);
		//	digitalWrite(GPLED2,PulseLed_state);
	}


	/*---------------------------------------*/
	//  Capture Data
	/*---------------------------------------*/
	if (rtcISR_flag && !MstrCntrl.sleepModeEn) {
		detachInterrupt(digitalPinToInterrupt(ANE_PULSE));
		tcReset();
		rtcISR_flag = 0;
		MstrCntrl.RTCUpdateFlag = 1;
		/*-----Notify and reset RTC------*/
		Serial1.println("Measure sensors ---------------");
		tempCount++;
		Serial1.println(tempCount);
		// Setup RTC
		rtcSetAlarm(0, MstrCntrl.measInterval);
		rtcUpdateTime();
		
		// Get data
		//collectData();
		saveData();
		framSaveData();
		//LPModeCheck();

		// Prepare for sending GSM data
		if ((measTime_SeverTx - rtcTime.currTime.min) <= 1) {
			sendData_flag = true;
			#if debug_GSM_EN
				GSM_Enable();
				delay(8000);
				//GSM_PowerSave();
			#endif
		}

		//Check night time, if between hours of sleep and be sure this only happens once
		if (((rtcTime.currTime.hour >= MstrCntrl.sleepHour) || (rtcTime.currTime.hour <= MstrCntrl.wakeupHour)) && MstrCntrl.sleepSingle){
			Serial1.println("----------------- Sleep En --------------------------");
			printCurTime();
			MstrCntrl.sleepModeEn = true;
			MstrCntrl.sleepModeSet = true;
			measTime_SeverTx = rtcTime.currTime.min;
			sendData_flag = true;
		}
		if (rtcTime.currTime.hour >= MstrCntrl.sleepHour){
			MstrCntrl.sleepSingle = true;
		}

		attachInterrupt(digitalPinToInterrupt(ANE_PULSE), pulseISR, FALLING);
		printCurTime();
		Serial1.println("End RTc Sample");
		delay(10);
	}



	/*---------------------------------------*/
	//  Server Send
	/*---------------------------------------*/
	if ((rtcTime.currTime.min == measTime_SeverTx) && sendData_flag) {
		Serial1.println("Send data to Server");
		sendData_flag = 0;
		resetMaxGust();

		// Get data
		rtcUpdateTime();
		framReadData();
		
		if (MstrCntrl.FRAM_NumPoints > 0){
			//Send data
			buildJSON();
			#if debug_GSM_EN
				connectGPRS();
				getSignalStrength();
				MstrData.weather.signalRSSI = sampledData.rssi;
				sendPost(JSONArray);
				readGPRSResponse();
			#endif
		}	
		
		// Reset global array
		JSONArray[0] = 0;
	
		// reset FRAM
		fram.write8(FRAM_NUM_P0INTS_Addr, 0);
		fram.write8(FRAM_MEM_POINTER_Addr, 0);

		// Set new time
		measTime_SeverTx = (rtcTime.currTime.min + MstrCntrl.serverInterval) % 60;
		rtcSetAlarm(0, MstrCntrl.measInterval);
		
		printCurTime();
		delay(1);
	}


	/*---------------------------------------*/
	//  Set night time sleep mode
	/*---------------------------------------*/
	if (MstrCntrl.sleepModeEn &&  MstrCntrl.sleepModeSet) {
		Serial1.println("set night time sleep mode");

		printCurTime();
		rtcSetAlarm(6, 0); // 8 hours later, change to certain time

		//Dettach interrupt
 		tcReset();
		rtcISR_flag = 0;

		Anemometer_Disable();
		GSM_Disable();

		//Add sleep to BME280 and batt management

		rtcUpdateTime();
		
		//Only set sleep mode once
		 MstrCntrl.sleepModeSet = false;
	}


	/*---------------------------------------*/
	//  Handle night time sleep mode wake up
	/*---------------------------------------*/
	if ((rtcTime.currTime.hour >= MstrCntrl.wakeupHour) && (rtcTime.currTime.hour < MstrCntrl.sleepHour) && (MstrCntrl.sleepModeEn)) {
		MstrCntrl.sleepModeEn = false;
		MstrCntrl.sleepSingle = false;
		Serial1.println("----------------- Wake up from Sleep ----------------");
		#if debug_GSM_EN
			GSM_Enable();
			//delay(10000);
			connectGPRS();
			findIP(serverData.server, serverData.serverIP, PORT_IP);
			setFromGSMTime();
		#endif
		rtcUpdateTime();
		Anemometer_Enable();
		rtcSetAlarm(0, MstrCntrl.measInterval);
		measTime_SeverTx = (rtcTime.currTime.min + MstrCntrl.serverInterval) % 60;
		tcReset();
		attachInterrupt(digitalPinToInterrupt(ANE_PULSE), pulseISR, FALLING);
	}

	/*---------------------------------------*/
	//  Handle Low Power Situation
	/*---------------------------------------*/
	if ((MstrCntrl.LP_Mode == 1)  && (MstrCntrl.RTCUpdateFlag == 1))
	{
		Serial1.println("Check for LPM");
		MstrCntrl.RTCUpdateFlag = 0;
		//Remove low power mode if > 3.6V
		if (sampledData.batteryV >= 3.6)
		{
			Serial1.println("-----------set LP state OFF");
			//	MstrData.weather.LowPwrMode = 0;
			MstrCntrl.LP_Mode = 0;
			MstrCntrl.serverInterval = ServerTx_INTERVAL;
			MstrCntrl.measInterval = MEAS_INTERVAL;
		}
		//low power mode if < 3.35V, increase interval every time
		if (sampledData.batteryV <= 3.35)
		{
			Serial1.print("------------- set LP state again - ");
			Serial1.println(sampledData.batteryV);
			Serial1.println(MstrCntrl.measInterval);
			//MstrData.weather.LowPwrMode = 1;
			MstrCntrl.measInterval += MEAS_INTERVAL;
		}
	}

	//Serial1.println("Going to sleep");
	//delay(10);
	//enterSleep();

	// degug only
	if(specialRTC_flag == 1){
		specialRTC_flag = 0;
		digitalWrite(GPLED2, !digitalRead(GPLED2));
		printCurTime();
	}


}

void rtcISR()
{
	rtcISR_flag = 1;
	rtcUpdateTime();

specialRTC_flag = 1;
//delay(3);
}


void TC4_Handler(void) {
	TC4->COUNT32.INTFLAG.bit.MC0 = 1; //part of the timer code
}

void lowPowerISR()
{
	MstrCntrl.LP_Mode = 1;
	MstrCntrl.serverInterval += 15;
}

void watchdogISR()
{
	resetWatchDog();
	rtcUpdateTime();
}
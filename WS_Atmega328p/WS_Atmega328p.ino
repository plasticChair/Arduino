// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
Name:       WS_Attiny.ino
Created:	1/27/2019 8:13:08 PM
Author:     Scott
*/


#include "WS_Atmega328p.h"
//#include "Adafruit_FRAM_I2C.h"

extern void RTC_SetAlarm_AneInterval();
extern void RTC_SetAlarm_MeasInterval();

int BuffFull = 0;
volatile bool led2Cntrl = 0;
bool toggle = 0;
// The setup() function runs once each time the micro-controller starts
void setup()
{
	//setup_timer2();
	Serial.begin(115200);
	Serial.println("------Starting-------");

	setup_BME280();
	setup_battMang();
	setup_fram();

	init_vars();
    init_GPIO();
	
	setup_pwrSave();


	fram.write8(FRAM_MEM_POINTER_Addr,0);
	fram.write8(FRAM_NUM_P0INTS_Addr, 0);


	Wire.begin();
	//setup_RTC();

	//Set Time
	RTC.set(10, 1, 1, 1, 1, 1,19);
	
	RTC_SetAlarm_MeasInterval();
	//RTC_SetAlarm_AneInterval();
	//byte set(byte seconds_in, byte minutes_in, byte hour_in, byte wday_in, byte day_in, byte month_in, byte year_in);
	setup_watchpuppy();
	
	GPIO_dance();

	Serial.println("Done with Setup");

	//sleep();
	
	
	//delay(5);
}





// Add the main program code into the continuous loop() function
void loop()
{
	float tempSpeed = 0.0;
	int tempDir = 0;
	int Ane_ErrorStatus = 0;

//	Serial.println("IN LOOP");

	//MstrCntrl.serviceRTCISR = 1;
	/*--------------------------------------*/
	/* ------------  Store Data ------------*/
	//MstrISR.serviceRTCISR = 0;
	if (MstrISR.serviceRTCISR) {

		disable_timer2();
		//disable INT PulseISR
		detachInterrupt(digitalPinToInterrupt(IOCntrl.ANE_Pulse));
		// Reset RTC Alarm
		Serial.println("---woke---");
		printDateTime();
		//RTC_SetAlarm_MeasInterval();



		//Check if 15 mins elapsed
		breakTime(RTC.get(), MstrCntrl.time);
		if ((MstrCntrl.time.Minute - MstrCntrl.prevtime.Minute) >= MstrCntrl.Server_Int) {
			MstrCntrl.prevtime = MstrCntrl.time;
			MstrCntrl.Server_Tx_En = 1;
		}

		
		//Interval Elapsed, Start collecting data
		MstrCntrl.ANE_enable = 1;
		digitalWrite(IOCntrl.ANE_En, !MstrCntrl.ANE_enable);
		RTC_SetAlarm_AneInterval();
		
		MstrISR.pulseSkip = 1;

		//delay(2000);
		//Go to Sleep
		sleep();


		//wake up 

	
		printDateTime();
		RTC_SetAlarm_MeasInterval();
		//Serial.println("3");

		enable_timer2();
		delay(10);
		/*---------------------------------------------------------------------------------------*/
		// Read wind data
		Ane_ErrorStatus = anemometer.read(sampledData.speed, sampledData.dir);
		//Serial.println(Ane_ErrorStatus);
		//error detect
		if (Ane_ErrorStatus < 4)
		{
			delay(100);
			anemometer.read(tempSpeed, tempDir);
		}
		//
		MstrCntrl.ANE_enable = 0;
		digitalWrite(IOCntrl.ANE_En, !MstrCntrl.ANE_enable);

		Serial.print("Wind info:" + String(sampledData.speed) + ", " + String(sampledData.dir) + ", " + String(sampledData.gustMax));
		//Serial.println(String(MstrData.gustBuff[7]) + ", " + String(MstrData.gustBuff[6]) + ", " + String(MstrData.gustBuff[5]) + ", " + String(MstrData.gustBuff[4]) + ", " + String(MstrData.gustBuff[3]) + ", " + String(MstrData.gustBuff[2]) + ", " + String(MstrData.gustBuff[1]) + ", " + String(MstrData.gustBuff[0]) );


		/*---------------------------------------------------------------------------------------*/
		//Get data
		Serial.print("Got temps: ");
		BME_Force();
		sampledData.temp  = BME.readTempC();
		sampledData.press = BME.readFloatPressure()/100.0;
		sampledData.humid = BME.readFloatHumidity();
		Serial.println(sampledData.temp);
		
		/*-----------------*/
		//Check battery level
		sampledData.batteryV = pwr_mgmt.voltage();
		sampledData.batteryP = pwr_mgmt.accuratePercent();
		Serial.println(sampledData.batteryV);
		Serial.println(sampledData.batteryP);

		/*-----------------*/
		//Store Time
		MstrData.weather.year   = (byte)(MstrCntrl.time.Year - 49);
		MstrData.weather.month  = (byte)(MstrCntrl.time.Month);
		MstrData.weather.day    = (uint8_t)(MstrCntrl.time.Day);
		MstrData.weather.hour   = (uint8_t)(MstrCntrl.time.Hour);
		MstrData.weather.minute = (uint8_t)(MstrCntrl.time.Minute);
		MstrData.weather.second = (uint8_t)(MstrCntrl.time.Second+2); //Account for 2 second delay for Anemometer

		/*---------------------------------------------------------------------------------------*/
		//Low Power Mode
		LP_mode_check();

		/*---------------------------------------------------------------------------------------*/
		MstrData.weather.speed    = (unsigned int)floatLimit(&sampledData.speed     , wxSF.windSF    , 0            , 114 , 0);
		MstrData.weather.dir      = 0xff & tempDir;

		MstrData.weather.gustMax  = (unsigned int)floatLimit(&sampledData.gustMax   , wxSF.gustSF    , 0            , 2047, 0);

		MstrData.weather.temp     = (unsigned int)floatLimit(&sampledData.temp      , wxSF.tempSF    , wxSF.tempOff , 65  , -30);
		MstrData.weather.press    = (unsigned int)floatLimit(&sampledData.press     , wxSF.pressSF   , wxSF.pressOff, 1100, 900); //hPa
		MstrData.weather.humid    = (unsigned int)floatLimit(&sampledData.humid     , wxSF.humidSF   , 0            , 100 , 0);

		MstrData.weather.batteryV = (unsigned int)floatLimit(&sampledData.batteryV  , wxSF.batteryVSF, wxSF.battOff , 4.2 , 3);
		MstrData.weather.batteryP = (unsigned int)floatLimit(&sampledData.batteryP  , wxSF.batteryPSF, 0            , 100 , 0);

		MstrData.weather.info = 0;
		
		storeData.weather = MstrData.weather;

		Serial.println("Stored Data");
		delay(10);

		//Write to FRAM
		
		MstrCntrl.FRAM_Idx = fram.read8(FRAM_NUM_P0INTS_Addr);
		MstrCntrl.FRAM_Idx != (fram.read8(FRAM_NUM_P0INTS_Addr+1) << 8);
		Serial.println("Mem pointer: " + String(MstrCntrl.FRAM_Idx));

		for (int ii = 0; ii < wxPackageSize; ii++) {
			fram.write8(MstrCntrl.FRAM_Idx*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET, storeData.TxData[ii]);
		}
		MstrCntrl.FRAM_Idx++;
		fram.write8(FRAM_NUM_P0INTS_Addr, MstrCntrl.FRAM_Idx & 0xFF);
		fram.write8(FRAM_NUM_P0INTS_Addr+1, MstrCntrl.FRAM_Idx >> 8);
		
		/*
		Serial.println("-----DeCODE----");

		Serial.println(storeData.weather.year);
		Serial.println(storeData.weather.month);
		Serial.println(storeData.weather.day);
		Serial.println(storeData.weather.hour);
		Serial.println(storeData.weather.minute);
		Serial.println(storeData.weather.second);
		Serial.println(storeData.weather.dir);
		Serial.println("floats");
		Serial.println((float)storeData.weather.speed * 0.1114);
		Serial.println((float)storeData.weather.gustMax * 6.9423);
		Serial.println("BME");
		Serial.println((float)storeData.weather.temp * 0.0929 -30);
		Serial.println(((float)storeData.weather.press *0.1955 +900)*100);
		Serial.println((float)storeData.weather.humid *0.0978);
		Serial.println("BATT");
		Serial.println(storeData.weather.batteryV );
		Serial.println(((float)storeData.weather.batteryV *0.0047)+3);
		Serial.println(storeData.weather.batteryP );
		Serial.println((float)storeData.weather.batteryP *0.3922);
		*/

		
		MstrISR.serviceRTCISR = 0;
		Serial.println("----------Saved Data---------");
		delay(20);
		attachInterrupt(digitalPinToInterrupt(IOCntrl.ANE_Pulse), PulseISRRoutine, FALLING);
		MstrISR.pulseSkip = 1;
		//Restart Timer2
		//enable_timer2();
	}





	MstrCntrl.Server_Tx_En = 0;
	/*---------------------------------------*/
	//  Server Send
	/*---------------------------------------*/
	if (MstrCntrl.Server_Tx_En)
	{
		//RTC_ClearAlarm_Interval2();
		//Read RAM one line at time
		MstrCntrl.FRAM_Idx = 0;
		MstrCntrl.FRAM_Idx = fram.read8(FRAM_NUM_P0INTS_Addr);
		MstrCntrl.FRAM_Idx |= (fram.read8(FRAM_NUM_P0INTS_Addr+1) << 8);
		for (int ii = 0; ii < MstrCntrl.FRAM_Idx; ii++) {
			for (int kk = 0; kk < wxPackageSize; kk++) {
				storeData.TxData[kk] = fram.read8(kk + FRAM_DATA_OFFSET + ii * FRAM_DATA_OFFSET);
				//Delete FRAM data
				fram.write8(kk + FRAM_DATA_OFFSET + ii * FRAM_DATA_OFFSET, 0);

			}

			//Send data to GSM Module
		}
		MstrCntrl.FRAM_NumPoints = 0;
		MstrCntrl.FRAM_Idx = 0;

		MstrCntrl.Server_Tx_En = 0;
	}




	/*----------------------------------------*/
	/* ------------  Pulse Detect ------------*/
	/*----------------------------------------*/
	if (MstrISR.servicePulseISR) {
		digitalWrite(11, HIGH);
		MstrData.gustBuff[MstrCntrl.PulseBuffIdx] = (MstrISR.pulseDt);
		MstrCntrl.PulseBuffIdx++;
		if (MstrCntrl.PulseBuffIdx >= PulseBuffSize) {
			MstrCntrl.PulseBuffIdx = 0;
			BuffFull = 1;
		}
		avgGustData();
		//Serial.println("--------------");
		//Serial.println(MstrData.gustAvg);
		//Serial.println(MstrISR.pulseDt);
		if (BuffFull) {
			findRunningPeaks();
		//	Serial.print("-----");
		//	Serial.println(MstrData.gustMax);
			sampledData.gustMax = MstrData.gustMax;
		}
		
		delay(2);
		MstrISR.servicePulseISR = 0;
		digitalWrite(11, LOW);
	}
	

	//watch puppy
	if (MstrISR.WP_Alert == 1) {
		digitalWrite(IOCntrl.DONE, HIGH);
		delay(1);
		digitalWrite(IOCntrl.DONE, LOW);
		MstrISR.WP_Alert = 0;
	}


	sleep();

	
}




/*------------------------------------------*/
/*--------------- ISR Routine --------------*/
/*------------------------------------------*/

void PulseISRRoutine() 
{
	if (MstrISR.pulseSkip) {
		MstrISR.pulseCt = millis();
		MstrISR.pulseSkip = 0;

	}
	else {
		MstrISR.pulseCt = millis();
		MstrISR.pulseDt = MstrISR.pulseCt - MstrISR.pulsePt;
	
		if (MstrISR.pulseDt > maxGustLimuS)
		{
			MstrISR.pulsePt = MstrISR.pulseCt;

			if (MstrISR.pulseDt > minGustLimuS) {
				MstrISR.pulseDt = minGustLimuS;
				MstrISR.servicePulseISR = 1;
			}
			else {
				MstrISR.servicePulseISR = 1;

			}

		}
	}
	EIFR = bit(INTF1);
}



void RTCISRRoutine() 
{
	MstrISR.serviceRTCISR = 1;
	MstrISR.RTC_count++;
	EIFR = bit(INTF0);
}

ISR(PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
	if (digitalRead(IOCntrl.BATT_ALRT) == 0) {
		Serial.println("Low Battery!");
		delay(5);
	}
}

ISR(PCINT1_vect) // handle pin change interrupt for D0 to D7 here
{
	if (digitalRead(IOCntrl.WAKE) == 0) {
		Serial.println("Got Alert");
		MstrISR.WP_Alert = 1;
	}
}
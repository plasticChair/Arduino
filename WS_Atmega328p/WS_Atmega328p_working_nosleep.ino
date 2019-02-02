// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
Name:       WS_Attiny.ino
Created:	1/27/2019 8:13:08 PM
Author:     Scott
*/


#include "WS_Atmega328p.h"
//#include "Adafruit_FRAM_I2C.h"

// Prototypes
void init_I2CM();
void init_I2CS();
void init_GPIO();
void init_vars();
void EnableSleep();
void avgGustData();
void PulseISRRoutine();
void RTCISRRoutine();
void printDateTime(time_t t);
void RTC_init();
void RTC_SetAlarm_Interval(byte select);
void BME280_setup();
unsigned long floatLimit(float value, float scalefactor, int offset, float max, float min);
void printDateTime(time_t t);

// Initilizations
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
LaCrosse_TX23 anemometer = LaCrosse_TX23(ANE_TX);
BME280 BME;
CntrlStruct MstrCntrl;
MstrDataStruct MstrData;
storeDataUnion storeData;
MAX17048 pwr_mgmt;
wxSFStruct wxSF;

volatile PulseStruct MstrsPulse;

int BuffFull = 0;
// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	Serial.println("------Starting-------");

	RTC_SetAlarm_Interval(2);

	fram.begin();
	

	setup_BME280();

	pwr_mgmt.attatch(Wire);
	pwr_mgmt.quickStart();
	pwr_mgmt.enSleep(true);

	
	// Read the first byte
	uint8_t test = fram.read8(0x0);
	// Test write ++
	fram.write8(0x0, test + 1);

	Serial.print("Fram test = ");
	Serial.println(test);

	init_vars();

	// Disable ADC
	adc_disable();
	PRR |= _BV(PRADC);

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above

	power_adc_disable(); // ADC converter
	power_spi_disable(); // SPI
	//power_timer0_disable();// Timer 0
	//power_timer1_disable();// Timer 1
//	power_timer2_disable();// Timer 2

	//power_usart0_disable();
	//power_twi_disable();
	wdt_disable();  // disable watchdog

							  // Set Sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);


	fram.write8(FRAM_MEM_POINTER_Addr,0);
	fram.write8(FRAM_NUM_P0INTS_Addr, 0);


	//Set Time
	RTC.set(1, 1, 1, 1, 1, 1,19);
	//byte set(byte seconds_in, byte minutes_in, byte hour_in, byte wday_in, byte day_in, byte month_in, byte year_in);

	init_GPIO();
	Serial.println("Done with Setup");
}





// Add the main program code into the continuous loop() function
void loop()
{
	float tempSpeed = 0.0;
	int tempDir = 0;

	//EIFR = bit(INTF0);  // clear flag for interrupt 0
	MstrCntrl.serviceRTCISR = 0;

	/* ------------  Store Data ------------*/
	if (MstrCntrl.serviceRTCISR) {
		// Reset RTC Alarm

		//RTC_SetAlarm_Interval(2);

		//Check if 15 mins elapsed
		if ((MstrCntrl.time.Minute - MstrCntrl.prevtime.Minute) >= 15) {
			MstrCntrl.prevtime = MstrCntrl.time;
			MstrCntrl.Server_Int = 1;
		}
		printDateTime(MstrCntrl.t);


		//Interval Elapsed, collect data
		MstrCntrl.ANE_enable = 1;
		digitalWrite(ANE_En, MstrCntrl.ANE_enable);
		//RTC_SetAlarm_Interval(1);


		//disable INT PulseISR
		detachInterrupt(digitalPinToInterrupt(ANE_Pulse));
		MstrCntrl.pulseSkip = 1;


		delay(2000);
		//Go to Sleep
		//sleepy();


		//wake up 
		RTC.alarmInterrupt(ALARM_1, false);

		// Read wind data
		anemometer.read(tempSpeed, tempDir);
		Serial.print("speed is:");
		Serial.println(tempSpeed);
		Serial.print("dir is:");
		Serial.println(tempDir);
		
		MstrData.weather.speed = (unsigned int) floatLimit(tempSpeed, wxSF.windSF, 0,114,0);
		MstrData.weather.dir = 0xff & tempDir;

		// Gusts
		MstrData.weather.gustMax = (unsigned int)floatLimit((float)MstrData.gustMax, wxSF.gustSF, 0, 114, 0);
		Serial.print("gust is:");
		Serial.println(MstrData.weather.gustMax);
		Serial.println(MstrData.gustAvg);
		Serial.println(String(MstrData.gustBuff[7]) + ", " + String(MstrData.gustBuff[6]) + ", " + String(MstrData.gustBuff[5]) + ", " + String(MstrData.gustBuff[4]) + ", " + String(MstrData.gustBuff[3]) + ", " + String(MstrData.gustBuff[2]) + ", " + String(MstrData.gustBuff[1]) + ", " + String(MstrData.gustBuff[0]) );

		//error detect
		//
		//
		MstrCntrl.ANE_enable = 0;
		digitalWrite(ANE_En, MstrCntrl.ANE_enable);

		//Get BME280 data
		Serial.print("Got temps: ");
		BME_Force();
		float tempVar = 0.0;
		tempVar = BME.readTempC();
		Serial.println(tempVar);
		MstrData.weather.temp = (unsigned int)floatLimit(tempVar, wxSF.tempSF , wxSF.tempOff,65,-30);
		tempVar = BME.readFloatPressure()/100.0;
		Serial.println(tempVar);
		MstrData.weather.press = (unsigned int)floatLimit(tempVar, wxSF.pressSF, wxSF.pressOff,1100,900); //hPa

		tempVar = BME.readFloatHumidity();
		MstrData.weather.humid = (unsigned int)floatLimit(tempVar, wxSF.humidSF, 0,100,0);
		Serial.println(tempVar);

		//Check battery level
		Serial.print("Batt is: ");
		tempVar = pwr_mgmt.voltage();
		Serial.println(tempVar);
		MstrData.weather.batteryV = (unsigned int)floatLimit(4.0, wxSF.batteryVSF, wxSF.battOff, 4.2, 3);
		tempVar = 87.88; //pwr_mgmt.accuratePercent();
		MstrData.weather.batteryP = (unsigned int)floatLimit(tempVar, wxSF.batteryPSF, 0, 100, 0);
		
		Serial.println(tempVar);

		delay(10);
		//Store Time
		Serial.println("time Data");
		Serial.println(MstrCntrl.time.Year);
		Serial.println(MstrCntrl.time.Month);
		Serial.println(MstrCntrl.time.Day);
		Serial.println(MstrCntrl.time.Hour);
		Serial.println(MstrCntrl.time.Minute);
		Serial.println(MstrCntrl.time.Second);


		MstrData.weather.year   = (byte)(MstrCntrl.time.Year - 49) ;
		MstrData.weather.month  = (byte)(MstrCntrl.time.Month);
		MstrData.weather.day    = (uint8_t)(MstrCntrl.time.Day);
		MstrData.weather.hour   = (uint8_t)(MstrCntrl.time.Hour);
		MstrData.weather.minute = (uint8_t)(MstrCntrl.time.Minute);
		MstrData.weather.second = (uint8_t)(MstrCntrl.time.Second);


		//Enable / disable charging
		



		storeData.weather = (MstrData.weather);

		Serial.println("Stored Data");
		delay(10);

		//Write to FRAM
		MstrCntrl.FRAM_Idx = fram.read8(FRAM_NUM_P0INTS_Addr);
		Serial.print("Mem pointer: ");
		Serial.println(MstrCntrl.FRAM_Idx);

		for (int ii = 0; ii < wxPackageSize; ii++) {
			
			fram.write8(MstrCntrl.FRAM_Idx*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET, storeData.TxData[ii]);
			//Serial.println(storeData.TxData[ii]);
			//delay(1);

		}
		fram.write8(FRAM_NUM_P0INTS_Addr, fram.read8(FRAM_NUM_P0INTS_Addr)+1);
		
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




		attachInterrupt(digitalPinToInterrupt(ANE_Pulse), PulseISRRoutine, FALLING);
		MstrCntrl.serviceRTCISR = 0;
		Serial.println("----------Saved Data---------");
	}

	MstrCntrl.Server_Int = 0;
	//  Server Send
	if (MstrCntrl.Server_Int)
	{
		//Read RAM one line at time
		MstrCntrl.FRAM_NumPoints = fram.read8(FRAM_NUM_P0INTS_Addr);
		for (int ii = 0; ii < MstrCntrl.FRAM_NumPoints; ii++) {
			for (int kk = 0; kk < wxPackageSize; kk++) {
				storeData.TxData[kk] = fram.read8(kk + FRAM_DATA_OFFSET + ii * FRAM_DATA_OFFSET);
				//Delete FRAM data
				fram.write8(kk + FRAM_DATA_OFFSET + ii * FRAM_DATA_OFFSET, 0);

			}

			//Send data to GSM Module
		}
		MstrCntrl.FRAM_NumPoints = 0;
		MstrCntrl.FRAM_Idx = 0;

		MstrCntrl.Server_Int = 0;
	}



	/* ------------  Pulse Detect ------------*/
	if (MstrCntrl.servicePulseISR) {
		MstrData.gustBuff[MstrCntrl.PulseBuffIdx] = (MstrsPulse.pulseDt >> 8);
		MstrCntrl.PulseBuffIdx++;
		if (MstrCntrl.PulseBuffIdx >= PulseBuffSize) {
			MstrCntrl.PulseBuffIdx = 0;
			BuffFull = 1;
		}

		avgGustData();
		Serial.print("--------------");
		Serial.println(MstrData.gustAvg);
		if (BuffFull) {
			findRunningPeaks();
			Serial.print("-----");
			Serial.println(MstrData.gustMax);
		}
		MstrCntrl.servicePulseISR = 0;
	}


	delay(1000);

}

/*------------------------------------------*/
/*--------------- ISR Routine --------------*/
/*------------------------------------------*/

void PulseISRRoutine() {

	//PCMSK &= ~_BV(PCINT1);

	if (MstrCntrl.pulseSkip) {
		MstrsPulse.pulseCt = micros();
		MstrCntrl.pulseSkip = 0;

	}
	else {
		// This is called when the interrupt occurs, but I don't need to do anything in it
		MstrsPulse.pulseCt = micros();
		MstrsPulse.pulseDt = MstrsPulse.pulseCt - MstrsPulse.pulsePt;
		MstrsPulse.pulsePt = MstrsPulse.pulseCt;

		if (MstrsPulse.pulseDt > minGustLimuS) {
			MstrsPulse.pulseDt = minGustLimuS;
			MstrCntrl.servicePulseISR = 1;
		}
		else if(MstrsPulse.pulseDt > maxGustLimuS) {
			MstrCntrl.servicePulseISR = 1;
		}
	}
	
	//PCMSK |= _BV(PCINT1);
}



void RTCISRRoutine() {
	MstrCntrl.serviceRTCISR = 1;
	MstrCntrl.RTC_count++;
}


/*------------------------------------------*/
/*------------- Init Functions -------------*/
/*------------------------------------------*/


void init_GPIO()
{
	pinMode(ANE_TX, INPUT);
	pinMode(ANE_Pulse, INPUT);
	pinMode(ANE_En, OUTPUT);
	digitalWrite(ANE_En, LOW);
	//delay(300000);
	////digitalWrite(ANE_En, HIGH);
	//delay(3000);
	//digitalWrite(ANE_En, LOW);
	attachInterrupt(digitalPinToInterrupt(ANE_Pulse), PulseISRRoutine, FALLING);
	//attachInterrupt(digitalPinToInterrupt(RTC_Alarm), RTCISRRoutine, FALLING);
}

void init_vars()
{
	MstrData.gustAvg = 0;
	MstrData.weather.gustMax = 0;
	MstrData.weather.dir = 0;
	MstrData.weather.speed = 0.0;
    MstrData.gustMax = 7201;

	MstrCntrl.gustAvgCnt = 0;
	MstrCntrl.dataStoreCnt = 0;
	MstrCntrl.servicePulseISR = 0;
	MstrCntrl.serviceRTCISR = 0;

	MstrCntrl.ANECnt = 0;
	MstrCntrl.RTC_count = 0;
	MstrCntrl.pulseSkip = 0;

	MstrCntrl.FRAM_NumPoints = 0;
	MstrCntrl.FRAM_Idx = 0;

	MstrCntrl.Meas_Int = MEAS_INTERVAL;
	MstrCntrl.Server_Int = ServerTx_INTERVAL;
	MstrCntrl.Ane_Int = ANE_INTERVAL;

	//Set time if needed
	tmElements_t temp;
	breakTime(RTC.get(), temp);

	MstrCntrl.time = temp;
	MstrCntrl.t = RTC.get();
	MstrCntrl.prevtime = temp;

	wxSF.windSF = txWindSF;
	wxSF.gustSF = txGustSF;
	wxSF.tempSF = txTempSF;
	wxSF.pressSF = txPressSF;
	wxSF.humidSF = txHumidSF;
	wxSF.batteryVSF = txBattVSF;
	wxSF.batteryPSF = txBattPSF;
	wxSF.tempOff = txTempOff;
	wxSF.pressOff = txPressOff;
	wxSF.battOff = txWBattOff;

}

void EnableSleep()
{
	sleep_enable();
	interrupts();             // guarantees next instruction executed
	sleep_cpu();              // sleep within 3 clock cycles of above
}

void avgGustData()
{
	unsigned long temp = 0;
	for (int ii = 0; ii < PulseBuffSize; ii++) {
		temp = temp + MstrData.gustBuff[ii];
	}
	MstrData.gustAvg = temp >> 3;  //Divide by 8
}

void findRunningPeaks()
{
	if (MstrData.gustMax > MstrData.gustAvg) {
		MstrData.gustMax = MstrData.gustAvg;
	}
}

void RTC_SetAlarm_Interval(byte select)
{
	//MstrCntrl.t = RTC.get();
	tmElements_t temp;
	byte temp2;
	breakTime(RTC.get(), temp);
	MstrCntrl.time = temp;
	if (select == 1) {
		//setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate)
		RTC.setAlarm(ALM1_MATCH_SECONDS, (byte)(MstrCntrl.time.Second + MstrCntrl.Ane_Int), 0, 0, 1);    //daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_1, true);
	}
	else {
		RTC.setAlarm(ALM2_EVERY_MINUTE, 0, (byte)(MstrCntrl.time.Minute + MstrCntrl.Meas_Int),0, 1);    //daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_2, true);
	}
}


unsigned long floatLimit(float value, float scalefactor, int offset, float max, float min)
{

	float temp;

	temp = (value - (float)offset) * scalefactor;


	return (unsigned long)temp;
}

void setup_BME280()
{
	BME.settings.commInterface = I2C_MODE;
	BME.settings.I2CAddress = 0x76;

	BME.settings.runMode = 1;
	BME.settings.filter = 1;
	BME.settings.tempOverSample = 1;
	BME.settings.pressOverSample = 1;
	BME.settings.humidOverSample = 1;

	BME.begin();
	//delay();

}

void BME_Force()
{
	uint8_t value = BME.readRegister(BME280_CTRL_MEAS_REG);
	value = (value & 0xFC) + 0x01;
	BME.writeRegister(BME280_CTRL_MEAS_REG, value);

	delay(10);

}

/*----------------------------------*/
/*------------- I2C RX -------------*/
/*----------------------------------*/
/*void requestEvent()
{

TinyWireS.send(storeData.i2c_tx[0]);
TinyWireS.send(storeData.i2c_tx[1]);
TinyWireS.send(storeData.i2c_tx[2]);
TinyWireS.send(storeData.i2c_tx[3]);
TinyWireS.send(storeData.i2c_tx[4]);
TinyWireS.send(storeData.i2c_tx[0]);
TinyWireS.send(0xEE);

}
*/

void printDateTime(time_t t)
{
	Serial.print(monthShortStr(month(t)));
	Serial.print(" ");
	Serial.print(day(t));
	Serial.print(", ");
	Serial.print((year(t)));
	Serial.print(" ");
	Serial.print((hour(t)));
	Serial.print(":");
	Serial.print((minute(t)));
	Serial.print(":");
	Serial.println((second(t)));
}

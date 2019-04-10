// General.h

#ifndef _GENERAL_h
#define _GENERAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "UserParams.h"
#include "GSMFun.h"


#include "pinMapGSM.h"
extern storeDataUnion  MstrData;
extern BME280 BME;
extern MAX17048 pwr_mgmt;
extern Adafruit_FRAM_I2C fram;
extern nativeData sampledData;
extern volatile CntrlStruct MstrCntrl;
extern wxSFStruct  wxSF;
extern volatile pulseStruct pulseData;
extern volatile RTCStruct rtcTime;
extern char preJSONArray[200][(wxPackageSize+2)*2];
extern LaCrosse_TX23     anemometer;




void printHex(int num, int precision) {
	char tmp[16];
	char format[128];

	sprintf(format, "%%.%dX", precision);

	sprintf(tmp, format, num);
	Serial1.print(tmp);
}

void Anemometer_Enable()
{
	digitalWrite(ANE_Vcc_EN, false);
}

void Anemometer_Disable()
{
	digitalWrite(ANE_Vcc_EN, true);
}

void GSM_Enable()
{
	digitalWrite(GSM_RESETN, LOW);
}

void GSM_Disable()
{
	digitalWrite(GSM_RESETN, HIGH);
}

void GSM_PowerSave()
{
	SerialGSM.println("AT+UPSV");
	delay(50);
	//SerialGSM.println("AT+CPWROFF");
}

bool tcIsSyncing()
{
	return TC4->COUNT32.STATUS.reg & TC_STATUS_SYNCBUSY;
}

//disable TC5
void tcDisable()
{
	TC4->COUNT32.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

void tcReset()
{
	TC4->COUNT32.COUNT.bit.COUNT = (uint32_t)(0);
	while (tcIsSyncing());
}

//This function enables TC5 and waits for it to be ready
void tcStartCounter()
{
	TC4->COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
	while (tcIsSyncing()); //wait until snyc'd
}

void enterSleep()
{
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__WFI();
}

void BME_Force()
{
	uint8_t value = BME.readRegister(BME280_CTRL_MEAS_REG);
	value = (value & 0xFC) + 0x01;
	BME.writeRegister(BME280_CTRL_MEAS_REG, value);
	delay(10);
	sampledData.temp  = BME.readTempC();
	sampledData.press = BME.readFloatPressure()/100.0;
	sampledData.humid = BME.readFloatHumidity();

}

void getBatteryInfo()
{
	sampledData.batteryV = pwr_mgmt.voltage();
	delay(3);
	sampledData.batteryP = pwr_mgmt.accuratePercent();
	//pwr_mgmt.enSleep(true);
	
	
}

unsigned long floatLimit(float *value, float scalefactor, int offset, float max_in, float min_in)
{

	float temp;
	temp = MIN(*value, max_in);
	temp = MAX(temp, min_in);
	temp = (temp - (float)offset) * scalefactor;

	return (unsigned long)temp;
}

void findRunningPeaks()
{
	if (sampledData.gustMax > sampledData.gustAvg) {
		sampledData.gustMax = sampledData.gustAvg;
	}
}

void medGustData()
{
	
	 volatile uint32_t temp;
	 int i, j;
	 int n = 32;  // N must be even
	 volatile uint32_t *x[PULSEDT_BUFFLENGTH];
	 *x = pulseData.buffer; 
	 // the following two loops sort the array x in ascending order
	 for(i=0; i<n-1; i++) {
		 for(j=i+1; j<n; j++) {
			 if(x[j] < x[i]) {
				 // swap elements
				 temp = *x[i];
				 x[i] = x[j];
				 *x[j] = temp;
			 }
		 }
	 }

	sampledData.gustAvg = ((*x[n/2] + *x[n/2 - 1]) / 2);

	////unsigned long temp = 0;
	////for (int ii = 0; ii < PULSEDT_BUFFLENGTH; ii++) {
	//	temp = temp + pulseData.buffer[ii];
	//}
//	sampledData.gustAvg = temp >> 5;  //Divide by 32
}

void saveData()
{
	/*---------------------------------------------------------------------------------------*/
	MstrData.weather.speed    = (unsigned int)floatLimit(&sampledData.speed     , wxSF.windSF    , 0            , 114 , 0);
	MstrData.weather.dir      = 0xff & sampledData.dir;

	MstrData.weather.gustMax  = (unsigned int)floatLimit(&sampledData.gustMax   , wxSF.gustSF    , 0            , 2047, 0);

	MstrData.weather.temp     = (unsigned int)floatLimit(&sampledData.temp      , wxSF.tempSF    , wxSF.tempOff , 65  , -30);
	MstrData.weather.press    = (unsigned int)floatLimit(&sampledData.press     , wxSF.pressSF   , wxSF.pressOff, 1100, 900); //hPa
	MstrData.weather.humid    = (unsigned int)floatLimit(&sampledData.humid     , wxSF.humidSF   , 0            , 100 , 0);

	MstrData.weather.batteryV = (unsigned int)floatLimit(&sampledData.batteryV  , wxSF.batteryVSF, wxSF.battOff , 4.2 , 3);
	MstrData.weather.batteryP = (unsigned int)floatLimit(&sampledData.batteryP  , wxSF.batteryPSF, 0            , 100 , 0);
	Serial1.println("done Save");
}

void framSaveData()
{
	MstrCntrl.FRAM_Idx = fram.read8(FRAM_NUM_P0INTS_Addr);
	//	MstrCntrl.FRAM_Idx != (fram.read8(FRAM_NUM_P0INTS_Addr+1) << 8);
	Serial1.println("Mem pointer: " + String(MstrCntrl.FRAM_Idx));

	for (int ii = 0; ii <= wxPackageSize-1; ii++) {
		//Serial1.print("Address Write: ");
		//Serial1.println(MstrCntrl.FRAM_Idx*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET+ii,HEX);
		
		fram.write8(MstrCntrl.FRAM_Idx*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET +ii, MstrData.TxData[ii]);
		printHex(MstrData.TxData[ii],2);
		//Serial1.println("");
	}
	MstrCntrl.FRAM_Idx++;
	fram.write8(FRAM_NUM_P0INTS_Addr, MstrCntrl.FRAM_Idx & 0xFF);
	//fram.write8(FRAM_NUM_P0INTS_Addr+1, MstrCntrl.FRAM_Idx >> 8);
	Serial1.println("");
	Serial1.println("done Frame Save");
}

void framReadData()
{
	//Temp data
	char format[128];
	sprintf(format, "%%.%dX", 2);
	volatile int temp;
	char tempStrFull[wxPackageSize*2+4];
	char tempStr[2] = "\0";
	strcpy(tempStrFull, "\0");
	
	// Grab Index from memory
	MstrCntrl.FRAM_CurrIdx = fram.read8(FRAM_MEM_POINTER_Addr);
	MstrCntrl.FRAM_NumPoints = fram.read8(FRAM_NUM_P0INTS_Addr);
	
	//	MstrCntrl.FRAM_CurrIdx != (fram.read8(FRAM_MEM_POINTER_Addr+1) << 8);
	Serial1.println("Curr Mem pointer: " + String(MstrCntrl.FRAM_CurrIdx,HEX));
	Serial1.println("Num points pointer: " + String(MstrCntrl.FRAM_NumPoints,HEX));

	for (int kk = 0; kk < MstrCntrl.FRAM_NumPoints; kk++){
		for (int ii = 0; ii <= wxPackageSize-1; ii++) {
			//Serial1.print("Address Read: ");
			//Serial1.println(kk*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET + ii,HEX);
			
			temp = fram.read8(kk*FRAM_DATA_OFFSET + FRAM_CNTRL_OFFSET + ii);
			//Serial1.println(temp);
			sprintf(tempStr, format, temp);
			strcat(tempStrFull, tempStr);
			//Serial1.println(tempStrFull);
			//Serial1.println(tempStr);
		}
		strcat(tempStrFull, "\0");
		strcpy(preJSONArray[kk], tempStrFull);
	//	Serial1.println("full");
		//Serial1.println(tempStrFull);
		//Serial1.println(preJSONArray[kk]);
		tempStr[0] = 0;
		tempStrFull[0] = 0;
		//memset(tempStr, 0, 2);
		//memset(tempStrFull, 0, wxPackageSize*2+2);

	
	}
	
	// Add to code later-----------
	//MstrCntrl.FRAM_NumPoints = 0;
	//MstrCntrl.FRAM_CurrIdx = 0;
	//fram.write8(FRAM_NUM_P0INTS_Addr, MstrCntrl.FRAM_NumPoints & 0xFF);
	//fram.write8(FRAM_MEM_POINTER_Addr, MstrCntrl.FRAM_CurrIdx & 0xFF);
	//fram.write8(FRAM_NUM_P0INTS_Addr+1, MstrCntrl.FRAM_Idx >> 8);

}

void resetMaxGust()
{
	sampledData.gustMax = 30000;	
}

void collectData()
{
	int aneErr = 4;
	int failCount = 0;
	
	//Store Time
	MstrData.weather.year   = (byte)(rtcTime.currTime.year);
	MstrData.weather.month  = (byte)(rtcTime.currTime.month);
	MstrData.weather.day    = (uint8_t)(rtcTime.currTime.day);
	MstrData.weather.hour   = (uint8_t)(rtcTime.currTime.hour);
	MstrData.weather.minute = (uint8_t)(rtcTime.currTime.min);
	MstrData.weather.second = (uint8_t)(rtcTime.currTime.sec);
	
	/*-----Get temp data------*/
	Serial1.print("Got temps: ");
	BME_Force();
	Serial1.println(sampledData.temp);
	
	/*-----Check battery level------*/
	failCount = 0;
	sampledData.batteryV = 0.0F;
	while((sampledData.batteryV < 0.1) && (failCount < 100)){
		getBatteryInfo();
		failCount++;
		delay(10);
	}
	Serial1.println("Battery data ->");
	Serial1.println(sampledData.batteryV);
	Serial1.println(sampledData.batteryP);
	Serial1.print("failCount - ");
	Serial1.println(failCount);
	
	/* --- Get wind --- */
	//aneErr = anemometer.read(sampledData.speed, sampledData.dir);
	if (aneErr != 4 || (sampledData.speed > 100)){
		pulseData.RTCFlag = true;
		delay(2010);
		aneErr = anemometer.read(sampledData.speed, sampledData.dir);
		if (aneErr != 4)
		{
			sampledData.speed = 0;
			sampledData.dir   = 0;
		}
	}
	//Serial1.println("Wind info:" + String(sampledData.speed) + ", " + String(sampledData.dir) + ", " + String(sampledData.gustMax));
	//Serial1.print("manual wind: ");
	//Serial1.println(1.0F/(pulseData.countVal/32768.0F) * 60.0F/54.0F);
	
	Serial1.print(sampledData.speed);
	Serial1.print(" ");
	Serial1.println(pulseData.countVal);
	
	
	/* --- Store data --- */
	MstrData.weather.LowPwrMode = MstrCntrl.LP_Mode;
	MstrData.weather.accessCode = 23;
	
	Serial1.println("done Collect");
}

void resetWatchDog()
{
	digitalWrite(DONE, LOW);
	delay(100);
	digitalWrite(DONE, HIGH);
	
}


void printCurTime()
{
			Serial1.print(rtcTime.currTime.hour);
			Serial1.print(" Hour : ");
			Serial1.print(rtcTime.currTime.min);
			Serial1.print(" Min : ");
			Serial1.print(rtcTime.currTime.sec);
			Serial1.println(" Sec : ");
}

void LPModeCheck()
{
	if (sampledData.batteryP < 10){
			// Double Check
			if (sampledData.batteryV < 3.35){
			MstrCntrl.LP_Mode = 1;
			Serial1.println("-------------LP mode engaged");
			}
	}

}



#endif


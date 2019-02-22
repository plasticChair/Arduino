#pragma once



extern Adafruit_FRAM_I2C fram;
extern nativeData sampledData;
extern CntrlStruct MstrCntrl;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


#define FRAM_RESET_CNT_Addr  1
#define FRAM_MEM_POINTER_Addr  1
#define FRAM_NUM_P0INTS_Addr  3
#define FRAM_MEAS_INT_Addr 4
#define FRAM_LP_MODE_Addr  5
#define FRAM_DATA_OFFSET 0x20
#define FRAM_CNTRL_OFFSET 0x10

void battAlertISR()
{
	pwr_mgmt.clearAlert();

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


unsigned long floatLimit(float *value, float scalefactor, int offset, float max_in, float min_in)
{

	float temp;
	temp = MIN(*value, max_in);
	temp = MAX(temp, min_in);
	temp = (temp - (float)offset) * scalefactor;

	return (unsigned long)temp;
}


void BME_Force()
{
	uint8_t value = BME.readRegister(BME280_CTRL_MEAS_REG);
	value = (value & 0xFC) + 0x01;
	BME.writeRegister(BME280_CTRL_MEAS_REG, value);

	delay(10);

}


void printDateTime()
{
	time_t t = RTC.get();
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





void LP_mode_check() {

	if (sampledData.batteryV <= 3.2) {
		MstrCntrl.LP_Mode_Cnt++;
		if (MstrCntrl.LP_Mode == 0) {
			MstrCntrl.LP_Mode = 1;
			fram.write8(FRAM_LP_MODE_Addr, MstrCntrl.LP_Mode);
			MstrCntrl.Meas_Int_LP = MstrCntrl.Meas_Int;
			MstrCntrl.Meas_Int = MstrCntrl.Meas_Int * 2.0;
			MstrCntrl.Server_Int = MstrCntrl.Server_Int * 2;
		}

		if (((MstrCntrl.LP_Mode_Cnt % 8) == 0) & (MstrCntrl.LP_Mode == 1)) {
			MstrCntrl.Meas_Int = MAX(MstrCntrl.Meas_Int * 2.0, MEAS_INT_MAX);
			MstrCntrl.Server_Int = MAX(MstrCntrl.Server_Int * 2, SERVER_INT_MAX);

		}
		MstrCntrl.LP_Mode = 1;

	}
	else if (sampledData.batteryV >= 3.4) {
		if (MstrCntrl.LP_Mode == 1) {
			MstrCntrl.LP_Mode = 0;
			MstrCntrl.LP_Mode_Cnt = 0;
			fram.write8(FRAM_LP_MODE_Addr, MstrCntrl.LP_Mode);
			MstrCntrl.Meas_Int = MstrCntrl.Meas_Int_LP;
		}
		MstrCntrl.LP_Mode = 0;
	}

}

void sleep()
{
	noInterrupts();
	sleep_enable();
	EIFR = bit(INTF1);       // clear flag for interrupt 0
	//attachInterrupt(digitalPinToInterrupt(IOCntrl.ANE_Pulse), PulseISRRoutine, FALLING);
	interrupts();             // guarantees next instruction executed
	sleep_cpu();
}

void disable_timer2() 
{
	noInterrupts();
	TCCR2B = 0x00;
	interrupts();
}

void enable_timer2()
{
	noInterrupts();
	TCCR2B = 0x04;
	interrupts();
}

void GPIO_dance()
{

	digitalWrite(IOCntrl.LED1, HIGH);
	digitalWrite(IOCntrl.LED2, LOW);
	delay(100);
	digitalWrite(IOCntrl.LED1, LOW);
	digitalWrite(IOCntrl.LED2, HIGH);
	delay(100);
	digitalWrite(IOCntrl.LED1, HIGH);
	digitalWrite(IOCntrl.LED2, LOW);
	delay(100);
	digitalWrite(IOCntrl.LED1, LOW);
	digitalWrite(IOCntrl.LED2, HIGH);
	delay(100);
	digitalWrite(IOCntrl.LED1, HIGH);
	digitalWrite(IOCntrl.LED2, LOW);
	delay(100);
	digitalWrite(IOCntrl.LED1, LOW);
	digitalWrite(IOCntrl.LED2, HIGH);
	delay(100);
	digitalWrite(IOCntrl.LED1, HIGH);
	digitalWrite(IOCntrl.LED2, LOW);
	delay(100);
	digitalWrite(IOCntrl.LED1, LOW);
	digitalWrite(IOCntrl.LED2, HIGH);
	delay(100);
	digitalWrite(IOCntrl.LED1, LOW);
	digitalWrite(IOCntrl.LED2, LOW);


}


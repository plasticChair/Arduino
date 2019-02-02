#pragma once

/*------------------------------------------*/
/*------------- Init Functions -------------*/
/*------------------------------------------*/

extern void PulseISRRoutine();
extern void RTCISRRoutine();

extern MstrDataStruct MstrData;
extern storeDataUnion storeData;
extern BME280 BME;
extern MAX17048 pwr_mgm;
extern wxSFStruct wxSF;
extern Adafruit_FRAM_I2C fram;
extern MAX17048 pwr_mgmt;
extern volatile unsigned long Tmr2Cnt;
extern volatile unsigned long *Tmr2CntPtr;

extern CntrlStruct MstrCntrl;

//nativeData sampledData;
//IOStruct IOCntrl;

extern volatile PulseStruct MstrISR;

// Pin Definitions
#define mANE_TX 9
#define mANE_Pulse 3 //PD3
#define mANE_En 8
#define mRTC_Alarm 2
#define mCharge_EN 17
#define mLED1 PD6
#define mLED2 PD7
#define mWAKE 14
#define mDONE 16
#define mBATT_ALRT 4

#define MEAS_INTERVAL 0.2 //minutes
#define ServerTx_INTERVAL 15 //Minutes
#define ANE_INTERVAL 2 // Seconds
#define MEAS_INT_MAX 5
#define SERVER_INT_MAX 30

#define txWindSF 8.9737
#define txGustSF 0.144
#define txTempSF 10.7684
#define txPressSF 5.115
#define txHumidSF 10.23
#define txBattVSF 212.5
#define txBattPSF 2.55
#define txTempOff -30
#define txPressOff 900
#define txWBattOff 3

#define adc_disable() (ADCSRA &= ~(1<<ADEN))


extern IOStruct IOCntrl;


void init_GPIO()
{
	pinMode(IOCntrl.LED1, OUTPUT);
	pinMode(IOCntrl.LED2, OUTPUT);
	pinMode(IOCntrl.ANE_En, OUTPUT);


	pinMode(IOCntrl.BATT_ALRT, INPUT);
	pinMode(IOCntrl.ANE_TX, INPUT);
	pinMode(IOCntrl.ANE_Pulse, INPUT);
	pinMode(IOCntrl.RTC_Alarm, INPUT);

	digitalWrite(IOCntrl.ANE_En, HIGH);
	digitalWrite(IOCntrl.LED1, HIGH);
	digitalWrite(IOCntrl.LED2, HIGH);
	//delay(300000);
	////digitalWrite(ANE_En, HIGH);
	//delay(3000);
	//digitalWrite(ANE_En, LOW);
	EIFR = (1 << INTF0);
	EIFR = (1 << INTF1);
	attachInterrupt(digitalPinToInterrupt(IOCntrl.ANE_Pulse), PulseISRRoutine, FALLING);
	attachInterrupt(digitalPinToInterrupt(IOCntrl.RTC_Alarm), RTCISRRoutine, FALLING);

	//Enable Pin Change ISR
	*digitalPinToPCMSK(IOCntrl.BATT_ALRT) |= bit(digitalPinToPCMSKbit(IOCntrl.BATT_ALRT));  // enable pin
	PCIFR |= bit(digitalPinToPCICRbit(IOCntrl.BATT_ALRT)); // clear any outstanding interrupt
	//PCICR |= bit(digitalPinToPCICRbit(IOCntrl.BATT_ALRT)); // enable interrupt for the group
}

void init_vars()
{
	MstrISR.pulseDt = 0;
	MstrISR.pulseCt = 0;
	MstrISR.pulsePt = 0;
	MstrISR.servicePulseISR = 0;
	MstrISR.serviceRTCISR = 0;
	MstrISR.RTC_count = 0;
	MstrISR.pulseSkip = 1;
	MstrISR.Tmr2Cnt = 0;

	MstrData.gustAvg = 0;
	MstrData.weather.gustMax = 0;
	MstrData.weather.dir = 0;
	MstrData.weather.speed = 0.0;
	MstrData.gustMax = 7201;

	MstrCntrl.gustAvgCnt = 0;
	MstrCntrl.dataStoreCnt = 0;
	//MstrCntrl.servicePulseISR = 0;
	//MstrCntrl.serviceRTCISR   = 0;

	MstrCntrl.ANECnt = 0;
	MstrCntrl.RTC_count = 0;
	//MstrCntrl.pulseSkip       = 0;


	MstrCntrl.FRAM_NumPoints = 0;
	MstrCntrl.FRAM_Idx = 0;

	MstrCntrl.Meas_Int = MEAS_INTERVAL;
	MstrCntrl.Server_Int = ServerTx_INTERVAL;
	MstrCntrl.Ane_Int = ANE_INTERVAL;
	MstrCntrl.Meas_Int_LP = MEAS_INTERVAL;
	MstrCntrl.Server_Tx_En = 0;

	MstrCntrl.LP_Mode = 0;
	MstrCntrl.LP_Mode_Cnt = 0;

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


	IOCntrl.ANE_TX = mANE_TX;
	IOCntrl.ANE_Pulse = mANE_Pulse;
	IOCntrl.ANE_En = mANE_En;
	IOCntrl.RTC_Alarm = mRTC_Alarm;
	IOCntrl.Charge_EN = mCharge_EN;
	IOCntrl.LED1 = mLED1;
	IOCntrl.LED2 = mLED2;
	IOCntrl.WAKE = mWAKE;
	IOCntrl.DONE = mDONE;
	IOCntrl.BATT_ALRT = mBATT_ALRT;



	Tmr2CntPtr = &Tmr2Cnt;
}


void setup_BME280()
{
	BME.settings.I2CAddress = 0x76;

	BME.settings.runMode = 1;
	BME.settings.filter = 1;
	BME.settings.tempOverSample = 1;
	BME.settings.pressOverSample = 1;
	BME.settings.humidOverSample = 1;

	BME.begin();
	//delay();

}



void setup_fram()
{
	fram.begin();
	// Read the first byte
	uint8_t test = fram.read8(0x0);
	// Test write ++
	fram.write8(0x0, test + 1);

	Serial.print("Fram test = ");
	Serial.println(test);
}

void setup_battMang()
{
	pwr_mgmt.attatch(Wire);
	pwr_mgmt.quickStart();
	pwr_mgmt.vAlertMinThreshold(3.1);
	pwr_mgmt.enSleep(true);

}


void setup_pwrSave()
{
	// Disable ADC
	adc_disable();
	PRR |= _BV(PRADC);

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above

	power_adc_disable(); // ADC converter
	power_spi_disable(); // SPI
	//power_timer0_disable();// Timer 0
	power_timer1_disable();// Timer 1
	//power_timer2_disable();// Timer 2

	//power_usart0_disable();
	//power_twi_disable();
	wdt_disable();  // disable watchdog
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
}

void setup_timer2()
{
	noInterrupts();

	//Setup Timer2 to fire every 62.5ns
	TCCR2B = 0x00;        //Disbale Timer2 while we set it up
	TCNT2 = 255;         //Reset Timer Count to 255 out of 255
	TIFR2 = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
	TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
	TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
	//ASSR |= bit(EXCLK);

	TCCR2B = 0x04;        //Timer2 Control Reg B: Timer Prescaler set to 64

	//TCCR2B = 0x00;
	interrupts();


}




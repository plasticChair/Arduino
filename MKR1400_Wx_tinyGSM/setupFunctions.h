// setupFunctions.h

#ifndef _SETUPFUNCTIONS_h
#define _SETUPFUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "pinMapGSM.h"
#include "Wire.h"


extern storeDataUnion  MstrData;
extern wxSFStruct  wxSF;
extern BME280 BME;
extern MAX17048 pwr_mgmt;
extern Adafruit_FRAM_I2C fram;
extern serverStruct serverData;
extern volatile pulseStruct pulseData;

extern void lowPowerISR();
extern void watchdogISR();


/*
void GPIOSetup();
void GPIODefaults();
void GPIO_dance();

void setup_BME280();
void setup_battMang();
void setup_fram();
*/

void GPIOSetup()
{
	Serial1.begin(115200);
	while (!Serial1);
	SerialGSM.begin(38400);
	while (!SerialGSM);
		
	pinMode(GPLED1, OUTPUT);
	pinMode(GPLED2, OUTPUT);
	pinMode(ANE_Vcc_EN, OUTPUT);
	pinMode(ANE_TX, INPUT);
	pinMode(ANE_PULSE, INPUT);
	pinMode(DONE, OUTPUT);
	pinMode(WAKE, INPUT);
	pinMode(BATT_ALRT, INPUT);
	pinMode(GSM_RESETN, OUTPUT);
	
	//attachInterrupt(digitalPinToInterrupt(BATT_ALRT), lowPowerISR, FALLING);
	
}

void GPIODefaults()
{
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_OFF);
    digitalWrite(GSM_RESETN, HIGH);
	digitalWrite(ANE_Vcc_EN, true);
	digitalWrite(DONE, LOW);
}


void GPIO_dance()
{

	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_ON);
	delay(100);
	digitalWrite(GPLED1, LED_ON);
	digitalWrite(GPLED2, LED_OFF);
	delay(100);
	digitalWrite(GPLED1, LED_OFF);
	digitalWrite(GPLED2, LED_OFF);


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
	MstrCntrl.FRAM_reset_count = fram.read8(0x0)+1;
	// Test write ++
	fram.write8(0x0, (MstrCntrl.FRAM_reset_count) % 255);
	
	Serial1.print("Fram test = ");
	Serial1.println(MstrCntrl.FRAM_reset_count);
}

void setup_battMang()
{
	pwr_mgmt.attatch(Wire);
	pwr_mgmt.quickStart();
	//pwr_mgmt.vAlertMinThreshold(LOWPOWER_LIMIT);
	//pwr_mgmt.enSleep(true);

}

void powerSave()
{
	SYSCTRL->OSC32K.bit.ENABLE = 0;
	SYSCTRL->XOSC32K.bit.ENABLE = 0;

	SYSCTRL->OSC32K.bit.RUNSTDBY = 0;
	SYSCTRL->BOD33.bit.ENABLE = 0;
	SYSCTRL->BOD33.bit.RUNSTDBY = 0;

	SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;
	SYSCTRL->XOSC.bit.RUNSTDBY = 0;
	
	
	
	//Disable clocks
	//PM->APBBMASK.reg &= ~PM_APBBMASK_PORT;  // needed for int
	//PM->APBBMASK.reg &= ~PM_APBBMASK_DMAC;
	//PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM0;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM1;
	//	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM2;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM3;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM4;
	//	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM5;
	PM->APBCMASK.reg &= ~PM_APBCMASK_TCC0;
	PM->APBCMASK.reg &= ~PM_APBCMASK_TCC1;
	PM->APBCMASK.reg &= ~PM_APBCMASK_TCC2;
	PM->APBCMASK.reg &= ~PM_APBCMASK_TC6;
	PM->APBCMASK.reg &= ~PM_APBCMASK_TC7;
	//PM->APBCMASK.reg &= ~CLK_TC4_APB;
	//PM->APBCMASK.reg &= ~PM_APBCMASK_TC5;
	PM->APBCMASK.reg &= ~PM_APBCMASK_ADC;
	PM->APBCMASK.reg &= ~PM_APBCMASK_DAC;
	PM->APBCMASK.reg &= ~PM_APBCMASK_I2S;
	PM->APBCMASK.reg &= ~PM_APBCMASK_PTC;
	PM->APBCMASK.reg &= ~PM_APBCMASK_AC;
	PM->APBCMASK.reg &= ~PM_APBCMASK_PAC2;


	SYSCTRL->XOSC32K.bit.RUNSTDBY=0;
	USB->HOST.CTRLA.bit.RUNSTDBY=0;
	SYSCTRL->OSC32K.bit.RUNSTDBY=0;
	SYSCTRL->OSC8M.bit.RUNSTDBY=0;
	SYSCTRL->DFLLCTRL.bit.RUNSTDBY=0;
	SYSCTRL->DPLLCTRLA.bit.RUNSTDBY=0;
	SYSCTRL->BOD33.bit.RUNSTDBY=0;
	// SYSCTRL->VREG.bit.RUNSTDBY=0;
	//GCLK->GENCTRL.bit.RUNSTDBY=0;
	// SYSCTRL->VREG.bit.RUNSTDBY=0;
	TCC0->CTRLA.bit.RUNSTDBY=0;
	SERCOM0->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM1->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM2->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM3->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM4->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM5->USART.CTRLA.bit.RUNSTDBY=0;
	SERCOM0->SPI.CTRLA.bit.RUNSTDBY=0;
	SERCOM1->SPI.CTRLA.bit.RUNSTDBY=0;
	SERCOM2->SPI.CTRLA.bit.RUNSTDBY=0;
	SERCOM3->SPI.CTRLA.bit.RUNSTDBY=0;
	SERCOM4->SPI.CTRLA.bit.RUNSTDBY=0;
	SERCOM5->SPI.CTRLA.bit.RUNSTDBY=0;
	ADC->CTRLA.bit.RUNSTDBY=0;
	DAC->CTRLA.bit.RUNSTDBY=0;
	AC->CTRLA.bit.RUNSTDBY=0;
	TCC0->CTRLA.bit.RUNSTDBY=0;
	TCC1->CTRLA.bit.RUNSTDBY=0;
	TCC2->CTRLA.bit.RUNSTDBY=0;
	USB->HOST.CTRLA.bit.RUNSTDBY=0;
	USB->DEVICE.CTRLA.bit.RUNSTDBY=0;
	


	SYSCTRL->VREG.bit.RUNSTDBY = 1;
	SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;


	PM->APBAMASK.reg |= PM_APBAMASK_RTC;
	
	//AHBMASK:  CLK_HPBA_AHB CLK_HPBB_AHB CLK_HPBC_AHB CLK_DSU_AHB CLK_NVMCTRL_AHB CLK_DMAC_AHB CLK_USB_AHB
	//APBAMASK:  CLK_PAC0_APB CLK_PM_APB CLK_SYSCTRL_APB CLK_GCLK_APB CLK_WDT_APB CLK_RTC_APB CLK_EIC_APB
	//APBBMASK:  CLK_PAC1_APB CLK_DSU_APB CLK_NVMCTRL_APB CLK_PORT_APB CLK_DMAC_APB CLK_USB_APB
	//APBCMASK:  CLK_SERCOM0_APB CLK_SERCOM1_APB CLK_SERCOM2_APB CLK_SERCOM3_APB CLK_SERCOM4_APB CLK_SERCOM5_APB CLK_TCC0_APB CLK_TCC1_APB CLK_TCC2_APB CLK_TC3_APB CLK_TC4_APB CLK_TC5_APB CLK_ADC_APB CLK_DAC_APB

}


void timerSetup()
{
	// Gen Clock setup
	GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(4) | GCLK_GENDIV_DIV(1));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(4));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	// Gen Clock setup
	GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(5) | GCLK_GENDIV_DIV(1));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(5));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);

	//////////////////////////////
	//TC3 Setup
	//Enable CLK_TCx_APB
	PM->APBCMASK.reg |= PM_APBCMASK_TC4;
	PM->APBCMASK.reg |= PM_APBCMASK_TC5;


	TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (TC4->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	while (TC4->COUNT16.CTRLA.bit.SWRST);
	TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;    // Disable TC3
	
	//Setup here
	TC4->COUNT32.CTRLA.reg = (uint32_t) (TC_CTRLA_RUNSTDBY | TC_CTRLA_MODE_COUNT32);
	while (tcIsSyncing());
	TC4->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1  |TC_CTRLA_ENABLE;
	//TC4->COUNT32.CC[0].reg = (uint32_t) (SystemCoreClock / 10 - 1);
	while (tcIsSyncing());
	

	// Configure interrupt request
	//	NVIC_DisableIRQ(TC4_IRQn);
	//	NVIC_ClearPendingIRQ(TC4_IRQn);
	//NVIC_SetPriority(TC4_IRQn, 0);
	//NVIC_EnableIRQ(TC4_IRQn);
	
	
	TC4->COUNT32.INTENSET.bit.MC0 = 1;
	while (tcIsSyncing()); //wait until TC5 is done syncing
	
	
	
	
	
	
	
	/*   TC3 */
	
	// Gen Clock setup
	GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(1));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(3));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID(GCM_TCC2_TC3)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);

	//////////////////////////////
	//TC3 Setup
	//Enable CLK_TCx_APB
	PM->APBCMASK.reg |= PM_APBCMASK_TC3;



	TC3->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	while (TC3->COUNT16.CTRLA.bit.SWRST);
	TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;    // Disable TC3
	
	//Setup here
	TC3->COUNT16.CTRLA.reg = (uint32_t) (TC_CTRLA_RUNSTDBY | TC_CTRLA_MODE_COUNT16);
	while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1  |TC_CTRLA_ENABLE;
	//TC4->COUNT32.CC[0].reg = (uint32_t) (SystemCoreClock / 10 - 1);
	while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	
	TC3->COUNT16.EVCTRL.reg = (uint32_t) (TC_EVCTRL_OVFEO);
	while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	
		TC3->COUNT16.COUNT.reg = (uint32_t) (2000);
		while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
	
	

	// Configure interrupt request
	NVIC_DisableIRQ(TC3_IRQn);
	NVIC_ClearPendingIRQ(TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 0);
	NVIC_EnableIRQ(TC3_IRQn);
	
	
	TC3->COUNT16.INTENSET.bit.MC0 = 1;
	while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY); //wait until TC5 is done syncing
	
}

void EIC_Setup()
{
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;  // Enable power
	
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_EIC)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);

	EIC->WAKEUP.reg = EIC_WAKEUP_WAKEUPEN2 | EIC_WAKEUP_WAKEUPEN10;	//Force wakeup for A1 and 4
}

void init_vars()
{
	MstrData.weather.gustMax  = 0;
	MstrData.weather.dir      = 0;
	MstrData.weather.speed    = 0.0;
	
	MstrData.weather.year     = 0;
	MstrData.weather.month    = 0;
	MstrData.weather.day      = 0;
	MstrData.weather.hour     = 0;
	MstrData.weather.minute   = 0;
	MstrData.weather.second   = 0;
	MstrData.weather.temp     = 0;
	MstrData.weather.press    = 0;
	MstrData.weather.humid    = 0;
	MstrData.weather.batteryP = 0;
	MstrData.weather.batteryV = 0;
	MstrData.weather.id       = WXSTATION_ID;
	
	sampledData.dir      = 0;
	sampledData.speed    = 0;
	sampledData.gustMax  = 30000;// cHANGE me to 30k
	sampledData.gustAvg  = 0;
	sampledData.temp     = 0;
	sampledData.press    = 0;
	sampledData.humid    = 0;
	sampledData.batteryP = 90.0;
	sampledData.batteryV = 4.0;
	sampledData.rssi     = 0;
	
	
	wxSF.windSF     = txWindSF;
	wxSF.gustSF     = txGustSF;
	wxSF.tempSF     = txTempSF;
	wxSF.pressSF    = txPressSF;
	wxSF.humidSF    = txHumidSF;
	wxSF.batteryVSF = txBattVSF;
	wxSF.batteryPSF = txBattPSF;
	wxSF.tempOff    = txTempOff;
	wxSF.pressOff   = txPressOff;
	wxSF.battOff    = txWBattOff;
	
	pulseData.countVal     = 0;
	pulseData.buffIdx      = 0;
	pulseData.buffFullFlag = false;
	pulseData.RTCFlag      = false;
	

	MstrCntrl.measInterval   = MEAS_INTERVAL;
	MstrCntrl.serverInterval = ServerTx_INTERVAL;
	
	MstrCntrl.RTCUpdateFlag = 0;
	MstrCntrl.WDFlag = false;
	
	MstrCntrl.sleepModeEn  = false;
	MstrCntrl.sleepModeSet = false;
	MstrCntrl.sleepSingle  = false;
	MstrCntrl.sleepHour    = SLEEP_HOUR;
	MstrCntrl.wakeupHour   = WAKE_HOUR;
	
	MstrCntrl.FRAM_Idx         = 0;
	MstrCntrl.FRAM_NumPoints   = 0;
	MstrCntrl.FRAM_CurrIdx     = 0;
	MstrCntrl.FRAM_reset_count = 0;
	MstrCntrl.jsonSize         = 0;
	
	MstrCntrl.sCmd_serverTxInt = 0;
	MstrCntrl.sCmd_reboot = 0;
	MstrCntrl.sCmd_Unlock = 7;
	MstrCntrl.sCmd_t_hours = 0;
	MstrCntrl.sCmd_t_mins = 0;
	
	serverData.portIP           = PORT_IP;
	serverData.portServer       = PORT_SERVER;
	serverData.serverConnection = false;
	strcpy(serverData.server,"scottn.ddns.net\0");
	strcpy(serverData.serverIP,"68.230.107.205\0");
	strcpy(serverData.path,"/formParse2SQLDB.php\0");

	for (int ii = 0; ii< PULSEDT_BUFFLENGTH; ii++){
		pulseData.buffer[ii] = 0;
	}	


}

#endif


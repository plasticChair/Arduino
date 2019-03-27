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

#include "GlobalTypes.h"
#include <MKRGSM.h>
#include "SAMD21_shieldtest.h"

GSMClient client;
GPRS gprs;
GSM gsmAccess;

#include "Arduino.h"
// Define Function Prototypes that use User Types below here or use a .h file
//

volatile int flag = 0;
volatile int state = LOW;
volatile int printFlag = 0;
volatile unsigned long timeNow = 0;
volatile unsigned long timeLast = 0;
volatile unsigned long timeDiff = 0;

volatile bool update = 0;
volatile uint32_t countVal = 0;
uint32_t sampleRate = 2;
// Define Functions below here or use other .ino or cpp files
//
bool tcIsSyncing();
void tcDisable();
void tcStartCounter();
void pulseISR();

void enterSleep();
//#define SerialGSM Serial2


// The setup() function runs once each time the micro-controller starts
unsigned long baud = 115200;
void setup()
{

	Serial1.begin(115200);
	while (!Serial1);
  SerialGSM.begin(baud);
  while (!SerialGSM);
	GPIOSetup();
	GPIODefaults();
	GPIO_dance();
	Serial1.println("Starting...");
	Anemometer_Enable();



pinMode(GSM_RESETN, OUTPUT);
digitalWrite(GSM_RESETN, HIGH);
delay(100);
//digitalWrite(GSM_RESETN, LOW);

delay(100);
SerialGSM.println("AT+UPSV");
delay(100);
SerialGSM.println("AT+CPWROFF");


	digitalWrite(GPLED2, LED_OFF);
	delay(100);





//	SYSCTRL->OSC32K.bit.ENABLE = 0;
	//SYSCTRL->XOSC32K.bit.ENABLE = 0;

	SYSCTRL->OSC32K.bit.RUNSTDBY = 0;
	SYSCTRL->BOD33.bit.ENABLE = 0;
	SYSCTRL->BOD33.bit.RUNSTDBY = 0;

	SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;
	SYSCTRL->XOSC.bit.RUNSTDBY = 0;
	
	
	
	//Disable clocks
	//PM->APBBMASK.reg &= ~PM_APBBMASK_PORT;  // needed for int
	PM->APBBMASK.reg &= ~PM_APBBMASK_DMAC;
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
 


	//AHBMASK:  CLK_HPBA_AHB CLK_HPBB_AHB CLK_HPBC_AHB CLK_DSU_AHB CLK_NVMCTRL_AHB CLK_DMAC_AHB CLK_USB_AHB
	//APBAMASK:  CLK_PAC0_APB CLK_PM_APB CLK_SYSCTRL_APB CLK_GCLK_APB CLK_WDT_APB CLK_RTC_APB CLK_EIC_APB
	//APBBMASK:  CLK_PAC1_APB CLK_DSU_APB CLK_NVMCTRL_APB CLK_PORT_APB CLK_DMAC_APB CLK_USB_APB
	//APBCMASK:  CLK_SERCOM0_APB CLK_SERCOM1_APB CLK_SERCOM2_APB CLK_SERCOM3_APB CLK_SERCOM4_APB CLK_SERCOM5_APB CLK_TCC0_APB CLK_TCC1_APB CLK_TCC2_APB CLK_TC3_APB CLK_TC4_APB CLK_TC5_APB CLK_ADC_APB CLK_DAC_APB




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
		
		GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;  //set up for generic clock gen 4 and using TC 4
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
	TC4->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1  |TC_CTRLA_ENABLE;    // Enable TC3
	TC4->COUNT32.CC[0].reg = (uint32_t) (SystemCoreClock / sampleRate - 1);
	while (tcIsSyncing());
	

	// Configure interrupt request
//	NVIC_DisableIRQ(TC4_IRQn);
//	NVIC_ClearPendingIRQ(TC4_IRQn);
	//NVIC_SetPriority(TC4_IRQn, 0);
	//NVIC_EnableIRQ(TC4_IRQn);
	
	
	TC4->COUNT32.INTENSET.bit.MC0 = 1;
	while (tcIsSyncing()); //wait until TC5 is done syncing
	
	

	 SYSCTRL->VREG.bit.RUNSTDBY = 1;
	 SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;
	// USBDevice.detach();
	attachInterrupt(digitalPinToInterrupt(ANE_PULSE), pulseISR, FALLING);
	
	
	
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;
	

		
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_EIC)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);

	 EIC->WAKEUP.reg = EIC_WAKEUP_WAKEUPEN2;
	 
	 
	digitalWrite(GPLED2, LED_OFF);


	
	Serial1.println("Entering Loop...");
	delay(100);

	digitalWrite(GPLED2, LED_ON);
	enterSleep();
//
}

void pulseISR()
{
	detachInterrupt(digitalPinToInterrupt(A1));
	//EIC->EVCTRL.bit.EXTINTEO2 = 0;
	countVal = TC4->COUNT32.COUNT.bit.COUNT;
	if (countVal >1000){
		state = !state;
		update = true;
		TC4->COUNT32.COUNT.bit.COUNT = (uint32_t) (0);
		while(tcIsSyncing());
		
	}
	//EIC->EVCTRL.bit.EXTINTEO2 = 1;

	attachInterrupt(digitalPinToInterrupt(A1), pulseISR, FALLING);
}



// Add the main program code into the continuous loop() function
void loop()
{
	//Serial1.begin(9600);

	//Serial1.print("Got woke");
	//delay(500);
	if (printFlag)
	{
		timeLast = micros();
		timeDiff = timeLast - timeNow;
		timeNow = timeLast;
		Serial1.println(timeDiff);
		printFlag = 0;
		digitalWrite(GPLED2, state);
		delay(10);

	}
	//	rtc.standbyMode();
	if (update)
	{
		//Serial1.println((float)countVal / 32768.0F);
		Serial1.println(countVal);
		update = false;
		delay(10);
		digitalWrite(A5,state);

	}
	
	Serial1.println("Going to sleep");
	delay(10);
	enterSleep();


	


}

void printWind()
{
	float tempSpeed = 0.0;
	int tempDir = 0;
	int error = 0;

//	GPIO_dance();
//	Serial1.begin(9600);
//	Serial.begin(115200);
//	while (!SerialUSB);

	digitalWrite(GPLED2, LED_ON);
	rtcTime.alarmTime.sec = rtc.getSeconds()  +3;
	rtcSetAlarm();
	rtcEnable();
	//error = anemometer.read(tempSpeed, tempDir);
	//Serial1.print("speed is:");
	//Serial1.println(tempSpeed);
	//Serial1.print("dir is:");
//	Serial1.println(tempDir);
//	Serial1.print("error is:");
//	Serial1.println(error);
	//delay(2000);
}



//this function gets called by the interrupt at <sampleRate>Hertz
void TC4_Handler (void) {
	//YOUR CODE HERE
	
	// END OF YOUR CODE
	TC4->COUNT32.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
}


//disable TC5
void tcDisable()
{
	TC4->COUNT32.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

bool tcIsSyncing()
{
	return TC4->COUNT32.STATUS.reg & TC_STATUS_SYNCBUSY;
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
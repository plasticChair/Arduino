// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       SAMD21_Timers.ino
    Created:	3/19/2019 11:22:54 PM
    Author:     DESKTOP-4LJLPCB\mosta
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//
#include "Arduino.h"
#include "RTCLib.h"
#include <RTCZero.h>
RTCZero                  rtc;
// Define User Types below here or use a .h file
//

//#include <RTCZero.h>
//#include <LowPower.h>

// Define Function Prototypes that use User Types below here or use a .h file
//

volatile int flag = 0;
//volatile int state = LOW;
volatile int printFlag = 0;
volatile unsigned long timeNow = 0;
volatile unsigned long timeLast = 0;
volatile unsigned long timeDiff = 0;
// Define Functions below here or use other .ino or cpp files
//

#define LED_PIN 13 //just for an example
volatile bool state = 0; //just for an example
volatile bool update = 0;
volatile uint32_t countVal = 0;
bool tcIsSyncing();
void tcDisable();
void tcStartCounter();
void pulseISR();

uint32_t sampleRate = 2;

// The setup() function runs once each time the micro-controller starts
unsigned long baud = 115200;
void setup()
{

	Serial1.begin(115200);
	while (!Serial1);
	Serial1.println("Starting");

	pinMode(LED_PIN,OUTPUT); 
	pinMode(A0,INPUT_PULLUP); 	
//	SCB->SCR.SLEEPDEEP=1;



	SYSCTRL->OSC32K.bit.ENABLE = 0;
	SYSCTRL->XOSC32K.bit.ENABLE = 0;
	SYSCTRL->OSC32K.bit.RUNSTDBY = 0;
	SYSCTRL->BOD33.bit.ENABLE = 0;
	SYSCTRL->BOD33.bit.RUNSTDBY = 0;

	SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;
	SYSCTRL->XOSC.bit.RUNSTDBY = 0;
	
	
	
	//Disable clocks
	//PM->APBBMASK.reg &= ~PM_APBBMASK_PORT;  // needed for int
	PM->APBBMASK.reg &= ~PM_APBBMASK_DMAC;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM1;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM2;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM3;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM4;
	PM->APBCMASK.reg &= ~PM_APBCMASK_SERCOM5;
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



	//AHBMASK:  CLK_HPBA_AHB CLK_HPBB_AHB CLK_HPBC_AHB CLK_DSU_AHB CLK_NVMCTRL_AHB CLK_DMAC_AHB CLK_USB_AHB
	//APBAMASK:  CLK_PAC0_APB CLK_PM_APB CLK_SYSCTRL_APB CLK_GCLK_APB CLK_WDT_APB CLK_RTC_APB CLK_EIC_APB
	//APBBMASK:  CLK_PAC1_APB CLK_DSU_APB CLK_NVMCTRL_APB CLK_PORT_APB CLK_DMAC_APB CLK_USB_APB
	//APBCMASK:  CLK_SERCOM0_APB CLK_SERCOM1_APB CLK_SERCOM2_APB CLK_SERCOM3_APB CLK_SERCOM4_APB CLK_SERCOM5_APB CLK_TCC0_APB CLK_TCC1_APB CLK_TCC2_APB CLK_TC3_APB CLK_TC4_APB CLK_TC5_APB CLK_ADC_APB CLK_DAC_APB




	// Gen Clock setup
	GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(4) );
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(4));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;  //set up for generic clock gen 4 and using TC 4
	while (GCLK->STATUS.bit.SYNCBUSY);
	
		// Gen Clock setup
/*		GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(5) | GCLK_GENDIV_DIV(1));
		while (GCLK->STATUS.bit.SYNCBUSY);
		
		GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(5));
		while (GCLK->STATUS.bit.SYNCBUSY);
	*/	
		GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;  //set up for generic clock gen 4 and using TC 4
		while (GCLK->STATUS.bit.SYNCBUSY);

	//////////////////////////////
	//TC3 Setup
	//Enable CLK_TCx_APB
//	PM->APBCMASK.reg |= PM_APBCMASK_TC4;
//	PM->APBCMASK.reg |= PM_APBCMASK_TC5;


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
	
	
	
	
	


	//GCLK->GENDIV.reg = (GCLK_GENDIV_ID(4) | GCLK_GENDIV_DIV(0));
	//while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
	//	;                                                         /*XOSC32K*/
	//GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(4));
	//GCLK->GENCTRL.bit.DIVSEL = 0;
	//while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
	//	;
	////GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
	//GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4));
	//GCLK->CLKCTRL.bit.ID = 0x1A;
	//while (GCLK->STATUS.bit.SYNCBUSY)
	//	;





//	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_EIC) |
//		GCLK_CLKCTRL_GEN_GCLK1 |
//		GCLK_CLKCTRL_CLKEN;


//	GCLK->CLKCTRL.bit.ID = 5;

//	EIC->NMICTRL.bit.NMISENSE = 2;
//	EIC->NMICTRL.bit.NMIFILTEN = 1;
//	EIC->EVCTRL.bit.EXTINTEO0 = 0;
//	EIC->WAKEUP.bit.WAKEUPEN0 = 1;
//	EIC->CTRL.bit.ENABLE = 1;




attachInterrupt(digitalPinToInterrupt(A0), pulseISR, FALLING);




 SYSCTRL->VREG.bit.RUNSTDBY = 1;
 //SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 1;
  USBDevice.detach();




// Gen Clock setup
GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_EIC)) ;  //set up for generic clock gen 4 and using TC 4
while (GCLK->STATUS.bit.SYNCBUSY);



		Serial1.println("Done Setup");
		delay(10);
		rtc.standbyMode();
}

void pulseISR()
{
	detachInterrupt(digitalPinToInterrupt(A0));
	//EIC->EVCTRL.bit.EXTINTEO2 = 0;
	countVal = TC4->COUNT32.COUNT.bit.COUNT;
	if (countVal >32000){
		state = !state;
		update = true;
		TC4->COUNT32.COUNT.bit.COUNT = (uint32_t) (0);
		while(tcIsSyncing());
			
	}
//EIC->EVCTRL.bit.EXTINTEO2 = 1;

attachInterrupt(digitalPinToInterrupt(A0), pulseISR, FALLING);
}
// Add the main program code into the continuous loop() function
void loop()
{
	//Serial1.begin(9600);


//	rtc.standbyMode();
if (update)
{
	//Serial1.println((float)countVal / 32768.0F);
	Serial1.println(countVal);
	update = false;
	delay(10);
	digitalWrite(LED_PIN,state);

}
	rtc.standbyMode();
	


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
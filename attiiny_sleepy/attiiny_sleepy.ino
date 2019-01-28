#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include <TinyWireS.h>
#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
#include "FRAM_tinywire.h"
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#include "LaCrosse_TX23.h"
#include <avr/sleep.h>
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

volatile uint8_t i2c_regs[] =
{
	0xDE,
	0xAD,
	0xBE,
	0xEF,
};



/*nion weatherDataStruct{
  byte byteData[4];
  int dir;
  int error;

} weatherData;
*/


typedef struct DataStruct {
	int dir;
	float speed;
	int error;
};

union {
	DataStruct weather;
	byte byteData[8];
} weatherData;


//volatile byte reg_position;
//const byte reg_size = sizeof(i2c_regs);

bool rawData[50];

volatile byte reg_position = 0;
const byte reg_size = sizeof(weatherData);


void requestEvent()
{
	// cancel sleep as a precaution
  //sleep_disable();
  // precautionary while we do other stuff
	PCMSK &= ~_BV(PCINT3);  //disbale
	//USICR &= ~_BV(USISIE); //USI INt Enable
	//USISR |= _BV(USISIF);


	TinyWireS.send(weatherData.byteData[0]);
	TinyWireS.send(weatherData.byteData[1]);
	TinyWireS.send(weatherData.byteData[2]);
	TinyWireS.send(weatherData.byteData[3]);
	TinyWireS.send(weatherData.byteData[4]);
	TinyWireS.send(weatherData.byteData[5]);
	TinyWireS.send(weatherData.byteData[6]);
	TinyWireS.send(weatherData.byteData[7]);

	TinyWireS.send(rawData[0]);
	TinyWireS.send(rawData[1]);
	TinyWireS.send(rawData[2]);
	TinyWireS.send(rawData[3]);
	TinyWireS.send(rawData[4]);

	/* TinyWireS.send(rawData[5]);
	 TinyWireS.send(rawData[6]);
	 TinyWireS.send(rawData[7]);
	 TinyWireS.send(rawData[8]);
	 TinyWireS.send(rawData[9]);
	 TinyWireS.send(rawData[10]);
	 TinyWireS.send(rawData[11]);
	 TinyWireS.send(rawData[12]);
	 TinyWireS.send(rawData[13]);

	 for (int ii = 0; ii<=15; ii++){
		 TinyWireS.send((byte)rawData[ii]);
	 }
	 for (int ii = 16; ii<=50; ii++){
		 TinyWireS.send((byte)rawData[ii]);
	 }
 */

	weatherData.weather.dir++;
	weatherData.weather.error++;//error;
	weatherData.weather.speed++;//weatherData.weather.speed +5.0;//speedData;

	delay(10);
	PCMSK |= _BV(PCINT3); //enable INT on PCINT3
	//USICR |= _BV(USISIE); //USI INt Enable


  //attachInterrupt(digitalPinToInterrupt(7), I2CEnable, FALLING);

	sleep_enable();
	sleep_cpu();

	
	
}



/*void I2CEnable()
{
  detachInterrupt(digitalPinToInterrupt(7));
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onRequest(requestEvent);
}
*/
unsigned long prevTime = 0;
volatile unsigned long curWindT = 0;
volatile int bufferData[25];
volatile int bufferIdx = 0;
float speedData = 0.0;
int windData = 25;

byte own_address = 10;
FRAM_TINYWIRE fram     = FRAM_TINYWIRE();

byte TxFlag = 0;
//DATA wire connected to arduino port 10
LaCrosse_TX23 anemometer = LaCrosse_TX23(3);


// the setup routine runs once when you press reset:
void setup() {

    fram.begin();
	// initialize the digital pin as an output.

	//attachInterrupt(1,pulseISR,RISING);
	/*GIMSK |= _BV(INT0);
	SREG |= _BV(SREG_I); // enable ext INT

	MCUCR |= _BV(ISC01);
	MCUCR &= ~_BV(ISC00); // Falling edge

	PCMSK |= _BV(PCINT1); //enable INT on PCINT
	*/

	
	
	TinyWireS.begin(I2C_SLAVE_ADDRESS);
	TinyWireS.onRequest(requestEvent);

	//initTimer1();
	//sei();
	weatherData.weather.dir = 44;
	weatherData.weather.error = 88;
	

	//GIMSK |= _BV(PCIE);
	//PCMSK |= _BV(PCINT3);
	  
	  //USICR |= _BV(USISIE); //USI INt Enable


	  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	  sleep_enable();

	  noInterrupts();
	  adc_disable();
	  PRR |= _BV(PRADC);

	  // turn off brown-out enable in software
	  MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	  MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above

	  interrupts();             // guarantees next instruction executed
	  sleep_cpu();              // sleep within 3 clock cycles of above
	  
}

// the loop routine runs over and over again forever:
void loop() {
	TinyWireS_stop_check();
	//tws_delay(2500);
}
/*
void onI2CRequest() {
  // sends one byte with content 'b' to the master, regardless how many bytes he expects
  // if the buffer is empty, but the master is still requesting, the slave aborts the communication
  // (so it is not blocking)
  TinyWire.send('-');
//  if (anemometer.read(speedData, windData)){
	TinyWire.send((char)(windData & 0xFF));
	TinyWire.send(',');
	TinyWire.send((char)(windData >> 8));
	TinyWire.send(55);
	TinyWire.send(',');
	TinyWire.send((char)error);
	TinyWire.send('!');
//  }
}
*/

void pulseISR() {
	unsigned long curTime = micros();
	unsigned long deltaT = curTime - prevTime;

	curWindT = deltaT;
	weatherData.weather.dir = windData;
}
/*

void onI2CReceive(){
  // loops, until all received bytes are read
  while(TinyWire.available()>0){
	// toggles the led everytime, when an 'a' is received
	if(TinyWire.read()=='a') {
	  TxFlag = 1;
	}
  }
}

void transmitI2C(){

  // disable ISR
  for (int ii = 0; ii < bufferIdx; ii++){
	TinyWire.send('b');
  }
  TxFlag = 0;

}


ISR(TIMER1_COMPA_vect)
{
  error = anemometer.read(speedData, windData);
  //bufferData[bufferIdx] = curWindT*8/5;
  //bufferIdx++;
}


static inline void initTimer1(void)
{
  cli(); // stop interrupts
TCCR0A = 0; // set entire TCCR1A register to 0
TCCR0B = 0; // same for TCCR1B
TCNT0  = 0; // initialize counter value to 0


  TCCR1 |= (1 << CTC1);  // clear timer on compare match
  TCCR1 |= (1 << CS13) | (1 << CS12) | (0 << CS11) | (1 << CS10); //clock prescaler 8192
  OCR1A = 122; // compare match value
  TIMSK |= (1 << OCIE1A); // enable compare match interrupt
}
*/

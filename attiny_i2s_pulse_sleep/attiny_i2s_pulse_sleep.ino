#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include <TinyWireS.h>
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#include "LaCrosse_TX23.h"
#include <avr/sleep.h>
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)




typedef struct DataStruct {
	int dir;
	float speed;
	int error;
};

union {
	DataStruct weather;
	byte byteData[8];
} weatherData;






void requestEvent()
{

	while (true)
	{
		digitalWrite(PB3, HIGH);
		delay(1000);
		digitalWrite(PB3, LOW);
		delay(1000);
	}


	//cli();
	TinyWireS.send(weatherData.byteData[0]);
	TinyWireS.send(weatherData.byteData[1]);
	TinyWireS.send(weatherData.byteData[2]);
	TinyWireS.send(weatherData.byteData[3]);
	TinyWireS.send(weatherData.byteData[4]);
	TinyWireS.send(weatherData.byteData[5]);
	TinyWireS.send(weatherData.byteData[6]);
	TinyWireS.send(weatherData.byteData[7]);



	/* TinyWireS.send(rawData[5]);
	

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

	
}


volatile unsigned long prevTime = 0;
volatile unsigned long curWindT = 0;

float speedData = 0.0;
int windData = 25;



//DATA wire connected to arduino port 10
LaCrosse_TX23 anemometer = LaCrosse_TX23(3);


// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.


	pinMode(PB3, OUTPUT);

	digitalWrite(PB3, HIGH);
	delay(20);
	digitalWrite(PB3, LOW);
	delay(20);
	digitalWrite(PB3, HIGH);
	delay(20);
	digitalWrite(PB3, LOW);
	delay(20);
	digitalWrite(PB3, HIGH);
	delay(20);
	digitalWrite(PB3, LOW);
	delay(20);
	TinyWireS.begin(I2C_SLAVE_ADDRESS);
	TinyWireS.onRequest(requestEvent);

	//initTimer1();
	//sei();
	delay(500);


	GIMSK |= _BV(PCIE);
	PCMSK |= _BV(PCINT1);
	sei();




  //noInterrupts();
	adc_disable();
	PRR |= _BV(PRADC);

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);  // turn on brown-out enable select
	MCUCR = bit(BODS);        // this must be done within 4 clock cycles of above

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	interrupts();             // guarantees next instruction executed
	sleep_cpu();              // sleep within 3 clock cycles of above

}

// the loop routine runs over and over again forever:
void loop() {
	
	/*tws_delay(30);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	interrupts();             // guarantees next instruction executed
	sleep_cpu();

	TinyWireS_stop_check();
	//tws_delay(2500);
	*/

}


ISR(PCINT0_vect) {

	PCMSK &= ~_BV(PCINT1);

	if (digitalRead(PB1) == 0) {
		// This is called when the interrupt occurs, but I don't need to do anything in it
		unsigned long curTime = micros();
		unsigned long deltaT = curTime - prevTime;

		curWindT = deltaT;

		digitalWrite(PB3, HIGH);
		delay(20);
		digitalWrite(PB3, LOW);
		delay(20);
		digitalWrite(PB3, HIGH);
	}

	PCMSK |= _BV(PCINT1);

}




#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
#include "FRAM_tinywire.h"
#include <TinyWireS.h>
#define I2C_SLAVE_ADDRESS 0x4
/* Example code for the Adafruit I2C FRAM breakout */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 5.0V DC
   Connect GROUND to common ground */
   
FRAM_TINYWIRE fram     = FRAM_TINYWIRE();
uint16_t          framAddr = 0;
byte  receivedRXbyte = 0;
byte requestRec = 0;
byte reqData = 0;
byte recData = 0;

byte i2c_rx_buffer[10];
byte i2c_fram_buffer[] = { 0x01,0x01,0x01,0x01,0x01 };

void requestEvent()
{
//	TinyWireM.beginTransmission(10);
	
	TinyWireS.send(i2c_fram_buffer[0]);                 // if one-shot, start conversions now
	TinyWireS.send(i2c_fram_buffer[1]);                 // if one-shot, start conversions now
	TinyWireS.send(i2c_fram_buffer[2]);                 // if one-shot, start conversions now
	TinyWireS.send(i2c_fram_buffer[3]);                 // if one-shot, start conversions now
	TinyWireS.send(i2c_fram_buffer[4]);                 // if one-shot, start conversions now
	TinyWireS.send(i2c_rx_buffer[0]);                 // if one-shot, start conversions now
	TinyWireS.send(7);                 // if one-shot, start conversions now

//	TinyWireM.endTransmission();
	
}

void receiveEvent(uint8_t num_bytes)
{
	uint8_t i;

	// store the data from the master into the data buffer
	//for (i = 0; i < num_bytes; i++)
		
	i2c_rx_buffer[0] = TinyWireS.receive();
	recData = 1;
	//digitalWrite(PB3, HIGH);
	//delay(1);
  //  digitalWrite(PB3, LOW); 
	
	

}

void setup(void) {
  
 uint8_t test = 0;
/* pinMode(PB3, OUTPUT);
 digitalWrite(PB3, LOW);
 delay(300);
 digitalWrite(PB3, HIGH);
 delay(300);
 digitalWrite(PB3, LOW);
 */
  fram.begin();
  
  // Read the first byte
  
  test = fram.read8(0x0);
 
  // Test write ++
  fram.write8(0x0, test+1);
  fram.write8(0x1, 0x75);

  
  
  // dump the entire 32K of memory!
 // uint8_t value;
  //for (uint16_t a = 0; a < 20; a++) {
  //  value = fram.read8(a);
  
//  }
  TinyWireS.begin(I2C_SLAVE_ADDRESS);

  // register the onReceive() callback function
  TinyWireS.onReceive(receiveEvent);

  // register the onRequest() callback function
  TinyWireS.onRequest(requestEvent);
  //cli();

  
}

void loop(void) {

	if (recData) {
	//	digitalWrite(PB3, HIGH);
		delayMicroseconds(800);
	//	digitalWrite(PB3, LOW);
		//USICR = USICR & (0 << USISIE);
		//cli();
		fram.begin();
		TinyWireM.begin();
		delayMicroseconds(100);
		fram.write8(0x1, 0x12);
		fram.write8(0x1, 0x14);
		fram.write8(0x2, 0x24);
		fram.write8(0x3, 0x34);
		fram.write8(0x4, 0x44);

		delayMicroseconds(100);
		i2c_fram_buffer[0] = fram.read8(0x0);
		i2c_fram_buffer[1] = fram.read8(0x1);
		i2c_fram_buffer[2] = fram.read8(0x2);
		i2c_fram_buffer[3] = fram.read8(0x3);
		i2c_fram_buffer[4] = fram.read8(0x4);

		recData = 0;
		
		TinyWireS.begin(I2C_SLAVE_ADDRESS);

		// register the onReceive() callback function
		//TinyWireS.onReceive(receiveEvent);

		// register the onRequest() callback function
		
		//TinyWireS.onRequest(requestEvent);
		
//		digitalWrite(PB3, HIGH);
	}
//	TinyWireS_stop_check();



}

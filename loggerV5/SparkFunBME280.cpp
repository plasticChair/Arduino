/******************************************************************************
SparkFunBME280.cpp
BME280 Arduino and Teensy Driver
Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/BME280_Breakout

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/
//See SparkFunBME280.h for additional topology notes.

#include "SparkFunBME280.h"
#include "stdint.h"
#include <math.h>

#include "Wire.h"
#include "SPI.h"

//****************************************************************************//
//
//  Settings and configuration
//
//****************************************************************************//

//Constructor -- Specifies default configuration
BME280::BME280(uint8_t devID, HardwareSerial5 &refSer)
{
	//Construct with these default settings if nothing is specified
	_serialRef = refSer;

	//Select CS pin for SPI.  Does nothing for I2C
	settings.chipSelectPin = 10;
	settings.runMode = 0;
	settings.tempOverSample = 0;
	settings.pressOverSample = 0;
	settings.humidOverSample = 0;

	if (devID == 0)
	{
		settings.I2CAddress = 0x76; //Ignored for SPI_MODE
	}
	else
	{
		settings.I2CAddress = 0x77; //Ignored for SPI_MODE
	}
  settings.devID = devID;

}


//****************************************************************************//
//
//  Configuration section
//
//  This uses the stored SensorSettings to start the IMU
//  Use statements such as "mySensor.settings.commInterface = SPI_MODE;" to 
//  configure before calling .begin();
//
//****************************************************************************//
uint8_t BME280::begin()
{

	uint8_t dataToWrite;

    Wire.begin();


  

  //Reading all compensation data, range 0x88:A1, 0xE1:E7
  
  calibration.dig_T1 = ((uint16_t)((readRegister(BME280_DIG_T1_MSB_REG) << 8) + readRegister(BME280_DIG_T1_LSB_REG)));
  calibration.dig_T2 = ((int16_t)((readRegister(BME280_DIG_T2_MSB_REG) << 8) + readRegister(BME280_DIG_T2_LSB_REG)));
  calibration.dig_T3 = ((int16_t)((readRegister(BME280_DIG_T3_MSB_REG) << 8) + readRegister(BME280_DIG_T3_LSB_REG)));

  calibration.dig_P1 = ((uint16_t)((readRegister(BME280_DIG_P1_MSB_REG) << 8) + readRegister(BME280_DIG_P1_LSB_REG)));
  calibration.dig_P2 = ((int16_t)((readRegister(BME280_DIG_P2_MSB_REG) << 8) + readRegister(BME280_DIG_P2_LSB_REG)));
  calibration.dig_P3 = ((int16_t)((readRegister(BME280_DIG_P3_MSB_REG) << 8) + readRegister(BME280_DIG_P3_LSB_REG)));
  calibration.dig_P4 = ((int16_t)((readRegister(BME280_DIG_P4_MSB_REG) << 8) + readRegister(BME280_DIG_P4_LSB_REG)));
  calibration.dig_P5 = ((int16_t)((readRegister(BME280_DIG_P5_MSB_REG) << 8) + readRegister(BME280_DIG_P5_LSB_REG)));
  calibration.dig_P6 = ((int16_t)((readRegister(BME280_DIG_P6_MSB_REG) << 8) + readRegister(BME280_DIG_P6_LSB_REG)));
  calibration.dig_P7 = ((int16_t)((readRegister(BME280_DIG_P7_MSB_REG) << 8) + readRegister(BME280_DIG_P7_LSB_REG)));
  calibration.dig_P8 = ((int16_t)((readRegister(BME280_DIG_P8_MSB_REG) << 8) + readRegister(BME280_DIG_P8_LSB_REG)));
  calibration.dig_P9 = ((int16_t)((readRegister(BME280_DIG_P9_MSB_REG) << 8) + readRegister(BME280_DIG_P9_LSB_REG)));

  calibration.dig_H1 = ((uint8_t)(readRegister(BME280_DIG_H1_REG)));
  calibration.dig_H2 = ((int16_t)((readRegister(BME280_DIG_H2_MSB_REG) << 8) + readRegister(BME280_DIG_H2_LSB_REG)));
  calibration.dig_H3 = ((uint8_t)(readRegister(BME280_DIG_H3_REG)));
  calibration.dig_H4 = ((int16_t)((readRegister(BME280_DIG_H4_MSB_REG) << 4) + (readRegister(BME280_DIG_H4_LSB_REG) & 0x0F)));
  calibration.dig_H5 = ((int16_t)((readRegister(BME280_DIG_H5_MSB_REG) << 4) + ((readRegister(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
  calibration.dig_H6 = ((uint8_t)readRegister(BME280_DIG_H6_REG));

  //Set the oversampling control words.
  //config will only be writeable in sleep mode, so first insure that.
  writeRegister(BME280_CTRL_MEAS_REG, 0x00);
  
  //Set the config word
  dataToWrite = (settings.tStandby << 0x5) & 0xE0;
  dataToWrite |= (settings.filter << 0x02) & 0x1C;
  writeRegister(BME280_CONFIG_REG, dataToWrite);
  
  //Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
  dataToWrite = settings.humidOverSample & 0x07; //all other bits can be ignored
  writeRegister(BME280_CTRL_HUMIDITY_REG, dataToWrite);
  
  //set ctrl_meas
  //First, set temp oversampling
  dataToWrite = (settings.tempOverSample << 0x5) & 0xE0;
  //Next, pressure oversampling
  dataToWrite |= (settings.pressOverSample << 0x02) & 0x1C;
  //Last, set mode
  dataToWrite |= (settings.runMode) & 0x03;
  //Load the byte
  writeRegister(BME280_CTRL_MEAS_REG, dataToWrite);
  
  return readRegister(0xD0);
}

// ---------------------------------------------------------------------------
void BME280::Setup()
{

	//runMode can be:
	//  0, Sleep mode
	//  1 or 2, Forced mode
	//  3, Normal mode
	settings.runMode = 3; //Normal mode

									 //tStandby can be:
									 //  0, 0.5ms
									 //  1, 62.5ms
									 //  2, 125ms
									 //  3, 250ms
									 //  4, 500ms
									 //  5, 1000ms
									 //  6, 10ms
									 //  7, 20ms
	settings.tStandby = 0;

	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	settings.filter = 1;

	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings.tempOverSample = 2;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings.pressOverSample = 2;

	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings.humidOverSample = 2;

	delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.]
	_serialRef.println("Here before init");
	_serialRef.println(begin(), HEX);




	_serialRef.print("Displaying ID, reset and ctrl regs\n");

	_serialRef.print("ID(0xD0): 0x");
	_serialRef.println(readRegister(BME280_CHIP_ID_REG), HEX);
	_serialRef.print("Reset register(0xE0): 0x");
	_serialRef.println(readRegister(BME280_RST_REG), HEX);
	_serialRef.print("ctrl_meas(0xF4): 0x");
	_serialRef.println(readRegister(BME280_CTRL_MEAS_REG), HEX);
	_serialRef.print("ctrl_hum(0xF2): 0x");
	_serialRef.println(readRegister(BME280_CTRL_HUMIDITY_REG), HEX);
	_serialRef.print("\n\n");

	_serialRef.print("Displaying all regs\n");
	uint8_t memCounter = 0x80;
	uint8_t tempReadData;
	for (int rowi = 8; rowi < 16; rowi++)
	{
		_serialRef.print("0x");
		_serialRef.print(rowi, HEX);
		_serialRef.print("0:");

		for (int coli = 0; coli < 16; coli++)
		{
			tempReadData = readRegister(memCounter);
			_serialRef.print((tempReadData >> 4) & 0x0F, HEX);//Print first hex nibble
			_serialRef.print(tempReadData & 0x0F, HEX);//Print second hex nibble
			_serialRef.print(" ");
			memCounter++;
		}
		_serialRef.print("\n");
	}


	_serialRef.print("\n\n");

	_serialRef.print("Displaying concatenated calibration words\n");
	_serialRef.print("dig_T1, uint16: ");
	_serialRef.println(calibration.dig_T1);
	_serialRef.print("dig_T2, int16: ");
	_serialRef.println(calibration.dig_T2);
	_serialRef.print("dig_T3, int16: ");
	_serialRef.println(calibration.dig_T3);

	_serialRef.print("dig_P1, uint16: ");
	_serialRef.println(calibration.dig_P1);
	_serialRef.print("dig_P2, int16: ");
	_serialRef.println(calibration.dig_P2);
	_serialRef.print("dig_P3, int16: ");
	_serialRef.println(calibration.dig_P3);
	_serialRef.print("dig_P4, int16: ");
	_serialRef.println(calibration.dig_P4);
	_serialRef.print("dig_P5, int16: ");
	_serialRef.println(calibration.dig_P5);
	_serialRef.print("dig_P6, int16: ");
	_serialRef.println(calibration.dig_P6);
	_serialRef.print("dig_P7, int16: ");
	_serialRef.println(calibration.dig_P7);
	_serialRef.print("dig_P8, int16: ");
	_serialRef.println(calibration.dig_P8);
	_serialRef.print("dig_P9, int16: ");
	_serialRef.println(calibration.dig_P9);

	_serialRef.print("dig_H1, uint8: ");
	_serialRef.println(calibration.dig_H1);
	_serialRef.print("dig_H2, int16: ");
	_serialRef.println(calibration.dig_H2);
	_serialRef.print("dig_H3, uint8: ");
	_serialRef.println(calibration.dig_H3);
	_serialRef.print("dig_H4, int16: ");
	_serialRef.println(calibration.dig_H4);
	_serialRef.print("dig_H5, int16: ");
	_serialRef.println(calibration.dig_H5);
	_serialRef.print("dig_H6, uint8: ");
	_serialRef.println(calibration.dig_H6);

	_serialRef.println();

}


//Strictly resets.  Run .begin() afterwards
void BME280::reset( void )
{
  writeRegister(BME280_RST_REG, 0xB6);
  
}

//****************************************************************************//
//
//  Pressure Section
//
//****************************************************************************//
float BME280::readFloatPressure( void )
{

  // Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
  // Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
  int32_t adc_P = ((uint32_t)readRegister(BME280_PRESSURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_PRESSURE_LSB_REG) << 4) | ((readRegister(BME280_PRESSURE_XLSB_REG) >> 4) & 0x0F);
  
  int64_t var1, var2, p_acc;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)calibration.dig_P6;
  var2 = var2 + ((var1 * (int64_t)calibration.dig_P5)<<17);
  var2 = var2 + (((int64_t)calibration.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)calibration.dig_P3)>>8) + ((var1 * (int64_t)calibration.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calibration.dig_P1)>>33;
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p_acc = 1048576 - adc_P;
  p_acc = (((p_acc<<31) - var2)*3125)/var1;
  var1 = (((int64_t)calibration.dig_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
  var2 = (((int64_t)calibration.dig_P8) * p_acc) >> 19;
  p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)calibration.dig_P7)<<4);
  
  return (float)p_acc / 256.0;
  
}

float BME280::readFloatAltitudeMeters( void )
{
  float heightOutput = 0;
  
  heightOutput = ((float)-45846.2)*(pow(((float)readFloatPressure()/(float)101325), 0.190263) - (float)1);
  return heightOutput;
  
}

float BME280::readFloatAltitudeFeet( void )
{
  float heightOutput = 0;
  
  heightOutput = readFloatAltitudeMeters() * 3.28084;
  return heightOutput;
  
}

//****************************************************************************//
//
//  Humidity Section
//
//****************************************************************************//
float BME280::readFloatHumidity( void )
{
  
  // Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
  // Output value of “47445” represents 47445/1024 = 46. 333 %RH
  int32_t adc_H = ((uint32_t)readRegister(BME280_HUMIDITY_MSB_REG) << 8) | ((uint32_t)readRegister(BME280_HUMIDITY_LSB_REG));
  
  int32_t var1;
  var1 = (t_fine - ((int32_t)76800));
  var1 = (((((adc_H << 14) - (((int32_t)calibration.dig_H4) << 20) - (((int32_t)calibration.dig_H5) * var1)) +
  ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)calibration.dig_H6)) >> 10) * (((var1 * ((int32_t)calibration.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
  ((int32_t)calibration.dig_H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)calibration.dig_H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);

  return (float)(var1>>12) / 1024.0;

}



//****************************************************************************//
//
//  Temperature Section
//
//****************************************************************************//

float BME280::readTempC( void )
{
  // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
  // t_fine carries fine temperature as global value

  //get the reading (adc_T);
  int32_t adc_T = ((uint32_t)readRegister(BME280_TEMPERATURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_TEMPERATURE_LSB_REG) << 4) | ((readRegister(BME280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);

  //By datasheet, calibrate
  int64_t var1, var2;

  var1 = ((((adc_T>>3) - ((int32_t)calibration.dig_T1<<1))) * ((int32_t)calibration.dig_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)calibration.dig_T1)) * ((adc_T>>4) - ((int32_t)calibration.dig_T1))) >> 12) *
  ((int32_t)calibration.dig_T3)) >> 14;
  t_fine = var1 + var2;
  float output = (t_fine * 5 + 128) >> 8;

  output = output / 100;
  
  return output;
}

float BME280::readTempF( void )
{
  float output = readTempC();
  output = (output * 9) / 5 + 32;

  return output;
}

//****************************************************************************//
//
//  Utility
//
//****************************************************************************//
void BME280::readRegisterRegion(uint8_t *outputPointer , uint8_t offset, uint8_t length)
{
  //define pointer that will point to the external space
  uint8_t i = 0;
  char c = 0;

  
    Wire.beginTransmission(settings.I2CAddress);
    Wire.write(offset);
    Wire.endTransmission();

    // request bytes from slave device
    Wire.requestFrom(settings.I2CAddress, length);
    while ( (Wire.available()) && (i < length))  // slave may send less than requested
    {
      c = Wire.read(); // receive a byte as character
      *outputPointer = c;
      outputPointer++;
      i++;
    }
   
    // take the chip select low to select the device:
    digitalWrite(settings.chipSelectPin, LOW);
    // send the device the register you want to read:
    SPI.transfer(offset | 0x80);  //Ored with "read request" bit
    while ( i < length ) // slave may send less than requested
    {
      c = SPI.transfer(0x00); // receive a byte as character
      *outputPointer = c;
      outputPointer++;
      i++;
    }
    // take the chip select high to de-select:
    digitalWrite(settings.chipSelectPin, HIGH);
 

}

uint8_t BME280::readRegister(uint8_t offset)
{
  //Return value
  uint8_t result;
  uint8_t numBytes = 1;
  
    Wire.beginTransmission(settings.I2CAddress);
    Wire.write(offset);
    Wire.endTransmission();

    Wire.requestFrom(settings.I2CAddress, numBytes);
    while ( Wire.available() ) // slave may send less than requested
    {
      result = Wire.read(); // receive a byte as a proper uint8_t
    }
    

 
  return result;
}

int16_t BME280::readRegisterInt16( uint8_t offset )
{
  uint8_t myBuffer[2];
  readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
  int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
  
  return output;
}

void BME280::writeRegister(uint8_t offset, uint8_t dataToWrite)
{
  
    //Write the byte
    Wire.beginTransmission(settings.I2CAddress);
    Wire.write(offset);
    Wire.write(dataToWrite);
    Wire.endTransmission();
  
    // take the chip select low to select the device:
    digitalWrite(settings.chipSelectPin, LOW);
    // send the device the register you want to read:
    SPI.transfer(offset & 0x7F);
    // send a value of 0 to read the first byte returned:
    SPI.transfer(dataToWrite);
    // decrement the number of bytes left to read:
    // take the chip select high to de-select:
    digitalWrite(settings.chipSelectPin, HIGH);
   
}


void BME280::print()
{

		
	//Each loop, take a reading.
	//Start with temperature, as that data is needed for accurate compensation.
	//Reading the temperature updates the compensators of the other functions
	//in the background.

	//BME280Sensor.writeRegister(BME280_CTRL_MEAS_REG, tempSensor.settings.initCtrlRegs);

	delay(100);

	readFloatPressure();
	_serialRef.print("Temperature: ");
	_serialRef.print(readTempF(), 2);
	_serialRef.println(" degrees F");

	// _serialRef.print("Pressure: ");
	// _serialRef.print(currentSensorData.BME280_pressure_kPa, 2);
	// _serialRef.println(" Pa");

	_serialRef.print("Altitude: ");
	_serialRef.print(readFloatAltitudeFeet(), 2);
	_serialRef.println("ft");

	_serialRef.print("%RH: ");
	_serialRef.print(readFloatHumidity(), 2);
	_serialRef.println(" %");

	_serialRef.println();
}


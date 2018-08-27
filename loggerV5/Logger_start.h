#ifndef _LOGGERSTART_h
#define _LOGGERSTART_h

#include <Arduino.h>
#include <NMEAGPS.h>
#include <SPI.h>
#include <SD.h>
#include <SD_t3.h>
#include <stdlib.h>
//#include <NeoHWSerial.h>
/*******************************************************/

#include <stdint.h>
#include "SparkFunBME280.h"
//#include "quaternionFilters.h"
#include "Wire.h"
//#include <i2c_t3.h>
//define gps_port NeoSerial1
#include "Streamers.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

#include "setupTest.h"
#include "IMU.h"
#include "BLE.h"
#include "GPS.h"

#include "Options.h"




// ADXL345
void ADXL345Setup(void);
void ADXL_ISR(void);
void printAccel(void);

// BME280
void BME280Setup(void);
void printBME280(void);

// SD Card
void SDsetup(void);
void SDWrite(void);

// GPS
//void gpsSetup();
void doSomeWork();

// General
void flashPin(int, bool,int *delays);



// Variables
volatile struct sensorDataStruct
{
	float accelX_g;
	float accelY_g;
	float accelZ_g;

	float BME280_humidity_pct;
	float BME280_pressure_kPa;
	float BME280_alt_ft;
	float BME280_temp_f;

	float gpsAlt_f;
	float gpsLong_deg;
	float gpsLat_deg;
	float gpsNumSat;
	float gpsTime;

	float gpsFiltPosX;
	float gpsFiltPosY;
	float gpsFiltPosAlt;

	float avgAlt[SDDataSize];

} currentSensorData;





int SD_CS = BUILTIN_SDCARD;


int SDLoopCount = 0;
int SDWriteCount = 0;

char LogFileName[30] = "GPS_Log_1.txt";

// Enable/ power pins
extern int SD_CS;
int Pin_GPS_pwr = 28;
int Pin_Batt_mon_en = 20;
int Pin_Batt_mon = 35;
int Pin_BLE_pwr = 37;  // Need to update
int Pin_Sens_pwr = 25;

// Function button pins
int Pin_fun1 = 23;
int Pin_fun2 = 22;
int Pin_fun3 = 21;

// LEDs
int Pin_led1 = 2;
int Pin_led2 = 3; // was 11
int Pin_led3 = 4;
int Pin_led4 = 5;
int sysLED = 13;


int flashCycle[10] = { 0,0,0,0,0,0,0,0,0,0 };

#endif 
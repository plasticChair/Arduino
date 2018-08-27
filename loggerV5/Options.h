#pragma once
#ifndef MY_HEADER_H
#define MY_HEADER_H


#define DEBUG_PORT Serial5 //Serial1
#define GPS_PORT Serial1
#define BLE_PORT Serial4

#define GPS_Enable 0
#define SD_Enable  0
#define BME_Enable 0
#define IMU_Enable 1
#define BLE_Enable 0


#define SDDataSize 10
#define SEALEVELPRESSURE_HPA (1013.25)

#define AHRS true         // Set to false for basic data read
#define SerialDebug true  // Set to true to get Serial output for debugging




#endif 
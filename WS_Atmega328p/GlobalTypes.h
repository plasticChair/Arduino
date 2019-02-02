#pragma once
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include "Arduino.h"

#define PulseBuffSize 8
#define wxPackageSize 22

// Structure defines
typedef struct ControlStruct {

	// Weather/Gust
	byte          PulseBuffIdx;
	byte          gustAvgCnt;
	unsigned long dataStoreCnt;  
	unsigned long ANECnt;
	unsigned int  gustAvg;

	//FRAM
	byte FRAM_Idx;
	byte FRAM_NumPoints;

	//GPIO
	bool ANE_enable;

	//RTC
	byte         RTC_count;
	time_t       t;
	time_t       prevt;
	tmElements_t prevtime;
	tmElements_t time;



	// Low Power Mode
	bool LP_Mode;
	int  LP_Mode_Cnt;

	// Measurement and Server Intervals
	float Meas_Int;
	float Meas_Int_LP;
	byte  Server_Int;
	bool  Server_Tx_En;
	byte  Ane_Int;

} CntrlStruct;


typedef struct wxStruct {
	byte         year     : 4;
	byte         month    : 4;
	unsigned int day      : 5;
	unsigned int hour     : 5;
	unsigned int minute   : 6;
	unsigned int second   : 6;
	byte         dir      : 4;
	unsigned int speed    : 10;
	unsigned int gustMax  : 10;
	unsigned int temp     : 10;
	unsigned int press    : 10;
	unsigned int humid    : 10;
	unsigned int batteryV : 8;
	unsigned int batteryP : 8;

};

typedef struct nativeData {
	int   dir;
	float speed;
	float gustMax;
	float temp;
	float press;
	float humid;
	float batteryV;
	float batteryP;

};

typedef struct wxSFStruct {
	float windSF;
	float gustSF;
	float tempSF;
	float pressSF;
	float humidSF;
	float batteryVSF;
	float batteryPSF;
	int   tempOff;
	int   pressOff;
	int   battOff;

};

typedef struct DataStruct {
	wxStruct     weather;
	unsigned int gustBuff[PulseBuffSize];
	unsigned int gustAvg;
	unsigned int gustMax;

} MstrDataStruct;

typedef struct PulseStruct {
	unsigned long pulseDt;
	unsigned long pulseCt;
	unsigned long pulsePt;
	bool          servicePulseISR;
	bool          serviceRTCISR;
	int           RTC_count;
	bool          pulseSkip;
	unsigned long Tmr2Cnt;//timer 2

};

typedef union {
	wxStruct weather;
	byte     TxData[wxPackageSize];
}storeDataUnion;


typedef struct IOStruct {
	uint8_t ANE_TX;
	uint8_t ANE_Pulse;
	uint8_t ANE_En;
	uint8_t RTC_Alarm;
	uint8_t Charge_EN;
	uint8_t LED1;
	uint8_t LED2;
	uint8_t WAKE;
	uint8_t DONE;
	uint8_t BATT_ALRT;
};
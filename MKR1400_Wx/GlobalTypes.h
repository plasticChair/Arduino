// GlobalTypes.h

#ifndef _GLOBALTYPES_h
#define _GLOBALTYPES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "UserParams.h"


typedef struct ControlStruct {

	//FRAM
	unsigned int FRAM_Idx;
	unsigned int FRAM_NumPoints;
	unsigned int FRAM_CurrIdx;
	uint8_t FRAM_reset_count;
	
	uint16_t measInterval;
	uint16_t serverInterval;
	unsigned int jsonSize;

	// Low Power Mode
	bool LP_Mode;
	int  LP_Mode_Cnt;
	
	bool RTCUpdateFlag;
	
	// Sleep Mode
	bool sleepModeEn;
	bool sleepModeSet;
	bool sleepSingle;
	int sleepHour;
	int wakeupHour;
	
} CntrlStruct;

typedef struct wxStruct {
	byte         year      : 4;
	byte         month     : 4;
	unsigned int day       : 6;
	unsigned int hour      : 6;
	unsigned int minute    : 6;
	unsigned int second    : 6; // 32
	
	byte         dir       : 4;
	unsigned int speed     : 12;
	unsigned int gustMax   : 12; 
	byte         accessCode: 4;	//32
	
	unsigned int temp      : 10;
	unsigned int press     : 12;
	unsigned int humid     : 10; // 32
	
	unsigned int batteryV  : 8;
	unsigned int batteryP  : 10;
	byte         signalRSSI: 5; 
	byte         LowPwrMode: 1;
	byte         info      : 4;
	byte         id        : 4;//32

};

typedef struct nativeData {
	int   dir;
	float speed;
	float gustMax;
	float gustAvg;
	float temp;
	float press;
	float humid;
	float batteryV;
	float batteryP;
	int rssi;

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



typedef union {
	wxStruct weather;
	byte     TxData[wxPackageSize];
}storeDataUnion;

typedef struct timeStruct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
};

typedef struct RTCStruct {
	timeStruct currTime;
	timeStruct alarmTime;
};

typedef struct pulseStruct{
	uint32_t countVal;
	uint32_t buffer[PULSEDT_BUFFLENGTH];
	uint8_t buffIdx;
	bool buffFullFlag;
	bool RTCFlag;
};

typedef struct serverStruct{
	// Server Control
	char server[17];
	char serverIP[17];
	char path[16];
	int portIP;
	int portServer;
	bool serverConnection;
};


#endif


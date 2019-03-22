// GlobalTypes.h

#ifndef _GLOBALTYPES_h
#define _GLOBALTYPES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


typedef struct timeStruct {
	byte sec;
	byte min;
	byte hour;
	byte day;
	byte month;
	byte year;
};

typedef struct RTCStruct {
	timeStruct currTime;
	timeStruct alarmTime;
};


#endif


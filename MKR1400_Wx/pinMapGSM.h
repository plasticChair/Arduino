// pinMapGSM.h

#ifndef _PINMAPGSM_h
#define _PINMAPGSM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define GPLED1 A6
#define GPLED2 A5
#define ANE_Vcc_EN 0
#define ANE_TX 1
#define ANE_PULSE A1
#define DONE 3
#define WAKE 4
#define BATT_ALRT 5
#define I2C_SCL 12
#define I2C_SDA 11


#endif


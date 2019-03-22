// General.h

#ifndef _GENERAL_h
#define _GENERAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "pinMapGSM.h"

void Anemometer_Enable()
{
	digitalWrite(ANE_Vcc_EN, false);
}

void Anemometer_Disable()
{
	digitalWrite(ANE_Vcc_EN, true);
}




#endif


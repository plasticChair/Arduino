// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       SAMD21_shieldtest.ino
    Created:	3/15/2019 10:55:07 AM
    Author:     DESKTOP-4LJLPCB\mosta
*/

// Define User Types below here or use a .h file
//
#include "setupFunctions.h"
#include "pinMapGSM.h"
#include "Adafruit_FRAM_I2C.h"
#include "LaCrosse_TX23.h"
#include <MAX17048.h>
#include "SparkFunBME280_I2C.h"


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts

void setup()
{
	GPIOSetup();
	GPIODefaults();
	GPIO_dance();
}

// Add the main program code into the continuous loop() function
void loop()
{


}

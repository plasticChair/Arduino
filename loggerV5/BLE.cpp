// 
// 
// 

#include "BLE.h"



void BLESetup()
{

	digitalWrite(Pin_BLE_pwr, HIGH);

	BLE_PORT.print("AT+MODE2"); // Enable AT commands over BLE

}
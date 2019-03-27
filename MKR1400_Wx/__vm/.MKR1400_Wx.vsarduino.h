/* 
	Editor: https://www.visualmicro.com/
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino MKR GSM 1400 No BL, Platform=samd, Package=arduino
*/

#define ARDUINO 10806
#define ARDUINO_MAIN
#define F_CPU 48000000L
#define printf iprintf
#define __SAMD__
#define F_CPU 48000000L
#define ARDUINO 10806
#define ARDUINO_SAMD_MKRGSM1400
#define ARDUINO_ARCH_SAMD
#define USE_ARDUINO_MKR_PIN_LAYOUT
#define __SAMD21G18A__
#define USB_VID 0x2341
#define USB_PID 0x8052
#define USBCON
#define USE_BQ24195L_PMIC

#include "pins_arduino.h" 
#include "variant.h" 
#include "arduino.h"
#include "MKR1400_Wx.ino"

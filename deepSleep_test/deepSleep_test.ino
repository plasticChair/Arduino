/***************************************
 This shows all the wakeups for deepSleep
 Expect IDD of  around 230uA (Teensy 3.x)
 and IDD of around 150uA for (Teensy LC).
 Teensy 3.5/3.6 are yet to be determined.
 ****************************************/
#include <Snooze.h>

// Load drivers
SnoozeDigital digital;

// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer  lc5vBuffer;
/***********************************************************
 Teensy 3.6/LC can't use Timer Driver with either Touch or
 Compare Drivers and Touch can't be used with Compare.
 
 Teensy 3.5 does not touch interface.
 
 Teensy LC does not have a rtc so Alarm driver can't be
 used as of yet.
 
 Teensy 3.2 can use any Core Drivers together.
 ***********************************************************/

SnoozeBlock config_teensyLC(digital, lc5vBuffer);


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    /********************************************************
     Define digital pins for waking the teensy up. This
     combines pinMode and attachInterrupt in one function.
     
     Teensy 3.x
     Digtal pins: 2,4,6,7,9,10,11,13,16,21,22,26,30,33
     
     Teensy LC
     Digtal pins: 2,6,7,9,10,11,16,21,22
     ********************************************************/
    digital.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
 

    
    Serial.begin(9600);
 digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
             digitalWrite(LED_BUILTIN, LOW);
            delay(200);
             digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
             digitalWrite(LED_BUILTIN, LOW);
            delay(200);
             digitalWrite(LED_BUILTIN, HIGH);
            delay(200);

  
}

void loop() {

    digitalWrite(LED_BUILTIN, HIGH);
            delay(5000);
    Serial.println("Entering Sleep");


    Snooze.deepSleep( config_teensyLC );// return module that woke processor


    Serial.println("I'm awake");

    digitalWrite(LED_BUILTIN, LOW);
            delay(5000);
    

    

}

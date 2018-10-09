// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
#define LED1 1
#define waterEnPin 6

void setup () {
  //while (!Serial); // for Leonardo/Micro/Zero

  
  Serial.begin(115200);

  delay(200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  pinMode(LED1,OUTPUT);
  pinMode(waterEnPin,OUTPUT);
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
      // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  digitalWrite(LED1, HIGH);
}

void loop () {
  
    DateTime now = rtc.now();
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.println();

    digitalWrite(waterEnPin, HIGH);
    digitalWrite(LED1, LOW);
   delay(2000);
   ESP.deepSleep(10*1000000, WAKE_RF_DEFAULT);

    }

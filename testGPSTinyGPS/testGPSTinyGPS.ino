#include <TinyGPS.h>
 
TinyGPS gps;
 
void
setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
 while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }// Start the serial terminal
    Serial.println("Starting");
}
 
void
loop()
{
    unsigned long date, time, age;
    long lat, lng ;
    for (;;) {
  
        while (Serial1.available()) {
            char c = Serial1.read();
            if (gps.encode(c)) {
                gps.get_datetime(&date, &time, &age);
                Serial.print("Date: ");
                Serial.println(date);
                Serial.print("Time: ");
                Serial.println(time);
                gps.get_position(&lat, &lng) ;
                Serial.print("LAT: ");
                Serial.println(lat);
                Serial.print("LNG: ");
                Serial.println(lng);
                Serial.println();
            }
        }
    }
}

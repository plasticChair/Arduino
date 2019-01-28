// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

typedef struct DataStruct{
  int dir;
  float speed;
  int error;
};

union {
  DataStruct weather;
   byte dirByte[8];
} weatherInfo;




/*
typedef union DataUnion{
DataStruct weatherData;
byte dirByte[sizeof(DataStruct)];
}weatherInfo;
*/
byte test = 7;

byte bufferData[55];

void setup() {
  Wire.begin(7);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  Serial.println("Setup Done");


  
}

void loop() {

//  Wire.requestFrom(4, 1);    // request 6 bytes from slave device #8

  delay(50);
  
  Wire.requestFrom(4, 15);    // request 6 bytes from slave device #8
  int idx = 0;
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    bufferData[idx] = c;
    //weatherData.dirByte[idx] = c;
    idx++;
  }

  weatherInfo.dirByte[0] = bufferData[0];
  weatherInfo.dirByte[1] = bufferData[1];
  weatherInfo.dirByte[2] = bufferData[2];
  weatherInfo.dirByte[3] = bufferData[3];
  
  weatherInfo.dirByte[4] = bufferData[4];
  weatherInfo.dirByte[5] = bufferData[5];
  weatherInfo.dirByte[6] = bufferData[6];
  weatherInfo.dirByte[7] = bufferData[7];
  



    Serial.print(weatherInfo.weather.dir);         // print the character
   Serial.print(',');         // print the character
    Serial.print(weatherInfo.weather.error);         // print the character
    Serial.print(',');
    Serial.print(weatherInfo.weather.speed);         // print the character
    Serial.print(',');
    for (int ii = 4; ii<15; ii++){
         Serial.print((bool)bufferData[ii+4]);         // print the character

    }
   

  Serial.println(" - sent Request");

  Serial.println(test);
  Serial.println(test|_BV(3));
  Serial.println(test & ~_BV(2));
  
  delay(5000);
}

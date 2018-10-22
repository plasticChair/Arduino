/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>
#include "Arduino.h"

void recvWithStartEndMarkers();

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // set the data rate for the SoftwareSerial port

  Serial.println("<" + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3)+ "," +  String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3)+ ">");
//  Serial.println("<0," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3) + ">");
//  delay(20);
//  Serial.println("<1," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3)+ ">"); 
 // delay(20);
//  Serial.println("<2," + String(0.0,3) + "," + String(0.0,3) + "," + String(0.0,3)+ ">");
}

void loop() { // run over and over
float randV;
float randA;
char charBuffer[12];

randV = random(99)/100.0;
randA = random(99)/100.0;

float v3V3_V = 3.3+randV/5;
float v3V3_A = randA;
float v3V3_W = v3V3_V*v3V3_A;

float v5V_V = 5+randV*0.3;
float v5V_A = randA*1.2;
float v5V_W = v5V_V*v5V_A;

float v12V_V = 12.0 + randV ;
float v12V_A = randA*4.2;
float v12V_W = v12V_V*v12V_A;

  recvWithStartEndMarkers();

//delay(10);
//<3.121,0.22, 5.1,0.98,12.89,1.23>  

 if (newData == true) {
  //Serial.println(receivedChars);
 // Serial.println(strcmp(receivedChars, "XXX"));
    if(!strcmp(receivedChars, "XXX")) {
 Serial.println("<" + String(v3V3_V,3) + "," + String(v3V3_A,3) + "," + String(v3V3_W,3) + "," + String(v5V_V,3) + "," + String(v5V_A,3) + "," + String(v5V_W,3)+ "," +  String(v12V_V,2) + "," + String(v12V_A,3) + "," + String(v12V_W,3)+ ">");
      /*
      Serial.println("<0," + String(v3V3_V,3) + "," + String(v3V3_A,3) + "," + String(v3V3_W,3) + ">");
      delay(50);
      Serial.println("<1," + String(v5V_V,3) + "," + String(v5V_A,3) + "," + String(v5V_W,3)+ ">"); 
      delay(50);
      Serial.println("<2," + String(v12V_V,3) + "," + String(v12V_A,3) + "," + String(v12V_W,3)+ ">");
    */
    }
    
        newData = false;
    }

}
  

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
              //  Serial.println(receivedChars);
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
            
        }
    }
}

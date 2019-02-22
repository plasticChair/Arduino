/*
  Web client

 This sketch connects to a website through a MKR GSM 1400 board. Specifically,
 this example downloads the URL "http://www.example.org/" and
 prints it to the Serial monitor.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe
*/

// libraries
#include <MKRGSM.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess = 1;

// URL, path and port (for example: example.org)
//char server[] = "scottn.ddns.net";
char server[] = "68.230.107.205";
char path[] = "/ardu2SQL.php";
int port = 5211; // port 80 is the default for HTTP

void setup() {
  // initialize serial communications and wait for port to open:
  Serial1.begin(9600);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial1.println("Starting Arduino web client.");
  // connection state
  bool connected = false;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  for (int ii = 0; ii<10; ii++){
    if( gsmAccess.begin() == GSM_READY){
      break;
    }
    delay(1000);
    Serial1.println(ii);
  }

  Serial1.println("stage 2");
  
   for (int ii = 0; ii<10; ii++){
    if( gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY){
      break;
    }
    delay(1000);
    Serial1.println(ii);
  }

  

  Serial1.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial1.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println("68.230.107.205");
    client.println("Content-Type: application/json");
    client.print("Content-Length: 13");
    client.println("{\"data\": \"bar\"}");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial1.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial1.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    Serial1.println();
    Serial1.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;)
      ;
  }
}

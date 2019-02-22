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
#include "GSMFun.h"



// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess = 1;


char server[] = "scottn.ddns.net";
char serverIP[] = "68.230.107.205";
char path[] = "/ardu2SQL.php?";
int portIP = 5211; // port 80 is the default for HTTP
int portServer = 80; // port 80 is the default for HTTP

void setup() {

  Serial1.begin(9600);
  while (!Serial1) {// wait for serial port to connect. Needed for native USB port only
    ; 
  }

  Serial1.println("Starting Arduino web client.");





  // connection state
  bool connected = false;

  connectGPRS();
  Serial1.println("connecting...");


char PostData[] = "{\"data\": \"Fred\", \"age\": 31}";

findIP(server, serverIP, portServer);
//readGPRSResponse();

Serial1.println("Returned");
Serial1.println(serverIP);

/*

  // if you get a connection, report back via serial:
  if (client.connect(serverIP, port)) {
    Serial1.println("connected");
	sendPost(serverIP, PostData);

  } else {
    // if you didn't get a connection to the server:
    Serial1.println("connection failed");
  }
  */
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:

	//readGPRSResponse();
		// do nothing forevermore:
	//for (;;)
	//	;
}

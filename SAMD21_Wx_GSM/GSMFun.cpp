#include "GSMFun.h"
#include "arduino_secrets.h"

extern GSMClient client;
extern GPRS gprs;
extern GSM gsmAccess;

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;


void findIP(char *server, char *serverIP, int port) {
	Serial1.println(server);
	Serial1.println(port);

	if (client.connect(server, port)){
		Serial1.println("connected");

		client.print("GET ");
		client.print("/");
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(server);
		client.println("Connection: close");
		client.println();

	 }
	 else {
	 // if you didn't get a connection to the server:
	 Serial1.println("connection failed");
	  }
	
	int buffIdx = 0;
	char buff[] = "0000000";
	bool IPFind = false;
	bool PortFind = false;
	char idStr[] = "http://";
	char ipAddr[15];
	char ipPort[6];
	bool connected = true;
	char colonFind = 'a';

	while (connected) {
		if (client.available()) {
			char c = client.read();
			Serial1.print(c);

			if (strcmp(buff, idStr)) {
				IPFind = true;
			}
			else {
				memmove(buff, &(buff[1]), strlen(&(buff[1])));
				buff[7] = c;
				Serial1.println(buff);
			}

			if (PortFind) {
				ipPort[buffIdx] = c;
				buffIdx++;

			}

			if ((c == colonFind) & IPFind) {
				PortFind = true;
				buffIdx = 0;
			}

			if (IPFind) {
				ipAddr[buffIdx] = c;
				buffIdx++;
			}
		}

		// if the server's disconnected, stop the client:
		if (!client.available() && !client.connected()) {
			Serial1.println();
			Serial1.println("disconnecting.");
			connected = false;
			client.stop();

		}
	}

	strcpy(serverIP, "123.123.123.123");


}


void copyB(char *src, int len) {
	//char temp[7];
	for (int i = 1; i < len; i++) {
		*src++ = (*src)++ +1;
	}
}


void connectGPRS()
{
	// After starting the modem with GSM.begin()
	// attach the shield to the GPRS network with the APN, login and password  
	gsmAccess.setTimeout(15000);
	for (int ii = 0; ii < 10; ii++) {
		if (gsmAccess.begin() == GSM_READY) {
			break;
		}
		delay(1000);
		Serial1.println(ii);
	}

	Serial1.println("stage 2");

	for (int ii = 0; ii < 10; ii++) {
		if (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY) {
			break;
		}
		delay(1000);
		Serial1.println(ii);
	}

}

void sendPost(char *path, char *PostData)
{
	// Make a HTTP request:
	client.print("POST ");
	client.print(path);
	client.println(" HTTP/1.1");
	client.print("Host: ");
	client.println("68.230.107.205");
	client.println("Connection: close");
	client.println("Content-Type: application/json");
	client.print("Content-Length: ");
	client.println(strlen(PostData));
	client.println();
	client.println(PostData);
	client.println();

}

void readGPRSResponse()
{
	bool connected = true;
	while (connected) {
		//Serial1.println("1");
		if (client.available()) {
			char c = client.read();
			Serial1.print(c);
		}

		// if the server's disconnected, stop the client:
		if (!client.available() && !client.connected()) {
			Serial1.println();
			Serial1.println("disconnecting.");
			connected = false;
			client.stop();
			//for (;;)
			//	;

		}
		//delay(100);
	}
}
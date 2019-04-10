#include "GSMFun.h"
#include "arduino_secrets.h"
#include "GlobalTypes.h"

#include <RTCZero.h>

extern GSMBand band;
extern GSMClient client;
extern GPRS gprs;
extern GSM gsmAccess;
extern serverStruct serverData;
extern nativeData       sampledData;
extern storeDataUnion   MstrData;

extern void saveData();
extern volatile CntrlStruct        MstrCntrl;

GSMScanner scannerNetworks;
extern char JSONArray[2+(10+wxPackageSize*2)*15-1];
extern char preJSONArray[200][(wxPackageSize+2)*2];
extern void GSM_Disable();
extern void GSM_Enable();
extern void rtcSetGSMTime(time_t now);

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;


void findIP(char *server, char *serverIP, int port) {

	int buffIdx = 0;
	char idStr[] = "http://\0";
	char buff[] =  "0000000\0";
	bool IPFind = false;
	bool PortFind = false;

	char ipAddr[17] = "xxx.xxx.xxx.xxx\0";
	char ipPort[] = "000000\0";
	bool connected = true;
	char colonFind = ':';
	char c = 'a';
	bool foundData = true;
	bool done = false;

	// Make GET request to port 80
	if (client.connect(server, PORT_SERVER)){
		Serial1.println("connected");
		client.print("GET ");
		client.print("/getIP.php?d=123123");
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(server);
		//client.println("Connection: close");
		client.println();
	 }
	 else {
	 // if you didn't get a connection to the server:
	 Serial1.println("connection failed find");
	  }
		
	// Parse response
	while (connected) {
		if (client.available() && foundData) {
			c = client.read();		
		
			// Shift buffer and append right justified
			memmove(buff, &(buff[1]), strlen(&(buff[1])));
			buff[6] = c;

			// Step 5 - Done
			if (PortFind & (c == '/' )){
				memmove(&ipPort[buffIdx], "\0", 2);
				//ipPort[buffIdx] = "\0";
				foundData = false;
				done = true;
				break;
			}

			// Step 4 - Get port
			if (PortFind) {
				ipPort[buffIdx] = c;
				buffIdx++;
			}

			// Step 3 - done with IP, get port
			if ((c == colonFind) & IPFind) {
				memmove(&ipAddr[buffIdx], "\0",2);
				//ipPort[buffIdx] = "\0";
				PortFind = true;
				IPFind = false;
				buffIdx = 0;
			}

			// Step 2 - get IP
			if (IPFind) {
				ipAddr[buffIdx] = c;
				buffIdx++;
			}
			
			// Step 1 - find http
			if (!strcmp(buff, idStr)) {
				IPFind = true;
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
	// Save Data
	if (done){
		strcpy(serverData.serverIP,ipAddr);
		serverData.portIP = atoi(ipPort);
	}
}

void copyB(char *src, int len) {
	//char temp[7];
	for (int i = 1; i < len; i++) {
		*src++ = (*src)++ +1;
	}
}


void connectGPRS()
{
	// Set band if needed
	char bandResult[16];
	String bandName = band.getBand(); // Get and print band name
	Serial1.print("Current band:");
	Serial1.println(bandName);
	bandName.toCharArray(bandResult, 16);
	 
	 // Set Band if needed
	if (strcmp(bandResult, GSM_MODE_GSM850_PCS)){
		bool operationSuccess;
		operationSuccess = band.setBand(GSM_MODE_GSM850_PCS);
		if (operationSuccess) {
			Serial1.println("Success");
			} else {
			Serial1.println("Error while changing band");
		}
	}
	  
	 

	gsmAccess.setTimeout(15000);
	gsmAccess.lowPowerMode();
	
	int ok = 0;
	//gsmAccess.begin(NULL, true,false);
	bool notConnected = false;
		 
	for (int ii = 0; ii < 50; ii++) {
	//  while(notConnected)
		// {
			  
	if (gsmAccess.begin("") == GSM_READY) {
		break;
	}
	
	/*	gsmAccess.ready(); //Call this if debugging is on. Otherwise we will never reach GSM_READY...?!?
	delay(10000);
		ok = gsmAccess.status();
		if (ok != GSM_READY){
			Serial1.print(F("GSM status: "));
			Serial1.println(ok);
			}
		else {
			notConnected = false;
		}
		*/
	Serial1.println(ii);
	}


	//Serial1.println(ii);
	

	Serial1.println("stage 2");
	gprs.setTimeout(15000);
	for (int ii = 0; ii < 10; ii++) {
	if (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY) {
		serverData.serverConnection = true;
		break;
	}
	//	GSM_Disable();
	//	delay(100);
	//	GSM_Enable();
	delay(2000);
	Serial1.println(ii);
	}
	

	Serial1.println("stage 3");
}

void sendPost(char *PostData)
{

		
	// Make a HTTP request:
	if (client.connect(serverData.serverIP, PORT_IP)) {
		client.print("POST ");
		client.print(serverData.path);
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
	  } else {
	  // if you didn't get a connection to the server:
	  Serial1.println("connection failed");
  }

}

void readGPRSResponse()
{
	while (serverData.serverConnection) {
		//Serial1.println("1");
		if (client.available()) {
			char c = client.read();
			Serial1.print(c);
		}

		// if the server's disconnected, stop the client:
		if (!client.available() && !client.connected()) {
			Serial1.println();
			Serial1.println("disconnecting.");
			serverData.serverConnection = false;
			client.stop();
			//for (;;)
			//	;

		}
		//delay(100);
	}
}

void buildJSON_man()
{
	char tempData[wxPackageSize*2+1] = "1001283800C0FF01404ED20FFEFFFF";
	int numData = 1;
	char tempStr[3];
	const char hex[] = "0123456789abcdef";
	
	
	MstrData.weather.year = 1;
	MstrData.weather.month = 2;
	MstrData.weather.day = 3;
	MstrData.weather.hour = 4;
	MstrData.weather.minute = 5;
	MstrData.weather.second = 59;

	sampledData.dir = 13;
	sampledData.speed = 45.2;
	sampledData.gustMax = 2000;
	
	sampledData.temp = 34.5;
	sampledData.press = 1125.2;
	sampledData.humid = 52.6;
	sampledData.batteryV = 3.98;
	sampledData.batteryP = 98.7;

	MstrData.weather.signalRSSI = 12;
	MstrData.weather.LowPwrMode = 1;
	MstrData.weather.info = MstrCntrl.FRAM_reset_count % 16;
	MstrData.weather.id = WXSTATION_ID;
	
	MstrData.weather.accessCode = 3;
	
	
	
	saveData();
	
	
	char JSONDataStr[2+(10+wxPackageSize*2)*numData-1 + numData*2];
	//JSONDataStr = 0;
	//byte * pout = MstrData.TxData;
	//JSONDataStr[0] = '{';
	//JSONDataStr[1] = 0;	
	strcpy(JSONDataStr,"{");
	//strcat(JSONDataStr, "{");
	//for (int ii = 0; ii<MstrCntrl.FRAM_NumPoints; ii++){
		for (int ii = 0; ii<numData; ii++){
		
		for (int ii = 0; ii < wxPackageSize; ii++){
			tempData[ii*2] =  hex[MstrData.TxData[ii] & 0xF];
			tempData[ii*2+1] =  hex[(MstrData.TxData[ii] >> 4) & 0xF];
			//pout++;
		}
		tempData[wxPackageSize*2] = 0;
		
		strcat(JSONDataStr, "\"");
		sprintf(tempStr,"d%02d",ii);
		strcat(JSONDataStr, tempStr);
		strcat(JSONDataStr, "\"");
		strcat(JSONDataStr, ": ");
		strcat(JSONDataStr, "\"");
		strcat(JSONDataStr, tempData);
		strcat(JSONDataStr, "\"");
		
		if (ii<(numData-1)){
			strcat(JSONDataStr, ", ");
		}
	}
	strcat(JSONDataStr, "}");
	strcat(JSONDataStr, "\0");
	Serial1.print("true data - ");
	Serial1.println(JSONDataStr);
	strcpy(JSONArray,JSONDataStr);
}

void buildJSON()
{


	char tempStr[3];
	const char hex[] = "0123456789abcdef";

		
	char JSONDataStr[2+(10+wxPackageSize*2)*30-1 +  30*2];
	//JSONDataStr = 0;
	//byte * pout = MstrData.TxData;
	//JSONDataStr[0] = '{';
	//JSONDataStr[1] = 0;
	strcpy(JSONDataStr,"{");
		//strcat(JSONDataStr, "{");
		//for (int ii = 0; ii<MstrCntrl.FRAM_NumPoints; ii++){
		for (int ii = 0; ii<MstrCntrl.FRAM_NumPoints; ii++){
		
			strcat(JSONDataStr, "\"");
			sprintf(tempStr,"d%02d",ii);
			strcat(JSONDataStr, tempStr);
			strcat(JSONDataStr, "\"");
			strcat(JSONDataStr, ": ");
			strcat(JSONDataStr, "\"");
			strcat(JSONDataStr, preJSONArray[ii]);
			strcat(JSONDataStr, "\"");
			
			if (ii < MstrCntrl.FRAM_NumPoints-1){
				strcat(JSONDataStr, ", ");
			}
		}
	strcat(JSONDataStr, "}");
	strcat(JSONDataStr, "\0");
	Serial1.print("true data - ");
	Serial1.println(JSONDataStr);
	strcpy(JSONArray,JSONDataStr);
}


void getSignalStrength()
{
	String temp = "";
	temp = scannerNetworks.getSignalStrength();
	sampledData.rssi = temp.toInt();

	Serial1.print("Signal Strength: ");
	Serial1.print(temp);
	Serial1.println(" [0-31]");
}

void setFromGSMTime()
{
	time_t now;
	now = gsmAccess.getLocalTime();

	rtcSetGSMTime(now);

}
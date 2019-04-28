#include "GSMFun.h"
#include "arduino_secrets.h"
#include "GlobalTypes.h"

#define TINY_GSM_MODEM_UBLOX
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_SleepyDog.h>
#include "Adafruit_FRAM_I2C.h"
#include <StreamDebugger.h>
#define SerialMon Serial1

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial2

#include <RTCZero.h>

extern GSMBand band;
//extern GSMClient client;
extern GPRS gprs;
extern GSM gsmAccess;
extern serverStruct serverData;
extern nativeData       sampledData;
extern storeDataUnion   MstrData;

extern Adafruit_FRAM_I2C fram;


extern void saveData();
extern volatile CntrlStruct        MstrCntrl;
extern volatile RTCStruct    rtcTime;
extern uint32_t measTime_SeverTx;

GSMScanner scannerNetworks;
extern char JSONArray[2+(10+wxPackageSize*2)*200-1];
extern char preJSONArray[200][(wxPackageSize+2)*2];
extern char JSONDataStr[2+(10+wxPackageSize*2)*200-1 +  200*2];
extern void GSM_Disable();
extern void GSM_Enable();
extern void rtcSetGSMTime(time_t now);
extern void GPIO_dance();
extern void framReadData();
extern void rtcUpdateTime();
extern void resetWatchDog();
extern int gethex(char *string);
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;


const char server[] = "scottn.ddns.net";
const char resource[] = "/ardu2SQL.php?d=12312";
const int  port = 80;

StreamDebugger debugger(SerialAT, Serial1);
TinyGsm modem(debugger);
//TinyGsm modem(SerialAT);
TinyGsmClient client2(modem);
HttpClient http(client2, server, port);

const char apn[]  = "pwg";
const char user[] = "";
const char pass[] = "";

#define debug_GSM_EN 1

bool getRequest()
{
	// Make GET request to port 80
	if (client2.connect(server, PORT_SERVER)){
		Serial1.println("connected");
		client2.print("GET ");
		client2.print("/getIP.php?d=123123");
		client2.println(" HTTP/1.1");
		client2.print("Host: ");
		client2.println(server);
		client2.println("Connection: close");
		client2.println();
		return true;
	}
	else {
		// if you didn't get a connection to the server:
		Serial1.println("connection failed find");
		return false;
	}
	
}

bool findIP(char *server, char *serverIP, int port) {

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

	for (int ii = 0; ii < 5; ii++){
		if (getRequest()) break; 
		else return false;
	}
	delay(200);
	// Parse response
	while (connected) {
		if (client2.available() && foundData) {
			c = client2.read();		
		
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

		// if the server's disconnected, stop the client2:
		if (!client2.available() && !client2.connected()) {
			Serial1.println();
			Serial1.println("disconnecting.");
			connected = false;
			client2.stop();

		}
	}
	// Save Data
	if (done){
		strcpy(serverData.serverIP,ipAddr);
		serverData.portIP = atoi(ipPort);
		return true;
	}
	
	return false;
}



void copyB(char *src, int len) {
	//char temp[7];
	for (int i = 1; i < len; i++) {
		*src++ = (*src)++ +1;
	}
}
void setBand()
{
	
		// Set band if needed
		char bandResult[16];
		String bandName = band.getBand(); // Get and print band name
		Serial1.print("Current band:");
		Serial1.println(bandName);
		bandName.toCharArray(bandResult, 16);
		
		// Set Band if needed
		if (strcmp(bandResult, GSM_MODE_EGSM_DCS)){
			bool operationSuccess;
			operationSuccess = band.setBand(GSM_MODE_EGSM_DCS);
			if (operationSuccess) {
				Serial1.println("Success");
				} else {
				Serial1.println("Error while changing band");
			//	delay(100);
				//operationSuccess = band.setBand(GSM_MODE_EGSM_DCS);
			}
		}
		
	
}

bool connectGPRS()
{
	int numTries = 5;
	setBand();
	delay(50);

	bool connected = false;

	// After starting the modem with GSM.begin()
		// attach the shield to the GPRS network with the APN, login and password
	
		for (int ii = 0; ii < numTries; ii++){
				gsmAccess.setTimeout(10000);
			    gprs.setTimeout(10000);
				if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
				(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
					serverData.serverConnection = true;
					connected = true;
					Serial1.println("Connected");
					return true;
					} 
				else {
					Serial1.println("Not connected");
					serverData.serverConnection = false;
				}
			
					Serial1.print("GSM Retry- ");
					Serial1.println(ii);
		
	
		}
		
		return false;
}
bool connectGPRS_sep()
{
	int numTries = 5;
	setBand();
	delay(50);

	bool connected = false;

	// After starting the modem with GSM.begin()
	// attach the shield to the GPRS network with the APN, login and password
	gsmAccess.setTimeout(20000);

			
			
	for (int ii = 0; ii < numTries; ii++){

		if ((gsmAccess.begin(PINNUMBER) == GSM_READY) ) {
		break;
		}
		
		Serial1.print("GSM Retry- ");
		Serial1.println(ii);
	}
		gprs.setTimeout(20000);
	Serial1.println("Stage 22222222222");
	delay(100);
	
	for (int ii = 0; ii < numTries; ii++){
		if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY){
			
			serverData.serverConnection = true;
			connected = true;
			Serial1.println("Connected");
			return true;
		}
		else {
			Serial1.println("Not connected");
			serverData.serverConnection = false;
		}
		delay(100);
	}
	return false;
}

bool connectGPRS2()
{

	setBand();
	delay(100);
	gsmAccess.setTimeout(15000);
	//gsmAccess.lowPowerMode();
	
	int ok = 0;
	bool notConnected = false;
	gsmAccess.begin(NULL, true,false);
	
		 Serial1.println("Do begin");
	for (int ii = 0; ii < 25; ii++) {

	
		gsmAccess.ready(); //Call this if debugging is on. Otherwise we will never reach GSM_READY...?!?
		
		ok = gsmAccess.status();
		Serial1.println(ok);
		if (ok == GSM_READY){
			Serial1.print(F("GSM status: "));
			Serial1.println(ok);
			ii = 25;
			break;
			}
		else {
			notConnected = false;
			return false;
		}
		delay(1000);
	Serial1.println(ii);
	}

	resetWatchDog();
	//Serial1.println(ii);
	

	Serial1.println("stage 2");
	gprs.setTimeout(15000);
	gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD, false);
	for (int ii = 0; ii < 100; ii++) {
		if (gprs.status() != GPRS_READY) {
			serverData.serverConnection = true;
			Serial1.println("Connected GPRS");
			return true;
			break;
		}
		
		delay(200);
		Serial1.println(ii);
	}
	
	return false;
}


void connectGPRSAsync()
{
	setBand();
	delay(50);
	  
	 
	
	gsmAccess.setTimeout(15000);
	//gsmAccess.lowPowerMode();
	
	int ok = 0;
	//gsmAccess.begin(NULL, true,false);
	bool notConnected = false;
		 Serial1.println("Do begin");
	
	
	gsmAccess.begin("",false,true);
	}
	
bool connectGPRSAsyncCheck()
{	
	GSM3_NetworkStatus_t ok;
	gsmAccess.ready(); //Call this if debugging is on. Otherwise we will never reach GSM_READY...?!?
	
		//ok = gsmAccess.status();
		if (ok != GSM_READY){
			Serial1.print(F("GSM status: "));
			Serial1.println(ok);
			return true;
			}
		else {
			return false;
		}
	}


void attachGPRSAsync()
{

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



bool connectGPRS_tiny()
{
	
	bool connected = true;
	
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
	  
	  SerialMon.print(F("Waiting for network..."));
	  if (!modem.waitForNetwork()) {
		  SerialMon.println(" fail");
		  return false;
	  }
	  SerialMon.println(" OK");

	  SerialMon.print(F("Connecting to "));
	  SerialMon.print(apn);
	  if (!modem.gprsConnect(apn, user, pass)) {
		  SerialMon.println(" fail");
		  return false;
	  }
	  SerialMon.println(" OK");
	  int csq = modem.getSignalQuality();
	  SerialMon.print("********** ");
	  SerialMon.println(csq);
	

	Serial1.println("stage 3");
	
	
	SerialMon.print(F("Performing HTTP GET request... "));
	// int err = http.get(resource);
	//if (err != 0) {
	//  SerialMon.println(F("failed to connect"));
	//   delay(10000);
	//   return;
	//}
	
	return true;

}





bool connectGPRS4()
{
	
	bool connected = true;
	
	setBand();
	delay(50);
	
	  
	  SerialMon.print(F("Waiting for network..."));
	  if (!modem.waitForNetwork()) {
		  SerialMon.println(" fail");
		  delay(10000);
		  return false;
	  }
	  SerialMon.println(" OK");

	  SerialMon.print(F("Connecting to "));
	  SerialMon.print(apn);
	  if (!modem.gprsConnect(apn, user, pass)) {
		  SerialMon.println(" fail");
		  return false;
	  }
	  SerialMon.println(" OK");

	

	Serial1.println("stage 3");
	serverData.serverConnection = true;
	


//	modem.gprsDisconnect();
	//SerialMon.println(F("GPRS disconnected"));
	 return true;
}

bool sendPost(char *PostData)
{

		Serial1.print("Post Req port - ");
		Serial1.println(serverData.portIP);
	// Make a HTTP request:
//	if (client2.connect(serverData.serverIP, PORT_IP)) {
		if (client2.connect("68.230.107.205",5211)) {
		client2.print("POST ");
		client2.print(serverData.path);
		client2.println(" HTTP/1.1");
		client2.print("Host: ");
		client2.println("68.230.107.205");
		client2.println("Connection: close");
		client2.println("Content-Type: application/json");
		client2.print("Content-Length: ");
		client2.println(strlen(PostData));
		client2.println();
		client2.println(PostData);
		client2.println();
		serverData.serverConnection = true;
		return true;
	  } else {
	  // if you didn't get a connection to the server:
	  Serial1.println("connection failed");
	  return false;
  }

/*
Serial1.println("Content-Type: application/json");
		Serial1.print("Content-Length: ");
		Serial1.println(strlen(PostData));
		Serial1.println();
		Serial1.println(PostData);
		Serial1.println();
*/
}

bool readGPRSResponse()
{
	
	int buffIdx = 0;
	char idStr2[] = "Status: \0";
	char buff2[] =  "XXXXXXXX\0";
	char idStr[] =  "Command: \0";
	char buff[] =   "XXXXXXXXX\0";
	char stsVal[5] = "xxx\0";
	char cmdVal[5] = "xxx\0";
	bool CmdFind = false;
	bool StsFind = false;
	bool DoneCmd = false;
	char colonFind = ':';
	int stsVal_int = 0;
	int cmdVal_int = 0;
	int intVal_int = 0;
	int rebootVal_int = 0;
	
	int timeCalc = 0;
	
	
	while (serverData.serverConnection) {
		//Serial1.println("1");
		if (client2.available()) {
			char c = client2.read();
			Serial1.print(c);
			
			if (!DoneCmd){
				// Shift buffer and append right justified
				memmove(buff, &(buff[1]), strlen(&(buff[1])));
				buff[8] = c;
				memmove(buff2, &(buff2[1]), strlen(&(buff2[1])));
				buff2[7] = c;

				// Step 3 - 
				if ((c == ',') & CmdFind) {
					memmove(&cmdVal[buffIdx], "\0",2);
					//ipPort[buffIdx] = "\0";
					DoneCmd = true;
					CmdFind = false;
					buffIdx = 0;
					client2.stop();
				//	memmove(cmdVal,  "xxx\0",5);
				}
				
				if ((c == ',') & StsFind) {
					memmove(&stsVal[buffIdx], "\0",2);
					//ipPort[buffIdx] = "\0";
					StsFind = false;
					buffIdx = 0;
				//	memmove(stsVal,  "xxx\0",5);
				}

				// Step 2 - get val
				if (CmdFind) {
					cmdVal[buffIdx] = c;
					buffIdx++;
				}
				// Step 2 - get val
				if (StsFind) {
					stsVal[buffIdx] = c;
					buffIdx++;
				}
			
				// Step 1 - find command
				if (!strcmp(buff, idStr)) {
					CmdFind = true;
				}
				// Step 1 - find command
				if (!strcmp(buff2, idStr2)) {
					StsFind = true;
				}
			}
		}
			
	

		// if the server's disconnected, stop the client2:
		if (!client2.available() && !client2.connected()) {
			Serial1.println();
			Serial1.println("disconnecting.");
			serverData.serverConnection = false;
			client2.stop();
			//for (;;)
			//	;

		}
	}
	stsVal_int = gethex(stsVal);
	cmdVal_int = gethex(cmdVal);
	intVal_int = (cmdVal_int & 0xFF0) >> 4;
	rebootVal_int = (cmdVal_int & 0xF);
	
	Serial1.print("Status is: ");
	Serial1.println(stsVal_int);
	Serial1.print("Command is: ");
	Serial1.println(cmdVal_int);
	Serial1.print("Int command is: ");
	Serial1.println(intVal_int);
	
	Serial1.print("Reboot command is: ");
	Serial1.println(rebootVal_int);
		
	
	if(stsVal_int != 0x84){
				Serial1.println("----------Bad Status");
		return false;
	}
	
	
	//if (((int)strtol(cmdVal,NULL,16) & 0xF) == MstrCntrl.sCmd_Unlock){
		
		if (rebootVal_int == 7){
			Watchdog.enable(1000);
		}
		
		if (sampledData.batteryP > 20){
			if((intVal_int >= 2)  && (intVal_int <= 30)  && (intVal_int != 15)){
				
				timeCalc = ((int)rtcTime.currTime.hour - MstrCntrl.sCmd_t_hours)*60 + ((int)rtcTime.currTime.min  - MstrCntrl.sCmd_t_mins);
				if ((timeCalc > 60) || (timeCalc < -60)){
					MstrCntrl.serverInterval = intVal_int;
					MstrCntrl.sCmd_t_hours = rtcTime.currTime.hour;
					MstrCntrl.sCmd_t_mins =  rtcTime.currTime.min;
				}
			}
		}
		
	//}
	
	return true;
	
}



void buildJSON()
{
	char tempStr[3];
	const char hex[] = "0123456789abcdef";

	strcpy(JSONDataStr,"{");
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
	int csq = modem.getSignalQuality();
	sampledData.rssi = csq;

	Serial1.print("Signal Strength: ");
	Serial1.print(csq);
	Serial1.println(" [0-31]");
}

void setFromGSMTime()
{
	time_t now;
	now = gsmAccess.getLocalTime();

	rtcSetGSMTime(now);

}

bool sendToServer2(bool findIPFlag)
{
	volatile bool status =  false;
	volatile bool statusIP =  false;
	bool globalStatus = false;
	bool gsmTxCount = false;
	bool gsmTxStatus = false;
	bool sendDataFlag = false;
	int errorCount = 0;
	
	globalStatus = false;
	#if debug_GSM_EN
		if (digitalRead(GSM_RESETN)){
			GSM_Enable();
			delay(9000);
		}
			
		framReadData();
		if(MstrCntrl.FRAM_NumPoints > 0) {
			buildJSON();
			sendDataFlag = true;
		}
	
		enum StateDef {ConnectingState = 0, findIPState = 1, updateTimeSignalState = 3, sendPostState = 4, readState = 5, eraseData = 6};
		enum StateDef State = ConnectingState;
	
		while (errorCount <= 5){
			switch(State){
				case ConnectingState:
				
					status = connectGPRS_tiny();
					if (status && findIPFlag) State = findIPState;
					else if (status) State = updateTimeSignalState;
					else errorCount++;
					break;

				case findIPState:
					if (!statusIP) statusIP = findIP(serverData.server, serverData.serverIP, PORT_IP);
					if (statusIP) State = updateTimeSignalState;
					else State = ConnectingState;
					break;
				
				case updateTimeSignalState:
					getSignalStrength();
					setFromGSMTime();
					resetWatchDog();
					if (sendDataFlag) State = sendPostState;
					else State = eraseData;
					break;
				
				case sendPostState:
					status = sendPost(JSONArray);
					if (status) State = readState;
					else State = ConnectingState;
					break;
				
				case readState:
					delay(200);
					gsmTxStatus = readGPRSResponse();
					modem.gprsDisconnect();
					if (gsmTxStatus) State = eraseData;
					else State = sendPostState;
					break;
				
				case eraseData:
					
					GPIO_dance();
					GPIO_dance();
					GPIO_dance();
					fram.write8(FRAM_NUM_P0INTS_Addr, 0);
					fram.write8(FRAM_MEM_POINTER_Addr, 0);
					errorCount = 99;//Done
					globalStatus = true;
					break;
				
				default:
					State = ConnectingState;
					break;
			}
		}
	#else
		GPIO_dance();
		GPIO_dance();
		GPIO_dance();
		globalStatus = true;
		fram.write8(FRAM_NUM_P0INTS_Addr, 0);
		fram.write8(FRAM_MEM_POINTER_Addr, 0);
	#endif
	
				
	GSM_Disable();
				
	// Reset global array
	JSONArray[0] = 0;
				
	// Set new time
	rtcUpdateTime();
	if(globalStatus){
		measTime_SeverTx = (rtcTime.currTime.min + MstrCntrl.serverInterval) % 60;
		return true;
	}
	else{
		measTime_SeverTx = (rtcTime.currTime.min + 1) % 60;
		return false;
	}
}
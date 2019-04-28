#pragma once

#include <Arduino.h>
#include <MKRGSM.h>
#include "arduino_secrets.h"




bool findIP(char *server, char *serverIP, int port);
void setBand();
bool connectGPRS();
bool connectGPRS2();
bool connectGPRS4();
void connectGPRSAsync();
bool connectGPRSAsyncCheck();
void attachGPRSAsync();
bool sendPost(char *PostData);
bool readGPRSResponse();
void buildJSON_man();
void buildJSON();
void copyB(char *src, int len);
void getSignalStrength();
void setFromGSMTime();
bool sendToServer(bool findIPFlag);
bool sendToServer2(bool findIPFlag);
bool getRequest();
bool connectGPRS_sep();
bool connectGPRS_tiny();
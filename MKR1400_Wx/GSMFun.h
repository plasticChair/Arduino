#pragma once

#include <Arduino.h>
#include <MKRGSM.h>
#include "arduino_secrets.h"



void findIP(char *server, char *serverIP, int port);
void connectGPRS();
void sendPost(char *PostData);
void readGPRSResponse();
void buildJSON_man();
void buildJSON();
void copyB(char *src, int len);
void getSignalStrength();
void setFromGSMTime();
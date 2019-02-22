#pragma once

#include <Arduino.h>
#include <MKRGSM.h>
#include "arduino_secrets.h"



void findIP(char *server, char *serverIP, int port);
void connectGPRS();
void sendPost(char *path, char *PostData);
void readGPRSResponse();

void copyB(char *src, int len);
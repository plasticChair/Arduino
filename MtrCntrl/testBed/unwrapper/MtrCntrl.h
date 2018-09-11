#ifndef MTRCNTRL_H
#define MTRCNTRL_H


#include "MtrCntrlClass.h"


/* #defines */
int ControlRate     = 1000.0;   // 1000 Hz, microseconds
float HeartBeatRate = 1e6;  // 1 Hz, microseconds
int TelemetryRate   = 5000; // 500 Hz, microseconds

int EN_GPIO         = 5;
int PWM2_GPIO       = 9;
int PWM1_GPIO       = 8;
int LED_GPIO        = 13;

/*
 * tq   0     
 * cnts 1023
 * 
 */


/* Function Prototypes */
void teleExec();
void mtrExec();
void heartBeat();

/* Globals */
volatile int printFlag = 0;
int numRxBytes         = 0;


volatile int bufferHeadIdx = 0;
volatile int bufferTailIdx = 0;
volatile int bufferIdx     = 0;




  #endif

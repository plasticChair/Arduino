#ifndef MTRCNTRL_H
#define MTRCNTRL_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"
#include "fastADC.h"
#include "TimerFour.h"
#include "TimerFive.h"
#include "MtrCntrlClass.h"
#include "mtrPosUnwrap.h"
#include <math.h>

/* #defines */
int ControlRate     = 1000.0;   // 1000 Hz, microseconds
float HeartBeatRate = 1e6;  // 1 Hz, microseconds
int TelemetryRate   = 20000; // 500 Hz, microseconds

int EN_GPIO         = 5;
int PWM2_GPIO       = 9;
int PWM1_GPIO       = 8;
int LED_GPIO        = 13;
int forceCell_GPIO  = A1;
int potMtr_GPIO     = A9;
int potHan_GPIO     = A8;

/*
 * tq   0     
 * cnts 1023
 * 
 */


/* Function Prototypes */
void teleExec();
void mtrExec();
void heartBeat();
void printMe();

/* Globals */
volatile int printFlag = 0;
int numRxBytes         = 0;
byte serialBuffer[64];

volatile int bufferHeadIdx = 0;
volatile int bufferTailIdx = 0;
volatile int bufferIdx     = 0;




/* -- Unions of Serial Buffer -- */
union {
  byte  ID;    // 1 byte
  int   Data1;  // 2 bytes
  int   Data2;  // 2 bytes
  unsigned char bytes[5];
  } serialRxBuf;

union {
  int   val1;  // 2 bytes
  int   val2;  // 2 bytes
  float val3;  // 4 bytes
  unsigned char bytes[8];
  } serialTxBuf;

  #endif

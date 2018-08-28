#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"
#include "fastADC.h"
#include "TimerThree.h"
#include "TimerFour.h"
#include "TimerFive.h"

/* #defines */
#define cntrlRate 1000 // microseconds
#define EN_GPIO 46
#define PWM2_GPIO 3 
#define PWM1_GPIO 2
#define LED_GPIO 13

#define potMtr A0
#define potHan A1


class MotorControl
{
  public:

   
    volatile int potMtrCnts      = 0;
    volatile int potHanCnts      = 0;
    volatile float mtrVel_cntsps = 0;

    int pwmCmdPrev;
    int pwmCmdCurr;

    char printData[50];
    int printFlag = 0;
    TimerThree* _Timer3;
    
    // methods
    void init(TimerThree* Timer3);
    void readSensor();
    void execCntrl();
    void updatePWM();


};



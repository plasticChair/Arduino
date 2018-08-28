#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"
#include "TimerThree.h"

extern int EN_GPIO;
extern int PWM2_GPIO;
extern int PWM1_GPIO;
extern int LED_GPIO;
extern int forceCell_GPIO;  
extern int potMtr_GPIO;
extern int potHan_GPIO;
extern int ControlRate;


class MotorControl
{
  public:
    // Pre Calcs
    float CNTPS2RPM = (60/1023)/(ControlRate*1e-6);
    
    // Thresholds
    int WRAP_THRESHOLD = 200;

    // Intermediate variables
    int wrapOffset = 0;
    int diffCounts = 0;

    // ADCs counts
    volatile int potMtrCnts      = 0;
    volatile int potHanCnts      = 0;
    volatile int forceCellCnts   = 0;

    // Calculated values
    volatile float mtrVel_cntsps = 0;

    // Command Variables
    int pwmCmdPrev;
    int pwmCmd;

    char printData[50];
    int printFlag = 0;
    TimerThree* _Timer3;
    
    // methods
    void Init(TimerThree* Timer3);
    void readSensor();
    void execCntrl();
    void updatePWM();
};

/* ---------------------------------*/
/* ----------- Timer 4--------------*/ 
/* ---------------------------------*/
//
//void initTimer4()
//{
//  unsigned char clockSelectBits;
//
//  TCCR4A = 0;
//  TCCR4B = 0;
//  TCNT4  = 0;
//
//  OCR4A = 15624;            //1 Hz, fclock/(prescaler * rate) - 1
//  TCCR4B |= (1 << WGM42);   // CTC mode
//  TCCR4B |= (1 << CS40) | (1 << CS42);    // 256 prescaler 
//  TIMSK4 |= (1 << OCIE4A);  // enable timer compare interrupt
//
//}
/* ISR for Timer 4 */
//ISR(TIMER4_COMPA_vect)          // timer compare interrupt service routine
//{
//  printFlag = 1;
 // PORTB ^= _BV(PB5);  // Toggle pin 13
//  digitalWrite(LED_GPIO, digitalRead(LED_GPIO) ^ 1);
//}

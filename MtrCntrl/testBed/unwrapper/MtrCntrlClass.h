
#include "math.h"
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
    float CNTPS2RPM = (60.0/1023.0)/(ControlRate*1.0e-6);

 
    // ADCs counts
    volatile int potMtrCnts      = 0;
    volatile int potHanCnts      = 0;
    volatile int forceCellCnts   = 0;

    // Calculated values
    volatile float mtrVel_rpm = 0;

    // Command Variables
    int pwmCmdPrev = 0;
    int pwmCmd = 0;

    char printData[50];
    int printFlag = 0;

    
    // methods
    void Init();
    void readSensor(int varHere);
    void execCntrl();
    void updatePWM();
	int getPosCnts();
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

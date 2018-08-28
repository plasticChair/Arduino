#include "MtrCntrlClass.h"


/* Function Prototypes */
void initTimer4();
void mtrExec();

/* Globals */
int serialDataIn       = 0;
int serialDataInPrev   = 0;
int newPWM             = 0;
volatile int printFlag = 0;
volatile bool bufferNotEmpty = 0;
volatile char serialBuffer[255];
volatile int bufferHeadIdx = 0;
volatile int bufferTailIdx = 0;
volatile int bufferIdx = 0;

/* Classes */
MotorControl mtrCntrl;
//TimerThree Timer3; 
//TimerFour Timer4; 
//TimerFive Timer5; 

union {
  int val1;  // 2
  int val2;  // 2
  float val3; // 4
  unsigned char bytes[8];
  } serialTxBuf;


void setup() {
  Serial.begin(115200);

  bufferNotEmpty = 0;
  /* GPIO Set */
  pinMode(LED_GPIO, OUTPUT);
  pinMode(EN_GPIO, OUTPUT);
  pinMode(PWM2_GPIO, OUTPUT);
  pinMode(PWM1_GPIO, OUTPUT);

  // ----- Init Timers ---------//
//  Timer4.initialize(1e8);
 // Timer4.attachInterrupt(timer4Exec, 1e8);

  Timer5.initialize(1e6);
  Timer5.attachInterrupt(heartBeat, 1e6);
  
  //Timer3.initialize(cntrlRate);
 // Timer3.attachInterrupt(mtrExec, cntrlRate);
 // Timer3.pwm(EN_GPIO, 0, cntrlRate);

  
  // ----- Init Motor Controller ---------//
//  mtrCntrl.init(&Timer3);
  Serial.println(" Init complete! ");
   
}

void loop() {

  // ----- Check for new commands via serial ---------//
  if (Serial.available()) {      // If anything comes in Serial (USB),
    mtrCntrl.pwmCmdCurr = Serial.parseInt();              // read it and send it out Serial1 (pins 0 & 1)
  }

  // ----- Print data based on Timer 4 ISR ---------//
  if (printFlag == 1){
    //Serial.println(TCNT3);
    Serial.print(mtrCntrl.potMtrCnts);
    Serial.print("      ");
    Serial.println(mtrCntrl.potHanCnts);
    printFlag = 0;
  }

 // if (mtrCntrl.printFlag == 1){
//    mtrCntrl.printFlag = 0;
 //   Serial.println(mtrCntrl.printData);
 // }

 // if(bufferHeadIdx != bufferIdx){
  //  Serial.write(serialBuffer[bufferHeadIdx:bufferHeadIdx+8], 8);
 //   bufferHeadIdx = bufferHeadIdx +8;     
 // }
   
}


/* ---------------------------------*/
/* ----------- ISR    --------------*/ 
/* ---------------------------------*/
void mtrExec()
{
    mtrCntrl.readSensor();
    mtrCntrl.execCntrl();
    mtrCntrl.updatePWM();
    
//    serialTxBuf.val1 = potMtrCnts; // 2 bytes
//    serialTxBuf.val2 = potHanCnts; // 2 bytes
//    serialTxBuf.val3 = mtrVel_cntsps; //4 bytes

    
//    memcpy(serialBuffer[bufferIdx:bufferIdx+8], serialTxBuf.bytes,8);
//    bufferIdx = bufferIdx + 8;
}


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

void heartBeat()
{
  digitalWrite(LED_GPIO, digitalRead(LED_GPIO) ^ 1);
  printFlag = 1;
}










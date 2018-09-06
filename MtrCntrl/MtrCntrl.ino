#include "MtrCntrl.h"



/* Classes */
MotorControl mtrCntrl;
TimerFive heartBeatTimer; 
TimerFour teleTimer;
TimerThree cntrlTimer;

void setup() {
  Serial.begin(115200);

  /* GPIO Set */
  pinMode(LED_GPIO, OUTPUT);
  pinMode(EN_GPIO, OUTPUT);
  pinMode(PWM2_GPIO, OUTPUT);
  pinMode(PWM1_GPIO, OUTPUT);

  // ----- Init Timers ---------//
   /* -- 1 Hz -- */
  heartBeatTimer.initialize();
  heartBeatTimer.attachInterrupt(heartBeat, HeartBeatRate);
  
  /* -- 500 Hz -- */
  teleTimer.initialize();
  teleTimer.attachInterrupt(teleExec, TelemetryRate);

  /* -- 1000 Hz -- */
  cntrlTimer.initialize();
  cntrlTimer.attachInterrupt(mtrExec, ControlRate);
  cntrlTimer.pwm(EN_GPIO, 0, ControlRate);

  // ----- Init Motor Controller ---------//
  mtrCntrl.Init(&cntrlTimer);
  Serial.println(" Init complete! ");
   
}

void loop() {

  // ----- Check for new commands via serial ---------//
  if (Serial.available()) {     
    mtrCntrl.pwmCmd = Serial.parseInt();          
   // numRxBytes = Serial.readBytesUntil('\n',serialBuffer,64);
  }
  //mtrCntrl.pwmCmd = 3000;
  // ----- Print data based on Timer 4 ISR ---------//
  if (printFlag == 1){
    printMe();
    printFlag = 0;
  }
}



void mtrExec()
{
    mtrCntrl.readSensor();
    mtrCntrl.execCntrl();
    mtrCntrl.updatePWM();
}


void teleExec()
{
  printFlag = 1;
 // memcpy(serialRxBuf.bytes, serialBuffer,numRxBytes);

 // if (serialRxBuf.ID == 1){
  //  mtrCntrl.pwmCmd = serialRxBuf.Data1;
  //}
}


void heartBeat()
{
  //PORTB ^= _BV(PB5);

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void printMe()
{
  
    Serial.print(mtrCntrl.potMtrCnts);
    Serial.print("    ");
    Serial.print(mtrCntrl.forceCellCnts);
    Serial.print("    ");
    Serial.print(mtrCntrl.diffCounts);
    Serial.print("    ");
    Serial.println(mtrCntrl.mtrVel_rpm);

    
//    Serial.print("");
//    Serial.println(mtrCntrl.potHanCnts); 


// if(bufferHeadIdx != bufferIdx){
  //  Serial.write(serialBuffer[bufferHeadIdx:bufferHeadIdx+8], 8);
 //   bufferHeadIdx = bufferHeadIdx +8;     
 // }
  
}

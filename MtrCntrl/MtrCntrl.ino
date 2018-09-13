#include "MtrCntrl.h"


unsigned long timeNow;
unsigned long timePrev;
unsigned long diffTime;
unsigned long timeLater;

/* Classes */
MotorControl mtrCntrl;
mtrPosUnwrap snsfFilter;

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
  snsfFilter.setFilter(1);
  snsfFilter.setThreshold(20);
  snsfFilter.init(mtrCntrl.getPosCnts());
  
  Serial.println(" Init complete! ");
   
}

void loop() {

  // ----- Check for new commands via serial ---------//
  if (Serial.available()) {     
    mtrCntrl.cmdCnts = Serial.parseInt();    
        
   // numRxBytes = Serial.readBytesUntil('\n',serialBuffer,64);
  }

  
  
  
  //mtrCntrl.pwmCmd = 3000;
  // ----- Print data based on Timer 4 ISR ---------//
  if (printFlag == 1){
    printMe();
   // Serial.println(diffTime);
    //Serial.println(timeNow - timeLater);
    
    printFlag = 0;
  }
}



void mtrExec()
{

  timeNow = micros();
    mtrCntrl.readSensor();

    snsfFilter.unwrapAlgo(mtrCntrl.getPosCnts());
    mtrCntrl.setPosCnts(snsfFilter.getPosCounts());

    mtrCntrl.mtrVel_rpm = ((float)snsfFilter.diffCounts)*mtrCntrl.CNTPS2RPM;
    mtrCntrl.execCntrl();
    mtrCntrl.updatePWM();

   timeLater = micros();
   
   //diffTime = timeNow - timePrev;
   diffTime = timeLater  - timeNow;
   timePrev = timeNow;


    
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
  
//    Serial.print(snsfFilter.augMtrPos);
//    Serial.print("    ");
//    Serial.print(snsfFilter.sampledMtrPos);
//    Serial.print("    ");
//    Serial.print(snsfFilter.predictMode_crit_1);
//    Serial.print("    ");
//    Serial.print(snsfFilter.predictMode_crit_2);
//    Serial.print("    ");
//    Serial.print(snsfFilter.predictMode_crit_3);
//    Serial.print("    ");
//    Serial.print(snsfFilter.predictMode_crit_4);
//    Serial.print("    ");
//    Serial.print(snsfFilter.predictMode_crit_5);
//    Serial.print("    ");
//    Serial.print(snsfFilter.PREDICT);
//    Serial.print("    ");
//    Serial.print(snsfFilter.TRACKING);
//    Serial.print("    ");
//    Serial.print(snsfFilter.AQRTRACK);
//    Serial.print("    ");


    
    Serial.println(mtrCntrl.sampledMtrPos);

    
//    Serial.print("");
//    Serial.println(mtrCntrl.potHanCnts); 


// if(bufferHeadIdx != bufferIdx){
  //  Serial.write(serialBuffer[bufferHeadIdx:bufferHeadIdx+8], 8);
 //   bufferHeadIdx = bufferHeadIdx +8;     
 // }
  
}

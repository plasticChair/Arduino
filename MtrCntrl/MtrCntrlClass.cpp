#include "MtrCntrlClass.h"
#include <avr/io.h>
#include <avr/interrupt.h>



void MotorControl::Init(TimerThree* Timer3)
{

  _Timer3 = Timer3;
  digitalWrite(PWM2_GPIO, HIGH);
  digitalWrite(PWM1_GPIO, LOW);

  readSensor();
  pwmCmdPrev = 0;
  pwmCmd = 0;

  readSensor();
  updateCurSector();
  updatePrev();

  
}

void MotorControl::readSensor()
{
  potMtrCnts    = analogRead(potMtr_GPIO);
  potHanCnts    = analogRead(potHan_GPIO);
  forceCellCnts = analogRead(forceCell_GPIO);

  sampledMtrPos = potMtrCnts;
  
}


void MotorControl::execCntrl()
{
    
}

void MotorControl::updatePWM()
{  
  if (pwmCmd != pwmCmdPrev){
    pwmCmdPrev = pwmCmd;
    //printFlag = 1;
     if (pwmCmd == 0)    { // Turn off if 0
          _Timer3->setPwmDuty(EN_GPIO, 0);
        }
        else if (pwmCmd > 0) { // Positive direction
          digitalWrite(PWM2_GPIO, HIGH);
          digitalWrite(PWM1_GPIO, LOW);
          _Timer3->setPwmDuty(EN_GPIO, pwmCmd);
        }
        else if (pwmCmd < 0) { // Negative direction
          digitalWrite(PWM2_GPIO, LOW);
          digitalWrite(PWM1_GPIO, HIGH);
          _Timer3->setPwmDuty(EN_GPIO, pwmCmd);
        }        
        else {
           _Timer3->setPwmDuty(EN_GPIO, 0);
        }
    }
}

void MotorControl::unwrapAlgo()
{
  updateCurSector();
  updateMode();

  if (TRACKING)
    TRACKmode();

  if (PREDICT)
    PREDICTmode();

  if (AQRTRACK)
    AQRTRACKmode();

  limitMtrPos();
  updateUnwrapPos();
  updatePrev();

  

}



void MotorControl::updateCurSector()
{
  curSector = ceil(sampledMtrPos / sizeSector);
}

void MotorControl::updateMode()
{
  int temp;
  
  int tempdiffCounts = 0;
  bool predictMode_crit_1;
  bool predictMode_crit_2;
  bool predictMode_crit_3;
  bool predictMode_crit_4;
  bool predictMode_crit_5;
  bool predictMode_crit;

  bool TRACKING_crit;


  tempdiffCounts = diffCounts;

  predictMode_crit_1 = (sampledMtrPos == 0);
  predictMode_crit_2 = (sampledMtrPos == 1023);
  temp = abs(curSector - prevSector);

  wrapCheck();
  
  predictMode_crit_3 = (temp > 1) && ( wrapChecked == 3);
  posWrapper();
  predictMode_crit_4 = abs(diffCounts) > WRAP_THRESHOLD;

  predictMode_crit_5 = (tempdiffCounts - diffCounts) > WRAP_THRESHOLD;
  predictMode_crit = predictMode_crit_1 || predictMode_crit_2 || predictMode_crit_3 || predictMode_crit_4 || predictMode_crit_5;

  TRACKING_crit = (trackingAck) > 2;

  diffCounts = tempdiffCounts;

  if (predictMode_crit) {
    PREDICT = 1;
    trackingAck = 0;
    trackingAckPrev = 0;
    TRACKING = 0;
    AQRTRACK = 1;
  }

  if (TRACKING_crit) {
    PREDICT = 0;
    TRACKING = 1;
    AQRTRACK = 0;
    trackingAck = 0;
    potMtrCntsPrev = trackingAckPrev;
  }

}

void MotorControl::TRACKmode()
{
  posWrapper();

  if (predictModePrev){
    diffCounts = diffCountsPrev;
  }


  if (abs(diffCounts) > WRAP_THRESHOLD) {
  PREDICT = 1;
  }

}

void MotorControl::PREDICTmode()
{
  sampledMtrPos = (potMtrCntsPrev + diffCountsPrev) % 1023;
  updateCurSector();
  posWrapper();
}

void MotorControl::AQRTRACKmode()
{
  int tempVar = sampledMtrPos - trackingAckPrev;
  if (abs(tempVar) < WRAP_THRESHOLD) {
    trackingAck = trackingAck + 1;
  }
  else {
    trackingAck = 0;
    //lostTrackSample = sampledMtrPos;
  }
  
  trackingAckPrev = sampledMtrPos;
}

void MotorControl::limitMtrPos()
{
  if (diffCounts > 0) {
    diffCounts = min(diffCounts, WRAP_THRESHOLD);
  }
  else if(diffCounts < 0) {
    diffCounts = max(diffCounts, -WRAP_THRESHOLD);
  }
  
}

void MotorControl::updateUnwrapPos()
{
  mtrPosCntsUnwrapped = sampledMtrPos + wrapCnt * 1023;
}

void MotorControl::updatePrev()
{
  predictModePrev = PREDICT;
  diffCountsPrev = diffCounts;
  potMtrCntsPrev = sampledMtrPos;
  prevSector = curSector;
}

void MotorControl::wrapCheck()
{
  if (((curSector == 1) || (curSector == 0)) && ((prevSector == (numSectors - 1)) || (prevSector == (numSectors)))) {
    wrapChecked = 1; // positive
  }
  else if (((curSector == (numSectors - 1)) || (curSector == (numSectors))) && ((prevSector == 0) || (prevSector == 1))) {
    wrapChecked = 2; // neg
  }
  else {
    if ((prevSector == 1) || (prevSector == numSectors)) {
      wrapChecked = 3; // edge sectors
    }
    else {
      wrapChecked = 0;
    }
  }
}

void MotorControl::posWrapper()
{
  int wrapOffset;

  if (wrapChecked == 1) {
    wrapOffset = 1023 - potMtrCntsPrev;
    diffCounts = (sampledMtrPos + wrapOffset);
    wrapCnt = wrapCnt + 1;
    // Wrap check up
  }
  else if (wrapChecked == 2) {
    wrapOffset = 1023 - sampledMtrPos;
    diffCounts = -(wrapOffset + potMtrCntsPrev);
    wrapCnt = wrapCnt - 1;
  }
  else {
    diffCounts = sampledMtrPos - potMtrCntsPrev;
  }



}


void MotorControl::setPos(int positionVal)
{
  sampledMtrPos = positionVal;
  
}


int  MotorControl::getPos()
{
  return sampledMtrPos;

}



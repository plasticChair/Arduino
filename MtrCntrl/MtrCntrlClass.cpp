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
}

void MotorControl::readSensor()
{
  potMtrCnts    = analogRead(potMtr_GPIO);
  potHanCnts    = analogRead(potHan_GPIO);
  forceCellCnts = analogRead(forceCell_GPIO);
  
}


void MotorControl::execCntrl()
{
    //int potMtrCntsPrev = 0;

    //pwmCmd = 4095 - forceCellCnts*4;
    int tempDiff = 0;

    sampledMtrPos = potMtrCnts;

   // Find out where we area
       curSector = ceil(sampledMtrPos / sizeSector);
       
       if((sampledMtrPos == 0) | (sampledMtrPos == 1023)){
          predictMode = 1;
        }
       
       if (predictModePrev){
        predictZone = deadzoneSizeCnt;
        }
       else{
          predictZone = 0;
        }
       

        if (abs(curSector - prevSector) > 1){
          predictMode = 1;
        }
        else if(((curSector - prevSector) != 1) & ((curSector - prevSector) != 0))
        {
         predictMode = 1;
        }
        else if(( abs(sampledMtrPos - potMtrCntsPrev) > WRAP_THRESHOLD) & (diffCountsPrev != 0)) // % add accel limit
        {
         predictMode = 1;
        }
      

        if(predictMode){
          sampledMtrPos = (potMtrCntsPrev + diffCountsPrev) % ( 1023+deadzoneSizeCnt);
          curSector = ceil(sampledMtrPos / sizeSector);
          predictZone = deadzoneSizeCnt;
        }

       // % Wrap check down
      if( (curSector == 1) && ((prevSector == 13) | (prevSector == 12 ))){
        wrapOffset = 1023 + predictZone- potMtrCntsPrev;
        diffCounts = (sampledMtrPos + wrapOffset);
       // wrapCnt = wrapCnt +1;
      }
      //% Wrap check up  
      else if( ((curSector == 12) | (curSector == 13 )) & ((prevSector == 0) |  (prevSector == 1))){
        wrapOffset = 1023 + predictZone - sampledMtrPos;
        diffCounts = -(wrapOffset + potMtrCntsPrev);
       // wrapCnt = wrapCnt - 1;
      }
      else{
           
        tempDiff = sampledMtrPos - potMtrCntsPrev;
        diffCounts = tempDiff;
      }
    
    mtrVel_rpm = ((float)diffCounts)*CNTPS2RPM;
    potMtrCntsPrev = potMtrCnts;
    predictModePrev = predictMode;
    diffCountsPrev = diffCounts;
    potMtrCntsPrev = sampledMtrPos;
    prevSector = curSector;    
    predictMode = 0;
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

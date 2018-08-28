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
    int potMtrCntsPrev = 0;
    int tempDiff = 0;

    tempDiff = potMtrCnts - potMtrCntsPrev ;
    
    /*  Wrap finder */
    if (tempDiff > WRAP_THRESHOLD){
      // forward Wrap, 800 - 100 = 700 > 200
      wrapOffset = 1023 - potMtrCnts;
      diffCounts = -(wrapOffset + potMtrCntsPrev);
    }
    else if(tempDiff < WRAP_THRESHOLD){
      // Backwards, 60 - 900 = -840 < 200
      wrapOffset = 1023 - potMtrCntsPrev;
      diffCounts = wrapOffset + potMtrCnts;
    }
    else{
    // no motion
    wrapOffset = 0;
    diffCounts = tempDiff;
    }
    
    mtrVel_cntsps = ((float)diffCounts)*CNTPS2RPM;
  potMtrCntsPrev = potMtrCnts;
}

void MotorControl::updatePWM()
{  
  if (pwmCmd != pwmCmdPrev){
    pwmCmdPrev = pwmCmd;
    printFlag = 1;
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

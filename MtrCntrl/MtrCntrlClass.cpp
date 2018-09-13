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
  potHanCnts    = 1023- analogRead(potHan_GPIO);
  forceCellCnts = analogRead(forceCell_GPIO);

  sampledMtrPos = potMtrCnts;

  //pwmCmd = forceCellCnts*4;
  
}


void MotorControl::execCntrl()
{
  int PWMCmdTemp = 0;

   PWMCmdTemp = ceil((potHanCnts - sampledMtrPos)*80);
    pwmCmd = map(PWMCmdTemp, -500, 500, 2500, 4095);
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





int  MotorControl::getPosCnts()
{
  return sampledMtrPos;

}

void  MotorControl::setPosCnts(int cntsIn)
{
  sampledMtrPos = cntsIn;

}



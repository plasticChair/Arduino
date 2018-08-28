
#include "MtrCntrlClass.h"



void MotorControl::init(TimerThree* Timer3)
{

  _Timer3 = Timer3;
  digitalWrite(PWM2_GPIO, HIGH);
  digitalWrite(PWM1_GPIO, LOW);

  readSensor();
  pwmCmdPrev = 0;
  pwmCmdCurr = 0;
}

void MotorControl::readSensor()
{
  potMtrCnts = analogRead(potMtr);
  potHanCnts = analogRead(potHan);
}


void MotorControl::execCntrl()
{
    int potMtrCntsPrev = 0;
    float tempDiff = 0;
    
    potMtrCntsPrev = potMtrCnts;
    potMtrCnts = analogReadFast(potMtr);

    tempDiff = potMtrCnts - potMtrCntsPrev;
    mtrVel_cntsps = (tempDiff)/(cntrlRate*1e-6);
 
    potHanCnts = analogReadFast(potHan);
  
}

void MotorControl::updatePWM()
{  
  if (pwmCmdCurr != pwmCmdPrev){
    pwmCmdPrev = pwmCmdCurr;
    printFlag = 1;
     if (pwmCmdCurr == 0)    {
          _Timer3->setPwmDuty(EN_GPIO, 0);
         // printData["stop"];
        }
        else if (pwmCmdCurr > 0) {
          digitalWrite(PWM2_GPIO, HIGH);
          digitalWrite(PWM1_GPIO, LOW);
          _Timer3->setPwmDuty(EN_GPIO, pwmCmdCurr);
          // printData["pwmCmdCurr"]);
        }
        else if (pwmCmdCurr < 0) {
          digitalWrite(PWM2_GPIO, LOW);
          digitalWrite(PWM1_GPIO, HIGH);
          _Timer3->setPwmDuty(EN_GPIO, pwmCmdCurr);
         //  printData["pwmCmdCurr"];
        }        else {
           _Timer3->setPwmDuty(EN_GPIO, 0);
         //   printData["invalid"];
        }
    }
}


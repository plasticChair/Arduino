#include "MtrCntrlClass.h"


void MotorControl::Init()
{

  pwmCmdPrev = 0;
  pwmCmd = 0;

  

  
}

void MotorControl::readSensor(int varHere)
{
 
	potMtrCnts = varHere;
  
}


void MotorControl::execCntrl()
{
    
}

void MotorControl::updatePWM()
{  

}

int MotorControl::getPosCnts()
{
	return potMtrCnts;
}





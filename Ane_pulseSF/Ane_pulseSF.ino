#include "LaCrosse_TX23.h"

//DATA wire connected to arduino port 10
LaCrosse_TX23 anemometer = LaCrosse_TX23(10);
unsigned long medianfilter(unsigned long input);


volatile unsigned long deltaT = 0;
volatile unsigned long curTime = 0;
volatile unsigned long PrevTime = 0;
volatile unsigned long curWindT = 0;

byte ISRFlag = 0;
float windRPM = 0.0;

byte idx = 0;

int bufferSize = 7;

volatile long filtArray[] = {0,0,0,0,0,0,0};
volatile unsigned long tempArray[] = {0,0,0,0,0,0,0};

void setup()
{
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(2),pulseISR,FALLING);
  
}

void loop()
{
  String dirTable[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW","W","WNW","NW","NNW"};
	float speed;
	int direction;
  
  long filtVal = 0;
  

  if (ISRFlag)
  {
   
   //  Serial.println(deltaT);
  //    Serial.println(curTime);
   // Serial.println(PrevTime);
    ISRFlag = 0;
  }
  
	if(anemometer.read(speed, direction))
  {
    //Serial.println("Speed = " + String(speed*2.23694,2) + " mph");
    
    //Serial.println("Speed = " + String(speed*2.23694,2) + " mph");
    //Serial.println("Dir = " + dirTable[direction]); 


     filtVal = 0;
    Serial.print(String(speed*2.23694,2));
    Serial.print(", ");
    for (int ii = 0; ii < 7; ii++)
    {
      filtVal = filtVal + filtArray[ii];
    }
    Serial.println(((float)filtVal)/7.0);
  }
  else
  {
    Serial.println("Read error");
  }

		//delay between succesive read requests must be at least 2sec, otherwise wind speed will read 0.
	delay(2000);
}

void pulseISR()
{
  //detachInterrupt(digitalPinToInterrupt(2));
    unsigned long filtVal;
    curTime = micros();
    if (curTime - PrevTime > 1000){
      deltaT = curTime - PrevTime;
    }
    PrevTime = curTime;
    ISRFlag = 1;

    filtArray[idx] = deltaT;
    idx++;
    if (idx > 6)
    {
      idx = 0;
    }

   //filtVal = medianfilter(deltaT);
    
    //windRPM = (1000000.0/(float)filtVal)*60.0;

   // attachInterrupt(digitalPinToInterrupt(2),pulseISR,FALLING);

}

unsigned long medianfilter(unsigned long input)
{
   long temp;
   filtArray[idx] = input;
   idx++;
   if (idx >= bufferSize){
      idx = 0 ;
   }
   
   memcpy(tempArray, filtArray, sizeof(tempArray));

   for (int kk = 1; kk >= bufferSize; kk++){
       for (int jj = kk+1; kk >= bufferSize; jj++){
           if(tempArray[jj] > tempArray[kk]){
               temp = tempArray[kk];
               tempArray[kk] = tempArray[jj];
               tempArray[jj] = temp;
          }
      }
   }   

   return tempArray[3];

}

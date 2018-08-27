

/* Function Prototypes */
void initTimer4();


/* Globals */
int serialDataIn       = 0;
int serialDataInPrev   = 0;
int newPWM             = 0;
volatile int printFlag = 0;
volatile bool bufferNotEmpty = 0;
volatile byte serialBuffer[255];
int bufferHeadIdx = 0;
volatile int bufferTailIdx = 0;
volatile int bufferIdx = 0;

/* Classes */
int count = 0;

int temp1 = 3;
int temp2 = 444;
float temp3 = 0.6;
byte *pc;

union {
  int val1;  // 2
  int val2;  // 2
  int val3;  // 2
  int val4;  // 2
  //float val3; // 4
  byte bytes[8];
  } serialTxBuf;

  
byte tempBuff[8];
byte tempBuff2[8];

void setup() {

  pinMode(13,OUTPUT);
  Serial.begin(9600);

  bufferNotEmpty = 0;
  /* GPIO Set */


  // ----- Init Timers ---------//
  initTimer4();


  
  // ----- Init Motor Controller ---------//

  Serial.println("Init complete!");
  Serial.println("");
  Serial.println("Datas");

//for(int ii = 0; ii <16; ii++)
//{
//  serialBuffer[ii*2] = (uint8_t)1;
//  serialBuffer[ii*2+1] = (uint8_t)2;
//}

bufferIdx = 24;

pc = (byte*)&temp3;

memcpy(serialBuffer, &temp1, 2);
memcpy(serialBuffer+2, &temp2, 2);
memcpy(serialBuffer+4, &temp3, 4);
memcpy(serialBuffer+8, &temp1, 2);
memcpy(serialBuffer+10, &temp2, 2);
memcpy(serialBuffer+12, &temp3, 4);


//memcpy(serialBuffer,     tempBuff, 8);
//memcpy(&serialBuffer[8], tempBuff, 8);
//memcpy(&serialBuffer[16], tempBuff, 8);
//memcpy(&serialBuffer[24], tempBuff, 8);
}

void loop() {

  // ----- Check for new commands via serial ---------//
  if (Serial.available()) {      // If anything comes in Serial (USB),
   // = Serial.parseInt();              // read it and send it out Serial1 (pins 0 & 1)
  }

  // ----- Print data based on Timer 4 ISR ---------//


  if(bufferIdx >= 24){
    memcpy(tempBuff,&serialBuffer[bufferHeadIdx],8);
    //Serial.write(tempBuff, 8);
  //  for (int ii = 0; ii< 8; ii++)
  //  {
     Serial.write(tempBuff,8);
     Serial.println("");
     //Serial.write(serialBuffer[ii]);
 //   }
//    Serial.print("--");
//    Serial.print(bufferHeadIdx);
//    Serial.print("    " );
//    Serial.println(bufferIdx);
    bufferHeadIdx += 8;  

    if (bufferHeadIdx > bufferIdx)
    {
      bufferIdx = 0;
      bufferHeadIdx =0;
//      Serial.println("Cleared!");
    }
    

  }


}


/* ---------------------------------*/
/* ----------- ISR    --------------*/ 
/* ---------------------------------*/



/* ---------------------------------*/
/* ----------- Timer 4--------------*/ 
/* ---------------------------------*/

void initTimer4()
{
  unsigned char clockSelectBits;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            //1 Hz, fclock/(prescaler * rate) - 1
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

}


/* ISR for Timer 4 */
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{

  count++;
  printFlag = 1;
   PORTB ^= _BV(PB5);  // Toggle pin 13
  // PORTB ^= B00100000;// toggles bit which affects pin13

 //memcpy(tempBuff2, count+48, 4*sizeof(count));
 tempBuff2[0] = count;
 tempBuff2[1] = count;
 tempBuff2[2] = count;
 tempBuff2[3] = count;

 //memcpy(serialBuffer + bufferIdx,tempBuff2 ,8);
  bufferIdx += 8;
}


// TIMER1_COMPA
// TIMER1_OVF


// TCTNT1 - Timer/counter
// OCRA/B - Output compare registers
// ICR1  - Input capture registers
// TCCR1A/B/C - Timer/counter control registers

void setup() {
  Serial.begin(115200);
   pinMode(11,OUTPUT);
  
  //DDRB |= (1 << PB1); // Set OCR1A to output
  
  TCCR1A = 0;
  TCCR1B = 0;

  //For normal mode, overflow vector
  if(1)
  {
    TCNT1 = 49911;  // 4, Hz65536 - fclock/(prescale * rate)
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10); //Prescaler of 1024
    TIMSK1 |= (1 << TOIE1);  // Enable timer overflow
  }
  
   //For CTC mode, using OCR1A/B, compA/B vector
  if(0)
  {
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10); //Prescaler of 1024
    TCCR1B |= (1 << WGM12);  // CTC mode
    TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);  // Enable Compare register
    OCR1A =  49910; // 1Hz, fclock/(prescaler * rate) - 1
    OCR1B =  3124; // <1 Hz
  }
  
  if(0)
  {
    // fast PWM , using OCR1A, Pin 9
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10); //Prescaler of 8
    TCCR1A |= (1 << WGM11) | (0 << WGM10) | (1 << COM1A1) | (1 << COM1B1);  // noninverting
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    OCR1A =  125; // ICR1/ (100/ dutycycle)
    ICR1 = 200;  // fclock/(prescaler * rate) - 1
  }

   if(1)
  {
    // phase correct PWM, will be 2x freq of fast PWM
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10); //Prescaler of 8
    TCCR1A |= (0 << WGM11) | (0 << WGM10) | (1 << COM1A1) | (1 << COM1B1);  // noninverting
    TCCR1B |= (1 << WGM13) | (0 << WGM12);
    OCR1A =  190; // ICR1/ (100/ dutycycle)
    ICR1 = 200;  // fclock/(prescaler * rate) - 1
  }
}

ISR(TIMER1_OVF_vect)
{
  Serial.println(millis());
  TCNT1 = 49911;
  digitalWrite(11, !digitalRead(11));
}


ISR(TIMER1_COMPA_vect)
{
  Serial.println(millis());
}


ISR(TIMER1_COMPB_vect)
{
  Serial.println(millis());
}

void loop() {
  // put your main code here, to run repeatedly:
  

}

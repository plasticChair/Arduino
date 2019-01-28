#include <Wire.h>
#include <DS3231.h>

DS3231 Clock;

byte Year = 19;
byte Month = 1;
byte Date = 19;
byte DoW = 3;
byte Hour = 12;
byte Minute = 43;
volatile byte Second = 0;
bool Century = false;
bool h12 = false;
bool PM = false;

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
    byte& Hour, byte& Minute, byte& Second);


int ISRPin = 2;

int alarmHigh = 0;
    
void setup () {
    Serial.begin(57600);
    Wire.begin();

    pinMode(ISRPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ISRPin), alarm, FALLING);

    Clock.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h

    Clock.setSecond(Second);
    Clock.setMinute(Minute);
    Clock.setHour(Hour);
    Clock.setDate(Date);
    Clock.setMonth(Month);
    Clock.setYear(Year);
    Clock.setDoW(DoW);

 Clock.enableOscillator(false, false, 0);
    Clock.setA1Time(DoW, Hour, (Minute) % 60, (Second+5) % 60, 0x1110, true ,false, false);
    // set A2 to two minutes past, on current day of month.
    Clock.setA2Time(Date, Hour, Minute+2, 0x0, false, false,       false);
    // Turn on both alarms, with external interrupt
     Clock.turnOnAlarm(1);
    Clock.turnOnAlarm(2);
  


  Serial.println("Done Setup");
  Serial.println(Clock.checkAlarmEnabled(1));
   Serial.println(Clock.checkIfAlarm(1));

   
}

void loop () {
  
    delay(1000);
  
     // Give time at next five seconds
  
        //GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

        Serial.print(Clock.getYear(), DEC);
        Serial.print("-");
        Serial.print(Clock.getMonth(Century), DEC);
        Serial.print("-");
        Serial.print(Clock.getDate(), DEC);
        Serial.print(" ");
        Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
        Serial.print(":");
        Serial.print(Clock.getMinute(), DEC);
        Serial.print(":");
        Serial.println(Clock.getSecond(), DEC);

        if (alarmHigh){
          Serial.println("ALARM");
          alarmHigh = 0;
        }

        Serial.println(Clock.checkIfAlarm(1));
    
}



void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
   byte& Hour, byte& Minute, byte& Second) {
  // Call this if you notice something coming in on 
  // the serial port. The stuff coming in should be in 
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j=0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] -48;
  Temp2 = (byte)InString[1] -48;
  Year = Temp1*10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] -48;
  Temp2 = (byte)InString[3] -48;
  Month = Temp1*10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] -48;
  Temp2 = (byte)InString[5] -48;
  Day = Temp1*10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;   
  // now Hour
  Temp1 = (byte)InString[7] -48;
  Temp2 = (byte)InString[8] -48;
  Hour = Temp1*10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] -48;
  Temp2 = (byte)InString[10] -48;
  Minute = Temp1*10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] -48;
  Temp2 = (byte)InString[12] -48;
  Second = Temp1*10 + Temp2;
}

void alarm(){

  alarmHigh = 1;
//GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);
Second = Second + 5;
 Clock.setA1Time(DoW, Hour, (Minute), (Second) % 60, 0x1110, true, false, false);

      Serial.println("ALARM!!!");

}

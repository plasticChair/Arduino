SYSTEM_MODE(SEMI_AUTOMATIC)
#define FLOW_METER_PIN 5

#include "UbidotsMicroESP8266.h"
#define TOKEN  "591fc0dc762542541c6eef58"  // Put here your Ubidots TOKEN
#define WIFISSID "Hp Wireless Printer"
#define PASSWORD "pattypsmomishot"


volatile unsigned long injTime1;
volatile unsigned long injTime2;
volatile float injTime;
double currentFlowRate = 0.0;
float prevFlowRate[10];
double totalFlow = 0.0000;
int prevFlowRateIdx = 0;
volatile int pumpEN = 0;
int PUMP_EN_PIN = 6;
int SOIL_MOIS_PIN = 11;
int SOIL_MOIS_THRES_PIN = 10;
int soilMoisture = 0;
volatile int soilMoisThres = 600;
double soilThresAvg = 0;
int waterLatch = 0;
volatile int pumpSpeed = 1023;
volatile float totalFlowThres = 0.5;
int setMode = 0; // 0 = auto, 1 = man

Ubidots client(TOKEN);

void timeCount()
{
  if(digitalRead(FLOW_METER_PIN) == HIGH)
  {
    injTime1 = micros();
  }
  else
  {
    injTime2 = micros();
    injTime =  injTime2 - injTime1;
  }
}


int modeSet(String modeSetVal)
{
  setMode = modeSetVal.toInt();
}
int pumpSpeedSet(String pumpSpeedVal)
{
  if (setMode){
  pumpSpeed = pumpSpeedVal.toInt();
  }
}
int pumpEnSet(String pumpEnVal)
{
  if (setMode){
    pumpEN = pumpEnVal.toInt();
  }
}
int flowThresSet(String flowThresVal)
{
  totalFlowThres = flowThresVal.toFloat();
}
int soilMoisThresSet(String soilMoisThresVal)
{
  soilMoisThres = soilMoisThresVal.toInt();
}


void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(115200);
  pinMode(1, OUTPUT); //LED on Oak
  pinMode(FLOW_METER_PIN,INPUT);
  pinMode(SOIL_MOIS_THRES_PIN,INPUT);
  pinMode(PUMP_EN_PIN,OUTPUT);
  analogWriteFreq(1);
  attachInterrupt(digitalPinToInterrupt(FLOW_METER_PIN), timeCount, CHANGE);

  Particle.variable("FlowRate",currentFlowRate);
  Particle.variable("TotalFlow",totalFlow);
  Particle.variable("SoilMoisture",soilMoisture);
  Particle.variable("ModeSelect",setMode);

  Particle.function("modeSet", modeSet);
  Particle.function("pumpSpeedSet", pumpSpeedSet);
  Particle.function("pumpEnSet", pumpEnSet);
  Particle.function("flowThresSet", flowThresSet);
  Particle.function("soilMoisThresSet", soilMoisThresSet);

  if (Particle.connected() == false)
  {
    Particle.connect();
  }

  digitalWrite(PUMP_EN_PIN,LOW);
  delay(10);
  client.wifiConnection(WIFISSID, PASSWORD);
  
}



// the loop routine runs over and over again forever:
void loop() { digitalWrite(1, !(digitalRead(1)));   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second

 
  soilMoisture = analogRead(A0);

if (!setMode){
  if (pumpEN)  {
    currentFlowRate = (1000.0/(float)injTime)*60.0/7.5;
    totalFlow = 0.1*(currentFlowRate/60.0) + totalFlow;
    injTime = 10000000000;
    
  }
  
  if ((soilMoisture > soilMoisThres ) && !pumpEN && !waterLatch)
  {
    pumpEN = 1;
    //pumpSpeed = 1023;
    waterLatch = 1;
    analogWrite(PUMP_EN_PIN,(float)pumpSpeed*(float)pumpEN);
    totalFlow = 0;
  }

  if ((waterLatch == 1) && (totalFlow > totalFlowThres))
  {
    waterLatch = 0;
    pumpEN = 0;
    analogWrite(PUMP_EN_PIN,pumpEN);
    currentFlowRate = 0;
    totalFlow = 0;
  }
}
else{
    currentFlowRate = (1000.0/(float)injTime)*60.0/7.5;
    totalFlow = 0.1*(currentFlowRate/60.0) + totalFlow;
    injTime = 10000000000;
    analogWrite(PUMP_EN_PIN,(float)pumpSpeed*(float)pumpEN);
}


  Serial.print("Pump is : ");
  Serial.println(pumpEN);
  Serial.print("Flow Rate is: ");
  Serial.print(currentFlowRate,4);
  Serial.print("(L/min)\n");
  Serial.print("Total Flow is: ");
  Serial.print(totalFlow,5);
  Serial.print(" (L) \n");
  Serial.print("Soil Moisture is: ");
  Serial.println(soilMoisture);
  Serial.print("water Latch: ");
  Serial.println(pumpSpeed);
  Serial.print("\n\n");




}

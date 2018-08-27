//#include "Adafruit_BME280.h"
#include "Options.h"

#include <MPU9250.h>
#include "Logger_start.h"
#include <Snooze.h>
// #include "Adafruit_Sensor.h"
// #include "Adafruit_BME280.h"

//extern void basicSetup();

SnoozeDigital digital;
SnoozeTimer timer;
Snoozelc5vBuffer  lc5vBuffer;


SnoozeBlock config_teensy36(digital, timer);

//File loggerFile;

//Global sensor object
BME280 BME280Sensor(1, DEBUG_PORT);
BME280 BMP280Sensor(0, DEBUG_PORT);


//MPU9250 myIMU;

/*
SCL = 19
SDA = 18



GPS
--------------
Rx = 33  GPS2TEENSY
Tx = 34  TEENSY2GPS

*/


// General Settings
int SDWriteLimit= 1;

void setup()
{
	
  
  DEBUG_PORT.begin(9600);

  //GPS_PORT.begin(9600);
 //  while (!GPS_PORT);

  delay(100);
  DEBUG_PORT.println("Program Started\n");

  // Setup Pin modes
  //digitalWriteFast(SD_CS,HIGH);
  
  
  basicSetup();


  analogWrite(Pin_led2,2);
  delay(2000);

  int flashCycle[] = {100, 100, 100, 100, 100, 9999,0,0,0,0};
  flashPin(sysLED, HIGH, flashCycle);
  flashPin(Pin_led1, HIGH, flashCycle);
  flashPin(Pin_led2, HIGH, flashCycle);
  flashPin(Pin_led3, LOW, flashCycle);
  flashPin(Pin_led4, LOW, flashCycle);

  #if BME_Enable
   DEBUG_PORT.println("Here");
   BME280Sensor.Setup(0);
   BMP280Sensor.Setup(1);
    DEBUG_PORT.println("Here2");
  #endif
  
  #if IMU_Enable
	//imuSetup();
  #endif
  
  #if SD_Enable
    SDsetup();
  #endif
  
  #if GPS_Enable
    gpsSetup();
  #endif
  
  #if BLE_Enable
	BLE_PORT.begin(9600);
    BLESetup();
  #endif

   //digital.pinMode(9, INPUT_PULLUP, RISING);//pin, mode, type
   //timer.setTimer(5000);// milliseconds
  
  DEBUG_PORT.println("Setup Done\n");
}




void loop() 
{ 
  
 
  #if GPS_Enable
     GPSloop();
     //delay(1000);
  #elif 0

	BME280Sensor.print();
	BMP280Sensor.print();
    
    //printAccel();
    int flashCycle[] = {50, 50, 50, 50, 50,50, 400, 9999,0,0,0,0};
    flashPin(sysLED, LOW, flashCycle);
    #if SD_Enable
      SDWrite();
    #endif

   // BLE_PORT.print(31);
    //BLE_PORT.println(currentSensorData.BME280_temp_f);
      
      //sprintf(currentSensorData.BME280_temp_f))
    delay(1000);
    DEBUG_PORT.println("---------------------------------");
   #elif 1


	delay(2000);


  #endif


  
}


void doSomeWork()
{
	// Print all the things!
	trace_all(DEBUG_PORT, gps, fix_data);

	DEBUG_PORT.println();

#if SD_Enable
	SDWrite();
#endif

	//printAccel();
	int flashCycle[] = { 50, 50, 50, 50, 50,50, 400, 9999,0,0,0,0 };
	flashPin(Pin_led1, HIGH, flashCycle);

	BME280Sensor.print();
	BMP280Sensor.print();
	//printAccel();
	trace_all(BLE_PORT, gps, fix_data);
	BLE_PORT.println("Done with loop!");
	DEBUG_PORT.println("Done with loop!");
	DEBUG_PORT.println("------");
	DEBUG_PORT.println("");


} // doSomeWork




void SDsetup()
{
     DEBUG_PORT.print("Trying SD: ");
      if (!SD.begin(SD_CS)) {
         DEBUG_PORT.println("SD initialization failed!");
         return;
	  }
      DEBUG_PORT.println(" sdinitialization done!");
}

void SDWrite()
{
  File loggerFile;
   int flashCycle[] = {50, 50, 50, 50, 50, 9999,0,0,0,0};

  if(SDLoopCount >= SDWriteLimit)
  {
     DEBUG_PORT.print("Trying SD: ");
    loggerFile = SD.open("GPSLog1.txt", FILE_WRITE);
     DEBUG_PORT.print(loggerFile);
   if (loggerFile) {
      
        DEBUG_PORT.print("Writing to test.txt...");
       
        flashPin(Pin_led2, HIGH, flashCycle);
        loggerFile.print("SD Write Count: ");
        loggerFile.println(SDWriteCount);
         DEBUG_PORT.println(SDWriteCount);
       flashPin(Pin_led2, LOW, flashCycle);
        // close the file:
        //trace_all( loggerFile, gps, fix_data ); // uncomment include Streamers.h
        loggerFile.println();
        loggerFile.close();
      //  loggerFile.close();

      flashPin(Pin_led2, HIGH, flashCycle);
      DEBUG_PORT.println("wrote to SD card ---------");
      SDLoopCount = 0;
      SDWriteCount++;
        
     } 
     else {
      // if the file didn't open, print an error:
      DEBUG_PORT.println(printf("error opening: %s", LogFileName));
       DEBUG_PORT.println("Error");
      SDLoopCount = 0;
     // loggerFile.close();

      digitalWrite(Pin_led4, LOW);
      delay(5000);     
    } 
  }
  else
  {
    SDLoopCount++;    
  }
}









void flashPin(int pin, bool initCond, int *delays)
{
  digitalWrite(pin, initCond);
  for (int ii = 0; delays[ii] < 9999; ii++)
  {
    
    digitalWrite(pin, !digitalRead(pin)); 
    delay(delays[ii]);
  }
}












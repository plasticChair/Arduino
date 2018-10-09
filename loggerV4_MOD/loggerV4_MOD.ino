//#include "Adafruit_BME280.h"
#include "Logger_start.h"
#include <Snooze.h>
// #include "Adafruit_Sensor.h"
// #include "Adafruit_BME280.h"
SnoozeDigital digital;
SnoozeTimer timer;
Snoozelc5vBuffer  lc5vBuffer;

#define DEBUG_PORT Serial
#define GPS_PORT Serial3
#define BLE_PORT Serial1

#define GPS_Enable 1
#define SD_Enable 0
#define BME_Enable 0
#define Accel_Enable 1
#define BLE_Enable 0




#define SDDataSize 10
#define SEALEVELPRESSURE_HPA (1013.25)


SnoozeBlock config_teensyLC(digital, timer,lc5vBuffer);

//File loggerFile;

//Global sensor object
BME280 BME280Sensor;
BME280 BMP280Sensor;
BME280 tempSensor;
ADXL345 adxl = ADXL345(); 

static NMEAGPS  gps;
static gps_fix  fix_data;
static gps_fix SdFixInfo;
int SDLoopCount = 0;
int SDWriteCount = 0;

char LogFileName[30] = "GPS_Log_1.txt";

// Enable/ power pins
int SD_CS           = 10;  // Was 6
int Pin_GPS_pwr     = 3;
int Pin_SD_pwr      = 21;  // was 12
int Pin_Batt_mon_en = 20;  // was 14
int Pin_Batt_mon    = 15; 
int Pin_BLE_pwr     = 17;

// Function button pins
int Pin_fun1 = 26;
int Pin_fun2 = 16;
int Pin_fun3 = 2;

// LEDs
int Pin_led1 = 4;
int Pin_led2 = 6; // was 11
int Pin_led3 = 22;
int Pin_led4 = 23;
int sysLED   = 13;

//Comm pins
int SD_MOSI = 11;  //was 21
int SD_MISO = 12;  // was 5
int SD_SCK  = 14;  // was 20

int flashCycle[10] = {0,0,0,0,0,0,0,0,0,0};

/*
SCL = 19
SDA = 18

SD
------------
CS = 6
SCK = 20
MOSI = 21
MISO = 5

GPS
--------------
Rx = 7  GPS2TEENSY
Tx = 8  TEENSY2GPS

*/

volatile struct sensorDataStruct
{
  float accelX_g;
  float accelY_g;
  float accelZ_g;

  float BME280_humidity_pct;
  float BME280_pressure_kPa;
  float BME280_alt_ft;
  float BME280_temp_f;

  float gpsAlt_f;
  float gpsLong_deg;
  float gpsLat_deg;
  float gpsNumSat;
  float gpsTime;

  float gpsFiltPosX;
  float gpsFiltPosY;
  float gpsFiltPosAlt;

  float avgAlt[SDDataSize];
    
} currentSensorData;

// General Settings
int SDWriteLimit= 1;

void setup()
{

 
  
  DEBUG_PORT.begin(9600);
  BLE_PORT.begin(9600);
  //GPS_PORT.begin(9600);
 //  while (!GPS_PORT);

  delay(500);
  DEBUG_PORT.println("Program Started\n");

  // Setup Pin modes
  //digitalWriteFast(SD_CS,HIGH);
  pinMode(SD_CS          , OUTPUT);
  pinMode(Pin_GPS_pwr    , OUTPUT);
  pinMode(Pin_SD_pwr     , OUTPUT);
  pinMode(Pin_Batt_mon_en, OUTPUT);
  pinMode(Pin_Batt_mon   , INPUT);
  pinMode(Pin_BLE_pwr    , OUTPUT);
  
  pinMode(Pin_fun1, INPUT);
  pinMode(Pin_fun2, INPUT);
  pinMode(Pin_fun3, INPUT);
  
  pinMode(Pin_led1, OUTPUT);
  pinMode(Pin_led2, OUTPUT);
  pinMode(Pin_led3, OUTPUT);
  pinMode(Pin_led4, OUTPUT);
  pinMode(sysLED  , OUTPUT);

  digitalWrite(SD_CS, LOW);
 // digitalWrite(Pin_GPS_pwr    , (GPS_Enable));
  digitalWrite(Pin_BLE_pwr    , (BLE_Enable));
  digitalWrite(Pin_SD_pwr     , !(SD_Enable));
  digitalWrite(Pin_Batt_mon_en, HIGH);

  digitalWrite(Pin_led1, LOW);
  
 // digitalWrite(Pin_led2, LOW);
  digitalWrite(Pin_led3, HIGH);
  digitalWrite(Pin_led4, HIGH);
  digitalWrite(sysLED  , LOW);
  analogWrite(Pin_led2,2);
   delay(2000);

  int flashCycle[] = {100, 100, 100, 100, 100, 9999,0,0,0,0};
  flashPin(sysLED, HIGH, flashCycle);
  flashPin(Pin_led1, HIGH, flashCycle);
;
   flashPin(Pin_led2, HIGH, flashCycle);
    flashPin(Pin_led3, LOW, flashCycle);
     flashPin(Pin_led4, LOW, flashCycle);

  #if BME_Enable
   DEBUG_PORT.println("Here");
    BME280Setup(0);
    BME280Setup(1);
    DEBUG_PORT.println("Here2");
  #endif
  
  #if Accel_Enable
    ADXL345Setup();
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

    digital.pinMode(9, INPUT_PULLUP, RISING);//pin, mode, type
   timer.setTimer(5000);// milliseconds
  
  DEBUG_PORT.println("Setup Done\n");
}


void basicSetup(){

pinMode(SD_CS          , OUTPUT);
  pinMode(Pin_GPS_pwr    , OUTPUT);
  pinMode(Pin_SD_pwr     , OUTPUT);
  pinMode(Pin_Batt_mon_en, OUTPUT);
  pinMode(Pin_Batt_mon   , INPUT);
  pinMode(Pin_BLE_pwr    , OUTPUT);
  
  pinMode(Pin_fun1, INPUT);
  pinMode(Pin_fun2, INPUT);
  pinMode(Pin_fun3, INPUT);
  
  pinMode(Pin_led1, OUTPUT);
  pinMode(Pin_led2, OUTPUT);
  pinMode(Pin_led3, OUTPUT);
  pinMode(Pin_led4, OUTPUT);
  pinMode(sysLED  , OUTPUT);

  digitalWrite(SD_CS, LOW);
 // digitalWrite(Pin_GPS_pwr    , (GPS_Enable));
  digitalWrite(Pin_BLE_pwr    , (BLE_Enable));
  digitalWrite(Pin_SD_pwr     , !(SD_Enable));
  digitalWrite(Pin_Batt_mon_en, HIGH);

   digitalWrite(Pin_led1, HIGH);
  
  digitalWrite(Pin_led2, HIGH);
  digitalWrite(Pin_led3, HIGH);
  digitalWrite(Pin_led4, HIGH);
  digitalWrite(sysLED  , LOW);
}

void loop() 
{ 
  
 
  #if GPS_Enable
     GPSloop();
     //delay(1000);
  #else

    printBME280(0);
    printBME280(1);
    
    //printAccel();
    int flashCycle[] = {50, 50, 50, 50, 50,50, 400, 9999,0,0,0,0};
    flashPin(Pin_led1, LOW, flashCycle);
    #if SD_Enable
      SDWrite();
    #endif

    BLE_PORT.print(31);
    //BLE_PORT.println(currentSensorData.BME280_temp_f);
      
      //sprintf(currentSensorData.BME280_temp_f))
    delay(1000);
    DEBUG_PORT.println("---------------------------------");
  #endif


  
}

static void GPSloop()
{

   //DEBUG_PORT.println("in loop");
   
  while (gps.available( GPS_PORT )) {
     BLE_PORT.println("read some data");
    fix_data = gps.read();
    doSomeWork();

    if(fix_data.status == 3)
    {
      digitalWrite(Pin_led3,LOW);
    }

   DEBUG_PORT.flush(); 
   GPS_PORT.flush(); 
   BLE_PORT.flush(); 
   delay(10);
   //delay(3000);
     
   // Snooze.deepSleep( config_teensyLC );// return module that woke processor
    delay(10);
  
    //GPS_PORT.begin(9600);
   // BLE_PORT.begin(9600);
   // Serial.begin(9600);
  
    // Serial.write(0x00);
    GPS_PORT.flush(); 
    BLE_PORT.flush(); 
    Serial.flush(); 

    basicSetup();
    //delay(300);
 //// Serial.println("terwerwerwer");
 // Serial.println("terwerwerwer");
//  Serial.println("terwerwerwer");

  //  delay(300);
  //    delay(300);

    
  }



 
  //digitalWrite(13, !digitalRead(13));
  //delay(100);
 //trace_all( DEBUG_PORT, gps, fix_data );

} // GPS


static void doSomeWork()
{
  // Print all the things!
  trace_all( DEBUG_PORT, gps, fix_data );
  
  DEBUG_PORT.println();

  #if SD_Enable
    SDWrite();
  #endif

  //printAccel();
    int flashCycle[] = {50, 50, 50, 50, 50,50, 400, 9999,0,0,0,0};
    flashPin(Pin_led1, HIGH, flashCycle);

    printBME280(0);
    printBME280(1);
   //printAccel();
  trace_all( BLE_PORT, gps, fix_data );
    BLE_PORT.println("Done with loop!");
  DEBUG_PORT.println("Done with loop!");
  DEBUG_PORT.println("------");
  DEBUG_PORT.println("");


} // doSomeWork

void SDsetup()
{
  
  SPI.setMOSI(SD_MOSI);  // Audio shield has MOSI on pin 7
  SPI.setSCK(SD_SCK);  // Audio shield has SCK on pin 14
  SPI.setMISO(SD_MISO);
     DEBUG_PORT.print("Trying SD: ");
      if (!SD.begin(SD_CS)) {
         DEBUG_PORT.println("initialization failed!");
         return;
      }
      DEBUG_PORT.println("initialization done!");
  
  digitalWrite(Pin_SD_pwr, LOW);
  
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

void ADXL345Setup()
{
  adxl.powerOn();                     // Power on the ADXL345
  DEBUG_PORT.println("Finished power on ");

  adxl.setRangeSetting(4);           // Give the range settings
                    // Accepted values are 2g, 4g, 8g or 16g
                    // Higher Values = Wider Measurement Range
                    // Lower Values = Greater Sensitivity
    
  adxl.setActivityXYZ(1, 1, 1);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)



  adxl.setInactivityXYZ(1, 1, 1);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(180);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 0); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment

  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(255);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment

  // Setting all interupts to take place on INT1 pin
  //adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                            // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                            // This library may have a problem using INT2 pin. Default to INT1 pin.

  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(0);
  adxl.ActivityINT(0);
  adxl.FreeFallINT(0);
  adxl.doubleTapINT(0);
  adxl.singleTapINT(0);

  //attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

}

void printAccel()
{
    // Accelerometer Readings
  int x,y,z;
  float  convX,convY,convZ;   
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z

  // Output Results to Serial
  /* UNCOMMENT TO VIEW X Y Z ACCELEROMETER VALUES */  
  convX = float((int16_t)x)*9.80665F*0.0078125F;
  convY = float((int16_t)y)*9.80665F*0.0078125F;
  convZ = float((int16_t)z)*9.80665F*0.0078125F;
  DEBUG_PORT.print("Accel Data:  ");
  DEBUG_PORT.print(convX );
  DEBUG_PORT.print(", ");
  DEBUG_PORT.print(convY);
  DEBUG_PORT.print(", ");
  DEBUG_PORT.println(convZ); 

  //currentSensorData.accelX_g = convX;
 // currentSensorData.accelY_g = convY;
 // currentSensorData.accelZ_g = convZ;


  // ADXL_ISR();
  // You may also choose to avoid using interrupts and simply run the functions within ADXL_ISR(); 
  //  and place it within the loop instead.  
  // This may come in handy when it doesn't matter when the action occurs. 

}

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */
void ADXL_ISR() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    DEBUG_PORT.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  } 
  
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    DEBUG_PORT.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    DEBUG_PORT.println("*** ACTIVITY ***"); 
     //add code here to do when activity is sensed
  }
  
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    DEBUG_PORT.println("*** DOUBLE TAP ***");
     //add code here to do when a 2X tap is sensed
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    DEBUG_PORT.println("*** TAP ***");
     //add code here to do when a tap is sensed
  } 
}


void BME280Setup(int dev_sel)
{

   int sensorAddress = 0x76;
  if(dev_sel == 1)
  {
    tempSensor = BME280Sensor;
    sensorAddress = 0x76;
  }
  else
  {
    tempSensor = BMP280Sensor;
    sensorAddress = 0x77;
  }

   tempSensor.settings.I2CAddress = sensorAddress;  
  //runMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  tempSensor.settings.runMode = 3; //Normal mode

  //tStandby can be:
  //  0, 0.5ms
  //  1, 62.5ms
  //  2, 125ms
  //  3, 250ms
  //  4, 500ms
  //  5, 1000ms
  //  6, 10ms
  //  7, 20ms
  tempSensor.settings.tStandby = 0;

  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  tempSensor.settings.filter = 1;

  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  tempSensor.settings.tempOverSample = 2;

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  tempSensor.settings.pressOverSample = 2;

  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  tempSensor.settings.humidOverSample = 2;
  
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.]
     DEBUG_PORT.println("Here before init");
  DEBUG_PORT.println(tempSensor.begin(), HEX);

  
  
  
  DEBUG_PORT.print("Displaying ID, reset and ctrl regs\n");

  DEBUG_PORT.print("ID(0xD0): 0x");
  DEBUG_PORT.println(tempSensor.readRegister(BME280_CHIP_ID_REG), HEX);
  DEBUG_PORT.print("Reset register(0xE0): 0x");
  DEBUG_PORT.println(tempSensor.readRegister(BME280_RST_REG), HEX);
  DEBUG_PORT.print("ctrl_meas(0xF4): 0x");
  DEBUG_PORT.println(tempSensor.readRegister(BME280_CTRL_MEAS_REG), HEX);
  DEBUG_PORT.print("ctrl_hum(0xF2): 0x");
  DEBUG_PORT.println(tempSensor.readRegister(BME280_CTRL_HUMIDITY_REG), HEX);
  DEBUG_PORT.print("\n\n");
  
  DEBUG_PORT.print("Displaying all regs\n");
  uint8_t memCounter = 0x80;
  uint8_t tempReadData;
  for(int rowi = 8; rowi < 16; rowi++ )
  {
    DEBUG_PORT.print("0x");
    DEBUG_PORT.print(rowi, HEX);
    DEBUG_PORT.print("0:");
    
    for(int coli = 0; coli < 16; coli++ )
    {
      tempReadData = tempSensor.readRegister(memCounter);
      DEBUG_PORT.print((tempReadData >> 4) & 0x0F, HEX);//Print first hex nibble
      DEBUG_PORT.print(tempReadData & 0x0F, HEX);//Print second hex nibble
      DEBUG_PORT.print(" ");
      memCounter++;
    }
    DEBUG_PORT.print("\n");
  }


  DEBUG_PORT.print("\n\n");

  DEBUG_PORT.print("Displaying concatenated calibration words\n");
  DEBUG_PORT.print("dig_T1, uint16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_T1);
  DEBUG_PORT.print("dig_T2, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_T2);
  DEBUG_PORT.print("dig_T3, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_T3);

  DEBUG_PORT.print("dig_P1, uint16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P1);
  DEBUG_PORT.print("dig_P2, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P2);
  DEBUG_PORT.print("dig_P3, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P3);
  DEBUG_PORT.print("dig_P4, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P4);
  DEBUG_PORT.print("dig_P5, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P5);
  DEBUG_PORT.print("dig_P6, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P6);
  DEBUG_PORT.print("dig_P7, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P7);
  DEBUG_PORT.print("dig_P8, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P8);
  DEBUG_PORT.print("dig_P9, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_P9);

  DEBUG_PORT.print("dig_H1, uint8: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H1);
  DEBUG_PORT.print("dig_H2, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H2);
  DEBUG_PORT.print("dig_H3, uint8: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H3);
  DEBUG_PORT.print("dig_H4, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H4);
  DEBUG_PORT.print("dig_H5, int16: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H5);
  DEBUG_PORT.print("dig_H6, uint8: ");
  DEBUG_PORT.println(tempSensor.calibration.dig_H6);

  if(dev_sel == 1)
  {
    BME280Sensor = tempSensor;
    sensorAddress = 0x76;
  }
  else
  {
    BMP280Sensor = tempSensor;
    sensorAddress = 0x77;
  }
  DEBUG_PORT.println();
  
}

void printBME280(int dev_sel)
{

  if(dev_sel == 1)
  {
    DEBUG_PORT.println("Device 1");
    DEBUG_PORT.println("");
  currentSensorData.BME280_humidity_pct = BME280Sensor.readFloatHumidity();
  currentSensorData.BME280_pressure_kPa = BME280Sensor.readFloatPressure();
  currentSensorData.BME280_alt_ft       = BME280Sensor.readFloatAltitudeFeet();
  currentSensorData.BME280_temp_f       = BME280Sensor.readTempF();
    int sensorAddress = 0x76;
  }
  else
  {
        DEBUG_PORT.println("Device 0");
    DEBUG_PORT.println("");

  currentSensorData.BME280_humidity_pct = BMP280Sensor.readFloatHumidity();
  currentSensorData.BME280_pressure_kPa = BMP280Sensor.readFloatPressure();
  currentSensorData.BME280_alt_ft       = BMP280Sensor.readFloatAltitudeFeet();
  currentSensorData.BME280_temp_f       = BMP280Sensor.readTempF();
    int sensorAddress = 0x77;
  }
  //Each loop, take a reading.
  //Start with temperature, as that data is needed for accurate compensation.
  //Reading the temperature updates the compensators of the other functions
  //in the background.

 //BME280Sensor.writeRegister(BME280_CTRL_MEAS_REG, tempSensor.settings.initCtrlRegs);

  delay(100);


  DEBUG_PORT.print("Temperature: ");
  DEBUG_PORT.print(currentSensorData.BME280_temp_f, 2);
  DEBUG_PORT.println(" degrees F");

 // DEBUG_PORT.print("Pressure: ");
 // DEBUG_PORT.print(currentSensorData.BME280_pressure_kPa, 2);
 // DEBUG_PORT.println(" Pa");

  DEBUG_PORT.print("Altitude: ");
  DEBUG_PORT.print( currentSensorData.BME280_alt_ft, 2);
  DEBUG_PORT.println("ft"); 

  DEBUG_PORT.print("%RH: ");
  DEBUG_PORT.print(currentSensorData.BME280_humidity_pct, 2);
  DEBUG_PORT.println(" %");

  DEBUG_PORT.println();
}


void gpsSetup(){

  
  // Start the GPS device
 // Serial2.attachInterrupt( GPSisr );
  //attachInterrupt(3,GPSisr,CHANGE);

  digitalWrite(Pin_GPS_pwr,(GPS_Enable));


  // Start the normal trace output
  //DEBUG_PORT.begin(9600);
 // while (!Serial)
 //   ;

 delay(100);

  DEBUG_PORT.print( F("NMEA.INO: started\n") );
  DEBUG_PORT.print( F("  fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("  gps object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on ") );

  #ifndef NMEAGPS_RECOGNIZE_ALL
    #error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
  #endif

  #ifdef NMEAGPS_INTERRUPT_PROCESSING
    #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
  #endif

  #if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

    DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

  #else
    if (gps.merging == NMEAGPS::NO_MERGING) {
      DEBUG_PORT.print  ( F("\nWARNING: displaying data from ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.print  ( F(" sentences ONLY, and only if ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.println( F(" is enabled.\n"
                            "  Other sentences may be parsed, but their data will not be displayed.") );
    }
  #endif

  DEBUG_PORT.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  DEBUG_PORT.println( F(" sentence is received.\n"
                        "  You should confirm this with NMEAorder.ino\n") );

  trace_header( DEBUG_PORT );

  DEBUG_PORT.flush();

  // Start the UART for the GPS device
  GPS_PORT.begin( 9600 );

  delay(100);

   // waitForFix();
}


static void waitForFix()
{
  DEBUG_PORT.print( F("Waiting for GPS fix...") );

  uint16_t lastToggle = millis();

  for (;;) {
    if (gps.available()) {
      if (gps.read().valid.location)
        break; // Got it!
    }

    // Slowly flash the LED until we get a fix
    /*
    if ((uint16_t) millis() - lastToggle > 500) {
      lastToggle += 500;
      digitalWrite( LED, !digitalRead(LED) );
      DEBUG_PORT.write( '.' );
    }
    */
  }
  DEBUG_PORT.println();

  //digitalWrite( LED, LOW );

  gps.overrun( false ); // we had to wait a while...

} // waitForFix

void flashPin(int pin, bool initCond, int *delays)
{
  digitalWrite(pin, initCond);
  for (int ii = 0; delays[ii] < 9999; ii++)
  {
    
    digitalWrite(pin, !digitalRead(pin)); 
    delay(delays[ii]);
  }
}


void BLESetup()
{

  digitalWrite(Pin_BLE_pwr,HIGH);  

  BLE_PORT.print("AT+MODE2"); // Enable AT commands over BLE

}










#define txWindSF 8.9737
#define txGustSF 1
#define txTempSF 10.7684
#define txPressSF 5.115
#define txHumidSF 10.23
#define txBattVSF 212.5
#define txBattPSF 2.55

#define txTempOff -30
#define txPressOff 900
#define txWBattOff 3

#define wxPackageSize 14
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct wxSFStruct {
  float windSF;
  float gustSF;
  float tempSF;
  float pressSF;
  float humidSF;
  float batteryVSF;
  float batteryPSF;
  int   tempOff;
  int   pressOff;
  int   battOff;

};

typedef struct wxStruct {
  unsigned int year     : 4;
  unsigned int month    : 4;
  unsigned int day      : 5;
  unsigned int hour     : 5;
  unsigned int minute   : 6;
  unsigned int second   : 6;
  unsigned int dir      : 4;
  unsigned int speed    : 10;
  unsigned int gustMax  : 11;
  unsigned int temp     : 10;
  unsigned int press    : 10;
  unsigned int humid    : 10;
  unsigned int batteryV : 8;
  unsigned int batteryP : 8;
  unsigned int info     : 8;

};


typedef struct DataStruct {
  wxStruct     weather;


} MstrDataStruct;



typedef union {
  wxStruct weather;
  byte     TxData[wxPackageSize];
}storeDataUnion;

unsigned long floatLimit(float value, float scalefactor, int offset, float max_in, float min_in)
{

  float temp;
  temp = MIN(value, max_in);
  temp = MAX(temp, min_in);
  temp = (temp - (float)offset) * scalefactor;

  return (unsigned long)temp;
}




wxSFStruct              wxSF;
MstrDataStruct      MstrData;
storeDataUnion     storeData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

wxSF.windSF = txWindSF;
  wxSF.gustSF = txGustSF;
  wxSF.tempSF = txTempSF;
  wxSF.pressSF = txPressSF;
  wxSF.humidSF = txHumidSF;
  wxSF.batteryVSF = txBattVSF;
  wxSF.batteryPSF = txBattPSF;
  wxSF.tempOff = txTempOff;
  wxSF.pressOff = txPressOff;
  wxSF.battOff = txWBattOff;

  
    MstrData.weather.year   = 1;
    MstrData.weather.month  = 2;
    MstrData.weather.day    = 3;
    MstrData.weather.hour   = 10;
    MstrData.weather.minute = 37;
    MstrData.weather.second = 54; //Account for 2 second delay for Anemometer


    /*---------------------------------------------------------------------------------------*/
    MstrData.weather.speed    = (unsigned int)floatLimit((float)35.8     , wxSF.windSF    , 0            , 114 , 0);
    MstrData.weather.dir      = 10;

    MstrData.weather.gustMax  = (unsigned int)floatLimit((float)100.0   , wxSF.gustSF    , 0            , 2047, 0);

    MstrData.weather.temp     = (unsigned int)floatLimit((float)23.5      , wxSF.tempSF    , wxSF.tempOff , 65  , -30);
    MstrData.weather.press    = (unsigned int)floatLimit((float)1002.2     , wxSF.pressSF   , wxSF.pressOff, 1100, 900); //hPa
    MstrData.weather.humid    = (unsigned int)floatLimit((float)56.2     , wxSF.humidSF   , 0            , 100 , 0);

    MstrData.weather.batteryV = (unsigned int)floatLimit((float)3.87  , wxSF.batteryVSF, wxSF.battOff , 4.2 , 3);
    MstrData.weather.batteryP = (unsigned int)floatLimit((float)68.4  , wxSF.batteryPSF, 0            , 100 , 0);



  storeData.weather = MstrData.weather;

  Serial.print(storeData.weather.year);     Serial.print(", "); Serial.println(storeData.weather.year    ,BIN);
  Serial.print(storeData.weather.month);    Serial.print(", "); Serial.println(storeData.weather.month   ,BIN);
  Serial.print(storeData.weather.day);      Serial.print(", "); Serial.println(storeData.weather.day     ,BIN);
  Serial.print(storeData.weather.hour);     Serial.print(", "); Serial.println(storeData.weather.hour    ,BIN);
  Serial.print(storeData.weather.minute);   Serial.print(", "); Serial.println(storeData.weather.minute  ,BIN);
  Serial.print(storeData.weather.second);   Serial.print(", "); Serial.println(storeData.weather.second  ,BIN);
  Serial.print(storeData.weather.dir);      Serial.print(", "); Serial.println(storeData.weather.dir     ,BIN);
  Serial.print(storeData.weather.speed);    Serial.print(", "); Serial.println(storeData.weather.speed   ,BIN);
  Serial.print(storeData.weather.gustMax);  Serial.print(", "); Serial.println(storeData.weather.gustMax ,BIN);
  Serial.print(storeData.weather.temp);     Serial.print(", "); Serial.println(storeData.weather.temp    ,BIN);
  Serial.print(storeData.weather.press);    Serial.print(", "); Serial.println(storeData.weather.press   ,BIN);
  Serial.print(storeData.weather.humid);    Serial.print(", "); Serial.println(storeData.weather.humid   ,BIN);
  Serial.print(storeData.weather.batteryV); Serial.print(", "); Serial.println(storeData.weather.batteryV,BIN);
  Serial.print(storeData.weather.batteryP); Serial.print(", "); Serial.println(storeData.weather.batteryP,BIN);
  
  Serial.println("--------------------");
  for(int ii = 0; ii< wxPackageSize; ii ++)
  {
    Serial.print(storeData.TxData[ii],HEX);
    Serial.print(", ");
    Serial.println(storeData.TxData[ii],BIN);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

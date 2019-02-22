typedef struct wxStruct {
  byte year             : 5;
  byte day              : 3;

};

typedef union {
  wxStruct weather;
  unsigned int TxData[2];
}storeDataUnion;


typedef struct DataStruct {
  wxStruct weather;
  unsigned int gustAvg;
  unsigned int gustMax;

} MstrDataStruct;



MstrDataStruct storeData;

storeDataUnion *pointData;

void setup() {
  // put your setup code here, to run once:

  storeData.weather.year = 0b010;
  storeData.weather.day = 0b111;

storeData.gustAvg = 5;
  storeData.gustMax = 8;

  Serial.begin(115200);
  delay(10);

  Serial.println(storeData.gustAvg,BIN);
  Serial.println(storeData.gustMax,BIN);
 Serial.println("eeeeee");
pointData->weather = storeData.weather;
  Serial.println(pointData->TxData[0],BIN);
  Serial.println(pointData->TxData[1],BIN);

  Serial.println("here");

}

void loop() {
  // put your main code here, to run repeatedly:

}

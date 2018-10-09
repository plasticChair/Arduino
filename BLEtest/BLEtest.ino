void setup() {  

  pinMode(17,OUTPUT);
  digitalWrite(17, HIGH);
  Serial.begin(9600);
  // If the baudrate of the HM-10 module has been updated,
  // you may need to change 9600 by another value
  // Once you have found the correct baudrate,
  // you can update it using AT+BAUDx command 
  // e.g. AT+BAUD0 for 9600 bauds
  Serial1.begin(9600);
}

void loop() {  
  char c;
  if (Serial.available()) {
    c = Serial.read();
    Serial1.print(c);
  }
  if (Serial1.available()) {
    c = Serial1.read();
    Serial.print(c);    
  }
}

#if 1

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

long timerCheck = 0;
int currChannel = 0;

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino

const int XP=7,XM=A1,YP=A2,YM=6; //320x480 ID=0x6814
const int TS_LEFT=130,TS_RT=900,TS_TOP=85,TS_BOT=920;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

Adafruit_GFX_Button v3V3_on_btn, v3V3_off_btn;
Adafruit_GFX_Button v5V_on_btn, v5V_off_btn;
Adafruit_GFX_Button v12V_on_btn, v12V_off_btn;



struct textLoc {
    
    int vX;
    int vY;

    int aX;
    int aY;  

    int pX;
    int pY; 

    int buttonEN_X;
    int buttonEN_Y;
    int buttonDIS_X;
    int buttonDIS_Y;
};

struct textLoc xy3V3; 
struct textLoc xy5V;
struct textLoc xy12V;


int timeLastChar = 0;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
  bool pressed = false;

    tp = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
   //if (tp.z > MINPRESSURE || tp.z < MAXPRESSURE)
  // {
    pressed= (tp.z > MINPRESSURE && tp.z < MAXPRESSURE);
    if (pressed) {
    pixel_x = map(tp.y, 85, 920, 0, 480);
    pixel_y = map(tp.x, 900, 130, 0, 320);
    }
  /*
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(0, 0);
    tft.print("tp.x=" + String(pixel_x) + " tp.y=" + String(pixel_y) + "    ");
    tft.setCursor(0, 25);
    tft.print(String(tp.z)+ "    ");
    */
    //   }
    return pressed;
}



int v3V3_x_offset = 10;
int v5_x_offset   = 170;
int v12_x_offset = 330;
const byte numChars = 128;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};

boolean newData = false;

struct PS_val{
    char V[6];
    char A[6];
    char W[6];
};

struct powerForms{
  PS_val v3V3;
  PS_val v5V;
  PS_val v12V;
};


/*
char v3V3_V[6];
char v3V3_A[6];
char v3V3_W[6];
char v5V_V[6];
char v5V_A[6];
char v5V_W[6];
char v12V_V[6];
char v12V_A[6];
char v12V_W[6];

char v12V_A_extra[6];
*/

void setup(void)
{
    Serial.begin(115200);
     //mySerial.begin(9600);
    tft.reset();
    uint16_t ID = tft.readID();
    
    tft.begin(ID);
    tft.setRotation(1);            //LANDSCAPE
    tft.fillScreen(BLACK);
    

    
xy3V3.vX = 25;
xy3V3.vY = 50;
xy3V3.aX = 25;
xy3V3.aY = xy3V3.vY+35;
xy3V3.pX = xy3V3.aX+25;
xy3V3.pY = xy3V3.aY+35;

xy5V.vX = 25+v5_x_offset;
xy5V.vY = 50;
xy5V.aX = 25+v5_x_offset;
xy5V.aY = xy5V.vY+ 35;
xy5V.pX = xy5V.aX + 5;
xy5V.pY = xy5V.aY+35;

xy12V.vX = 25+v12_x_offset;
xy12V.vY = 50;
xy12V.aX = 25+v12_x_offset;
xy12V.aY = xy12V.vY+35;
xy12V.pX = xy12V.aX + 5;
xy12V.pY = xy12V.aY+35;

xy3V3.buttonEN_X  = 85;
xy3V3.buttonEN_Y  = 200;
xy3V3.buttonDIS_X = 85;
xy3V3.buttonDIS_Y = 275;

xy5V.buttonEN_X  = 250;
xy5V.buttonEN_Y  = 200;
xy5V.buttonDIS_X = 250;
xy5V.buttonDIS_Y = 275;

xy12V.buttonEN_X  = 410;
xy12V.buttonEN_Y  = 200;
xy12V.buttonDIS_X = 410;
xy12V.buttonDIS_Y = 275;

    //v3V3_on_btn.initButton(&tft,  xy3V3.pX+30, xy3V3.pY+75, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
    //v3V3_off_btn.initButton(&tft, xy3V3.pX+30, xy3V3.pY+150, 100, 65, WHITE, WHITE, RED, "Disable", 2);

    v3V3_on_btn.initButton(&tft,  xy3V3.buttonEN_X, xy3V3.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
    v3V3_off_btn.initButton(&tft, xy3V3.buttonDIS_X, xy3V3.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
    v3V3_on_btn.drawButton(true);
    v3V3_off_btn.drawButton(true);


    v5V_on_btn.initButton(&tft,  xy5V.buttonEN_X, xy5V.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
    v5V_off_btn.initButton(&tft, xy5V.buttonDIS_X, xy5V.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
    v5V_on_btn.drawButton(true);
    v5V_off_btn.drawButton(true);

    v12V_on_btn.initButton(&tft,  xy12V.buttonEN_X, xy12V.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
    v12V_off_btn.initButton(&tft, xy12V.buttonDIS_X, xy12V.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
    v12V_on_btn.drawButton(true);
    v12V_off_btn.drawButton(true);


    


  //drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  tft.drawRoundRect(v3V3_x_offset, 0, 150, 315, 10, GREEN);
  tft.fillRoundRect(v3V3_x_offset, 0, 150, 30 , 5, GREEN);
  showmsgXY(50,  5, 3, "3.3V", BLACK);

  showmsgXY(xy3V3.vX+115,  xy3V3.vY, 3, "V", WHITE);
  tft.drawFastHLine(25,xy3V3.vY + 27 , 100, WHITE);
  showmsgXY(xy3V3.aX+115,  xy3V3.aY, 3, "A", WHITE);
  showmsgXY(xy3V3.pX+90,  xy3V3.pY, 2, "W", YELLOW);

 
  //updateOutVals(0,0,0);

  tft.drawRoundRect(v5_x_offset, 0, 150, 315, 10, 0xFFE0);
  tft.fillRoundRect(v5_x_offset, 0, 150, 30 , 5, 0xFFE0);
  showmsgXY(v5_x_offset+60,  5, 3, "5V", BLACK);
  showmsgXY(xy5V.vX+105,  xy5V.vY, 3, "V", WHITE);
  tft.drawFastHLine(xy5V.vX - 10, xy5V.vY + 27 , 100, WHITE);
  showmsgXY(xy5V.aX+105,  xy5V.aY, 3, "A", WHITE);
  showmsgXY(xy5V.pX+105,  xy5V.pY, 2, "W", YELLOW);



 // updateOutVals(0,0,1, WHITE);

  tft.drawRoundRect(v12_x_offset, 0, 150, 315, 10, 0x001F);
  tft.fillRoundRect(v12_x_offset, 0, 150, 30 , 5, 0x001F);
  showmsgXY(v12_x_offset + 50,  5, 3, "12V", BLACK);
  showmsgXY(xy12V.vX+105,  xy12V.vY, 3, "V", WHITE);
  
  tft.drawFastHLine(xy12V.vX-10, xy12V.vY + 27 , 100, WHITE);
  showmsgXY(xy12V.aX+105,  xy12V.aY, 3, "A", WHITE);
  showmsgXY(xy12V.pX+105,  xy12V.pY, 2, "W", YELLOW);


//  updateOutVals(0,0,2, WHITE);

   tft.setTextColor(WHITE, BLACK);
   Serial.println("Starting....");
}



/* two buttons are quite simple
 */
void loop(void)
{

struct powerForms serialData;

recvWithStartEndMarkers();
  if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData(&serialData);

        //Serial.println(newData);
        

    updateOutVals(0, WHITE,&serialData);
    updateOutVals(1, WHITE,&serialData);
    updateOutVals(2, WHITE,&serialData);

  /* 
    Serial.print(serialData.v3V3.V);
    Serial.print("- ");
    Serial.print(serialData.v3V3.A);
    Serial.print("- ");
    Serial.println(serialData.v3V3.W);
    
    Serial.print(serialData.v5V.V);
    Serial.print("- ");
    Serial.print(serialData.v5V.A);
    Serial.print("- ");
    Serial.println(serialData.v5V.W);
    
    Serial.print(serialData.v12V.V);
    Serial.print("- ");
    Serial.print(serialData.v12V.A);
    Serial.print("- ");
    Serial.println(serialData.v12V.W);
*/
     //delay(5000);
   
      
    //delay(5000);
    Serial.println("<XXX>");
   
    newData = false;
  
  }

    int randVar = 0;
    float randFloatV = 0;
    float randFloatA = 0;

    bool down = Touch_getXY();

   // if (millis() - timerCheck > 1000)
   // {
  //    Serial.println("Heartbeat me");
  //    timerCheck = millis();
  //  }

      
   if (down)
   {
      v3V3_on_btn.press(v3V3_on_btn.contains(pixel_x, pixel_y));
      v3V3_off_btn.press( v3V3_off_btn.contains(pixel_x, pixel_y));
  
      v5V_on_btn.press( v5V_on_btn.contains(pixel_x, pixel_y));
      v5V_off_btn.press( v5V_off_btn.contains(pixel_x, pixel_y));
  
      v12V_on_btn.press( v12V_on_btn.contains(pixel_x, pixel_y));
      v12V_off_btn.press(v12V_off_btn.contains(pixel_x, pixel_y));

      
   // }

    if (v3V3_on_btn.isPressed()){
          v3V3_on_btn.initButton(&tft,  xy3V3.buttonEN_X, xy3V3.buttonEN_Y, 100, 65, WHITE, WHITE, GREEN, "Enable", 2);
          v3V3_off_btn.initButton(&tft, xy3V3.buttonDIS_X, xy3V3.buttonDIS_Y, 100, 65, WHITE, RED, BLACK, "Disable", 2);
         v3V3_on_btn.drawButton(true);
         v3V3_off_btn.drawButton(true);
        Serial.println("<3V3_ON>");
    }
    if (v3V3_off_btn.isPressed()){
           v3V3_on_btn.initButton(&tft,  xy3V3.buttonEN_X, xy3V3.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
          v3V3_off_btn.initButton(&tft, xy3V3.buttonDIS_X, xy3V3.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
           v3V3_on_btn.drawButton(true);
            v3V3_off_btn.drawButton(true);
         Serial.println("<3V3_OFF>");
    }

    if (v5V_on_btn.isPressed()){
        v5V_on_btn.initButton(&tft,  xy5V.buttonEN_X, xy5V.buttonEN_Y, 100, 65, WHITE, WHITE, GREEN, "Enable", 2);
          v5V_off_btn.initButton(&tft, xy5V.buttonDIS_X, xy5V.buttonDIS_Y, 100, 65, WHITE, RED, BLACK, "Disable", 2);
          v5V_on_btn.drawButton(true);
          v5V_off_btn.drawButton(true);
         Serial.println("<5V_ON>");
    }
    if (v5V_off_btn.isPressed()){
           v5V_on_btn.initButton(&tft,  xy5V.buttonEN_X, xy5V.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
          v5V_off_btn.initButton(&tft, xy5V.buttonDIS_X, xy5V.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
          v5V_on_btn.drawButton(true);
          v5V_off_btn.drawButton(true);
          Serial.println("<5V_OFF>");
    }


    if (v12V_on_btn.isPressed()){
          v12V_on_btn.initButton(&tft,  xy12V.buttonEN_X, xy12V.buttonEN_Y, 100, 65, WHITE, WHITE, GREEN, "Enable", 2);
          v12V_off_btn.initButton(&tft, xy12V.buttonDIS_X, xy12V.buttonDIS_Y, 100, 65, WHITE, RED, BLACK, "Disable", 2);
          v12V_on_btn.drawButton(true);
          v12V_off_btn.drawButton(true);
          Serial.println("<12V_ON>");
    }
    if (v12V_off_btn.isPressed()){
          v12V_on_btn.initButton(&tft,  xy12V.buttonEN_X, xy12V.buttonEN_Y, 100, 65, WHITE, GREEN, BLACK, "Enable", 2);
          v12V_off_btn.initButton(&tft, xy12V.buttonDIS_X, xy12V.buttonDIS_Y, 100, 65, WHITE, WHITE, RED, "Disable", 2);
          v12V_on_btn.drawButton(true);
          v12V_off_btn.drawButton(true);
          Serial.println("<12V_OFF>");
    }


    

   }
    



    
  

   
}


void showmsgXY(int x, int y, int sz, const char *msg, int color)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    //tft.drawFastHLine(0, y, tft.width(), WHITE);
    tft.setCursor(x, y);
    tft.setTextColor(color);
    tft.setTextSize(sz);
    tft.print(msg);
    delay(1000);
}

void updateOutVals(int channel, int color,struct powerForms *serialData)
{
   char convert[12];
 tft.setTextColor(color, BLACK);
  tft.setTextSize(3);
  if(channel == 0)
  {
    tft.setCursor(xy3V3.vX, xy3V3.vY);
    tft.print(serialData->v3V3.V);
    tft.setCursor(xy3V3.aX, xy3V3.aY);
    tft.print(serialData->v3V3.A);    

    tft.setTextColor(YELLOW, BLACK);
    tft.setTextSize(2);
    tft.setCursor(xy3V3.pX, xy3V3.pY);
    tft.print(serialData->v3V3.W);
   }
   else if (channel == 1)
   {
    tft.setCursor(xy5V.vX, xy5V.vY);
    tft.print(serialData->v5V.V);
    tft.setCursor(xy5V.aX, xy5V.aY);
    tft.print(serialData->v5V.A);   
    tft.setTextSize(2);
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(xy5V.pX, xy5V.pY);
    tft.print(serialData->v5V.W);
   }
    else if (channel == 2)
   {
    tft.setCursor(xy12V.vX, xy12V.vY);
    tft.print(serialData->v12V.V);
    tft.setCursor(xy12V.aX, xy12V.aY);
    tft.print(serialData->v12V.A);
  //  tft.setCursor(xy12V.aX, xy12V.aY);
  //  tft.print(v12V_A_extra);   
    tft.setTextSize(2);
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(xy12V.pX, xy12V.pY);
    tft.print(serialData->v12V.W);
   }
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
/*
    if(recvInProgress && ((millis() - timeLastChar) > 100))
    {
      recvInProgress = false;
      ndx = 0;
    }
 */
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        //Serial.print(rc);
        
        timeLastChar = millis();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}



void parseData(struct powerForms *serialData) {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index
  //  Serial.println(" ");
   // Serial.println(tempChars);
    char * pch;
    int ii = 0;
    int delim_loc[9];




    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(serialData->v3V3.V, strtokIndx);     // convert this part to an integer
  
    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v3V3.A, strtokIndx);
  
    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v3V3.W, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v5V.V, strtokIndx);
    
    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v5V.A, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v5V.W, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v12V.V, strtokIndx);
    //Serial.println(strtokIndx);
    
    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v12V.A, strtokIndx);   

    strtokIndx = strtok(NULL, ",");
    strcpy(serialData->v12V.W, strtokIndx);



}



#endif

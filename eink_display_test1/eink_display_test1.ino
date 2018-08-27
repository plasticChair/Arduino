
#include <GxEPD.h>
#include <GxFont_GFX.h>
//#include "icon_set.h"
#include "ico_l1.h"
#include "ico_l2.h"
#include "battery.h"
#include "cal.h"
#include "clock.h"
#include "comp.h"
#include "distance.h"
#include "up.h"
#include "down.h"
#include "humidity.h"
#include "mountains.h"
#include "temp.h"
#include "statusTabInactive.h"
#include "statusTabActive.h"

#include "newFont.h"
#include "newFontSmall.h"

void screenSetDistance(float distanceIN);
void screenSetAlt(float altIN, float altHIN, float altLIN);
void screenSetSpeed(float speedIN);
void screenSetCompass(float directionIN);
void screenSetTemp(float tempIN, float tempHIN, float tempLIN);
void screenSetHumidity(float humidityIN);
void screenSetTime(char* timeIN);
void screenSetDate(char* dateIN);
void screenSetBattery(float batteryIN);
void screenSetStatus( bool statusIN, int statusNum);

#define screenSize  200
#define statusBarSize  30
#define numRows  4
#define numCols  2

#define lineWidth 2  // Pixels
#define yScreenSize (screenSize-statusBarSize)
#define rowHeight (yScreenSize/numRows)
#define rowWidth (screenSize/numCols)
#define xIconSpace (yScreenSize/numRows)
#define iconSize 25
#define iconSizeSmall 15
#define iconEdgeSpace 5
  
void dispLayout();
void tempFun1();
void tempFun2();
void screenInit();
/************************************************************************************
   GxEPD_SPI_TestExample : test example for e-Paper displays from Dalian Good Display Co., Ltd.: www.good-display.com

   based on Demo Example from Good Display, now available on http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html

   Author : J-M Zingg

   Version : see library.properties

   Support: limited, provided as example, no claim to be fit for serious use

*/

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Dispay ePaper for Arduino : https://forum.arduino.cc/index.php?topic=436411.0

// mapping suggestion from Waveshare SPI e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one

#include <GxGDEW0154Z04/GxGDEW0154Z04.cpp>  // 1.54" b/w/r


//#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

//GxIO_Class io(SPI, SS, 0, 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class


GxIO_Class io(SPI, 10, 8, 6,7); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io);





struct screenPos{
  int X;
  int Y;
};

struct screenField{
  screenPos scDate;
  screenPos scTime;
  screenPos humid; 
  screenPos curAlt;
  screenPos distance;
  screenPos curTemp;
  screenPos tempH;
  screenPos tempL;
  screenPos altGain;
  screenPos altLow;
  screenPos tripTime;
  screenPos comp;
  screenPos avgSpd;
  screenPos battV;

  screenPos iconDT;
  screenPos iconAlt;
  screenPos iconHum;
  screenPos iconDis;
  screenPos iconTemp;
  screenPos iconTempH;
  screenPos iconTempL;
  screenPos iconAltH;
  screenPos iconAltL;
  screenPos iconComp;
  screenPos iconSpd;
  screenPos iconBatt;

  screenPos iconStatusBar1;
  screenPos iconStatusBar2;
  screenPos iconStatusBar3;
  screenPos iconStatusBar4;
  screenPos iconStatusBar5;

} screen;

  int iconPosY[2] = {1, (rowWidth + iconEdgeSpace)};
  int iconPosX[4] = {iconEdgeSpace, (xIconSpace+ iconEdgeSpace), (xIconSpace*2 + iconEdgeSpace), (xIconSpace*3 + iconEdgeSpace)};

void setup()
{
  Serial.begin(115200);
  Serial.println();
   Serial.println();
    Serial.println();
  Serial.println("setup");

  

  display.init(115200); // enable diagnostic output on Serial

  Serial.println("setup done");
screenInit();
// display.drawPicture(gImage_ico_l1,gImage_ico_l2, sizeof(gImage_ico_l1),sizeof(gImage_ico_l2));
  
}

void loop()
{







dispLayout();
Serial.println("done layout");
//display.update();
delay(1000);

screenSetDistance(5.6);
screenSetAlt(1238, 456,-187);
screenSetSpeed(2.3);
screenSetCompass(254.8);
screenSetTemp(25.3, 32.2, 15.2);
screenSetHumidity(38.2);
screenSetTime("2:24");
screenSetDate("04/13/18");
screenSetBattery(3.84);


 screenSetStatus(true, 0);
 screenSetStatus( true, 3);
screenSetStatus( true, 4);
 // screenSetStatus(" GPS \n Track", true, 2);
//screenSetStatus(" GPS \n On", true, 3);
 // screenSetStatus(" SD \n Write", true, 5);

//display.updateWindow(screen.humid.Y, screen.humid.X-15, 35,15, false);
//delay(1000);
//display.fillRect(screen.humid.Y, screen.humid.X-15, 35, 15, GxEPD_BLACK);
//display.updateWindow(screen.humid.Y, screen.humid.X-15, 35,15, false);
//delay(1000);
display.update();

//display.updateWindow(box_x, box_y, box_w, box_h, true);



 
  delay(100000);
}



void dispLayout()
{


 
  
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);

 //Vertical line
  display.fillRect(screenSize/numCols, 0, lineWidth, screenSize-statusBarSize, GxEPD_BLACK);
  //Horz
  display.fillRect(0,rowHeight, screenSize/2, lineWidth, GxEPD_BLACK);
  display.fillRect(0,rowHeight*2, screenSize/2, lineWidth, GxEPD_BLACK);
  display.fillRect(screenSize/2,rowHeight*2-10, screenSize, lineWidth, GxEPD_BLACK);
  display.fillRect(0, rowHeight*3, screenSize, lineWidth, GxEPD_BLACK);
  display.fillRect(0, screenSize - statusBarSize, screenSize, lineWidth, GxEPD_BLACK);
  

  // Draw Icons
  display.drawBitmap(gImage_cal          , screen.iconDT.Y   , screen.iconDT.X   , iconSize     , iconSize     , GxEPD_BLACK,1);
  display.drawBitmap(gImage_humidity     , screen.iconHum.Y  , screen.iconHum.X  , iconSize     , iconSize     , GxEPD_BLACK, 0);
  display.drawBitmap(gImage_temp         , screen.iconTemp.Y , screen.iconTemp.X , iconSize     , iconSize     , GxEPD_BLACK);
  display.drawBitmap(gImage_up           , screen.iconTempH.Y, screen.iconTempH.X, iconSizeSmall, iconSizeSmall, GxEPD_BLACK);
  display.drawBitmap(gImage_down         , screen.iconTempL.Y, screen.iconTempL.X, iconSizeSmall, iconSizeSmall, GxEPD_BLACK);
  display.drawBitmap(gImage_comp         , screen.iconComp.Y , screen.iconComp.X   , iconSize     , iconSize     , GxEPD_BLACK);
  display.drawBitmap(gImage_mountains    , screen.iconAlt.Y  , screen.iconAlt.X  , iconSize     , iconSize     , GxEPD_BLACK);
  display.drawBitmap(gImage_distance     , screen.iconDis.Y  , screen.iconDis.X  , iconSize     , iconSize     , GxEPD_BLACK);
  display.drawBitmap(gImage_up           , screen.iconAltH.Y, screen.iconAltH.X, iconSizeSmall, iconSizeSmall     , GxEPD_BLACK);
  display.drawBitmap(gImage_down         , screen.iconAltL.Y, screen.iconAltL.X, iconSizeSmall, iconSizeSmall     , GxEPD_BLACK);
  display.drawBitmap(gImage_battery      , screen.iconBatt.Y , screen.iconBatt.X , iconSize     , iconSize     , GxEPD_BLACK);


   display.fillRect(0, yScreenSize, screenSize, statusBarSize, GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar1.Y+1 , screen.iconStatusBar1.X , statusBarSize     , 40     , GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar2.Y+1 , screen.iconStatusBar2.X , statusBarSize     , 40     , GxEPD_BLACK);
  ///display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar3.Y+1, screen.iconStatusBar3.X , statusBarSize     , 40     , GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar4.Y+1, screen.iconStatusBar4.X , statusBarSize     , 40     , GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar5.Y+1, screen.iconStatusBar5.X , statusBarSize     , 40     , GxEPD_BLACK);

//display.update();

 screenSetStatus( false, 0);
 screenSetStatus( false, 1);
 screenSetStatus( false, 2);
screenSetStatus( false, 3);
screenSetStatus( false, 4);
 
 //display.drawBitmap(gImage_ico_l1, 0,0, 200, 200, GxEPD_BLACK);
 // display.drawBitmap(gImage_ico_l2, 0,0, 200, 200, GxEPD_BLACK);

}

void screenSetDate(char* dateIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.scDate.Y,  screen.scDate.X);
  display.setTextColor(GxEPD_RED);
  display.print(dateIN);
}

void screenSetTime(char* timeIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.scTime.Y,  screen.scTime.X);
  display.setTextColor(GxEPD_RED);
  display.print(timeIN);
}



void screenSetHumidity(float humidityIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.humid.Y,  screen.humid.X);
  display.setTextColor(GxEPD_RED);
  display.print(humidityIN,1);
  display.setTextColor(GxEPD_BLACK);
  display.print(" %");
}

void screenSetTemp(float tempIN, float tempHIN, float tempLIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.curTemp.Y,  screen.curTemp.X);
  display.setTextColor(GxEPD_RED);
  display.print(tempIN,1);
  display.setTextColor(GxEPD_BLACK);
  display.drawCircle(screen.curTemp.Y+35, screen.curTemp.X-13,2,GxEPD_BLACK);
  f = &SansSerif_bold_13;
  display.setFont(f);
  display.print("C");
  

  f = &SansSerif_bold_13;
  display.setFont(f);
  
  display.setCursor( screen.tempH.Y,  screen.tempH.X);
  display.setTextColor(GxEPD_RED);
  display.print(tempHIN,0);
  display.setTextColor(GxEPD_BLACK);
  display.print((char)223);
  //display.print("C");
  
  display.setCursor( screen.tempL.Y,  screen.tempL.X);
  display.setTextColor(GxEPD_RED);
  display.print(tempLIN,0);
  display.setTextColor(GxEPD_BLACK);
  display.print((char)223);
  //display.print("C");
  
}

void screenSetCompass(float directionIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.comp.Y,  screen.comp.X);
  display.setTextColor(GxEPD_RED);
  display.print(directionIN,0);
  display.drawCircle(screen.comp.Y+35, screen.comp.X-12,2,GxEPD_BLACK);
  display.print((char)223);
}
void screenSetSpeed(float speedIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.avgSpd.Y,  screen.avgSpd.X);
  display.setTextColor(GxEPD_RED);
  display.print(speedIN,1);
  f = &SansSerif_bold_13;
  display.setFont(f);
  
  display.setTextColor(GxEPD_BLACK);
  display.print(" mph");
}
void screenSetAlt(float altIN, float altHIN, float altLIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.curAlt.Y,  screen.curAlt.X);
  display.setTextColor(GxEPD_RED);
  display.print(altIN,0);
  display.setTextColor(GxEPD_BLACK);
  display.print(" ft");

  display.setCursor( screen.altGain.Y,  screen.altGain.X);
  display.setTextColor(GxEPD_RED);
  display.print(altHIN,0);
  display.setTextColor(GxEPD_BLACK);
  display.print(" ft");

  display.setCursor( screen.altLow.Y,  screen.altLow.X-1);
  display.setTextColor(GxEPD_RED);
  display.print(altLIN,0);
  display.setTextColor(GxEPD_BLACK);
  display.print(" ft");  
}
void screenSetDistance(float distanceIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.distance.Y,  screen.distance.X);
  display.setTextColor(GxEPD_RED);
  display.print(distanceIN,2);
  display.setTextColor(GxEPD_BLACK);
  display.print(" mi");

  display.setCursor( screen.distance.Y,  screen.distance.X+20);
  display.setTextColor(GxEPD_RED);
  display.print("002:15");
  display.setTextColor(GxEPD_BLACK);
  //display.print(" {H:M:S)");
}

void screenSetBattery(float batteryIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.battV.Y,  screen.battV.X);
  display.setTextColor(GxEPD_RED);
  display.print(batteryIN,2);
  display.setTextColor(GxEPD_BLACK);
  display.print(" V");

  f = &SansSerif_bold_11;
  display.setFont(f);
  display.setCursor( screen.battV.Y-25,  screen.battV.X+22);
  display.setTextColor(GxEPD_RED);
  display.print("01  14  53");
  display.setTextColor(GxEPD_BLACK);
  display.setCursor( screen.battV.Y-23,  screen.battV.X+22);
  display.print("   d    h    m");
}

void screenSetStatus( bool statusIN, int statusNum)
{

  int vertStart = 40*(statusNum+1) - 3;
  const GFXfont* f = &SansSerif_bold_8;
  int statusTxtBoarderOffset, statusTxtNewLineOffset;
  display.setFont(f);
    
  if (!statusIN)  {
    // Draw Black background
    display.drawBitmap(gImage_statusTabActive      , 40*statusNum , screen.iconStatusBar1.X , 40     , 30     , GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);   
  }
  else {
    // Draw background behind red tab
    display.drawBitmap(gImage_statusTabActive, 40*statusNum , screen.iconStatusBar1.X , 40     , statusBarSize     , GxEPD_RED);
    display.fillRect(40*(statusNum)      , yScreenSize, 40,  3, GxEPD_BLACK);
    display.fillRect(40*(statusNum+1) - 2, yScreenSize,  2, 30, GxEPD_BLACK);
      
    display.fillRect(40*(statusNum+1) - 3, yScreenSize  , 1, 8, GxEPD_BLACK);
    display.fillRect(40*(statusNum+1) - 7, yScreenSize+3, 5, 1, GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-5 , yScreenSize+4,       GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-4 , yScreenSize+4,       GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-4 , yScreenSize+5,       GxEPD_BLACK);
  }

  statusTxtBoarderOffset = 15;
  statusTxtNewLineOffset = statusTxtBoarderOffset +10;
  if(statusNum == 0)  {
    display.setCursor(40*statusNum,  yScreenSize+15  );
    if(statusIN){
      display.setTextColor(GxEPD_WHITE);  
      display.print("  GPS \n  Acq");
    }
    else {
      display.setTextColor(GxEPD_BLACK);  
      display.print("  GPS \n  Off");
    }
    
  }
  else if(statusNum == 1)
  {
     if(statusIN){
      display.setTextColor(GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("  GPS");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print(" Track");
    }
    else{
     display.setTextColor(GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("  GPS");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print(" Track"); 
    }
  }
  
  else if(statusNum == 2)
  {
     if(statusIN){
      display.setTextColor(GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("  GPS");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print("  On");
    }
    else{
      display.setTextColor(GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("  GPS");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print("   Off"); 
    }
  }    
  else if(statusNum == 3)
  {
     if(statusIN){
      display.setTextColor(GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("   BT");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print("   On");
    }
    else{
      display.setTextColor(GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("   BT");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print("   Off"); 
    }

  }
  else if(statusNum == 4)
  {
     if(statusIN){
      display.setTextColor(GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("   SD");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print("   OFF");
    }
    else{
     display.setTextColor(GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("   SD");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print(" Write"); 
    }
  }
}
void screenInit()
{
int textSize = 10;
  //Column 1
  screen.scDate    = {iconPosX[0]+textSize  , 28};
  screen.scTime    = {iconPosX[0]+textSize +20, 35};
  screen.humid     = {iconPosX[1] + 20, 30};
  screen.curTemp   = {iconPosX[2] + 20, 20};
  screen.tempH     = {iconPosX[2] +10, 80};
  screen.tempL     = {iconPosX[2] +30, 80};
  screen.comp      = {iconPosX[3] +textSize+2, 35};
  screen.avgSpd    = {iconPosX[3] + textSize+20, 35};
  

  // Column 2
  screen.curAlt    = {iconPosX[0] + 20, iconPosY[1] + 25};
  screen.altGain   = {iconPosX[1]     , iconPosY[1] + 35};
  screen.altLow    = {iconPosX[1] + 20, iconPosY[1] + 30};
  screen.distance  = {iconPosX[2] + 10, iconPosY[1] + 30};
  screen.tripTime  = {iconPosX[2] + 30, iconPosY[1] + 30};                                                     
  screen.battV     = {iconPosX[3] + 15, iconPosY[1] + 30};

  //Column 1
  screen.iconDT    = {iconPosX[0]   , iconPosY[0]   };
  screen.iconHum   = {iconPosX[1]   , iconPosY[0]   };
  screen.iconTemp  = {iconPosX[2]+5   , iconPosY[0]-5   };
  screen.iconTempH = {iconPosX[2]-2   , iconPosY[0]+65};
  screen.iconTempL = {iconPosX[2]+18, iconPosY[0]+65};
  screen.iconComp  = {iconPosX[3]+5   , iconPosY[0]   };
  screen.iconSpd   = {iconPosX[3]   , iconPosY[0]   };
                     
   // Column 2       
  screen.iconAlt   = {iconPosX[0], iconPosY[1]};
  screen.iconDis   = {iconPosX[2]-5, iconPosY[1]};
//  screen.iconTempH = {iconPosX[2], iconPosY[1]+30};
//  screen.iconTempL = {iconPosX[2]+10, iconPosY[1]+30};
  screen.iconAltH  = {iconPosX[0]+30, iconPosY[1]+10};
  screen.iconAltL  = {iconPosX[0]+45, iconPosY[1]+10};
  screen.iconBatt  = {iconPosX[3], iconPosY[1]};

  //Status bar
  screen.iconStatusBar1 = {yScreenSize   , 0   };
  screen.iconStatusBar2 = {yScreenSize   , 40   };
  screen.iconStatusBar3 = {yScreenSize   , 80   };
  screen.iconStatusBar4 = {yScreenSize   , 120   };
  screen.iconStatusBar5 = {yScreenSize   , 160   };

  //screen.iconStatusBar5 = {yScreenSize   , 160   };
 
}



//updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation = true)
//eraseDisplay(bool using_partial_update = false) 
//update();
// fillScreen(uint16_t color)
//drawPixel(int16_t x, int16_t y, uint16_t color)
//fillRect(0, 0, 8, 8, GxEPD_BLACK);


//display.setTextColor(GxEPD_RED);
//display.setCursor(0, 0);
/*
void showFont(const char name[], const GFXfont* f)
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
  display.setTextColor(GxEPD_RED);
#endif
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
  display.update();
  delay(5000);
}

void showFontCallback()
{
  const char* name = "FreeMonoBold9pt7b";
  
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
  display.setTextColor(GxEPD_RED);
#endif
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
}

void drawCornerTest()
{
 // display.drawCornerTest();
  delay(5000);
  uint8_t rotation = display.getRotation();
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
    display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
    display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
    display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
    display.update();
    delay(5000);
  }
  display.setRotation(rotation); // restore
}

void showBitmapExample()
{
  display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1));
  delay(2000);
  display.drawExampleBitmap(BitmapExample2, sizeof(BitmapExample2));
  delay(5000);
  display.fillScreen(GxEPD_WHITE);
  display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
  display.update();
  delay(5000);
 
}
*/



#include "screenControl.h"
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
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


void screenControl::screenControl(GxGDEW0154Z04 _display){GxGDEW0154Z04 display = _display};

void screenControl::init()
{

  screenField screen;
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

  int iconPosY[2] = {1, (rowWidth + iconEdgeSpace)};
  int iconPosX[4] = {iconEdgeSpace, (xIconSpace+ iconEdgeSpace), (xIconSpace*2 + iconEdgeSpace), (xIconSpace*3 + iconEdgeSpace)};

}
  
void void screenControl::dispLayout()
{


 
  
  display.setRotation(0);
  display.fillScreen(display.GxEPD_WHITE);

 //Vertical line
  display.fillRect(screenSize/numCols, 0, lineWidth, screenSize-statusBarSize, display.GxEPD_BLACK);
  //Horz
  display.fillRect(0,rowHeight, screenSize/2, lineWidth, display.GxEPD_BLACK);
  display.fillRect(0,rowHeight*2, screenSize/2, lineWidth, display.GxEPD_BLACK);
  display.fillRect(screenSize/2,rowHeight*2-10, screenSize, lineWidth, display.GxEPD_BLACK);
  display.fillRect(0, rowHeight*3, screenSize, lineWidth, display.GxEPD_BLACK);
  display.fillRect(0, screenSize - statusBarSize, screenSize, lineWidth, display.GxEPD_BLACK);
  

  // Draw Icons
  display.drawBitmap(gImage_cal          , screen.iconDT.Y   , screen.iconDT.X   , iconSize     , iconSize     , display.GxEPD_BLACK,1);
  display.drawBitmap(gImage_humidity     , screen.iconHum.Y  , screen.iconHum.X  , iconSize     , iconSize     , display.GxEPD_BLACK, 0);
  display.drawBitmap(gImage_temp         , screen.iconTemp.Y , screen.iconTemp.X , iconSize     , iconSize     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_up           , screen.iconTempH.Y, screen.iconTempH.X, iconSizeSmall, iconSizeSmall, display.GxEPD_BLACK);
  display.drawBitmap(gImage_down         , screen.iconTempL.Y, screen.iconTempL.X, iconSizeSmall, iconSizeSmall, display.GxEPD_BLACK);
  display.drawBitmap(gImage_comp         , screen.iconComp.Y , screen.iconComp.X   , iconSize     , iconSize     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_mountains    , screen.iconAlt.Y  , screen.iconAlt.X  , iconSize     , iconSize     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_distance     , screen.iconDis.Y  , screen.iconDis.X  , iconSize     , iconSize     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_up           , screen.iconAltH.Y, screen.iconAltH.X, iconSizeSmall, iconSizeSmall     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_down         , screen.iconAltL.Y, screen.iconAltL.X, iconSizeSmall, iconSizeSmall     , display.GxEPD_BLACK);
  display.drawBitmap(gImage_battery      , screen.iconBatt.Y , screen.iconBatt.X , iconSize     , iconSize     , display.GxEPD_BLACK);


   display.fillRect(0, yScreenSize, screenSize, statusBarSize, display.GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar1.Y+1 , screen.iconStatusBar1.X , statusBarSize     , 40     , display.GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar2.Y+1 , screen.iconStatusBar2.X , statusBarSize     , 40     , display.GxEPD_BLACK);
  ///display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar3.Y+1, screen.iconStatusBar3.X , statusBarSize     , 40     , display.GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar4.Y+1, screen.iconStatusBar4.X , statusBarSize     , 40     , display.GxEPD_BLACK);
  //display.drawBitmap(gImage_statusTabInactive      , screen.iconStatusBar5.Y+1, screen.iconStatusBar5.X , statusBarSize     , 40     , display.GxEPD_BLACK);

//display.update();

 setStatus("", false, 0);
 setStatus("", false, 1);
 setStatus("", false, 2);
setStatus("", false, 3);
setStatus("", false, 4);
 
 //display.drawBitmap(gImage_ico_l1, 0,0, 200, 200, display.GxEPD_BLACK);
 // display.drawBitmap(gImage_ico_l2, 0,0, 200, 200, display.GxEPD_BLACK);

}

void screenControl::setDate(char* dateIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.scDate.Y,  screen.scDate.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(dateIN);
}

void screenControl::setTime(char* timeIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.scTime.Y,  screen.scTime.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(timeIN);
}



void screenControl::setHumidity(float humidityIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.humid.Y,  screen.humid.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(humidityIN,1);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" %");
}

void screenControl::setTemp(float tempIN, float tempHIN, float tempLIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.curTemp.Y,  screen.curTemp.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(tempIN,1);
  display.setTextColor(display.GxEPD_BLACK);
  display.drawCircle(screen.curTemp.Y+35, screen.curTemp.X-13,2,display.GxEPD_BLACK);
  f = &SansSerif_bold_13;
  display.setFont(f);
  display.print("C");
  

  f = &SansSerif_bold_13;
  display.setFont(f);
  
  display.setCursor( screen.tempH.Y,  screen.tempH.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(tempHIN,0);
  display.setTextColor(display.GxEPD_BLACK);
  display.print((char)223);
  //display.print("C");
  
  display.setCursor( screen.tempL.Y,  screen.tempL.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(tempLIN,0);
  display.setTextColor(display.GxEPD_BLACK);
  display.print((char)223);
  //display.print("C");
  
}

void screenControl::setCompass(float directionIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.comp.Y,  screen.comp.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(directionIN,0);
  display.drawCircle(screen.comp.Y+35, screen.comp.X-12,2,display.GxEPD_BLACK);
  display.print((char)223);
}
void screenControl::setSpeed(float speedIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.avgSpd.Y,  screen.avgSpd.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(speedIN,1);
  f = &SansSerif_bold_13;
  display.setFont(f);
  
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" mph");
}
void screenControl::setAlt(float altIN, float altHIN, float altLIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.curAlt.Y,  screen.curAlt.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(altIN,0);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" ft");

  display.setCursor( screen.altGain.Y,  screen.altGain.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(altHIN,0);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" ft");

  display.setCursor( screen.altLow.Y,  screen.altLow.X-1);
  display.setTextColor(display.GxEPD_RED);
  display.print(altLIN,0);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" ft");  
}
void screenControl::setDistance(float distanceIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.distance.Y,  screen.distance.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(distanceIN,2);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" mi");

  display.setCursor( screen.distance.Y,  screen.distance.X+20);
  display.setTextColor(display.GxEPD_RED);
  display.print("002:15");
  display.setTextColor(display.GxEPD_BLACK);
  //display.print(" {H:M:S)");
}

void screenControl::setBattery(float batteryIN)
{
  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setCursor( screen.battV.Y,  screen.battV.X);
  display.setTextColor(display.GxEPD_RED);
  display.print(batteryIN,2);
  display.setTextColor(display.GxEPD_BLACK);
  display.print(" V");

  f = &SansSerif_bold_11;
  display.setFont(f);
  display.setCursor( screen.battV.Y-25,  screen.battV.X+22);
  display.setTextColor(display.GxEPD_RED);
  display.print("01  14  53");
  display.setTextColor(display.GxEPD_BLACK);
  display.setCursor( screen.battV.Y-23,  screen.battV.X+22);
  display.print("   d    h    m");
}

void screenControl::setStatus(bool statusIN, int statusNum)
{

  int vertStart = 40*(statusNum+1) - 3;
  const GFXfont* f = &SansSerif_bold_8;
  int statusTxtBoarderOffset, statusTxtNewLineOffset;
  display.setFont(f);
    
  if (!statusIN)  {
    // Draw Black background
    display.drawBitmap(gImage_statusTabActive      , 40*statusNum , screen.iconStatusBar1.X , 40     , 30     , display.GxEPD_WHITE);
    display.setTextColor(display.GxEPD_BLACK);   
  }
  else {
    // Draw background behind red tab
    display.drawBitmap(gImage_statusTabActive, 40*statusNum , screen.iconStatusBar1.X , 40     , statusBarSize     , display.GxEPD_RED);
    display.fillRect(40*(statusNum)      , yScreenSize, 40,  3, display.GxEPD_BLACK);
    display.fillRect(40*(statusNum+1) - 2, yScreenSize,  2, 30, display.GxEPD_BLACK);
      
    display.fillRect(40*(statusNum+1) - 3, yScreenSize  , 1, 8, display.GxEPD_BLACK);
    display.fillRect(40*(statusNum+1) - 7, yScreenSize+3, 5, 1, display.GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-5 , yScreenSize+4,       display.GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-4 , yScreenSize+4,       display.GxEPD_BLACK);
    display.drawPixel(40*(statusNum+1)-4 , yScreenSize+5,       display.GxEPD_BLACK);
  }

  statusTxtBoarderOffset = 15;
  statusTxtNewLineOffset = statusTxtBoarderOffset +10;
  if(statusNum == 0)  {
    display.setCursor(40*statusNum,  yScreenSize+15  );
    if(statusIN){
      display.setTextColor(display.GxEPD_WHITE);  
      display.print("  GPS \n  Acq");
    }
    else {
      display.setTextColor(display.GxEPD_BLACK);  
      display.print("  GPS \n  Off");
    }
    
  }
  else if(statusNum == 1)
  {
     if(statusIN){
      display.setTextColor(display.GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("  GPS");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print(" Track");
    }
    else{
     display.setTextColor(display.GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("  GPS");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print(" Track"); 
    }
  }
  
  else if(statusNum == 2)
  {
     if(statusIN){
      display.setTextColor(display.GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("  GPS");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print("  On");
    }
    else{
      display.setTextColor(display.GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("  GPS");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print("   Off"); 
    }
  }    
  else if(statusNum == 3)
  {
     if(statusIN){
      display.setTextColor(display.GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print("   BT");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print("   On");
    }
    else{
      display.setTextColor(display.GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("   BT");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print("   Off"); 
    }

  }
  else if(statusNum == 4)
  {
     if(statusIN){
      display.setTextColor(display.GxEPD_WHITE);  
      display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
      display.print(" SD");
      display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
      display.print(" OFF");
    }
    else{
     display.setTextColor(display.display.GxEPD_BLACK);  
     display.setCursor(40*statusNum,  yScreenSize + statusTxtBoarderOffset  );
     display.print("   SD");
     display.setCursor(40*statusNum,  yScreenSize + statusTxtNewLineOffset  );
     display.print(" Write"); 
    }
  }
}


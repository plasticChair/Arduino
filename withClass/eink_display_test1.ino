#include <GxEPD.h>
#include <GxFont_GFX.h>
#include <GxGDEW0154Z04/GxGDEW0154Z04.cpp>  // 1.54" b/w/r
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>



#include "screenControl.h"




GxIO_Class io(SPI, 10, 8, 6,7); //CS, DC, RST, BL arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io);


screenControl screen(display);

void setup()
{

  
  Serial.begin(115200);
  Serial.println();
   Serial.println();
    Serial.println();
  Serial.println("setup");

 

  Serial.println("setup done");

// display.drawPicture(gImage_ico_l1,gImage_ico_l2, sizeof(gImage_ico_l1),sizeof(gImage_ico_l2));
  
}

void loop()
{


screen.dispLayout();
Serial.println("done layout");
//display.update();

delay(1000);

screen.setDistance(5.6);
screen.setAlt(1238, 456,-187);
screen.setSpeed(2.3);
screen.setCompass(254.8);
screen.setTemp(25.3, 32.2, 15.2);
screen.setHumidity(38.2);
screen.setTime("2:24");
screen.setDate("04/13/18");
screen.setBattery(3.84);


 screen.setStatus( true, 0);
 screen.setStatus( true, 3);

 // setStatus(" GPS \n Track", true, 2);
//setStatus(" GPS \n On", true, 3);
 // setStatus(" SD \n Write", true, 5);

//display.updateWindow(humid.Y, humid.X-15, 35,15, false);
//delay(1000);
//display.fillRect(humid.Y, humid.X-15, 35, 15, GxEPD_BLACK);
//display.updateWindow(humid.Y, humid.X-15, 35,15, false);
//delay(1000);
//display.update();

//display.updateWindow(box_x, box_y, box_w, box_h, true);



 
  delay(100000);
}




 


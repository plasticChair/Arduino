



#include <GxEPD.h>
#include <GxFont_GFX.h>
#include <GxGDEW0154Z04.h>
#include <GxIO.h>
#include <GxIO_SPI.h>





GxIO_SPI io(SPI, 10, 8, 6, 7); //CS, DC, RST, BL arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxGDEW0154Z04 display(io);







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


	display.dispLayout();
	Serial.println("done layout");
	//display.update();

	delay(1000);

	display.setDistance(5.6);
	display.setAlt(1238, 456, -187);
	display.setSpeed(2.3);
	display.setCompass(254.8);
	display.setTemp(25.3, 32.2, 15.2);
	display.setHumidity(38.2);
	display.setTime("2:24");
	display.setDate("04/13/18");
	display.setBattery(3.84);


	display.setStatus(true, 0);
	display.setStatus(true, 3);
	display.setStatus(true, 4);

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






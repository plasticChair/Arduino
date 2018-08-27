#include <Arduino.h>
//#include "icon_set.h"







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




class screenControl
{
	public: 

  screenControl(GxGDEW0154Z04& display);
  //GxGDEW0154Z04 display;
  //screenControl();
	void setDistance(float distanceIN);
	void setAlt(float altIN, float altHIN, float altLIN);
	void setSpeed(float speedIN);
	void setCompass(float directionIN);
	void setTemp(float tempIN, float tempHIN, float tempLIN);
	void setHumidity(float humidityIN);
	void setTime(char* timeIN);
	void setDate(char* dateIN);
	void setBattery(float batteryIN);
	void setStatus(bool statusIN, int statusNum);
	void init();
  void dispLayout();
	
	struct screenPos{
	  int X;
	  int Y;
	};

	typedef struct{
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

	} screenField;
	
	screenField screen;
	private:
	
};




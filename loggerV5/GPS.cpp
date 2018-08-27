// 
// 
// 

#include "GPS.h"


void gpsSetup() {


	// Start the GPS device
	// Serial2.attachInterrupt( GPSisr );
	//attachInterrupt(3,GPSisr,CHANGE);

	digitalWrite(Pin_GPS_pwr, (GPS_Enable));


	// Start the normal trace output
	//DEBUG_PORT.begin(9600);
	// while (!Serial)
	//   ;

	delay(100);

	DEBUG_PORT.print(F("NMEA.INO: started\n"));
	DEBUG_PORT.print(F("  fix object size = "));
	DEBUG_PORT.println(sizeof(gps.fix()));
	DEBUG_PORT.print(F("  gps object size = "));
	DEBUG_PORT.println(sizeof(gps));
	DEBUG_PORT.println(F("Looking for GPS device on "));

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

	DEBUG_PORT.println(F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed."));

#else
	if (gps.merging == NMEAGPS::NO_MERGING) {
		DEBUG_PORT.print(F("\nWARNING: displaying data from "));
		DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
		DEBUG_PORT.print(F(" sentences ONLY, and only if "));
		DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
		DEBUG_PORT.println(F(" is enabled.\n"
			"  Other sentences may be parsed, but their data will not be displayed."));
	}
#endif

	DEBUG_PORT.print(F("\nGPS quiet time is assumed to begin after a "));
	DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
	DEBUG_PORT.println(F(" sentence is received.\n"
		"  You should confirm this with NMEAorder.ino\n"));

	trace_header(DEBUG_PORT);

	DEBUG_PORT.flush();

	// Start the UART for the GPS device
	GPS_PORT.begin(9600);

	delay(100);

	// waitForFix();
}


static void waitForFix()
{
	DEBUG_PORT.print(F("Waiting for GPS fix..."));

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

	gps.overrun(false); // we had to wait a while...

} // waitForFix


static void GPSloop()
{

	//DEBUG_PORT.println("in loop");

	while (gps.available(GPS_PORT)) {
		BLE_PORT.println("read some data");
		fix_data = gps.read();
		doSomeWork();

		if (fix_data.status == 3)
		{
			digitalWrite(Pin_led3, LOW);
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


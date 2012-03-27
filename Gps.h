/*
 * Gps.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef GPS_H_
#define GPS_H_

#include "Arduino.h"

#include <NMEA/nmea.h>
#include <PString/PString.h>
#include "FreeBoardConstants.h"
#include "Lcd.h"

class Gps {
public:
	Gps(NMEA* gpsSource);
	virtual ~Gps();

	/* NMEA GPS routines
	 */
	void showGPSData(Lcd lcd, int menuState);
	bool decode(byte inByte);
	bool getRmcStatus();
	float getRmcCourse();
	float getRmcMetersTo(float lat, float lon);
	char* getLatString(float lat, int decimals, int padding);
	char* getLonString(float lon, int decimals, int padding);
	float getLatitude();
	float getLongitude();
	//bool getGpsDecode();
	unsigned long getLastGpsFix();
	/*
	 Set baud rate and various message frequencies.
	 */
	void setupGps();
private:
	/*
	 Reset the GPS to factory defaults
	 */
	int autoBaud();
	bool testMsg();
	void resetGPS();
	bool gpsDecode;
	unsigned long lastGpsFix;
	unsigned long lastLcdUpdate;
	char latBuffer[40];
	char lonBuffer[40];
	// create a GPS data connection to GPRMC sentence type
	//NMEA gps(GPRMC);
	NMEA* gpsSource;
};
#endif /* GPS_H_ */

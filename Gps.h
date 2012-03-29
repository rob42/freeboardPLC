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
#include "FreeBoardModel.h"
#include "Lcd.h"

class Gps {
public:
	Gps(NMEA* gpsSource, FreeBoardModel* model);
	virtual ~Gps();

	/* NMEA GPS routines
	 */

	bool decode(byte inByte);
	//bool getRmcStatus();
	//float getRmcCourse();
	static float getMetersTo(float targetLat, float targetLon, float currentLat, float currentLon);
	static PString getLatString(float lat, int decimals, int padding, PString str);
	static PString getLonString(float lon, int decimals, int padding, PString str);
	//float getLatitude();
	//float getLongitude();
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
	//bool gpsDecode;
	//unsigned long gpsLastFix;


	// create a GPS data connection to GPRMC sentence type
	//NMEA gps(GPRMC);
	NMEA* gpsSource;
	FreeBoardModel* model;
};
#endif /* GPS_H_ */

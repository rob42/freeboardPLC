/*
 * Copyright 2010,2011,2012,2013 Robert Huitema robert@42.co.nz
 *
 * This file is part of FreeBoard. (http://www.42.co.nz/freeboard)
 *
 *  FreeBoard is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  FreeBoard is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with FreeBoard.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * Gps.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef GPS_H_
#define GPS_H_

#include "Arduino.h"

#include <nmea.h>
#include <PString.h>
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"
#include "FreeBoardPLC.h"


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
	long autoBaud();
	long detectRate(int rcvPin);
	bool testMsg();
	void resetGPS();
	void setupGpsImpl();
	//bool gpsDecode;
	//unsigned long gpsLastFix;


	// create a GPS data connection to GPRMC sentence type
	//NMEA gps(GPRMC);
	NMEA* gpsSource;
	FreeBoardModel* model;
};
#endif /* GPS_H_ */

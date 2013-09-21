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
#include "Gps.h"
char gpsBuffer[40];
PString gpsStr(gpsBuffer, sizeof(gpsBuffer));

Gps::~Gps() {
}
Gps::Gps(NMEA* gpsSource, FreeBoardModel* model) {

	this->model = model;
	this->gpsSource = gpsSource;
	//setupGps();
}

bool Gps::testMsg() {
	//clear buffer of rubbish
	int x = 0;
	while (x < 100) {
		if (Serial1.available()) {
			Serial1.read();
			x++;
		}
	}
	//5 secs
	unsigned long now = millis();
	bool valid = true;
	while (now + 5000 > millis() && valid) {
		if (Serial1.available()) {
			int c = Serial1.read();
			//Serial.print(c);
			//Serial.print(",");
			//not Cntrl-n or printable so invalid
			if (c > 126) valid = false;
		}
	}
	if (DEBUG) {
		if (valid)
			Serial.println("OK");
		else
			Serial.println("FAIL");
	}
	return valid;
}


int Gps::autoBaud() {
	//try the various baud rates until one makes sense
	//should only output simple NMEA [$A-Z0-9*\r\c]
	//start with saved default
	if (DEBUG){
		Serial.print("   try autobaud ");
		Serial.print(model->getSerialBaud1());
		Serial.println("..");
	}
	Serial1.begin(model->getSerialBaud1());
		if (testMsg()) return model->getSerialBaud1();
	Serial1.end();

	if (DEBUG) Serial.println("   try autobaud 4800..");
	Serial1.begin(4800);
	if (testMsg()) return 4800;
	Serial1.end();
	if (DEBUG) Serial.println("   try autobaud 9600..");
	Serial1.begin(9600);
	if (testMsg()) return 9600;
	Serial1.end();
	if (DEBUG) Serial.println("   try autobaud 19200..");
	Serial1.begin(19200);
	if (testMsg()) return 19200;
	Serial1.end();
	if (DEBUG) Serial.println("   try autobaud 38400..");
	Serial1.begin(38400);
	if (testMsg()) return 38400;
	Serial1.end();
	if (DEBUG) Serial.println("   try autobaud 57600..");
	Serial1.begin(57600);
	if (testMsg()) return 57600;
	Serial1.end();
	if (DEBUG) Serial.println("   default to 4800..");
	return 4800;
}

//bool Gps::getGpsDecode() {
//	return gpsDecode;
//}
/*
 Set baud rate and various message frequencies.
 */
void Gps::setupGps() {

	/*
	 Value Description
	 0 GGA Fix information
	 1 GLL Lat/Lon data
	 2 GSA Overall Satellite data
	 3 GSV Detailed Satellite data
	 4 RMC Recommended minimum info
	 5 VTG Vector track an Speed over the Ground
	 6 MSS (If internal beacon is supported)
	 7 Not defined
	 8 ZDA (if 1PPS output is supported)
	 9 Not defined
	 */
	autoBaud();

	setupGpsImpl();
	//now flush and restart
	Serial1.flush();
	Serial1.end();
	Serial1.begin(model->getSerialBaud1(), SERIAL_8N1);
	//Serial1.begin(38400, SERIAL_8N1);
}

float Gps::getMetersTo(float targetLat, float targetLon, float currentLat, float currentLon) {
	// returns distance in meters between two positions, both specified
	// as signed decimal-degrees latitude and longitude. Uses great-circle
	// distance computation for hypothised sphere of radius 6372795 meters.
	// Because Earth is no exact sphere, rounding errors may be upto 0.5%.
	float delta = radians(targetLon - currentLon);
	float sdlong = sin(delta);
	float cdlong = cos(delta);
	targetLat = radians(targetLat);
	currentLat = radians(currentLat);
	float slat1 = sin(targetLat);
	float clat1 = cos(targetLat);
	float slat2 = sin(currentLat);
	float clat2 = cos(currentLat);
	delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
	delta = sq(delta);
	delta += sq(clat2 * sdlong);
	delta = sqrt(delta);
	float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
	delta = atan2(delta, denom);
	return delta * 6372795 * MTR;
}

bool Gps::decode(byte inByte) {
	// check if the character completes a valid GPS sentence
	model->setGpsDecode(gpsSource->decode(inByte));
	//if(DEBUG)
	//Serial.println(inByte);
	if (model->isGpsDecode()) {
		model->setGpsStatus(gpsSource->gprmc_status());
		if (gpsSource->gprmc_status() == 'A' && gpsSource->term(0)[2] != 'R' && gpsSource->term(0)[3] != 'M' && gpsSource->term(0)[4] != 'C') {
			model->setGpsLastFix(millis());
			model->setGpsCourse(gpsSource->gprmc_course());
			model->setGpsLatitude(gpsSource->gprmc_latitude());
			model->setGpsLongitude(gpsSource->gprmc_longitude());
			model->setGpsSpeed(gpsSource->gprmc_speed(model->getGpsSpeedUnit()));
			model->setGpsUtc(gpsSource->gprmc_utc());
		}
	}
	return model->isGpsDecode();
}

/*
 Reset the GPS to factory defaults


void Gps::resetGPS() {
	//set debug on
	Serial1.println("$PSRF105,1*3E");

	//factory reset
	//$PSRF101,0,0,0,0,0,0,12,8*1C
	Serial1.println("$PSRF101,0,0,0,0,0,0,12,8*1C");
	while (Serial1.available() > 0) {
		Serial.print(Serial1.read());
	}
	//debug off
	Serial1.println("$PSRF105,0*3F");
}*/

PString Gps::getLatString(float lat, int decimals, int padding, PString str) {

	str.begin();
	if (lat >= 0.0) {
		str.print("N");
		str.print(lat, 4);
		str.print("    ");
	} else {
		str.print("S");
		float plusLat = 0 - lat;
		str.print(plusLat, 4);
		str.print("    ");
	}
	//float absLat = abs(lat);
	//char * rslt = padFloat(absLat,decimals,padding);
	//str.print(rslt);
	return str;
}

PString Gps::getLonString(float lon, int decimals, int padding, PString str) {

	str.begin();
	if (lon >= 0) {
		str.print("E");
		str.print(lon, 4);
		str.print("    ");
	} else {
		str.print("W");
		float plusLon = 0 - lon;
		str.print(plusLon, 4);
		str.print("    ");
	}
	// float absLon = abs(lon);
	//char * rslt = padFloat(absLon,decimals,padding);
	//str.print(printFloat(abs(lon),decimals));

	return str;
}
/*
 * setup implementations for various models of GPS.
 * Define the GPS in GPS.h
 */
void Gps::setupGpsImpl(){
	//setup based on GPS type - probably wants a more modular way if many GPS types appear
	Serial.println("Setting GPS config..." );
	if(GPS_GENERIC == model->getGpsModel()){
			Serial.println("Setting GPS to GENERIC" );
	}
	if(GPS_EM_406A == model->getGpsModel()){
		Serial.println("Setting GPS to EM_406A" );
		//Serial1.begin(38400, 8, 1, 0); //gps
		//set debug on
		Serial1.println("$PSRF105,1*3E");

		//set VTG off -  Vector track an Speed over the Ground
		Serial1.println("$PSRF103,05,00,00,01*21");
		//set GLL off -  Lat/Lon data
		Serial1.println("$PSRF103,01,00,00,01*25");
		//set GGA on, 5sec, constant -  Fix information
		Serial1.println("$PSRF103,00,00,05,01*21");
		//set GSA on, 5 sec, constant - Overall Satellite data
		Serial1.println("$PSRF103,02,00,05,01*23");
		//set GSV on, 20sec, constant -Detailed Satellite data
		Serial1.println("$PSRF103,03,00,20,01*25");
		//set RMC on, 1sec, constant, Recommended minimum info
		Serial1.println("$PSRF103,04,00,01,01*21");

		//debug off
		Serial1.println("$PSRF105,0*3F");

		//set baud faster ?
		//GPS SIRF configuration strings...
		//#define SIRF_BAUD_RATE_4800    "$PSRF100,1,4800,8,1,0*0E\r\n"
		//#define SIRF_BAUD_RATE_9600    "$PSRF100,1,9600,8,1,0*0D\r\n"
		//#define SIRF_BAUD_RATE_19200    "$PSRF100,1,19200,8,1,0*38\r\n"
		//#define SIRF_BAUD_RATE_38400    "$PSRF100,1,38400,8,1,0*3D\r\n"
		//#define SIRF_BAUD_RATE_57600    "$PSRF100,1,57600,8,1,0*36\r\n"

		//$PSRF100,1,38400,8,1,0*3D\r\n
		char gpsSentence [30];
		PString str(gpsSentence, sizeof(gpsSentence));
		str.print("$PSRF100,1,");
		str.print(model->getSerialBaud1());
		str.print(",8,1,0*");
		//calculate the checksum
		byte cs = getChecksum(gpsSentence); //clear any old checksum
		//bug - arduino prints 0x007 as 7, 0x02B as 2B, so we add it now
		if (cs < 0x10) str.print('0');
		str.print(cs, HEX); // Assemble the final message and send it out the serial port
		Serial1.println(gpsSentence);
		//Serial1.println("$PSRF100,1,38400,8,1,0*3D");
		Serial.println(gpsSentence);
	}
	if(GPS_MTEK_3329 == model->getGpsModel()){
		Serial.println("Setting GPS to MTEK_3329" );
		//setting update rate to 1Hz
		Serial1.println("$PMTK220,1000*1F");
		//setting the NMEA Output to get RMC, GGA, GSA & GSV.
		Serial1.println("$PMTK314,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28");

		/*
		  -Change unit refresh rate:
		$PMTK220,100*2F       //Will set the GPS to 10hz (or updates every 100 milliseconds)
		$PMTK220,250*29       //Will set the GPS to 4hz (or updates every 250 milliseconds)
		$PMTK220,1000*1F     //Will set the GPS to 1hz (updates every 1000 milliseconds)
		You can set the GPS to any desired refresh rate, you change the value inside the string and generate a new check sum here: http://www.hhhh.org/wiml/proj/nmeaxor.html
		-To change the baud rates:
		$PMTK251,4800*14
		$PMTK251,9600*17
		$PMTK251,19200*22
		$PMTK251,38400*27
		You can also set the GPS to any desired baud rate speed by changing the value inside the string and generate a new checksum here: http://www.hhhh.org/wiml/proj/nmeaxor.html
		 */

		char gpsSentence [30];
		PString str(gpsSentence, sizeof(gpsSentence));
		str.print("$PMTK251,");
		str.print(model->getSerialBaud1());
		str.print("*");
		//calculate the checksum
		byte cs = getChecksum(gpsSentence); //clear any old checksum
		//bug - arduino prints 0x007 as 7, 0x02B as 2B, so we add it now
		if (cs < 0x10) str.print('0');
		str.print(cs, HEX); // Assemble the final message and send it out the serial port
		Serial1.println(gpsSentence);

	}
}

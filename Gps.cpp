#include "Gps.h"
char gpsBuffer[40];
PString gpsStr(gpsBuffer, sizeof(gpsBuffer));

Gps::~Gps() {
}
Gps::Gps(NMEA* gpsSource,FreeBoardModel* model) {

	this->model=model;
	model->setGpsDecode(false);
	model->setGpsLastFix(0);
	lastLcdUpdate=0;
	this->gpsSource = gpsSource;
	//setupGps();
}

bool Gps::testMsg(){
	//clear buffer of rubbish
	int x =0;
	while( x<100){
		if(Serial1.available()){
			Serial1.read();
			x++;
		}
	}
	//5 secs
	unsigned long now = millis();
	bool valid=true;
	while(now+5000>millis() && valid){
		if(Serial1.available()){
			int c = Serial1.read();
			//Serial.print(c);
			//Serial.print(",");
			//not Cntrl-n or printable so invalid
			if( c>128)valid=false;
		}
	}
	if(DEBUG){
		if(valid)
			Serial.println("OK");
		else
			Serial.println("FAIL");
	}
	return valid;
}

int Gps::autoBaud(){
	//try the various baud rates until one makes sense
	//should only output simple NMEA [$A-Z0-9*\r\c]


	if(DEBUG)Serial.println("   try autobaud 4800..");
	Serial1.begin(4800);
	if(testMsg())return 4800;
	Serial1.end();
	if(DEBUG)Serial.println("   try autobaud 9600..");
	Serial1.begin(9600);
	if(testMsg())return 9600;
	Serial1.end();
	if(DEBUG)Serial.println("   try autobaud 19200..");
	Serial1.begin(19200);
	if(testMsg())return 19200;
	Serial1.end();
	if(DEBUG)Serial.println("   try autobaud 38400..");
		Serial1.begin(38400);
		if(testMsg())return 38400;
	Serial1.end();
	if(DEBUG)Serial.println("   try autobaud 57600..");
	Serial1.begin(57600);
	if(testMsg())return 57600;
	Serial1.end();
	if(DEBUG)Serial.println("   default to 4800..");
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

	//get debug
	//delay(1000);
//	while (Serial1.available() > 0) {
//		Serial.print(Serial1.read());
//	}
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
	Serial1.print("$PSRF100,1,38400,8,1,0*3D\r\n");
	Serial1.flush();
	Serial1.end();
	Serial1.begin(38400);

}


float Gps::getMetersTo(float targetLat, float targetLon, float currentLat, float currentLon){
		// returns distance in meters between two positions, both specified
		// as signed decimal-degrees latitude and longitude. Uses great-circle
		// distance computation for hypothised sphere of radius 6372795 meters.
		// Because Earth is no exact sphere, rounding errors may be upto 0.5%.
	  float delta = radians(targetLon-currentLon);
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
	//if(DEBUG)Serial.print(inByte,BYTE);
	if (model->isGpsDecode()) {
		model->setGpsStatus(gpsSource->gprmc_status());
		if (gpsSource->gprmc_status() == 'A'
				&& gpsSource->term(0)[2] != 'R' && gpsSource->term(0)[3] != 'M'
				&& gpsSource->term(0)[4] != 'C') {
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
 */

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
}


PString Gps::getLatString(float lat, int decimals, int padding, PString str) {

	str.begin();
	if (lat >= 0.0) {
		str.print("N");
		str.print(lat,4);
		str.print("    ");
	} else {
		str.print("S");
		float plusLat = 0 - lat;
		str.print(plusLat,4);
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
		str.print(lon,4);
		str.print("    ");
	} else {
		str.print("W");
		float plusLon = 0 - lon;
		str.print(plusLon,4);
		str.print("    ");
	}
	// float absLon = abs(lon);
	//char * rslt = padFloat(absLon,decimals,padding);
	//str.print(printFloat(abs(lon),decimals));

	return str;
}


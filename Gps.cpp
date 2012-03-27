#include "Gps.h"

Gps::~Gps() {
}
Gps::Gps(NMEA* gpsSource) {
	gpsDecode = false;
	lastGpsFix = 0;
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



float Gps::getLatitude() {
	return gpsSource->gprmc_latitude();
}

float Gps::getLongitude() {
	return gpsSource->gprmc_longitude();
}

unsigned long Gps::getLastGpsFix() {
	return lastGpsFix;
}

bool Gps::getRmcStatus() {
	return (gpsSource->gprmc_status() == 'A');
}

float Gps::getRmcMetersTo(float lat, float lon) {
	return gpsSource->gprmc_distance_to(lat, lon, MTR);
}
float Gps::getRmcCourse() {
	return gpsSource->gprmc_course();
}

bool Gps::decode(byte inByte) {
	// check if the character completes a valid GPS sentence
	gpsDecode = gpsSource->decode(inByte);
	//if(DEBUG)Serial.print(inByte,BYTE);
	if (gpsDecode) {
		if (gpsSource->gprmc_status() == 'A') {
			lastGpsFix = millis();
		}
	}
	return gpsDecode;
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

/* NMEA GPS routines
 */
void Gps::showGPSData(Lcd lcd, int menuState) {
	if (menuState != GPS){
		return;
	}
	//check if active
	if (millis() - lastLcdUpdate > 2000) {
		//update
		lastLcdUpdate= millis();

		//check we still have a fix
		if (millis() - lastGpsFix > 10000) {
			//show bad GPS fix
			lcd.setCursor(2, 20);
			lcd.print("WARNING: NO GPS FIX! ");
			delay(50);
			return;
		}

		if (lastGpsFix>0 && gpsSource->gprmc_status() == 'A') {
			if ((gpsSource->term(0))[2] != 'R' && gpsSource->term(0)[3] != 'M'
					&& gpsSource->term(0)[4] != 'C')
				return;

			//clearLcd();

			lcd.setCursor(2, 60);
			lcd.print("Lat: ");
			lcd.print(getLatString(gpsSource->gprmc_latitude(), 4, 10));
			delay(50);

			lcd.setCursor(2, 50);
			lcd.print("Lon: ");
			lcd.print(getLonString(gpsSource->gprmc_longitude(), 4, 10));
			delay(50);

			lcd.setCursor(2, 40);
			lcd.print("Knots: ");
			lcd.print(gpsSource->gprmc_speed(KTS),1);
			delay(50);
			//lcd.print("  ");

			lcd.setCursor(2, 30);
			lcd.print("Heading: ");
			lcd.print(gpsSource->gprmc_course(),0);
			lcd.print("T   ");
			delay(50);
			lcd.setCursor(2, 20);
			lcd.print("                    ");
			lcd.writeButtonLabels(EMPTY,PGDN,PGUP);
			delay(50);
		}

	}
}

char* Gps::getLatString(float lat, int decimals, int padding) {
	PString str(latBuffer, sizeof(latBuffer));
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
	return latBuffer;
}

char* Gps::getLonString(float lon, int decimals, int padding) {
	PString str(lonBuffer, sizeof(lonBuffer));
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

	return lonBuffer;
}


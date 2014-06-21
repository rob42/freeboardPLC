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
 * FreeBoardModel.cpp
 *
 *  Created on: Mar 28, 2012
 *      Author: robert
 */

#include "FreeBoardModel.h"

FreeBoardModel::FreeBoardModel() {
	alarmLast = 0; //millis time of last beep state change
	alarmSnooze = 0; //5 minute alarm snooze
	//unsigned long alarmTriggered ; //true if any alarm is triggered - derived

	//anchor
	//float anchorRadius; //anchor alarm radius in meters
	anchorState.anchorRadiusDeg = 0.0; //anchor alarm radius in decimal degrees, eg 1deg = 60NM.

	//anchor alarm data
	//float anchorLat; // variable for reading the anchor latitude
	//float anchorLon; // variable for reading the anchor longitude
	anchorState.anchorDistance = 0.0;
	anchorState.anchorMaxDistance = 0.0;
	//bool anchorAlarmOn; //flag to turn anchor alarm on/off toggle
	anchorState.anchorAlarmTriggered = false; //set to true to trigger anchor alarm

	//a box around the anchor, shrinks with every GPS reading to home in on the anchor itself
	anchorState.anchorN = 90.0;
	anchorState.anchorS = -90.0;
	anchorState.anchorE = 180.0;
	anchorState.anchorW = -180.0;

	//autopilot
	autopilotState.autopilotOn = false;
	//disengage the autopilot if we reboot!!
	//Dont want to go screaming off on wrong course.
	autopilotState.autopilotReference = AUTOPILOT_COMPASS;
	autopilotState.autopilotTargetHeading = 0; //Setpoint
	autopilotState.autopilotRudderCommand = 33; //Output (rudder central)
	//bool autopilotAlarmOn;
	autopilotState.autopilotAlarmTriggered = false;
	autopilotState.autopilotAlarmMaxXTError = 100; // +/- meters cross track error
	autopilotState.autopilotAlarmMaxWindError = 10; // +/- wind angle change, for over 1 minute
	autopilotState.autopilotAlarmMaxCourseError = 10; // +/- course error, for over 1 minute
	autopilotState.autopilotSpeed=128;
	//compass
	magneticHeading = 0;
	declination = 0;
	//gps
	gpsState.gpsDecode = false; //flag to indicate a new sentence was decoded.
	gpsState.gpsLastFix = 0; //time of last good gps fix.
	gpsState.gpsUtc = 0; // decimal value of UTC term in last full GPRMC sentence
	gpsState.gpsStatus = 'V'; //  status character in last full GPRMC sentence ('A' or 'V')
	gpsState.gpsLatitude = 0.0; // signed degree-decimal value of latitude terms in last full GPRMC sentence
	gpsState.gpsLongitude = 0.0; // signed degree-decimal value of longitude terms in last full GPRMC sentence
	//float gpsSpeedUnit; //unit multiplier for gpsSpeed. 1.0 = KNT,1.1507794	=MPH, see nmea.h
	gpsState.gpsSpeed = 0.0; // speed-on-ground term in last full GPRMC sentence
	gpsState.gpsCourse = 0.0; // track-angle-made-good term in last full GPRMC sentence
	//bool gpsAlarmOn; //true to engage alarm
	gpsState.gpsAlarmTriggered = false; //set to true to trigger gps alarm
	//double gpsAlarmFixTime; //max time in millis without fix

	//seatalk
	//volatile bool radarAlarmOn; //set to true to enable radar alarm
	radarAlarmTriggered = false; //set to true to trigger radar alarm
	//volatile bool mobAlarmOn; //set to true to enable mob alarm
	mobAlarmTriggered = false; //set to true to trigger MOB alarm
	lvl1AlarmTriggered = false;
	config.lvl1UpperLimit = 1025; //0-1024 analogue range - higher is slower to alarm, eg more lvl1
	config.lvl1LowerLimit=-1; //0-1024 analogue range - lower is slower to alarm, eg less lvl1
	lvl2AlarmTriggered =false; //set to true to trigger lvl2 alarm
	config.lvl2UpperLimit= 1025; //0-1024 analogue range - higher is slower to alarm, eg more lvl2
	config.lvl2LowerLimit=-1; //0-1024 analogue range - lower is slower to alarm, eg less lvl2
	lvl3AlarmTriggered =false; //set to true to trigger lvl3 alarm
	config.lvl3UpperLimit= 1025; //0-1024 analogue range - higher is slower to alarm, eg more lvl3
	config.lvl3LowerLimit=-1; //0-1024 analogue range - lower is slower to alarm, eg less lvl3
	//wind
	windState.windLastUpdate = 0;
	windState.windAverage = 0.0;
	//windState.windFactor=0;
	windState.windMax = 0;
	windState.windApparentDir = 0;
	windState.windTrueDir = 0;
	//int windAlarmSpeed;
	//bool windAlarmOn;
	windState.windAlarmTriggered = false;

	//struct Configuration{
	config.anchorLat = 0.0;
	config.anchorLon = 0.0;
	config.anchorRadius = 40.0;
	config.anchorAlarmOn = false;
	config.autopilotAlarmOn = false;
	config.autopilotDeadZone = 0;
	config.autopilotSlack = 0;
	config.gpsSpeedUnit = KTS;
	config.gpsAlarmOn = false;
	config.gpsAlarmFixTime = 1000l * 60 * 5; //5 min
	config.radarAlarmOn = false;
	config.mobAlarmOn = false;
	config.windAlarmSpeed = 99;
	config.windAlarmOn = false;
	config.windFactor = 10000.0;
	config.windZeroOffset = 0;
	//ver6
	config.gpsModel = GPS_EM_406A;
	config.serialBaud = 38400l;
	config.serialBaud1 = 38400l;
	config.serialBaud2 = 9600l; //seatalk?
	config.serialBaud3 = 9600l; //16 bytes
	config.serialBaud4 = 9600l; //16 bytes
	config.autopilotBaud = 9600l; //16 bytes
	config.seaTalk = false;
	//}config;

//we change this if we change the struct so we can tell before reloading incompatible versions
	version = EEPROM_VER;
}

template<class T> int writeObject(HardwareSerial ser, T& value, char name) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	char checksum = '0';
	ser.write('@');
	ser.write(name);
	for (i = 0; i < sizeof(value); i++) {
		ser.write(*p++);
		checksum = checksum ^ *p;
	}
	ser.write(checksum);
	ser.write('\n');
	return i;

}

int FreeBoardModel::writeSimple(HardwareSerial ser) {
	//ArduIMU output format
	//!!VER:1.9,RLL:-0.52,PCH:0.06,YAW:80.24,IMUH:253,MGX:44,MGY:-254,MGZ:-257,MGH:80.11,LAT:-412937350,LON:1732472000,ALT:14,COG:116,SOG:0,FIX:1,SAT:5,TOW:22504700,

	ser.print("!!VER:1.9,");
	ser.print("UID:MEGA,APX:");
	ser.print(autopilotState.autopilotOn);
	ser.print(",APS:");
	ser.print(autopilotState.autopilotReference);
	//if autopilot on, send autopilot data
	if (autopilotState.autopilotOn) {
		ser.print(",APT:");
		ser.print(getAutopilotTargetHeading());
		ser.print(",APC:");
		ser.print(getAutopilotCurrentHeading());
		ser.print(",APR:");
		ser.print(autopilotState.autopilotRudderCommand - 33.0); // 0-66 in model
	}
	//if anchor alarm on, send data
	ser.print(",AAX:");
	ser.print(config.anchorAlarmOn);
	ser.print(",AAR:");
	ser.print(config.anchorRadius);
	if (config.anchorAlarmOn) {
		ser.print(",AAN:");
		ser.print(config.anchorLat);
		ser.print(",AAE:");
		ser.print(config.anchorLon);
		ser.print(",AAD:");
		ser.print(getAnchorDistance());
	}
	//if wind alarm on, send data
	ser.print(",WSX:");
	ser.print(config.windAlarmOn);
	ser.print(",WSK:");
	ser.print(config.windAlarmSpeed);

	ser.println(",");
	return 0;
}
/*
 * Write out the config to serial
 */
int FreeBoardModel::writeConfig(HardwareSerial ser) {
	//ArduIMU output format
	//!!VER:1.9,RLL:-0.52,PCH:0.06,YAW:80.24,IMUH:253,MGX:44,MGY:-254,MGZ:-257,MGH:80.11,LAT:-412937350,LON:1732472000,ALT:14,COG:116,SOG:0,FIX:1,SAT:5,TOW:22504700,

	ser.print("!!VER:1.9,");
	ser.print("UID:MEGA,APX:");

	ser.print(",WZJ:");
	ser.print(getWindZeroOffset());
	ser.print(",GPS:");
	ser.print(getGpsModel());
	ser.print(",SB0:");
	ser.print(getSerialBaud());
	ser.print(",SB1:");
	ser.print(getSerialBaud1());
	ser.print(",SB2:");
	ser.print(getSerialBaud2());
	ser.print(",SB3:");
	ser.print(getSerialBaud3());
	ser.print(",STK:");
	ser.print(getSeaTalk());

	ser.print(",LU1:");
	ser.print(getLvl1UpperLimit());
	ser.print(",LL1:");
	ser.print(getLvl1LowerLimit());

	ser.print(",LU2:");
	ser.print(getLvl2UpperLimit());
	ser.print(",LL2:");
	ser.print(getLvl2LowerLimit());

	ser.print(",LU3:");
	ser.print(getLvl3UpperLimit());
	ser.print(",LL3:");
	ser.print(getLvl3LowerLimit());

	ser.println(",");
	return 0;
}
int FreeBoardModel::sendData(HardwareSerial ser, char name) {
	if (CONFIG_T == name) {
		return writeObject(ser, config, name);
	}
	if (DYNAMIC_T == name) {
		return writeObject(ser, gpsState, name);
	}
	if (SIMPLE_T == name) {
		return writeSimple(ser);
	}
	return -1;
}

template<class T> int readObject(HardwareSerial ser, T& value, char name) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	char checksum = '0';
	for (i = 0; i < sizeof(value); i++) {
		*p++ = ser.read();
		checksum = checksum ^ *p;
	}
	//TODO: make sure this actually works
	if (ser.read() == checksum) {
		Serial.print("Checksum valid");
	} else {
		Serial.print("Checksum invalid");
	}

	return i;
}

int FreeBoardModel::receiveData(HardwareSerial ser, char name) {
	if (CONFIG_T == name) {
		return readObject(ser, config, name);
	}
	return -1;
}

template<class T> int EEPROM_writeAnything(int ee, T& value) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		EEPROM.write(ee++, *p++);
	return i;
}
//saving
template<class T> int EEPROM_readAnything(int ee, T& value) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
	return i;
}
void FreeBoardModel::saveConfig() {
	//write out a current version
	EEPROM_writeAnything(0, version);
	//write data
	EEPROM_writeAnything(EEPROM_DATA, config);
}

void FreeBoardModel::readConfig() {
	//check versions here
	int ver;
	EEPROM_readAnything(0, ver);
	if (ver != version) {
		//save a default config, since we cant read the old one safely
		saveConfig();
	}

	//now we know its compatible
	EEPROM_readAnything(EEPROM_DATA, config);

}

//accessors
bool FreeBoardModel::isWindAlarmTriggered() {
	return windState.windAlarmTriggered;
}

unsigned long FreeBoardModel::getAlarmLast() {
	return alarmLast;
}

unsigned long FreeBoardModel::getAlarmSnooze() {
	return alarmSnooze;
}

float FreeBoardModel::getAnchorDistance() {
	return anchorState.anchorDistance;
}

float FreeBoardModel::getAnchorE() {
	return anchorState.anchorE;
}

float FreeBoardModel::getAnchorLat() {
	return config.anchorLat;
}

float FreeBoardModel::getAnchorLon() {
	return config.anchorLon;
}

float FreeBoardModel::getAnchorMaxDistance() {
	return anchorState.anchorMaxDistance;
}

float FreeBoardModel::getAnchorN() {
	return anchorState.anchorN;
}

float FreeBoardModel::getAnchorRadius() {
	return config.anchorRadius;
}

float FreeBoardModel::getAnchorRadiusDeg() {
	return anchorState.anchorRadiusDeg;
}

float FreeBoardModel::getAnchorS() {
	return anchorState.anchorS;
}

float FreeBoardModel::getAnchorW() {
	return anchorState.anchorW;
}

/*
 * Returns -179 to +180 as the degrees off course
 */
double FreeBoardModel::getAutopilotOffCourse() {
	//get degrees between
	autopilotState.autopilotOffCourse = getAutopilotTargetHeading() - getAutopilotCurrentHeading();
	autopilotState.autopilotOffCourse += (autopilotState.autopilotOffCourse > 180) ? -360 : (autopilotState.autopilotOffCourse < -180) ? 360 : 0;

	return autopilotState.autopilotOffCourse;
}

char FreeBoardModel::getAutopilotReference() {
	return autopilotState.autopilotReference;
}

double FreeBoardModel::getAutopilotAlarmMaxCourseError() {
	return autopilotState.autopilotAlarmMaxCourseError;
}

double FreeBoardModel::getAutopilotAlarmMaxWindError() {
	return autopilotState.autopilotAlarmMaxWindError;
}

double FreeBoardModel::getAutopilotAlarmMaxXtError() {
	return autopilotState.autopilotAlarmMaxXTError;
}

double FreeBoardModel::getAutopilotRudderCommand() {
	return autopilotState.autopilotRudderCommand;
}

double FreeBoardModel::getAutopilotTargetHeading() {
	return autopilotState.autopilotTargetHeading;
}
double FreeBoardModel::getAutopilotCurrentHeading() {
	if (autopilotState.autopilotReference == AUTOPILOT_WIND) {
		return windState.windApparentDir;
	}
	//default option - compass
	return magneticHeading;
}

int FreeBoardModel::getAutopilotDeadZone() {
	return this->config.autopilotDeadZone;
}
int FreeBoardModel::getAutopilotSlack() {
	return this->config.autopilotSlack;
}
long FreeBoardModel::getAutopilotSpeed() {
	return this->config.autopilotSpeed;
}

long FreeBoardModel::getGpsAlarmFixTime() {
	return config.gpsAlarmFixTime;
}

float FreeBoardModel::getGpsCourse() {
	return gpsState.gpsCourse;
}

unsigned long FreeBoardModel::getGpsLastFix() {
	return gpsState.gpsLastFix;
}

float FreeBoardModel::getGpsLatitude() {
	return gpsState.gpsLatitude;
}

float FreeBoardModel::getGpsLongitude() {
	return gpsState.gpsLongitude;
}

float FreeBoardModel::getGpsSpeed() {
	return gpsState.gpsSpeed;
}

float FreeBoardModel::getGpsSpeedUnit() {
	return config.gpsSpeedUnit;
}

char FreeBoardModel::getGpsStatus() {
	return gpsState.gpsStatus;
}

float FreeBoardModel::getGpsUtc() {
	return gpsState.gpsUtc;
}

float FreeBoardModel::getMagneticHeading() {
	return this->magneticHeading;
}
float FreeBoardModel::getDeclination() {
	return declination;
}

volatile bool FreeBoardModel::isMobAlarmTriggered() {
	return mobAlarmTriggered;
}
volatile bool FreeBoardModel::isLvl1AlarmTriggered() {
	return lvl1AlarmTriggered;
}
int FreeBoardModel::getLvl1UpperLimit(){
	return config.lvl1UpperLimit;
}
void FreeBoardModel::setLvl1UpperLimit(int lvl1UpperLimit){
	this->config.lvl1UpperLimit=lvl1UpperLimit;
}

int FreeBoardModel::getLvl1LowerLimit(){
	return config.lvl1LowerLimit;
}
void FreeBoardModel::setLvl1LowerLimit(int lvl1LowerLimit){
	this->config.lvl1LowerLimit=lvl1LowerLimit;
}
volatile bool FreeBoardModel::isLvl2AlarmTriggered() {
	return lvl2AlarmTriggered;
}
int FreeBoardModel::getLvl2UpperLimit(){
	return config.lvl2UpperLimit;
}
void FreeBoardModel::setLvl2UpperLimit(int lvl2UpperLimit){
	this->config.lvl2UpperLimit=lvl2UpperLimit;
}

int FreeBoardModel::getLvl2LowerLimit(){
	return config.lvl2LowerLimit;
}
void FreeBoardModel::setLvl2LowerLimit(int lvl2LowerLimit){
	this->config.lvl2LowerLimit=lvl2LowerLimit;
}

volatile bool FreeBoardModel::isLvl3AlarmTriggered() {
	return lvl3AlarmTriggered;
}
int FreeBoardModel::getLvl3UpperLimit(){
	return config.lvl3UpperLimit;
}
void FreeBoardModel::setLvl3UpperLimit(int lvl3UpperLimit){
	this->config.lvl3UpperLimit=lvl3UpperLimit;
}

int FreeBoardModel::getLvl3LowerLimit(){
	return config.lvl3LowerLimit;
}
void FreeBoardModel::setLvl3LowerLimit(int lvl3LowerLimit){
	this->config.lvl3LowerLimit=lvl3LowerLimit;
}


volatile bool FreeBoardModel::isRadarAlarmTriggered() {
	return radarAlarmTriggered;
}

int FreeBoardModel::getWindZeroOffset() {
	return config.windZeroOffset;
}

int FreeBoardModel::getWindAlarmSpeed() {
	return config.windAlarmSpeed;
}

int FreeBoardModel::getWindApparentDir() {
	return windState.windApparentDir;
}

int FreeBoardModel::getWindTrueDir() {
	return windState.windTrueDir;
}

float FreeBoardModel::getWindAverage() {
	return windState.windAverage;
}

float FreeBoardModel::getWindFactor() {
	return config.windFactor;
}

unsigned long FreeBoardModel::getWindLastUpdate() {
	return windState.windLastUpdate;
}

int FreeBoardModel::getWindMax() {
	return windState.windMax;
}

bool FreeBoardModel::isAnchorAlarmOn() {
	return config.anchorAlarmOn;
}

bool FreeBoardModel::isAnchorAlarmTriggered() {
	return anchorState.anchorAlarmTriggered;
}

bool FreeBoardModel::isAutopilotAlarmOn() {
	return config.autopilotAlarmOn;
}

bool FreeBoardModel::isAutopilotAlarmTriggered() {
	return autopilotState.autopilotAlarmTriggered;
}

bool FreeBoardModel::isGpsAlarmOn() {
	return config.gpsAlarmOn;
}

bool FreeBoardModel::isGpsAlarmTriggered() {
	return gpsState.gpsAlarmTriggered;
}

bool FreeBoardModel::isGpsDecode() {
	return gpsState.gpsDecode;
}

bool FreeBoardModel::isWindAlarmOn() {
	return config.windAlarmOn;
}

void FreeBoardModel::setAlarmLast(unsigned long alarmLast) {
	this->alarmLast = alarmLast;
}

void FreeBoardModel::setAlarmSnooze(unsigned long alarmSnooze) {
	this->alarmSnooze = alarmSnooze;
}

void FreeBoardModel::setAnchorAlarmOn(bool anchorAlarmOn) {
	this->config.anchorAlarmOn = anchorAlarmOn;
}

void FreeBoardModel::setAnchorAlarmTriggered(bool anchorAlarmTriggered) {
	this->anchorState.anchorAlarmTriggered = anchorAlarmTriggered;
}

void FreeBoardModel::setAnchorDistance(float anchorDistance) {
	this->anchorState.anchorDistance = anchorDistance;
}

void FreeBoardModel::setAnchorE(float anchorE) {
	this->anchorState.anchorE = anchorE;
}

void FreeBoardModel::setAnchorLat(float anchorLat) {
	this->config.anchorLat = anchorLat;
}

void FreeBoardModel::setAnchorLon(float anchorLon) {
	this->config.anchorLon = anchorLon;
}

void FreeBoardModel::setAnchorMaxDistance(float anchorMaxDistance) {
	this->anchorState.anchorMaxDistance = anchorMaxDistance;
}

void FreeBoardModel::setAnchorN(float anchorN) {
	this->anchorState.anchorN = anchorN;
}

void FreeBoardModel::setAnchorRadius(float anchorRadius) {
	this->config.anchorRadius = anchorRadius;
}

void FreeBoardModel::setAnchorRadiusDeg(float anchorRadiusDeg) {
	this->anchorState.anchorRadiusDeg = anchorRadiusDeg;
}

void FreeBoardModel::setAnchorS(float anchorS) {
	this->anchorState.anchorS = anchorS;
}

void FreeBoardModel::setAnchorW(float anchorW) {
	this->anchorState.anchorW = anchorW;
}

void FreeBoardModel::setAutopilotReference(char autopilotReference) {

	if (autopilotReference != AUTOPILOT_WIND && autopilotReference != AUTOPILOT_COMPASS) return;
	this->autopilotState.autopilotReference = autopilotReference;
	if (autopilotState.autopilotReference == AUTOPILOT_WIND) {
		autopilotState.autopilotTargetHeading = windState.windApparentDir;
	}
	if (autopilotState.autopilotReference == AUTOPILOT_COMPASS) {
		autopilotState.autopilotTargetHeading = magneticHeading;
	}
	//and netralise the rudder position too.
	this->autopilotState.autopilotRudderCommand = 33;
}

void FreeBoardModel::setAutopilotAlarmMaxCourseError(double autopilotAlarmMaxCourseError) {
	this->autopilotState.autopilotAlarmMaxCourseError = autopilotAlarmMaxCourseError;
}

void FreeBoardModel::setAutopilotAlarmMaxWindError(double autopilotAlarmMaxWindError) {
	this->autopilotState.autopilotAlarmMaxWindError = autopilotAlarmMaxWindError;
}

void FreeBoardModel::setAutopilotAlarmMaxXtError(double autopilotAlarmMaxXtError) {
	autopilotState.autopilotAlarmMaxXTError = autopilotAlarmMaxXtError;
}

void FreeBoardModel::setAutopilotAlarmOn(bool autopilotAlarmOn) {
	this->config.autopilotAlarmOn = autopilotAlarmOn;
}

void FreeBoardModel::setAutopilotAlarmTriggered(bool autopilotAlarmTriggered) {
	this->autopilotState.autopilotAlarmTriggered = autopilotAlarmTriggered;
}

void FreeBoardModel::setAutopilotRudderCommand(double autopilotRudderCommand) {
	this->autopilotState.autopilotRudderCommand = autopilotRudderCommand;
}
void FreeBoardModel::setAutopilotSpeed(long autopilotSpeed) {
	this->autopilotState.autopilotSpeed = autopilotSpeed;
}

/**
 * For magnetic it will be 0-360degM
 * For wind it will be -180 to +180 from bow.
 * Since this matches with target heading, and we convert in autopilot to 0-360, alls good?
 */
void FreeBoardModel::setAutopilotTargetHeading(double autopilotTargetHeading) {
	//make this 0-360 range only
	this->autopilotState.autopilotTargetHeading = (double) (((int) autopilotTargetHeading + 360) % 360);
}

void FreeBoardModel::setGpsAlarmFixTime(long gpsAlarmFixTime) {
	this->config.gpsAlarmFixTime = gpsAlarmFixTime;
}

void FreeBoardModel::setGpsAlarmOn(bool gpsAlarmOn) {
	this->config.gpsAlarmOn = gpsAlarmOn;
}

void FreeBoardModel::setGpsAlarmTriggered(bool gpsAlarmTriggered) {
	this->gpsState.gpsAlarmTriggered = gpsAlarmTriggered;
}

void FreeBoardModel::setGpsCourse(float gpsCourse) {
	this->gpsState.gpsCourse = gpsCourse;
}

void FreeBoardModel::setGpsDecode(bool gpsDecode) {
	this->gpsState.gpsDecode = gpsDecode;
}

void FreeBoardModel::setGpsLastFix(unsigned long gpsLastFix) {
	this->gpsState.gpsLastFix = gpsLastFix;
}

void FreeBoardModel::setGpsLatitude(float gpsLatitude) {
	this->gpsState.gpsLatitude = gpsLatitude;
}

void FreeBoardModel::setGpsLongitude(float gpsLongitude) {
	this->gpsState.gpsLongitude = gpsLongitude;
}

void FreeBoardModel::setGpsSpeed(float gpsSpeed) {
	this->gpsState.gpsSpeed = gpsSpeed;
}

void FreeBoardModel::setGpsSpeedUnit(float gpsSpeedUnit) {
	this->config.gpsSpeedUnit = gpsSpeedUnit;
}

void FreeBoardModel::setGpsStatus(char gpsStatus) {
	this->gpsState.gpsStatus = gpsStatus;
}

void FreeBoardModel::setGpsUtc(float gpsUtc) {
	this->gpsState.gpsUtc = gpsUtc;
}

void FreeBoardModel::setMagneticHeading(float magneticHeading) {
	this->magneticHeading = magneticHeading;
}

void FreeBoardModel::setDeclination(float declination) {
	this->declination = declination;
}

void FreeBoardModel::setMobAlarmTriggered(volatile bool mobAlarmTriggered) {
	this->mobAlarmTriggered = mobAlarmTriggered;
}
void FreeBoardModel::setLvl1AlarmTriggered(volatile bool lvl1AlarmTriggered) {
	this->lvl1AlarmTriggered = lvl1AlarmTriggered;
}
void FreeBoardModel::setLvl2AlarmTriggered(volatile bool lvl2AlarmTriggered) {
	this->lvl2AlarmTriggered = lvl2AlarmTriggered;
}
void FreeBoardModel::setLvl3AlarmTriggered(volatile bool lvl3AlarmTriggered) {
	this->lvl3AlarmTriggered = lvl3AlarmTriggered;
}

void FreeBoardModel::setRadarAlarmTriggered(volatile bool radarAlarmTriggered) {
	this->radarAlarmTriggered = radarAlarmTriggered;
}

void FreeBoardModel::setWindZeroOffset(int windZeroOffset) {
	this->config.windZeroOffset = windZeroOffset;
}

void FreeBoardModel::setWindAlarmOn(bool windAlarmOn) {
	this->config.windAlarmOn = windAlarmOn;
}

void FreeBoardModel::setWindAlarmSpeed(int windAlarmSpeed) {
	this->config.windAlarmSpeed = windAlarmSpeed;
}
/* 0-360deg off the bow clockwise.*/
void FreeBoardModel::setWindApparentDir(int windApparentDir) {
	this->windState.windApparentDir = windApparentDir;
}
void FreeBoardModel::setWindTrueDir(int windTrueDir) {
	this->windState.windTrueDir = windTrueDir;
}

void FreeBoardModel::setWindAverage(float windAverage) {
	this->windState.windAverage = windAverage;
}

void FreeBoardModel::setWindFactor(float windFactor) {
	this->config.windFactor = windFactor;
}

void FreeBoardModel::setWindLastUpdate(unsigned long windLastUpdate) {
	this->windState.windLastUpdate = windLastUpdate;
}

volatile bool FreeBoardModel::isAlarmTriggered() {
	return windState.windAlarmTriggered && radarAlarmTriggered && gpsState.gpsAlarmTriggered && anchorState.anchorAlarmTriggered
			&& autopilotState.autopilotAlarmTriggered && mobAlarmTriggered && lvl1AlarmTriggered && lvl2AlarmTriggered && lvl3AlarmTriggered;
}

volatile bool FreeBoardModel::isMobAlarmOn() {
	return config.mobAlarmOn;
}

volatile bool FreeBoardModel::isRadarAlarmOn() {
	return config.radarAlarmOn;
}

void FreeBoardModel::setMobAlarmOn(volatile bool mobAlarmOn) {
	this->config.mobAlarmOn = mobAlarmOn;
}

bool FreeBoardModel::isAutopilotOn() {
	return autopilotState.autopilotOn;
}

void FreeBoardModel::setAutopilotOn(bool autopilotOn) {
	//this is potentally dangerous, since we dont want the boat diving off on an old target heading.
	//ALWAYS reset target heading to current magnetic or wind dir here
	setAutopilotReference(getAutopilotReference());
	this->autopilotState.autopilotOn = autopilotOn;
}

void FreeBoardModel::setAutopilotDeadZone(int deadZone) {
	this->config.autopilotDeadZone = deadZone;
}

void FreeBoardModel::setAutopilotSlack(int slack) {
	this->config.autopilotSlack = slack;
}

void FreeBoardModel::setRadarAlarmOn(volatile bool radarAlarmOn) {
	this->config.radarAlarmOn = radarAlarmOn;
}

void FreeBoardModel::setWindMax(int windMax) {
	this->windState.windMax = windMax;
}

void FreeBoardModel::setWindAlarmTriggered(bool windAlarmTriggered) {
	this->windState.windAlarmTriggered = windAlarmTriggered;
}

short FreeBoardModel::getGpsModel() {
	return this->config.gpsModel;
}
void FreeBoardModel::setGpsModel(short gpsModel) {
	this->config.gpsModel = gpsModel;
}
long FreeBoardModel::getSerialBaud() {
	return this->config.serialBaud;
}
void FreeBoardModel::setSerialBaud(long serialBaud) {
	this->config.serialBaud = serialBaud;
}
long FreeBoardModel::getSerialBaud1() {
	return this->config.serialBaud1;
}
void FreeBoardModel::setSerialBaud1(long serialBaud1) {
	this->config.serialBaud1 = serialBaud1;
}
long FreeBoardModel::getSerialBaud2() {
	return this->config.serialBaud2;
}
void FreeBoardModel::setSerialBaud2(long serialBaud2) {
	this->config.serialBaud2 = serialBaud2;
}
long FreeBoardModel::getSerialBaud3() {
	return this->config.serialBaud3;
}
void FreeBoardModel::setSerialBaud3(long serialBaud3) {
	this->config.serialBaud3 = serialBaud3;
}
long FreeBoardModel::getSerialBaud4() {
	return this->config.serialBaud4;
}
void FreeBoardModel::setSerialBaud4(long serialBaud4) {
	this->config.serialBaud4 = serialBaud4;
}
long FreeBoardModel::getSerialBaud5() {
	return this->config.serialBaud5;
}
void FreeBoardModel::setSerialBaud5(long serialBaud5) {
	this->config.serialBaud5 = serialBaud5;
}
long FreeBoardModel::getAutopilotBaud() {
	return this->config.autopilotBaud;
}
void FreeBoardModel::setAutopilotBaud(long autopilotBaud) {
	this->config.autopilotBaud = autopilotBaud;
}
bool FreeBoardModel::getSeaTalk() {
	return this->config.seaTalk;
}
void FreeBoardModel::setSeaTalk(bool seaTalk) {
	this->config.seaTalk = seaTalk;
}

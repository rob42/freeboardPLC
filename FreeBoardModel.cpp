/*
 * FreeBoardModel.cpp
 *
 *  Created on: Mar 28, 2012
 *      Author: robert
 */

#include "FreeBoardModel.h"

FreeBoardModel::FreeBoardModel(){
	alarmLast=0; //millis time of last beep state change
	alarmSnooze=0; //5 minute alarm snooze
	//unsigned long alarmTriggered ; //true if any alarm is triggered - derived

	//anchor
	//float anchorRadius; //anchor alarm radius in meters
	anchorState.anchorRadiusDeg=0.0; //anchor alarm radius in decimal degrees, eg 1deg = 60NM.

	//anchor alarm data
	//float anchorLat; // variable for reading the anchor latitude
	//float anchorLon; // variable for reading the anchor longitude
	anchorState.anchorDistance=0.0;
	anchorState.anchorMaxDistance=0.0;
	//bool anchorAlarmOn; //flag to turn anchor alarm on/off toggle
	anchorState.anchorAlarmTriggered=false; //set to true to trigger anchor alarm

	//a box around the anchor, shrinks with every GPS reading to home in on the anchor itself
	anchorState.anchorN=90.0;
	anchorState.anchorS=-90.0;
	anchorState.anchorE=180.0;
	anchorState.anchorW=-180.0;

	//autopilot
	//bool autopilotOn;
	autopilotState.autopilotReference=AUTOPILOT_COMPASS;
	autopilotState.autopilotCurrentHeading=0; //Input
	autopilotState.autopilotTargetHeading=0; //Setpoint
	autopilotState.autopilotRudderCommand=33; //Output (rudder central)
	//bool autopilotAlarmOn;
	autopilotState.autopilotAlarmTriggered=false;
	autopilotState.autopilotAlarmMaxXTError=100; // +/- meters cross track error
	autopilotState.autopilotAlarmMaxWindError=10; // +/- wind angle change, for over 1 minute
	autopilotState.autopilotAlarmMaxCourseError=10; // +/- course error, for over 1 minute

	//gps
	gpsState.gpsDecode=false; //flag to indicate a new sentence was decoded.
	gpsState.gpsLastFix=0; //time of last good gps fix.
	gpsState.gpsUtc=0; // decimal value of UTC term in last full GPRMC sentence
	gpsState.gpsStatus='V'; //  status character in last full GPRMC sentence ('A' or 'V')
	gpsState.gpsLatitude=0.0; // signed degree-decimal value of latitude terms in last full GPRMC sentence
	gpsState.gpsLongitude=0.0; // signed degree-decimal value of longitude terms in last full GPRMC sentence
	//float gpsSpeedUnit; //unit multiplier for gpsSpeed. 1.0 = KNT,1.1507794	=MPH, see nmea.h
	gpsState.gpsSpeed=0.0; // speed-on-ground term in last full GPRMC sentence
	gpsState.gpsCourse=0.0; // track-angle-made-good term in last full GPRMC sentence
	//bool gpsAlarmOn; //true to engage alarm
	gpsState.gpsAlarmTriggered=false; //set to true to trigger gps alarm
	//double gpsAlarmFixTime; //max time in millis without fix

	//lcd
	lcdLastUpdate=0;

	//menu
	menuState=GPS; //default, show gps data
	menuLevel=0;

	//seatalk
	//volatile bool radarAlarmOn; //set to true to enable radar alarm
	radarAlarmTriggered=false; //set to true to trigger radar alarm
	//volatile bool mobAlarmOn; //set to true to enable mob alarm
	mobAlarmTriggered=false; //set to true to trigger MOB alarm

	//wind
	windState.windLastUpdate=0;
	windState.windAverage=0;
	//windState.windFactor=0;
	windState.windMax=0;
	windState.windApparentDir=0;
	//int windAlarmSpeed;
	//bool windAlarmOn;
	windState.windAlarmTriggered=false;

	//struct Configuration{
		config.anchorLat=0.0;
		config.anchorLon=0.0;
		config.anchorRadius=40.0;
		config.anchorAlarmOn=false;
		config.autopilotOn=false;
		config.autopilotAlarmOn=false;
		config.gpsSpeedUnit=KTS;
		config.gpsAlarmOn=false;
		config.gpsAlarmFixTime=1000l*60*5; //5 min
		config.radarAlarmOn=false;
		config.mobAlarmOn=false;
		config.windAlarmSpeed=99;
		config.windAlarmOn=false;
		windState.windFactor=0;
		config.windZeroOffset=0;
	//}config;

//we change this if we change the struct so we can tell before reloading incompatible versions
	version=3;
}

template<class T> int  writeObject(HardwareSerial ser, T& value, char name ) {
	unsigned char* p = ( unsigned char*) ( void*) &value;
	unsigned int i;
	char checksum = '0';
	ser.write('@');
	ser.write(name);
	for (i = 0; i < sizeof(value); i++){
		ser.write(*p++);
		checksum=checksum ^ *p;
	}
	ser.write(checksum);
	ser.write('\n');
	return i;

}

int FreeBoardModel::sendData(HardwareSerial ser,  char name ) {
	if(CONFIG_T== name){
		return writeObject(ser, config, name);
	}
	return -1;
}



template<class T> int readObject(HardwareSerial ser,T& value, char name) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	char checksum = '0';
	for (i = 0; i < sizeof(value); i++){
		*p++ = ser.read();
		checksum=checksum ^ *p;
	}
	//TODO: make sure this actually works
	if(ser.read()==checksum){
		Serial.print("Checksum valid");
	}else{
		Serial.print("Checksum invalid");
	}

	return i;
}

int FreeBoardModel::receiveData(HardwareSerial ser, char name){
	if(CONFIG_T== name){
		return readObject(ser, config, name);
	}
	return -1;
}

template<class T> int EEPROM_writeAnything(int ee,  T& value) {
	 unsigned char* p = ( unsigned char*) ( void*) &value;
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
void FreeBoardModel::saveConfig()
{
	//write out a current version
	EEPROM_writeAnything(0,version);
	//write data
	EEPROM_writeAnything(4,config);
}

void FreeBoardModel::readConfig()
{
	//check versions here
	int ver;
	EEPROM_readAnything(0,ver);
	if(ver!=version){
		//do any conversion of old data here
		//save
		saveConfig();
	}

	//now we know its compatible
	EEPROM_readAnything(4,config);
	//update
	windState.windFactor=config.windFactor;


}
//accessors
bool FreeBoardModel::isWindAlarmTriggered()
{
    return windState.windAlarmTriggered;
}

unsigned long FreeBoardModel::getAlarmLast()
{
    return alarmLast;
}

unsigned long FreeBoardModel::getAlarmSnooze()
{
    return alarmSnooze;
}

float FreeBoardModel::getAnchorDistance()
{
    return anchorState.anchorDistance;
}

float FreeBoardModel::getAnchorE()
{
    return anchorState.anchorE;
}

float FreeBoardModel::getAnchorLat()
{
    return config.anchorLat;
}

float FreeBoardModel::getAnchorLon()
{
    return config.anchorLon;
}

float FreeBoardModel::getAnchorMaxDistance()
{
    return anchorState.anchorMaxDistance;
}

float FreeBoardModel::getAnchorN()
{
    return anchorState.anchorN;
}

float FreeBoardModel::getAnchorRadius()
{
    return config.anchorRadius;
}

float FreeBoardModel::getAnchorRadiusDeg()
{
    return anchorState.anchorRadiusDeg;
}

float FreeBoardModel::getAnchorS()
{
    return anchorState.anchorS;
}

float FreeBoardModel::getAnchorW()
{
    return anchorState.anchorW;
}

/*
 * Returns -179 to +180 as the degrees off course
 */
double FreeBoardModel::getAutopilotOffCourse(){
	//get degrees between
	autopilotState.autopilotOffCourse=(getAutopilotTargetHeading()+360)- (getAutopilotCurrentHeading()+360);
	autopilotState.autopilotOffCourse=fmod(autopilotState.autopilotOffCourse,360.0);
	//if its >abs(180), then we want to go the -ve (shorter) direction
	if(fabs(autopilotState.autopilotOffCourse)>180)autopilotState.autopilotOffCourse=autopilotState.autopilotOffCourse-360;
	return autopilotState.autopilotOffCourse;
}

int FreeBoardModel::getAutopilotReference()
{
    return autopilotState.autopilotReference;
}


double FreeBoardModel::getAutopilotAlarmMaxCourseError()
{
    return autopilotState.autopilotAlarmMaxCourseError;
}

double FreeBoardModel::getAutopilotAlarmMaxWindError()
{
    return autopilotState.autopilotAlarmMaxWindError;
}

double FreeBoardModel::getAutopilotAlarmMaxXtError()
{
    return autopilotState.autopilotAlarmMaxXTError;
}


double FreeBoardModel::getAutopilotRudderCommand()
{
    return autopilotState.autopilotRudderCommand;
}

double FreeBoardModel::getAutopilotTargetHeading()
{
    return autopilotState.autopilotTargetHeading;
}
double FreeBoardModel::getAutopilotCurrentHeading()
{
    return autopilotState.autopilotCurrentHeading;
}

long FreeBoardModel::getGpsAlarmFixTime()
{
    return config.gpsAlarmFixTime;
}

float FreeBoardModel::getGpsCourse()
{
    return gpsState.gpsCourse;
}

unsigned long FreeBoardModel::getGpsLastFix()
{
    return gpsState.gpsLastFix;
}

float FreeBoardModel::getGpsLatitude()
{
    return gpsState.gpsLatitude;
}

float FreeBoardModel::getGpsLongitude()
{
    return gpsState.gpsLongitude;
}

float FreeBoardModel::getGpsSpeed()
{
    return gpsState.gpsSpeed;
}

float FreeBoardModel::getGpsSpeedUnit()
{
    return config.gpsSpeedUnit;
}

char FreeBoardModel::getGpsStatus()
{
    return gpsState.gpsStatus;
}

float FreeBoardModel::getGpsUtc()
{
    return gpsState.gpsUtc;
}

unsigned long FreeBoardModel::getLcdLastUpdate()
{
    return lcdLastUpdate;
}

volatile int FreeBoardModel::getMenuLevel()
{
    return menuLevel;
}

volatile int FreeBoardModel::getMenuState()
{
    return menuState;
}

volatile bool FreeBoardModel::isMobAlarmTriggered()
{
    return mobAlarmTriggered;
}

volatile bool FreeBoardModel::isRadarAlarmTriggered()
{
    return radarAlarmTriggered;
}

int FreeBoardModel::getWindZeroOffset()
{
    return config.windZeroOffset;
}

int FreeBoardModel::getWindAlarmSpeed()
{
    return config.windAlarmSpeed;
}

int FreeBoardModel::getWindApparentDir()
{
    return windState.windApparentDir;
}

int FreeBoardModel::getWindAverage()
{
    return windState.windAverage;
}

float FreeBoardModel::getWindFactor()
{
    return windState.windFactor;
}

unsigned long FreeBoardModel::getWindLastUpdate()
{
    return windState.windLastUpdate;
}

int FreeBoardModel::getWindMax()
{
    return windState.windMax;
}


bool FreeBoardModel::isAnchorAlarmOn()
{
    return config.anchorAlarmOn;
}

bool FreeBoardModel::isAnchorAlarmTriggered()
{
    return anchorState.anchorAlarmTriggered;
}

bool FreeBoardModel::isAutopilotAlarmOn()
{
    return config.autopilotAlarmOn;
}

bool FreeBoardModel::isAutopilotAlarmTriggered()
{
    return autopilotState.autopilotAlarmTriggered;
}

bool FreeBoardModel::isGpsAlarmOn()
{
    return config.gpsAlarmOn;
}

bool FreeBoardModel::isGpsAlarmTriggered()
{
    return gpsState.gpsAlarmTriggered;
}

bool FreeBoardModel::isGpsDecode()
{
    return gpsState.gpsDecode;
}

bool FreeBoardModel::isWindAlarmOn()
{
    return config.windAlarmOn;
}


void FreeBoardModel::setAlarmLast(unsigned long  alarmLast)
{
    this->alarmLast = alarmLast;
}

void FreeBoardModel::setAlarmSnooze(unsigned long  alarmSnooze)
{
    this->alarmSnooze = alarmSnooze;
}

void FreeBoardModel::setAnchorAlarmOn(bool anchorAlarmOn)
{
    this->config.anchorAlarmOn = anchorAlarmOn;
}

void FreeBoardModel::setAnchorAlarmTriggered(bool anchorAlarmTriggered)
{
    this->anchorState.anchorAlarmTriggered = anchorAlarmTriggered;
}

void FreeBoardModel::setAnchorDistance(float anchorDistance)
{
    this->anchorState.anchorDistance = anchorDistance;
}

void FreeBoardModel::setAnchorE(float anchorE)
{
    this->anchorState.anchorE = anchorE;
}

void FreeBoardModel::setAnchorLat(float anchorLat)
{
    this->config.anchorLat = anchorLat;
}

void FreeBoardModel::setAnchorLon(float anchorLon)
{
    this->config.anchorLon = anchorLon;
}

void FreeBoardModel::setAnchorMaxDistance(float anchorMaxDistance)
{
    this->anchorState.anchorMaxDistance = anchorMaxDistance;
}

void FreeBoardModel::setAnchorN(float anchorN)
{
    this->anchorState.anchorN = anchorN;
}

void FreeBoardModel::setAnchorRadius(float anchorRadius)
{
    this->config.anchorRadius = anchorRadius;
}

void FreeBoardModel::setAnchorRadiusDeg(float anchorRadiusDeg)
{
    this->anchorState.anchorRadiusDeg = anchorRadiusDeg;
}

void FreeBoardModel::setAnchorS(float anchorS)
{
    this->anchorState.anchorS = anchorS;
}

void FreeBoardModel::setAnchorW(float anchorW)
{
    this->anchorState.anchorW = anchorW;
}

void FreeBoardModel::setAutopilotReference(int autopilotReference)
{
    this->autopilotState.autopilotReference = autopilotReference;
}

void FreeBoardModel::setAutopilotAlarmMaxCourseError(double autopilotAlarmMaxCourseError)
{
    this->autopilotState.autopilotAlarmMaxCourseError = autopilotAlarmMaxCourseError;
}

void FreeBoardModel::setAutopilotAlarmMaxWindError(double autopilotAlarmMaxWindError)
{
    this->autopilotState.autopilotAlarmMaxWindError = autopilotAlarmMaxWindError;
}

void FreeBoardModel::setAutopilotAlarmMaxXtError(double autopilotAlarmMaxXtError)
{
	autopilotState.autopilotAlarmMaxXTError = autopilotAlarmMaxXtError;
}

void FreeBoardModel::setAutopilotAlarmOn(bool autopilotAlarmOn)
{
    this->config.autopilotAlarmOn = autopilotAlarmOn;
}

void FreeBoardModel::setAutopilotAlarmTriggered(bool autopilotAlarmTriggered)
{
    this->autopilotState.autopilotAlarmTriggered = autopilotAlarmTriggered;
}


void FreeBoardModel::setAutopilotCurrentHeading(double autopilotCurrentHeading)
{
	//make this 0-360 range only
		if(autopilotCurrentHeading>=0 && autopilotCurrentHeading <=360)
			this->autopilotState.autopilotCurrentHeading = autopilotCurrentHeading;
}

void FreeBoardModel::setAutopilotRudderCommand(double autopilotRudderCommand)
{
    this->autopilotState.autopilotRudderCommand = autopilotRudderCommand;
}



void FreeBoardModel::setAutopilotTargetHeading(double autopilotTargetHeading)
{
	//make this 0-360 range only
	if(autopilotTargetHeading>=0 && autopilotTargetHeading <=360)
		this->autopilotState.autopilotTargetHeading = autopilotTargetHeading;
}


void FreeBoardModel::setGpsAlarmFixTime(long gpsAlarmFixTime)
{
    this->config.gpsAlarmFixTime = gpsAlarmFixTime;
}

void FreeBoardModel::setGpsAlarmOn(bool gpsAlarmOn)
{
    this->config.gpsAlarmOn = gpsAlarmOn;
}

void FreeBoardModel::setGpsAlarmTriggered(bool gpsAlarmTriggered)
{
    this->gpsState.gpsAlarmTriggered = gpsAlarmTriggered;
}

void FreeBoardModel::setGpsCourse(float gpsCourse)
{
    this->gpsState.gpsCourse = gpsCourse;
}

void FreeBoardModel::setGpsDecode(bool gpsDecode)
{
    this->gpsState.gpsDecode = gpsDecode;
}

void FreeBoardModel::setGpsLastFix(unsigned long  gpsLastFix)
{
    this->gpsState.gpsLastFix = gpsLastFix;
}

void FreeBoardModel::setGpsLatitude(float gpsLatitude)
{
    this->gpsState.gpsLatitude = gpsLatitude;
}

void FreeBoardModel::setGpsLongitude(float gpsLongitude)
{
    this->gpsState.gpsLongitude = gpsLongitude;
}

void FreeBoardModel::setGpsSpeed(float gpsSpeed)
{
    this->gpsState.gpsSpeed = gpsSpeed;
}

void FreeBoardModel::setGpsSpeedUnit(float gpsSpeedUnit)
{
    this->config.gpsSpeedUnit = gpsSpeedUnit;
}

void FreeBoardModel::setGpsStatus(char gpsStatus)
{
    this->gpsState.gpsStatus = gpsStatus;
}

void FreeBoardModel::setGpsUtc(float gpsUtc)
{
    this->gpsState.gpsUtc = gpsUtc;
}

void FreeBoardModel::setLcdLastUpdate(unsigned long  lcdLastUpdate)
{
    this->lcdLastUpdate = lcdLastUpdate;
}

void FreeBoardModel::setMenuLevel(volatile int menuLevel)
{
    this->menuLevel = menuLevel;
}

void FreeBoardModel::setMenuState(volatile int menuState)
{
    this->menuState = menuState;
}

void FreeBoardModel::setMobAlarmTriggered(volatile bool mobAlarmTriggered)
{
    this->mobAlarmTriggered = mobAlarmTriggered;
}

void FreeBoardModel::setRadarAlarmTriggered(volatile bool radarAlarmTriggered)
{
    this->radarAlarmTriggered = radarAlarmTriggered;
}


void FreeBoardModel::setWindZeroOffset(int windZeroOffset)
{
    this->config.windZeroOffset = windZeroOffset;
}

void FreeBoardModel::setWindAlarmOn(bool windAlarmOn)
{
    this->config.windAlarmOn = windAlarmOn;
}

void FreeBoardModel::setWindAlarmSpeed(int windAlarmSpeed)
{
    this->config.windAlarmSpeed = windAlarmSpeed;
}

void FreeBoardModel::setWindApparentDir(int windApparentDir)
{
    this->windState.windApparentDir = windApparentDir;
}

void FreeBoardModel::setWindAverage(int windAverage)
{
    this->windState.windAverage = windAverage;
}

void FreeBoardModel::setWindFactor(float windFactor)
{
    this->windState.windFactor = windFactor;
}

void FreeBoardModel::setWindLastUpdate(unsigned long  windLastUpdate)
{
    this->windState.windLastUpdate = windLastUpdate;
}

volatile bool FreeBoardModel::isAlarmTriggered()
{
    return windState.windAlarmTriggered && radarAlarmTriggered && gpsState.gpsAlarmTriggered
    		&& anchorState.anchorAlarmTriggered && autopilotState.autopilotAlarmTriggered && mobAlarmTriggered;
}

volatile bool FreeBoardModel::isMobAlarmOn()
{
    return config.mobAlarmOn;
}

volatile bool FreeBoardModel::isRadarAlarmOn()
{
    return config.radarAlarmOn;
}

void FreeBoardModel::setMobAlarmOn(volatile bool mobAlarmOn)
{
    this->config.mobAlarmOn = mobAlarmOn;
}

bool FreeBoardModel::isAutopilotOn()
{
    return config.autopilotOn;
}

void FreeBoardModel::setAutopilotOn(bool autopilotOn)
{
    this->config.autopilotOn = autopilotOn;
}

void FreeBoardModel::setRadarAlarmOn(volatile bool radarAlarmOn)
{
    this->config.radarAlarmOn = radarAlarmOn;
}

void FreeBoardModel::setWindMax(int windMax)
{
    this->windState.windMax = windMax;
}

void FreeBoardModel::setWindAlarmTriggered(bool windAlarmTriggered)
{
    this->windState.windAlarmTriggered = windAlarmTriggered;
}


















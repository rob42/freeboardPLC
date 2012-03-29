/*
 * FreeBoardModel.cpp
 *
 *  Created on: Mar 28, 2012
 *      Author: robert
 */

#include "FreeBoardModel.h"


bool FreeBoardModel::isWindAlarmTriggered() const
{
    return windAlarmTriggered;
}

unsigned long FreeBoardModel::getAlarmLast() const
{
    return alarmLast;
}

unsigned long FreeBoardModel::getAlarmSnooze() const
{
    return alarmSnooze;
}

float FreeBoardModel::getAnchorDistance() const
{
    return anchorDistance;
}

float FreeBoardModel::getAnchorE() const
{
    return anchorE;
}

float FreeBoardModel::getAnchorLat() const
{
    return anchorLat;
}

float FreeBoardModel::getAnchorLon() const
{
    return anchorLon;
}

float FreeBoardModel::getAnchorMaxDistance() const
{
    return anchorMaxDistance;
}

float FreeBoardModel::getAnchorN() const
{
    return anchorN;
}

float FreeBoardModel::getAnchorRadius() const
{
    return anchorRadius;
}

float FreeBoardModel::getAnchorRadiusDeg() const
{
    return anchorRadiusDeg;
}

float FreeBoardModel::getAnchorS() const
{
    return anchorS;
}

float FreeBoardModel::getAnchorW() const
{
    return anchorW;
}


double FreeBoardModel::getAutopilotAlarmMaxCourseError() const
{
    return autopilotAlarmMaxCourseError;
}

double FreeBoardModel::getAutopilotAlarmMaxWindError() const
{
    return autopilotAlarmMaxWindError;
}

double FreeBoardModel::getAutopilotAlarmMaxXtError() const
{
    return autopilotAlarmMaxXTError;
}


double FreeBoardModel::getAutopilotRudderCommand() const
{
    return autopilotRudderCommand;
}

double FreeBoardModel::getAutopilotTargetHeading() const
{
    return autopilotTargetHeading;
}
double FreeBoardModel::getAutopilotCurrentHeading() const
{
    return autopilotCurrentHeading;
}

double FreeBoardModel::getGpsAlarmFixTime() const
{
    return gpsAlarmFixTime;
}

float FreeBoardModel::getGpsCourse() const
{
    return gpsCourse;
}

unsigned long FreeBoardModel::getGpsLastFix() const
{
    return gpsLastFix;
}

float FreeBoardModel::getGpsLatitude() const
{
    return gpsLatitude;
}

float FreeBoardModel::getGpsLongitude() const
{
    return gpsLongitude;
}

float FreeBoardModel::getGpsSpeed() const
{
    return gpsSpeed;
}

float FreeBoardModel::getGpsSpeedUnit() const
{
    return gpsSpeedUnit;
}

char FreeBoardModel::getGpsStatus() const
{
    return gpsStatus;
}

float FreeBoardModel::getGpsUtc() const
{
    return gpsUtc;
}

unsigned long FreeBoardModel::getLcdLastUpdate() const
{
    return lcdLastUpdate;
}

volatile int FreeBoardModel::getMenuLevel() const
{
    return menuLevel;
}

volatile int FreeBoardModel::getMenuState() const
{
    return menuState;
}

volatile bool FreeBoardModel::isMobAlarmTriggered() const
{
    return mobAlarmTriggered;
}

volatile bool FreeBoardModel::isRadarAlarmTriggered() const
{
    return radarAlarmTriggered;
}

int FreeBoardModel::getWindAlarmSpeed() const
{
    return windAlarmSpeed;
}

int FreeBoardModel::getWindApparentDir() const
{
    return windApparentDir;
}

int FreeBoardModel::getWindAverage() const
{
    return windAverage;
}

float FreeBoardModel::getWindFactor() const
{
    return windFactor;
}

unsigned long FreeBoardModel::getWindLastUpdate() const
{
    return windLastUpdate;
}

int FreeBoardModel::getWindMax() const
{
    return windMax;
}


bool FreeBoardModel::isAnchorAlarmOn() const
{
    return anchorAlarmOn;
}

bool FreeBoardModel::isAnchorAlarmTriggered() const
{
    return anchorAlarmTriggered;
}

bool FreeBoardModel::isAutopilotAlarmOn() const
{
    return autopilotAlarmOn;
}

bool FreeBoardModel::isAutopilotAlarmTriggered() const
{
    return autopilotAlarmTriggered;
}

bool FreeBoardModel::isGpsAlarmOn() const
{
    return gpsAlarmOn;
}

bool FreeBoardModel::isGpsAlarmTriggered() const
{
    return gpsAlarmTriggered;
}

bool FreeBoardModel::isGpsDecode() const
{
    return gpsDecode;
}

bool FreeBoardModel::isWindAlarmOn() const
{
    return windAlarmOn;
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
    this->anchorAlarmOn = anchorAlarmOn;
}

void FreeBoardModel::setAnchorAlarmTriggered(bool anchorAlarmTriggered)
{
    this->anchorAlarmTriggered = anchorAlarmTriggered;
}

void FreeBoardModel::setAnchorDistance(float anchorDistance)
{
    this->anchorDistance = anchorDistance;
}

void FreeBoardModel::setAnchorE(float anchorE)
{
    this->anchorE = anchorE;
}

void FreeBoardModel::setAnchorLat(float anchorLat)
{
    this->anchorLat = anchorLat;
}

void FreeBoardModel::setAnchorLon(float anchorLon)
{
    this->anchorLon = anchorLon;
}

void FreeBoardModel::setAnchorMaxDistance(float anchorMaxDistance)
{
    this->anchorMaxDistance = anchorMaxDistance;
}

void FreeBoardModel::setAnchorN(float anchorN)
{
    this->anchorN = anchorN;
}

void FreeBoardModel::setAnchorRadius(float anchorRadius)
{
    this->anchorRadius = anchorRadius;
}

void FreeBoardModel::setAnchorRadiusDeg(float anchorRadiusDeg)
{
    this->anchorRadiusDeg = anchorRadiusDeg;
}

void FreeBoardModel::setAnchorS(float anchorS)
{
    this->anchorS = anchorS;
}

void FreeBoardModel::setAnchorW(float anchorW)
{
    this->anchorW = anchorW;
}


void FreeBoardModel::setAutopilotAlarmMaxCourseError(double autopilotAlarmMaxCourseError)
{
    this->autopilotAlarmMaxCourseError = autopilotAlarmMaxCourseError;
}

void FreeBoardModel::setAutopilotAlarmMaxWindError(double autopilotAlarmMaxWindError)
{
    this->autopilotAlarmMaxWindError = autopilotAlarmMaxWindError;
}

void FreeBoardModel::setAutopilotAlarmMaxXtError(double autopilotAlarmMaxXtError)
{
    autopilotAlarmMaxXTError = autopilotAlarmMaxXtError;
}

void FreeBoardModel::setAutopilotAlarmOn(bool autopilotAlarmOn)
{
    this->autopilotAlarmOn = autopilotAlarmOn;
}

void FreeBoardModel::setAutopilotAlarmTriggered(bool autopilotAlarmTriggered)
{
    this->autopilotAlarmTriggered = autopilotAlarmTriggered;
}

void FreeBoardModel::setAutopilotCurrentHeading(double autopilotCurrentHeading)
{
    this->autopilotCurrentHeading = autopilotCurrentHeading;
}

void FreeBoardModel::setAutopilotRudderCommand(double autopilotRudderCommand)
{
    this->autopilotRudderCommand = autopilotRudderCommand;
}

void FreeBoardModel::setAutopilotTargetHeading(double autopilotTargetHeading)
{
    this->autopilotTargetHeading = autopilotTargetHeading;
}

void FreeBoardModel::setGpsAlarmFixTime(double gpsAlarmFixTime)
{
    this->gpsAlarmFixTime = gpsAlarmFixTime;
}

void FreeBoardModel::setGpsAlarmOn(bool gpsAlarmOn)
{
    this->gpsAlarmOn = gpsAlarmOn;
}

void FreeBoardModel::setGpsAlarmTriggered(bool gpsAlarmTriggered)
{
    this->gpsAlarmTriggered = gpsAlarmTriggered;
}

void FreeBoardModel::setGpsCourse(float gpsCourse)
{
    this->gpsCourse = gpsCourse;
}

void FreeBoardModel::setGpsDecode(bool gpsDecode)
{
    this->gpsDecode = gpsDecode;
}

void FreeBoardModel::setGpsLastFix(unsigned long  gpsLastFix)
{
    this->gpsLastFix = gpsLastFix;
}

void FreeBoardModel::setGpsLatitude(float gpsLatitude)
{
    this->gpsLatitude = gpsLatitude;
}

void FreeBoardModel::setGpsLongitude(float gpsLongitude)
{
    this->gpsLongitude = gpsLongitude;
}

void FreeBoardModel::setGpsSpeed(float gpsSpeed)
{
    this->gpsSpeed = gpsSpeed;
}

void FreeBoardModel::setGpsSpeedUnit(float gpsSpeedUnit)
{
    this->gpsSpeedUnit = gpsSpeedUnit;
}

void FreeBoardModel::setGpsStatus(char gpsStatus)
{
    this->gpsStatus = gpsStatus;
}

void FreeBoardModel::setGpsUtc(float gpsUtc)
{
    this->gpsUtc = gpsUtc;
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

void FreeBoardModel::setWindAlarmOn(bool windAlarmOn)
{
    this->windAlarmOn = windAlarmOn;
}

void FreeBoardModel::setWindAlarmSpeed(int windAlarmSpeed)
{
    this->windAlarmSpeed = windAlarmSpeed;
}

void FreeBoardModel::setWindApparentDir(int windApparentDir)
{
    this->windApparentDir = windApparentDir;
}

void FreeBoardModel::setWindAverage(int windAverage)
{
    this->windAverage = windAverage;
}

void FreeBoardModel::setWindFactor(float windFactor)
{
    this->windFactor = windFactor;
}

void FreeBoardModel::setWindLastUpdate(unsigned long  windLastUpdate)
{
    this->windLastUpdate = windLastUpdate;
}

volatile bool FreeBoardModel::isAlarmTriggered() const
{
    return windAlarmTriggered && radarAlarmTriggered && gpsAlarmTriggered
    		&& anchorAlarmTriggered && autopilotAlarmTriggered && mobAlarmTriggered;
}

volatile bool FreeBoardModel::isMobAlarmOn() const
{
    return mobAlarmOn;
}

volatile bool FreeBoardModel::isRadarAlarmOn() const
{
    return radarAlarmOn;
}

void FreeBoardModel::setMobAlarmOn(volatile bool mobAlarmOn)
{
    this->mobAlarmOn = mobAlarmOn;
}

bool FreeBoardModel::isAutopilotOn() const
{
    return autopilotOn;
}

void FreeBoardModel::setAutopilotOn(bool autopilotOn)
{
    this->autopilotOn = autopilotOn;
}

void FreeBoardModel::setRadarAlarmOn(volatile bool radarAlarmOn)
{
    this->radarAlarmOn = radarAlarmOn;
}

void FreeBoardModel::setWindMax(int windMax)
{
    this->windMax = windMax;
}

void FreeBoardModel::setWindAlarmTriggered(bool windAlarmTriggered)
{
    this->windAlarmTriggered = windAlarmTriggered;
}

template<class T> int EEPROM_writeAnything(int ee, const T& value) {
	const unsigned char* p = (const unsigned char*) (const void*) &value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		EEPROM.write(ee++, *p++);
	return i;
}

template<class T> int EEPROM_readAnything(int ee, T& value) {
	unsigned char* p = (unsigned char*) (void*) &value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
	return i;
}
void FreeBoardModel::saveConfig()
{
	//load data
	config.anchorAlarmOn=isAnchorAlarmOn();
	config.anchorLat=getAnchorLat();
	config.anchorLon=getAnchorLon();
	config.anchorRadius=getAnchorRadius();
	config.autopilotOn=isAutopilotOn();
	config.autopilotAlarmOn=isAutopilotOn();
	config.gpsSpeedUnit=getGpsSpeedUnit();
	config.gpsAlarmOn=isGpsAlarmOn();
	config.radarAlarmOn=isRadarAlarmOn();
	config.mobAlarmOn=isMobAlarmOn();
	config.windAlarmSpeed=getWindAlarmSpeed();
	config.windAlarmOn=isWindAlarmOn();
	config.windFactor=getWindFactor();
	EEPROM_writeAnything(4,config);
}

void FreeBoardModel::readConfig()
{
	EEPROM_readAnything(4,config);
	setAnchorAlarmOn(config.anchorAlarmOn );
	setAnchorLat(config.anchorLat);
	setAnchorLon(config.anchorLon);
	setAnchorRadius(config.anchorRadius);
	setAutopilotOn(config.autopilotOn);
	setAutopilotOn(config.autopilotAlarmOn);
	setGpsSpeedUnit(config.gpsSpeedUnit);
	setGpsAlarmOn(config.gpsAlarmOn);
	setRadarAlarmOn(config.radarAlarmOn);
	setMobAlarmOn(config.mobAlarmOn);
	setWindAlarmSpeed(config.windAlarmSpeed);
	setWindAlarmOn(config.windAlarmOn);
	setWindFactor(config.windFactor);

}









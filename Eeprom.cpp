/*
 * EEPROM.cpp
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */
#ifndef EEPROM_CPP_
#define EEPROM_CPP_

#include <EEPROM/EEPROM.h>
//EEPROM memory addresses
#define anchorStateAddr 16
#define anchorLatAddr  1
#define anchorLonAddr  6
#define anchorRadiusAddr 11
#define windAlarmSpeedAddr 15
#define windAlarmOnAddr 19

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
//Wind
void saveWindAlarmState(bool windAlarm) {
	EEPROM_writeAnything(windAlarmOnAddr, windAlarm);
}
bool readWindAlarmState() {
	bool alarm = false;
	EEPROM_readAnything(windAlarmOnAddr, alarm);
	return alarm;
}
void saveWindAlarmSpeed(int windSpeed) {
	EEPROM_writeAnything(windAlarmSpeedAddr, windSpeed);

}
int readWindAlarmSpeed() {
	int speed = 0.0;
	EEPROM_readAnything(windAlarmSpeedAddr, speed);
	return speed;
}

//Anchor
void saveAnchorAlarmState(bool anchorAlarmOn) {
	EEPROM_writeAnything(anchorStateAddr, anchorAlarmOn);
}
void saveAnchorAlarmLat(float anchorLat) {
	EEPROM_writeAnything(anchorLatAddr, anchorLat);
}
void saveAnchorAlarmLon(float anchorLon) {
	EEPROM_writeAnything(anchorLonAddr, anchorLon);
}
void saveAnchorAlarmRadius(float anchorRadius) {
	EEPROM_writeAnything(anchorRadiusAddr, anchorRadius);
}
// read the last anchor alarm values
bool getAnchorAlarmState() {
	bool anchorAlarmOn = false;
	EEPROM_readAnything(anchorStateAddr, anchorAlarmOn);
	return anchorAlarmOn;
}
float getAnchorAlarmLat() {
	float anchorLat = 0.0;
	EEPROM_readAnything(anchorLatAddr, anchorLat);
	return anchorLat;
}
float getAnchorAlarmLon() {
	float anchorLon = 0.0;
	EEPROM_readAnything(anchorLonAddr, anchorLon);
	return anchorLon;
}
float getAnchorAlarmRadius() {
	float anchorRadius = 3.0;
	EEPROM_readAnything(anchorRadiusAddr, anchorRadius);
	return anchorRadius;
}

#endif

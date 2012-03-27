/*
 * Wind.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef WIND_H_
#define WIND_H_

#include "Arduino.h"

#include <PString/PString.h>
#include <SoftwareSerial/SoftwareSerial.h>
#include "FreeBoardConstants.h"
#include "Lcd.h"
#include "NmeaSerial.h"
#include <AverageList/AverageList.h>

extern void saveWindAlarmState(bool windAlarmOn);
extern void saveWindAlarmSpeed(int windAlarmSpeed);
extern int readWindAlarmSpeed();
extern bool readWindAlarmState();
class Wind {

public:
	Wind(NmeaSerial* nmea);
	void printWindNmea();
	void setupWindAlarm();
	void checkWindAlarm();
	void calcWindData();
	void setWindAlarm(bool flag);
	bool getWindAlarm();

	void drawWindScreen(Lcd lcd, int menuLevel);
	void drawWindBox(Lcd lcd, int menuState);
	void eraseWindBox(Lcd lcd, int menuState);
	unsigned long getLastWindReading() const;

	unsigned long getLastWindUpdate() const;

	int getWindAverage() const;

	int getWindMax() const;
	void setLastWindReading( unsigned long lastWindReading);

	void setLastWindUpdate( unsigned long lastWindUpdate);
	void setWindAlarmSpeed( int windAlarmSpeed);
	int getWindAlarmSpeed();
	void setWindAverage( int windAverage);

	void setWindMax( int windMax);
	void showWindData(Lcd lcd, int menuLevel,int menuState);
	void readWindDataSpeed();
	void readWindDataDir();
private:

	NmeaSerial* nmea;

	 unsigned long lastWindUpdate;
	 unsigned long lastLcdUpdate;
	 int windAverage;
	 float windFactor;
	 int windMax;
	 int windApparentDir;
	 int windAlarmSpeed;
	 bool windAlarmOn;
	 bool windAlarmTriggered;
	 char windSentence [30];
	 int cs;



};
#endif /* WIND_H_ */

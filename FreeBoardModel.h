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
 * FreeBoardModel.h
 *
 *Holds all the global model vars
 *
 *  Created on: Mar 28, 2012
 *      Author: robert
 */

#ifndef FREEBOARDMODEL_H_
#define FREEBOARDMODEL_H_

#include "Arduino.h"
#include <avr/pgmspace.h>
#include <nmea.h>
#include <EEPROM.h>
#include "FreeBoardConstants.h"
#include "math.h"

#define AUTOPILOT_WIND 'W'
#define AUTOPILOT_COMPASS 'C'
#define EEPROM_VER 13
#define EEPROM_DATA 4
class FreeBoardModel {
public:
//alarms
	FreeBoardModel();

	bool isWindAlarmTriggered();
	void setWindAlarmTriggered(bool windAlarmTriggered);
	unsigned long getAlarmLast();
	unsigned long getAlarmSnooze();
	float getAnchorDistance();
	float getAnchorE();
	float getAnchorLat();
	float getAnchorLon();
	float getAnchorMaxDistance();
	float getAnchorN();
	float getAnchorRadius();
	float getAnchorRadiusDeg();
	float getAnchorS();
	float getAnchorW();
	char getAutopilotReference();
	double getAutopilotOffCourse();
	double getAutopilotAlarmMaxCourseError();
	double getAutopilotAlarmMaxWindError();
	double getAutopilotAlarmMaxXtError();
	double getAutopilotCurrentHeading();
	double getAutopilotRudderCommand();
	double getAutopilotTargetHeading();
	int getAutopilotDeadZone();
	int getAutopilotSlack();
	long getAutopilotSpeed();
	long getGpsAlarmFixTime();
	float getGpsCourse();
	unsigned long getGpsLastFix();
	float getGpsLatitude();
	float getGpsLongitude();
	float getGpsSpeed();
	float getGpsSpeedUnit();
	char getGpsStatus();
	float getGpsUtc();
	float getMagneticHeading();
	float getDeclination();
	volatile bool isMobAlarmTriggered();
	volatile bool isLvl1AlarmTriggered();
	int getLvl1UpperLimit();
	void setLvl1UpperLimit(int lvl1UpperLimit);
	int getLvl1LowerLimit();
	void setLvl1LowerLimit(int lvl1LowerLimit);

	volatile bool isLvl2AlarmTriggered();
	int getLvl2UpperLimit();
	void setLvl2UpperLimit(int lvl2UpperLimit);
	int getLvl2LowerLimit();
	void setLvl2LowerLimit(int lvl2LowerLimit);

	volatile bool isLvl3AlarmTriggered();
	int getLvl3UpperLimit();
	void setLvl3UpperLimit(int lvl3UpperLimit);
	int getLvl3LowerLimit();
	void setLvl3LowerLimit(int lvl3LowerLimit);

	volatile bool isRadarAlarmTriggered();
	int getWindZeroOffset();
	int getWindAlarmSpeed();
	int getWindApparentDir();
	int getWindTrueDir();
	float getWindAverage();
	float getWindFactor();
	unsigned long getWindLastUpdate();
	int getWindMax();

	//bool isAlarmBeepState() ;
	bool isAnchorAlarmOn();
	bool isAnchorAlarmTriggered();
	bool isAutopilotAlarmOn();
	bool isAutopilotAlarmTriggered();
	bool isGpsAlarmOn();
	bool isGpsAlarmTriggered();
	bool isGpsDecode();
	bool isWindAlarmOn();

	// void setAlarmBeepState(bool alarmBeepState);
	void setAlarmLast(unsigned long alarmLast);
	void setAlarmSnooze(unsigned long alarmSnooze);
	void setAnchorAlarmOn(bool anchorAlarmOn);
	void setAnchorAlarmTriggered(bool anchorAlarmTriggered);
	void setAnchorDistance(float anchorDistance);
	void setAnchorE(float anchorE);
	void setAnchorLat(float anchorLat);
	void setAnchorLon(float anchorLon);
	void setAnchorMaxDistance(float anchorMaxDistance);
	void setAnchorN(float anchorN);
	void setAnchorRadius(float anchorRadius);
	void setAnchorRadiusDeg(float anchorRadiusDeg);
	void setAnchorS(float anchorS);
	void setAnchorW(float anchorW);
	void setAutopilotReference(char ref);
	void setAutopilotAlarmMaxCourseError(double autopilotAlarmMaxCourseError);
	void setAutopilotAlarmMaxWindError(double autopilotAlarmMaxWindError);
	void setAutopilotAlarmMaxXtError(double autopilotAlarmMaxXtError);
	void setAutopilotAlarmOn(bool autopilotAlarmOn);
	void setAutopilotAlarmTriggered(bool autopilotAlarmTriggered);
	//void setAutopilotCurrentHeading(double autopilotCurrentHeading);
	void setAutopilotRudderCommand(double autopilotRudderCommand);
	void setAutopilotTargetHeading(double autopilotTargetHeading);
	void setAutopilotDeadZone(int deadZone);
	void setAutopilotSlack(int slack);
	void setAutopilotSpeed(long speed);
	void setGpsAlarmFixTime(long gpsAlarmFixTime);
	void setGpsAlarmOn(bool gpsAlarmOn);
	void setGpsAlarmTriggered(bool gpsAlarmTriggered);
	void setGpsCourse(float gpsCourse);
	void setGpsDecode(bool gpsDecode);
	void setGpsLastFix(unsigned long gpsLastFix);
	void setGpsLatitude(float gpsLatitude);
	void setGpsLongitude(float gpsLongitude);
	void setGpsSpeed(float gpsSpeed);
	void setGpsSpeedUnit(float gpsSpeedUnit);
	void setGpsStatus(char gpsStatus);
	void setGpsUtc(float gpsUtc);
	void setLcdLastUpdate(unsigned long lcdLastUpdate);
	void setMenuLevel(volatile int menuLevel);
	void setMenuState(volatile int menuState);
	void setMobAlarmTriggered(volatile bool mobAlarmTriggered);
	void setLvl1AlarmTriggered(volatile bool lvl1AlarmTriggered);
	void setLvl2AlarmTriggered(volatile bool lvl2AlarmTriggered);
	void setLvl3AlarmTriggered(volatile bool lvl3AlarmTriggered);
	void setRadarAlarmTriggered(volatile bool radarAlarmTriggered);
	void setWindZeroOffset(int windZeroOffset);
	void setWindAlarmOn(bool windAlarmOn);
	void setWindAlarmSpeed(int windAlarmSpeed);
	void setWindApparentDir(int windApparentDir);
	void setWindAverage(float windAverage);
	void setWindFactor(float windFactor);
	void setWindLastUpdate(unsigned long windLastUpdate);
	void setWindMax(int windMax);
	void setWindTrueDir(int windTrueDir);
	void setMagneticHeading(float magneticHeading);
	void setDeclination(float declination);
	volatile bool isAlarmTriggered();
	volatile bool isMobAlarmOn();
	volatile bool isRadarAlarmOn();
	void setMobAlarmOn(volatile bool mobAlarmOn);
	void setRadarAlarmOn(volatile bool radarAlarmOn);
	bool isAutopilotOn();
	void setAutopilotOn(bool autopilotOn);
	short getGpsModel();
	void setGpsModel(short gpsModel);
	//console
	long getSerialBaud();
	void setSerialBaud(long serialBaud);
	//GPS
	long getSerialBaud1();
	void setSerialBaud1(long serialBaud1);
	//NMEA1
	long getSerialBaud2();
	void setSerialBaud2(long serialBaud2);
	//NMEA2
	long getSerialBaud3();
	void setSerialBaud3(long serialBaud3);
	//NMEA3 - SPI-2
	long getSerialBaud4();
	void setSerialBaud4(long serialBaud4);
	//NMEA talker
	long getSerialBaud5();
	void setSerialBaud5(long serialBaud5);
	//autopilot - SPI-1
	long getAutopilotBaud();
	void setAutopilotBaud(long autopilotBaud);

	bool getSeaTalk();
	void setSeaTalk(bool seaTalk);

	void saveConfig();
	void readConfig();
	int writeSimple(HardwareSerial ser);
	int writeConfig(HardwareSerial ser);
	int sendData(HardwareSerial ser, char name);
	int receiveData(HardwareSerial ser, char name);
private:

	unsigned long alarmLast; //toggle to make alarm beep - beep beep
	unsigned long alarmSnooze; //5 minute alarm snooze
	//unsigned long alarmTriggered ; //true if any alarm is triggered - derived
	float magneticHeading;
	float declination;
	//anchor
	struct AnchorState {
		//float anchorRadius; //anchor alarm radius in meters
		float anchorRadiusDeg; //anchor alarm radius in decimal degrees, eg 1deg = 60NM.

		//anchor alarm data
		//float anchorLat; // variable for reading the anchor latitude
		//float anchorLon; // variable for reading the anchor longitude
		float anchorDistance;
		float anchorMaxDistance;
		//bool anchorAlarmOn; //flag to turn anchor alarm on/off toggle
		bool anchorAlarmTriggered; //set to true to trigger anchor alarm

		//a box around the anchor, shrinks with every GPS reading to home in on the anchor itself
		float anchorN;
		float anchorS;
		float anchorE;
		float anchorW;
	} anchorState;

	//autopilot
	struct AutopilotState {
		bool autopilotOn;
		double autopilotOffCourse; //-179 to +180
		char autopilotReference; //WIND (W) or COMPASS(C)
		//these need to always be positive for autopilot
		//double autopilotCurrentHeading; //Input 0-360
		double autopilotTargetHeading; //Setpoint 0-360
		double autopilotRudderCommand; //Output 0-66
		//bool autopilotAlarmOn;
		bool autopilotAlarmTriggered;
		double autopilotAlarmMaxXTError; //cross track error
		double autopilotAlarmMaxWindError; //wind angle change while on windpilot
		double autopilotAlarmMaxCourseError; //course error
		long autopilotSpeed; //angular movement rate as (increments of total)/sec
	} autopilotState;

	//gps
	//eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62  //64 bytes
	struct GpsState { //27 bytes
		bool gpsDecode; //flag to indicate a new sentence was decoded.  					1byte
		unsigned long gpsLastFix; //time of last good gps fix.							32 bits (4 bytes). non neg
		float gpsUtc; // decimal value of UTC term in last full GPRMC sentence					32 bits (4 bytes)
		char gpsStatus; //  status character in last full GPRMC sentence ('A' or 'V')				1byte
		float gpsLatitude; // signed degree-decimal value of latitude terms in last full GPRMC sentence		32 bits (4 bytes)
		float gpsLongitude; // signed degree-decimal value of longitude terms in last full GPRMC sentence	32 bits (4 bytes)
		//float gpsSpeedUnit; //unit multiplier for gpsSpeed. 1.0 = KNT,1.1507794	=MPH, see nmea.h
		float gpsSpeed; // speed-on-ground term in last full GPRMC sentence					32 bits (4 bytes)
		float gpsCourse; // track-angle-made-good term in last full GPRMC sentence				32 bits (4 bytes)
		//bool gpsAlarmOn; //true to engage alarm
		bool gpsAlarmTriggered; //set to true to trigger gps alarm						1byte
		//double gpsAlarmFixTime; //max time in millis without fix
	} gpsState;

	//seatalk
	//volatile bool radarAlarmOn; //set to true to enable radar alarm
	bool radarAlarmTriggered; //set to true to trigger radar alarm
	//volatile bool mobAlarmOn; //set to true to enable mob alarm
	bool mobAlarmTriggered; //set to true to trigger MOB alarm
	bool lvl1AlarmTriggered; //set to true to trigger lvl1 alarm
	bool lvl2AlarmTriggered; //set to true to trigger lvl2 alarm
	bool lvl3AlarmTriggered; //set to true to trigger lvl3 alarm

	//wind
	struct WindState { //15 bytes

		unsigned long windLastUpdate; //32 bits (4 bytes). non neg
		float windAverage; //16 bits (2 bytes)
		//float windFactor;		//32 bits (4 bytes). 
		int windMax; //16 bits (2 bytes).
		int windApparentDir; //16 bits (2 bytes)
		int windTrueDir; //16 bits (2 bytes)
		//int windAlarmSpeed;
		//bool windAlarmOn;
		bool windAlarmTriggered; //1 byte
	} windState;

	//CONFIG_T
	struct Configuration { // 57 bytes
		//ver 5
		float anchorLat; //32 bits (4 bytes).
		float anchorLon; //32 bits (4 bytes).
		float anchorRadius; //32 bits (4 bytes).
		bool anchorAlarmOn; //1 byte
		bool autopilotAlarmOn; //1 byte
		int autopilotDeadZone; //16 bits (2 bytes)
		int autopilotSlack; //16 bits (2 bytes)
		long autopilotSpeed; //32 bits (4 bytes)
		float gpsSpeedUnit; //32 bits (4 bytes).
		bool gpsAlarmOn; //1 byte
		long gpsAlarmFixTime; //32 bits (4 bytes).
		bool radarAlarmOn; //1 byte
		bool mobAlarmOn; //1 byte
		int windAlarmSpeed; //16 bits (2 bytes)
		bool windAlarmOn; //1 byte
		float windFactor; //32 bits (4 bytes).
		int windZeroOffset; //16 bits (2 bytes)
		//ver 6
		short gpsModel; //2 bytes
		long serialBaud; //4 bytes, baud can be > 64000!
		long serialBaud1; //4 bytes
		long serialBaud2; //4 bytes
		long serialBaud3; //4 bytes
		long serialBaud4; //4 bytes
		long serialBaud5; //4 bytes
		long autopilotBaud; //4 bytes
		bool seaTalk; //1 bytes
		int lvl1UpperLimit; //0-1024 analogue range - higher is slower to alarm, eg more lvl1
		int lvl1LowerLimit; //0-1024 analogue range - lower is slower to alarm, eg less lvl1
		int lvl2UpperLimit; //0-1024 analogue range - higher is slower to alarm, eg more lvl2
		int lvl2LowerLimit; //0-1024 analogue range - lower is slower to alarm, eg less lvl2
		int lvl3UpperLimit; //0-1024 analogue range - higher is slower to alarm, eg more lvl3
		int lvl3LowerLimit; //0-1024 analogue range - lower is slower to alarm, eg less lvl3
	} config;

	int version;
};

#endif /* FREEBOARDMODEL_H_ */

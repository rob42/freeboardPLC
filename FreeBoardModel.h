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
#include <NMEA/nmea.h>
#include <EEPROM/EEPROM.h>
#include "FreeBoardConstants.h"
#include "math.h"

#define AUTOPILOT_WIND 1;
#define AUTOPILOT_COMPASS 0;
class FreeBoardModel {
public:
//alarms
	FreeBoardModel();

	bool isWindAlarmTriggered() ;
	void setWindAlarmTriggered(bool windAlarmTriggered);
	unsigned long getAlarmLast() ;
	unsigned long getAlarmSnooze() ;
	float getAnchorDistance() ;
	float getAnchorE() ;
	float getAnchorLat() ;
	float getAnchorLon() ;
	float getAnchorMaxDistance() ;
	float getAnchorN() ;
	float getAnchorRadius() ;
	float getAnchorRadiusDeg() ;
	float getAnchorS() ;
	float getAnchorW() ;
	int getAutopilotReference() ;
	double getAutopilotOffCourse();
	double getAutopilotAlarmMaxCourseError() ;
	double getAutopilotAlarmMaxWindError() ;
	double getAutopilotAlarmMaxXtError() ;
	double getAutopilotCurrentHeading();
	double getAutopilotRudderCommand();
	double getAutopilotTargetHeading();
	long getGpsAlarmFixTime() ;
	float getGpsCourse() ;
	unsigned long getGpsLastFix() ;
	float getGpsLatitude() ;
	float getGpsLongitude() ;
	float getGpsSpeed() ;
	float getGpsSpeedUnit() ;
	char getGpsStatus() ;
	float getGpsUtc() ;
	unsigned long getLcdLastUpdate() ;
	volatile int getMenuLevel() ;
	volatile int getMenuState() ;
	volatile bool isMobAlarmTriggered() ;
	volatile bool isRadarAlarmTriggered() ;
	int getWindZeroOffset() ;
	int getWindAlarmSpeed() ;
	int getWindApparentDir() ;
	int getWindAverage() ;
	float getWindFactor() ;
	unsigned long getWindLastUpdate() ;
	int getWindMax() ;
	//bool isAlarmBeepState() ;
	bool isAnchorAlarmOn() ;
	bool isAnchorAlarmTriggered() ;
	bool isAutopilotAlarmOn() ;
	bool isAutopilotAlarmTriggered() ;
	bool isGpsAlarmOn() ;
	bool isGpsAlarmTriggered() ;
	bool isGpsDecode() ;
	bool isWindAlarmOn() ;
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
	void setAutopilotReference(int ref);
	void setAutopilotAlarmMaxCourseError(double autopilotAlarmMaxCourseError);
	void setAutopilotAlarmMaxWindError(double autopilotAlarmMaxWindError);
	void setAutopilotAlarmMaxXtError(double autopilotAlarmMaxXtError);
	void setAutopilotAlarmOn(bool autopilotAlarmOn);
	void setAutopilotAlarmTriggered(bool autopilotAlarmTriggered);
	void setAutopilotCurrentHeading(double autopilotCurrentHeading);
	void setAutopilotRudderCommand(double autopilotRudderCommand);
	void setAutopilotTargetHeading(double autopilotTargetHeading);
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
	void setRadarAlarmTriggered(volatile bool radarAlarmTriggered);
	void setWindZeroOffset(int windZeroOffset);
	void setWindAlarmOn(bool windAlarmOn);
	void setWindAlarmSpeed(int windAlarmSpeed);
	void setWindApparentDir(int windApparentDir);
	void setWindAverage(int windAverage);
	void setWindFactor(float windFactor);
	void setWindLastUpdate(unsigned long windLastUpdate);
	void setWindMax(int windMax);
	volatile bool isAlarmTriggered() ;
	volatile bool isMobAlarmOn() ;
	volatile bool isRadarAlarmOn() ;
	void setMobAlarmOn(volatile bool mobAlarmOn);
	void setRadarAlarmOn(volatile bool radarAlarmOn);
    bool isAutopilotOn() ;
    void setAutopilotOn(bool autopilotOn);
    void saveConfig();
    void readConfig();
    int sendConfig(HardwareSerial ser);
    int receiveConfig(HardwareSerial ser);
private:

	unsigned long alarmLast; //toggle to make alarm beep - beep beep
	unsigned long alarmSnooze; //5 minute alarm snooze
	//unsigned long alarmTriggered ; //true if any alarm is triggered - derived


	//anchor
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

	//autopilot
	//bool autopilotOn;

	double autopilotOffCourse; //-179 to +180
	int autopilotReference; //WIND (1) or COMPASS(0)
	//these need to always be positive for autopilot
	double autopilotCurrentHeading; //Input 0-360
	double autopilotTargetHeading; //Setpoint 0-360
	double autopilotRudderCommand; //Output 0-66
	//bool autopilotAlarmOn;
	bool autopilotAlarmTriggered;
	double autopilotAlarmMaxXTError; //cross track error
	double autopilotAlarmMaxWindError; //wind angle change while on windpilot
	double autopilotAlarmMaxCourseError; //course error

	//gps
	bool gpsDecode; //flag to indicate a new sentence was decoded.
	unsigned long gpsLastFix; //time of last good gps fix.
	float gpsUtc; // decimal value of UTC term in last full GPRMC sentence
	char gpsStatus; //  status character in last full GPRMC sentence ('A' or 'V')
	float gpsLatitude; // signed degree-decimal value of latitude terms in last full GPRMC sentence
	float gpsLongitude; // signed degree-decimal value of longitude terms in last full GPRMC sentence
	//float gpsSpeedUnit; //unit multiplier for gpsSpeed. 1.0 = KNT,1.1507794	=MPH, see nmea.h
	float gpsSpeed; // speed-on-ground term in last full GPRMC sentence
	float gpsCourse; // track-angle-made-good term in last full GPRMC sentence
	//bool gpsAlarmOn; //true to engage alarm
	bool gpsAlarmTriggered; //set to true to trigger gps alarm
	//double gpsAlarmFixTime; //max time in millis without fix

	//lcd
	unsigned long lcdLastUpdate;

	//menu
	volatile int menuState; //default, show gps data
	volatile int menuLevel;

	//seatalk
	//volatile bool radarAlarmOn; //set to true to enable radar alarm
	bool radarAlarmTriggered; //set to true to trigger radar alarm
	//volatile bool mobAlarmOn; //set to true to enable mob alarm
	bool mobAlarmTriggered; //set to true to trigger MOB alarm

	//wind
	unsigned long windLastUpdate;
	int windAverage;
	//float windFactor;
	int windMax;
	int windApparentDir;
	//int windAlarmSpeed;
	//bool windAlarmOn;
	bool windAlarmTriggered;

	struct Configuration{
		float anchorLat;
		float anchorLon;
		float anchorRadius;
		bool anchorAlarmOn;
		bool autopilotOn;
		bool autopilotAlarmOn;
		float gpsSpeedUnit;
		bool gpsAlarmOn;
		long gpsAlarmFixTime;
		bool radarAlarmOn;
		bool mobAlarmOn;
		int windAlarmSpeed;
		bool windAlarmOn;
		float windFactor;
		int windZeroOffset;

	}config;

	int version;
};

#endif /* FREEBOARDMODEL_H_ */

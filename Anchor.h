/*
 * Anchor.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef ANCHOR_H_
#define ANCHOR_H_

#include "Arduino.h"

//#include <NewSoftSerial/NewSoftSerial.h>
#include "Gps.h"
#include "Lcd.h"

extern void saveAnchorAlarmState(bool anchorAlarmOn) ;
extern void saveAnchorAlarmLat(float anchorLat) ;
extern void saveAnchorAlarmLon(float anchorLon) ;
extern void saveAnchorAlarmRadius(float anchorRadius);
// read the last anchor alarm values
extern bool getAnchorAlarmState() ;
extern float getAnchorAlarmLat();
extern float getAnchorAlarmLon();
extern float getAnchorAlarmRadius();


// 0.00053995680 nautical miles per meter
//decimal lat lon is in degrees, and we have 60 NM per degree so degrees per meter
#define LLMTRS .00000899928
#define MINMTRS	LLMTRS*5.0
//anchor
class Anchor {
public:
	Anchor(Gps* gps);
	virtual ~Anchor();

	bool getAnchorAlarmTriggered();
	void setAnchorAlarmTriggered(bool state);
	bool getAnchorAlarmState();
	void setAnchorAlarmState(bool state);
	void setAnchorAlarm(boolean flag);

	void setAnchorRadius(float radius);
	float getAnchorRadius();
	void setAnchorPoint();
	void drawAnchorScreen(Lcd lcd, int menuState);
	void showAnchorAlarmData(Lcd lcd, int menuLevel, int menuState);
	void drawAnchorBox(Lcd lcd, int menuState);
	void eraseAnchorBox(Lcd lcd, int menuState);
	void checkAnchor();
private:

	void updateAnchorBox(float laty, float lonx);
	void resetAnchorBox(float laty, float lonx);

	bool inAnchorBox(float laty, float lonx);

	unsigned long lastLcdUpdate;
	float anchorRadius; //alarm radius in meters
	float anchorRadiusDeg; //alarm radius in decimal degrees

	//anchor alarm data
	float anchorLat; // variable for reading the anchor latitude
	float anchorLon; // variable for reading the anchor longitude
	float anchorDistance;
	float maxAnchorDistance;

	//a box around the anchor, shrinks with every GPS reading to home in on the anchor itself
	float anchorN;
	float anchorS;
	float anchorE;
	float anchorW;

	//alarm
	unsigned long snoozeAlarm; //5 minute alarm snooze
	bool anchorAlarmOn; //flag to turn anchor alarm on/off toggle
	bool anchorAlarmTriggered; //set to true to trigger anchor alarm
	Gps* gps;
	static const unsigned long MAX_SINCE_LAST_GPS_FIX = 300000;

};

#endif /* ANCHOR_H_ */

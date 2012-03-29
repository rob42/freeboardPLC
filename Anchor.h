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
#include <PString/PString.h>
#include "Gps.h"
#include "Lcd.h"
#include "FreeBoardModel.h"

extern void saveAnchorAlarmState(bool anchorAlarmOn) ;
extern void saveAnchorAlarmLat(float anchorLat) ;
extern void saveAnchorAlarmLon(float anchorLon) ;
extern void saveAnchorAlarmRadius(float anchorRadius);
// read the last anchor alarm values
extern bool getAnchorAlarmState() ;
extern float getAnchorAlarmLat();
extern float getAnchorAlarmLon();
extern float getAnchorAlarmRadius();



//anchor

class Anchor {

public:
	Anchor(FreeBoardModel* model);
	virtual ~Anchor();



	void setAnchorRadius(float radius);
	float getAnchorRadius();
	void setAnchorPoint();

	void checkAnchor();
private:

	void updateAnchorBox(float laty, float lonx);
	void resetAnchorBox(float laty, float lonx);

	bool inAnchorBox(float laty, float lonx);



	FreeBoardModel* model;
	static const unsigned long MAX_SINCE_LAST_GPS_FIX = 300000;

};

#endif /* ANCHOR_H_ */

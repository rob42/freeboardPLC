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
/* NMEA Anchor alarm
 */

#include "Anchor.h"



Anchor::Anchor(FreeBoardModel* model) {


	this->model=model;
	resetAnchorBox(model->getAnchorLat(), model->getAnchorLon());
}

Anchor::~Anchor(){

}


/* Adjust the size of the anchor box. Will not go smaller than MINMTRS */
void Anchor::updateAnchorBox(float laty, float lonx) {
	//TODO: error testing around 0, 90, 180deg

	float n = laty + model->getAnchorRadiusDeg();
	float s = laty - model->getAnchorRadiusDeg();
	float e = lonx + model->getAnchorRadiusDeg();
	float w = lonx - model->getAnchorRadiusDeg();
	//shrink anchor box if we can
	if ((n - MINMTRS) > model->getAnchorS() && n < model->getAnchorN())
		model->setAnchorN(n);
	if ((s + MINMTRS) < model->getAnchorN() && s > model->getAnchorS())
		model->setAnchorS(s);
	if ((e - MINMTRS) > model->getAnchorW() && e < model->getAnchorE())
		model->setAnchorE(e);
	if ((w + MINMTRS) < model->getAnchorE() && w > model->getAnchorW())
		model->setAnchorW(w);

}

void Anchor::resetAnchorBox(float laty, float lonx) {
	model->setAnchorN(90.0);
	model->setAnchorS(-90.0);
	model->setAnchorE(180.0);
	model->setAnchorW(-180.0);
	model->setAnchorRadiusDeg(model->getAnchorRadius() * LLMTRS);
	updateAnchorBox(laty, lonx);
}


/*True if the provided lat/lon is within the anchor box*/
bool Anchor::inAnchorBox(float laty, float lonx) {
	if (laty < model->getAnchorS())
		return false;
	if (laty > model->getAnchorN())
		return false;
	if (lonx > model->getAnchorE())
		return false;
	if (lonx < model->getAnchorW())
		return false;
	return true;
}

float Anchor::getAnchorRadius() {
	return  model->getAnchorRadius();
}

/*set anchor radius and adjust box*/
void Anchor::setAnchorRadius(float radius) {
	if (radius < 0)
		radius = 0;
	float change = radius -  model->getAnchorRadius();
	 model->setAnchorRadius(radius);
	 model->setAnchorRadiusDeg(model->getAnchorRadius() * LLMTRS);
	saveAnchorAlarmRadius(model->getAnchorRadius());

	//deal with the changed box size
	if (change <= 0) {
		return; //no need, new box is smaller, it will sort itself
	} else {
		//grow the box? - add change to all edges?
		change = change * LLMTRS;
		model->setAnchorN(model->getAnchorN() + change);
		model->setAnchorS(model->getAnchorS() - change);
		model->setAnchorE(model->getAnchorE() + change);
		model->setAnchorW(model->getAnchorW() - change);

	}
}
/*calc all params, and set alarm buzzer state*/
void Anchor::checkAnchor() {

	//we dont need to do this if the anchor alarm is off
	if(model->isAnchorAlarmOn()){
		//TODO: if anchor distance greater than 1000 mtrs its probably a mistake so dont fire alarm?
		// check if GPS positioning was active, if we havent had a signal in a while that may be a problem....
		if ((millis() - model->getGpsLastFix() > MAX_SINCE_LAST_GPS_FIX)
				&& model->isAnchorAlarmOn()) {
			model->setAlarmSnooze(0);
			model->setAnchorAlarmTriggered(true);
		}
		//now check position
		if (model->getGpsStatus()) {
			model->setAnchorDistance(Gps::getMetersTo(model->getAnchorLat(), model->getAnchorLon(),model->getGpsLatitude(),model->getGpsLongitude()));
			if (model->getAnchorDistance() > model->getAnchorMaxDistance())
				model->setAnchorMaxDistance(model->getAnchorDistance());

			//update the anchor box
			updateAnchorBox(model->getGpsLatitude(), model->getGpsLongitude());

			// read distance to anchorPoint in meters and set anchorAlarm accordingly
			// if ( anchorAlarmOn && (anchorDistance > anchorRadius || !inAnchorBox(gps->gprmc_latitude(),gps->gprmc_longitude()))) {
			if (model->isAnchorAlarmOn() && model->getAnchorDistance() > model->getAnchorRadius()) {
				model->setAlarmSnooze(0);
				model->setAnchorAlarmTriggered(true);
			} else {
				model->setAnchorAlarmTriggered(false);
			}
		}
	}

}

/*Set anchor position*/
void Anchor::setAnchorPoint() {
	model->setAnchorLat(model->getGpsLatitude());
	model->setAnchorLon(model->getGpsLongitude());
	model->setAnchorMaxDistance(0);

}




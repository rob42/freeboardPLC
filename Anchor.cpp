/* NMEA Anchor alarm
 */

#include "Anchor.h"

Anchor::Anchor(Gps* gps) {
	this->gps = gps;
	lastLcdUpdate= 0;
	anchorRadiusDeg = 0.0;
	anchorDistance = 0;
	maxAnchorDistance = 0;

	//a box around the anchor, shrinks with every GPS reading to home in on the anchor itself
	anchorN = 90.0;
	anchorS = -90.0;
	anchorE = 180.0;
	anchorW = -180.0;

	//alarm

	anchorAlarmTriggered = false; //set to true to trigger anchor alarm

	// read the last anchor alarm values
	anchorAlarmOn = getAnchorAlarmState();//flag to turn anchor alarm on/off toggle
	anchorLat = getAnchorAlarmLat();
	anchorLon = getAnchorAlarmLon();
	anchorRadius = getAnchorAlarmRadius();
	resetAnchorBox(anchorLat, anchorLon);
}

Anchor::~Anchor(){

}

bool Anchor::getAnchorAlarmTriggered() {
	return anchorAlarmTriggered;
}

void Anchor::setAnchorAlarmTriggered(bool state) {
	anchorAlarmTriggered = state;
}

bool Anchor::getAnchorAlarmState() {
	return anchorAlarmOn;
}

void Anchor::setAnchorAlarmState(bool state) {
	anchorAlarmOn = state;
}
/* Adjust the size of the anchor box. Will not go smaller than MINMTRS */
void Anchor::updateAnchorBox(float laty, float lonx) {
	//TODO: error testing around 0, 90, 180deg

	float n = laty + anchorRadiusDeg;
	float s = laty - anchorRadiusDeg;
	float e = lonx + anchorRadiusDeg;
	float w = lonx - anchorRadiusDeg;
	//shrink anchor box if we can
	if ((n - MINMTRS) > anchorS && n < anchorN)
		anchorN = n;
	if ((s + MINMTRS) < anchorN && s > anchorS)
		anchorS = s;
	if ((e - MINMTRS) > anchorW && e < anchorE)
		anchorE = e;
	if ((w + MINMTRS) < anchorE && w > anchorW)
		anchorW = w;

}

void Anchor::resetAnchorBox(float laty, float lonx) {
	anchorN = 90.0;
	anchorS = -90.0;
	anchorE = 180.0;
	anchorW = -180.0;
	anchorRadiusDeg = anchorRadius * LLMTRS;
	updateAnchorBox(laty, lonx);
}


/*True if the provided lat/lon is within the anchor box*/
bool Anchor::inAnchorBox(float laty, float lonx) {
	if (laty < anchorS)
		return false;
	if (laty > anchorN)
		return false;
	if (lonx > anchorE)
		return false;
	if (lonx < anchorW)
		return false;
	return true;
}

float Anchor::getAnchorRadius() {
	return anchorRadius;
}

/*set anchor radius and adjust box*/
void Anchor::setAnchorRadius(float radius) {
	if (radius < 0)
		radius = 0;
	float change = radius - anchorRadius;
	anchorRadius = radius;
	anchorRadiusDeg = anchorRadius * LLMTRS;
	saveAnchorAlarmRadius(anchorRadius);

	//deal with the changed box size
	if (change <= 0) {
		return; //no need, new box is smaller, it will sort itself
	} else {
		//grow the box? - add change to all edges?
		change = change * LLMTRS;
		anchorN = anchorN + change;
		anchorS = anchorS - change;
		anchorE = anchorE + change;
		anchorW = anchorW - change;

	}
}
/*calc all params, and set alarm buzzer state*/
void Anchor::checkAnchor() {

	//we dont need to do this if the anchor alarm is off
	if(anchorAlarmOn){
		//TODO: if anchor distance greater than 1000 mtrs its probably a mistake so dont fire alarm?
		// check if GPS positioning was active, if we havent had a signal in a while that may be a problem....
		if ((millis() - gps->getLastGpsFix() > MAX_SINCE_LAST_GPS_FIX)
				&& anchorAlarmOn) {
			snoozeAlarm = 0;
			anchorAlarmTriggered = true;
		}
		//now check position
		if (gps->getRmcStatus()) {
			anchorDistance = gps->getRmcMetersTo(anchorLat, anchorLon);
			if (anchorDistance > maxAnchorDistance)
				maxAnchorDistance = anchorDistance;

			//update the anchor box
			updateAnchorBox(gps->getLatitude(), gps->getLongitude());

			// read distance to anchorPoint in meters and set anchorAlarm accordingly
			// if ( anchorAlarmOn && (anchorDistance > anchorRadius || !inAnchorBox(gps->gprmc_latitude(),gps->gprmc_longitude()))) {
			if (anchorAlarmOn && anchorDistance > anchorRadius) {
				snoozeAlarm = 0;
				anchorAlarmTriggered = true;
			} else {
				anchorAlarmTriggered = false;
			}
		}
	}

}

/*Set anchor position*/
void Anchor::setAnchorPoint() {
	anchorLat = gps->getLatitude();
	anchorLon = gps->getLongitude();
	maxAnchorDistance = 0;
	saveAnchorAlarmLat(anchorLat);
	saveAnchorAlarmLon(anchorLon);
}

/*Set anchor alarm ON/OFF */
void Anchor::setAnchorAlarm(boolean flag) {
	anchorAlarmOn = flag;
	maxAnchorDistance = 0;
	saveAnchorAlarmState(anchorAlarmOn);
	//populate the box

}

void Anchor::drawAnchorScreen(Lcd lcd, int menuState) {
	lcd.setCursor(2, 60);
	if (anchorAlarmOn) {
		lcd.print("Anchor alarm ON  ");
	} else {
		lcd.print("Anchor alarm OFF ");
	}
	lcd.setCursor(2, 50);
	lcd.print("Lat: ");

	lcd.print(gps->getLatString(anchorLat, 4, 10));
	lcd.setCursor(2, 40);
	lcd.print("Lon: ");
	lcd.print(gps->getLonString(anchorLon, 4, 10));
	lcd.setCursor(2, 30);
	lcd.print("Radius : ");
	lcd.print((int) anchorRadius);
	lcd.print("M     ");
	lcd.setCursor(2, 20);
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		lcd.print("Back to prev menu ");
		//lcd.writeButtonLabels(lcd.SET, lcd.NEXT, lcd.PREV);
	} else {
		lcd.print("Now: ");
		lcd.print((int) anchorDistance);
		// lcd.print("M, max: ");
		//lcd.print((int)maxAnchorDistance);
		//lcd.print("M     ");
		lcd.print("M,Box:");
		float w = (anchorE - anchorW) / LLMTRS;
		float h = (anchorN - anchorS) / LLMTRS;
		lcd.print((int) w);
		lcd.print("x");
		lcd.print((int) h);
		lcd.writeButtonLabels(EDIT, PGDN, PGUP);

	}
	lastLcdUpdate= millis();
}

/*display anchor data on screen*/
void Anchor::showAnchorAlarmData(Lcd lcd, int menuLevel, int menuState) {
	if (menuLevel == 0 && (menuState == ANCHOR || (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU))
			&& gps->getRmcStatus()) {
		if (millis() - lastLcdUpdate > 1000) {
			//update
			lastLcdUpdate= millis();
			drawAnchorScreen(lcd, menuState);
		}
	}
}
void Anchor::drawAnchorBox(Lcd lcd, int menuState) {
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		//draw the correct box
		if (menuState == ANCHORSTATE)
			lcd.drawBox(0, 51, 127, 62);
		if (menuState == ANCHORPOINT)
			lcd.drawBox(0, 31, 127, 52);
		if (menuState == ANCHORRADIUS)
			lcd.drawBox(0, 21, 127, 32);
		if (menuState == ANCHORRTN)
			lcd.drawBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}

void Anchor::eraseAnchorBox(Lcd lcd, int menuState) {
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		//draw the correct box
		if (menuState == ANCHORSTATE)
			lcd.eraseBox(0, 51, 127, 62);
		if (menuState == ANCHORPOINT)
			lcd.eraseBox(0, 31, 127, 52);
		if (menuState == ANCHORRADIUS)
			lcd.eraseBox(0, 21, 127, 32);
		if (menuState == ANCHORRTN)
			lcd.eraseBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}


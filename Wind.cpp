/*=== MWV - Wind Speed and Angle ===
 
 ------------------------------------------------------------------------------
 	 	 1  2  3  4 5  6
 	 	 |  |  |  | |  |
 $--MWV,x.x,a,x.x,a,A*hh<CR><LF>
 ------------------------------------------------------------------------------
 
 Field Number: 
 
 1. Wind Angle, 0 to 360 degrees
 2. Reference, R = Relative, T = True
 3. Wind Speed
 4. Wind Speed Units, Km/Meters/kNots
 5. Status, A = Data Valid
 6. Checksum
 */

#include "Wind.h"
const unsigned int SPEED_DEBOUNCE = 0;
const unsigned int DIR_DEBOUNCE = 0;
volatile unsigned long lastPulse;
volatile unsigned long windSpeedDur;
volatile unsigned long windSpeedMicros;
volatile unsigned long windDirDur;
volatile bool windSpeedFlag;
volatile bool windDirFlag;
unsigned long lastNmeaUpdate;
//volatile int windCount;
typedef volatile long val; //change float to the datatype you want to use
const byte MAX_NUMBER_OF_READINGS = 5;
val speedStorage[MAX_NUMBER_OF_READINGS] = {0};
val dirStorage[MAX_NUMBER_OF_READINGS] = {0};

AverageList<val> speedList = AverageList<val>( speedStorage, MAX_NUMBER_OF_READINGS );
AverageList<val> dirList = AverageList<val>( dirStorage, MAX_NUMBER_OF_READINGS );

Wind::Wind(NmeaSerial* nmea) {
	this->nmea = nmea;
	//initialise the wind interrupt
	//analogReference(INTERNAL); //ref to about 1.1V
	lastWindUpdate = millis(); // time of last screen update
	lastLcdUpdate = millis();

	windAverage = 0; // the average of spikes over time/factor to give knots
	windFactor = 1000000; //adjust factor to actual knots
	windMax = 0; //max of wind average
	windAlarmTriggered = false; //set to true to trigger wind alarm
	lastNmeaUpdate=0;
	windSpeedDur=0;
	windSpeedMicros=micros();
	windDirDur=0;
	windSpeedFlag=true;
	windDirFlag=false;
	speedList.reset();
	dirList.reset();
	cs = 0;
	// read the last wind alarm values
	windAlarmSpeed = readWindAlarmSpeed();
	if (windAlarmSpeed > 99) {
		windAlarmSpeed = 99;
		saveWindAlarmSpeed(windAlarmSpeed);
	}
	windAlarmOn = readWindAlarmState();

}

void Wind::printWindNmea() {
		//Assemble a sentence of the various parts so that we can calculate the proper checksum

		PString str(windSentence, sizeof(windSentence));
		str.print("$IIMWV,");
		str.print(windApparentDir);
		str.print(".0,R,");
		str.print(windAverage);
		str.print(".0,N,A*");
		//calculate the checksum

		cs = 0; //clear any old checksum
		for (unsigned int n = 1; n < strlen(windSentence) - 1; n++) {
			cs ^= windSentence[n]; //calculates the checksum
		}
		str.print(cs, HEX); // Assemble the final message and send it out the serial port
		if(DEBUG)Serial.println(windSentence);
		nmea->printNmea(windSentence);

}

void Wind::setupWindAlarm() {

}

void Wind::checkWindAlarm(){
	//check alarm val
		if (windAlarmOn && windAlarmSpeed > 0 && windAverage > windAlarmSpeed) {
			//TODO: Alarm snooze, better handling of this
			//setSnoozeAlarm(0);
			windAlarmTriggered = true;
		} else {
			windAlarmTriggered = false;
		}
}
/* NMEA Wind routines and alarm
 */
void Wind::readWindDataSpeed() {
	if(windSpeedFlag){
		windSpeedFlag=false;
		windDirFlag=true;
		//called by speed pin interrupt
		//micros resets every 50 min, reset if that happens
		//counts clicks, one per rotation, about 2-3 rotations/s =1m/s = 1.94knts
		//therefore about 50/ms = 100knts = 150 rpm = 1000000/150 = 6500 us per pulse
		//at 100 knts about 20us per degree of rotation
		lastPulse=millis();
		windSpeedDur=micros()-windSpeedMicros;
		//debounce 5ms
		if(windSpeedDur>SPEED_DEBOUNCE){
			//average a bit
			speedList.addValue(windSpeedDur);
			windSpeedMicros=micros();
		}else if (windSpeedDur<0){
			windDirDur=0;
			windSpeedDur=0;
			windSpeedMicros=micros();
		}
	}

}
void Wind::readWindDataDir() {
	if(windDirFlag){
		windSpeedFlag=true;
		windDirFlag=false;
		//called by dir pin interrupt
		//micros resets every 50 min, reset if that happens
		//debounce 5ms
		windDirDur=micros()-windSpeedMicros;
		// calc direction, degrees clockwise
		//check for 0's and no wind.
		if(windSpeedDur>windDirDur && windDirDur>DIR_DEBOUNCE && windSpeedDur>SPEED_DEBOUNCE && windSpeedDur< (3*1000000)){
			//total time to rotate = windSpeedDur
			//time to arrow = windDirDur
			//so windDirDur/windSpeedDur gives the fraction of 360deg
			//should round to int, min 1
			dirList.addRotationalValue(360*((float)windDirDur/windSpeedDur));
		}else if(windDirDur<0){
			windDirDur=0;
			windSpeedDur=0;
		}
	}
}

void Wind::calcWindData() {
//		Serial.print("Windspeed list:");
//		Serial.print(speedList.getTotalAverage());
//		Serial.print(", WindSpeedDur:");
//		Serial.println(windSpeedDur);
//		Serial.print("WindDir list:");
//		Serial.print(dirList.getTotalAverage());
//		Serial.print(", WindDirDur:");
//		Serial.println(windDirDur);
		lastWindUpdate = millis();

		//convert to windAverage
		if(millis()-lastPulse>3000){
			//no rotation, no wind
			windAverage=0;
			speedList.reset();
		}else{
			windAverage =  windFactor / speedList.getTotalAverage() ;

			//update gusts
			if (windAverage > windMax)
				windMax = windAverage;
		}

	// calc direction, degrees clockwise
	//should round to int, min 1
	windApparentDir=(int)dirList.getRotationalAverage();

}

/*Set wind alarm ON/OFF */
void Wind::setWindAlarm(bool flag) {
	this->windAlarmOn = flag;
	saveWindAlarmState(windAlarmOn);
}

unsigned long Wind::getLastWindUpdate() const {
	return this->lastWindUpdate;
}


int Wind::getWindAverage() const {
	return this->windAverage;
}

int Wind::getWindMax() const {
	return this->windMax;
}


void Wind::setLastWindUpdate(unsigned long lastWindUpdate) {
	this->lastWindUpdate = lastWindUpdate;
}

void Wind::setWindAlarmSpeed(int windAlarmSpeed) {
	this->windAlarmSpeed = windAlarmSpeed;
	saveWindAlarmSpeed(windAlarmSpeed);
}

void Wind::setWindAverage(int windAverage) {
	this->windAverage = windAverage;
}

int Wind::getWindAlarmSpeed(){
	return windAlarmSpeed;
}
bool Wind::getWindAlarm() {
	return this->windAlarmOn;
}

void Wind::setWindMax(volatile int windMax) {
	this->windMax = windMax;
}

/*
 Write wind data to screen
 */
void Wind::showWindData(Lcd lcd, int menuLevel, int menuState) {
	if (menuState == WIND && menuLevel == 0) {
		//how long
		if (millis() - lastLcdUpdate > 1500) {
			drawWindScreen(lcd, menuLevel);
			lastLcdUpdate= millis();
		}
	}
}
void Wind::drawWindScreen(Lcd lcd, int menuLevel) {
	//update screen
	lcd.setCursor(2, 60);
	lcd.print("WIND: ");
	lcd.print((int) windAverage);
	lcd.print("KNTS    ");
	lcd.setCursor(2, 50);
	lcd.print("Max Gust: ");
	lcd.print((int) windMax);
	lcd.print("KNTS   ");
	//alarms
	lcd.setCursor(2, 40);
	lcd.print("Wind Alarm: ");
	if (windAlarmOn) {
		lcd.print("ON    ");
	} else {
		lcd.print("OFF    ");
	}
	lcd.setCursor(2, 30);
	lcd.print("Alarm Speed: ");
	lcd.print((int) windAlarmSpeed);
	lcd.print("KNTS   ");
	if (menuLevel >= 1) {
		lcd.setCursor(2, 20);
		lcd.print("Return to prev menu");
		lcd.writeButtonLabels(SET, NEXT, PREV);
	} else {
		lcd.writeButtonLabels(EDIT, PGDN, PGUP);
	}
	lastLcdUpdate= millis();
	if (DEBUG) {
		Serial.print("Wind (KNTS): ");
		Serial.print(windAverage, DEC);
		Serial.print(", gust (KNTS): ");
		Serial.println(windMax, DEC);
	}
}

void Wind::drawWindBox(Lcd lcd, int menuState) {
	if (menuState >= WINDMINMENU && menuState <= WINDMAXMENU) {
		//draw the correct box
		if (menuState == WINDSTATE)
			lcd.drawBox(0, 31, 127, 42);
		if (menuState == WINDSPEED)
			lcd.drawBox(0, 21, 127, 32);
		if (menuState == WINDRTN)
			lcd.drawBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}

void Wind::eraseWindBox(Lcd lcd, int menuState) {
	if (menuState >= WINDMINMENU && menuState <= WINDMAXMENU) {
		//draw the correct box
		if (menuState == WINDSTATE)
			lcd.eraseBox(0, 31, 127, 42);
		if (menuState == WINDSPEED)
			lcd.eraseBox(0, 21, 127, 32);
		if (menuState == WINDRTN)
			lcd.eraseBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}


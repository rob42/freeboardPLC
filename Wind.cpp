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

/*
 * In ULTIMETER Weather Stations, speed is determined by measuring the time interval between
 * two successive closures of the speed reed. Calibration is done as follows (RPS = revolutions
 * per second):
 * 0.010 < RPS < 3.229 (approximately 0.2 < MPH < 8.2):
 * windSpeedDur<309693
 * MPH = -0.1095(RPS2) + 2.9318(RPS) – 0.1412
 * KNTS = -0.09515(RPS2) + 2.5476(RPS) – 0.1226
 *
 * 3.230 < RPS < 54.362 (approximately 8.2 < MPH < 136.0):
 * windSpeedDur < 18395
 * MPH = 0.0052(RPS2) + 2.1980(RPS) + 1.1091
 * KNTS = 0.0045(RPS2) + 1.9099(RPS) + 0.9638
 *
 * 54.363 < RPS < 66.332 (approximately 136.0 < MPH < 181.5):
 *
 * MPH = 0.1104(RPS2) – 9.5685(RPS) + 329.87
 * KNTS = 0.09593(RPS2) – 8.3147(RPS) + 286.65
 *
 * Conversions used are: mph * 0.86897 = knots; mph * 1.6094 = kmph; mph * 0.48037 = m/s
 *
 */

#include "Wind.h"

//lots out here because they are accessed by interrupts
const unsigned int SPEED_DEBOUNCE = 0;
const unsigned int DIR_DEBOUNCE = 0;
volatile unsigned long lastPulse;
volatile unsigned long windSpeedDur=0;
volatile  float windSpeedRpm;
volatile unsigned long windSpeedMicros;
volatile unsigned long windDirDur;
volatile bool windSpeedFlag;
volatile bool windDirFlag;

typedef volatile long val; //change float to the datatype you want to use
typedef volatile float rval; //change float to the datatype you want to use
const byte MAX_NUMBER_OF_READINGS = 3;
val speedStorage[MAX_NUMBER_OF_READINGS] = {0};
rval dirStorage[MAX_NUMBER_OF_READINGS] = {0.0};

AverageList<val> speedList = AverageList<val>( speedStorage, MAX_NUMBER_OF_READINGS );
AverageList<rval> dirList = AverageList<rval>( dirStorage, MAX_NUMBER_OF_READINGS );

Wind::Wind( FreeBoardModel* model) {
	this->model=model;
	//initialise the wind interrupt
	//analogReference(INTERNAL); //ref to about 1.1V
	windSpeedDur=0;
	windSpeedMicros=micros();
	windDirDur=0;
	windSpeedFlag=true;
	windDirFlag=false;
	speedList.reset();
	dirList.reset();

	// read the last wind alarm values
	if (model->getWindAlarmSpeed() > 99) {
		model->setWindAlarmSpeed(99);
	}

}



/* NMEA Wind routines and alarm
 */
void Wind::readWindDataSpeed() {
	if(windSpeedFlag){
		windSpeedFlag=false;
		//called by speed pin interrupt
		//micros resets every 50 min, reset if that happens
		//counts clicks, one per rotation, about 2-3 rotations/s =1m/s = 1.94knts
		//therefore about 50/ms = 100knts = 150 rps = 1000000/150 = 6500 us per pulse
		//at 100 knts about 20us per degree of rotation
		//ULTIMETER:
		// 66RPS = 136MPH == 118 KNTS
		// so 1000000/66=15151/360=42us/degree at 118knts
		lastPulse=millis();
		windSpeedDur=micros()-windSpeedMicros;
		if(windSpeedDur>0){
			speedList.addValue(windSpeedDur);
		}
		windSpeedMicros=micros();
	}

}
void Wind::readWindDataDir() {
	if(!windSpeedFlag){
		windSpeedFlag=true;
		//called by dir pin interrupt
		//micros resets every 50 min, reset if that happens
		//debounce 5ms
		windDirDur=micros()-windSpeedMicros;
		if(windDirDur>0 && windSpeedDur>windDirDur)
			dirList.addValue(((float)windDirDur/(float)windSpeedDur));

	}
}

/*
 * Calculates wind data. Direction is apparent, 0-360 deg off the bow, clockwise, in degrees.
 */
void Wind::calcWindData() {
		//Serial.print("Windspeed list:");
		//Serial.print(speedList.getTotalAverage());
		//Serial.print(", WindSpeedDur:");
		//Serial.println(lastPulse);
		//Serial.print("WindDir list:");
		//Serial.print(dirList.getTotalAverage());
		//Serial.print(", WindDirDur:");
		//Serial.println(windSpeedMicros);


		model->setWindLastUpdate(millis());
		/*if(DEBUG){
			Serial.print("Wind dir:");
						Serial.print(dirList.getTotalAverage());
						Serial.print(", Wind speed:");
						Serial.println(speedList.getTotalAverage());
					}*/
		//convert to windAverage
		if(millis()-lastPulse>3000){
			//no rotation, no wind
			model->setWindAverage(0);
			//Serial.println("Wind speed reset");
			speedList.reset();
		}else{
			if(speedList.getTotalAverage()>0){
				windSpeedRpm=1000000.0/speedList.getTotalAverage();
				//convert to KNTS
				if(windSpeedRpm<3.229){
					model->setWindAverage(model->getWindFactor() *( -0.09515*(windSpeedRpm*windSpeedRpm) + 2.5476*(windSpeedRpm) - 0.1226));
				}else if(windSpeedRpm < 54.362){
					model->setWindAverage(model->getWindFactor() *(0.0045*(windSpeedRpm*windSpeedRpm) + 1.9099*(windSpeedRpm) + 0.9638));
				}else{
					model->setWindAverage(model->getWindFactor() *(0.09593*(windSpeedRpm*windSpeedRpm) - 8.3147*(windSpeedRpm) + 286.65));
				}
				//Serial.print(", WindAvg:");
				//		Serial.println(( -0.09515*(windSpeedRpm*windSpeedRpm) + 2.5476*(windSpeedRpm) - 0.1226));
			}
			//update gusts
			if (model->getWindAverage() > model->getWindMax())
				model->setWindMax(model->getWindAverage());


			// calc direction, degrees clockwise
			//should round to int, min 1
			int dir = (int)dirList.getRotationalAverage();
			//limit to +-360, after adjust zero
			dir = (dir + model->getWindZeroOffset()) %360;
			//if minus, then reverse to +ve
			if (dir<0){
				dir = 360+dir;
			}
			model->setWindApparentDir(dir);
		}
}



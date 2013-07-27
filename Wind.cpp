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
volatile unsigned long windSpeedDur = 0;
volatile long windSpeedRps;
volatile unsigned long windSpeedMicros;
volatile unsigned long windDirDur;
volatile bool windSpeedFlag;
//volatile bool windDirFlag;

typedef volatile unsigned long val; //change float to the datatype you want to use
typedef volatile unsigned long rval; //change float to the datatype you want to use
const byte MAX_NUMBER_OF_READINGS = 3;
val speedStorage[MAX_NUMBER_OF_READINGS] = { 0 };
rval dirStorage[MAX_NUMBER_OF_READINGS] = { 0 };
const unsigned int isinTable16[] = { 0, 1144, 2287, 3430, 4571, 5712, 6850, 7987, 9121, 10252, 11380, 12505, 13625, 14742, 15854, 16962, 18064, 19161, 20251,
		21336, 22414, 23486, 24550, 25607, 26655, 27696, 28729, 29752, 30767, 31772, 32768,

		33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243, 42125, 42995, 43851, 44695, 45524, 46340, 47142, 47929, 48702, 49460, 50203, 50930,
		51642, 52339, 53019, 53683, 54331, 54962, 55577, 56174, 56755,

		57318, 57864, 58392, 58902, 59395, 59869, 60325, 60763, 61182, 61583, 61965, 62327, 62671, 62996, 63302, 63588, 63855, 64103, 64331, 64539, 64728,
		64897, 65047, 65176, 65286, 65375, 65445, 65495, 65525, 65535, };

AverageList<val> speedList = AverageList<val>(speedStorage, MAX_NUMBER_OF_READINGS);
AverageList<rval> dirList = AverageList<rval>(dirStorage, MAX_NUMBER_OF_READINGS);

Wind::Wind(FreeBoardModel* model) {
	this->model = model;
	//initialise the wind interrupt
	//analogReference(INTERNAL); //ref to about 1.1V
	windSpeedDur = 0;
	windSpeedMicros = micros();
	windDirDur = 0;
	windSpeedFlag = true;
	//windDirFlag=false;
	speedList.reset();
	dirList.reset();

	// read the last wind alarm values
	if (model->getWindAlarmSpeed() > 99) {
		model->setWindAlarmSpeed(99);
	}
}

//routines for fast sin and cos

float Wind::isin(long x) {
	boolean pos = true; // positive - keeps an eye on the sign.
	if (x < 0) {
		x = -x;
		pos = !pos;
	}
	if (x >= 360) x %= 360;
	if (x > 180) {
		x -= 180;
		pos = !pos;
	}
	if (x > 90) x = 180 - x;
//  if (pos) return isinTable8[x] * 0.003921568627; // = /255.0
//  return isinTable8[x] * -0.003921568627 ;
	if (pos) return isinTable16[x] * 0.0000152590219; // = /65535.0
	return isinTable16[x] * -0.0000152590219;
}

float Wind::icos(long x) {
	return isin(x + 90);
}

float Wind::itan(long x) {
	return isin(x) / icos(x);
}

float Wind::fsin(float d) {
	float a = isin(d);
	float b = isin(d + 1);
	return a + (d - int(d)) * (b - a);
}

/*
 * return the average of all circular (0-360) values in DEGREES added with an addRotationalValue call
 * Range is 0-360 DEGREES
 */
int Wind::getRotationalAverage() {
	float x = 0;
	float y = 0;
	//float angle=0;
	//Serial.print("DEBUG:windDirDur=");
	//unsigned long wdDur = windDirDur;
	//Serial.print(wdDur);
	//Serial.print(",windSpeedDur=");
	//unsigned long wsDur = windSpeedDur;
	//Serial.println(wsDur);

	for (byte i = 0; i < dirList.getCurrentSize(); i++) {
		//angle=TWO_PI*dirList.getValue(i);
		//x += cos(angle);
		//y += sin(angle);
		x += icos(dirList.getValue(i));
		y += isin(dirList.getValue(i));
		//Serial.print("DEBUG:angle=");
		//Serial.println(dirList.getValue(i));
	}

	//Serial.print(", cos=");
	//Serial.print(x);
	//Serial.print("sin=");
	//Serial.println(y);
	//TODO:watch out for zeros
	if (x == 0.0f && y == 0.0f) {
		return 0.0f;
	}
	return degrees(atan2(y, x));

}

/* NMEA Wind routines and alarm
 */
void Wind::readWindDataSpeed() {
	//fastest rps = 15ms - avoid bounce
	if (windSpeedFlag && millis() - lastPulse > 20ul) {
		windSpeedFlag = false;
		//called by speed pin interrupt
		//micros resets every 50 min, reset if that happens
		//FROM ULTIMETER:
		//at 130 knts about 46us per degree of rotation

		//should not change too fast
		volatile unsigned long wsTemp = (micros() - windSpeedMicros);
		windSpeedMicros = micros();
		//discard rubbish
		if(wsTemp<50ul || wsTemp > 3000000)return;

		if (windSpeedDur > 0ul) {
			volatile unsigned long windSpeedDurDiff = (wsTemp * 100ul) / windSpeedDur;
			//can only change 10% per rev
			if ( windSpeedDurDiff < 90ul) {
				windSpeedDur = (windSpeedDur * 9ul) / 10ul;
			}else if (windSpeedDurDiff > 110ul ) {
				windSpeedDur = (windSpeedDur * 11ul) / 10ul;
			}else{
				//running average, to avoid random errors, adjust by speed
				if(wsTemp<250000){
					windSpeedDur=((10ul*windSpeedDur)+wsTemp)/11ul;
				}else if(wsTemp<500000){
					windSpeedDur=((6ul*windSpeedDur)+wsTemp)/7ul;
				}else{
					windSpeedDur=((3ul*windSpeedDur)+wsTemp)/4ul;
				}
			}
			speedList.addValue(windSpeedDur);
		} else {
			windSpeedDur = wsTemp;
		}


	}
	lastPulse = millis();

}
/*
 * The anemometer turns up to 60rps at 140knots - so min 16ms/16000us per turn
 * Mostly much lower :-)
 */
void Wind::readWindDataDir() {
	if (!windSpeedFlag) {
		windSpeedFlag = true;
		//called by dir pin interrupt
		//micros resets every 50 min, reset if that happens
		//FROM ULTIMETER:
		//at 130 knts about 46us per degree of rotation
		//debounce 5ms
		volatile unsigned long wdTemp = (micros() - windSpeedMicros);

		//dirList.addValue(windDirDur);
		if (wdTemp > 50ul && windSpeedDur > wdTemp) {
			wdTemp = (wdTemp * 360ul) / windSpeedDur;
			//running average, to avoid random errors, adjust by speed
			if(windSpeedDur<250000){
				windDirDur=((10ul*windDirDur)+wdTemp)/11ul;
			}else if(windSpeedDur<500000){
				windDirDur=((6ul*windDirDur)+wdTemp)/7ul;
			}else{
				windDirDur=((3ul*windDirDur)+wdTemp)/4ul;
			}
			dirList.addValue(windDirDur);
		}

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

	//fix rollover
	if (millis() < lastPulse) lastPulse = millis();

	model->setWindLastUpdate(millis());
	/*if(DEBUG){
	 Serial.print("Wind dir:");
	 Serial.print(dirList.getTotalAverage());
	 Serial.print(", Wind speed:");
	 Serial.println(speedList.getTotalAverage());
	 }*/
	//convert to windAverage
	if (millis() - lastPulse > 3000) {
		//no rotation, no wind
		model->setWindAverage(0);
		//Serial.println("Wind speed reset");
		speedList.reset();
	} else {
		//speedlist is type long -  max value = 3000000 micros
		if (speedList.getTotalAverage() > 0) {
			// arduino long = -2,147,483,648 to 2,147,483,647
			//1000 millis = 1 rps - this is 1000 x rps (for int arithmetic) range 333 - 33333
			windSpeedRps = 100000000 / speedList.getTotalAverage();
			//NOTE:converted multipliers to KNTS
			//need to avoid div/0 errors
			if (windSpeedRps < 323) {
				//need extra accuracy here, zero is very unlikely
				windSpeedRps = windSpeedRps * 10;
				model->setWindAverage(((((windSpeedRps * windSpeedRps) / -105) + ((25476 * windSpeedRps) / 100) - 12260)) / model->getWindFactor() * 10);
			} else if (windSpeedRps < 5436) {
				//rps2 = min 10426441, max 30,864,197, cant get div/0 here?
				model->setWindAverage((((windSpeedRps * windSpeedRps) / 2222) + ((19099 * windSpeedRps) / 100) + 9638) / model->getWindFactor());
			} else {
				model->setWindAverage(((((windSpeedRps * windSpeedRps) / 1042) * 100) - (8314700 * windSpeedRps) + 2866500) / model->getWindFactor());
			}
		}
		//update gusts
		if (model->getWindAverage() > model->getWindMax()) model->setWindMax(model->getWindAverage());

		// calc direction, degrees clockwise
		//should round to int, min 1
		int dir = (int) getRotationalAverage();
		//limit to +-360, after adjust zero
		//C = A – B * (A / B)
		dir = (dir + model->getWindZeroOffset()); // %360;
		if (dir != 0) {
			dir = dir - 360 * (dir / 360);
		}
		//if minus, then reverse to +ve
		if (dir < 0) {
			dir = 360 + dir;
		}
		model->setWindApparentDir(dir);
	}
}


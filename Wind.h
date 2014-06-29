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
 * Wind.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef WIND_H_
#define WIND_H_

#include "Arduino.h"

#include <PString.h>

#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"
#include <AverageList.h>

extern void saveWindAlarmState(bool windAlarmOn);
extern void saveWindAlarmSpeed(int windAlarmSpeed);
extern int readWindAlarmSpeed();
extern bool readWindAlarmState();
class Wind {

public:
	Wind( FreeBoardModel* model);
	void calcWindSpeedAndDir();
	void calcWindData();
	void readWindDataSpeed();
	void readWindDataDir();
	long isinLong(long x);
	float isin(long x);
	long icosLong(long x);
	float icos(long x);
	float itan(long x);
	float fsin(float d);
	int getRotationalAverage();
private:

	FreeBoardModel* model;

};
#endif /* WIND_H_ */

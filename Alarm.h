/*
 * Copyright 2012,2013 Robert Huitema robert@42.co.nz
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
 * Alarm.h
 *
 *  Created on: 19/12/2010
 *      Author: robert
 */

#ifndef ALARM_H_
#define ALARM_H_

#include "Arduino.h"

#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class Alarm {
public:
	Alarm(FreeBoardModel* model);
	virtual ~Alarm();
	void checkAlarms();
	bool alarmTriggered();
	void checkWindAlarm();
	void checkLvlAlarms();

private:
	FreeBoardModel* model;
	//unsigned long alarmLast;   //toggle to make alarm beep - beep beep
	bool alarmBeepState; //beep on or off
	//char alarmValue;
	//unsigned long alarmSnooze ; //5 minute alarm snooze
};

#endif /* ALARM_H_ */

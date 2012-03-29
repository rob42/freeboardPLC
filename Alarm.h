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

private:
	FreeBoardModel* model;
	//unsigned long alarmLast;   //toggle to make alarm beep - beep beep
	bool alarmBeepState; //beep on or off
	//char alarmValue;
	//unsigned long alarmSnooze ; //5 minute alarm snooze
};

#endif /* ALARM_H_ */

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

class Alarm {
public:
	Alarm();
	virtual ~Alarm();
	void checkAlarms();
	bool alarmTriggered();
	void setAlarm(int alarm);
	void clearAlarm(int alarm);
	void setSnoozeAlarm(unsigned long value);
	unsigned long getSnoozeAlarm();
	bool isMobAlarmTriggered();
	bool isRadarAlarmTriggered();
	bool isWindAlarmTriggered();

private:
	unsigned long lastAlarm;   //toggle to make alarm beep - beep beep
	bool alarmBeepState; //beep on or off
	char alarmValue;
	unsigned long snoozeAlarm ; //5 minute alarm snooze
};

#endif /* ALARM_H_ */

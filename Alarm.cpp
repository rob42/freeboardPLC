/*
 * Alarm.cpp
 *
 *  Created on: 19/12/2010
 *      Author: robert
 */

#include "Alarm.h"

Alarm::Alarm() {
	alarmBeepState = false;
	lastAlarm = 0;
	alarmValue = 0;
	pinMode(alarmPin0, OUTPUT);
	pinMode(alarmPin1, OUTPUT);
	pinMode(alarmPin2, OUTPUT);
	pinMode(alarmPin3, OUTPUT);
}

Alarm::~Alarm() {

}
void Alarm::setAlarm(int alarm) {
	alarmValue=alarmValue+alarm;
}
void Alarm::clearAlarm(int alarm) {
	alarmValue=alarmValue-alarm;
}
bool Alarm::alarmTriggered() {
	if (alarmValue > 0) {
		if (getSnoozeAlarm() > millis()) {
			return false;
		}
		return true;
	}
	return false;
}

bool Alarm::isWindAlarmTriggered() {
	if ((alarmValue & WINDALARM) == WINDALARM)
		return true;
	return false;
}

bool Alarm::isRadarAlarmTriggered() {
	if ((alarmValue & RADARALARM) == RADARALARM)
		return true;
	return false;
}
bool Alarm::isMobAlarmTriggered() {
	if ((alarmValue & MOBALARM) == MOBALARM)
		return true;
	return false;
}
/* Take action if alarms are triggered*/
void Alarm::checkAlarms() {
	if (alarmTriggered()) {
		//alarm beeps on off on off
		//once in the alarm state, hitting any button will give a 5 minute respite from the beeping, eg snooze
		if (millis() - lastAlarm > 1000) {
			digitalWrite(alarmPin0, alarmBeepState);
			digitalWrite(alarmPin1, alarmBeepState);
			digitalWrite(alarmPin2, alarmBeepState);
			digitalWrite(alarmPin3, alarmBeepState);
			alarmBeepState = !alarmBeepState;
			lastAlarm = millis();
			snoozeAlarm = 0; //5 minute alarm snooze
		}
	} else {
		//no alarm
		digitalWrite(alarmPin0, LOW);
		digitalWrite(alarmPin1, LOW);
		digitalWrite(alarmPin2, LOW);
		digitalWrite(alarmPin3, LOW);
	}
}

void Alarm::setSnoozeAlarm(unsigned long value) {
	snoozeAlarm = value;
}
unsigned long Alarm::getSnoozeAlarm() {
	return snoozeAlarm;
}

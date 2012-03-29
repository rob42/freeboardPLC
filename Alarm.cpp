/*
 * Alarm.cpp
 *
 *  Created on: 19/12/2010
 *      Author: robert
 */

#include "Alarm.h"

Alarm::Alarm(FreeBoardModel* model) {
	this->model=model;
	alarmBeepState=false;
	model->setAlarmLast(0);
	pinMode(alarmPin0, OUTPUT);
	pinMode(alarmPin1, OUTPUT);
	pinMode(alarmPin2, OUTPUT);
	pinMode(alarmPin3, OUTPUT);
}

Alarm::~Alarm() {

}

bool Alarm::alarmTriggered() {
	return model->isAlarmTriggered() && model->getAlarmSnooze() < millis() ;
}


/* Take action if alarms are triggered*/
void Alarm::checkAlarms() {
	if (alarmTriggered()) {
		//alarm beeps on off on off
		//once in the alarm state, hitting any button will give a 5 minute respite from the beeping, eg snooze
		if (millis() - model->getAlarmLast() > 1000) {
			digitalWrite(alarmPin0, alarmBeepState);
			digitalWrite(alarmPin1, alarmBeepState);
			digitalWrite(alarmPin2, alarmBeepState);
			digitalWrite(alarmPin3, alarmBeepState);
			alarmBeepState = !alarmBeepState;
			model->setAlarmLast(millis());
			//model->setAlarmSnooze(0); //5 minute alarm snooze
		}
	} else {
		//no alarm
		digitalWrite(alarmPin0, LOW);
		digitalWrite(alarmPin1, LOW);
		digitalWrite(alarmPin2, LOW);
		digitalWrite(alarmPin3, LOW);
	}
}

void Alarm::checkWindAlarm(){
	//check alarm val
		if (model->isWindAlarmOn() && model->getWindAlarmSpeed() > 0
				&& model->getWindAverage() > model->getWindAlarmSpeed()) {
			//TODO: Alarm snooze, better handling of this
			//setSnoozeAlarm(0);
			model->setWindAlarmTriggered(true);
		} else {
			model->setWindAlarmTriggered(false);
		}
}

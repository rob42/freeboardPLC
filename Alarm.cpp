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
	pinMode(alarmPin0, OUTPUT); //main MOSFET pin
	pinMode(alarmPin1, OUTPUT);
	pinMode(alarmPin2, OUTPUT);
	pinMode(alarmPin3, OUTPUT);
	//TODO: setup lvl3 pin - actually its analogue
	pinMode(lvl3Pin, INPUT);
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

void Alarm::checkLvlAlarms(){
	//check lvl* alarm val, 0 is low, 1024 is high
	int lvl1 = analogRead(lvl1Pin);
			if ( lvl1 < model->getLvl1LowerLimit() || lvl1 >model->getLvl1UpperLimit()) {

				model->setLvl1AlarmTriggered(true);
			} else {
				model->setLvl1AlarmTriggered(false);
			}
	int lvl2 = analogRead(lvl2Pin);
			if ( lvl2 < model->getLvl2LowerLimit() || lvl2 >model->getLvl2UpperLimit()) {

				model->setLvl2AlarmTriggered(true);
			} else {
				model->setLvl2AlarmTriggered(false);
			}
	int lvl3 = analogRead(lvl3Pin);
		if ( lvl3 < model->getLvl3LowerLimit() || lvl3 >model->getLvl3UpperLimit()) {

			model->setLvl3AlarmTriggered(true);
		} else {
			model->setLvl3AlarmTriggered(false);
		}
}

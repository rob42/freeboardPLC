/*
 * Menu.cpp
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#include "Menu.h"



Menu::Menu(Lcd* lcd, Alarm* alarm, Anchor* anchor, Wind* wind, Seatalk* seatalk) {
	this->lcd = lcd;
	this->alarm=alarm;
	this->anchor=anchor;
	this->wind=wind;
	this->seatalk=seatalk;

	this->button0Bouncer = LOW; //anchorPoint
	this->button1Bouncer = LOW; //radius
	this->button2Bouncer = LOW; //toggle alarm
	this->button3Bouncer = LOW; //remote alarm



	this->lastButtonCheck = 0;
	this->menuState = GPS; //default, show gps data
	this->menuLevel = 0; //main menu
}

Menu::~Menu() {
	// TODO Auto-generated destructor stub
}

void Menu::setButton(uint8_t buttonPin) {
	//skip if a button is un-processed
	if(button0Bouncer || button1Bouncer || button2Bouncer || button3Bouncer) return;
	button0Pin=buttonPin;
	if(buttonPin == button0)button0Bouncer=HIGH;
	if(buttonPin == button1)button1Bouncer=HIGH;
	if(buttonPin == button2)button2Bouncer=HIGH;
	if(buttonPin == button3)button3Bouncer=HIGH;
}
/*Check if buttons were pressed initiating a menu
 */
void Menu::checkButtons() {
//	if(DEBUG)Serial.print("ButtonPin=");
//	if(DEBUG)Serial.println(button0Pin);
//	//mainMenu();
//	if(DEBUG)Serial.print("Menu level=");
//	if(DEBUG)Serial.println(menuLevel);
	switch (menuLevel) {
	case 0:
		mainMenu();
		break;

	case 1:
		if (menuState == ANCHOR || (menuState >= ANCHORMINMENU
				&& menuState <= ANCHORMAXMENU)) {
			if(DEBUG)Serial.print("Anchor menu");
			anchorMenu();
			return;
		}
		if (menuState == WIND || (menuState >= WINDMINMENU && menuState
				<= WINDMAXMENU)) {
			windMenu();
			return;
		}
		menuState = MINMENU;
		menuLevel = 0;

		break;

	case 2:
		if (menuState == ANCHORRADIUS) {
			anchorRadiusMenu();
			return;
		}
		if (menuState == WINDSPEED) {
			windSpeedMenu();
			return;
		}
		break;
		// default:

	}
}

void Menu::mainMenu() {

	//remote button;
	if (button3Bouncer == HIGH) {
		if(DEBUG)Serial.print("Pushed button 3");
		handleButtonPushOnAlarm();
		button3Bouncer=LOW;
	}
	//arrow buttons on main menu
	if (button0Bouncer == HIGH) {

		if(DEBUG)Serial.print("Pushed button 0");
		handleButtonPushOnAlarm();

		menuState++;
		lcd->clearLcd();
		if (menuState > MAXMENU)
			menuState = MINMENU;
		button0Bouncer=LOW;
		return;
	}

	if (button1Bouncer== HIGH) {

		if(DEBUG)Serial.print("Pushed button 1");
		handleButtonPushOnAlarm();
		menuState--;
		lcd->clearLcd();
		if (menuState < MINMENU)
			menuState = MAXMENU;
		button1Bouncer=LOW;
		return;
	}

	if (button2Bouncer == HIGH) {

		if(DEBUG)Serial.print("Pushed button 2");
		handleButtonPushOnAlarm();
		if (menuState == ANCHOR) {
			menuState = ANCHORMINMENU;
			menuLevel++;
			anchor->drawAnchorBox(*lcd,getMenuState());
			anchor->drawAnchorScreen(*lcd,getMenuState());
		}else if (menuState == WIND) {
			menuState = WINDMINMENU;
			menuLevel++;
			wind->drawWindBox(*lcd, getMenuState());
			wind->drawWindScreen(*lcd, getMenuLevel());
		}else{
			button2Bouncer=LOW;
		}
		return;
	}

}

/*
 Check if the anchorPoint and Radius buttons are pressed
 */

void Menu::anchorMenu() {
	if (button0Bouncer == HIGH) {
		anchor->eraseAnchorBox(*lcd, getMenuState());
		menuState++;
		if (menuState > ANCHORMAXMENU)
			menuState = ANCHORMINMENU;
		anchor->drawAnchorBox(*lcd, getMenuState());
		anchor->drawAnchorScreen(*lcd, getMenuState());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		anchor->eraseAnchorBox(*lcd, getMenuState());
		menuState--;
		if (menuState < ANCHORMINMENU)
			menuState = ANCHORMAXMENU;
		anchor->drawAnchorBox(*lcd, getMenuState());
		anchor->drawAnchorScreen(*lcd, getMenuState());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		switch (menuState) {
		case ANCHORSTATE:
			anchor->setAnchorAlarmState(!anchor->getAnchorAlarmState());
			break;
		case ANCHORPOINT:
			anchor->setAnchorPoint();
			break;
		case ANCHORRADIUS:
			menuLevel++;
			break;
		case ANCHORRTN:
			lcd->clearLcd();
			menuLevel--;
			menuState = ANCHOR;

			break;

		}
		anchor->drawAnchorScreen(*lcd, getMenuState());
		button2Bouncer=LOW;
		return;
	}

}

/* Adjust the anchor alarm radius*/
void Menu::anchorRadiusMenu() {
	if (button0Bouncer == HIGH) {

		//add 5M to anchor radius
		anchor->setAnchorRadius(anchor->getAnchorRadius() + 5.0);
		anchor->drawAnchorScreen(*lcd, getMenuState());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		//subtract 5M from anchor radius
		anchor->setAnchorRadius(anchor->getAnchorRadius() - 5.0);
		anchor->drawAnchorScreen(*lcd, getMenuState());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		menuLevel--;
		anchor->drawAnchorScreen(*lcd,getMenuState());
		button2Bouncer=LOW;
		return;
	}

}

/*
 Check if the windAlarm and speed buttons are pressed
 */

void Menu::windMenu() {
	if (button0Bouncer == HIGH) {

		wind->eraseWindBox(*lcd, getMenuState());
		menuState++;
		if (menuState > WINDMAXMENU)
			menuState = WINDMINMENU;
		wind->drawWindBox(*lcd, getMenuState());
		wind->drawWindScreen(*lcd,getMenuLevel());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		wind->eraseWindBox(*lcd, getMenuState());
		menuState--;
		if (menuState < WINDMINMENU)
			menuState = WINDMAXMENU;
		wind->drawWindBox(*lcd, getMenuState());
		wind->drawWindScreen(*lcd,getMenuLevel());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		switch (menuState) {
		case WINDSTATE:
			wind->setWindAlarm(!wind->getWindAlarm());
			break;
		case WINDSPEED:
			menuLevel++;
			break;
		case WINDRTN:
			lcd->clearLcd();
			menuLevel--;
			menuState = WIND;
			break;

		}
		wind->drawWindScreen(*lcd, getMenuLevel());
		button2Bouncer=LOW;
		return;
	}

}

/* Adjust the wind alarm speed*/
void Menu::windSpeedMenu() {
	if (button0Bouncer == HIGH) {

		//add 5kts to windspeed
		wind->setWindAlarmSpeed(wind->getWindAlarmSpeed() + 5);
		wind->drawWindScreen(*lcd, getMenuLevel());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {
		//add 5kts to windspeed

		wind->setWindAlarmSpeed(wind->getWindAlarmSpeed() - 5);
		if (wind->getWindAlarmSpeed() < 0)
			wind->setWindAlarmSpeed(0);
		wind->drawWindScreen(*lcd, getMenuLevel());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {
		menuLevel--;

		wind->drawWindScreen(*lcd, getMenuLevel());
		button2Bouncer=LOW;
		return;
	}

}

/* handle the alarm if a button is pushed while alarming*/
void Menu::handleButtonPushOnAlarm() {
	if (alarm->alarmTriggered()) {
		//give 5 minutes respite from noise!
		alarm->setSnoozeAlarm(millis() + SNOOZE_TIME_MILLIS);
		if (alarm->isMobAlarmTriggered()) {
			//send cancel ? no, waypoint is cancelled
			//seatalk->cancelMOB();
		}
		if (alarm->isRadarAlarmTriggered()) {
			//send acknowledge alarm
			seatalk->radarAlarmOff();
		}
		if (alarm->isWindAlarmTriggered()) {
			//send acknowledge alarm
			wind->setWindAlarm(false);
		}
	}
}

int Menu::getMenuLevel() {
	return menuLevel;
}

int Menu::getMenuState()  {
	return menuState;
}

void Menu::setMenuLevel(int level) {
	this->menuLevel = level;
}

void Menu::setMenuState(int state) {
	this->menuState = state;
}


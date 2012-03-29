/*
 * Menu.cpp
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#include "Menu.h"



Menu::Menu(Lcd* lcd, FreeBoardModel* model) {
	this->model=model;


	this->button0Bouncer = LOW; //anchorPoint
	this->button1Bouncer = LOW; //radius
	this->button2Bouncer = LOW; //toggle alarm
	this->button3Bouncer = LOW; //remote alarm



	this->lastButtonCheck = 0;
	model->setMenuState(GPS); //default, show gps data
	model->setMenuLevel(0); //main menu
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
	switch (model->getMenuLevel()) {
	case 0:
		mainMenu();
		break;

	case 1:
		if (model->getMenuState() == ANCHOR || (model->getMenuState() >= ANCHORMINMENU
				&& model->getMenuState() <= ANCHORMAXMENU)) {
			if(DEBUG)Serial.print("Anchor menu");
			anchorMenu();
			return;
		}
		if (model->getMenuState() == WIND || (model->getMenuState() >= WINDMINMENU && model->getMenuState()
				<= WINDMAXMENU)) {
			windMenu();
			return;
		}
		model->setMenuState(MINMENU);
		model->setMenuLevel(0);

		break;

	case 2:
		if (model->getMenuState() == ANCHORRADIUS) {
			anchorRadiusMenu();
			return;
		}
		if (model->getMenuState() == WINDSPEED) {
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

		model->setMenuState(model->getMenuState()+1);
		lcd->clearLcd();
		if (model->getMenuState() > MAXMENU)
			model->setMenuState(MINMENU);
		button0Bouncer=LOW;
		return;
	}

	if (button1Bouncer== HIGH) {

		if(DEBUG)Serial.print("Pushed button 1");
		handleButtonPushOnAlarm();
		model->setMenuState(model->getMenuState()-1);
		lcd->clearLcd();
		if (model->getMenuState() < MINMENU)
			model->setMenuState(MAXMENU);
		button1Bouncer=LOW;
		return;
	}

	if (button2Bouncer == HIGH) {

		if(DEBUG)Serial.print("Pushed button 2");
		handleButtonPushOnAlarm();
		if (model->getMenuState() == ANCHOR) {
			model->setMenuState(ANCHORMINMENU);
			model->setMenuLevel(model->getMenuLevel()+1);
			lcd->drawAnchorBox(model->getMenuState());
			lcd->drawAnchorScreen(model->getMenuState());
		}else if (model->getMenuState() == WIND) {
			model->setMenuState(WINDMINMENU);
			model->setMenuLevel(model->getMenuLevel()+1);
			lcd->drawWindBox(model->getMenuState());
			lcd->drawWindScreen(model->getMenuLevel());
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
		lcd->eraseAnchorBox( model->getMenuState());
		model->setMenuState(model->getMenuState()+1);
		if (model->getMenuState() > ANCHORMAXMENU)
			model->setMenuState(ANCHORMINMENU);
		lcd->drawAnchorBox( model->getMenuState());
		lcd->drawAnchorScreen( model->getMenuState());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		lcd->eraseAnchorBox(model->getMenuState());
		model->setMenuState(model->getMenuState()-1);
		if (model->getMenuState() < ANCHORMINMENU)
			model->setMenuState(ANCHORMAXMENU);
		lcd->drawAnchorBox( model->getMenuState());
		lcd->drawAnchorScreen( model->getMenuState());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		switch (model->getMenuState()) {
		case ANCHORSTATE:
			model->setAnchorAlarmOn(!model->isAnchorAlarmOn());
			break;
		case ANCHORPOINT:
			model->setAnchorLat(model->getGpsLatitude());
			model->setAnchorLon(model->getGpsLongitude());
			break;
		case ANCHORRADIUS:
			model->setMenuLevel(model->getMenuLevel()+1);
			break;
		case ANCHORRTN:
			lcd->clearLcd();
			model->setMenuLevel(model->getMenuLevel()-1);
			model->setMenuState(ANCHOR);

			break;

		}
		lcd->drawAnchorScreen( model->getMenuState());
		button2Bouncer=LOW;
		return;
	}

}

/* Adjust the anchor alarm radius*/
void Menu::anchorRadiusMenu() {
	if (button0Bouncer == HIGH) {

		//add 5M to anchor radius
		model->setAnchorRadius(model->getAnchorRadius() + 5.0);
		lcd->drawAnchorScreen( model->getMenuState());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		//subtract 5M from anchor radius
		model->setAnchorRadius(model->getAnchorRadius() - 5.0);
		lcd->drawAnchorScreen( model->getMenuState());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		model->setMenuLevel(model->getMenuLevel()-1);
		lcd->drawAnchorScreen(model->getMenuState());
		button2Bouncer=LOW;
		return;
	}

}

/*
 Check if the windAlarm and speed buttons are pressed
 */

void Menu::windMenu() {
	if (button0Bouncer == HIGH) {

		lcd->eraseWindBox( model->getMenuState());
		model->setMenuState(model->getMenuState()+1);
		if (model->getMenuState() > WINDMAXMENU)
			model->setMenuState(WINDMINMENU);
		lcd->drawWindBox( model->getMenuState());
		lcd->drawWindScreen(model->getMenuLevel());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {

		lcd->eraseWindBox( model->getMenuState());
		model->setMenuState(model->getMenuState()-1);
		if (model->getMenuState() < WINDMINMENU)
			model->setMenuState(WINDMAXMENU);
		lcd->drawWindBox( model->getMenuState());
		lcd->drawWindScreen(model->getMenuLevel());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {

		switch (model->getMenuState()) {
		case WINDSTATE:
			model->setWindAlarmOn(!model->isWindAlarmOn());
			break;
		case WINDSPEED:
			model->setMenuLevel(model->getMenuLevel()+1);
			break;
		case WINDRTN:
			lcd->clearLcd();
			model->setMenuLevel(model->getMenuLevel()-1);
			model->setMenuState(WIND);
			break;

		}
		lcd->drawWindScreen( model->getMenuLevel());
		button2Bouncer=LOW;
		return;
	}

}

/* Adjust the wind alarm speed*/
void Menu::windSpeedMenu() {
	if (button0Bouncer == HIGH) {

		//add 5kts to windspeed
		model->setWindAlarmSpeed(model->getWindAlarmSpeed() + 5);
		lcd->drawWindScreen( model->getMenuLevel());
		button0Bouncer=LOW;
		return;
	}
	if (button1Bouncer == HIGH) {
		//add 5kts to windspeed

		model->setWindAlarmSpeed(model->getWindAlarmSpeed() - 5);
		if (model->getWindAlarmSpeed() < 0)
			model->setWindAlarmSpeed(0);
		lcd->drawWindScreen( model->getMenuLevel());
		button1Bouncer=LOW;
		return;
	}
	if (button2Bouncer == HIGH) {
		model->setMenuLevel(model->getMenuLevel()-1);

		lcd->drawWindScreen( model->getMenuLevel());
		button2Bouncer=LOW;
		return;
	}

}

/* handle the alarm if a button is pushed while alarming*/
void Menu::handleButtonPushOnAlarm() {
	if (model->isAlarmTriggered()) {
		//give 5 minutes respite from noise!
		model->setAlarmSnooze(millis() + SNOOZE_TIME_MILLIS);
		model->setRadarAlarmTriggered(false);
		model->setMobAlarmTriggered(false);
		model->setWindAlarmTriggered(false);
		model->setAnchorAlarmTriggered(false);
		model->setGpsAlarmTriggered(false);

	}
}



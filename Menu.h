/*
 * Menu.h
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"

#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"
#include "Lcd.h"
#include "Alarm.h"
#include "Anchor.h"
#include "Wind.h"
#include "Seatalk.h"

class Menu {
public:
	Menu(Lcd* lcd,  FreeBoardModel* model);
	virtual ~Menu();

	/*Check if buttons are pressed initiating a menu
	 */
	void setButton(uint8_t);
	void checkButtons();
	void mainMenu();
	/*
	 Check if the anchorPoint and Radius buttons are pressed
	 */

	void anchorMenu();
	/* Adjust the anchor alarm radius*/
	void anchorRadiusMenu();
	/*
	 Check if the windAlarm and speed buttons are pressed
	 */
	void windMenu();
	/* Adjust the wind alarm speed*/
	void windSpeedMenu();
	void handleButtonPushOnAlarm();


private:
	static const unsigned long SNOOZE_TIME_MILLIS = 300000;
	static const unsigned long BUTTON_DEBOUNCE_TIME=50ul;
	//flags to hold button state
	volatile bool button0Bouncer; //anchorPoint
	volatile bool button1Bouncer; //radius
	volatile bool button2Bouncer; //toggle alarm
	volatile bool button3Bouncer; //remote alarm
	volatile int button0Pin;
		volatile int lastButtonCheck;
		//volatile int menuState; //default, show gps data
		//volatile int menuLevel;
	Lcd* lcd;

	FreeBoardModel* model;

};

#endif /* MENU_H_ */

/*
 * Lcd.h
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#ifndef LCD_H_
#define LCD_H_

#include "Arduino.h"

#include <SoftwareSerial/SoftwareSerial.h>
#include <PString/PString.h>
#include "Gps.h"
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class Lcd: public SoftwareSerial {
public:

	Lcd(FreeBoardModel* model, uint8_t receivePin, uint8_t transmitPin,
			bool inverse_logic = false) :model(model),lastScreenUpdate(0),
			SoftwareSerial(receivePin, transmitPin, inverse_logic = false){

	};

	~Lcd();

	/* Routines to write to lcd*/

	void setBacklight(int dutyCycle);
	void clearLcd();
	void setupLcd();
	/* Text offsets measured in pixels from the bottom left. The x,y point defines the upper left of the character cell*/
	void setCursor(int x, int y);
	/*Draw a box from lower lx,ly to upper ux,uy*/
	void drawBox(int lx, int ly, int ux, int uy);
	/*Erase a box from lower lx,ly to upper ux,uy*/
	void eraseBox(int lx, int ly, int ux, int uy);
	/*Draw a line from lower lx,ly to upper ux,uy*/
	void drawLine(int lx, int ly, int ux, int uy);
	/*Erase a line from lower lx,ly to upper ux,uy*/
	void eraseLine(int lx, int ly, int ux, int uy);
	void writeButtonLabels(char* b0, char* b1, char* b2);
	unsigned long getLastScreenUpdate();

	void setLastScreenUpdate(unsigned long lastScreenUpdate);

	//anchor
	unsigned long lastLcdUpdate;
	void drawAnchorScreen(int menuState);
	void showAnchorAlarmData(int menuLevel, int menuState);
	void drawAnchorBox(int menuState);
	void eraseAnchorBox(int menuState);

	//wind
	void drawWindScreen( int menuLevel);
	void drawWindBox( int menuState);
	void eraseWindBox( int menuState);
	void showWindData(int menuLevel, int menuState);

	//gps
	void showGPSData(int menuState);

private:
	unsigned long lastScreenUpdate;
	FreeBoardModel* model;
};


#endif /* LCD_H_ */

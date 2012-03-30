/*
 * Lcd.h
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#ifndef LCD_H_
#define LCD_H_

#include "Arduino.h"

#include <AltSoftSerial/AltSoftSerial.h>
#include <PString/PString.h>
#include "Gps.h"
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class Lcd: public AltSoftSerial {
public:

	Lcd(FreeBoardModel* model) :model(model),lastScreenUpdate(0){

	};

	~Lcd();

	/* Routines to write to lcd*/

	void setBacklight(int dutyCycle);
	void clearLcd();
	void setupLcd();
	void showData();
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
	void writeButtonLabels(String b0, String b1, String b2);

	//anchor
	unsigned long lastLcdUpdate;
	void drawAnchorScreen();
	void showAnchorAlarmData();
	void drawAnchorBox();
	void eraseAnchorBox();

	//wind
	void drawWindScreen( );
	void drawWindBox( );
	void eraseWindBox( );
	void showWindData();

	//gps
	void showGPSData();

private:
	unsigned long lastScreenUpdate;
	FreeBoardModel* model;
};


#endif /* LCD_H_ */

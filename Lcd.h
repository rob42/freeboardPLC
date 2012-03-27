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
#include "FreeBoardConstants.h"

class Lcd: public SoftwareSerial {
public:

	Lcd(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic=false):SoftwareSerial(receivePin,transmitPin,inverse_logic=false) {};
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

private:
	unsigned long lastScreenUpdate;
};

#endif /* LCD_H_ */

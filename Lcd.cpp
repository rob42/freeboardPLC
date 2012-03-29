/*
 * Lcd.cpp
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#include "Lcd.h"
char lcdBuffer[40];
PString lcdStr(lcdBuffer, sizeof(lcdBuffer));



Lcd:: ~Lcd(){

}

unsigned long Lcd::getLastScreenUpdate() {
	return lastScreenUpdate;
}

void Lcd::setLastScreenUpdate(unsigned long lastScreenUpdate) {
	this->lastScreenUpdate = lastScreenUpdate;
}

/* Routines to write to lcd*/

void Lcd::setBacklight(int dutyCycle) {
	write(0x7C);
	write(0x02);
	write(dutyCycle);
	write((byte)0x00);
	flush();
	delay(50);
}



void Lcd::clearLcd() {
	write(0x7C);
	write((byte)0x00);
	//writeButtonLabels();
	setCursor(0, 0);
	lastScreenUpdate=0;
	flush();
	delay(50);
}

void Lcd::setupLcd() {
	lastScreenUpdate=0;
	Serial.println("Lcd reset..");
	begin(115200);
	print("x");
	flush();

	delay(2000);
	Serial.println("Lcd set baud 19200..");
	write(0x7C);
	write(0x07);
	write(0x33);//19200
	//write(0x34);//38400
	flush();
	delay(1000);
	begin(19200);
	//set reverse off
	// print(0x7C);
	// print(0x12);
	clearLcd();
	setBacklight(0);

	//run demo
	//print(0x7C,BYTE); // cmd
	//print(0x04,BYTE); // demo

	setCursor(20, 50);
	print("FreeBoard");
	setCursor(10, 40);
	print("Copyright RTH 2012");
	setCursor(10, 30);
	print("Init-please wait..");
	delay(2000);

}

/* Text offsets measured in pixels from the bottom left. The x,y point defines the upper left of the character cell*/
void Lcd::setCursor(int x, int y) {
	//delay(5);
	//x_offset
	write(0x7C);
	write(0x18);
	write(x);
	//write((byte)0x00);
	flush();
	delay(100);
	//y_offset
	write(0x7C);
	write(0x19);
	write(y);
	//write((byte)0x00);
	flush();
	delay(100);


}

/*Draw a box from lower lx,ly to upper ux,uy*/
void Lcd::drawBox(int lx, int ly, int ux, int uy) {
	write(0x7C);
	write(0x0F);
	write(lx);
	write(ly);
	write(ux);
	write(uy);
	flush();
	delay(50);
}
/*Erase a box from lower lx,ly to upper ux,uy*/
void Lcd::eraseBox(int lx, int ly, int ux, int uy) {
	write(0x7C);
	write(0x05);
	write(lx);
	write(ly);
	write(ux);
	write(uy);
	flush();
	delay(50);
}

/*Draw a line from lower lx,ly to upper ux,uy*/
void Lcd::drawLine(int lx, int ly, int ux, int uy) {
	write(0x7C);
	write(0x0C);
	write(lx);
	write(ly);
	write(ux);
	write(uy);
	flush();
	delay(50);
}
/*Erase a line from lower lx,ly to upper ux,uy*/
void Lcd::eraseLine(int lx, int ly, int ux, int uy) {
	write(0x7C);
	write(0x05);
	write(lx);
	write(ly);
	write(ux);
	write(uy);
	flush();
	delay(50);
}

void Lcd::writeButtonLabels(char* b0, char* b1, char* b2) {
	 setCursor(0,10);
	 print(b0);
	 setCursor(50,10);
	 print(b1);
	 setCursor(90,10);
	 print(b2);
	 flush();
	 delay(50);

}

void Lcd::drawAnchorBox( int menuState) {
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		//draw the correct box
		if (menuState == ANCHORSTATE)
			drawBox(0, 51, 127, 62);
		if (menuState == ANCHORPOINT)
			drawBox(0, 31, 127, 52);
		if (menuState == ANCHORRADIUS)
			drawBox(0, 21, 127, 32);
		if (menuState == ANCHORRTN)
			drawBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}

void Lcd::eraseAnchorBox( int menuState) {
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		//draw the correct box
		if (menuState == ANCHORSTATE)
			eraseBox(0, 51, 127, 62);
		if (menuState == ANCHORPOINT)
			eraseBox(0, 31, 127, 52);
		if (menuState == ANCHORRADIUS)
			eraseBox(0, 21, 127, 32);
		if (menuState == ANCHORRTN)
			eraseBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}


void Lcd::drawAnchorScreen( int menuState) {
	setCursor(2, 60);
	if (model->isAnchorAlarmOn()) {
		print("Anchor alarm ON  ");
	} else {
		print("Anchor alarm OFF ");
	}
	setCursor(2, 50);
	print("Lat: ");

	print(Gps::getLatString(model->getAnchorLat(), 4, 10,lcdStr));
	setCursor(2, 40);
	print("Lon: ");

	print(Gps::getLonString(model->getAnchorLon(), 4, 10,lcdStr));
	setCursor(2, 30);
	print("Radius : ");
	print((int) model->getAnchorRadius());
	print("M     ");
	setCursor(2, 20);
	if (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU) {
		print("Back to prev menu ");
		//writeButtonLabels(SET, NEXT, PREV);
	} else {
		print("Now: ");
		print((int) model->getAnchorDistance());
		// print("M, max: ");
		//print((int)maxAnchorDistance);
		//print("M     ");
		print("M,Box:");
		float w = (model->getAnchorE() - model->getAnchorW()) / LLMTRS;
		float h = (model->getAnchorN() - model->getAnchorS()) / LLMTRS;
		print((int) w);
		print("x");
		print((int) h);
		writeButtonLabels(EDIT, PGDN, PGUP);

	}
	lastLcdUpdate= millis();
}

/*display anchor data on screen*/
void Lcd::showAnchorAlarmData( int menuLevel, int menuState) {
	if (menuLevel == 0 && (menuState == ANCHOR || (menuState >= ANCHORMINMENU && menuState <= ANCHORMAXMENU))
			&& model->getGpsStatus()) {
		if (millis() - lastLcdUpdate > 1000) {
			//update
			lastLcdUpdate= millis();
			drawAnchorScreen( menuState);
		}
	}
}


/*
 Write wind data to screen
 */
void Lcd::showWindData( int menuLevel, int menuState) {
	if (menuState == WIND && menuLevel == 0) {
		//how long
		if (millis() - lastLcdUpdate > 1500) {
			drawWindScreen( menuLevel);
			lastLcdUpdate= millis();
		}
	}
}
void Lcd::drawWindScreen( int menuLevel) {
	//update screen
	setCursor(2, 60);
	print("WIND: ");
	print((int) model->getWindAverage());
	print("KNTS    ");
	setCursor(2, 50);
	print("Max Gust: ");
	print((int) model->getWindMax());
	print("KNTS   ");
	//alarms
	setCursor(2, 40);
	print("Wind Alarm: ");
	if (model->isWindAlarmOn()) {
		print("ON    ");
	} else {
		print("OFF    ");
	}
	setCursor(2, 30);
	print("Alarm Speed: ");
	print((int) model->getWindAlarmSpeed());
	print("KNTS   ");
	if (menuLevel >= 1) {
		setCursor(2, 20);
		print("Return to prev menu");
		writeButtonLabels(SET, NEXT, PREV);
	} else {
		writeButtonLabels(EDIT, PGDN, PGUP);
	}
	lastLcdUpdate= millis();
	if (DEBUG) {
		Serial.print("Wind (KNTS): ");
		Serial.print(model->getWindAverage(), DEC);
		Serial.print(", gust (KNTS): ");
		Serial.println(model->getWindMax(), DEC);
	}
}

void Lcd::drawWindBox(int menuState) {
	if (menuState >= WINDMINMENU && menuState <= WINDMAXMENU) {
		//draw the correct box
		if (menuState == WINDSTATE)
			drawBox(0, 31, 127, 42);
		if (menuState == WINDSPEED)
			drawBox(0, 21, 127, 32);
		if (menuState == WINDRTN)
			drawBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}

void Lcd::eraseWindBox( int menuState) {
	if (menuState >= WINDMINMENU && menuState <= WINDMAXMENU) {
		//draw the correct box
		if (menuState == WINDSTATE)
			eraseBox(0, 31, 127, 42);
		if (menuState == WINDSPEED)
			eraseBox(0, 21, 127, 32);
		if (menuState == WINDRTN)
			eraseBox(0, 11, 127, 22);
		lastLcdUpdate= millis();
	}
}

/* NMEA GPS routines
 */
void Lcd::showGPSData( int menuState) {
	if (menuState != GPS){
		return;
	}
	//check if active
	if (millis() - lastLcdUpdate > 2000) {
		//update
		lastLcdUpdate= millis();

		//check we still have a fix
		if (millis() - model->getGpsLastFix() > 10000) {
			//show bad GPS fix
			setCursor(2, 20);
			print("WARNING: NO GPS FIX! ");
			delay(50);
			return;
		}

		if ( model->getGpsLastFix()>0 ) {

			//clearLcd();

			setCursor(2, 60);
			print("Lat: ");

			print(Gps::getLatString(model->getGpsLatitude(), 4, 10, lcdStr));
			delay(50);

			setCursor(2, 50);
			print("Lon: ");
			print(Gps::getLonString(model->getGpsLongitude(), 4, 10,lcdStr));
			delay(50);

			setCursor(2, 40);
			//TODO: what if its not knots?
			print("Knots: ");
			print(model->getGpsSpeed(),1);
			delay(50);
			//print("  ");

			setCursor(2, 30);
			print("Heading: ");
			print(model->getGpsCourse(),0);
			print("T   ");
			delay(50);
			setCursor(2, 20);
			print("                    ");
			writeButtonLabels(EMPTY,PGDN,PGUP);
			delay(50);
		}

	}
}

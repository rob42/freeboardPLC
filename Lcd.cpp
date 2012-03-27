/*
 * Lcd.cpp
 *
 *  Created on: 18/12/2010
 *      Author: robert
 */

#include "Lcd.h"

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

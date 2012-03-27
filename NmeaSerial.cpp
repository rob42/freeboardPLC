/*
 * NmeaSerial.cpp
 *
 *  Created on: 23/12/2010
 *      Author: robert
 */

#include "NmeaSerial.h"


NmeaSerial::~NmeaSerial(){

}
void NmeaSerial::begin(long speed){
	//initialise the nmea output
		//pinMode(nmeaRxPin, INPUT);
		//pinMode(nmeaTxPin, OUTPUT);
		SoftwareSerial::begin(speed);
}

void NmeaSerial::printNmea(char* sentence){
	//sentence can be up to 80 chars
	  //make it as perfect as possible, as the Raymarine C70 is very touchy
	 // noInterrupts();
	  println(sentence);
	 // interrupts();
	  //if(DEBUG)Serial.println(sentence);
}

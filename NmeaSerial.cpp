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

void NmeaSerial::printWindNmea() {
		//Assemble a sentence of the various parts so that we can calculate the proper checksum

		PString str(windSentence, sizeof(windSentence));
		str.print("$IIMWV,");
		str.print(model->getWindApparentDir());
		str.print(".0,R,");
		str.print(model->getWindAverage());
		str.print(".0,N,A*");
		//calculate the checksum

		cs = 0; //clear any old checksum
		for (unsigned int n = 1; n < strlen(windSentence) - 1; n++) {
			cs ^= windSentence[n]; //calculates the checksum
		}
		str.print(cs, HEX); // Assemble the final message and send it out the serial port
		if(DEBUG)Serial.println(windSentence);
		printNmea(windSentence);

}

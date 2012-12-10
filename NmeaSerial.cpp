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
		AltSoftSerial::begin(speed);
}

void NmeaSerial::printNmea(char* sentence){
	//sentence can be up to 80 chars
	  //make it as perfect as possible, as the Raymarine C70 is very touchy
	 // noInterrupts();
	  println(sentence);
	 // interrupts();
	  //if(DEBUG)Serial.println(sentence);
}

/*=== MWV - Wind Speed and Angle ===
*
* ------------------------------------------------------------------------------
*        1   2 3   4 5
*         |   | |   | |
*  $--MWV,x.x,a,x.x,a*hh<CR><LF>
* ------------------------------------------------------------------------------
*
* Field Number:
*
* 1. Wind Angle, 0 to 360 degrees
* 2. Reference, R = Relative, T = True
* 3. Wind Speed
* 4. Wind Speed Units, K/M/N
* 5. Status, A = Data Valid
* 6. Checksum
 *
 */
void NmeaSerial::printWindNmea() {
		//Assemble a sentence of the various parts so that we can calculate the proper checksum

		PString str(windSentence, sizeof(windSentence));
		str.print("$IIMWV,");
		str.print(model->getWindApparentDir());
		str.print(".0,R,");
		str.print(model->getWindAverage());
		str.print(",N,A*");
		//calculate the checksum

		cs = 0; //clear any old checksum
		for (unsigned int n = 1; n < strlen(windSentence) - 1; n++) {
			cs ^= windSentence[n]; //calculates the checksum
		}
		str.print(cs, HEX); // Assemble the final message and send it out the serial port
		if(DEBUG)Serial.println(windSentence);
		printNmea(windSentence);

}

/*
 * Copyright 2010,2011,2012,2013 Robert Huitema robert@42.co.nz
 *
 * This file is part of FreeBoard. (http://www.42.co.nz/freeboard)
 *
 *  FreeBoard is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  FreeBoard is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with FreeBoard.  If not, see <http://www.gnu.org/licenses/>.
 */
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
		Serial.println(windSentence);
		printNmea(windSentence);

}

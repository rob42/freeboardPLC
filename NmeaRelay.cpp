/*
  nmea.cpp - NMEA 0183 sentence decoding library for Wiring & Arduino
  Copyright (c) 2008 Maarten Lamers, The Netherlands.
  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "NmeaRelay.h"

//
// constructor method
//

NMEARelay::NMEARelay(int connect)
{
	// private properties
	n = 0;
	_state = 0;

}


//
// public methods
//

int NMEARelay::decode(char c) {
  // avoid runaway sentences (>99 chars or >20 terms) and terms (>14 chars)
  if ((n >= 98)) { _state = 0; }
  // LF and CR always reset parser
  if ((c == 0x0A) || (c == 0x0D)) { _state = 0; }
  // '$' always starts a new sentence
  if (c == '$') {
	  //null out sentence
	  memset(_sentence, 0, sizeof(_sentence));
    _sentence[0] = c;
    n = 1;
    _state = 1;
    return 0;
  }
  // parse other chars according to parser state
  switch(_state) {
  case 0:
    // waiting for '$', do nothing
    break;
  case 1:
  	// decode chars after '$' and before '*' found
    // add received char to sentence
    _sentence[n++] = c;
    switch (c) {
    case '*':
    	// '*' delimits term and precedes checksum term
    	_sentence[n++] = c;
      _state++;
      break;
    default:
    	// all other chars between '$' and '*' are part of a term
    	_sentence[n++] = c;
      break;
    }
    break;
  case 2:
  	// first char following '*' is checksum MSB
    _sentence[n++] = c;
    _state++;
    break;
  case 3:
  	// second char after '*' completes the checksum (LSB)
    _sentence[n++] = c;
    _sentence[n++] = '\0'; //null
    //copy to f_sentence
    strcpy(f_sentence, _sentence);
     return 1;
    break;
  default:
    _state = 0;
    break;
  }
  return 0;
}



char* NMEARelay::sentence() {
	// returns last received full sentence as null terminated string
  return f_sentence;
}



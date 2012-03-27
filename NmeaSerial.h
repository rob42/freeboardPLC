/*
 * NmeaSerial.h
 *
 *  Created on: 23/12/2010
 *      Author: robert
 */

#ifndef NMEASERIAL_H_
#define NMEASERIAL_H_

#include "Arduino.h"

#include <SoftwareSerial/SoftwareSerial.h>
#include "FreeBoardConstants.h"

class NmeaSerial: SoftwareSerial {
public:
	NmeaSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false):SoftwareSerial(receivePin, transmitPin, inverse_logic = false){};
	virtual ~NmeaSerial();
	void printNmea(char* sentence);
	void begin(long speed);
};


#endif /* NMEASERIAL_H_ */

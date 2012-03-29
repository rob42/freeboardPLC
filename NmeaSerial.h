/*
 * NmeaSerial.h
 *
 *  Created on: 23/12/2010
 *      Author: robert
 */

#ifndef NMEASERIAL_H_
#define NMEASERIAL_H_

#include "Arduino.h"
#include <PString/PString.h>
#include <SoftwareSerial/SoftwareSerial.h>
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class NmeaSerial: SoftwareSerial {
public:
	NmeaSerial(FreeBoardModel* model, uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false):model(model),SoftwareSerial(receivePin, transmitPin, inverse_logic = false){};
	virtual ~NmeaSerial();
	void printNmea(char* sentence);
	void printWindNmea();
	void begin(long speed);
private:
	FreeBoardModel* model;
	char windSentence [30];
	int cs;
};


#endif /* NMEASERIAL_H_ */

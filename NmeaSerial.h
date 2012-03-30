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
#include <AltSoftSerial/AltSoftSerial.h>
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class NmeaSerial: AltSoftSerial {
public:
	NmeaSerial(FreeBoardModel* model):model(model){};
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

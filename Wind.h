/*
 * Wind.h
 *
 *  Created on: 17/12/2010
 *      Author: robert
 */

#ifndef WIND_H_
#define WIND_H_

#include "Arduino.h"

#include <PString/PString.h>

#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"
#include <AverageList/AverageList.h>

extern void saveWindAlarmState(bool windAlarmOn);
extern void saveWindAlarmSpeed(int windAlarmSpeed);
extern int readWindAlarmSpeed();
extern bool readWindAlarmState();
class Wind {

public:
	Wind( FreeBoardModel* model);

	void calcWindData();
	void readWindDataSpeed();
	void readWindDataDir();
private:

	FreeBoardModel* model;





};
#endif /* WIND_H_ */

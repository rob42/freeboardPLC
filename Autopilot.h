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
/* Autopilot code here*/
/*Start with something very simple, a target heading. We
try to keep the boat on this target heading.
*/

/*Arguments:

    * *Input : Link (pointer) with the double that will be the PID's Input
    * *Ouput : Link to the Output
    * *SetPoint : Link to the Set Point
    * P_Param : Proportional Parameter. MUST BE NON-ZERO.
    * I_Param : Integral Parameter. Must be non-negative.
    * D_Param : Derivative Parameter. Must be non-negative. 
    */
#ifndef AUTOPILOT_H_
#define AUTOPILOT_H_

#include "Arduino.h"

#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"
#include "Gps.h"
#include <PID_v1.h>
#include <Kangaroo.h>
#include <MultiSerial.h>
/**
 * Parameters and what they do (sort of)
 * P_Param:  the bigger the number the harder the controller pushes.
 * I_Param:  the SMALLER the number (except for 0, which turns it off,)  the more quickly the controller reacts to load changes, but the greater the risk of oscillations.
 * D_Param: the bigger the number  the more the controller dampens oscillations (to the point where performance can be hindered)

 * These are rules of thumb and aren't exactly what's going on inside the controller, but they should help when deciding which number to change and in which direction.

 * And lastly, if the second you enable the controller the output goes in the wrong direction and pegs at 0 or 100%, you've got the sign of the P_Param wrong.
 * That is, if you've got a 3 in there, make it -3.
 */
#define P_Param  0.45 //proportional param
#define I_Param  0.08 //manual response time (in Seconds)/4. How fast it reacts.
#define D_Param  0.52 //aggressive factor. Dampen oscillations. Change sign to invert command direction

class Autopilot {
public:
	Autopilot(FreeBoardModel* model);
	~Autopilot();
	void calcAutoPilot();
	double getRudderCorrection();
	void process(char* message);
	void autopilotEvent();
	void init();

private:

	double autopilotTargetHeading; //Setpoint
	double autopilotCurrentHeading; //Input
	double autopilotRudderCommand; //Output
	double lastRudderCommand;
	bool	lastDirection; //last rudder movement direction
	char inputAutopilotArray[10];
	int inputSerialPos;
	int inputAutopilotPos;

	FreeBoardModel* model;
	PID headingPid;
};

#endif

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

 TODO: Add dynamic tuning - make PID more agressive when:
 further from target
 hard on the wind
 approaching dead downwind (ramp gain 20deg either side)
 Could use formula like: 20 -(abs(((apparentWind+180)%360)-currentHeading) = gain factor

 Add deadband about target to reduce tiny movements and power consumption - maybe low pass filter to avoid small variations
 Add allowance for drive slop +/- deg, always add when changing rudder direction

 Auto-tune...

 */

/*Arguments:

 * *Input : Link (pointer) with the double that will be the PID's Input
 * *Ouput : Link to the Output
 * *SetPoint : Link to the Set Point
 * P_Param : Proportional Parameter. MUST BE NON-ZERO.
 * I_Param : Integral Parameter. Must be non-negative.
 * D_Param : Derivative Parameter. Must be non-negative.

 * Parameters and what they do (sort of)
 * P_Param:  the bigger the number the harder the controller pushes.
 * I_Param:  the SMALLER the number (except for 0, which turns it off,)  the more quickly the controller reacts to load changes, but the greater the risk of oscillations.
 * D_Param: the bigger the number  the more the controller dampens oscillations (to the point where performance can be hindered)

 * These are rules of thumb and aren't exactly what's going on inside the controller, but they should help when deciding which number to change and in which direction.

 * And lastly, if the second you enable the controller the output goes in the wrong direction and pegs at 0 or 100%, you've got the sign of the P_Param wrong.
 * That is, if you've got a 3 in there, make it -3.
 */
#include "Autopilot.h"

Autopilot::Autopilot(FreeBoardModel* model) {
	this->model = model;
	autopilotTargetHeading = model->getAutopilotTargetHeading() + 720;
	autopilotCurrentHeading = autopilotTargetHeading + model->getAutopilotOffCourse();
	autopilotRudderCommand = model->getAutopilotRudderCommand();
	lastDirection=true;
	this->headingPid = PID(&autopilotCurrentHeading, &autopilotRudderCommand, &autopilotTargetHeading, P_Param, I_Param, D_Param, REVERSE);

	headingPid.SetOutputLimits(0.0, 66.0); //output limits
	headingPid.SetSampleTime(100);

}
Autopilot::~Autopilot() {

}

void Autopilot::calcAutoPilot() {
	//we dont do this if the autopilot is MANUAL
	if (model->isAutopilotOn()) {
		//does nothing if its already on, inits if off
		headingPid.SetMode(AUTOMATIC);
		autopilotTargetHeading = model->getAutopilotTargetHeading() + 720;
		autopilotCurrentHeading = autopilotTargetHeading + model->getAutopilotOffCourse();
		headingPid.Compute();
		//check deadzone
		if (abs(autopilotRudderCommand - lastRudderCommand) > model->getAutopilotDeadZone()) {
			//then we move the rudder.
			//is it changing movement direction, we need to compensate for slack
			if (lastDirection && autopilotRudderCommand > lastRudderCommand) {
				//same direction to stbd, no slack
				lastRudderCommand = autopilotRudderCommand;
			}else if (lastDirection && autopilotRudderCommand < lastRudderCommand) {
				//changed direction to port, subtract slack
				lastDirection = false;
				lastRudderCommand = autopilotRudderCommand - model->getAutopilotSlack();
			}else if (!lastDirection && autopilotRudderCommand < lastRudderCommand) {
				//same direction to port
				lastRudderCommand = autopilotRudderCommand;
			}else if (!lastDirection && autopilotRudderCommand > lastRudderCommand) {
				//changed direction to stbd, add slack
				lastDirection = true;
				lastRudderCommand = autopilotRudderCommand + model->getAutopilotSlack();
			}
			//constrain(lastRudderCommand,0.0,66.0);
			//update model
			model->setAutopilotRudderCommand(lastRudderCommand);
		}

	} else {
		headingPid.SetMode(MANUAL);
	}
}


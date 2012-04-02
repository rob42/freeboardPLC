/* Autopilot code here*/
/*Start with something very simple, a target heading. We
 try to keep the boat on this target heading.

 TODO: Add dynamic tuning - make PID more agressive when:
 further from target
 hard on the wind
 approaching dead downwind (ramp gain 20deg either side)
 Could use formula like: 20 -(abs(((apparentWind+180)%360)-currentHeading) = gain factor

 Add hysterisis about target to reduce tiny movements and power consumption

 Auto-tune...

 */

/*Arguments:

 * *Input : Link (pointer) with the double that will be the PID's Input
 * *Ouput : Link to the Output
 * *SetPoint : Link to the Set Point
 * P_Param : Proportional Parameter. MUST BE NON-ZERO.
 * I_Param : Integral Parameter. Must be non-negative.
 * D_Param : Derivative Parameter. Must be non-negative.
 */
#include "Autopilot.h"

Autopilot::Autopilot(FreeBoardModel* model) {
	this->model = model;
	autopilotTargetHeading=model->getAutopilotTargetHeading()+720;
	autopilotCurrentHeading = autopilotTargetHeading + model->getAutopilotOffCourse();
	autopilotRudderCommand=model->getAutopilotRudderCommand();
	this->headingPid = PID(&autopilotCurrentHeading, &autopilotRudderCommand, &autopilotTargetHeading, P_Param, I_Param, D_Param, DIRECT);

	headingPid.SetOutputLimits(0,66); //output limits
	headingPid.SetSampleTime(100);

}
Autopilot::~Autopilot() {

}



void Autopilot::calcAutoPilot() {
	//we dont do this if the autopilot is MANUAL
	if (model->isAutopilotOn()){
		//does nothing if its already on, inits if off
		headingPid.SetMode(AUTOMATIC);
		autopilotTargetHeading=model->getAutopilotTargetHeading()+720;
		autopilotCurrentHeading = autopilotTargetHeading + model->getAutopilotOffCourse();
		headingPid.Compute();
		if (DEBUG ) {
					Serial.print("Target deg = ");
					Serial.print(model->getAutopilotTargetHeading());
					Serial.print("Heading deg = ");
					Serial.print(model->getAutopilotCurrentHeading());
					Serial.print(", Rudder = ");
					Serial.println(model->getAutopilotRudderCommand());
					model->setAutopilotCurrentHeading(model->getAutopilotCurrentHeading()+(0.1*(model->getAutopilotRudderCommand()-33)));
				}
		model->setAutopilotRudderCommand(autopilotRudderCommand);

	}else{
		headingPid.SetMode(MANUAL);
	}
}


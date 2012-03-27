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

double currentHeading = 0;
double targetHeading; //Setpoint
double actualHeading; //Input
double rudderCommand; //Output
PID headingPid(&actualHeading, &rudderCommand, &targetHeading, P_Param, I_Param,
		D_Param, DIRECT);

Autopilot::Autopilot(Gps* gps) {
	this->gps = gps;
	targetHeading = 0; //Setpoint
	actualHeading = 0; //Input
	rudderCommand = 0; //Output
	//headingPid.SetOutputLimits(0,66); //output limits
	headingPid.SetSampleTime(100);

}
Autopilot::~Autopilot() {

}

bool Autopilot::isEnabled(){
	return headingPid.GetMode();
}

void Autopilot::enableAutoPilot() {
	//initialize the variables we're linked to
	targetHeading = getCurrentHeading();
	actualHeading = getCurrentHeading();

	//turn the PID on
	headingPid.SetMode(AUTOMATIC);
}

void Autopilot::disableAutoPilot() {
	headingPid.SetMode(MANUAL);
}

/*
 * Returns the current heading 0-360 deg (assume T?)
 */
double Autopilot::getCurrentHeading() {
	return currentHeading; //gps->getRmcCourse();
}

void Autopilot::setCurrentHeading(double heading) {
	currentHeading = heading; //gps->getRmcCourse();
}

void Autopilot::setTargetHeading(double target) {
	targetHeading = target;
}
double Autopilot::getTargetHeading() {
	return targetHeading;
}

double Autopilot::getRudderCorrection() {
	//send out -33 to 33 as deg rudder correction
	return (rudderCommand - 128) / 6;
}
void Autopilot::calcAutoPilot() {
	//we dont do this if the autopilot is MANUAL
	if (headingPid.GetMode() == AUTOMATIC) {
		actualHeading = getCurrentHeading();
		headingPid.Compute();

	}
}

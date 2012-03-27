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
#include "Gps.h"
#include <PID_v1/PID_v1.h>
#define P_Param  2.0l //proportional param
#define I_Param  5.0l //manual response time (in Seconds)/4
#define D_Param  0.3l //aggressive factor. Change sign to invert command direction

class Autopilot {
public:
	Autopilot(Gps* gps);
	~Autopilot();
	void enableAutoPilot();
	void disableAutoPilot();
	void calcAutoPilot();
	double getRudderCorrection();
	void sendAutopilotNmea();
	double getCurrentHeading();
	void setCurrentHeading(double heading);
	void setTargetHeading(double target);
	double getTargetHeading();
	bool isEnabled();

private:


//PID headingPid;
Gps* gps;
};

#endif

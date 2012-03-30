// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef FreeBoardPLC_H_
#define FreeBoardPLC_H_

typedef unsigned char byte;

#include "Arduino.h"
//add your includes for the project  here
#include "FreeBoardConstants.h"
#include <EEPROM/EEPROM.h>
#include <PString/PString.h>
//#include <SoftwareSerial/SoftwareSerial.h>
//#include <PinChangeInt/PinChangeInt.h>
//#include <PinChangeInt/PinChangeIntConfig.h>
#include <PinCatcher/PinCatcher.h>
#include <FlexiTimer2/FlexiTimer2.h>
#include "Lcd.h"
#include "Menu.h"
#include "Seatalk.h"
#include "Wind.h"
#include "Gps.h"
#include "Anchor.h"
#include "NmeaSerial.h"
#include "Autopilot.h"
#include "FreeBoardModel.h"


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif



//add your function definitions for the project  here
void check_mem();
void readWDS();
void readWDD();
void checkPress0();
void checkPress1();
void checkPress2();
void calculate();


//Do not add code below this line
#endif /* FreeBoardPLC_H_*/

#ifndef FREEBOARDCONSTANTS_H_
#define FREEBOARDCONSTANTS_H_

//debug
#define DEBUG true
//enable mux onto Serial.print
#define MUX true

//misc output - SoftwareSerial
//lcd
#define rxPin 50
#define txPin 51
//mux NMEA output
#define nmeaRxPin 52
#define nmeaTxPin 53

//alarm (piezo) - tx only so digital pins 22+
//#define alarmPin0 50     // the number of the buzzer pin
//#define alarmPin1 51     // the number of the buzzer pin
#define alarmPin0 22     // the number of the buzzer pin
#define alarmPin1 23     // the number of the buzzer pin
#define alarmPin2 24     // the number of the buzzer pin
#define alarmPin3 25     // the number of the buzzer pin
//button pins - using PCINT2 analog 9-15, mega pins 63-69
#define button0 63     // the number of the pushbutton pin
#define button1 64     // the number of the pushbutton pin
#define button2 65     // the number of the pushbutton pin
#define button3 66       //remote button 1

#define buttonLabel0  "Select"
#define buttonLabel1  "Prev"
#define buttonLabel2  "Next"

// need reliable efficient interrupts, so INT2/INT3
#define windSpeedPin 21 // pin21
#define windSpeedInterrupt 2 // pin21
#define windDirPin 20 // pin 20
#define windDirInterrupt 3 // pin 20

//Softkeys
#define EMPTY  "      "
#define SELECT  "Select"
#define ENTER  "Enter"
#define EDIT  "Edit"
#define PREV  "Prev"
#define NEXT  "Next"
#define PGDN  "Next"
#define PGUP  "Prev"
#define PLUS5  "+5"
#define MINUS5  "-5"
#define SET  "Set"

//menu states
#define MINMENU 0
#define GPS MINMENU

#define WIND 2
#define MAXMENU WIND

//anchor menu, level 1
#define ANCHOR  1
#define ANCHORMINMENU  10
#define ANCHORSTATE  ANCHORMINMENU
#define ANCHORPOINT  11
#define ANCHORRADIUS  12
#define ANCHORRTN  13
#define ANCHORMAXMENU  ANCHORRTN

//wind menu
#define WINDMINMENU 20
#define WINDSTATE WINDMINMENU
#define WINDSPEED 21
#define WINDRTN 22
#define WINDMAXMENU WINDRTN

//alarms
#define ANCHORALARM 1
#define WINDALARM	2
#define RADARALARM	4
#define MOBALARM	8

#endif

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

// need reliable efficient interrupts, so INT0/INT1
//Wind speed on pin 3 - INT1 - yellow wire
 //Wind dir on pin 2 - INT0
#define windSpeedPin 3 // pin3
#define windSpeedInterrupt 1 // INT1
#define windDirPin 2 // pin 20
#define windDirInterrupt 0 // INT0



// 0.00053995680 nautical miles per meter
//decimal lat lon is in degrees, and we have 60 NM per degree so degrees per meter
#define  LLMTRS .00000899928
#define  MINMTRS	LLMTRS*5.0

//freeboard model structs
#define CONFIG_T 'C'
#define DYNAMIC_T 'D' //all dynamically changing params, gps ,speed, wind, compass, etc
#define SIMPLE_T 'S' //basic string format
//nav values
	/** version */
#define VER  "VER"
	/** Roll degrees */
#define RLL  "RLL"
	/** Pitch degrees */
#define PCH  "PCH"
	/** Yaw degrees*/
#define YAW  "YAW"
	/** IMU Health?*/
#define IMUH  "IMUH"
	/** Mag X - roll*/
#define MGX  "MGX"
	/** Mag Y - pitch*/
#define MGY  "MGY"
	/** Mag Z - heading */
#define MGZ  "MGZ"
	/** Mag Heading */
#define MGH  "MGH"
	/** Latitude*/
#define LAT  "LAT"
	/** Longitude*/
#define LON  "LON"
	/**Altitude */
#define ALT  "ALT"
	/**CourseOverGround - true*/
#define COG  "COG"
	
	/** Speed over ground */
#define SOG  "SOG"
	/** GPS fix true/false*/
#define FIX  "FIX"
	/** satellites*/
#define SAT  "SAT"
	/** Time of week*/
#define TOW  "TOW"
	/**mag declination*/
//#define DEC  "DEC"
	/**Wind speed apparent*/
#define WSA  "WSA"
	/**Wind speed true*/
#define WST  "WST"
	/**Wind dir apparent*/
#define WDA  "WDA"
	/**Wind dir true*/
#define WDT  "WDT"
	/**Wind speed units*/
#define WSU  "WSU"
/**Wind alarm speed (knots) */
#define WSK  "WSK"
	/**Wind speed alarm state */
#define WSX  "WSX"
	
	/** Autopilot state (on/off)*/
#define APX  "APX" //0off, 1on
	/** Autopilot offset from source*/
#define APT  "APT" //offset from source +/- deg
	/** Autopilot source*/
#define APS  "APS" //W wind or C compass
	
	/**Anchor alarm state*/
#define AAX  "AAX"
	/**Anchor alarm radius*/
#define AAR  "AAR"
	/**Anchor alarm Lat*/
#define AAN  "AAN"
	/**Anchor alarm Lon*/
#define AAE  "AAE"

//attached device types
#define UID  "UID"
#define IMU  "IMU"
#define MEGA  "MEGA"

//Commands
#define VERSION "#VER"
#define AUTOPILOT_STATE "#APX"
#define AUTOPILOT_TARGET "#APT" //offset from source +/- deg
#define AUTOPILOT_ADJUST "#APJ" //adjust +/- deg
#define AUTOPILOT_SOURCE "#APS" //wind or compass

#define ANCHOR_ALARM_STATE "#AAX"
#define ANCHOR_ALARM_RADIUS "#AAR"
#define ANCHOR_ALARM_LAT "#AAN"
#define ANCHOR_ALARM_LON "#AAE"

#define WIND_SPEED_ALARM_STATE "#WSX"
#define WIND_ALARM_KNOTS "#WSK"


#endif

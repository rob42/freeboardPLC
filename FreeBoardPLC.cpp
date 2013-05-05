/*
 * Copyright 2010, 2011,2012,2013 Robert Huitema robert@42.co.nz
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

// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Teensy 2.0         9        10       (none)
// Teensy++ 2.0      25         4       26, 27
// Arduino Uno        9         8         10
// Arduino Mega      46        48       44, 45
// Wiring-S           5         6          4
// Sanguino          13        14         12
/*
 10 Jan 2012 - ported to Eclipse Indigo build env
 Updated to use Arduino 1.0 IDE classes.
 Added Peet Bros wind instruments
 Using interrupts for buttons, wind, and timed events

 Provides:
 GPS
 NMEA mux
 Seatalk<>NMEA bridge
 Anchor alarm
 Autopilot
 Wind speed and dir (true and apparent)
 
 Connections:
 Receives from the serial ports 1,2,3, sends to Serial0.
 GPS attached to serial1
 Other NMEA talkers attached to Serial2, Serial3, etc
 All NMEA recv devices should listen on Serial0 Tx or nmea repeater on 52/53
 All at 4800 default
 GPS at 38400, pins Rx19 - yellow, Tx20 - orange
 Lcd at 19200
 Lcd disabled,  on 50,51 (lcd)
 AltSoftSerial Tx46,Rx48 (NMEA)
 Alarm devices on pin 22,23,24,25
 Wind speed on pin 3 - INT1 - yellow
 Wind dir on pin 2 - INT0
 */

#include "FreeBoardPLC.h"

volatile boolean execute = false;
volatile int interval = 0;
int inByteSerial1;
int inByteSerial2;
int inByteSerial3;
char input;
//volatile int intCnt = 0;

//freeboard model
FreeBoardModel model;

// set up a new serial port for the lcd
//Lcd lcd(&model,(uint8_t) rxPin, (uint8_t) txPin);

//NMEA output - The arduino puts out TTL, NMEA is RS232. They are different V and amps. The +-5V levels may need inverting or you get
// garbage.
// See http://forums.parallax.com/forums/default.aspx?f=19&m=50925
// See http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=63469&start=0
NmeaSerial nmea(&model);

//NMEA ports
NMEA gpsSource(ALL);
NMEA talker0(ALL);
NMEA talker2(ALL);
NMEA talker3(ALL);

//alarm
Alarm alarm(&model);

//wind
Wind wind(&model);

//Gps
Gps gps(&gpsSource, &model);

//Autopilot
Autopilot autopilot(&model);

//Anchor
Anchor anchor(&model);

Seatalk seatalk(&Serial2, &model);

String inputSerial = ""; // a string to hold incoming data
//boolean inputSerialComplete = false; // whether the string is complete
boolean inputSerial1Complete = false; // whether the GPS string is complete
boolean inputSerial2Complete = false; // whether the string is complete
boolean inputSerial3Complete = false; // whether the string is complete

void setup() {

	//model.readConfig();
	inputSerial.reserve(40);
	// initialize  serial ports:
	Serial.begin(38400, 8, 1, 0);
	if (DEBUG) Serial.println("Initializing..");

	//start gps on serial1, autobaud
	if (DEBUG) Serial.println("Start gps..");
	gps.setupGps();
	Serial1.begin(38400);

	if (DEBUG) Serial.println("Start seatalk - serial2..");
	Serial2.begin(4800, 9, 1, 0); //Seatalk interface

	if (DEBUG) Serial.println("Start nmea Rx - serial3..");
	Serial3.begin(4800, 8, 1, 0); //talker2

	if (DEBUG) Serial.println("Start nmea Tx..");
	pinMode(nmeaRxPin, INPUT);
	pinMode(nmeaTxPin, OUTPUT);
	nmea.begin(4800);

	//setup interrupts to windPins
	if (DEBUG) Serial.println("Start wind..");
	pinMode(windSpeedPin, INPUT);
	attachInterrupt(windSpeedInterrupt, readWDS, FALLING);
	pinMode(windDirPin, INPUT);
	attachInterrupt(windDirInterrupt, readWDD, FALLING);

//	//setup timers
	if (DEBUG) Serial.println("Start timer..");
	FlexiTimer2::set(100, calculate); // 100ms period
	FlexiTimer2::start();
	//lcd.clearLcd();

	if (DEBUG) Serial.println("Setup complete..");
	//print out the config
	model.sendData(Serial, CONFIG_T);
}
/*
 * Timer interrupt driven method to do time sensitive calculations
 * The calc flag causes the main loop to execute other less sensitive calls.
 */
void calculate() {
	//we create 100ms pings here
	execute = true;
	//we record the ping count out to 2 secs
	interval++;
	interval = interval % 20;
}

void readWDS() {
	//intCnt++;
	wind.readWindDataSpeed();
}

void readWDD() {
	//intCnt++;
	wind.readWindDataDir();
}

/*
 SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char) Serial.read();
		// add it to the inputString:
		inputSerial += inChar;
		if (inChar == '\n') {
			//inputSerialComplete = true;
			char carray[inputSerial.length() + 1]; //determine size of the array
			inputSerial.toCharArray(carray, sizeof(carray));
			process(carray, ',');
			inputSerial = "";
			//inputSerialComplete = false;
		}

	}
}

void serialEvent1() {
	while (Serial1.available()) {
		inputSerial1Complete = gps.decode(Serial1.read());
		// read from port 1 (GPS), send to port 0:
		if (inputSerial1Complete) {
			if (MUX) nmea.printNmea(gpsSource.sentence());
			Serial.println(gpsSource.sentence());
			//loop every sentence
			break;
		}
	}
}

void serialEvent2() {
	while (Serial2.available()) {
		seatalk.processSeaTalkByte(Serial2.read());
	}
}

void serialEvent3() {
	while (Serial3.available()) {
		inputSerial3Complete = talker3.decode(Serial3.read());
		if (inputSerial3Complete) {
			if (MUX) nmea.printNmea(talker3.sentence());
			Serial.println(talker3.sentence());
			//loop every sentence
			break;
		}
	}
}

void loop() {

	//if (DEBUG)
	//Serial.println("Looping..");

	if (execute) {
		//timer ping
		//do these every 100ms
		autopilot.calcAutoPilot();

//		if (interval % 2 == 0) {
//			//do every 200ms
		wind.calcWindData();
//		}
		if (interval % 5 == 0) {
			//do every 500ms

			//fire any alarms
			//alarm.checkAlarms();
			model.writeSimple(Serial);
		}
		if (interval % 10 == 0) {
			//do every 1000ms
			anchor.checkAnchor();
			alarm.checkWindAlarm();
			nmea.printWindNmea();
			nmea.printTrueHeading();
			//Serial.print("Interrupts:");
			//Serial.println(intCnt);
		}
		//if (interval % 20 == 0) {
		//do every 2000ms
		//}

		execute = false;
	}

//DEBUG
//printf("Looping\n");
	/*
	 //Check memory
	 check_mem() ;
	 String s;
	 s.append("Heap: ");

	 s.append((int)heapptr);
	 s.append(", stack: ");
	 s.append((int)stackptr);
	 if(DEBUG)Serial.println(s);
	 */

}

void process(char * s, char parser) {
	//if (DEBUG) Serial.print("Process str:");
	//if (DEBUG) Serial.println(s);
	char *cmd = strtok(s, ",");
	while (cmd != NULL && strlen(cmd) > 3) {
		//starts with # its a command
		//if (DEBUG) Serial.print("Process incoming..l=");
		//if (DEBUG) Serial.print(strlen(cmd));
		//if (DEBUG) Serial.print(", ");
		//if (DEBUG) Serial.println(cmd);

		char key[5];
		int l = strlen(cmd);

		if (cmd[0] == '#') {
			//
			strncpy(key, cmd, 4);
			key[4] = '\0';
			char val[l - 4];
			memcpy(val, &cmd[5], l - 5);
			val[l - 5] = '\0';
			//if (DEBUG) Serial.print(key);
			//if (DEBUG) Serial.print(" = ");
			//if (DEBUG) Serial.println(val);

			//anchor
			if (strcmp(key, ANCHOR_ALARM_STATE) == 0) {
				//if (DEBUG) Serial.print("AA Entered..");
				model.setAnchorAlarmOn(atoi(val));
				if (atoi(val) == 1) {
					anchor.setAnchorPoint();
				}
			} else if (strcmp(key, ANCHOR_ALARM_ADJUST) == 0) {
				model.setAnchorRadius(model.getAnchorRadius() + atof(val));
			} else if (strcmp(key, ANCHOR_ALARM_LAT) == 0) {
				model.setAnchorLat(atof(val));
			} else if (strcmp(key, ANCHOR_ALARM_LON) == 0) {
				model.setAnchorLon(atof(val));
			}
			//autopliot
			else if (strcmp(key, AUTOPILOT_STATE) == 0) {
				//if (DEBUG) Serial.print("AP Entered..");
				//if (DEBUG) Serial.println(val);
				//this is potentailly dangerous, since we dont want the boat diving off on an old target heading.
				//in model we ALWAYS reset to current magnetic or wind heading at this point
				model.setAutopilotOn(atoi(val));
			} else if (strcmp(key, AUTOPILOT_ADJUST) == 0) {
				model.setAutopilotTargetHeading(model.getAutopilotTargetHeading() + atol(val));
			} else if (strcmp(key, AUTOPILOT_SOURCE) == 0) {
				model.setAutopilotReference(val[0]);
			}
			//wind
			else if (strcmp(key, WIND_SPEED_ALARM_STATE) == 0) {
				model.setWindAlarmOn(atoi(val));
			} else if (strcpy(key, WIND_ALARM_KNOTS) == 0) {
				model.setWindAlarmSpeed(atoi(val));
			} else if (strcpy(key, WIND_ZERO_ADJUST) == 0) {
				model.setWindZeroOffset(atoi(val));
			}
		} else {
			strncpy(key, cmd, 3);
			key[3] = '\0';
			char val[l - 3];
			memcpy(val, &cmd[4], l - 4);
			val[l - 4] = '\0';
			//if (DEBUG) Serial.print(key);
			//if (DEBUG) Serial.print(" = ");
			//if (DEBUG) Serial.println(val);
			// incoming data = WST,WSA,WDT,WDA,WSU,LAT,LON,COG,MGH,SOG,YAW
			if (strcmp(key, MGH) == 0) {
				model.setMagneticHeading(atof(val));
			}
			if (strcmp(key, DECL) == 0) {
				model.setDeclination(atof(val));
			}
			if (strcmp(key, WDT) == 0) {
				model.setWindTrueDir(atoi(val));
			}

		}
		//next token
		cmd = strtok(NULL, ",");
	}
	//if (DEBUG) Serial.println("Process str exit");
}


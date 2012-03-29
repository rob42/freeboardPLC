/* FreeBoard
 * Copyright R T Huitema, All rights reserved
 */

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
 GPS at 38400
 Lcd at 19200
 SoftSerials on 50,51 (lcd) ,52,53 (NMEA)
 Alarm devices on pin 22,23,24,25
 Wind speed on pin 21 - INT3
 Wind dir on pin 20 - INT2
 */

#include "FreeBoardPLC.h"

volatile boolean execute = false;
volatile int interval = 0;
int inByteSerial1;
int inByteSerial2;
int inByteSerial3;
char input;

//freeboard model
FreeBoardModel model;

// set up a new serial port for the lcd
Lcd lcd(&model,(uint8_t) rxPin, (uint8_t) txPin);

//NMEA output - The arduino puts out TTL, NMEA is RS232. They are different V and amps. The +-5V levels may need inverting or you get
// garbage.
// See http://forums.parallax.com/forums/default.aspx?f=19&m=50925
// See http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=63469&start=0
NmeaSerial nmea(&model,(uint8_t) nmeaRxPin, (uint8_t) nmeaTxPin, (bool) false);

//NMEA ports
NMEA gpsSource(ALL);
NMEA talker0(ALL);
NMEA talker2(ALL);
NMEA talker3(ALL);


//alarm
Alarm alarm(&model);

//wind
Wind wind( &model);

//Gps
Gps gps(&gpsSource, &model);

//Autopilot
Autopilot autopilot(&model);

//Anchor
Anchor anchor(&model);

Seatalk seatalk( &Serial2, &model);

//Menu
Menu menu(&lcd, &model);



//re-enable ISR2 in SoftwareSerial.cpp line 314 if you stop using this
class ButtonCatcher: public PinCatcher {
	typedef PinCatcher parent;
public:
	ButtonCatcher() {
	}

	~ButtonCatcher() {
	}

	void watch(unsigned pin) {
		attach(pin);
	}

	void ignore(unsigned pin) {
		detach(pin);
	}

	virtual void handle(unsigned pin, bool rising_edge_transition) {
		if (pin == button0)
			checkPress0();
		if (pin == button1)
			checkPress1();
		if (pin == button2)
			checkPress2();
	}
};

ButtonCatcher p;

void setup() {
	model.readConfig();
	// initialize  serial ports:
	Serial.begin(38400, 8, 1, 0);
	if (DEBUG)
		Serial.println("Initializing..");

	lcd.setupLcd();

	//start gps on serial1, autobaud
	if (DEBUG)
		Serial.println("Start gps..");
	gps.setupGps();

	if (DEBUG)
		Serial.println("Start seatalk - serial2..");
	Serial2.begin(4800, 9, 1, 0); //Seatalk interface

	if (DEBUG)
		Serial.println("Start nmea Rx - serial3..");
	Serial3.begin(4800, 8, 1, 0); //talker2

	if (DEBUG)
		Serial.println("Start nmea Tx..");
	pinMode(nmeaRxPin, INPUT);
	pinMode(nmeaTxPin, OUTPUT);
	nmea.begin(4800);

	//setup interrupts to windPins
	if (DEBUG)
		Serial.println("Start wind..");
	pinMode(windSpeedPin, INPUT);
	attachInterrupt(windSpeedInterrupt, readWDS, RISING);
	pinMode(windDirPin, INPUT);
	attachInterrupt(windDirInterrupt, readWDD, RISING);

	if (DEBUG)
		Serial.println("Start button interrupts..");

	pinMode(button0, INPUT);
	pinMode(button1, INPUT);
	pinMode(button2, INPUT);
	//pinMode(button3, INPUT);

	//PinCatcher
	p.watch(button0);
	p.watch(button1);
	p.watch(button2);
	//p.watch(button3);

//	//setup timers
	if (DEBUG)
		Serial.println("Start timer..");
	FlexiTimer2::set(100, calculate); // 100ms period
	FlexiTimer2::start();
	lcd.clearLcd();

	if (DEBUG)
		Serial.println("Setup complete..");
//	autopilot.enableAutoPilot();
//	autopilot.setTargetHeading(50);
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
	wind.readWindDataSpeed();
}

void readWDD() {
	wind.readWindDataDir();
}

void checkPress0() {
	//set button
	menu.setButton(button0);
}
void checkPress1() {
	//set button
	menu.setButton(button1);
}
void checkPress2() {
	//set button
	menu.setButton(button2);
}

void loop() {

	if (DEBUG)
		//Serial.println("Looping..");
	//dont get caught endlessly reading/writing
	//allow single character commands
	while (Serial.available() > 0) {
		input = Serial.read();
		if (input == 'a') {
			Serial.println("Enable autopilot..");
			model.setAutopilotTargetHeading(model.getAutopilotCurrentHeading());
			model.setAutopilotOn(true);
		}
		if (input == 'd') {
			Serial.println("Disable autopilot..");
			model.setAutopilotOn(false);
		}
		if (input == '-') {
			Serial.println("Autopilot heading - 5");
			//TODO: consider the 0-360 circle, cant be neg
			model.setAutopilotTargetHeading(model.getAutopilotTargetHeading() - 5);
		}
		if (input == '+') {
			Serial.println("Autopilot heading + 5");
			model.setAutopilotTargetHeading(model.getAutopilotTargetHeading() + 5);
		}
		input = ' ';
	}
	// read from port 1, send to port 0:
	while (Serial1.available() > 0) {
		inByteSerial1 = Serial1.read();
		//if(DEBUG)Serial.print(inByte);

		if (gps.decode(inByteSerial1)) {
			if (MUX) {
				nmea.printNmea(gpsSource.sentence());
			}
			if (MUX && DEBUG)
				Serial.println(gpsSource.sentence());
			break; //every sentence
		}

	}

	while (Serial2.available() > 0) {
  	inByteSerial2=Serial2.read();
		seatalk.processSeaTalkByte(inByteSerial2);
	}

	while (Serial3.available() > 0) {
		inByteSerial3=Serial3.read();
		// check if the character completes a valid NMEA sentence
		if(DEBUG)Serial.print(inByteSerial3);
		if (talker3.decode(inByteSerial3) && MUX) {
			nmea.printNmea(talker3.sentence());
		}
		if(MUX && DEBUG)Serial.println(talker3.sentence());
		break;//every sentence
	}
	if (execute) {
		//timer ping
		//do these every 100ms
		autopilot.calcAutoPilot();

//		if (interval % 2 == 0) {
//			//do every 200ms
//		}
		if (interval % 5 == 0) {
			//do every 500ms

			//fire any alarms
			alarm.checkAlarms();

		}
		if (interval % 10 == 0) {
			//do every 1000ms
			wind.calcWindData();
			if (DEBUG && model.isAutopilotOn()) {
					Serial.print("Target deg = ");
					Serial.print(model.getAutopilotTargetHeading());
					Serial.print("Heading deg = ");
					Serial.print(model.getAutopilotCurrentHeading());
					Serial.print(", Rudder = ");
					Serial.println(model.getAutopilotRudderCommand());
					model.setAutopilotCurrentHeading(model.getAutopilotCurrentHeading()+model.getAutopilotRudderCommand());
				}
			anchor.checkAnchor();
			alarm.checkWindAlarm();
			nmea.printWindNmea();
		}
		//if (interval % 20 == 0) {
			//do every 2000ms
		//}

		execute = false;
	}

	//show data, these are internally timed
	lcd.showData();

//	//check buttons
	menu.checkButtons();

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


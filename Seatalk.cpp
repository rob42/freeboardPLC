/* Seatalk routines*/

/* Commands (My C70 says 6E 77...)
 6E  07  00  00 00 00 00 00 00 00 MOB (Man Over Board), (ST80), preceded
 by a Waypoint 999 command: 82 A5 40 BF 92 6D 24 DB
 36  00  01      Cancel MOB (Man Over Board) condition

 Wind
 66  00  XY      Wind alarm as indicated by flags in XY:
 X&8 = 8: Apparent Wind angle low
 X&4 = 4: Apparent Wind angle high
 X&2 = 2: Apparent Wind speed low
 X&1 = 1: Apparent Wind speed high
 Y&8 = 8: True Wind angle low
 Y&4 = 4: True Wind angle high
 Y&2 = 2: True Wind speed low
 Y&1 = 1: True Wind speed high (causes Wind-High-Alarm on ST40 Wind Instrument)
 XY  =00: End of wind alarm (only sent once)

 68  X1 01 00   Alarm acknowledgment keystroke (from ST80 Masterview)
 68  X1 03 00   Alarm acknowledgment keystroke (from ST80 Masterview)
 68  41 15 00   Alarm acknowledgment keystroke (from ST40 Wind Instrument)
 X: 1=Shallow Shallow Water Alarm, 2=Deep Water Alarm, 3=Anchor Alarm
 4=True Wind High Alarm, 5=True Wind Low Alarm, 6=True Wind Angle high
 7=True Wind Angle low, 8=Apparent Wind high Alarm, 9=Apparent Wind low Alarm
 A=Apparent Wind Angle high, B=Apparent Wind Angle low

 Radar
 A8  53  80 00 00 D3  Alarm ON  for Guard #1 or #2
 A8  43  80 00 00 C3  Alarm OFF for Guard #1 or #2
 AB  53  80 00 00 D3  Alarm ON  for Guard #1 or #2
 AB  43  80 00 00 C3  Alarm OFF for Guard #1 or #2

 Autopilot
 83  07  XX  00  00  00  00  00  80  00  00  Sent by course computer.
 XX = 0 after clearing a failure condition, also sent once after power-up.
 XX = 1 failure, auto release error. Repeated once per second.
 XX = 8 failure, drive stopped.

 */

#include "Seatalk.h"

	Seatalk::Seatalk(Alarm* alarm, HardwareSerial* serial) {
	this->alarm=alarm;
	this->serial=serial;
	radarAlarmTriggered = false; //set to true to trigger radar alarm
	mobAlarmTriggered = false; //set to true to trigger MOB alarm
	seaTalkPos = 0;
}

bool Seatalk::getMobAlarmTriggered() {
	return mobAlarmTriggered;
}

bool Seatalk::getRadarAlarmTriggered() {
	return radarAlarmTriggered;
}
void Seatalk::windAlarmOff() {
	// send 66  00 00 Wind alarm over
	serial->write9(0x66, true);
	serial->write9(0x00, false);
	serial->write9(0x00, false);
}
void Seatalk::radarAlarmOff() {
	// send A8  43  80 00 00 C3  Alarm OFF for Guard #1 or #2
	serial->write9(0xA8, true);
	serial->write9(0x43, false);
	serial->write9(0x00, false);
	serial->write9(0x00, false);
	// send AB  43  80 00 00 C3  Alarm OFF for Guard #1 or #2
	serial->write9(0xAB, true);
	serial->write9(0x43, false);
	serial->write9(0x00, false);
	serial->write9(0x00, false);
}

void Seatalk::cancelMOB() {
	// Send 36  00  01      Cancel MOB (Man Over Board) condition
	serial->write9(0x36, true);
	serial->write9(0x00, false);
	serial->write9(0x01, false);
}
void Seatalk::processSeaTalkByte(byte inByte) {
	if (inByte > 256) {
		//process last datagram....would be nice to do this earlier but how to know its ended?
		//second byte (4 least significant bits have additional bytes. Length is 3 + additional bytes
		processSeatalk(seatalkStream);
		seaTalkPos = 0;
		// Serial.print("Command:");
		inByte = inByte - 256;
	}
	seatalkStream[seaTalkPos] = inByte;
	if (seaTalkPos < 19)
		seaTalkPos++;

}

void Seatalk::setMobAlarmTriggered(bool value) {
	mobAlarmTriggered = value;
}

void Seatalk::setRadarAlarmTriggered(bool value) {
	radarAlarmTriggered = value;
}
void Seatalk::windCommand(byte * seatalkStream) {
	if (seatalkStream[1] == 0x00) {
		if (seatalkStream[2] == 0x00) {
			//alarm off
			alarm->clearAlarm(WINDALARM);
		} else {
			alarm->setSnoozeAlarm(0);
			alarm->setAlarm(WINDALARM);
		}

	}
}

void Seatalk::radarCommand(byte * seatalkStream) {
	if ((seatalkStream[1] == 0x43 || seatalkStream[1] == 0x53)
			&& seatalkStream[2] == 0x80 && seatalkStream[3] == 0x00
			&& seatalkStream[4] == 0x00) {
		if (seatalkStream[5] == 0xD3) {
			if (DEBUG)
				Serial.println("  Radar Guard Zone Alarm ON");
			alarm->setSnoozeAlarm(0);
			radarAlarmTriggered = true;
		}
		if (seatalkStream[5] == 0xC3) {
			if (DEBUG)
				Serial.println("  Radar Guard Zone Alarm OFF");
			radarAlarmTriggered = false;
		}

	}
}
void Seatalk::processSeatalk(byte * seatalkStream) {
	/* if(DEBUG){
	 Serial.print("Processing ");
	 int x =0;
	 while(seatalkStream[x]!=0x00){
	 Serial.print(seatalkStream[x],HEX);
	 x++;
	 }
	 Serial.print(": ");
	 }*/
	//what alarm?
	switch (seatalkStream[0]) {
	//MOB, Thomas Knauf says 67 07... but my C70 gives 67 77 ... listen for both
	case 0x67:
		if ((seatalkStream[1] == 0x77 || seatalkStream[1] == 0x07)
				&& seatalkStream[2] == 0x00 && seatalkStream[3] == 0x00
				&& seatalkStream[4] == 0x00 && seatalkStream[5] == 0x00
				&& seatalkStream[6] == 0x00 && seatalkStream[7] == 0x00
				&& seatalkStream[8] == 0x00 && seatalkStream[9] == 0x00) {
			if (DEBUG)
				Serial.println("  MOB Alarm ON");
			mobAlarmTriggered = true;
		}
		break;
	case 0x36:
		if (seatalkStream[1] == 0x00 && seatalkStream[2] == 0x01) {
			if (DEBUG)
				Serial.println("  Cancel MOB Alarm");
			mobAlarmTriggered = false;
		}
		break;

		//radar guard zone
	case 0xA8:
		radarCommand(seatalkStream);
		break;
	case 0xAB:
		radarCommand(seatalkStream);
		break;
	case 66:
		windCommand(seatalkStream);
		break;
	default:
		//if(DEBUG)Serial.println("  Command ignored");
		break;
	}
}


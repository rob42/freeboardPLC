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

#ifndef SEATALK_H_
#define SEATALK_H_

#include "Arduino.h"

#include "HardwareSerial.h"
#include "FreeBoardConstants.h"
#include "FreeBoardModel.h"

class Seatalk {

public:
	Seatalk( HardwareSerial* serial, FreeBoardModel* model);

	void windAlarmOff();
	void radarAlarmOff() ;
	void cancelMOB() ;
	void processSeaTalkByte(byte inByte);
private:
	void windCommand(byte * seatalkStream);
	void radarCommand(byte * seatalkStream);
	void processSeatalk(byte * seatalkStream);

	//seatalk
	volatile int seaTalkPos;
	byte seatalkStream[20]; //Seatalk datagram is 3-18 chars
	FreeBoardModel* model;
	HardwareSerial* serial;
};
#endif

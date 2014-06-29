/*
  nmea.cpp - NMEA 0183 sentence decoding library for Wiring & Arduino
  Copyright (c) 2008 Maarten Lamers, The Netherlands.
  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef NmeaRelay_h
#define NmeaRelay_h

//#include "WConstants.h"

class NMEARelay
{
  public:
		NMEARelay(int connect);					// constructor for NMEA parser object; parse sentences of GPRMC or all datatypes.
		int		decode(char c);				// parse one character received from GPS; returns 1 when full sentence found w/ checksum OK, 0 otherwise
		char*	sentence();						// returns last received full sentence as zero terminated string
  private:
  	// properties
		char	_sentence[100];
		char	f_sentence[100];
		int		n;
		int		_state;

};

#endif

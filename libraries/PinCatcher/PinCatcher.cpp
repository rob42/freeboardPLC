// PinCatcher - A pin change interrupt catcher.
// Copyright (C) 2011  Scott Bailey.  All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PinCatcher_impl.h"
#include "PinCatcher.h"
#include <avr/interrupt.h>


//--- constructors/destructor ------------------------------------------------------------------------------------------

PinCatcher::PinCatcher()
{
}


PinCatcher::~PinCatcher()
{
}

//--- alphabetic -------------------------------------------------------------------------------------------------------

void PinCatcher::attach(unsigned pin)
{
	// mega1280
		// PCINT0 -> PCMSK0 (PCINT0 through PCINT7) pins 10-13
		// PCINT1 -> PCMSK1 (PCINT8 through PCINT15) pins 50-53
		// PCINT2 -> PCMSK2 (PCINT16 through PCINT23) pins 62-69

	// mega1280 - from pins_arduino.h
	//A majority of the pins are NOT PCINTs, SO BE WARNED (i.e. you cannot use them as receive pins)
	// Only pins available for RECEIVE (TRANSMIT can be on any pin):
	// (I've deliberately left out pin mapping to the Hardware USARTs - seems senseless to me)
	// Pins: 10, 11, 12, 13,  50, 51, 52, 53,  62, 63, 64, 65, 66, 67, 68, 69

   if( (pin > 9 && pin < 15) || (pin>49 && pin <54) || (pin >61 && pin < 70) )
      pc_i.attachPin(pin, this);
}

void PinCatcher::detach(unsigned pin)
{
   if( (pin > 9 && pin < 15) || (pin>49 && pin <54) || (pin >61 && pin < 70))
      pc_i.detachPin(pin);
}


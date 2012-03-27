
// Please read Bounce.h for information about the liscence and authors

#include "Arduino.h"
#include "Bounce.h"


Bounce::Bounce(uint8_t pin,unsigned long interval_millis)
{
	interval(interval_millis);
	previous_millis = millis();
	state = digitalRead(pin);
    this->pin = pin;
}


void Bounce::write(int new_state)
       {
       	state = new_state;
       	digitalWrite(pin,state);
       }


void Bounce::interval(unsigned long interval_millis)
{
  //interval_millis = interval_millis;
  rebounce_millis = 0;
}

void Bounce::rebounce(unsigned long interval)
{
	 rebounce_millis = interval;
}



int Bounce::update()
{
	
	  if ( debounce() ) {
	   rebounce(0);
	   return 1;
	 }

     // We need to rebounce, so simulate a state change
     
	 if ( rebounce_millis && (millis() - previous_millis >= rebounce_millis) ) {
		 previous_millis = millis();
		 rebounce(0);
		 return 1;
	 }

	return 0;
	
}


unsigned long Bounce::duration()
{
  return millis() - previous_millis;
}


int Bounce::read()
{
	return (int)state;
}


// Protected: debounces the pin
int Bounce::debounce() {
	
	uint8_t newState = digitalRead(pin);
	if (state != newState ) {
  		if (millis() - previous_millis >= interval_millis) {
  			previous_millis = millis();
  			state = newState;
  			return 1;
	}
  }
  
  return 0;
	
}


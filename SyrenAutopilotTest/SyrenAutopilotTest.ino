#include "FreeBoardConstants.h"
#include <AltSoftSerial.h>
#include <Kangaroo.h>

AltSoftSerial altSerial; 
KangarooSerial  K(altSerial);
KangarooChannel K1(K, '1');

/*
Tx = S1
Rx = S2
DIP1 = on 
DIP3 = on
DIP4 = on

Need absolute feedback device, eg potentiometer
*/

void setup() {
  Serial.begin(38400);
  altSerial.begin(9600);
  Serial.print("Starting Kangaroo..");
  K1.start();
  Serial.print("homing..");
  K1.home().wait();
  Serial.println("done");

}

void loop() {
  Serial.println("Kangaroo test: rotate to min at default speed...");
  // Go to the minimum side at whatever speed limit is set on the potentiometers.
  long minimum = K1.getMin().value();
  K1.p(minimum).wait();
  Serial.println("Kangaroo test: rotate to max at default speed...");
  // Go to the maximum side at whatever speed limit is set on the potentiometers.
  long maximum = K1.getMax().value();
  K1.p(maximum).wait();
  
  delay(2000);
  Serial.print("homing..");
  K1.home().wait();
  Serial.println("done");
  delay(2000);
  Serial.println("Kangaroo test: rotate to max over 4 secs...");
  // Going to the maximum side, limit speed to 1/4th of the range per second
  // (at least 4 seconds of travel time).
  long speedLimit = (maximum - minimum) / 8;
  K1.p(maximum, speedLimit).wait();

  delay(2000);
  
  Serial.print("homing..");
  K1.home().wait();
  Serial.println("done");

  Serial.println("Kangaroo test: rotate to 1/4 min at default speed...");
  
  long offset = (maximum - minimum) / 8;
  K1.p(offset).wait();

  delay(2000);
  
  Serial.println("Kangaroo test: rotate very slowly to home...");
  speedLimit = (maximum - minimum) / 20;
  offset = (maximum - minimum)/2;
  K1.p(offset,speedLimit).wait();
   Serial.println("done");
}

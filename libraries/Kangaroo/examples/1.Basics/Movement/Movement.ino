// Movement Sample for Kangaroo
// Copyright (c) 2013 Dimension Engineering LLC
// See license.txt for license details.

#include <SoftwareSerial.h>
#include <Kangaroo.h>

// Arduino TX (pin 11) goes to Kangaroo S1
// Arduino RX (pin 10) goes to Kangaroo S2
// Arduino GND         goes to Kangaroo 0V
// Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
#define TX_PIN 11
#define RX_PIN 10

// Independent mode channels on Kangaroo are, by default, '1' and '2'.
SoftwareSerial  SerialPort(RX_PIN, TX_PIN);
KangarooSerial  K(SerialPort);
KangarooChannel K1(K, '1');
KangarooChannel K2(K, '2');

void setup()
{
  SerialPort.begin(9600);
  SerialPort.listen();
  
  K1.start();
  K1.home().wait();
}

// .wait() waits until the command is 'finished'. For position, this means it is within the deadband
// distance from the requested position. You can also call K1.p(position); without .wait() if you want to command it
// but not wait until it gets to the destination. If you do this, you may want to use K1.getP().value()
// to check progress.
void loop()
{
  // Go to the minimum side at whatever speed limit is set on the potentiometers.
  long minimum = K1.getMin().value();
  K1.p(minimum).wait();
  
  delay(2000);
  
  // Going to the maximum side, limit speed to 1/10th of the range per second
  // (at least 10 seconds of travel time).
  long maximum = K1.getMax().value();
  long speedLimit = (maximum - minimum) / 10;
  K1.p(maximum, speedLimit).wait();

  delay(2000);
}

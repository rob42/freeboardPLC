// Serial Timeout Sample for Kangaroo
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
  K1.serialTimeout(1000); // If we don't send anything to the Kangaroo for 1 second (1000 ms),
                          // it will abort and hold position (if the last command was position)
                          // or hold velocity (if the last command was velocity).
                          // Since serial timeout is intended as a form of safety abort, to
                          // recover from a serial timeout, start() has to be issued again.
                          // Try disconnecting the TX line from S1 temporarily to see it in action.
  K1.home().wait();
}

void loop()
{
  long minimum = K1.getMin().value();
  K1.p(minimum).wait();
  
  delay(250);
  
  long maximum = K1.getMax().value();
  K1.p(maximum).wait();

  delay(250);
}

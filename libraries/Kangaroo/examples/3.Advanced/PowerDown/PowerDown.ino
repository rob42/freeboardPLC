// Power Down Sample for Kangaroo
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

void loop()
{
  long minimum = K1.getMin().value();
  K1.p(minimum).wait();
  
  // Now that we are in position, power down for five seconds.
  // Notice that the Kangaroo provides no special resistance while the channel is powered down.
  K1.powerDown();
  delay(5000);
  
  long maximum = K1.getMax().value();
  K1.p(maximum).wait();

  K1.powerDown();
  delay(5000);
}

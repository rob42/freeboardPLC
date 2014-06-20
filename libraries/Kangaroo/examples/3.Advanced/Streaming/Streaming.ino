// Streaming Sample for Kangaroo
// Copyright (c) 2014 Dimension Engineering LLC
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
  // Go to the minimum side.
  long minimum = K1.getMin().value();  
  long maximum = K1.getMax().value();
  
  K1.p(minimum).wait();
  
  // We turn streaming on because it will allow us to achieve a much
  // higher command rate. We're sending commands continually, so the
  // rate is more important than if a few intermediates are lost.
  K1.streaming(true);
  
  // Let's inch along one tick at a time in this example.
  // In the real world you may want to send the Kangaroo movements
  // from a joystick, your robot's inverse kinematics routines, etc.
  for (long position = minimum; position <= maximum; position ++)
  {
    K1.p(position);
  }
  
  for (long position = maximum; position >= minimum; position --)
  {
    K1.p(position);
  }
  
  // Turn streaming back off, as we want the next command to be reliable.
  K1.streaming(false);
  
  // Go back to the minimum side.
  K1.p(minimum).wait();
}

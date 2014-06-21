
#include "FreeBoardConstants.h"
#include <AltSoftSerial.h>
#include <SPI.h>
#include <MultiSerial.h>


AltSoftSerial altSerial; 
MultiSerial mSerial0 = MultiSerial(CS_PIN,0);
MultiSerial mSerial1 = MultiSerial(CS_PIN,1);
int cnt=1;

void setup() {
  // put your setup code here, to run once:
Serial.begin(38400);
Serial1.begin(9600); 
pinMode(A13, OUTPUT);
	Serial.println("CS_PIN set to OUTPUT");
	delay(10);
	//Clear Chip Select
	digitalWrite(A13,HIGH);
delay(10);
mSerial0.begin(9600ul,7372800ul);
delay(100);
mSerial1.begin(9600);
delay(100);
Serial2.begin(9600);
Serial3.begin(9600);

altSerial.begin(9600); 

}

void loop() {
  
  while(cnt< 3){
    delay(2000);
    //Serial1 (GPS) to SPI0 (autopilot)
      
     Serial.println();
      Serial.print("*****Testing Autopilot loopback:");
      char testStrng[] = "test";
      String content = "";
      char character;
        //tx>rx
        int i=0;
      for (i = 0; i < sizeof(testStrng) - 1; i++){
	  mSerial0.write(testStrng[i]);
      }
      delay(50);
      
      while(mSerial0.available()>0) {
          character = mSerial0.read();
           delay(20);
          //Serial.print(character);
          content.concat(character);
      }
    
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      //rx>tx
      
      Serial.println();
      Serial.print("*****Testing GPS loopback:");
      content = "";
      //character = null;
      Serial1.print("test");
      delay(50);
      while(Serial1.available()) {
          character = Serial1.read();
          //Serial.print(character);
          delay(50);
          content.concat(character);
      }
    
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
    //tx>rx
      //rx>tx

      //Serial2(NMEA1) to Serial3(NMEA2)
      
      Serial.println();
      Serial.print("***** NMEA1 loopback");
      content = "";
      
        //tx>rx
      Serial2.print("test");
  
      delay(150);
      Serial.println(Serial2.available());
      while(Serial2.available()>0) {
          character = Serial2.read();
          Serial.println(character);
          delay(50);
          content.concat(character);
      }
      Serial.print(content);
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      //rx>tx
      Serial.println();
      Serial.print("*****Testing NMEA2 loopback");
      content = "";
      //character = null;
      Serial3.print("test");
      delay(50);
      while(Serial3.available()) {
          character = Serial3.read();
          delay(50);
          content.concat(character);
      }
    
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
    
      
      //mSerial1(NMEA3) to AltSerial(NMEAtalk)
      

     Serial.println();
      Serial.print("*****Testing NMEA3 transmit to NMEAtalker:");
      content = "";
      
     // char testStrng[] = "test";
      content = "";
      
        //tx>rx
       i=0;
      for (i = 0; i < sizeof(testStrng) - 1; i++){
	  mSerial1.write(testStrng[i]);
      }
      delay(50);
    
      while(altSerial.available()) {
          character = altSerial.read();
          //Serial.print(character);
          content.concat(character);
           delay(50);
      }
    
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      
      //rx>tx
      Serial.print("*****Testing NMEAtalker transmit to NMEA3:");
      content = "";
      //character = null;
      altSerial.print("test");
      delay(50);
      while(mSerial1.available()) {
          character = mSerial1.read();
          //Serial.print(character);
          delay(50);
          content.concat(character);
      }
    
      if (content == "test") {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      Serial.println();
      
      //test level sensors. They are linked to ALM M0SFET, which grounds them
      Serial.print("*****Testing Level1 :");
      //setup MOSFET
      pinMode(alarmPin0, OUTPUT);
      digitalWrite(alarmPin0,LOW);
      
      pinMode(lvl2Pin, INPUT_PULLUP);
      pinMode(lvl1Pin, INPUT_PULLUP); 
      delay(200);
      //Serial.println(digitalRead(lvl1Pin));
      if (HIGH == digitalRead(lvl1Pin)) {        
        Serial.print("pass1..");
      }else{
        Serial.print("fail1..");
      }
      digitalWrite(alarmPin0, HIGH); //MOSFET grounds out
      delay(200);
      //Serial.println(digitalRead(lvl1Pin));
      if (LOW == digitalRead(lvl1Pin)) {        
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      //level2
       Serial.print("*****Testing Level2 :");

      digitalWrite(alarmPin0, LOW); //MOSFET open
       //Serial.println(digitalRead(lvl2Pin));
       delay(200);
      if (HIGH == digitalRead(lvl2Pin)) {        
        Serial.print("pass1..");
      }else{
        Serial.print("fail1..");
      }
      digitalWrite(alarmPin0, HIGH); //MOSFET grounds out
      delay(200);
      //Serial.println(digitalRead(lvl2Pin));
      if (LOW == digitalRead(lvl2Pin)) {        
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      digitalWrite(alarmPin0, LOW); //MOSFET open
      //wind
      
      
      Serial.println();
      //log and lvl3. They are linked to SOL M0SFET, which grounds them
        Serial.print("*****Testing LOG:");
        //setup MOSFET
      pinMode(autopilotEngagePin, OUTPUT);
      digitalWrite(autopilotEngagePin,LOW);
      //level1
      pinMode(logPin, INPUT_PULLUP);
      pinMode(lvl3Pin, INPUT_PULLUP); 
      delay(200);
      if (HIGH == digitalRead(logPin)) {
        Serial.print("pass1..");
      }else{
        Serial.print("fail1..");
      }
      digitalWrite(autopilotEngagePin, HIGH);
     delay(200);
      if (LOW == digitalRead(logPin)) {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      //lvl3
      Serial.print("*****Testing LPG:");
      //level1
      digitalWrite(autopilotEngagePin, LOW);
      delay(200);
      byte pin = digitalRead(lvl3Pin);
      if (HIGH == pin) {
        Serial.print("pass1..");
      }else{
        Serial.print("fail1..");
      }
      digitalWrite(autopilotEngagePin, HIGH);
     delay(200);
     pin = digitalRead(lvl3Pin);
      if (LOW == pin) {
        Serial.println("passed");
      }else{
        Serial.println("failed");
      }
      delay(2000);
      digitalWrite(autopilotEngagePin, LOW);
  
  //CAN (CANL/CANH)*/
  cnt++;
  }
  
}

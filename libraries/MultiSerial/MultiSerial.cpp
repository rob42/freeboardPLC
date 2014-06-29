/*
  MultiSerial.cpp - Arduino library for the StrichLabs MultiSerial Shield
  Copyright (c) 2010 Sarah Nordstrom.  Licensed under Creative Commons BY-NC-SA.
*/

/*
 * Modified for the Freeboard project to provide SPI comms rather than I2C
 */

// include this library's description file
#include "MultiSerial.h"

// include description files for Wire, as the shield uses I2C
//no,no ...lets use SPI!

//Freeboard interface has 7.372800MHz xtal.
//#define   MS_HZ 11059200L
#define   MS_HZ 7372800ul

const byte Read =  B10000000;
// track if the shield has been reset, as we only ever want to do this once
static int needsReset=1;

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

// Description: Creates a variable of type MultiSerial, to communciate with the MultiSerial Shield.
// Syntax: MultiSerial(address, port)
// Parameter: selectPin - The select pin CS_PIN
// Parameter: port - Which port (0 or 1) on the shield to associate this instance with
// Returns: Instance of MultiSerial associated with a specific port on a specific shield.
MultiSerial::MultiSerial(byte chipSelectPin, byte selectedPort) {
  selectPin = chipSelectPin;
  port = selectedPort;

  // each channel gets a one byte 'peek buffer', to support peek() even though
  // the hardware itself has no ability to peek into the buffer
  //peek_buf_valid[0] = 0;
  //peek_buf_valid[1] = 0;
}

MultiSerial::MultiSerial(void) {
  // this 'fake constructor' allows creating an 'empty' MultiSerial type variable
  // later on, the 'real constructor' can be called to actually begin using it.
	selectPin=0;
}

// Public Methods //////////////////////////////////////////////////////////////
// Functions available in Wiring sketches, this library, and other libraries

// Description: Sets the data rate in bits per second for serial data transmission.  This is the normal constructor used.
// Syntax: MultiSerialInstance.begin(speed)
// Parameter: speed - communications speed in bits per second
// Returns: nothing
void MultiSerial::begin(unsigned long baud) {
  // just call the other constructor, using the standard crystal frequency
  begin(baud, MS_HZ);
}

// Description: Sets the data rate in bits per second for serial data transmission.  This constructor should be used if you are using a non-standard crystal.
// Syntax: MultiSerialInstance.begin(speed, crystalHz)
// Parameter: speed - communications speed in bits per second
// Parameter: crystalHz - Speed of the crystal on the board, in Hz
// Returns: nothing
void MultiSerial::begin(unsigned long baud, unsigned long crystalHz) {

	Serial.println(F("MultiSerial::begin"));

	SPI.begin();
	//delay(100);
	//Serial.println("SPI begin done");
	delay(10);
	SPI.setBitOrder(MSBFIRST);
	//delay(100);
	//Serial.println("SPI bitorder done");
	//delay(10);

	  //SPI interface will run at 1MHz if 8MHz chip or 2Mhz if 16Mhz
	  SPI.setClockDivider(SPI_CLOCK_DIV16);
	  //delay(100);
	  //Serial.println("SPI clock done");
	  //delay(10);
	  //Data is clocked on the rising edge and clock is low when inactive
	  SPI.setDataMode(SPI_MODE0);
	  //delay(10);
	  //Serial.println("SPI mode done");
	  Serial.println(F("SPI started"));
	  //delay(10);

  // if this is the first time the shield is used, reset the UART so
  // that we can start off in a known state
  // FIXME: will this work for >1 shield though?
  if(needsReset==1) {
  	msWriteRegister(IOControl, 0x8);
  	needsReset=0;
  	Serial.println(F("SPI UART reset"));
  }

  delay(100);

  // switch to the special register bank
  msWriteRegister(LCR, 128);

  // set the baud rate
  unsigned short baudDivisor;
  baudDivisor = (crystalHz + baud * 8L) / (baud * 16L);
  //Serial.print("SPI UART baud divisor:");
  //Serial.println(baudDivisor);
  msWriteRegister(DLL, baudDivisor & 0xFF);
  msWriteRegister(DLH, baudDivisor >> 8);
  //if(true)return;
  // switch back to the normal register bank
  msWriteRegister(LCR, 0);

  // set the word size to 8 bits
  setWordSize(8);

  // enable the TX/RX fifos
  msWriteRegister(FCR, 7);
  Serial.println(F("SPI init complete"));
}

// Description: Transmit a byte of data out the serial port.
// Syntax: MultiSerialInstance.write(data)
// Parameter: data - byte of data to write
// Returns: nothing
// FIXME: This function should return something useful
size_t MultiSerial::write(uint8_t val) {
  msWriteRegister(THR, val);
  return B00000001;
}

// Description: Transmit a series of bytes of data out the serial port.
// Syntax: MultiSerialInstance.write(data, nbrBytes)
// Parameter: data - array of bytes of data to write
// Parameter: nbrBytes - number of elements in the data array
// Returns: nothing
// FIXME: This function should return something useful
void MultiSerial::write(uint8_t val[], uint8_t nbrBytes) {
  msWriteRegister(THR, val, nbrBytes);

}


// Description: Read the next available byte of available serial data.
// Syntax: MultiSerialInstance.read()
// Parameter: none
// Returns: first available byte of serial data, or -1 if no data is available
int MultiSerial::read(void) {

    return msReadRegister(RHR);

}

// Description: Get the number of bytes available for reading from the serial port.
// Syntax: MultiSerialInstance.available()
// Parameter: none
// Returns: number of bytes available to read
int MultiSerial::available(void) {

  return msReadRegister(RXLVL);
}

// Description: Store one byte of data associated with a specific shield and port.  This byte is stored on the shield itself, not in the Arduino's memory.  Each successive call to store() will overwrite any previous byte stored.
// Syntax: MultiSerialInstance.store(data)
// Parameter: one byte of data to associate with a specific port on a specific shield
// Returns: nothing
void MultiSerial::store(byte val) {
  msWriteRegister(SPR, val);
}

// Description: Retrieve the byte of data aassociated with a specific shield and port, that had been stored earlier.
// Syntax: MultiSerialInstance.retrieve()
// Parameter: none
// Returns: one byte of data that is associated with a specific port on a specific shield
byte MultiSerial::retrieve(void) {
  return msReadRegister(SPR);
}

// Description: Flush all waiting data out of the buffer.
// Syntax: MultiSerialInstance.flush()
// Parameter: none
// Returns: nothing
void MultiSerial::flush() {
  msWriteRegister(FCR, 6);    // flush receive FIFO
}

// Description: Read the next available byte of available serial data, but leave it in the buffer.  Every call to peek() will return the same byte of data, until read() is called to return/remove it from the buffer or flush() is called to purge it.
// Syntax: MultiSerialInstance.peek()
// Parameter: none
// Returns: first available byte of serial data, or -1 if no data is available
int MultiSerial::peek() {
  if(msReadRegister(RXLVL) < 1) return -1;
  return -1; //peek_buf[chan];
}

// Description: Configured the specified GPIO pin to be an input or an output.
// Syntax: MultiSerialInstance.msPinMode(pin, mode);
// Parameter: pin - GPIO pin number to change
// Parameter: mode - either INPUT or OUTPUT
// Returns: nothing
void MultiSerial::msPinMode(byte pin, byte direction) {
  byte regDirs;

  regDirs = msReadRegister(IODir);
  if(direction == OUTPUT) {
    regDirs = regDirs | (1 << pin);
  } else {
    msWriteRegister(IODir, 0);
    regDirs = regDirs & ~(1 << pin);
  }
  msWriteRegister(IODir, regDirs);
}

// Description: Set a specified GPIO pin to a HIGH or LOW state.  Unlike the Arduino digital pins, these do not have pullup resistors, so digitalWrite when the pin is set to an input has no effect.
// Syntax: MultiSerialInstance.msDigitalWrite(pin, value)
// Parameter: pin - GPIO pin number to change
// Parameter: value - HIGH or LOW
// Returns: nothing
void MultiSerial::msDigitalWrite(byte pin, byte value) {
  byte regValues;

  regValues = msReadRegister(IOState);
  if(value == HIGH) {
    regValues = regValues | (1 << pin);
  } else {
    regValues = regValues & ~(1 << pin);
  }
  msWriteRegister(IOState, regValues);
}

// Description: Read the current value of a specific GPIO pin.
// Syntax: MultiSerialInstance.msDigitalRead(pin)
// Parameter: pin - GPIO pin number to read the value of
// Returns: HIGH or LOW
byte MultiSerial::msDigitalRead(byte pin) {
  byte regValues;

  regValues = msReadRegister(IOState);
  if((regValues & (1 << pin)) > 0) {
    return HIGH;
  } else {
    return LOW;
  }
}

// Description: Set the data word size to use on a specific serial port.  Defaults to 8 bits when begin() is called, but can be changed if required.
// Syntax: MultiSerialInstance.setWordSize(size)
// Parameter: size - word size to use on the port, valid values are 5, 6, 7, or 8
// Returns: nothing
void MultiSerial::setWordSize(byte wordSize) {
  byte regValue;

  wordSize -= 5;
  regValue = msReadRegister(LCR);
  regValue = (regValue & ~(1|2)) | (wordSize & (1|2));
  msWriteRegister(LCR, regValue);
}

// Description: Set the number of stop bits.  Defaults to 1 when begin() is called, but can be changed if required.
// Syntax: MultiSerialInstance.setStop(size)
// Parameter: stopBits - 0 -> 1 stop bit; 1 -> 1.5 stop bits if word size == 5, 2 stop bits if word size != 5
// Returns: nothing
void MultiSerial::setStopBits(byte stopBits) {
  byte regValue;
  regValue = msReadRegister(LCR);
  bitWrite(regValue, 2, stopBits);
  msWriteRegister(LCR, regValue);
}

// Description: Enable specific interrupt types.
// Syntax: MultiSerialInstance.enableInterrupt(types)
// Parameter: types - bitmask of INT_RX and/or INT_TX for which interrupt(s) to enable
// Returns: nothing
void MultiSerial::enableInterrupt(byte interruptTypes) {
  byte curIntFlags = msReadRegister(IER) & (INT_RX | INT_TX);
  msWriteRegister(IER, (interruptTypes | curIntFlags) & (INT_RX | INT_TX));
}

// Description: Disable specific interrupt types.
// Syntax: MultiSerialInstance.disableInterrupt(types)
// Parameter: types - bitmask of INT_RX and/or INT_TX for which interrupt(s) to disable
// Returns: nothing
void MultiSerial::disableInterrupt(byte interruptTypes) {
  byte curIntFlags = msReadRegister(IER) & (INT_RX | INT_TX);
  msWriteRegister(IER, (interruptTypes ^ curIntFlags) & (INT_RX | INT_TX));
}

// Private Methods /////////////////////////////////////////////////////////////
// Functions only available to other functions in this library

// write a value to one of the controller chip's registers
void MultiSerial::msWriteRegister(byte reg, byte val) {
	// take the chip select low to select the device:
	  digitalWrite(selectPin, LOW);
	  int subAddr = 0;
	  subAddr |= (port << 1);
	  subAddr |= (reg << 3);
	  SPI.transfer(subAddr); //Send register location
	  SPI.transfer(val);  //Send value to record into register
	  delay(2);
	  // take the chip select high to de-select:
	  digitalWrite(selectPin, HIGH);

}

// write a series of values to sequential addresses in the the controller chip's registers
void MultiSerial::msWriteRegister(byte reg, byte val[], byte nbrValues) {
  byte i;
  // take the chip select low to select the device:
  	  digitalWrite(selectPin, LOW);

  	  int subAddr = 0;
  	  subAddr |= (port << 1);
  	  subAddr |= (reg << 3); //Send register location
  	  // send the actual data we want to write and commit the transaction
  	  for(i=0; i<nbrValues; i++) {
  		SPI.transfer(val[i]);
  	  }//Send value to record into register
  	  delay(2);
  	  // take the chip select high to de-select:
  	  digitalWrite(selectPin, HIGH);
}

// read a value from one of the controller chip's registers and return it
byte MultiSerial::msReadRegister(byte reg) {
	// take the chip select low to select the device:
	byte readback = 0;
	  digitalWrite(selectPin, LOW);
	  delay(2);
	  readback |= (port << 1);
	  readback |= (reg << 3);
	  readback = readback | Read;
	  SPI.transfer(readback);
	  //msSendSubAddr(reg);
	  readback = SPI.transfer(0);  //Get Value from register
	  delay(2);
	  digitalWrite(selectPin, HIGH);
	  return readback;

}


// Documentation ///////////////////////////////////////////////////////////////

// Description: Outputs data to the serial port, converting numbers to their human-readable ASCII representations.
// Syntax: MultiSerialInstance.print(value)
// Syntax: MultiSerialInstance.print(value, base)
// Syntax: MultiSerialInstance.print(string)
// Parameter: value - Variable or literal to output
// Parameter: base - Base to display number in (OCT, DEC, BIN, HEX) or number of decimal places for floating point values, DEC assumed if not supplied
// Parameter: string - String to output
// Returns: nothing
// void MultiSerial::print(int value, byte base) {}

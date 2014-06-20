/*
  MultiSerial.h - Arduino library for the StrichLabs MultiSerial Shield - description
  Copyright (c) 2010 Sarah Nordstrom.  Licensed under Creative Commons BY-NC-SA.
*/

// ensure this library description is only included once
#ifndef MultiSerial_h
#define MultiSerial_h

#if ARDUINO < 100
  #error The MultiSerial library requires Arduino 1.0 or later.  Please upgrade your development environment or use an older revision of the MultiSerial library.
#endif // ARDUINO < 100

// include core Arduno functionality
#include "Arduino.h"
// include print support
#include "Stream.h"
#include "SPI.h"
//#include "FreeBoardConstants.h"

// General register set
#define RHR       0x00    // Receive Holding Register - R
#define THR       0x00    // Transmit Holding Register - W
#define IER       0x01    // Interrupt Enable Register - W
#define FCR       0x02    // FIFO Control Register - W
#define IIR       0x02    // Interrupt Identification Register - R
#define LCR       0x03    // Line Control Register - R/W
#define MCR       0x04    // Modem Control Register - R/W
#define LSR       0x05    // Line Status Register - R
#define MSR       0x06    // Modem Status Register - R
#define SPR       0x07    // Scratchpad Register - R/W
#define TCR       0x06    // Transmission Control Register- R/W
#define TLR       0x07    // Trigger Level Register- R/W
#define TXLVL     0x08    // Transmitter FIFO Level Register - R
#define RXLVL     0x09    // Receiver FIFO Level Register- R
#define IODir     0x0A    // GPIO Direction Register - R/W
#define IOState   0x0B    // GPIO State Register - R/W
#define IOIntEna  0x0C    // GPIO Interrupt Enable Register - R/W
#define IOControl 0x0E    // GPIO Control Register - R/W
#define EFCR      0x0F    // Extra Features Control Register- R/W

// Special register set
// if LCR[7] == 1 and LCR != 0xBF, these registers are accessed
#define DLL       0x00     // BRG Divisor Low Byte Register - R/W
#define DLH       0x01     // BRG Divisor Low Byte Register - R/W

// Enhanced register set
// if LCR = 0xBF, these registers are accessed
#define EFR       0x02    // Enhanced Features Register - R/W
#define XON1      0x04    // XON1 Word - R/W
#define XON2      0x05    // XON2 Word - R/W
#define XOFF1     0x06    // XOFF1 Word - R/W
#define XOFF2     0x07    // XOFF1 Word - R/W

// Interrupt types for enableInterrupt/disableInterrupt
#define INT_RX    0x01    // Receive Holding Register
#define INT_TX    0x02    // Transmit Holding Register

// Number of stop bits
#define STOPBITS1   0x00  // 1 stop bit
#define STOPBITS15  0x01  // 1.5 stop bits  (if 5-bit word size)
#define STOPBITS2   0x01  // 2 stop bits    (if not 5-bit word size)

// library interface description
class MultiSerial : public Stream {
  // user-accessible "public" interface
  public:
    MultiSerial(byte, byte);
    MultiSerial(void);
    void begin(unsigned long);
    void begin(unsigned long, unsigned long);
    virtual size_t write(uint8_t);
    virtual void write(uint8_t[], uint8_t);
    virtual int read(void);
    virtual int available(void);
    virtual void flush(void);
    virtual int peek(void);
    void store(byte);
    byte retrieve(void);
    void setWordSize(byte);
    void msPinMode(byte, byte);
    void msDigitalWrite(byte, byte);
    byte msDigitalRead(byte);
    void enableInterrupt(byte);
    void disableInterrupt(byte);
    void setStopBits(byte);
    using Print::write; // pull in write(str) and write(buf, size) from Print

  // library-accessible "private" interface
  private:
    byte selectPin;
    byte port;
    //byte peek_buf[2];
    //byte peek_buf_valid[2];
    void msWriteRegister(byte, byte);
    void msWriteRegister(byte, byte[], byte);
    byte msReadRegister(byte);
};

#endif

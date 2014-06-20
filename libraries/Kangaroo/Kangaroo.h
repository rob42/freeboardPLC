/*
Arduino Library for Kangaroo
Copyright (c) 2013-2014 Dimension Engineering LLC
http://www.dimensionengineering.com/kangaroo

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef Kangaroo_h
#define Kangaroo_h

/*!
\file Kangaroo.h
Include this file to use the Kangaroo Arduino library.
*/

#if defined(ARDUINO) && ARDUINO < 100
#error "This library requires Arduino 1.0 or newer."
#endif

#include <Arduino.h>
#define KANGAROO_BIT_PACKED_MAX_VALUE           536870911
#define KANGAROO_COMMAND_MAX_DATA_LENGTH        27
#define KANGAROO_COMMAND_MAX_BUFFER_LENGTH      32
#define KANGAROO_CRC_GOOD_VALUE                 0x307e
#define KANGAROO_DEFAULT_ADDRESS                128                       //!< The default Packet Serial address for Kangaroo is 128.
#define KANGAROO_DEFAULT_COMMAND_RETRY_INTERVAL 100                       //!< By default, commands retry after 100 milliseconds if no response is received.
#define KANGAROO_DEFAULT_COMMAND_TIMEOUT        KANGAROO_INFINITE_TIMEOUT //!< By default, commands are retried forever.
#define KANGAROO_INFINITE_TIMEOUT              -1                         //!< This value, passed as a timeout, disables the timeout.
#define KANGAROO_UNSPECIFIED_LIMIT             -1                         //!< This value, passed as a limit, leaves the limit unspecified.

class Kangaroo;
class KangarooChannel;
class KangarooMonitor;

/*!
\enum KangarooError
Errors that can occur in response to status queries.
*/
enum KangarooError
{
  // Negative values are responses from the library, though they may *correspond* to remote states.
  // Positive values are responses from the Kangaroo.
  KANGAROO_NO_ERROR          =  0x00, //!< No error occurred.
  KANGAROO_NOT_STARTED       =  0x01, //!< The channel is not started. Call KangarooChannel::start() on the channel.
  KANGAROO_NOT_HOMED         =  0x02, //!< The channel needs to be homed. Call KangarooChannel::home() on the channel.
  KANGAROO_CONTROL_ERROR     =  0x03, //!< A control error has occurred. Call KangarooChannel::start() on the channel to clear the control error.
  KANGAROO_WRONG_MODE        =  0x04, //!< The controller is in the wrong mode. For example, the DIP switches may be in Mixed Mode while the tune was done in Independent Mode.
  KANGAROO_UNRECOGNIZED_CODE =  0x05,
  KANGAROO_SERIAL_TIMEOUT    =  0x06, //!< A serial timeout occurred, or the TX line was disconnected. Call KangarooChannel::start() on the channel to clear the serial timeout.
  KANGAROO_INVALID_STATUS    = -0x01,
  KANGAROO_TIMED_OUT         = -0x02, //!< The requested timeout expired.
  KANGAROO_PORT_NOT_OPEN     = -0x03 // The serial port is not open. (Not returned by the Arduino library at present.)
};

/*!
\enum KangarooGetType
The possible 'get' request types.
*/
enum KangarooGetType
{
  KANGAROO_GETP   = 0x01, //!< Absolute position.
  KANGAROO_GETPI  = 0x41, //!< Incremental position (relative to the position when the last command was issued).
  KANGAROO_GETS   = 0x02, //!< Absolute velocity.
  KANGAROO_GETSI  = 0x42, //!< Incremental velocity (relative to the speed when the last command was issued).
  KANGAROO_GETMIN = 0x08, //!< The minimum position. This corresponds to DEScribe's Nominal Travel minimum.
  KANGAROO_GETMAX = 0x09  //!< The maximum position. This corresponds to DEScribe's Nominal Travel maximum.
};

/*!
\enum KangarooGetFlags
Flags that modify the behavior of a 'get' request.
*/
enum KangarooGetFlags
{
  KANGAROO_GET_DEFAULT       = 0x00, //!< Normal behavior.
  KANGAROO_GET_ECHO_CODE     = 0x10,
  KANGAROO_GET_RAW_UNITS     = 0x20, //!< Use raw units. For analog, raw units are millivolts. For quadrature, 4 raw units equal 1 line.
  KANGAROO_GET_SEQUENCE_CODE = 0x40
};

/*!
\enum KangarooMoveFlags
Flags that modify the behavior of a motion request.
*/
enum KangarooMoveFlags
{
  KANGAROO_MOVE_DEFAULT           = 0x00, //!< Normal behavior.
  KANGAROO_MOVE_NO_DEFAULT_LIMITS = 0x08, //!< Do not apply the speed limit and speed ramping source settings. By default, the speed limit comes from Kangaroo's potentiometers.
  KANGAROO_MOVE_RAW_UNITS         = 0x20, //!< Use raw units. For analog, raw units are millivolts. For quadrature, 4 raw units equal 1 line.
  KANGAROO_MOVE_SEQUENCE_CODE     = 0x40
};

/*!
\enum KangarooStatusFlags
Flags that provide details about a query response.
*/
enum KangarooStatusFlags
{
  KANGAROO_STATUS_NONE          = 0x00, //!< The response is normal.
  KANGAROO_STATUS_ERROR         = 0x01, //!< The response is an error.
  KANGAROO_STATUS_BUSY          = 0x02, //!< For a motion, this means the motion is not finished. For an error, this means the error (such as KANGAROO_NOT_HOMED) should self-clear. This corresponds to a lowercase letter in Simplified Serial.
  KANGAROO_STATUS_ECHO_CODE     = 0x10,
  KANGAROO_STATUS_RAW_UNITS     = 0x20, //!< The value in the response is in raw units. For analog, raw units are millivolts. For quadrature, 4 raw units equal 1 line.
  KANGAROO_STATUS_SEQUENCE_CODE = 0x40
};

enum KangarooCommand
{
  KANGAROO_CMD_START  = 0x20,
  KANGAROO_CMD_UNITS  = 0x21,
  KANGAROO_CMD_HOME   = 0x22,
  KANGAROO_CMD_STATUS = 0x23,
  KANGAROO_CMD_MOVE   = 0x24,
  KANGAROO_CMD_SYSTEM = 0x25
};

/*!
\enum KangarooSystemCommand
Advanced commands that have special effects on the Kangaroo motion controller.
*/
enum KangarooSystemCommand
{
  KANGAROO_SYS_POWER_DOWN                 = 0x00, //!< Powers down the channel.
  KANGAROO_SYS_POWER_DOWN_ALL             = 0x01, //!< Powers down all channels of the controller that receives the command.
  KANGAROO_SYS_TUNE_ENTER_MODE            = 0x03, /*!< Enters tune mode 1 (Teach), 2 (Limit Switches), or 3 (Mechanical Stops).
                                                       This corresponds to pressing the tune button to get to the mode you want.
                                                       
                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_TUNE_SET_DISABLED_CHANNELS = 0x08, /*!< Initially all channels are disabled for safety reasons after entering a tune mode.
                                                       You must send this bitmask before beginning the tune. 0 enables all channels.

                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_TUNE_CONTROL_OPEN_LOOP     = 0x06, /*!< Sets the open loop power. This can be used to position for a Teach tune.
                                                       The range is -(2^28-1) to 2^28-1.
                                                   
                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_TUNE_GO                    = 0x04, /*!< Begins the tune.
                                                       This corresponds to pressing the tune button after you are in the desired mode.

                                                       Tuning has an automatic serial timeout for safety reasons.
                                                       You must continually send packets or it will abort. KangarooChannel::getP() in a loop does the job.

                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_TUNE_ABORT                 = 0x05, /*!< Aborts the tune.
  
                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_SET_BAUD_RATE              = 0x20, /*!< Sets the baud rate.
  
                                                       Do not expect a reply from this command. */
  KANGAROO_SYS_SET_SERIAL_TIMEOUT         = 0x21  //!< Sets the serial timeout.
};

enum KangarooReplyCode
{
  KANGAROO_RC_STATUS = 0x43
};

class KangarooCRC
{
public:
  void begin();
  void write(byte data);
  void write(const byte* data, size_t lengthOfData);
  void end  ();
  
public:
  inline uint16_t value() const       { return _crc; }
         void     value(uint16_t crc) { _crc = crc;  }
  
  static uint16_t value(const byte* data, size_t lengthOfData);
  
private:
  uint16_t _crc;
};

class KangarooCommandWriter
{
public:
  KangarooCommandWriter();
  
public:
  inline const byte* data   () const { return _data;   }
  inline       size_t length () const { return _length; }
  
public:
  void write               (      byte    data                );
  void write               (const byte*   data, size_t length);
  void writeBitPackedNumber(      int32_t number              );
  
public:
         size_t writeToBuffer(byte* buffer, byte address, KangarooCommand command) const;
  static size_t writeToBuffer(byte* buffer, byte address, KangarooCommand command, const byte* data, size_t lengthOfData);
         void   writeToStream(Stream& port, byte address, KangarooCommand command) const;
  static void   writeToStream(Stream& port, byte address, KangarooCommand command, const byte* data, size_t lengthOfData);
  
private:
  byte _address, _command; size_t _length;
  byte _data[KANGAROO_COMMAND_MAX_DATA_LENGTH];
};

class KangarooReplyReceiver
{
public:
  KangarooReplyReceiver();
  
public:
  inline       byte              address() const { return  _data[0];                    }
  inline       KangarooReplyCode command() const { return  (KangarooReplyCode)_data[1]; }
  inline const byte*             data   () const { return &_data[3];                    }
  inline       size_t            length () const { return  _length - 5;                 }
  
public:
  inline boolean ready() const { return _ready; }
         void    read (byte data);
         void    reset();
  
private:
  size_t _length; boolean _ready;
  byte _data[KANGAROO_COMMAND_MAX_BUFFER_LENGTH];
};

class KangarooReplyReader
{
public:
  KangarooReplyReader(const byte* data, size_t length);
  
public:
  boolean canRead               () const;
  boolean tryRead               (byte* value);
  byte       read               ();
  int32_t    readBitPackedNumber();
  
private:
  const byte* _data;
  const byte* _dataEnd;
};

/*!
\class KangarooStatus
\brief Stores the response to a status request.

Returned by KangarooChannel::getP(), KangarooMonitor::status(), and others.
*/
class KangarooStatus
{
public:
  KangarooStatus();
  KangarooStatus(const byte* data, size_t length);
  
public:
  static KangarooStatus createInvalidStatus();
  static KangarooStatus createTimedOut     ();
  
public:
  /*!
  Gets the channel name.
  \return The channel name.
  */
  inline char channel() const { return (char)_channel; }
  
  /*!
  Gets the status flags associated with this response.
  \return The status flags.
  */
  inline KangarooStatusFlags flags() const { return (KangarooStatusFlags)_flags; }
  
  /*!
  Gets the type of the request.
  \return The type of the request.
  */
  inline KangarooGetType type() const { return (KangarooGetType)_type; }
  
  /*!
  Gets the value associated with this response.
  \return The value.
  */
  inline int32_t value() const { return _value; }
  
  inline byte    echoCode    () const { return _echoCode;     }
  inline byte    sequenceCode() const { return _sequenceCode; }
  inline boolean valid       () const { return _valid;        }
  
public:
  /*!
  Gets whether a recently-requested command is still executing.
  For example, KangarooChannel::p() is busy while the channel is getting into position, and KangarooChannel::home() is busy while the channel is homing.
  \return True if the channel is busy.
  */
  boolean busy() const { return 0 != (flags() & KANGAROO_STATUS_BUSY ); }
  
  /*!
  If the response is an error, gets the error.
  \return The error, or KANGAROO_NO_ERROR if the response is not an error.
  */
  KangarooError error() const { return (KangarooError)(0 != (flags() & KANGAROO_STATUS_ERROR) ? value() : 0); }
  
  /*!
  Gets whether a recently-requested command has completed execution.
  For example, KangarooChannel::p() is done when the channel is in position.
  \return True if the command is done.
  */
  boolean done() const { return !busy (); }
  
  /*!
  Gets whether the response is not an error and KangarooStatus::value() is the requested value.
  \return True if the response is not an error.
  */
  boolean ok() const { return !error(); }
  
  /*!
  Gets whether a timeout has occurred.
  \return True if the KangarooStatus::error() equals KANGARO_TIMED_OUT.
  */
  boolean timedOut() const { return error() == KANGAROO_TIMED_OUT; }

private:
  static KangarooStatus createFromError(KangarooError error);
  
private:
  void    init ();
  boolean parse(const byte* data, size_t length);
  
private:
  boolean _valid;
  byte    _channel, _flags, _echoCode, _sequenceCode, _type;
  int32_t _value;
};

/*!
\class KangarooSerial
\brief Create a KangarooSerial for the serial port you are using, and then attach a KangarooChannel for each channel you want to communicate with.
*/
class KangarooSerial
{
  friend class KangarooChannel;
  
public:
  /*!
  Constructs a KangarooSerial object.
  \param port The serial port the motion controller is on.
  */
  KangarooSerial(Stream& port);
  
public:
  /*!
  Gets the serial port being used.
  \return The serial port.
  */
  inline Stream& port() { return _port; }

private:
  boolean tryReceivePacket();
  
private:
  KangarooSerial  (KangarooSerial& serial); // no copy
  void operator = (KangarooSerial& serial);

private:
  KangarooReplyReceiver _receiver;
  Stream&               _port;
};

/*!
\class KangarooTimeout
\brief Encapsulates a starting time and duration.

The same KangarooTimeout can be reused between different calls.
*/
class KangarooTimeout
{
public:
  /*!
  Constructs a KangarooTimeout object. The starting time is captured.
  \param timeoutMS The timeout duration, in milliseconds.
  */
  KangarooTimeout(int32_t timeoutMS);
  
public:
  /*!
  Gets whether the timeout can expire.
  If the KangarooTimeout was created with a timeout of KANGAROO_INFINITE_TIMEOUT, it cannot expire.
  \return True if the timeout can expire.
  */
  boolean canExpire() const;
  
  /*!
  Gets whether the timeout has expired.
  \return True if the timeout has expired.
  */
  boolean expired() const;
  
  /*!
  Causes the timeout to expire immediately.
  */
  void expire();
  
  /*!
  Captures the current time and uses it as the new starting time for the timeout.
  */
  void reset();
  
private:
  uint32_t _start;
  int32_t  _timeoutMS;
};

/*!
\class KangarooChannel
\brief Lets you send commands to and get position, speed, etc. for a specific channel of the Kangaroo.

Any number of KangarooChannel objects can be associated with a single KangarooSerial object.
*/
class KangarooChannel
{
  friend class Kangaroo;
  friend class KangarooMonitor;
  
public:
  /*!
  Constructs a KangarooChannel object.
  \param serial The KangarooSerial whose serial port the controller is on.
  \param name The name of the channel (a single character).
              By default, mixed mode channels are 'D' (Drive) and 'T' (Turn). Independent mode channels are '1' and '2'.
              You can change these in DEScribe. Mainly this is useful if you have multiple Kangaroos daisy-chained and have
              'Enable multi-Kangaroo mode (shared signal lines).' checked, or if your particular application would be clearer
              if for instance the channels were named 'X', 'Y', and 'Z'.
  \param address The Packet Serial address of the controller.
                 Normally this can be left at its default. If you daisy-chain the TX/S1 line between your Kangaroo and a 
                 Sabertooth or SyRen motor driver set in Packet Serial, however, make sure they aren't both using the same address.
  */
   KangarooChannel(KangarooSerial& serial, char name, byte address = KANGAROO_DEFAULT_ADDRESS);
  ~KangarooChannel();
  
public:
  /*!
  Gets the channel name.
  \return The channel name.
  */
  inline char name() const { return _name; }
  
  /*!
  Gets the Packet Serial address of the controller.
  \return The packet serial address of the controller.
  */
  inline byte address() const { return _address; }

public:
  /*!
  Starts the channel. Also, the Kangaroo LED will shine brightly for a third of a second.
  \return A KangarooError.
          Most commonly, this will be KANGAROO_NO_ERROR if the channel does not require homing, or KANGAROO_NOT_HOMED if it does.
  */
  KangarooError start();
  
  /*!
  Sets custom units for the channel.
  This command may be called after you start the channel but before you home it.
  If you do not set custom units, the units you have set up in DEScribe will be used.
  If you haven't set any, machine units will be used.
  \param desiredUnits The amount in your units that correspond to the specified amount of machine units.
  \param machineUnits The amount of machine units (millivolts or lines) corresponding to the specified amount in your units.
  \return A KangarooError.
          Most commonly, this will be KANGAROO_NO_ERROR if the channel does not require homing, or KANGAROO_NOT_HOMED if it does.
  */
  KangarooError units(int32_t desiredUnits, int32_t machineUnits);
  
  /*!
  Homes the channel.
  \return A KangarooMonitor for tracking the homing request.
          The easiest way to use the KangarooMonitor here is to call KangarooMonitor::wait().
  */
  KangarooMonitor home();
   
  /*!
  Moves to the specified absolute position.
  This command is most useful for absolutely-positioned systems.
  \param position   The position to move to.
  \param speedLimit The speed limit for the move.
  \param flags      Modifiers for the move command.
  \return A KangarooMonitor for tracking the move request.
  */
  KangarooMonitor p(int32_t position, int32_t speedLimit = KANGAROO_UNSPECIFIED_LIMIT, KangarooMoveFlags flags = KANGAROO_MOVE_DEFAULT);
  
  /*!
  Makes an incremental move.
  This command is useful any time you want to make a motion relative to where you are right now.
  Rovers are a case where this is almost always what you want.
  \param positionIncrement The amount to increment the current position by.
  \param speedLimit        The speed limit for the move.
  \param flags             Modifiers for the move command.
  \return A KangarooMonitor for tracking the move request.
  */
  KangarooMonitor pi(int32_t positionIncrement, int32_t speedLimit = KANGAROO_UNSPECIFIED_LIMIT, KangarooMoveFlags flags = KANGAROO_MOVE_DEFAULT);
  
  /*!
  Moves at a particular speed.
  \param velocity  The speed to move at.
  \param rampLimit The max rate at which to change the commanded speed.
  \param flags     Modifiers for the move command.
  \return A KangarooMonitor for tracking the move request.
  */
  KangarooMonitor s(int32_t velocity, int32_t rampLimit  = KANGAROO_UNSPECIFIED_LIMIT, KangarooMoveFlags flags = KANGAROO_MOVE_DEFAULT);

  /*!
  Moves at a particular speed, incremental from the current speed.
  This is useful for cruise control: a negative increment will slow down,
                                     a positive increment will speed up,
                                     and a zero increment will hold the current speed.
  \param velocityIncrement The amount to increment the current speed by.
  \param rampLimit         The max rate at which to change the commanded speed.
  \param flags             Modifiers for the move command.
  \return A KangarooMonitor for tracking the move request.
  */
  KangarooMonitor si(int32_t velocityIncrement, int32_t rampLimit  = KANGAROO_UNSPECIFIED_LIMIT, KangarooMoveFlags flags = KANGAROO_MOVE_DEFAULT);

public:
  /*!
  Issues a 'get' request.
  \param type The type of the 'get' request.
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the response.
  */
  KangarooStatus get(KangarooGetType type, KangarooGetFlags flags = KANGAROO_GET_DEFAULT);
  
  /*!
  Gets the absolute position.
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the position.
  */
  inline KangarooStatus getP(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETP, flags);
  }

  /*!
  Gets the incremental position (relative to the position when the last command was issued).
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the position.
  */  
  inline KangarooStatus getPI(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETPI, flags);
  }
  
  /*!
  Gets the absolute speed (positive or negative depending on direction).
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the speed.
  */
  inline KangarooStatus getS(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETS, flags);
  }
  
  /*!
  Gets the incremental speed (relative to the speed when the last command was issued).
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the speed.
  */
  inline KangarooStatus getSI(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETSI, flags);
  }
  
  /*!
  Gets the minimum position. This corresponds to DEScribe's Nominal Travel minimum.
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the limit.
  */
  inline KangarooStatus getMin(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETMIN, flags);
  }

  /*!
  Gets the maximum position. This corresponds to DEScribe's Nominal Travel maximum.
  \param flags Flags modifying the 'get' request.
  \return A KangarooStatus object describing the limit.
  */
  inline KangarooStatus getMax(KangarooGetFlags flags = KANGAROO_GET_DEFAULT)
  {
    return get(KANGAROO_GETMAX, flags);
  }
  
public:  
  // While the following calls violate the standard Arduino casing, they sure are nicer to type.
  // The contrast between 'p' and 'getP' may confuse as well, so we include these to make life easier for everyone.
  inline KangarooStatus getp  (KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getP  (flags); }
  inline KangarooStatus getpi (KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getPI (flags); }
  inline KangarooStatus gets  (KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getS  (flags); }  
  inline KangarooStatus getsi (KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getSI (flags); }
  inline KangarooStatus getmin(KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getMin(flags); }
  inline KangarooStatus getmax(KangarooGetFlags flags = KANGAROO_GET_DEFAULT) { return getMax(flags); }

public:
  /*! Gets the command retry interval.
  \return The command retry interval, in milliseconds.
  */
  inline int32_t commandRetryInterval() const { return _commandRetryInterval; }
  
  /*! Sets the command retry interval.
  \param intervalMS The command retry interval, in milliseconds.
  */
  inline void commandRetryInterval(int32_t intervalMS) { _commandRetryInterval = intervalMS; }
  
  /*! Gets the command timeout.
  \return The command timeout, in milliseconds.
  */
  inline int32_t commandTimeout() const { return _commandTimeout; }
  
  /*! Sets the command timeout.
  \param timeoutMS The command timeout, in milliseconds.
  */
  inline void commandTimeout(int32_t timeoutMS) { _commandTimeout = timeoutMS; }
  
  /*! Gets whether streaming is enabled.
  \return True if streaming is enabled.
  */
  inline boolean streaming() const { return _streaming; }
  
  /*!
  Enables or disables streaming.
  When streaming is disabled (the default), commands are delivered reliably:
  a request is sent to the Kangaroo after every command to make sure the command was received.
  When streaming is enabled, commands are delivered unreliably:
  no effort is made to verify that they are received.

  If you are constantly sending position or speed commands, you may not care if an intermediate one is lost.
  Enabling streaming can more than double your command rate, at the cost of reduced reliability.
  If you only have a transmit line and no way to receive data back from the Kangaroo, you should enable streaming.
  KangarooMonitor::wait() cannot be meaningfully called for a command that was streamed.
  */
  inline void streaming(boolean enabled) { _streaming = enabled; }
  
  /*!
  Sets the baud rate.
  This affects all channels of the controller that receives the command.
  \param baudRate The baud rate to use. Supported rates are 9600, 19200, 38400, and 115200.
  */
  void baudRate(int32_t baudRate);
  
  /*!
  Powers down the channel.
  \return A KangarooError, usually KANGAROO_NO_ERROR.
  */
  KangarooError powerDown();
  
  /*!
  Powers down all channels of the controller that receives the command.
  \return A KangarooError, usually KANGAROO_NO_ERROR.
  */
  KangarooError powerDownAll();
  
  /*!
  Sets the serial timeout.
  This affects all channels of the controller that receives the command.
  \param milliseconds The timeout, in milliseconds. A value of zero uses the DEScribe setting. KANGAROO_INFINITE_TIMEOUT disables the timeout.
  \return A KangarooError, usually KANGAROO_NO_ERROR.
  */
  KangarooError serialTimeout(int32_t milliseconds);
  
  /*!
  Sends a system command. These are mostly for advanced use cases.
  \param systemCommand The command to set.
  \param expectReply   Whether or not to expect a reply.
                       If a command has an unusual effect on the state of the Kangaroo, expecting a reply may
                       cause the command to retry until it times out.
  \param values        The parameters of the command.
  \param valueCount    The number of parameters.
  \return A KangarooError.
  */
  KangarooError systemCommand(KangarooSystemCommand systemCommand, boolean expectReply, int32_t values[], size_t valueCount);

private:
  KangarooStatus  getSpecial(KangarooGetType type, KangarooGetFlags flags, const KangarooTimeout& timeout);
  KangarooMonitor motion    (byte motionType, int32_t motionValue, byte limitType, int32_t limitValue, KangarooMoveFlags flags);
  KangarooMonitor set       (KangarooCommand        command,
                             KangarooCommandWriter& contents,
                             KangarooMoveFlags      moveFlags = KANGAROO_MOVE_DEFAULT,
                             KangarooGetType        getType   = KANGAROO_GETP);
  void setNoReply(KangarooCommand command, KangarooCommandWriter& contents,
                  KangarooMoveFlags moveFlags = KANGAROO_MOVE_DEFAULT);
                             
  boolean getInitialSequenceCodeIfNecessary(const KangarooTimeout& timeout, KangarooStatus& status);
  boolean updateMonitoredResult(const KangarooTimeout& timeout, boolean acceptRepliesWithStartupSequenceCode);

private:
  KangarooChannel (KangarooChannel& channel); // no copy
  void operator = (KangarooChannel& channel);
  
private:
  KangarooSerial&  _serial;
  char             _name;
  byte             _address;
  int32_t          _commandRetryInterval;
  int32_t          _commandTimeout;
  byte             _echoCode;
  uint32_t         _monitorCode;
  KangarooGetType  _monitoredGetType;
  KangarooGetFlags _monitoredGetFlags;
  KangarooStatus   _monitoredGetResult;
  byte             _monitoredSequenceCode;
  boolean          _monitoredSequenceCodeIsReady;
  boolean          _streaming;
};

/*!
\class KangarooMonitor
\brief Makes it easy to track the status of position commands, speed commands, and any other commands that take time to complete.
*/
class KangarooMonitor
{
  friend class KangarooChannel;

public:
  /*!
  Gets the most recently received status.
  \return A KangarooStatus.
  */
  KangarooStatus status() const;
  
  /*!
  Gets whether the status is valid.
  If you keep an old KangarooMonitor object around and issue a new command on the same channel, the old KangarooMonitor will become invalid.
  \return True if the status is valid.
  */
  boolean valid() const;

  /*!
  Updates the status. The channel's command timeout is used.
  \return The same KangarooMonitor.
  */
  KangarooMonitor update();
  
  /*!
  Updates the status.
  \param timeoutMS The timeout to use, in milliseconds.
  \return The same KangarooMonitor.
  */
  KangarooMonitor update(int32_t timeoutMS);
  
  /*!
  Updates the status.
  \param timeout The timeout object to use.
  \return The same KangarooMonitor.
  */
  KangarooMonitor update(const KangarooTimeout& timeout);
  
  /*!
  Waits until KangarooStatus::done() returns true or the timeout expires.
  \param timeoutMS The timeout to use, in milliseconds.
  \return The same KangarooMonitor.
  */
  KangarooMonitor wait(int32_t timeoutMS = KANGAROO_INFINITE_TIMEOUT);
  
  /*!
  Waits until KangarooStatus::done() returns true or the timeout object expires.
  \param timeout The timeout object to use.
  \return The same KangarooMonitor.
  */
  KangarooMonitor wait(const KangarooTimeout& timeout);
  
private:
  KangarooMonitor(KangarooChannel* channel, uint32_t monitorCode);
  
private:
  struct
  {
    KangarooChannel* channel;
    uint32_t         monitorCode;
  } _state;
};

/*!
Waits until all of the monitors are done or the timeout expires.
\param count     The number of monitors.
\param monitors  The monitors to wait for.
\param timeoutMS The timeout to use, in milliseconds.
\return True if all of the monitors are done, or false if the timeout expired.
\remark If a monitor is null, it is ignored.
*/
boolean waitAll(size_t count, KangarooMonitor* monitors[], int32_t timeoutMS = KANGAROO_INFINITE_TIMEOUT);

/*!
Waits until all of the monitors are done or the timeout object expires.
\param count    The number of monitors.
\param monitors The monitors to wait for.
\param timeout  The timeout object to use.
\return True if all of the monitors are done, or false if the timeout expired.
\remark If a monitor is null, it is ignored.
*/
boolean waitAll(size_t count, KangarooMonitor* monitors[], const KangarooTimeout& timeout);

/*!
Waits until at least one of the monitors is done or the timeout expires.
\param count     The number of monitors.
\param monitors  The monitors to wait for.
\param timeoutMS The timeout to use, in milliseconds.
\return The array index of the monitor that is done, or -1 if the timeout expired.
\remark If a monitor is null, it is ignored.
*/
int waitAny(size_t count, KangarooMonitor* monitors[], int32_t timeoutMS = KANGAROO_INFINITE_TIMEOUT);

/*!
Waits until at least one of the monitors is done or the timeout expires.
\param count    The number of monitors.
\param monitors The monitors to wait for.
\param timeout  The timeout object to use.
\return The array index of the monitor that is done, or -1 if the timeout expired.
\remark If a monitor is null, it is ignored.
*/
int waitAny(size_t count, KangarooMonitor* monitors[], const KangarooTimeout& timeout);
  
#endif

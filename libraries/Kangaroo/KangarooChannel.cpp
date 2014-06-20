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

#include "Kangaroo.h"

static byte nextCode(byte code)
{
  if (++code >= 0x80) { code = 1; } // The device will start up with code 0, so we do this to distinguish that case.
  return code;
}

KangarooChannel::KangarooChannel(KangarooSerial& serial, char name, byte address)
  : _serial(serial), _name(), _address(address), _echoCode(), _monitorCode(), _monitoredSequenceCode(), _monitoredSequenceCodeIsReady()
{
  // Simplified Serial autocapitalizes, but Packet Serial does not. So, we do it here.
  // If the compiler does LTO this should constant fold to use no code space.
  if (name >= 'a' && name <= 'z')
  {
    name += 'A' - 'a';
  }
  
  if ((name >= 'A' && name <= 'Z') ||
      (name >= '0' && name <= '9'))
  {
    _name = name;
  }
  
  commandRetryInterval(KANGAROO_DEFAULT_COMMAND_RETRY_INTERVAL);
  commandTimeout      (KANGAROO_DEFAULT_COMMAND_TIMEOUT       );
  
  streaming(false);
}

KangarooChannel::~KangarooChannel()
{

}

KangarooError KangarooChannel::start()
{
  KangarooCommandWriter contents;
  return set(KANGAROO_CMD_START, contents).status().error();
}

KangarooError KangarooChannel::units(int32_t desiredUnits, int32_t machineUnits)
{
  KangarooCommandWriter contents;
  contents.writeBitPackedNumber(desiredUnits);
  contents.writeBitPackedNumber(machineUnits);
  return set(KANGAROO_CMD_UNITS, contents).status().error();
}

KangarooMonitor KangarooChannel::home()
{
  KangarooCommandWriter contents;
  return set(KANGAROO_CMD_HOME, contents);
}

KangarooMonitor KangarooChannel::p(int32_t position, int32_t speedLimit, KangarooMoveFlags flags)
{
  return motion(0x01, position, 0x02, speedLimit, flags);
}

KangarooMonitor KangarooChannel::pi(int32_t positionIncrement, int32_t speedLimit, KangarooMoveFlags flags)
{
  return motion(0x41, positionIncrement, 0x02, speedLimit, flags);
}

KangarooMonitor KangarooChannel::s(int32_t velocity, int32_t rampLimit, KangarooMoveFlags flags)
{
  return motion(0x02, velocity, 0x03, rampLimit, flags);
}

KangarooMonitor KangarooChannel::si(int32_t velocityIncrement, int32_t rampLimit, KangarooMoveFlags flags)
{
  return motion(0x42, velocityIncrement, 0x03, rampLimit, flags);
}

KangarooMonitor KangarooChannel::motion(byte motionType, int32_t motionValue, byte limitType, int32_t limitValue,
                                        KangarooMoveFlags flags)
{
  KangarooCommandWriter contents;
  
  contents.write(motionType);
  contents.writeBitPackedNumber(motionValue);
  
  if (limitValue >= 0)
  {
    contents.write(limitType);
    contents.writeBitPackedNumber(limitValue);
  }
  
  return set(KANGAROO_CMD_MOVE, contents, flags);
}

KangarooStatus KangarooChannel::get(KangarooGetType type, KangarooGetFlags flags)
{
  KangarooTimeout timeout(commandTimeout());
  
  KangarooStatus initialStatus;
  if (getInitialSequenceCodeIfNecessary(timeout, initialStatus)) { return initialStatus; }
  
  return getSpecial(type, flags, timeout);
}

void KangarooChannel::baudRate(int32_t baudRate)
{
  uint8_t index;
  switch (baudRate)
  {
  case   9600: default: index = 0; break;
  case  19200:          index = 1; break;
  case  38400:          index = 2; break;
  case 115200:          index = 3; break;
  }
  
  int32_t values[1] = { index };
  systemCommand(KANGAROO_SYS_SET_BAUD_RATE, false, values, 1);
}

KangarooError KangarooChannel::powerDown()
{
  int32_t values[0];
  return systemCommand(KANGAROO_SYS_POWER_DOWN, true, values, 0);
}

KangarooError KangarooChannel::powerDownAll()
{
  int32_t values[0];
  return systemCommand(KANGAROO_SYS_POWER_DOWN_ALL, true, values, 0);
}

KangarooError KangarooChannel::serialTimeout(int32_t milliseconds)
{
  int32_t values[] = { milliseconds < 0 ? -1 : (milliseconds * 2 + 124) / 125 }; // The command takes time in 16ths of a second. Round up.
  return systemCommand(KANGAROO_SYS_SET_SERIAL_TIMEOUT, true, values, 1);
}

KangarooError KangarooChannel::systemCommand(KangarooSystemCommand systemCommand, boolean expectReply, int32_t values[], size_t valueCount)
{
    KangarooCommandWriter contents;
    contents.write((byte)systemCommand);
    for (size_t i = 0; i < valueCount; i ++) { contents.writeBitPackedNumber(values[i]); }

    if (expectReply)
    {
      return set(KANGAROO_CMD_SYSTEM, contents).status().error();
    }
    else
    {
      setNoReply(KANGAROO_CMD_SYSTEM, contents);
      return KANGAROO_NO_ERROR;
    }
}
        
KangarooStatus KangarooChannel::getSpecial(KangarooGetType type, KangarooGetFlags flags,
                                           const KangarooTimeout& timeout)
{
  KangarooTimeout retry(commandRetryInterval()); retry.expire();
  flags = (KangarooGetFlags)(flags | KANGAROO_GET_ECHO_CODE);
  
  while (1)
  {
    if (timeout.expired())
    {
      return KangarooStatus::createTimedOut();
    }
    
    if (retry.expired())
    {
      retry.reset();
      _echoCode = nextCode(_echoCode);
      
      // Send the Get command.
      KangarooCommandWriter writer;
      writer.write((byte)name());
      writer.write((byte)flags);
      writer.write(_echoCode);
      writer.write((byte)type);
      
      Stream& port = _serial.port();
      writer.writeToStream(port, address(), KANGAROO_CMD_STATUS);
    }
    
    // Check for a valid reply.   
    if (!_serial.tryReceivePacket()) { continue; }

    KangarooReplyReceiver& receiver = _serial._receiver;
    if (receiver.address() != address()         ) { continue; }
    if (receiver.command() != KANGAROO_RC_STATUS) { continue; }
    
    KangarooStatus result(receiver.data(), receiver.length());
    if (!result.valid()) { continue; }
    
    if (result.echoCode() != _echoCode) { continue; }

    return result;
  }
}

void KangarooChannel::setNoReply(KangarooCommand command, KangarooCommandWriter& contents,
                                 KangarooMoveFlags moveFlags)
{
  Stream& port = _serial.port();

  KangarooCommandWriter writer;
  writer.write((byte)name());
  writer.write((byte)moveFlags);
  if (moveFlags & KANGAROO_MOVE_SEQUENCE_CODE)
  {
    writer.write(_monitoredSequenceCode);
  }
  writer.write(contents.data(), contents.length());
  
  writer.writeToStream(port, address(), command);
}

// We return a status only if there is an error.
boolean KangarooChannel::getInitialSequenceCodeIfNecessary(const KangarooTimeout& timeout, KangarooStatus& status)
{
    if (_monitoredSequenceCodeIsReady) { return false; }
    status = getSpecial(KANGAROO_GETP, KANGAROO_GET_SEQUENCE_CODE, timeout);
    if (status.error() < 0) { return true; }

    _monitoredSequenceCode = status.sequenceCode();
    _monitoredSequenceCodeIsReady = true;
    return false;
}
        
KangarooMonitor KangarooChannel::set(KangarooCommand        command,
                                     KangarooCommandWriter& contents,
                                     KangarooMoveFlags      moveFlags,
                                     KangarooGetType        getType)
{
  KangarooMonitor monitor(this, ++_monitorCode);
  
  if (streaming())
  {
    setNoReply(command, contents, moveFlags);
    
    // Motions from other commands cannot be tracked through.
  }
  else
  {
    KangarooTimeout timeout(commandTimeout());
    moveFlags = (KangarooMoveFlags)(moveFlags | KANGAROO_MOVE_SEQUENCE_CODE);

    // Invalidate the older command monitor and set up a new one.
    if (!getInitialSequenceCodeIfNecessary(timeout, _monitoredGetResult))
    {
      // If we have a proper initial sequence code, send the command.
      _monitoredGetType = getType;
      _monitoredGetFlags = (KangarooGetFlags)moveFlags;
      _monitoredSequenceCode = nextCode(_monitoredSequenceCode);

      do
      {
        setNoReply(command, contents, moveFlags);
      }
      // Make sure the packet was received correctly -- proper sequence code and the like. If not, we'll resend.
      while (!updateMonitoredResult(timeout, false));
    }
  }
  
  return monitor;
}

// Wait for a response, to ensure the packet was received at all.
// Internally generated errors (like timeouts) have negative numbers, so we pass them along.
// For any received error, we've failed to update if the sequence codes don't match.
boolean KangarooChannel::updateMonitoredResult(const KangarooTimeout& timeout,
                                               boolean acceptRepliesWithStartupSequenceCode)
{
  _monitoredGetResult = getSpecial(_monitoredGetType, _monitoredGetFlags, timeout);
  
  if (_monitoredGetResult.error() < 0)
  {
    return true;
  }
  
  if (_monitoredGetResult.sequenceCode() == _monitoredSequenceCode)
  {
    return true;
  }
  
  if (_monitoredGetResult.sequenceCode() == 0 && acceptRepliesWithStartupSequenceCode)
  {
    return true;
  }
  
  return false;
}

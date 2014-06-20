/*
Arduino Library for Kangaroo
Copyright (c) 2013 Dimension Engineering LLC
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

KangarooStatus::KangarooStatus()
{
  init();
}

KangarooStatus::KangarooStatus(const byte* data, size_t length)
{
  init();
  if (parse(data, length)) { _valid = true; } else { init(); }
}

KangarooStatus KangarooStatus::createInvalidStatus()
{
  return createFromError(KANGAROO_INVALID_STATUS);
}

KangarooStatus KangarooStatus::createTimedOut()
{
  return createFromError(KANGAROO_TIMED_OUT);
}

KangarooStatus KangarooStatus::createFromError(KangarooError error)
{
  KangarooStatus result;
  result._valid = true;
  result._flags = (byte)KANGAROO_STATUS_ERROR;
  result._value = error;
  return result;
}

void KangarooStatus::init()
{
  _valid   = false;
  _channel = _flags = _echoCode = _sequenceCode = _type = 0;
  _value   = 0;
}

boolean KangarooStatus::parse(const byte* data, size_t length)
{
  KangarooReplyReader parser(data, length);

  if (!parser.tryRead(&_channel)) { return false; }
  if (!parser.tryRead(&_flags)) { return false; }
  
  if (_flags & KANGAROO_STATUS_ECHO_CODE)
  {
    if (!parser.tryRead(&_echoCode)) { return false; }
  }
  
  if (_flags & KANGAROO_STATUS_SEQUENCE_CODE)
  {
    if (!parser.tryRead(&_sequenceCode)) { return false; }
  }
    
  if (!parser.tryRead(&_type)) { return false; }
  
  _value = parser.readBitPackedNumber();
  if (_flags & KANGAROO_STATUS_ERROR)
  {
  // Security check: Non-positive numbers should NEVER be received.
  //                 They are for library-returned errors (<0) and 'None' (=0).
    if (_value <= 0) { return false; }
  }

  return true;
}

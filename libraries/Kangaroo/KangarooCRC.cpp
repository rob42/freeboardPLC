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

void KangarooCRC::begin()
{
  _crc = 0x3fff;
}

void KangarooCRC::write(byte data)
{
  _crc ^= data & 0x7f;
  
  for (byte bit = 0; bit < 7; bit ++)
  {
    if (_crc & 1)
    {
      _crc >>= 1; _crc ^= 0x22f0;
    }
    else
    {
      _crc >>= 1;
    }
  }
}

void KangarooCRC::write(const byte* data, size_t lengthOfData)
{
  for (size_t i = 0; i < lengthOfData; i ++) { write(data[i]); }
}

void KangarooCRC::end()
{
  _crc ^= 0x3fff;
}

uint16_t KangarooCRC::value(const byte* data, size_t lengthOfData)
{
  KangarooCRC crc;
  crc.begin(); crc.write(data, lengthOfData); crc.end();
  return crc.value();
}

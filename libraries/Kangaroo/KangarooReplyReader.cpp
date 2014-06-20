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

KangarooReplyReader::KangarooReplyReader(const byte* data, size_t length)
  : _data(data), _dataEnd(data + length)
{

}

boolean KangarooReplyReader::canRead() const
{
  return _data != _dataEnd;
}

boolean KangarooReplyReader::tryRead(byte* value)
{
  if (canRead()) { *value = read(); return true; } else { return false; }
}

byte KangarooReplyReader::read()
{
  byte data = *_data; _data ++; return data;
}

int32_t KangarooReplyReader::readBitPackedNumber()
{
	uint32_t encodedNumber = 0; byte shift = 0;

	for (byte i = 0; i < 5; i ++) // 30 bits
	{
		uint8_t byte;
		if (!tryRead(&byte)) { break; }
		
		encodedNumber |= (uint32_t)(byte & 0x3f) << shift; shift += 6;
		if (!(byte & 0x40)) { break; }
	}

	return (encodedNumber & 1) ? -(int32_t)(encodedNumber >> 1) : (int32_t)(encodedNumber >> 1);
}

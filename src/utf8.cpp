/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <cstdint>
#include <sstream>

#include "utf8.h"
#include "caseconvert.h"
#include "textstream.h"

uint8_t getUTF8CharNumBytes(char c)
{
  uint8_t num=1;
  unsigned char uc = static_cast<unsigned char>(c);
  if (uc>=0x80u) // multibyte character
  {
    if ((uc&0xE0u)==0xC0u)
    {
      num=2; // 110x.xxxx: 2 byte character
    }
    if ((uc&0xF0u)==0xE0u)
    {
      num=3; // 1110.xxxx: 3 byte character
    }
    if ((uc&0xF8u)==0xF0u)
    {
      num=4; // 1111.0xxx: 4 byte character
    }
    if ((uc&0xFCu)==0xF8u)
    {
      num=5; // 1111.10xx: 5 byte character
    }
    if ((uc&0xFEu)==0xFCu)
    {
      num=6; // 1111.110x: 6 byte character
    }
  }
  return num;
}

//! Decodes a given input of utf8 data to a unicode code point
//! given the number of bytes it's made of
static inline uint32_t decode_utf8( const char* data , int numBytes ) noexcept
{
  uint32_t cp = static_cast<unsigned char>(*data);
  if (numBytes>1)
  {
    cp &= 0x7F >> numBytes; // Mask out the header bits
    for (int i=1 ; i<numBytes ; i++)
    {
      cp = (cp<<6) | (static_cast<unsigned char>(data[i])&0x3F);
    }
  }
  return cp;
}

static inline uint32_t convertUTF8CharToUnicode(const char *s,size_t bytesLeft,int &len)
{
  if (s==nullptr || bytesLeft==0)
  {
    len=0;
    return 0;
  }
  unsigned char uc = static_cast<unsigned char>(*s);
  if (uc<128) // ASCII case
  {
    len=1;
    return uc;
  }
  switch (bytesLeft)
  {
    default:
      if ((uc&0xFEu)==0xFCu)// 1111110X six bytes
      {
        len=6;
        return decode_utf8(s,len);
      }
      // fall through
    case 5:
      if ((uc&0xFCu)==0xF8u) // 111110XX five bytes
      {
        len=5;
        return decode_utf8(s,len);
      }
      // fall through
    case 4:
      if ((uc&0xF8u)==0xF0u) // 11110XXX four bytes
      {
        len=4;
        return decode_utf8(s,len);
      }
      // fall through
    case 3:
      if ((uc&0xF0u)==0xE0u) // 1110XXXX three bytes
      {
        len=3;
        return decode_utf8(s,len);
      }
      // fall through
    case 2:
      if ((uc&0xE0u)==0xC0u) // 110XXXXX two bytes
      {
        len=2;
        return decode_utf8(s,len);
      }
      // fall through
    case 1:
      {
        len=1;
        return uc;
      }
  }
}

std::string getUTF8CharAt(const std::string &input,size_t pos)
{
  if (input.length()<=pos) return std::string();
  int numBytes=getUTF8CharNumBytes(input[pos]);
  if (input.length()<pos+numBytes) return std::string();
  return input.substr(pos,numBytes);
}

uint32_t getUnicodeForUTF8CharAt(const std::string &input,size_t pos)
{
  std::string charS = getUTF8CharAt(input,pos);
  int len=0;
  return convertUTF8CharToUnicode(charS.c_str(),charS.length(),len);
}

static inline char asciiToLower(uint32_t code)
{
  return code>='A' && code<='Z' ? static_cast<char>(code+'a'-'A') : static_cast<char>(code);
}

static inline char asciiToUpper(uint32_t code)
{
  return code>='a' && code<='z' ? static_cast<char>(code+'A'-'a') : static_cast<char>(code);
}

static inline std::string caseConvert(const std::string &input,
                                      char (*asciiConversionFunc)(uint32_t code),
                                      const char *(*conversionFunc)(uint32_t code))
{
  uint32_t code=0;
  std::string result;
  result.reserve(input.length()); // assume all ASCII characters
  int len=0;
  size_t bytesLeft = input.length();
  const char *p = input.c_str();
  while ((code=convertUTF8CharToUnicode(p,bytesLeft,len)))
  {
    if (code<128) // ASCII case
    {
      char c = asciiConversionFunc(code);
      result+=c;
    }
    else // generic case
    {
      const char *conv = conversionFunc(code);
      if (conv==nullptr) // no difference between lower and upper case
      {
        result.append(p,len);
      }
      else // replace the input character with the conversion result
      {
        result.append(conv);
      }
    }
    p+=len;
    bytesLeft-=len;
  }
  return result;
}

std::string convertUTF8ToLower(const std::string &input)
{
  return caseConvert(input,asciiToLower,convertUnicodeToLower);
}

std::string convertUTF8ToUpper(const std::string &input)
{
  return caseConvert(input,asciiToUpper,convertUnicodeToUpper);
}

const char *writeUTF8Char(TextStream &t,const char *s)
{
  if (s==nullptr) return nullptr;
  uint8_t len = getUTF8CharNumBytes(*s);
  for (uint8_t i=0;i<len;i++)
  {
    if (s[i]==0) // detect premature end of string (due to invalid UTF8 char)
    {
      len=i;
    }
  }
  t.write(s,len);
  return s+len;
}

bool lastUTF8CharIsMultibyte(const std::string &input)
{
  // last byte is part of a multibyte UTF8 char if bit 8 is set and bit 7 is not
  return !input.empty() && (static_cast<unsigned char>(input[input.length()-1])&0xC0)==0x80;
}

bool isUTF8CharUpperCase(const std::string &input,size_t pos)
{
  if (input.length()<=pos) return false;
  int len=0;
  // turn the UTF8 character at position pos into a unicode value
  uint32_t code = convertUTF8CharToUnicode(input.c_str()+pos,input.length()-pos,len);
  // check if the character can be converted to lower case, if so it was an upper case character
  return convertUnicodeToLower(code)!=nullptr;
}

int isUTF8NonBreakableSpace(const char *input)
{
  return (static_cast<unsigned char>(input[0])==0xC2 &&
          static_cast<unsigned char>(input[1])==0xA0) ? 2 : 0;
}

bool isUTF8PunctuationCharacter(uint32_t unicode)
{
  bool b = isPunctuationCharacter(unicode);
  return b;
}


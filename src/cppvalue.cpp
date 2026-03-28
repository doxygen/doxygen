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

#include <cstdlib>
#include <cassert>

#include "cppvalue.h"
#include "constexp.h"

CPPValue CPPValue::parseOctal(const std::string &token)
{
  long val = 0;
  for (const char c : token)
  {
    if (c >= '0' && c <= '7') val = val * 8 + c - '0';
  }
  return CPPValue(val);
}

CPPValue CPPValue::parseDecimal(const std::string &token)
{
  long val = 0;
  for (const char c : token)
  {
    if (c >= '0' && c <= '9') val = val * 10 + c - '0';
  }
  return CPPValue(val);
}

CPPValue CPPValue::parseHexadecimal(const std::string &token)
{
  long val = 0;
  for (const char c : token)
  {
    if      (c >= '0' && c <= '9') val = val * 16 + c - '0';
    else if (c >= 'a' && c <= 'f') val = val * 16 + c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') val = val * 16 + c - 'A' + 10;
  }
  //printf("parseHexadecimal %s->%x\n",qPrint(token),val);
  return CPPValue(val);
}

CPPValue CPPValue::parseBinary(const std::string &token)
{
  long val = 0;
  for (const char c : token)
  {
    if (c >= '0' && c <= '1') val = val * 2 + c - '0';
  }
  return CPPValue(val);
}

CPPValue CPPValue::parseCharacter(const std::string &token) // does not work for '\n' and the alike
{
  assert(token.length()>0);
  if (token[1]=='\\')
  {
    assert(token.length()>1);
    switch(token[2])
    {
      case 'n':  return CPPValue('\n');
      case 't':  return CPPValue('\t');
      case 'v':  return CPPValue('\v');
      case 'b':  return CPPValue('\b');
      case 'r':  return CPPValue('\r');
      case 'f':  return CPPValue('\f');
      case 'a':  return CPPValue('\a');
      case '\\': return CPPValue('\\');
      case '?':  return CPPValue('\?');
      case '\'': return CPPValue('\'');
      case '"':  return CPPValue('"');
      case '0':  // fall through
      case '1':  // fall through
      case '2':  // fall through
      case '3':  // fall through
      case '4':  // fall through
      case '5':  // fall through
      case '6':  // fall through
      case '7':  // fall through
                 return parseOctal(token);
      case 'x':
      case 'X':  return parseHexadecimal(token);
      default:   printf("Invalid escape sequence %s found!\n",std::string(token).c_str());
                 return CPPValue(0L);
    }
  }
  return CPPValue(token[1]);
}

CPPValue CPPValue::parseFloat(const std::string &token)
{
  return CPPValue(std::stod(token));
}

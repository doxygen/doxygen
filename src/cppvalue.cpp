/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include <stdlib.h>

#include "cppvalue.h"
#include "constexp.h"

CPPValue parseOctal(const std::string& token)
{
  long val = 0;
  for (const char *p = token.c_str(); *p != 0; p++)
  {
    if (*p >= '0' && *p <= '7') val = val * 8 + *p - '0';
  }
  return CPPValue(val);
}

CPPValue parseDecimal(const std::string& token)
{
  long val = 0;
  for (const char *p = token.c_str(); *p != 0; p++)
  {
    if (*p >= '0' && *p <= '9') val = val * 10 + *p - '0';
  }
  return CPPValue(val);
}

CPPValue parseHexadecimal(const std::string& token)
{
  long val = 0;
  for (const char *p = token.c_str(); *p != 0; p++)
  {
    if      (*p >= '0' && *p <= '9') val = val * 16 + *p - '0';
    else if (*p >= 'a' && *p <= 'f') val = val * 16 + *p - 'a' + 10;
    else if (*p >= 'A' && *p <= 'F') val = val * 16 + *p - 'A' + 10;
  }
  //printf("parseHexadecimal %s->%x\n",qPrint(token),val);
  return CPPValue(val);
}

CPPValue parseBinary(const std::string& token)
{
  long val = 0;
  for (const char *p = token.c_str(); *p != 0; p++)
  {
    if (*p >= '0' && *p <= '1') val = val * 2 + *p - '0';
  }
  return CPPValue(val);
}

CPPValue parseCharacter(const std::string& token) // does not work for '\n' and the alike
{
  if (token[1]=='\\')
  {
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
      default:   printf("Invalid escape sequence %s found!\n",token.c_str());
                 return CPPValue(0L);
    }
  }
  return CPPValue(token[1]);
}

CPPValue parseFloat(const std::string& token)
{
  return CPPValue(std::stod(token));
}

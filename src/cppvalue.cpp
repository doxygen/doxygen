/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

CPPValue parseOctal()
{
  long val = 0;
  for (const char *p = g_strToken.data(); *p != 0; p++)
  {
    if (*p >= '0' && *p <= '7') val = val * 8 + *p - '0';
  }
  return CPPValue(val);
}

CPPValue parseDecimal()
{
  long val = 0;
  for (const char *p = g_strToken.data(); *p != 0; p++)
  {
    if (*p >= '0' && *p <= '9') val = val * 10 + *p - '0';
  }
  return CPPValue(val);
}

CPPValue parseHexadecimal()
{
  long val = 0;
  for (const char *p = g_strToken.data(); *p != 0; p++)
  {
    if      (*p >= '0' && *p <= '9') val = val * 16 + *p - '0';
    else if (*p >= 'a' && *p <= 'f') val = val * 16 + *p - 'a' + 10;
    else if (*p >= 'A' && *p <= 'F') val = val * 16 + *p - 'A' + 10;
  }
  //printf("parseHexadecimal %s->%x\n",g_strToken.data(),val);
  return CPPValue(val);
}

CPPValue parseCharacter() // does not work for '\n' and the alike 
{
  if (g_strToken[1]=='\\')
  {
    switch(g_strToken[2])
    {
      case 'n':  return CPPValue((long)'\n');
      case 't':  return CPPValue((long)'\t');
      case 'v':  return CPPValue((long)'\v');
      case 'b':  return CPPValue((long)'\b');
      case 'r':  return CPPValue((long)'\r');
      case 'f':  return CPPValue((long)'\f');
      case 'a':  return CPPValue((long)'\a');
      case '\\': return CPPValue((long)'\\');
      case '?':  return CPPValue((long)'\?');
      case '\'': return CPPValue((long)'\'');
      case '"':  return CPPValue((long)'"');
      case '0':  // fall through
      case '1':  // fall through
      case '2':  // fall through
      case '3':  // fall through
      case '4':  // fall through
      case '5':  // fall through
      case '6':  // fall through
      case '7':  // fall through
                 return parseOctal();
      case 'x': 
      case 'X':  return parseHexadecimal();
      default:   printf("Invalid escape sequence %s found!\n",g_strToken.data()); 
                 return CPPValue(0L); 
    }
  }
  return CPPValue((long)g_strToken[1]);
}

CPPValue parseFloat()
{
  return CPPValue(atof(g_strToken));
}

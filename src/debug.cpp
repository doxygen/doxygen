/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include "qtbc.h"
#include "debug.h"

Debug::DebugMask Debug::curMask = Debug::Quiet;
int Debug::curPrio = 0;

void Debug::print(DebugMask mask,int prio,const char *fmt,...)
{
  if ((curMask&mask) && prio<=curPrio)
  {
    va_list args;
    va_start(args,fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}

static int labelToEnumValue(const char *l)
{
  QCString label=l;
  if (label=="FindMembers") 
    return Debug::FindMembers;
  else if (label=="Functions")
    return Debug::Functions;
  else if (label=="Variables")
    return Debug::Variables;
  else if (label=="Preprocessor")
    return Debug::Preprocessor;
  else
    return 0;
}

void Debug::setFlag(const char *lab)
{
  curMask = (DebugMask)(curMask | labelToEnumValue(lab));   
}

void Debug::clearFlag(const char *lab)
{
  curMask = (DebugMask)(curMask & ~labelToEnumValue(lab));
}

void Debug::setPriority(int p)
{
  curPrio = p;
}

bool Debug::isFlagSet(DebugMask mask)
{
  return (curMask & mask)!=0;
}

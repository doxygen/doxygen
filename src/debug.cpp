/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
  label=label.lower();
  if (label=="findmembers") 
    return Debug::FindMembers;
  else if (label=="functions")
    return Debug::Functions;
  else if (label=="variables")
    return Debug::Variables;
  else if (label=="preprocessor")
    return Debug::Preprocessor;
  else if (label=="classes")
    return Debug::Classes;
  else if (label=="commentcnv")
    return Debug::CommentCnv;
  else if (label=="validate")
    return Debug::Validate;
  else if (label=="printtree")
    return Debug::PrintTree;
  else if (label=="time")
    return Debug::Time;
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

/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "define.h"
#include "config.h"

Define::Define()
{ 
  undef=FALSE; 
  fileDef=0; 
  isPredefined=FALSE;
  nonRecursive=FALSE;
}

Define::Define(const Define &d)
      : name(d.name),definition(d.definition),fileName(d.fileName)
{ 
  //name=d.name; definition=d.definition; fileName=d.fileName; 
  lineNr=d.lineNr; 
  nargs=d.nargs; 
  undef=d.undef; 
  fileDef=0;
}

Define::~Define()
{
}

bool Define::hasDocumentation()
{
  return definition && (doc || Config::extractAllFlag);
}

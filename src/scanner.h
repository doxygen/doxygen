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

#ifndef SCANNER_H
#define SCANNER_H

#include "qtbc.h"
#include <stdio.h>
#include <qlist.h>
#include <qintdict.h>

#include "entry.h"
#include "code.h"
#include "membergroup.h"

class OutputList;
//typedef QIntDict<MemberGroup> MemberGroupDict;
typedef QIntDictIterator<MemberGroup> MemberGroupDictIterator;

extern void parseMain(Entry *);
extern void parseDoc(OutputList &ol,const char *clName, const char *memName,
                           const QCString &docString);
extern void parseExample(OutputList &ol,const QCString &docString, 
                           const char *fileName);
extern void parseText(OutputList &ol,const QCString &txtString);

extern MemberGroupDict memberGroupDict;

#endif

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

#include <stdio.h>
#include <qlist.h>
#include <qstring.h>

#include "entry.h"
#include "code.h"

class OutputList;

extern void parseMain(Entry *);
extern void parseDoc(OutputList &ol,const char *clName, const char *memName,
                           const QString &docString);
extern void parseExample(OutputList &ol,const QString &docString, 
                           const char *fileName);
#endif

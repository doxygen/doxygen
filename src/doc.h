/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef DOC_H
#define DOC_H

#include "qtbc.h"

class OutputList;

extern void parseDoc(OutputList &ol,
                     const char *fileName,int startLine,
                     const char *clName, const char *memName,
                     const QCString &docString);
extern void parseExample(OutputList &ol,const QCString &docString, 
                           const char *fileName);
extern void parseText(OutputList &ol,const QCString &txtString);

#endif

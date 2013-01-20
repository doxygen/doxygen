/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef _CONSTEXP_H
#define _CONSTEXP_H

#include "cppvalue.h"
#include <qcstring.h>

extern bool parseCppExpression(const char *fileName,int line,const QCString &s);
extern int cppExpYYparse();
extern int cppExpYYdebug;
extern QCString g_strToken;
extern CPPValue g_resultValue;
extern QCString g_constExpFileName;
extern int      g_constExpLineNr;

#endif

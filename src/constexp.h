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

#ifndef _CONSTEXP_H
#define _CONSTEXP_H

#include "cppvalue.h"
#include <qcstring.h>

#define YYSTYPE CPPValue
typedef void* yyscan_t;
struct constexpYY_state
{
  QCString    g_strToken;
  CPPValue    g_resultValue;
  int         g_constExpLineNr;
  QCString    g_constExpFileName;

  const char *g_inputString;
  int         g_inputPosition;
};

extern bool parseconstexp(const char *fileName,int line,const QCString &s);
extern int constexpYYparse(yyscan_t);
extern int constexpYYlex(YYSTYPE *lvalp, yyscan_t);
struct constexpYY_state*  constexpYYget_extra (yyscan_t yyscanner );
#endif

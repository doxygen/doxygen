/******************************************************************************
 *
 * $Id$
 *
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

#ifndef _CONSTEXP_H
#define _CONSTEXP_H

#include <qstring.h>
#include "cppvalue.h"

extern bool parseCppExpression(const QString &s);
extern int cppExpYYparse();
extern int cppExpYYdebug;
extern QString strToken;
extern CPPValue resultValue;

#endif

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

#ifndef DECLINFO_H
#define DECLINFO_H

#include "qtbc.h"
#include <stdio.h>

extern void parseFuncDecl(const QCString &decl,
                          QCString &clName,
                          QCString &classTempList,
                          QCString &type,
                          QCString &name,
                          QCString &args,
                          QCString &funcTempList,
                          QCString &exceptions
                         );
#endif

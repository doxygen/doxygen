/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef DECLINFO_H
#define DECLINFO_H

#include <stdio.h>

#include "types.h"

class DString;

extern void parseFuncDecl(const DString &decl,
                          const SrcLangExt lang,
                          DString &clName,
                          DString &type,
                          DString &name,
                          DString &args,
                          DString &funcTempList,
                          DString &exceptions
                         );
#endif

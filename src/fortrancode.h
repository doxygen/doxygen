/******************************************************************************
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

#ifndef FORTRANCODE_H
#define FORTRANCODE_H

#include "types.h"

class CodeOutputInterface;
class FileDef;
class MemberDef;
class QCString;
class Definition;

void parseFortranCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd,
            int startLine,int endLine,bool inlineFragment,
            const MemberDef *memberDef,bool showLineNumbers,const Definition *searchCtx,
            bool collectRefs, FortranFormat format);
void resetFortranCodeParserState();
void codeFreeScanner();

const int fixedCommentAfter = 72;
#endif

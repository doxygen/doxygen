/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

#ifndef CODE_H
#define CODE_H

#include "qtbc.h"
#include <stdio.h>

class CodeOutputInterface;
class FileDef;
class MemberDef;

void parseFortranCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd=0,
            int startLine=-1,int endLine=-1,bool inlineFragment=FALSE,
            MemberDef *memberDef=0,bool showLineNumbers=TRUE);
void resetFortranCodeParserState();
void codeFreeScanner();

#endif

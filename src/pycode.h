/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada), executed
 *  as part of CS179e (Compiler design project) at the UC Riverside,
 *  under supervision of Peter H. Fröhlic.
 */

#ifndef PYCODE_H
#define PYCODE_H

#include "qtbc.h"
#include <stdio.h>

class CodeOutputInterface;
class FileDef;
class MemberDef;

extern void parsePythonCode(CodeOutputInterface &,const char *,const QCString &, 
             bool ,const char *,FileDef *fd=0,
	     int startLine=-1,int endLine=-1,bool inlineFragment=FALSE,
             MemberDef *memberDef=0);
extern void resetPythonCodeParserState();

#endif

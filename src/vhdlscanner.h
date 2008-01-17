/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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

#ifndef VHDLSCANNER_H
#define VHDLSCANNER_H

#include "parserintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <qarray.h>
#include <unistd.h>
#include <qfile.h>
#include <qdict.h>

#include "entry.h"
#include "memberlist.h"

#if 0
#include "qcstring.h"
#include "qlist.h"
#include "qstringlist.h"
#include "filedef.h"
#include "classlist.h"
#include "classdef.h"
#include "translator.h"
#include "qregexp.h"
#include "outputlist.h"
#include "membername.h"
#include "memberdef.h"
#include "memberlist.h"
#endif

class Entry;
class ClassSDict;
class FileStorage;
class ClassDef;
class MemberDef;
class QStringList;


/** \brief VHDL parser using state-based lexical scanning.
 *
 * This is the VHDL language parser for doxygen.
 */
class VHDLLanguageScanner : public ParserInterface
{
  public:
    virtual ~VHDLLanguageScanner() {}
    void parseInput(const char * fileName, 
                    const char *fileBuf, 
                    Entry *root);
    bool needsPreprocessing(const QCString &extension);
    void parseCode(CodeOutputInterface &codeOutIntf,
                   const char *scopeName,
                   const QCString &input,
                   bool isExampleBlock,
                   const char *exampleName=0,
                   FileDef *fileDef=0,
                   int startLine=-1,
                   int endLine=-1,
                   bool inlineFragment=FALSE,
                   MemberDef *memberDef=0
                  );
    void resetCodeParserState();
    void parsePrototype(const char *text);
};

void vhdlscanFreeScanner();

//---------------------------------------------------------------------------------

#endif

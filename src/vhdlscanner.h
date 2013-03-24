/******************************************************************************
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
#include "vhdldocgen.h"
#include "entry.h"

class Entry;
class ClassSDict;
class FileStorage;
class ClassDef;
class MemberDef;
class QStringList;
class MemberList;


/** VHDL parser using state-based lexical scanning.
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
                   MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   Definition *searchCtx=0
                  );
    void resetCodeParserState();
    void parsePrototype(const char *text);
};

/** Container for vhdlscanner */
struct VhdlContainer
{
  int yyLineNr;          // current line no
  int iLine;             // line no of last t_identifier  
  QCString qstr;         //  t_identifier 
  QCString fileName;     // current file
  Entry*  root;          // root
};   

/** Configuration node for VHDL */
struct VhdlConfNode
{ 
  VhdlConfNode(const char*  a,const char*  b,const char* config,const char* cs,bool leaf) 
  { 
    arch=a;              // architecture  e.g. for iobuffer
    arch=arch.lower();
    binding=b;           // binding e.g.  use entiy work.xxx(bev)
    binding=binding.lower();
    confVhdl=config;     // configuration foo is bar
    compSpec=cs;        
    isInlineConf=false;  // primary configuration?
    isLeaf=leaf;
  };

  QCString confVhdl;
  QCString arch;
  QCString binding;
  QCString compSpec;
  int level;
  bool isLeaf;
  bool isInlineConf;

};



// returns the current conpound entity,architecture, package,package body 
Entry* getVhdlCompound();

// return the current parsed entry
Entry* getCurrentVhdlEntry();

void newVhdlEntry();

void initVhdlParser();

struct VhdlContainer* getVhdlCont();

// returns the  parsed line 
// @ param object index of vhdl keyword like t_Identifier t_Entity 
int getParsedLine(int object);

void vhdlscanFreeScanner();

void vhdlParse();

// return the list of component instantiations e.g. foo: component bar 
QList<Entry> &  getVhdlInstList();

// returns   configuration list
QList<VhdlConfNode>& getVhdlConfiguration();

// returns library/used list
QList<Entry> &  getLibUse();

void isVhdlDocPending();


#endif

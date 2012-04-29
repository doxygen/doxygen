/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
                   MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE
                  );
    void resetCodeParserState();
    void parsePrototype(const char *text);
};

// container for vhdlscanner 
struct  s_contVhdl
{
  int yyLineNr;          // current line no
  int iLine;             // line no of last t_identifier  
  QCString qstr;         //  t_identifier 
  QCString fileName;     // current file
  Entry*  root;          // root
};   

struct ConfNode
{ 
  ConfNode *prevNode;
  ConfNode(const char*  a,const char*  b,const char* config) 
  { 
    arch=a;              // architecture  e.g. for iobuffer
    binding=b;           // binding e.g.  use entiy work.xxx(bev)
    confVhdl=config;     // configuration foo is bar
    isBind=false;
    prevNode=NULL;
    isRoot=false;          
    isInlineConf=false;  // primary configuration?
  };

  QCString confVhdl;
  QCString arch;
  QCString binding;
  QList<ConfNode> confN;
  bool isBind;
  bool isInlineConf;
  bool isRoot;

  void addNode(ConfNode* n) { confN.append(n); }
  bool isBinding()          { return binding.isEmpty(); }
} ;

// returns the current conpound entity,architecture, package,package body 
Entry* getVhdlCompound();

// return the current parsed entry
Entry* getCurrentVhdlEntry();

void newVhdlEntry();

void initVhdlParser();

struct s_contVhdl*    getVhdlCont();

// returns the  parsed line 
// @ param object index of vhdl keyword like t_Identifier t_Entity 
int getParsedLine(int object);

void vhdlscanFreeScanner();

// return the list of component instantiations e.g. foo: component bar 
QList<Entry> &  getVhdlInstList();

// returns the list of found configurations
QList<ConfNode>& getVhdlConfiguration();

void isVhdlDocPending();
#endif

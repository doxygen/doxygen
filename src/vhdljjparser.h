#ifndef VHDLJJPARSER_H
#define VHDLJJPARSER_H

#include "parserintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <qarray.h>
#include <qcstringlist.h>

#include <qfile.h>
#include <qdict.h>
#include <string>
#include "types.h"
#include "entry.h"
#include "vhdldocgen.h"
#include "vhdlcode.h"
#include "memberlist.h"
#include "config.h"




enum  { GEN_SEC=0x1, PARAM_SEC,CONTEXT_SEC,PROTECTED_SEC } ;
void  parserVhdlfile(const char* inputBuffer);

class Entry;
class ClassSDict;
class FileStorage;
class ClassDef;
class MemberDef;
struct VhdlConfNode;


/** \brief VHDL parser using state-based lexical scanning.
 *
 * This is the VHDL language parser for doxygen.
 */
class VHDLLanguageScanner : public ParserInterface
{
  public:
    virtual ~VHDLLanguageScanner() {}
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char * fileName,
                    const char *fileBuf,
                    Entry *root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
  
 void parseCode(CodeOutputInterface &codeOutIntf,
                   const char *scopeName,
                   const QCString &input,
                   SrcLangExt lang,
                   bool isExampleBlock,
                   const char *exampleName=0,
                   FileDef *fileDef=0,
                   int startLine=-1,
                   int endLine=-1,
                   bool inlineFragment=FALSE,
                   const MemberDef *memberDef=0,
                   bool showLineNumbers=TRUE,
                   const Definition *searchCtx=0,
                   bool collectXRefs=TRUE
				   );
		bool needsPreprocessing(const QCString &) { return TRUE; }
		void resetCodeParserState(){};
	    void parsePrototype(const char *text);
};

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

void vhdlscanFreeScanner();

QList<VhdlConfNode>& getVhdlConfiguration();
QList<Entry>& getVhdlInstList();

#endif

#ifndef VHDLCODE_H
#define VHDLCODE_H

#include "parserintf.h"

class CodeOutputInterface;
class FileDef;
class MemberDef;

void codeFreeVhdlScanner();

class VHDLCodeParser : public CodeParserInterface
{
  public:
    virtual ~VHDLCodeParser() {}
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
    void resetCodeParserState() {}
};


#endif 

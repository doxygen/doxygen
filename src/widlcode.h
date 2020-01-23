#ifndef DOXYGEN_WIDLCODE_H
#define DOXYGEN_WIDLCODE_H

#include <stdio.h>
#include "parserintf.h"
#include "outputgen.h"

class CodeOutputInterface;
class FileDef;
class MemberDef;
class QCString;
class Definition;


class WidlCodeParser : public CodeParserInterface
{
  public:
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
                   bool collectXrefs=TRUE
                  ) {
        codeOutIntf.codify(input);
        fprintf(stderr, "[WIDL1] WidlCodeParser(%s)\n", scopeName);
    }
    void resetCodeParserState() {}
};


#endif

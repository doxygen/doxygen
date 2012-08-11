#ifndef VHDLCODE_H
#define VHDLCODE_H

#include "qtbc.h"
#include <stdio.h>

class CodeOutputInterface;
class FileDef;
class MemberDef;

void parseVhdlCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd,
            int startLine,int endLine,bool inlineFragment,
            MemberDef *memberDef,bool showLineNumbers,Definition *searchCtx);
void resetVhdlCodeParserState();
void codeFreeVhdlScanner();

#endif 

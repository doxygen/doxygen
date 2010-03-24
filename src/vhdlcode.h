#ifndef VHDLCODE_H
#define VHDLCODE_H

#include "qtbc.h"
#include <stdio.h>

class CodeOutputInterface;
class FileDef;
class MemberDef;

void parseVhdlCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd=0,
            int startLine=-1,int endLine=-1,bool inlineFragment=FALSE,
            MemberDef *memberDef=0,bool showLineNumbers=TRUE);
void resetVhdlCodeParserState();
void codeFreeVhdlScanner();

#endif 

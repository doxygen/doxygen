#ifndef VHDLCODE_H
#define VHDLCODE_H

class CodeOutputInterface;
class FileDef;
class MemberDef;

void parseVhdlCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd,
            int startLine,int endLine,bool inlineFragment,
            MemberDef *memberDef,bool showLineNumbers,Definition *searchCtx,
            bool collectXRefs);
void resetVhdlCodeParserState();
void codeFreeVhdlScanner();

#endif 

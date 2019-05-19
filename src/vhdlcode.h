#ifndef VHDLCODE_H
#define VHDLCODE_H

class CodeOutputInterface;
class FileDef;
class MemberDef;

void parseVhdlCode(CodeOutputInterface &,const char *,const QCString &, 
            bool ,const char *,FileDef *fd,
            int startLine,int endLine,bool inlineFragment,
            const MemberDef *memberDef,bool showLineNumbers,const Definition *searchCtx,
            bool collectXRefs);
void resetVhdlCodeParserState();
void codeFreeVhdlScanner();

#endif 

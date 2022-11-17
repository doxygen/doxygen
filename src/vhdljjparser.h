#ifndef VHDLJJPARSER_H
#define VHDLJJPARSER_H

#include <vector>
#include <memory>
#include <string>

#include "parserintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "types.h"
#include "entry.h"
#include "vhdldocgen.h"
#include "config.h"
#include "util.h"

enum  { GEN_SEC=0x1, PARAM_SEC,CONTEXT_SEC,PROTECTED_SEC } ;
//void  parserVhdlfile(const char* inputBuffer);

/** \brief VHDL parser using state-based lexical scanning.
 *
 * This is the VHDL language parser for doxygen.
 */
class VHDLOutlineParser : public OutlineParserInterface
{
  public:
    VHDLOutlineParser();
    virtual ~VHDLOutlineParser();
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser);

    bool needsPreprocessing(const QCString &) const { return TRUE; }
    void parsePrototype(const QCString &text);


    // interface for generated parser code

    void setLineParsed(int tok);
    int getLine(int tok);
    int getLine();
    void lineCount(const char*);
    void lineCount();
    void addProto(const char *s1,const char *s2,const char *s3,const char *s4,const char *s5,const char *s6);
    //void addConfigureNode(const char* a,const char*b, bool,bool isLeaf,bool inlineConf);
    void createFunction(const char *impure,uint64 spec,const char *fname);
    void addVhdlType(const char *n,int startLine,int section, uint64 spec,const char* args,const char* type,Protection prot);
    void addCompInst(const char *n, const char* instName, const char* comp,int iLine);
    void handleCommentBlock(const QCString &doc,bool brief);
    void handleFlowComment(const char*);
    void initEntry(Entry *e);
    void newEntry();
    bool isFuncProcProced();
    void pushLabel(QCString &,QCString&);
    QCString popLabel(QCString & q);
    bool addLibUseClause(const QCString &type);
    void mapLibPackage( Entry* root);
    void createFlow();
    void error_skipto(int kind);
    void oneLineComment(QCString qcs);
    void setMultCommentLine();
    bool checkMultiComment(QCString& qcs,int line);
    void insertEntryAtLine(std::shared_ptr<Entry> ce,int line);
    QCString getNameID();
    int checkInlineCode(QCString & doc);
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


const EntryList &getVhdlInstList();

QCString filter2008VhdlComment(const char *s);

#endif

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

enum  class VhdlSection { UNKNOWN = 0, GEN_SEC=0x1, PARAM_SEC,CONTEXT_SEC,PROTECTED_SEC } ;
//void  parserVhdlfile(const char* inputBuffer);

/** \brief VHDL parser using state-based lexical scanning.
 *
 * This is the VHDL language parser for doxygen.
 */
class VHDLOutlineParser : public OutlineParserInterface
{
  public:
    VHDLOutlineParser();
   ~VHDLOutlineParser() override;
    NON_COPYABLE(VHDLOutlineParser)
    void parseInput(const QCString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser) override;

    bool needsPreprocessing(const QCString &) const override { return TRUE; }
    void parsePrototype(const QCString &text) override;


    // interface for generated parser code

    void setLineParsed(int tok);
    int getLine(int tok);
    int getLine();
    void lineCount(const QCString &);
    void lineCount();
    void addProto(const QCString &s1,const QCString &s2,const QCString &s3,const QCString &s4,const QCString &s5,const QCString &s6);
    void createFunction(const QCString &impure,VhdlSpecifier spec,const QCString &fname);
    void addVhdlType(const QCString &n,int startLine,EntryType section, VhdlSpecifier spec,const QCString &args,const QCString &type,Protection prot);
    void addCompInst(const QCString &n, const QCString &instName, const QCString &comp,int iLine);
    void handleCommentBlock(const QCString &doc,bool brief);
    void handleFlowComment(const QCString &);
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

QCString filter2008VhdlComment(const QCString &s);

#endif

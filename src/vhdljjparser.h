#ifndef VHDLJJPARSER_H
#define VHDLJJPARSER_H

#include <memory>

#include "parserintf.h"
#include "vhdldocgen.h"
#include "config.h"
#include "util.h"
#include "stringutil.h"

enum  class VhdlSection { UNKNOWN = 0, GEN_SEC=0x1, PARAM_SEC,CONTEXT_SEC,PROTECTED_SEC } ;
//void  parserVhdlfile(const char* inputBuffer);

/** \brief VHDL parser using state-based lexical scanning.
 *
 * This is the VHDL language parser for doxygen.
 */
class VHDLOutlineParser final : public OutlineParserInterface
{
  public:
    VHDLOutlineParser();
   ~VHDLOutlineParser() override;
    NON_COPYABLE(VHDLOutlineParser)
    void parseInput(const DString &fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    ClangTUParser *clangParser) override;

    bool needsPreprocessing(const DString &) const override { return true; }
    void parsePrototype(const DString &text) override;


    // interface for generated parser code

    void setLineParsed(int tok);
    int getLine(int tok);
    int getLine();
    void lineCount(const DString &);
    void lineCount();
    void addProto(const DString &s1,const DString &s2,const DString &s3,const DString &s4,const DString &s5,const DString &s6);
    void createFunction(const DString &impure,VhdlSpecifier spec,const DString &fname);
    void addVhdlType(const DString &n,int startLine,EntryType section, VhdlSpecifier spec,const DString &args,const DString &type,Protection prot);
    void addCompInst(const DString &n, const DString &instName, const DString &comp,int iLine);
    void handleCommentBlock(const DString &doc,bool brief);
    void handleFlowComment(const DString &);
    void initEntry(Entry *e);
    void newEntry();
    bool isFuncProcProced();
    void pushLabel(DString &,DString&);
    DString popLabel(DString & q);
    bool addLibUseClause(const DString &type);
    void mapLibPackage( Entry* root);
    void createFlow();
    void error_skipto(int kind);
    void oneLineComment(DString qcs);
    void setMultCommentLine();
    bool checkMultiComment(DString& qcs,int line);
    void insertEntryAtLine(std::shared_ptr<Entry> ce,int line);
    DString getNameID();
    size_t checkInlineCode(DString & doc);
  private:
    struct Private;
    std::unique_ptr<Private> p;
};


const EntryList &getVhdlInstList();

DString filter2008VhdlComment(const DString &s);

#endif

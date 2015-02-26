/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef _DOCVISITOR_H
#define _DOCVISITOR_H

// ids
const int DocVisitor_Html  = 0;
const int DocVisitor_Latex = 1;
const int DocVisitor_XML   = 2;
const int DocVisitor_RTF   = 3;
const int DocVisitor_Man   = 4;
const int DocVisitor_Text  = 5;
const int DocVisitor_Other = 6;
const int DocVisitor_Docbook = 7;

// forward declarations
class DocWord;
class DocWhiteSpace;
class DocAutoList;
class DocAutoListItem;
class DocPara;
class DocRoot;
class DocSymbol;
class DocURL;
class DocStyleChange;
class DocSimpleSect;
class DocTitle;
class DocSimpleList;
class DocSimpleListItem;
class DocSection;
class DocVerbatim;
class DocXRefItem;
class DocHtmlList;
class DocHtmlListItem;
class DocHtmlDescList;
class DocHtmlDescTitle;
class DocHtmlDescData;
class DocHtmlTable;
class DocHtmlRow;
class DocHtmlCell;
class DocHtmlCaption;
class DocLineBreak;
class DocHorRuler;
class DocAnchor;
class DocIndexEntry;
class DocInternal;
class DocHRef;
class DocInclude;
class DocIncOperator;
class DocHtmlHeader;
class DocImage;
class DocDotFile;
class DocMscFile;
class DocDiaFile;
class DocLink;
class DocCite;
class DocRef;
class DocFormula;
class DocSecRefItem;
class DocSecRefList;
class DocLinkedWord;
class DocParamSect;
class DocParamList;
class DocInternalRef;
class DocCopy; // TODO: no longer generated => remove
class DocText;
class DocSimpleSectSep;
class DocHtmlBlockQuote;
class DocVhdlFlow;
class DocParBlock;

/*! @brief Abstract visitor that participates in the visitor pattern.
 */
class DocVisitor
{
    int m_id;
  public:
    DocVisitor(int id) : m_id(id) {}
    virtual ~DocVisitor() {}
    int id() const { return m_id; }

    /*! @name Visitor functions for leaf nodes 
     *  @{
     */
    virtual void visit(DocWord *) = 0;
    virtual void visit(DocWhiteSpace *) = 0;
    virtual void visit(DocSymbol *) = 0;
    virtual void visit(DocURL *) = 0;
    virtual void visit(DocStyleChange *) = 0;
    virtual void visit(DocVerbatim *) = 0;
    virtual void visit(DocLineBreak *) = 0;
    virtual void visit(DocHorRuler *) = 0;
    virtual void visit(DocAnchor *) = 0;
    virtual void visit(DocInclude *) = 0;
    virtual void visit(DocIncOperator *) = 0;
    virtual void visit(DocFormula *) = 0;
    virtual void visit(DocLinkedWord *) = 0;
    virtual void visit(DocIndexEntry *) = 0;
    virtual void visit(DocSimpleSectSep *) = 0;
    virtual void visit(DocCite *) = 0;
    /*! @} */

    /*! @name Visitor functions for internal nodes 
     *  @{
     */
    virtual void visitPre(DocAutoList *) = 0;
    virtual void visitPost(DocAutoList *) = 0;
    virtual void visitPre(DocAutoListItem *) = 0;
    virtual void visitPost(DocAutoListItem *) = 0;
    virtual void visitPre(DocPara *) = 0;
    virtual void visitPost(DocPara *) = 0;
    virtual void visitPre(DocRoot *) = 0;
    virtual void visitPost(DocRoot *) = 0;
    virtual void visitPre(DocSimpleSect *) = 0;
    virtual void visitPost(DocSimpleSect *) = 0;
    virtual void visitPre(DocTitle *) = 0;
    virtual void visitPost(DocTitle *) = 0;
    virtual void visitPre(DocSimpleList *) = 0;
    virtual void visitPost(DocSimpleList *) = 0;
    virtual void visitPre(DocSimpleListItem *) = 0;
    virtual void visitPost(DocSimpleListItem *) = 0;
    virtual void visitPre(DocSection *) = 0;
    virtual void visitPost(DocSection *) = 0;
    virtual void visitPre(DocHtmlList *) = 0;
    virtual void visitPost(DocHtmlListItem *) = 0;
    virtual void visitPre(DocHtmlListItem *) = 0;
    virtual void visitPost(DocHtmlList *) = 0;
    virtual void visitPre(DocHtmlDescList *) = 0;
    virtual void visitPost(DocHtmlDescList *) = 0;
    virtual void visitPre(DocHtmlDescTitle *) = 0;
    virtual void visitPost(DocHtmlDescTitle *) = 0;
    virtual void visitPre(DocHtmlDescData *) = 0;
    virtual void visitPost(DocHtmlDescData *) = 0;
    virtual void visitPre(DocHtmlTable *) = 0;
    virtual void visitPost(DocHtmlRow *) = 0;
    virtual void visitPre(DocHtmlCell *) = 0;
    virtual void visitPost(DocHtmlCell *) = 0;
    virtual void visitPre(DocHtmlRow *) = 0;
    virtual void visitPost(DocHtmlTable *) = 0;
    virtual void visitPre(DocHtmlCaption *) = 0;
    virtual void visitPost(DocHtmlCaption *) = 0;
    virtual void visitPre(DocInternal *) = 0;
    virtual void visitPost(DocInternal *) = 0;
    virtual void visitPre(DocHRef *) = 0;
    virtual void visitPost(DocHRef *) = 0;
    virtual void visitPre(DocHtmlHeader *) = 0;
    virtual void visitPost(DocHtmlHeader *) = 0;
    virtual void visitPre(DocImage *) = 0;
    virtual void visitPost(DocImage *) = 0;
    virtual void visitPre(DocDotFile *) = 0;
    virtual void visitPost(DocDotFile *) = 0;
    virtual void visitPre(DocMscFile *) = 0;
    virtual void visitPost(DocMscFile *) = 0;   
    virtual void visitPre(DocDiaFile *) = 0;
    virtual void visitPost(DocDiaFile *) = 0;
    virtual void visitPre(DocLink *) = 0;
    virtual void visitPost(DocLink *) = 0;
    virtual void visitPre(DocRef *) = 0;
    virtual void visitPost(DocRef *) = 0;
    virtual void visitPre(DocSecRefItem *) = 0;
    virtual void visitPost(DocSecRefItem *) = 0;
    virtual void visitPre(DocSecRefList *) = 0;
    virtual void visitPost(DocSecRefList *) = 0;
    virtual void visitPre(DocParamSect *) = 0;
    virtual void visitPost(DocParamSect *) = 0;
    virtual void visitPre(DocParamList *) = 0;
    virtual void visitPost(DocParamList *) = 0;
    virtual void visitPre(DocXRefItem *) = 0;
    virtual void visitPost(DocXRefItem *) = 0;
    virtual void visitPre(DocInternalRef *) = 0;
    virtual void visitPost(DocInternalRef *) = 0;
    virtual void visitPre(DocCopy *) = 0;
    virtual void visitPost(DocCopy *) = 0;
    virtual void visitPre(DocText *) = 0;
    virtual void visitPost(DocText *) = 0;
    virtual void visitPre(DocHtmlBlockQuote *) = 0;
    virtual void visitPost(DocHtmlBlockQuote *) = 0;
    virtual void visitPre(DocVhdlFlow *) = 0;
    virtual void visitPost(DocVhdlFlow *) = 0;
    virtual void visitPre(DocParBlock *) = 0;
    virtual void visitPost(DocParBlock *) = 0;
    /*! @} */
};

#endif

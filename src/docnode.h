/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef DOCNODE_H
#define DOCNODE_H

#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>
#include <variant>
#include <type_traits>

#include "qcstring.h"
#include "docvisitor.h"
#include "docparser.h"
#include "htmlattrib.h"
#include "htmlentity.h"
#include "growvector.h"
#include "section.h"
#include "construct.h"
#include "doctokenizer.h"

class MemberDef;
class Definition;
class DocParser;

//---------------------------------------------------------------------------

#define DOC_NODES                                                                                                                                      \
/*  0 */  DN(DocWord)           DN_SEP DN(DocLinkedWord)    DN_SEP DN(DocURL)            DN_SEP DN(DocLineBreak)    DN_SEP DN(DocHorRuler)    DN_SEP   \
/*  5 */  DN(DocAnchor)         DN_SEP DN(DocCite)          DN_SEP DN(DocStyleChange)    DN_SEP DN(DocSymbol)       DN_SEP DN(DocEmoji)       DN_SEP   \
/* 10 */  DN(DocWhiteSpace)     DN_SEP DN(DocSeparator)     DN_SEP DN(DocVerbatim)       DN_SEP DN(DocInclude)      DN_SEP DN(DocIncOperator) DN_SEP   \
/* 15 */  DN(DocFormula)        DN_SEP DN(DocIndexEntry)    DN_SEP DN(DocAutoList)       DN_SEP DN(DocAutoListItem) DN_SEP DN(DocTitle)       DN_SEP   \
/* 20 */  DN(DocXRefItem)       DN_SEP DN(DocImage)         DN_SEP DN(DocDotFile)        DN_SEP DN(DocMscFile)      DN_SEP DN(DocDiaFile)     DN_SEP   \
/* 25 */  DN(DocVhdlFlow)       DN_SEP DN(DocLink)          DN_SEP DN(DocRef)            DN_SEP DN(DocInternalRef)  DN_SEP DN(DocHRef)        DN_SEP   \
/* 30 */  DN(DocHtmlHeader)     DN_SEP DN(DocHtmlDescTitle) DN_SEP DN(DocHtmlDescList)   DN_SEP DN(DocSection)      DN_SEP DN(DocSecRefItem)  DN_SEP   \
/* 35 */  DN(DocSecRefList)     DN_SEP DN(DocInternal)      DN_SEP DN(DocParBlock)       DN_SEP DN(DocSimpleList)   DN_SEP DN(DocHtmlList)    DN_SEP   \
/* 40 */  DN(DocSimpleSect)     DN_SEP DN(DocSimpleSectSep) DN_SEP DN(DocParamSect)      DN_SEP DN(DocPara)         DN_SEP DN(DocParamList)   DN_SEP   \
/* 45 */  DN(DocSimpleListItem) DN_SEP DN(DocHtmlListItem)  DN_SEP DN(DocHtmlDescData)   DN_SEP DN(DocHtmlCell)     DN_SEP DN(DocHtmlCaption) DN_SEP   \
/* 50 */  DN(DocHtmlRow)        DN_SEP DN(DocHtmlTable)     DN_SEP DN(DocHtmlBlockQuote) DN_SEP DN(DocText)         DN_SEP DN(DocRoot)        DN_SEP   \
/* 55 */  DN(DocHtmlDetails)    DN_SEP DN(DocHtmlSummary)   DN_SEP DN(DocPlantUmlFile)                                                                      \

// forward declarations
#define DN(x) class x;
#define DN_SEP
DOC_NODES
#undef DN
#undef DN_SEP

// define a variant type
#define DN(x) x
#define DN_SEP ,
using DocNodeVariant = std::variant<
DOC_NODES
>;
#undef DN
#undef DN_SEP

// getter functions to return the name of a doc node type
#define DN(x) constexpr const char *docNodeName(const x &/* n */) { return #x; }
#define DN_SEP
DOC_NODES
#undef DN
#undef DN_SEP

/** Abstract node interface with type information. */
class DocNode
{
  public:
    /*! Creates a new node */
    DocNode(DocParser *parser,DocNodeVariant *parent) : m_parser(parser), m_parent(parent) {}
   ~DocNode() = default;
    ONLY_DEFAULT_MOVABLE(DocNode)

    /*! Returns the parent of this node or nullptr for the root node. */
    DocNodeVariant *parent() { return m_parent; }
    const DocNodeVariant *parent() const { return m_parent; }

    DocNodeVariant *thisVariant() { return m_thisVariant; }
    const DocNodeVariant *thisVariant() const { return m_thisVariant; }

    void setThisVariant(DocNodeVariant *thisVariant) { m_thisVariant=thisVariant; }

    DocParser *parser() { return m_parser; }
    const DocParser *parser() const { return m_parser; }

    /*! Sets a new parent for this node. */
    void setParent(DocNodeVariant *parent) { m_parent = parent; }

    /*! Returns TRUE iff this node is inside a preformatted section */
    bool isPreformatted() const { return m_insidePre; }

  protected:
    /*! Sets whether or not this item is inside a preformatted section */
    void setInsidePreformatted(bool p) { m_insidePre = p; }
    enum RefType { Unknown, Anchor, Section, Table };
  private:
    bool m_insidePre = false;
    DocParser *m_parser;
    DocNodeVariant *m_parent;
    DocNodeVariant *m_thisVariant = nullptr;
};

struct DocNodeList : public GrowVector<DocNodeVariant>
{
  /** Append a new DocNodeVariant to the list by constructing it with type T and
   *  parameters Args.
   */
  template<class T,class...Args>
  void append(Args&&... args);

  /** moves the element of list \a l at the end of this list.
   * List \a l will become empty. */
  void move_append(DocNodeList &l);

  /** Returns a pointer to the last element in the list if that element exists and
   *  holds a T, otherwise nullptr is returned.
   */
  template<class T>
  T *get_last();
};

/** Base class for nodes with children */
class DocCompoundNode : public DocNode
{
  public:
    DocCompoundNode(DocParser *parser,DocNodeVariant *parent)
      : DocNode(parser,parent) {}
    DocNodeList &children()             { return m_children; }
    const DocNodeList &children() const { return m_children; }

  private:
    DocNodeList m_children;
};

/** Node representing a word
 */
class DocWord : public DocNode
{
  public:
    DocWord(DocParser *parser,DocNodeVariant *parent,const QCString &word);
    QCString word() const { return m_word; }

  private:
    QCString  m_word;
};

/** Node representing a word that can be linked to something
 */
class DocLinkedWord : public DocNode
{
  public:
    DocLinkedWord(DocParser *parser,DocNodeVariant *parent,const QCString &word,
                  const QCString &ref,const QCString &file,
                  const QCString &anchor,const QCString &tooltip);
    QCString word() const       { return m_word; }
    QCString file() const       { return m_file; }
    QCString relPath() const    { return m_relPath; }
    QCString ref() const        { return m_ref; }
    QCString anchor() const     { return m_anchor; }
    QCString tooltip() const    { return m_tooltip; }

  private:
    QCString  m_word;
    QCString  m_ref;
    QCString  m_file;
    QCString  m_relPath;
    QCString  m_anchor;
    QCString  m_tooltip;
};

/** Node representing a URL (or email address) */
class DocURL : public DocNode
{
  public:
    DocURL(DocParser *parser,DocNodeVariant *parent,const QCString &url,bool isEmail) :
      DocNode(parser,parent), m_url(url), m_isEmail(isEmail) {}
    QCString url() const        { return m_url; }
    bool isEmail() const       { return m_isEmail; }

  private:
    QCString  m_url;
    bool m_isEmail = false;
};

/** Node representing a line break */
class DocLineBreak : public DocNode
{
  public:
    DocLineBreak(DocParser *parser,DocNodeVariant *parent) : DocNode(parser,parent) {}
    DocLineBreak(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
      : DocNode(parser,parent), m_attribs(attribs) {}

    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    HtmlAttribList m_attribs;
};

/** Node representing a horizontal ruler */
class DocHorRuler : public DocNode
{
  public:
    DocHorRuler(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
      : DocNode(parser,parent), m_attribs(attribs) {}

    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    HtmlAttribList m_attribs;
};

/** Node representing an anchor */
class DocAnchor : public DocNode
{
  public:
    DocAnchor(DocParser *parser,DocNodeVariant *parent,const QCString &id,bool newAnchor);
    QCString anchor() const    { return m_anchor; }
    QCString file() const      { return m_file; }

    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    QCString  m_anchor;
    QCString  m_file;
    HtmlAttribList m_attribs;
};

/** Node representing a citation of some bibliographic reference */
class DocCite : public DocNode
{
  public:
    DocCite(DocParser *parser,DocNodeVariant *parent,const QCString &target,const QCString &context);
    QCString file() const        { return m_file; }
    QCString relPath() const     { return m_relPath; }
    QCString ref() const         { return m_ref; }
    QCString anchor() const      { return m_anchor; }
    QCString text() const        { return m_text; }

  private:
    QCString   m_file;
    QCString   m_relPath;
    QCString   m_ref;
    QCString   m_anchor;
    QCString   m_text;
};


/** Node representing a style change */
class DocStyleChange : public DocNode
{
  public:
    enum Style { Bold          = (1<<0),
                 Italic        = (1<<1),
                 Code          = (1<<2),
                 Center        = (1<<3),
                 Small         = (1<<4),
                 Subscript     = (1<<5),
                 Superscript   = (1<<6),
                 Preformatted  = (1<<7),
                 Span          = (1<<8),
                 Div           = (1<<9),
                 Strike        = (1<<10),
                 Underline     = (1<<11),
                 Del           = (1<<12),
                 Ins           = (1<<13),
                 S             = (1<<14),
                 Cite          = (1<<15)
               };

    DocStyleChange(DocParser *parser,DocNodeVariant *parent,size_t position,Style s,
                   const QCString &tagName,bool enable, const HtmlAttribList *attribs=nullptr)
      : DocNode(parser,parent), m_position(position), m_style(s), m_enable(enable)
    {
      if (attribs) m_attribs=*attribs;
      m_tagName = tagName.lower();
    }
    Style style() const                   { return m_style; }
    const char *styleString() const;
    bool enable() const                   { return m_enable; }
    size_t position() const               { return m_position; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    QCString tagName() const              { return m_tagName; }

  private:
    size_t   m_position = 0;
    Style    m_style = Bold;
    bool     m_enable = false;
    HtmlAttribList m_attribs;
    QCString m_tagName;
};

/** Node representing a special symbol */
class DocSymbol : public DocNode
{
  public:
    DocSymbol(DocParser *parser,DocNodeVariant *parent,HtmlEntityMapper::SymType s)
      : DocNode(parser,parent), m_symbol(s) {}
    HtmlEntityMapper::SymType symbol() const     { return m_symbol; }
    static HtmlEntityMapper::SymType decodeSymbol(const QCString &symName);

  private:
    HtmlEntityMapper::SymType  m_symbol = HtmlEntityMapper::Sym_Unknown;
};

/** Node representing an emoji */
class DocEmoji : public DocNode
{
  public:
    DocEmoji(DocParser *parser,DocNodeVariant *parent,const QCString &symName);
    QCString name() const      { return m_symName; }
    int index() const          { return m_index; }

  private:
    QCString m_symName;
    int m_index = 0;
};

/** Node representing some amount of white space */
class DocWhiteSpace : public DocNode
{
  public:
    DocWhiteSpace(DocParser *parser,DocNodeVariant *parent,const QCString &chars)
      : DocNode(parser,parent), m_chars(chars) {}
    QCString chars() const     { return m_chars; }
  private:
    QCString  m_chars;
};

/** Node representing a separator */
class DocSeparator : public DocNode
{
  public:
    DocSeparator(DocParser *parser,DocNodeVariant *parent,const QCString &chars)
      : DocNode(parser,parent), m_chars(chars) {}
    QCString chars() const     { return m_chars; }
  private:
    QCString  m_chars;
};

/** Node representing a verbatim, unparsed text fragment */
class DocVerbatim : public DocNode
{
  public:
    enum Type { Code, HtmlOnly, ManOnly, LatexOnly, RtfOnly, XmlOnly, Verbatim, Dot, Msc, DocbookOnly, PlantUML, JavaDocCode, JavaDocLiteral };
    DocVerbatim(DocParser *parser,DocNodeVariant *parent,const QCString &context,
                const QCString &text, Type t,bool isExample,
                const QCString &exampleFile,bool isBlock=FALSE,const QCString &lang=QCString());
    Type type() const            { return p->type; }
    QCString text() const        { return p->text; }
    QCString context() const     { return p->context; }
    bool isExample() const       { return p->isExample; }
    QCString exampleFile() const { return p->exampleFile; }
    QCString relPath() const     { return p->relPath; }
    QCString language() const    { return p->lang; }
    bool isBlock() const         { return p->isBlock; }
    bool hasCaption() const      { return !p->children.empty(); }
    QCString width() const       { return p->width; }
    QCString height() const      { return p->height; }
    QCString engine() const      { return p->engine; }
    bool useBitmap() const       { return p->useBitmap; }
    const DocNodeList &children() const { return p->children; }
    DocNodeList &children()      { return p->children; }
    QCString srcFile() const     { return p->srcFile; }
    int srcLine() const          { return p->srcLine; }
    void setText(const QCString &t)   { p->text=t;   }
    void setWidth(const QCString &w)  { p->width=w;  }
    void setHeight(const QCString &h) { p->height=h; }
    void setEngine(const QCString &e) { p->engine=e; }
    void setUseBitmap(const bool &u)  { p->useBitmap=u; }
    void setLocation(const QCString &file,int line) { p->srcFile=file; p->srcLine=line; }

  private:
    struct Private
    {
      Private(const QCString &context_,const QCString &text_, Type type_, bool isExample_,
              const QCString &exampleFile_, const QCString &relPath_,const QCString &lang_, bool isBlock_)
        : context(context_),         text(text_),       type(type_),       isExample(isExample_),
          exampleFile(exampleFile_), relPath(relPath_), lang(lang_), isBlock(isBlock_) {}
      QCString  context;
      QCString  text;
      Type      type = Code;
      bool      isExample;
      QCString  exampleFile;
      QCString  relPath;
      QCString  lang;
      bool      isBlock;
      QCString  width;
      QCString  height;
      QCString  engine;
      bool      useBitmap=false; // some PlantUML engines cannot output data in EPS format so bitmap format is required
      DocNodeList children;
      QCString  srcFile;
      int       srcLine = -1;
    };
    std::unique_ptr<Private> p;
};


/** Node representing an included text block from file */
class DocInclude : public DocNode
{
  public:
  enum Type { Include, DontInclude, VerbInclude, HtmlInclude, LatexInclude,
	      IncWithLines, Snippet , SnippetWithLines,
	      DontIncWithLines, RtfInclude, ManInclude, DocbookInclude, XmlInclude
            };
    DocInclude(DocParser *parser,DocNodeVariant *parent,const QCString &file,
               const QCString &context, Type t, bool stripCodeComments,
               bool isExample,const QCString &exampleFile,
               const QCString &blockId, bool isBlock, bool trimLeft)
    : DocNode(parser,parent), m_file(file), m_context(context), m_type(t),
      m_stripCodeComments(stripCodeComments),
      m_isExample(isExample), m_isBlock(isBlock), m_trimLeft(trimLeft),
      m_exampleFile(exampleFile), m_blockId(blockId) {}
    QCString file() const        { return m_file; }
    QCString extension() const   { int i=m_file.findRev('.'); return i!=-1 ? m_file.mid(i) : QCString(); }
    Type type() const            { return m_type; }
    QCString text() const        { return m_text; }
    QCString context() const     { return m_context; }
    QCString blockId() const     { return m_blockId; }
    bool stripCodeComments() const { return m_stripCodeComments; }
    bool isExample() const       { return m_isExample; }
    QCString exampleFile() const { return m_exampleFile; }
    bool isBlock() const         { return m_isBlock; }
    bool trimLeft() const        { return m_trimLeft; }
    void parse();

  private:
    QCString  m_file;
    QCString  m_context;
    QCString  m_text;
    Type      m_type;
    bool      m_stripCodeComments;
    bool      m_isExample;
    bool      m_isBlock;
    bool      m_trimLeft;
    QCString  m_exampleFile;
    QCString  m_blockId;
};

/** Node representing a include/dontinclude operator block */
class DocIncOperator : public DocNode
{
  public:
    enum Type { Line, SkipLine, Skip, Until };
    DocIncOperator(DocParser *parser,DocNodeVariant *parent,Type t,const QCString &pat,
                   const QCString &context,bool stripCodeComments,bool isExample,const QCString &exampleFile)
    : DocNode(parser,parent), m_type(t), m_pattern(pat), m_context(context),
      m_isFirst(FALSE), m_isLast(FALSE), m_stripCodeComments(stripCodeComments),
      m_isExample(isExample), m_exampleFile(exampleFile) {}
    Type type() const           { return m_type; }
    const char *typeAsString() const
    {
      switch(m_type)
      {
        case Line:     return "line";
        case SkipLine: return "skipline";
        case Skip:     return "skip";
        case Until:    return "until";
      }
      return "";
    }
    int line() const             { return m_line; }
    bool showLineNo() const      { return m_showLineNo; }
    QCString text() const        { return m_text; }
    QCString pattern() const     { return m_pattern; }
    QCString context() const     { return m_context; }
    bool isFirst() const         { return m_isFirst; }
    bool isLast() const          { return m_isLast; }
    void markFirst(bool v=TRUE)  { m_isFirst = v; }
    void markLast(bool v=TRUE)   { m_isLast = v; }
    bool stripCodeComments() const { return m_stripCodeComments; }
    bool isExample() const       { return m_isExample; }
    QCString exampleFile() const { return m_exampleFile; }
    QCString includeFileName() const { return m_includeFileName; }
    void parse();

  private:
    Type     m_type = Line;
    int      m_line = 0;
    bool     m_showLineNo = false;
    QCString  m_text;
    QCString  m_pattern;
    QCString  m_context;
    bool     m_isFirst = false;
    bool     m_isLast = false;
    bool     m_stripCodeComments = true;
    bool     m_isExample = false;
    QCString  m_exampleFile;
    QCString m_includeFileName;
};

/** Node representing an item of a cross-referenced list */
class DocFormula : public DocNode
{
  public:
    DocFormula(DocParser *parser,DocNodeVariant *parent,int id);
    QCString name() const       { return m_name; }
    QCString text() const       { return m_text; }
    QCString relPath() const    { return m_relPath; }
    int id() const              { return m_id; }
    bool isInline() const
    {
      if (m_text.length()>1 && m_text.at(0)=='\\' && m_text.at(1)=='[') return false;
      if (m_text.startsWith("\\begin{")) return false;
      return true;
    }

  private:
    QCString  m_name;
    QCString  m_text;
    QCString  m_relPath;
    int      m_id = 0;
};

/** Node representing an entry in the index. */
class DocIndexEntry : public DocNode
{
  public:
    DocIndexEntry(DocParser *parser,DocNodeVariant *parent,const Definition *scope,const MemberDef *md)
      : DocNode(parser,parent), m_scope(scope), m_member(md) {}
    Token parse();
    const Definition *scope() const    { return m_scope;  }
    const MemberDef *member() const    { return m_member; }
    QCString entry() const        { return m_entry;  }

  private:
    QCString     m_entry;
    const Definition *m_scope = nullptr;
    const MemberDef  *m_member = nullptr;
};

//-----------------------------------------------------------------------

/** Node representing an auto List */
class DocAutoList : public DocCompoundNode
{
  public:
    enum ListType
    {
       Unnumbered=1, Unchecked=-2, Checked_x=-3, Checked_X=-4 // positive numbers give the label
    };
    DocAutoList(DocParser *parser,DocNodeVariant *parent,int indent,bool isEnumList,
                         int depth, bool isCheckedList);

    bool isEnumList() const    { return m_isEnumList; }
    int  indent() const        { return m_indent; }
    bool isCheckedList() const { return m_isCheckedList; }
    int depth() const          { return m_depth; }
    Token parse();

  private:
    int      m_indent = 0;
    bool     m_isEnumList = false;
    bool     m_isCheckedList = false;
    int      m_depth = 0;
};

/** Node representing an item of a auto list */
class DocAutoListItem : public DocCompoundNode
{
  public:
    DocAutoListItem(DocParser *parser,DocNodeVariant *parent,int indent,int num);
    int itemNumber() const     { return m_itemNum; }
    Token parse();

  private:
    int m_indent = 0;
    int m_itemNum = 0;
};

/** Node representing a simple section title */
class DocTitle : public DocCompoundNode
{
  public:
    DocTitle(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    void parse();
    void parseFromString(DocNodeVariant *,const QCString &title);
    bool hasTitle() const { return !children().empty(); }

  private:
};

/** Node representing an item of a cross-referenced list */
class DocXRefItem : public DocCompoundNode
{
  public:
    DocXRefItem(DocParser *parser,DocNodeVariant *parent,int id,const QCString &key);
    QCString file() const       { return m_file; }
    QCString anchor() const     { return m_anchor; }
    QCString title() const      { return m_title; }
    QCString relPath() const    { return m_relPath; }
    QCString key() const        { return m_key; }
    bool parse();

  private:
    int      m_id = 0;
    QCString  m_key;
    QCString  m_file;
    QCString  m_anchor;
    QCString  m_title;
    QCString  m_relPath;
};

/** Node representing an image */
class DocImage : public DocCompoundNode
{
  public:
    enum Type { Html, Latex, Rtf, DocBook, Xml };
    DocImage(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,
             const QCString &name,Type t,const QCString &url=QCString(), bool inlineImage = TRUE);
    Type type() const           { return p->type; }
    QCString name() const       { return p->name; }
    bool hasCaption() const     { return !children().empty(); }
    QCString width() const      { return p->width; }
    QCString height() const     { return p->height; }
    QCString relPath() const    { return p->relPath; }
    QCString url() const        { return p->url; }
    bool isInlineImage() const  { return p->inlineImage; }
    bool isSVG() const;
    const HtmlAttribList &attribs() const { return p->attribs; }
    void parse();

  private:
    struct Private
    {
      Private(const HtmlAttribList &attribs_,const QCString &name_,Type type_,
              const QCString &relPath_, const QCString &url_,bool inlineImage_)
        : attribs(attribs_), name(name_), type(type_),
          relPath(relPath_), url(url_),   inlineImage(inlineImage_) {}
      HtmlAttribList attribs;
      QCString  name;
      Type      type = Html;
      QCString  width;
      QCString  height;
      QCString  relPath;
      QCString  url;
      bool      inlineImage;
    };
    std::unique_ptr<Private> p;
};

class DocDiagramFileBase : public DocCompoundNode
{
  public:
    DocDiagramFileBase(DocParser *parser, DocNodeVariant *parent,const QCString &name,
                       const QCString &context, const QCString &srcFile,int srcLine)
    : DocCompoundNode(parser,parent), p(std::make_unique<Private>(name, context, srcFile, srcLine)) {}
    QCString name() const      { return p->name; }
    QCString file() const      { return p->file; }
    QCString relPath() const   { return p->relPath; }
    bool hasCaption() const    { return !children().empty(); }
    QCString width() const     { return p->width; }
    QCString height() const    { return p->height; }
    QCString context() const   { return p->context; }
    QCString srcFile() const   { return p->srcFile; }
    int srcLine() const        { return p->srcLine; }

  protected:
    struct Private
    {
      Private(const QCString &name_,const QCString &context_,const QCString &srcFile_,int srcLine_)
        : name(name_), context(context_), srcFile(srcFile_), srcLine(srcLine_) {}
      QCString  name;
      QCString  file;
      QCString  relPath;
      QCString  width;
      QCString  height;
      QCString  context;
      QCString  srcFile;
      int       srcLine;
    };
    std::unique_ptr<Private> p;
};

/** Node representing a dot file */
class DocDotFile : public DocDiagramFileBase
{
  public:
    DocDotFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
               const QCString &srcFile,int srcLine);
    bool parse();
};

/** Node representing a msc file */
class DocMscFile : public DocDiagramFileBase
{
  public:
    DocMscFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
               const QCString &srcFile,int srcLine);
    bool parse();
};

/** Node representing a dia file */
class DocDiaFile : public DocDiagramFileBase
{
  public:
    DocDiaFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
               const QCString &srcFile,int srcLine);
    bool parse();
};

/** Node representing a uml file */
class DocPlantUmlFile : public DocDiagramFileBase
{
  public:
    DocPlantUmlFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
               const QCString &srcFile,int srcLine);
    bool parse();
};

/** Node representing a VHDL flow chart */
class DocVhdlFlow : public DocCompoundNode
{
  public:
    DocVhdlFlow(DocParser *parser,DocNodeVariant *parent);
    void parse();
    bool hasCaption() const { return !children().empty(); }
  private:
};

/** Node representing a link to some item */
class DocLink : public DocCompoundNode
{
  public:
    DocLink(DocParser *parser,DocNodeVariant *parent,const QCString &target);
    QCString parse(bool,bool isXmlLink=FALSE);
    QCString file() const       { return m_file; }
    QCString relPath() const    { return m_relPath; }
    QCString ref() const        { return m_ref; }
    QCString anchor() const     { return m_anchor; }

  private:
    QCString  m_file;
    QCString  m_relPath;
    QCString  m_ref;
    QCString  m_anchor;
    QCString  m_refText;
};

/** Node representing a reference to some item */
class DocRef : public DocCompoundNode
{
  public:
    DocRef(DocParser *parser,DocNodeVariant *parent,const QCString &target,const QCString &context);
    void parse();
    QCString file() const         { return m_file; }
    QCString relPath() const      { return m_relPath; }
    QCString ref() const          { return m_ref; }
    QCString anchor() const       { return m_anchor; }
    QCString targetTitle() const  { return m_text; }
    SectionType sectionType() const { return m_sectionType; }
    bool hasLinkText() const      { return !children().empty(); }
    bool refToAnchor() const      { return m_refType==Anchor; }
    bool refToSection() const     { return m_refType==Section; }
    bool refToTable() const       { return m_refType==Table; }
    bool isSubPage() const        { return m_isSubPage; }

  private:
    RefType    m_refType = Unknown;
    SectionType m_sectionType = SectionType::Anchor;
    bool       m_isSubPage = false;
    QCString   m_file;
    QCString   m_relPath;
    QCString   m_ref;
    QCString   m_anchor;
    QCString   m_text;
};

/** Node representing an internal reference to some item */
class DocInternalRef : public DocCompoundNode
{
  public:
    DocInternalRef(DocParser *parser,DocNodeVariant *parent,const QCString &target);
    void parse();
    QCString file() const         { return m_file; }
    QCString relPath() const      { return m_relPath; }
    QCString anchor() const       { return m_anchor; }

  private:
    QCString   m_file;
    QCString   m_relPath;
    QCString   m_anchor;
};

/** Node representing a Hypertext reference */
class DocHRef : public DocCompoundNode
{
  public:
    DocHRef(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,const QCString &url,
           const QCString &relPath, const QCString &file)
    : DocCompoundNode(parser,parent), m_attribs(attribs), m_url(url),
      m_relPath(relPath), m_file(file) {}
    Token parse();
    QCString url() const        { return m_url; }
    QCString file() const       { return m_file; }
    QCString relPath() const    { return m_relPath; }
    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    HtmlAttribList m_attribs;
    QCString   m_url;
    QCString   m_relPath;
    QCString   m_file;
};

/** Node Html summary */
class DocHtmlSummary : public DocCompoundNode
{
  public:
    DocHtmlSummary(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs) :
       DocCompoundNode(parser,parent), m_attribs(attribs) {}
    const HtmlAttribList &attribs() const { return m_attribs; }
    void parse();

  private:
    HtmlAttribList m_attribs;
};

/** Node Html details */
class DocHtmlDetails : public DocCompoundNode
{
  public:
    DocHtmlDetails(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs) :
       DocCompoundNode(parser,parent), m_attribs(attribs) {}
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    void parseSummary(DocNodeVariant *,HtmlAttribList &attribs);
    const DocNodeVariant *summary() const { return m_summary.get(); }

  private:
    HtmlAttribList m_attribs;
    std::unique_ptr<DocNodeVariant> m_summary;
};

/** Node Html heading */
class DocHtmlHeader : public DocCompoundNode
{
  public:
    DocHtmlHeader(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,int level) :
       DocCompoundNode(parser,parent), m_level(level), m_attribs(attribs) {}
    int level() const                     { return m_level; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();

  private:
    int           m_level = 0;
    HtmlAttribList m_attribs;
};

/** Node representing a Html description item */
class DocHtmlDescTitle : public DocCompoundNode
{
  public:
    DocHtmlDescTitle(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs) :
      DocCompoundNode(parser,parent), m_attribs(attribs) {}
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();

  private:
    HtmlAttribList m_attribs;
};

/** Node representing a Html description list */
class DocHtmlDescList : public DocCompoundNode
{
  public:
    DocHtmlDescList(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs) :
      DocCompoundNode(parser,parent), m_attribs(attribs) {}
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();

  private:
    HtmlAttribList m_attribs;
};

/** Node representing a normal section */
class DocSection : public DocCompoundNode
{
  public:
    DocSection(DocParser *parser,DocNodeVariant *parent,int level,const QCString &id) :
      DocCompoundNode(parser,parent), m_level(level), m_id(id) {}
    int level() const           { return m_level; }
    const DocNodeVariant *title() const { return m_title.get(); }
    QCString anchor() const     { return m_anchor; }
    QCString id() const         { return m_id; }
    QCString file() const       { return m_file; }
    Token parse();

  private:
    int       m_level = 0;
    QCString  m_id;
    std::unique_ptr<DocNodeVariant> m_title;
    QCString  m_anchor;
    QCString  m_file;
};

/** Node representing a reference to a section */
class DocSecRefItem : public DocCompoundNode
{
  public:
    DocSecRefItem(DocParser *parser,DocNodeVariant *parent,const QCString &target);
    QCString target() const      { return m_target; }
    QCString file() const        { return m_file; }
    QCString anchor() const      { return m_anchor; }
    QCString relPath() const     { return m_relPath; }
    QCString ref() const         { return m_ref; }
    bool refToTable() const      { return m_refType==Table; }
    bool isSubPage() const       { return m_isSubPage; }
    void parse();

  private:
    QCString   m_target;
    RefType    m_refType = Unknown;
    bool       m_isSubPage = false;
    QCString   m_file;
    QCString   m_relPath;
    QCString   m_ref;
    QCString   m_anchor;
};

/** Node representing a list of section references */
class DocSecRefList : public DocCompoundNode
{
  public:
    DocSecRefList(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    void parse();

  private:
};

/** Node representing an internal section of documentation */
class DocInternal : public DocCompoundNode
{
  public:
    DocInternal(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    Token parse(int);

  private:
};

/** Node representing an block of paragraphs */
class DocParBlock : public DocCompoundNode
{
  public:
    DocParBlock(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    Token parse();

  private:
};


/** Node representing a simple list */
class DocSimpleList : public DocCompoundNode
{
  public:
    DocSimpleList(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    Token parse();

  private:
};

/** Node representing a Html list */
class DocHtmlList : public DocCompoundNode
{
  public:
    enum Type { Unordered, Ordered };
    DocHtmlList(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,Type t) :
      DocCompoundNode(parser,parent), m_type(t), m_attribs(attribs) {}
    Type type() const          { return m_type; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    Token parseXml();

  private:
    Type           m_type = Unordered;
    HtmlAttribList m_attribs;
};

/** Node representing a simple section */
class DocSimpleSect : public DocCompoundNode
{
  public:
    enum Type
    {
       Unknown, See, Return, Author, Authors, Version, Since, Date,
       Note, Warning, Copyright, Pre, Post, Invar, Remark, Attention, Important,
       User, Rcs
    };
    DocSimpleSect(DocParser *parser,DocNodeVariant *parent,Type t);
    Type type() const       { return m_type; }
    QCString typeString() const;
    Token parse(bool userTitle,bool needsSeparator);
    Token parseRcs();
    Token parseXml();
    void appendLinkWord(const QCString &word);
    bool hasTitle() const;
    const DocNodeVariant *title() const { return m_title.get(); }

  private:
    Type            m_type = Unknown;
    std::unique_ptr<DocNodeVariant> m_title;
};

/** Node representing a separator between two simple sections of the
 *  same type.
 */
class DocSimpleSectSep : public DocNode
{
  public:
    DocSimpleSectSep(DocParser *parser,DocNodeVariant *parent) : DocNode(parser,parent) {}

  private:
};

/** Node representing a parameter section */
class DocParamSect : public DocCompoundNode
{
    friend class DocParamList;
  public:
    enum Type
    {
       Unknown, Param, RetVal, Exception, TemplateParam
    };
    enum Direction
    {
       In=1, Out=2, InOut=3, Unspecified=0
    };
    DocParamSect(DocParser *parser,DocNodeVariant *parent,Type t)
      : DocCompoundNode(parser,parent), m_type(t), m_hasInOutSpecifier(FALSE), m_hasTypeSpecifier(FALSE)
    {}
    Token parse(const QCString &cmdName,bool xmlContext,Direction d);
    Type type() const          { return m_type; }
    bool hasInOutSpecifier() const { return m_hasInOutSpecifier; }
    bool hasTypeSpecifier() const  { return m_hasTypeSpecifier; }

  private:
    Type            m_type = Unknown;
    bool            m_hasInOutSpecifier = false;
    bool            m_hasTypeSpecifier = false;
};

/** Node representing a paragraph in the documentation tree */
class DocPara : public DocCompoundNode
{
  public:
    DocPara(DocParser *parser,DocNodeVariant *parent);
    Token parse();
    bool isEmpty() const        { return children().empty(); }
    void markFirst(bool v=TRUE) { m_isFirst=v; }
    void markLast(bool v=TRUE)  { m_isLast=v; }
    bool isFirst() const        { return m_isFirst; }
    bool isLast() const         { return m_isLast; }

    Token handleCommand(char cmdChar,const QCString &cmdName);
    Token handleHtmlStartTag(const QCString &tagName,const HtmlAttribList &tagHtmlAttribs);
    Token handleHtmlEndTag(const QCString &tagName);
    Token handleSimpleSection(DocSimpleSect::Type t,bool xmlContext=FALSE);
    Token handleXRefItem();
    Token handleParamSection(const QCString &cmdName,DocParamSect::Type t, bool xmlContext, int direction);
    void handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t);
    template<class T> void handleFile(const QCString &cmdName);
    void handleInclude(const QCString &cmdName,DocInclude::Type t);
    void handleLink(const QCString &cmdName,bool isJavaLink);
    void handleCite(char cmdChar,const QCString &cmdName);
    void handleDoxyConfig(char cmdChar,const QCString &cmdName);
    void handleEmoji(char cmdChar,const QCString &cmdName);
    void handleRef(char cmdChar,const QCString &cmdName);
    void handleSection(char cmdChar,const QCString &cmdName);
    void handleInheritDoc();
    void handleVhdlFlow();
    void handleILine(char cmdChar,const QCString &cmdName);
    void handleIFile(char cmdChar,const QCString &cmdName);
    void handleShowDate(char cmdChar,const QCString &cmdName);
    Token handleStartCode();
    Token handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level);

    bool injectToken(Token tok,const QCString &tokText);
    const HtmlAttribList &attribs() const { return m_attribs; }
    void setAttribs(const HtmlAttribList &attribs) { m_attribs = attribs; }

  private:
    bool      m_isFirst = false;
    bool      m_isLast = false;
    HtmlAttribList m_attribs;
};

/** Node representing a parameter list. */
class DocParamList : public DocNode
{
  public:
    DocParamList(DocParser *parser,DocNodeVariant *parent,DocParamSect::Type t,DocParamSect::Direction d)
      : DocNode(parser,parent), m_type(t), m_dir(d) {}
    const DocNodeList &parameters() const { return m_params; }
    const DocNodeList &paramTypes() const { return m_paramTypes; }
    const DocNodeList &paragraphs() const { return m_paragraphs; }
    DocParamSect::Type type() const { return m_type; }
    DocParamSect::Direction direction() const { return m_dir; }
    void markFirst(bool b=TRUE)     { m_isFirst=b; }
    void markLast(bool b=TRUE)      { m_isLast=b; }
    bool isFirst() const            { return m_isFirst; }
    bool isLast() const             { return m_isLast; }
    Token parse(const QCString &cmdName);
    Token parseXml(const QCString &paramName);

  private:
    DocNodeList             m_paragraphs;
    DocNodeList             m_params;
    DocNodeList             m_paramTypes;
    DocParamSect::Type      m_type = DocParamSect::Unknown;
    DocParamSect::Direction m_dir = DocParamSect::Unspecified;
    bool                    m_isFirst = false;
    bool                    m_isLast = false;
};

/** Node representing a simple list item */
class DocSimpleListItem : public DocNode
{
  public:
    DocSimpleListItem(DocParser *parser,DocNodeVariant *parent);
    Token parse();
    const DocNodeVariant *paragraph() const { return m_paragraph.get(); }

  private:
    std::unique_ptr<DocNodeVariant> m_paragraph;
};

/** Node representing a HTML list item */
class DocHtmlListItem : public DocCompoundNode
{
  public:
    DocHtmlListItem(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,int num)
    : DocCompoundNode(parser,parent), m_attribs(attribs), m_itemNum(num) {}
    int itemNumber() const                { return m_itemNum; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    Token parseXml();

  private:
    HtmlAttribList m_attribs;
    int            m_itemNum = 0;
};

/** Node representing a HTML description data */
class DocHtmlDescData : public DocCompoundNode
{
  public:
    DocHtmlDescData(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent) {}
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();

  private:
    HtmlAttribList m_attribs;
};

/** Node representing a HTML table cell */
class DocHtmlCell : public DocCompoundNode
{
    friend class DocHtmlTable;
  public:
    enum Alignment { Left, Right, Center };
    enum Valignment {Top, Middle, Bottom};
    DocHtmlCell(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,bool isHeading) :
       DocCompoundNode(parser,parent), m_isHeading(isHeading), m_attribs(attribs) {}
    bool isHeading() const      { return m_isHeading; }
    bool isFirst() const        { return m_isFirst; }
    bool isLast() const         { return m_isLast; }
    void markFirst(bool v=TRUE) { m_isFirst=v; }
    void markLast(bool v=TRUE)  { m_isLast=v; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    Token parseXml();
    uint32_t rowIndex() const        { return m_rowIdx; }
    uint32_t columnIndex() const     { return m_colIdx; }
    uint32_t rowSpan() const;
    uint32_t colSpan() const;
    Alignment alignment() const;
    Valignment valignment() const;

  private:
    void setRowIndex(uint32_t idx)    { m_rowIdx = idx; }
    void setColumnIndex(uint32_t idx) { m_colIdx = idx; }
    bool           m_isHeading = false;
    bool           m_isFirst = false;
    bool           m_isLast = false;
    HtmlAttribList m_attribs;
    uint32_t           m_rowIdx = static_cast<uint32_t>(-1);
    uint32_t           m_colIdx = static_cast<uint32_t>(-1);
};

/** Node representing a HTML table caption */
class DocHtmlCaption : public DocCompoundNode
{
  public:
    DocHtmlCaption(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs);
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    bool hasCaptionId() const { return m_hasCaptionId; }
    QCString file() const     { return m_file;         }
    QCString anchor() const   { return m_anchor;       }

  private:
    HtmlAttribList m_attribs;
    bool           m_hasCaptionId = false;
    QCString       m_file;
    QCString       m_anchor;
};

/** Node representing a HTML table row */
class DocHtmlRow : public DocCompoundNode
{
    friend class DocHtmlTable;
  public:
    DocHtmlRow(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
      : DocCompoundNode(parser,parent), m_attribs(attribs) {}
    size_t numCells() const      { return children().size(); }
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    Token parseXml(bool header);
    bool isHeading() const;
    void setVisibleCells(uint32_t n) { m_visibleCells = n; }
    uint32_t visibleCells() const    { return m_visibleCells; }
    uint32_t rowIndex() const        { return m_rowIdx; }

  private:
    void setRowIndex(uint32_t idx)   { m_rowIdx = idx; }
    HtmlAttribList m_attribs;
    uint32_t m_visibleCells = 0;
    uint32_t m_rowIdx = static_cast<uint32_t>(-1);
};

/** Node representing a HTML table */
class DocHtmlTable : public DocCompoundNode
{
  public:
    DocHtmlTable(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
      : DocCompoundNode(parser,parent), m_attribs(attribs) {}
    size_t numRows() const  { return children().size(); }
    bool hasCaption() const;
    const HtmlAttribList &attribs() const { return m_attribs; }
    Token parse();
    Token parseXml();
    size_t numColumns() const { return m_numCols; }
    const DocNodeVariant *caption() const;
    const DocNodeVariant *firstRow() const;

  private:
    void computeTableGrid();
    std::unique_ptr<DocNodeVariant> m_caption;
    HtmlAttribList     m_attribs;
    size_t m_numCols = 0;
};

/** Node representing an HTML blockquote */
class DocHtmlBlockQuote : public DocCompoundNode
{
  public:
    DocHtmlBlockQuote(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
      : DocCompoundNode(parser,parent), m_attribs(attribs) {}
    Token parse();
    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    HtmlAttribList m_attribs;
};

/** Root node of a text fragment */
class DocText : public DocCompoundNode
{
  public:
    DocText(DocParser *parser) : DocCompoundNode(parser,nullptr) {}
    void parse();
    bool isEmpty() const    { return children().empty(); }
};

/** Root node of documentation tree */
class DocRoot : public DocCompoundNode
{
  public:
    DocRoot(DocParser *parser,bool indent,bool sl)
      : DocCompoundNode(parser,nullptr), m_indent(indent), m_singleLine(sl) {}
    void parse();
    bool indent() const { return m_indent; }
    bool singleLine() const { return m_singleLine; }
    bool isEmpty() const { return children().empty(); }

  private:
    bool m_indent = false;
    bool m_singleLine = false;
};

//--------------------------------------------------------------------------------------

/// returns the parent node of a given node \a n or nullptr if the node has no parent.
constexpr DocNodeVariant *parent(DocNodeVariant *n)
{
  return n ? std::visit([](auto &&x)->decltype(auto) { return x.parent(); }, *n) : nullptr;
}

/// returns the parent node of a given node \a n or nullptr if the node has no parent.
constexpr const DocNodeVariant *parent(const DocNodeVariant *n)
{
  return n ? std::visit([](auto &&x)->decltype(auto) { return x.parent(); }, *n) : nullptr;
}

namespace details
{

template<class T,class... Ts>
struct Impl
{
  static constexpr bool holds_one_of_alternatives(const DocNodeVariant &v)
  {
    return std::holds_alternative<T>(v) || Impl<Ts...>::holds_one_of_alternatives(v);
  }
};

template<class T>
struct Impl<T>
{
  static constexpr bool holds_one_of_alternatives(const DocNodeVariant &v)
  {
    return std::holds_alternative<T>(v);
  }
};

} // namespace details

/// returns true iff \a v holds one of types passed as template parameters
template<class... Ts>
constexpr bool holds_one_of_alternatives(const DocNodeVariant &v)
{
  return details::Impl<Ts...>::holds_one_of_alternatives(v);
}

namespace details
{

// Helper type trait to check if a type has member function children(). Default case is false.
template <typename T, typename = void>
struct has_method_children : std::false_type {};

// Use SFINAE to have a partial template specialization derived from std::true_type in case T has method children()
template <typename T>
struct has_method_children<T, std::void_t<decltype(std::declval<T>().children())>> : std::true_type {};

} // namespace details

// Call children() on variant v if the contained type has this method, otherwise return nullptr
inline DocNodeList* call_method_children(DocNodeVariant *v)
{
  return std::visit([](auto&& value) -> DocNodeList* {
    if constexpr (details::has_method_children<decltype(value)>::value) {
      return &value.children();
    } else {
      return nullptr;
    }
  }, *v);
}

//----------------- DocNodeList ---------------------------------------

template<class T,class...Args>
inline void DocNodeList::append(Args&&... args)
{
  // add a DocNodeVariant to the list containing an node T as its active member.
  emplace_back(T(std::forward<Args>(args)...));
  // store a pointer to the variant holding node T inside the node itself.
  // Since DocNodeList is a GrowVector this reference will remain valid even if new
  // elements are added (which would not be the case if a std::vector was used)
  std::get_if<T>(&back())->setThisVariant(&back());
}

template<class T>
inline T *DocNodeList::get_last()
{
  return std::get_if<T>(&back());
}

// ---------------- Debug helpers -------------------------------

#define DN(x)  #x
#define DN_SEP ,
inline const char *docNodeName(const DocNodeVariant &v)
{
  static const char *table[] = { DOC_NODES };
  return table[v.index()];
}
#undef DN
#undef DN_SEP

inline void dumpDocNodeSizes()
{
#define DN(x)  #x
#define DN_SEP ,
  static const char *tableWithNames[] = { DOC_NODES };
#undef DN
#undef DN_SEP

#define DN(x)  sizeof(x)
#define DN_SEP ,
  static size_t tableWithSizes[] = { DOC_NODES };
#undef DN
#undef DN_SEP

  size_t maxSize=0;
  printf("DocNodeVariant(\n");
  for (size_t i=0;i<sizeof(tableWithNames)/sizeof(tableWithNames[0]);i++)
  {
    printf("  /* %2zu */ sizeof(%s)=%zu\n",i,tableWithNames[i],tableWithSizes[i]);
    if (tableWithSizes[i]>maxSize) maxSize = tableWithSizes[i];
  }
  printf(")=%zu\n",maxSize);
}

inline void dumpDocNodeList(const DocNodeList &children)
{
  printf("children=[\n");
  for (const auto &child : children)
  {
    const DocWord *w = std::get_if<DocWord>(&child);
    printf("  %s (%p) %s\n",docNodeName(child),(void*)&child,qPrint(w?w->word():""));
  }
  printf("]\n");
}

//----------------------------------------------------------------------------------

/** Class representing the abstract syntax tree of a documentation block */
class DocNodeAST : public IDocNodeAST
{
  public:
    // Note that r can only be a rvalue, not a general forwarding reference.
    // The compiler will error on lvalues because DotNodeVariant doesn't have a copy constructor
    template<class DocNode>
    DocNodeAST(DocNode &&r) : root(std::move(r))
    {
      std::get_if<DocNode>(&root)->setThisVariant(&root);
    }
    bool isEmpty() const override
    {
      if (std::holds_alternative<DocRoot>(root))
      {
        return std::get<DocRoot>(root).isEmpty();
      }
      else if (std::holds_alternative<DocText>(root))
      {
        return std::get<DocText>(root).isEmpty();
      }
      return false;
    }
    DocNodeVariant root;
};

template<class T,class... Args>
std::unique_ptr<DocNodeVariant> createDocNode(Args&&...args)
{
  auto node = std::make_unique<DocNodeVariant>(T(std::forward<Args>(args)...));
  std::get_if<T>(node.get())->setThisVariant(node.get());
  return node;
}

#endif

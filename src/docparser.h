/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#ifndef _DOCPARSER_H
#define _DOCPARSER_H

#include <stdio.h>

#include <qlist.h>
#include <qstrlist.h>
#include <qstring.h>

#include "docvisitor.h"
#include "htmlattrib.h"

class DocNode;
class MemberDef;
class Definition;
class MemberGroup;
class SectionDict;

//---------------------------------------------------------------------------

/*! Initialize the documentation parser */
void initDocParser();

/*! Main entry point for the documentation parser.
 *  @param fileName  File in which the documentation block is found (or the
 *                   name of the example file in case isExample is TRUE).
 *  @param startLine Line at which the documentation block is found.
 *  @param context   Class or namespace to which this block belongs.
 *  @param md        Member definition to which the documentation belongs.
 *                   Can be 0.
 *  @param input     String representation of the documentation block.
 *  @param indexWords Indicates whether or not words should be put in the 
 *                   search index.
 *  @param isExample TRUE if the documentation belongs to an example.
 *  @param exampleName Base name of the example file (0 if isExample is FALSE).
 *  @param singleLine Output should be presented on a single line, so without
 *                   starting a new paragraph at the end.
 *  @returns         Root node of the abstract syntax tree. Ownership of the
 *                   pointer is handed over to the caller.
 */
DocNode *validatingParseDoc(const char *fileName,int startLine,
                            Definition *context, MemberDef *md,
                            const char *input,bool indexWords,
                            bool isExample,const char *exampleName=0,
                            bool singleLine=FALSE);

/*! Main entry point for parsing simple text fragments. These 
 *  fragments are limited to words, whitespace and symbols.
 */
DocNode *validatingParseText(const char *input);

/*! Searches for section and anchor commands in the input */
void docFindSections(const char *input,
                     Definition *d,
                     MemberGroup *m,
                     const char *fileName);

//---------------------------------------------------------------------------

/*! @brief Abstract node interface with type information. */
class DocNode
{
  public:
    /*! Available node types. */
    enum Kind { Kind_Root           = 0, 
                Kind_Word           = 1, 
                Kind_WhiteSpace     = 2, 
                Kind_Para           = 3, 
                Kind_AutoList       = 4, 
                Kind_AutoListItem   = 5,
                Kind_Symbol         = 6,
                Kind_URL            = 7,
                Kind_StyleChange    = 8,
                Kind_SimpleSect     = 9,
                Kind_Title          = 10,
                Kind_SimpleList     = 11,
                Kind_SimpleListItem = 12,
                Kind_Section        = 13,
                Kind_Verbatim       = 14,
                Kind_XRefItem       = 15,
                Kind_HtmlList       = 16,
                Kind_HtmlListItem   = 17,
                Kind_HtmlDescList   = 18,
                Kind_HtmlDescData   = 19,
                Kind_HtmlDescTitle  = 20,
                Kind_HtmlTable      = 21,
                Kind_HtmlRow        = 22,
                Kind_HtmlCell       = 23,
                Kind_HtmlCaption    = 24,
                Kind_LineBreak      = 25,
                Kind_HorRuler       = 26,
                Kind_Anchor         = 27,
                Kind_IndexEntry     = 28,
                Kind_Internal       = 29,
                Kind_HRef           = 30,
                Kind_Include        = 31,
                Kind_IncOperator    = 32,
                Kind_HtmlHeader     = 33,
                Kind_Image          = 34,
                Kind_DotFile        = 35,
                Kind_Link           = 36,
                Kind_Ref            = 37,
                Kind_Formula        = 38,
                Kind_SecRefItem     = 39,
                Kind_SecRefList     = 40,
                Kind_Language       = 41,
                Kind_LinkedWord     = 42,
                Kind_ParamSect      = 43,
                Kind_ParamList      = 44,
                Kind_InternalRef    = 45,
                Kind_Copy           = 46,
                Kind_Text           = 47
              };
    /*! Creates a new node */
    DocNode() : m_insidePre(FALSE) {}

    /*! Destroys a node. */
    virtual ~DocNode() {}

    /*! Returns the kind of node. Provides runtime type information */
    virtual Kind kind() const = 0;

    /*! Returns the parent of this node or 0 for the root node. */
    virtual DocNode *parent() const = 0;

    /*! Acceptor function for node visitors. Part of the visitor pattern. 
     *  @param v Abstract visitor.
     */
    virtual void accept(DocVisitor *v) = 0;

    /*! Returns TRUE iff this node is inside a preformatted section */
    bool isPreformatted() const { return m_insidePre; }

  protected:
    /*! Sets whether or not this item is inside a preformatted section */
    void setInsidePreformatted(bool p) { m_insidePre = p; }
  private:

    bool m_insidePre;
};

/*! @brief Default accept implementation for compound nodes in the abstract
 *  syntax tree.
 */
template<class T> class CompAccept
{
  public:
    CompAccept() { m_children.setAutoDelete(TRUE); }
    void accept(T *obj, DocVisitor *v) 
    { 
      v->visitPre(obj); 
      QListIterator<DocNode> cli(m_children);
      DocNode *n;
      for (cli.toFirst();(n=cli.current());++cli) n->accept(v);
      v->visitPost(obj); 
    }
  
  protected:
    QList<DocNode> m_children;
};


/*! @brief Node representing a word 
 */
class DocWord : public DocNode
{
  public:
    DocWord(DocNode *parent,const QString &word);
    QString word() const { return m_word; }
    Kind kind() const { return Kind_Word; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QString  m_word;
};

/*! @brief Node representing a word that can be linked to something
 */
class DocLinkedWord : public DocNode
{
  public:
    DocLinkedWord(DocNode *parent,const QString &word,
                  const QString &ref,const QString &file,
                  const QString &anchor);
    QString word() const       { return m_word; }
    Kind kind() const          { return Kind_Word; }
    DocNode *parent() const    { return m_parent; }
    QString file() const       { return m_file; }
    QString relPath() const    { return m_relPath; }
    QString ref() const        { return m_ref; }
    QString anchor() const     { return m_anchor; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QString  m_word;
    QString  m_ref;
    QString  m_file;
    QString  m_relPath;
    QString  m_anchor;
};

/*! @brief Node representing an URL (or email address) */
class DocURL : public DocNode
{
  public:
    DocURL(DocNode *parent,const QString &url,bool isEmail) : 
      m_parent(parent), m_url(url), m_isEmail(isEmail) {}
    QString url() const        { return m_url; }
    Kind kind() const          { return Kind_URL; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }
    bool isEmail() const       { return m_isEmail; }

  private:
    DocNode *m_parent;
    QString  m_url;
    bool m_isEmail;
};

/*! @brief Node representing a line break */
class DocLineBreak : public DocNode
{
  public:
    DocLineBreak(DocNode *parent) : 
      m_parent(parent) {}
    Kind kind() const          { return Kind_LineBreak; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
};

/*! @brief Node representing a horizonal ruler */
class DocHorRuler : public DocNode
{
  public:
    DocHorRuler(DocNode *parent) : 
      m_parent(parent) {}
    Kind kind() const          { return Kind_HorRuler; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
};

/*! @brief Node representing an anchor */
class DocAnchor : public DocNode
{
  public:
    DocAnchor(DocNode *parent,const QString &id,bool newAnchor);
    Kind kind() const          { return Kind_Anchor; }
    DocNode *parent() const    { return m_parent; }
    QString anchor() const     { return m_anchor; }
    QString file() const       { return m_file; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QString  m_anchor;
    QString  m_file;
};

/*! @brief Node representing a style change */
class DocStyleChange : public DocNode
{
  public:
    enum Style { Bold, Italic, Code, Center, Small, 
                 Subscript, Superscript, Preformatted,
                 Span, Div
               };
    DocStyleChange(DocNode *parent,uint position,Style s,bool enable,
                   const HtmlAttribList *attribs=0) : 
      m_parent(parent), m_position(position), m_style(s), m_enable(enable)
      { if (attribs) m_attribs=*attribs; }
    Kind kind() const                     { return Kind_StyleChange; }
    Style style() const                   { return m_style; }
    const char *styleString() const;
    bool enable() const                   { return m_enable; }
    uint position() const                 { return m_position; }
    DocNode *parent() const               { return m_parent; }
    void accept(DocVisitor *v)            { v->visit(this); }
    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    DocNode *m_parent;
    uint     m_position;
    Style    m_style;
    bool     m_enable;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a special symbol */
class DocSymbol : public DocNode
{
  public:
    enum SymType { Unknown=0, BSlash,At,Less,Greater,Amp,Dollar,Hash,Percent, 
                   Copy, Tm, Reg, Apos, Quot, Uml, Acute, Grave, Circ, Tilde, Szlig,
                   Cedil, Ring, Nbsp, Slash
                 };
    DocSymbol(DocNode *parent,SymType s,char letter='\0') : 
      m_parent(parent), m_symbol(s), m_letter(letter) {}
    SymType symbol() const     { return m_symbol; }
    char letter() const        { return m_letter; }
    Kind kind() const          { return Kind_Symbol; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }
    static SymType decodeSymbol(const QString &symName,char *letter);

  private:
    DocNode *m_parent;
    SymType  m_symbol;
    char     m_letter;
};

/*! @brief Node representing some amount of white space */
class DocWhiteSpace : public DocNode
{
  public:
    DocWhiteSpace(DocNode *parent,const QString &chars) : 
      m_parent(parent), m_chars(chars) {}
    Kind kind() const          { return Kind_WhiteSpace; }
    QString chars() const      { return m_chars; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }
  private:
    DocNode *m_parent;
    QString  m_chars;
};

/*! @brief Node representing a verbatim, unparsed text fragment */
class DocVerbatim : public DocNode
{
  public:
    enum Type { Code, HtmlOnly, ManOnly, LatexOnly, XmlOnly, Verbatim, Dot };
    DocVerbatim(DocNode *parent,const QString &context,
                const QString &text, Type t,bool isExample,
                const QString &exampleFile);
    Kind kind() const           { return Kind_Verbatim; }
    Type type() const           { return m_type; }
    QString text() const        { return m_text; }
    QString context() const     { return m_context; }
    DocNode *parent() const     { return m_parent; }
    void accept(DocVisitor *v)  { v->visit(this); }
    bool isExample() const      { return m_isExample; }
    QString exampleFile() const { return m_exampleFile; }
    QString relPath() const    { return m_relPath; }

  private:
    DocNode *m_parent;
    QString  m_context;
    QString  m_text;
    Type     m_type;
    bool     m_isExample;
    QString  m_exampleFile;
    QString  m_relPath;
};


/*! @brief Node representing an included text block from file */
class DocInclude : public DocNode
{
  public:
    enum Type { Include, DontInclude, VerbInclude, HtmlInclude, IncWithLines };
    DocInclude(DocNode *parent,const QString &file,
               const QString context, Type t,
               bool isExample,const QString exampleFile) : 
      m_parent(parent), m_file(file), m_context(context), m_type(t),
      m_isExample(isExample), m_exampleFile(exampleFile) {}
    Kind kind() const           { return Kind_Include; }
    QString file() const        { return m_file; }
    Type type() const           { return m_type; }
    QString text() const        { return m_text; }
    QString context() const     { return m_context; }
    DocNode *parent() const     { return m_parent; }
    bool isExample() const      { return m_isExample; }
    QString exampleFile() const { return m_exampleFile; }
    void accept(DocVisitor *v)  { v->visit(this); }
    void parse();

  private:
    DocNode *m_parent;
    QString  m_file;
    QString  m_context;
    QString  m_text;
    Type     m_type;
    bool     m_isExample;
    QString  m_exampleFile;
};

/*! @brief Node representing a include/dontinclude operator block */
class DocIncOperator : public DocNode
{
  public:
    enum Type { Line, SkipLine, Skip, Until };
    DocIncOperator(DocNode *parent,Type t,const QString &pat,
                   const QString &context,bool isExample,const QString &exampleFile) : 
      m_parent(parent), m_type(t), m_pattern(pat), m_context(context), 
      m_isFirst(FALSE), m_isLast(FALSE),
      m_isExample(isExample), m_exampleFile(exampleFile) {}
    Kind kind() const           { return Kind_IncOperator; }
    Type type() const           { return m_type; }
    QString text() const        { return m_text; }
    QString pattern() const     { return m_pattern; }
    QString context() const     { return m_context; }
    DocNode *parent() const     { return m_parent; }
    void accept(DocVisitor *v)  { v->visit(this); }
    bool isFirst() const        { return m_isFirst; }
    bool isLast() const         { return m_isLast; }
    void markFirst(bool v=TRUE) { m_isFirst = v; }
    void markLast(bool v=TRUE)  { m_isLast = v; }
    bool isExample() const      { return m_isExample; }
    QString exampleFile() const { return m_exampleFile; }
    void parse();

  private:
    DocNode *m_parent;
    Type     m_type;
    QString  m_text;
    QString  m_pattern;
    QString  m_context;
    bool     m_isFirst;
    bool     m_isLast;
    bool     m_isExample;
    QString  m_exampleFile;
};

/*! @brief Node representing an item of a cross-referenced list */
class DocFormula : public DocNode
{
  public:
    DocFormula(DocNode *parent,int id);
    Kind kind() const          { return Kind_Formula; }
    QString name() const       { return m_name; }
    QString text() const       { return m_text; }
    QString relPath() const    { return m_relPath; }
    int id() const             { return m_id; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QString  m_name;
    QString  m_text;
    QString  m_relPath;
    int      m_id;
};

/*! @brief Node representing an entry in the index. */
class DocIndexEntry : public DocNode
{
  public:
    DocIndexEntry(DocNode *parent) : m_parent(parent) { }
    Kind kind() const { return Kind_IndexEntry; }
    int parse();
    DocNode *parent() const      { return m_parent; }
    void accept(DocVisitor *v)   { v->visit(this); }
    QString entry()              { return m_entry; }

  private:
    DocNode *m_parent;
    QString  m_entry;
};

//-----------------------------------------------------------------------

/*! @brief Node representing a entry in the index. */
class DocCopy : public CompAccept<DocCopy>, public DocNode
{
  public:
    DocCopy(DocNode *parent,const QString &link) 
      : m_parent(parent), m_link(link) { }
    Kind kind() const          { return Kind_IndexEntry; }
    QString link() const       { return m_link; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocCopy>::accept(this,v); }
    void parse();

  private:
    DocNode *m_parent;
    QString  m_link;
};

/*! @brief Node representing an auto List */
class DocAutoList : public CompAccept<DocAutoList>, public DocNode
{
  public:
    DocAutoList(DocNode *parent,int indent,bool isEnumList) : 
      m_parent(parent), m_indent(indent), m_isEnumList(isEnumList) {}
    Kind kind() const          { return Kind_AutoList; }
    bool isEnumList() const    { return m_isEnumList; }
    int  indent() const        { return m_indent; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocAutoList>::accept(this,v); }
    int parse();

  private:
    DocNode *m_parent;
    int      m_indent;
    bool     m_isEnumList;
};


/*! @brief Node representing a simple section title */
class DocTitle : public CompAccept<DocTitle>, public DocNode
{
  public:
    DocTitle(DocNode *parent) : m_parent(parent) {}
    void parse();
    void parseFromString(const QString &title);
    Kind kind() const          { return Kind_Title; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocTitle>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! @brief Node representing an item of a cross-referenced list */
class DocXRefItem : public CompAccept<DocXRefItem>, public DocNode
{
  public:
    //enum Type { Bug, Test, Todo, Deprecated };
    DocXRefItem(DocNode *parent,int id,const char *key);
    Kind kind() const          { return Kind_XRefItem; }
    QString file() const       { return m_file; }
    QString anchor() const     { return m_anchor; }
    QString title() const      { return m_title; }
    DocNode *parent() const    { return m_parent; }
    QString relPath() const    { return m_relPath; }
    void accept(DocVisitor *v) { CompAccept<DocXRefItem>::accept(this,v); }
    bool parse();

  private:
    DocNode *m_parent;
    int      m_id;
    QString  m_key;
    QString  m_file;
    QString  m_anchor;
    QString  m_title;
    QString  m_relPath;
};

/*! @brief Node representing an image */
class DocImage : public CompAccept<DocImage>, public DocNode
{
  public:
    enum Type { Html, Latex, Rtf };
    DocImage(DocNode *parent,const HtmlAttribList &attribs,const QString &name,Type t);
    Kind kind() const          { return Kind_Image; }
    Type type() const          { return m_type; }
    QString name() const       { return m_name; }
    DocNode *parent() const    { return m_parent; }
    bool hasCaption() const    { return !m_children.isEmpty(); }
    QString width() const      { return m_width; }
    QString height() const     { return m_height; }
    QString relPath() const    { return m_relPath; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    void accept(DocVisitor *v) { CompAccept<DocImage>::accept(this,v); }
    void parse();

  private:
    DocNode *m_parent;
    HtmlAttribList m_attribs;
    QString  m_name;
    Type     m_type;
    QString  m_width;
    QString  m_height;
    QString  m_relPath;
};

/*! @brief Node representing a dot file */
class DocDotFile : public CompAccept<DocDotFile>, public DocNode
{
  public:
    DocDotFile(DocNode *parent,const QString &name);
    void parse();
    Kind kind() const          { return Kind_DotFile; }
    QString name() const       { return m_name; }
    QString file() const       { return m_file; }
    QString relPath() const    { return m_relPath; }
    bool hasCaption() const    { return !m_children.isEmpty(); }
    QString width() const      { return m_width; }
    QString height() const     { return m_height; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocDotFile>::accept(this,v); }
  private:
    DocNode *m_parent;
    QString  m_name;
    QString  m_file;
    QString  m_relPath;
    QString  m_width;
    QString  m_height;
};

/*! @brief Node representing a link to some item */
class DocLink : public CompAccept<DocLink>, public DocNode
{
  public:
    DocLink(DocNode *parent,const QString &target);
    QString parse(bool);
    Kind kind() const          { return Kind_Link; }
    QString file() const       { return m_file; }
    QString relPath() const    { return m_relPath; }
    QString ref() const        { return m_ref; }
    QString anchor() const     { return m_anchor; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocLink>::accept(this,v); }

  private:
    DocNode *m_parent;
    QString  m_file;
    QString  m_relPath;
    QString  m_ref;
    QString  m_anchor;
    QString  m_refText;
};

/*! @brief Node representing a reference to some item */
class DocRef : public CompAccept<DocRef>, public DocNode
{
  public:
    DocRef(DocNode *parent,const QString &target);
    void parse();
    Kind kind() const            { return Kind_Ref; }
    QString file() const         { return m_file; }
    QString relPath() const      { return m_relPath; }
    QString ref() const          { return m_ref; }
    QString anchor() const       { return m_anchor; }
    QString targetTitle() const  { return m_text; }
    DocNode *parent() const      { return m_parent; }
    bool hasLinkText() const     { return !m_children.isEmpty(); }
    bool refToAnchor() const     { return m_refToAnchor; }
    bool refToSection() const    { return m_refToSection; }
    void accept(DocVisitor *v)   { CompAccept<DocRef>::accept(this,v); }

  private:
    DocNode * m_parent;
    bool      m_refToSection;
    bool      m_refToAnchor;
    QString   m_file;
    QString   m_relPath;
    QString   m_ref;
    QString   m_anchor;
    QString   m_text;
};

/*! @brief Node representing an internal reference to some item */
class DocInternalRef : public CompAccept<DocInternalRef>, public DocNode
{
  public:
    DocInternalRef(DocNode *parent,const QString &target);
    void parse();
    Kind kind() const            { return Kind_Ref; }
    QString file() const         { return m_file; }
    QString relPath() const      { return m_relPath; }
    QString anchor() const       { return m_anchor; }
    DocNode *parent() const      { return m_parent; }
    void accept(DocVisitor *v)   { CompAccept<DocInternalRef>::accept(this,v); }

  private:
    DocNode * m_parent;
    QString   m_file;
    QString   m_relPath;
    QString   m_anchor;
};

/*! @brief Node representing a Language specific section */
class DocLanguage : public CompAccept<DocLanguage>, public DocNode
{
  public:
    DocLanguage(DocNode *parent,const QString &id) : 
      m_parent(parent), m_id(id) {}
    QString id() const         { return m_id; }
    Kind kind() const          { return Kind_Language; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocLanguage>::accept(this,v); }
    int parse();
    
  private:
    DocNode * m_parent;
    QString   m_id;
};

/*! @brief Node representing a Hypertext reference */
class DocHRef : public CompAccept<DocHRef>, public DocNode
{
  public:
    DocHRef(DocNode *parent,const HtmlAttribList &attribs,const QString &url) : 
      m_parent(parent), m_attribs(attribs), m_url(url) {}
    int parse();
    QString url() const        { return m_url; }
    Kind kind() const          { return Kind_HRef; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHRef>::accept(this,v); }
    const HtmlAttribList &attribs() const { return m_attribs; }

  private:
    DocNode * m_parent;
    HtmlAttribList m_attribs;
    QString   m_url;
};

/*! @brief Node Html heading */
class DocHtmlHeader : public CompAccept<DocHtmlHeader>, public DocNode
{
  public:
    DocHtmlHeader(DocNode *parent,const HtmlAttribList &attribs,int level) : 
       m_parent(parent), m_level(level), m_attribs(attribs) {}
    int level() const                     { return m_level; }
    Kind kind() const                     { return Kind_HtmlHeader; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    DocNode *parent() const               { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlHeader>::accept(this,v); }
    int parse();

  private:
    DocNode *     m_parent;
    int           m_level;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a Html description item */
class DocHtmlDescTitle : public CompAccept<DocHtmlDescTitle>, public DocNode
{
  public:
    DocHtmlDescTitle(DocNode *parent,const HtmlAttribList &attribs) : 
      m_parent(parent), m_attribs(attribs) {}
    Kind kind() const                     { return Kind_HtmlDescTitle; }
    DocNode *parent() const               { return m_parent; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescTitle>::accept(this,v); }
    int parse();

  private:
    DocNode *     m_parent;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a Html description list */
class DocHtmlDescList : public CompAccept<DocHtmlDescList>, public DocNode
{
  public:
    DocHtmlDescList(DocNode *parent,const HtmlAttribList &attribs) 
      : m_parent(parent), m_attribs(attribs) {}
    Kind kind() const                     { return Kind_HtmlDescList; }
    DocNode *parent() const               { return m_parent; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescList>::accept(this,v); }
    int parse();

  private:
    DocNode *     m_parent;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a normal section */
class DocSection : public CompAccept<DocSection>, public DocNode
{
  public:
    DocSection(DocNode *parent,int level,const QString &id) :
      m_parent(parent), m_level(level), m_id(id) {} 
    Kind kind() const          { return Kind_Section; }
    int level() const          { return m_level; }
    QString title() const      { return m_title; }
    QString anchor() const     { return m_anchor; }
    QString id() const         { return m_id; }
    QString file() const       { return m_file; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSection>::accept(this,v); }
    int parse();

  private:
    DocNode *m_parent;
    int      m_level;
    QString  m_id;
    QString  m_title;
    QString  m_anchor;
    QString  m_file;
};

/*! @brief Node representing a reference to a section */
class DocSecRefItem : public CompAccept<DocSecRefItem>, public DocNode
{
  public:
    DocSecRefItem(DocNode *parent,const QString &target) : 
      m_parent(parent), m_target(target) {}
    Kind kind() const          { return Kind_SecRefItem; }
    QString target() const     { return m_target; }
    QString file() const       { return m_file; }
    QString anchor() const     { return m_anchor; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSecRefItem>::accept(this,v); }
    void parse();

  private:
    DocNode *m_parent;
    QString  m_target;
    QString  m_file;
    QString  m_anchor;
};

/*! @brief Node representing a list of section references */
class DocSecRefList : public CompAccept<DocSecRefList>, public DocNode
{
  public:
    DocSecRefList(DocNode *parent) : m_parent(parent) {}
    void parse();
    Kind kind() const          { return Kind_SecRefList; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSecRefList>::accept(this,v); }

  private:
    DocNode  *m_parent;
};

/*! @brief Node representing an internal section of documentation */
class DocInternal : public CompAccept<DocInternal>, public DocNode
{
  public:
    DocInternal(DocNode *parent) : m_parent(parent) {}
    int parse(int);
    Kind kind() const          { return Kind_Internal; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocInternal>::accept(this,v); }

  private:
    DocNode  *m_parent;
};

/*! @brief Node representing a simple list */
class DocSimpleList : public CompAccept<DocSimpleList>, public DocNode
{
  public:
    DocSimpleList(DocNode *parent) : m_parent(parent) {}
    Kind kind() const          { return Kind_SimpleList; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSimpleList>::accept(this,v); }
    int parse();

  private:
    DocNode *m_parent;
};

/*! @brief Node representing a Html list */
class DocHtmlList : public CompAccept<DocHtmlList>, public DocNode
{
  public:
    enum Type { Unordered, Ordered };
    DocHtmlList(DocNode *parent,const HtmlAttribList &attribs,Type t) : 
      m_parent(parent), m_type(t), m_attribs(attribs) {}
    Kind kind() const          { return Kind_HtmlList; }
    Type type() const          { return m_type; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlList>::accept(this,v); }
    const HtmlAttribList &attribs() const { return m_attribs; }
    int parse();

  private:
    DocNode *     m_parent;
    Type          m_type;
    HtmlAttribList m_attribs;
};

/*! Node representing a simple section */
class DocSimpleSect : public CompAccept<DocSimpleSect>, public DocNode
{
  public:
    enum Type 
    {  
       Unknown, See, Return, Author, Authors, Version, Since, Date,
       Note, Warning, Pre, Post, Invar, Remark, Attention, User, Rcs
    };
    DocSimpleSect(DocNode *parent,Type t);
    virtual ~DocSimpleSect();
    Kind kind() const       { return Kind_SimpleSect; }
    Type type() const       { return m_type; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v);
    int parse(bool userTitle);
    int parseRcs();

  private:
    DocNode *       m_parent;
    Type            m_type;
    DocTitle *      m_title;
};

/*! Node representing a parameter section */
class DocParamSect : public CompAccept<DocParamSect>, public DocNode
{
  public:
    enum Type 
    {  
       Unknown, Param, RetVal, Exception 
    };
    enum Direction
    {
       In=1, Out=2, InOut=3, Unspecified=0
    };
    DocParamSect(DocNode *parent,Type t) 
      : m_parent(parent), m_type(t) {}
    int parse(const QString &cmdName,Direction d);
    Kind kind() const          { return Kind_ParamSect; }
    Type type() const          { return m_type; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocParamSect>::accept(this,v); }

  private:
    DocNode *       m_parent;
    Type            m_type;
    Direction       m_dir;
};

/*! Node representing a paragraph in the documentation tree */
class DocPara : public CompAccept<DocPara>, public DocNode
{
  public:
    DocPara(DocNode *parent) : m_parent(parent), 
             m_isFirst(FALSE), m_isLast(FALSE) {}
    int parse();
    Kind kind() const           { return Kind_Para; }
    DocNode *parent() const     { return m_parent; }
    bool isEmpty() const        { return m_children.isEmpty(); }
    void accept(DocVisitor *v)  { CompAccept<DocPara>::accept(this,v); }
    void markFirst(bool v=TRUE) { m_isFirst=v; }
    void markLast(bool v=TRUE)  { m_isLast=v; }
    bool isFirst() const        { return m_isFirst; }
    bool isLast() const         { return m_isLast; }
    const QList<DocNode> &children() const { return m_children; }

    int handleCommand(const QString &cmdName);
    int handleHtmlStartTag(const QString &tagName,const HtmlAttribList &tagHtmlAttribs);
    int handleHtmlEndTag(const QString &tagName);
    int handleSimpleSection(DocSimpleSect::Type t);
    int handleXRefItem();
    int handleParamSection(const QString &cmdName,DocParamSect::Type t,
                           int direction);
    void handleIncludeOperator(const QString &cmdName,DocIncOperator::Type t);
    void handleImage(const QString &cmdName);
    void handleDotFile(const QString &cmdName);
    void handleInclude(const QString &cmdName,DocInclude::Type t);
    void handleLink(const QString &cmdName,bool isJavaLink);
    void handleRef(const QString &cmdName);
    void handleSection(const QString &cmdName);
    int handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level);
    int handleLanguageSwitch();

  private:
    DocNode *m_parent;
    QString  m_sectionId;
    bool     m_isFirst;
    bool     m_isLast;
};

/*! @brief Node representing a parameter list. */
class DocParamList : public DocNode
{
  public:
    DocParamList(DocNode *parent,DocParamSect::Type t,DocParamSect::Direction d) 
      : m_parent(parent) , m_type(t), m_dir(d), m_isFirst(TRUE), m_isLast(TRUE)
    { m_paragraph=new DocPara(this); }
    virtual ~DocParamList()         { delete m_paragraph; }
    Kind kind() const               { return Kind_ParamList; }
    DocNode *parent() const         { return m_parent; }
    const QStrList &parameters()    { return m_params; }
    DocParamSect::Type type() const { return m_type; }
    DocParamSect::Direction direction() const { return m_dir; }
    void markFirst(bool b=TRUE)     { m_isFirst=b; }
    void markLast(bool b=TRUE)      { m_isLast=b; }
    bool isFirst() const            { return m_isFirst; }
    bool isLast() const             { return m_isLast; }
    void accept(DocVisitor *v)
    { 
      v->visitPre(this); 
      m_paragraph->accept(v);
      v->visitPost(this); 
    }
    int parse(const QString &cmdName);

  private:
    DocNode *               m_parent;
    DocPara *               m_paragraph;
    QStrList                m_params;
    DocParamSect::Type      m_type;
    DocParamSect::Direction m_dir;
    bool                    m_isFirst;
    bool                    m_isLast;
};

/*! @brief Node representing an item of a auto list */
class DocAutoListItem : public DocNode
{
  public:
    DocAutoListItem(DocNode *parent,int num) : m_parent(parent), m_itemNum(num)
    { m_paragraph=new DocPara(this); }
    virtual ~DocAutoListItem() { delete m_paragraph; }
    Kind kind() const          { return Kind_AutoListItem; }
    DocNode *parent() const    { return m_parent; }
    int itemNumber() const     { return m_itemNum; }
    void accept(DocVisitor *v) 
    { 
      v->visitPre(this); 
      m_paragraph->accept(v);
      v->visitPost(this); 
    }
    int parse();

  private:
    DocNode *m_parent;
    DocPara *m_paragraph;
    int m_itemNum;
};

/*! @brief Node representing a simple list item */
class DocSimpleListItem : public DocNode
{
  public:
    DocSimpleListItem(DocNode *parent) : m_parent(parent) 
    { m_paragraph=new DocPara(this); }
    int parse();
    virtual ~DocSimpleListItem() { delete m_paragraph; }
    Kind kind() const            { return Kind_SimpleListItem; }
    DocNode *parent() const      { return m_parent; }
    void accept(DocVisitor *v)
    {
      v->visitPre(this); 
      m_paragraph->accept(v);
      v->visitPost(this); 
    }

  private:
    DocNode *m_parent;
    DocPara *m_paragraph;
};

/*! @brief Node representing a HTML list item */
class DocHtmlListItem : public CompAccept<DocHtmlListItem>, public DocNode
{
  public:
    DocHtmlListItem(DocNode *parent,const HtmlAttribList &attribs,int num) : 
      m_parent(parent), m_attribs(attribs), m_itemNum(num) {}
    Kind kind() const                     { return Kind_HtmlListItem; }
    int itemNumber() const                { return m_itemNum; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    DocNode *parent() const               { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlListItem>::accept(this,v); }
    int parse();

  private:
    DocNode *      m_parent;
    HtmlAttribList m_attribs;
    int            m_itemNum;
};

/*! @brief Node representing a HTML description data */
class DocHtmlDescData : public CompAccept<DocHtmlDescData>, public DocNode
{
  public:
    DocHtmlDescData(DocNode *parent) : 
      m_parent(parent) {}
    Kind kind() const                     { return Kind_HtmlDescData; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    DocNode *parent() const               { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescData>::accept(this,v); }
    int parse();

  private:
    DocNode *     m_parent;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a HTML table cell */
class DocHtmlCell : public CompAccept<DocHtmlCell>, public DocNode
{
  public:
    DocHtmlCell(DocNode *parent,const HtmlAttribList &attribs,bool isHeading) : 
       m_parent(parent), m_isHeading(isHeading), 
       m_isFirst(FALSE), m_isLast(FALSE), m_attribs(attribs) {}
    bool isHeading() const      { return m_isHeading; }
    bool isFirst() const        { return m_isFirst; }
    bool isLast() const         { return m_isLast; }
    Kind kind() const           { return Kind_HtmlCell; }
    DocNode *parent() const     { return m_parent; }
    void accept(DocVisitor *v)  { CompAccept<DocHtmlCell>::accept(this,v); }
    void markFirst(bool v=TRUE) { m_isFirst=v; }
    void markLast(bool v=TRUE)  { m_isLast=v; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    int parse();

  private:
    DocNode *      m_parent;
    bool           m_isHeading;
    bool           m_isFirst;
    bool           m_isLast;
    HtmlAttribList  m_attribs;
};

/*! @brief Node representing a HTML table caption */
class DocHtmlCaption : public CompAccept<DocHtmlCaption>, public DocNode
{
  public:
    DocHtmlCaption(DocNode *parent,const HtmlAttribList &attribs) : 
      m_parent(parent), m_attribs(attribs) {}
    Kind kind() const          { return Kind_HtmlCaption; }
    DocNode *parent() const    { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlCaption>::accept(this,v); }
    const HtmlAttribList &attribs() const { return m_attribs; }
    int parse();

  private:
    DocNode *     m_parent;
    HtmlAttribList m_attribs;
    bool           m_atTop;
};

/*! @brief Node representing a HTML table row */
class DocHtmlRow : public CompAccept<DocHtmlRow>, public DocNode
{
  public:
    DocHtmlRow(DocNode *parent,const HtmlAttribList &attribs) 
      : m_parent(parent), m_attribs(attribs) {}
    Kind kind() const          { return Kind_HtmlRow; }
    DocNode *parent() const    { return m_parent; }
    uint numCells() const      { return m_children.count(); }
    void accept(DocVisitor *v) { CompAccept<DocHtmlRow>::accept(this,v); }
    const HtmlAttribList &attribs() const { return m_attribs; }
    int parse();

  private:
    DocNode *  m_parent;
    HtmlAttribList m_attribs;
};

/*! @brief Node representing a HTML table */
class DocHtmlTable : public CompAccept<DocHtmlTable>, public DocNode
{
  public:
    DocHtmlTable(DocNode *parent,const HtmlAttribList &attribs) 
      : m_parent(parent), m_attribs(attribs) 
    { m_caption=0; }
    ~DocHtmlTable()         { delete m_caption; }
    Kind kind() const       { return Kind_HtmlTable; }
    DocNode *parent() const { return m_parent; }
    uint numRows() const    { return m_children.count(); }
    bool hasCaption()       { return m_caption!=0; }
    const HtmlAttribList &attribs() const { return m_attribs; }
    int parse();
    uint numCols() const;
    void accept(DocVisitor *v);

  private:
    DocNode *          m_parent;
    DocHtmlCaption    *m_caption;
    HtmlAttribList         m_attribs;
};

/*! @brief Root node of a text fragment */
class DocText : public CompAccept<DocText>, public DocNode
{
  public:
    DocText() {}
    Kind kind() const       { return Kind_Text; }
    DocNode *parent() const { return 0; }
    void accept(DocVisitor *v) { CompAccept<DocText>::accept(this,v); }
    void parse();
};

/*! @brief Root node of documentation tree */
class DocRoot : public CompAccept<DocRoot>, public DocNode
{
  public:
    DocRoot(bool indent,bool sl) : m_indent(indent), m_singleLine(sl) {}
    Kind kind() const       { return Kind_Root; }
    DocNode *parent() const { return 0; }
    void accept(DocVisitor *v) { CompAccept<DocRoot>::accept(this,v); }
    void parse();
    bool indent() const { return m_indent; }
    bool singleLine() const { return m_singleLine; }

  private:
    bool m_indent;
    bool m_singleLine;
};


#endif

/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include <qlist.h>
#include <qstrlist.h>
#include <stdio.h>

#include "docvisitor.h"
#include "doctokenizer.h"

class DocNode;

//---------------------------------------------------------------------------

/*! Main entry point for the documentation parser.
 *  @param fileName  File in which the documentation block is found.
 *  @param startLine Line at which the documentation block is found.
 *  @param context   Class or namespace in which of the item to which this
 *                   block belongs.
 *  @param input     String representation of the documentation block.
 *  @returns         Root node of the abstract syntax tree. Ownership of the
 *                   pointer is handed over to the caller.
 */
DocNode *validatingParseDoc(const char *fileName,int startLine,
                            const char *context, const char *input);

//---------------------------------------------------------------------------

/*! Abstract node interface with type information. */
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
                Kind_HtmlPre        = 18,
                Kind_HtmlDescList   = 19,
                Kind_HtmlDescData   = 20,
                Kind_HtmlDescTitle  = 21,
                Kind_HtmlTable      = 22,
                Kind_HtmlRow        = 23,
                Kind_HtmlCell       = 24,
                Kind_HtmlCaption    = 25,
                Kind_LineBreak      = 26,
                Kind_HorRuler       = 27,
                Kind_Anchor         = 28,
                Kind_IndexEntry     = 29,
                Kind_Internal       = 30,
                Kind_HRef           = 31,
                Kind_Copy           = 32,
                Kind_Include        = 33,
                Kind_IncOperator    = 34,
                Kind_HtmlHeader     = 35,
                Kind_Image          = 36,
                Kind_DotFile        = 37,
                Kind_Link           = 38,
                Kind_Ref            = 39,
                Kind_Formula        = 40,
                Kind_SecRefItem     = 41,
                Kind_SecRefList     = 42,
                Kind_Language       = 43,
                Kind_LinkedWord     = 44,
                Kind_ParamSect      = 45,
                Kind_ParamList      = 46,
                Kind_InternalRef    = 47
              };
    /*! Destructor. */
    virtual ~DocNode() {}
    /*! Returns the kind of node. Provides runtime type information */
    virtual Kind kind() const = 0;
    /*! Returns the parent of this node or 0 for the root node. */
    virtual DocNode *parent() const = 0;
    /*! Acceptor function for node visitors. Part of the visitor pattern. 
     *  @param v Abstract visitor.
     */
    virtual void accept(DocVisitor *v) = 0;
};

/*! Default accept implementation for compound nodes in the abstract
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


/*! Node representing a word 
 */
class DocWord : public DocNode
{
  public:
    DocWord(DocNode *parent,const QCString &word) : 
      m_parent(parent), m_word(word) {}
    QCString word() const { return m_word; }
    Kind kind() const { return Kind_Word; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_word;
};

/*! Node representing a word that can be linked to something
 */
class DocLinkedWord : public DocNode
{
  public:
    DocLinkedWord(DocNode *parent,const QCString &word,
                  const QCString &ref,const QCString &file,
                  const QCString &anchor) : 
      m_parent(parent), m_word(word), m_ref(ref), 
      m_file(file), m_anchor(anchor) {}
    QCString word() const      { return m_word; }
    Kind kind() const          { return Kind_Word; }
    DocNode *parent() const    { return m_parent; }
    QCString file() const      { return m_file; }
    QCString ref() const       { return m_ref; }
    QCString anchor() const    { return m_anchor; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_word;
    QCString m_ref;
    QCString m_file;
    QCString m_anchor;
};

/*! Node representing an URL (or email address) */
class DocURL : public DocNode
{
  public:
    DocURL(DocNode *parent,const QCString &url) : 
      m_parent(parent), m_url(url) {}
    QCString url() const { return m_url; }
    Kind kind() const { return Kind_URL; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_url;
};

/*! Node representing a line break */
class DocLineBreak : public DocNode
{
  public:
    DocLineBreak(DocNode *parent) : 
      m_parent(parent) {}
    Kind kind() const { return Kind_LineBreak; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
};

/*! Node representing a horizonal ruler */
class DocHorRuler : public DocNode
{
  public:
    DocHorRuler(DocNode *parent) : 
      m_parent(parent) {}
    Kind kind() const { return Kind_HorRuler; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
};

/*! Node representing an anchor */
// TODO: not just ignore </a>...
class DocAnchor : public DocNode
{
  public:
    DocAnchor(DocNode *parent,const QCString &anchor) : 
      m_parent(parent), m_anchor(anchor) {}
    Kind kind() const { return Kind_Anchor; }
    DocNode *parent() const { return m_parent; }
    QCString anchor() const { return m_anchor; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_anchor;
};

/*! Node representing a style change */
class DocStyleChange : public DocNode
{
  public:
    enum Style { Bold, Italic, Code, Center, Small, 
                 Subscript, Superscript 
               };
    DocStyleChange(DocNode *parent,uint position,Style s,bool enable) : 
      m_parent(parent), m_position(position), m_style(s), m_enable(enable) {}
    Kind kind() const { return Kind_StyleChange; }
    Style style() const { return m_style; }
    bool enable() const { return m_enable; }
    uint position() const { return m_position; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    uint m_position;
    Style m_style;
    bool m_enable;
};

/*! Node representing a special symbol */
class DocSymbol : public DocNode
{
  public:
    enum SymType { Unknown=0, BSlash,At,Less,Greater,Amp,Dollar,Hash,Percent, 
                   Copy, Apos, Quot, Uml, Acute, Grave, Circ, Tilde, Szlig,
                   Cedil, Ring, Nbsp
                 };
    DocSymbol(DocNode *parent,SymType s,char letter='\0') : 
      m_parent(parent), m_symbol(s), m_letter(letter) {}
    SymType symbol() const { return m_symbol; }
    char letter() const { return m_letter; }
    Kind kind() const { return Kind_Symbol; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }
    static SymType decodeSymbol(const QCString &symName,char *letter);

  private:
    DocNode *m_parent;
    SymType m_symbol;
    char m_letter;
};

/*! Node representing some amount of white space */
class DocWhiteSpace : public DocNode
{
  public:
    DocWhiteSpace(DocNode *parent,const QCString &chars) : 
      m_parent(parent), m_chars(chars) {}
    Kind kind() const { return Kind_WhiteSpace; }
    QCString chars() const { return m_chars; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }
  private:
    DocNode *m_parent;
    QCString m_chars;
};

/*! Verbatim, unparsed text fragment */
// TODO: parse code
class DocVerbatim : public DocNode
{
  public:
    enum Type { Code, HtmlOnly, LatexOnly, Verbatim };
    DocVerbatim(DocNode *parent,const QCString &context,
                const QCString &text, Type t) : 
      m_parent(parent), m_context(context), m_text(text), m_type(t) {}
    Kind kind() const { return Kind_Verbatim; }
    Type type() const { return m_type; }
    QCString text() const { return m_text; }
    QCString context() const { return m_context; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_context;
    QCString m_text;
    Type m_type;
};

/*! "to be copied" documentation block */
// TODO: Find the matching block and insert it.
//       (This should be a separate pass, or can we do it in one pass?).
class DocCopy : public DocNode
{
  public:
    DocCopy(DocNode *parent,const QCString &link) : 
      m_parent(parent), m_link(link) {}
    Kind kind() const { return Kind_Copy; }
    QCString link() const { return m_link; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_link;
};

/*! include/dontinclude block */
// TODO: locate the file and handle it.
class DocInclude : public DocNode
{
  public:
    enum Type { Include, DontInclude, VerbInclude, HtmlInclude };
    DocInclude(DocNode *parent,const QCString &file,Type t) : 
      m_parent(parent), m_file(file), m_type(t) {}
    Kind kind() const { return Kind_Include; }
    QCString file() const { return m_file; }
    Type type() const { return m_type; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_file;
    Type     m_type;
};

/*! include/dontinclude operator block */
// TODO: check which file we should be working on and do the operation
class DocIncOperator : public DocNode
{
  public:
    enum Type { Line, SkipLine, Skip, Until };
    DocIncOperator(DocNode *parent,Type t,const QCString &pat) : 
      m_parent(parent), m_type(t), m_pattern(pat) {}
    Kind kind() const { return Kind_IncOperator; }
    Type type() const { return m_type; }
    QCString pattern() const { return m_pattern; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    Type m_type;
    QCString m_pattern;
};

/*! Item of a cross-referenced list */
class DocFormula : public DocNode
{
  public:
    DocFormula(DocNode *parent,int id);
    Kind kind() const { return Kind_Formula; }
    QCString name() const { return m_name; }
    QCString text() const { return m_text; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { v->visit(this); }

  private:
    DocNode *m_parent;
    QCString m_name;
    QCString m_text;
};

//-----------------------------------------------------------------------

/*! Node representing a entry in the index. */
class DocIndexEntry : public CompAccept<DocIndexEntry>, public DocNode
{
  public:
    DocIndexEntry(DocNode *parent) : m_parent(parent) { }
    Kind kind() const { return Kind_IndexEntry; }
    int parse();
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocIndexEntry>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! Auto List */
class DocAutoList : public CompAccept<DocAutoList>, public DocNode
{
  public:
    DocAutoList(DocNode *parent,int indent,bool isEnumList) : 
      m_parent(parent), m_indent(indent), m_isEnumList(isEnumList) {}
    Kind kind() const { return Kind_AutoList; }
    int parse();
    bool isEnumList() const { return m_isEnumList; }
    int  indent() const { return m_indent; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocAutoList>::accept(this,v); }

  private:
    DocNode *m_parent;
    int m_indent;
    bool m_isEnumList;
};


/*! Simple section title */
class DocTitle : public CompAccept<DocTitle>, public DocNode
{
  public:
    DocTitle(DocNode *parent) : m_parent(parent) {}
    void parse();
    Kind kind() const { return Kind_Title; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocTitle>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! Item of a cross-referenced list */
class DocXRefItem : public CompAccept<DocXRefItem>, public DocNode
{
  public:
    enum Type { Bug, Test, Todo, Deprecated };
    DocXRefItem(DocNode *parent,int id,Type t) : 
      m_parent(parent), m_id(id), m_type(t) {}
    Kind kind() const { return Kind_XRefItem; }
    Type type() const { return m_type; }
    QCString file() const { return m_file; }
    QCString anchor() const { return m_anchor; }
    QCString title() const { return m_title; }
    void parse();
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocXRefItem>::accept(this,v); }

  private:
    DocNode *m_parent;
    int  m_id;
    Type m_type;
    QCString m_file;
    QCString m_anchor;
    QCString m_title;
};

/*! Image */
class DocImage : public CompAccept<DocImage>, public DocNode
{
  public:
    enum Type { Html, Latex, Rtf };
    DocImage(DocNode *parent,const QCString &name,Type t) : 
      m_parent(parent), m_name(name), m_type(t) {}
    void parse();
    Kind kind() const { return Kind_Image; }
    Type type() const { return m_type; }
    QCString name() const { return m_name; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocImage>::accept(this,v); }

  private:
    DocNode  *m_parent;
    QCString  m_name;
    Type      m_type;
};

/*! Node representing a dot file */
class DocDotFile : public CompAccept<DocDotFile>, public DocNode
{
  public:
    DocDotFile(DocNode *parent,const QCString &name) : 
      m_parent(parent), m_name(name) { }
    void parse();
    Kind kind() const { return Kind_DotFile; }
    QCString name() const { return m_name; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocDotFile>::accept(this,v); }

  private:
    DocNode  *m_parent;
    QCString  m_name;
};

/*! Node representing a link to some item */
class DocLink : public CompAccept<DocLink>, public DocNode
{
  public:
    DocLink(DocNode *parent,const QCString &target);
    QCString parse(bool);
    Kind kind() const { return Kind_Link; }
    QCString file() const { return m_file; }
    QCString ref() const { return m_ref; }
    QCString anchor() const { return m_anchor; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocLink>::accept(this,v); }

  private:
    DocNode  *m_parent;
    QCString  m_file;
    QCString  m_ref;
    QCString  m_anchor;
};

/*! Node representing a reference to some item */
class DocRef : public CompAccept<DocRef>, public DocNode
{
  public:
    DocRef(DocNode *parent,const QCString &target);
    void parse();
    Kind kind() const            { return Kind_Ref; }
    QCString file() const        { return m_file; }
    QCString ref() const         { return m_ref; }
    QCString anchor() const      { return m_anchor; }
    QCString targetTitle() const { return m_text; }
    DocNode *parent() const      { return m_parent; }
    bool hasLinkText() const     { return !m_children.isEmpty(); }
    bool refToAnchor() const     { return m_refToAnchor; }
    bool refToSection() const    { return m_refToSection; }
    void accept(DocVisitor *v)   { CompAccept<DocRef>::accept(this,v); }

  private:
    DocNode *      m_parent;
    bool           m_refToSection;
    bool           m_refToAnchor;
    QCString       m_file;
    QCString       m_ref;
    QCString       m_anchor;
    QCString       m_text;
};

/*! Node representing an internal reference to some item */
class DocInternalRef : public CompAccept<DocInternalRef>, public DocNode
{
  public:
    DocInternalRef(DocNode *parent,const QCString &target);
    void parse();
    Kind kind() const            { return Kind_Ref; }
    QCString file() const        { return m_file; }
    QCString anchor() const      { return m_anchor; }
    DocNode *parent() const      { return m_parent; }
    void accept(DocVisitor *v)   { CompAccept<DocInternalRef>::accept(this,v); }

  private:
    DocNode * m_parent;
    QCString  m_file;
    QCString  m_anchor;
};

/*! Language specific node */
class DocLanguage : public CompAccept<DocLanguage>, public DocNode
{
  public:
    DocLanguage(DocNode *parent,const QCString &id) : 
      m_parent(parent), m_id(id) {}
    QCString id() const { return m_id; }
    int parse();
    Kind kind() const { return Kind_Language; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocLanguage>::accept(this,v); }
    
  private:
    DocNode * m_parent;
    QCString  m_id;
};

/*! Hypertext reference */
class DocHRef : public CompAccept<DocHRef>, public DocNode
{
  public:
    DocHRef(DocNode *parent,const QCString &url) : 
      m_parent(parent), m_url(url) {}
    int parse();
    QCString url() const { return m_url; }
    Kind kind() const { return Kind_HRef; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHRef>::accept(this,v); }

  private:
    DocNode *           m_parent;
    QCString            m_url;
};

/*! Html heading */
class DocHtmlHeader : public CompAccept<DocHtmlHeader>, public DocNode
{
  public:
    DocHtmlHeader(DocNode *parent,int level) : 
       m_parent(parent), m_level(level) {}
    int parse();
    int level() const { return m_level; }
    Kind kind() const { return Kind_HtmlHeader; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlHeader>::accept(this,v); }

  private:
    DocNode *           m_parent;
    int                 m_level;
};

/*! Html description item */
class DocHtmlDescTitle : public CompAccept<DocHtmlDescTitle>, public DocNode
{
  public:
    DocHtmlDescTitle(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlDescTitle; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescTitle>::accept(this,v); }

  private:
    DocNode *           m_parent;
};

/*! Html description list */
class DocHtmlDescList : public CompAccept<DocHtmlDescList>, public DocNode
{
  public:
    DocHtmlDescList(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlDescList; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescList>::accept(this,v); }

  private:
    DocNode *       m_parent;
};

/*! Normal section */
class DocSection : public CompAccept<DocSection>, public DocNode
{
  public:
    DocSection(DocNode *parent,int level,const QCString &id) :
      m_parent(parent), m_level(level), m_id(id) {} 
    int parse();
    Kind kind() const { return Kind_Section; }
    int level() const { return m_level; }
    QCString id() const { return m_id; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSection>::accept(this,v); }

  private:
    DocNode  *m_parent;
    int       m_level;
    QCString  m_id;
};

/*! Reference to a section */
class DocSecRefItem : public CompAccept<DocSecRefItem>, public DocNode
{
  public:
    DocSecRefItem(DocNode *parent,const QCString &target) : 
      m_parent(parent), m_target(target) {}
    void parse();
    Kind kind() const { return Kind_SecRefItem; }
    QCString target() const { return m_target; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSecRefItem>::accept(this,v); }

  private:
    DocNode  *m_parent;
    QCString  m_target;
};

/*! List of section references */
class DocSecRefList : public CompAccept<DocSecRefList>, public DocNode
{
  public:
    DocSecRefList(DocNode *parent) : m_parent(parent) {}
    void parse();
    Kind kind() const { return Kind_SecRefList; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSecRefList>::accept(this,v); }

  private:
    DocNode  *m_parent;
};

/*! Internal section of documentation */
// TODO: decide if it should be visible
// Separate pass to remove internal parts?
class DocInternal : public CompAccept<DocInternal>, public DocNode
{
  public:
    DocInternal(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_Internal; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocInternal>::accept(this,v); }

  private:
    DocNode  *m_parent;
};

/*! Simple list */
class DocSimpleList : public CompAccept<DocSimpleList>, public DocNode
{
  public:
    DocSimpleList(DocNode *parent) : m_parent(parent) {}
    Kind kind() const { return Kind_SimpleList; }
    int parse();
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocSimpleList>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! Html list */
class DocHtmlList : public CompAccept<DocHtmlList>, public DocNode
{
  public:
    enum Type { Unordered, Ordered };
    DocHtmlList(DocNode *parent,Type t) : m_parent(parent), m_type(t) {}
    Kind kind() const { return Kind_HtmlList; }
    Type type() const { return m_type; }
    int parse();
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlList>::accept(this,v); }

  private:
    DocNode *m_parent;
    Type m_type;
};

/*! Simple section */
class DocSimpleSect : public CompAccept<DocSimpleSect>, public DocNode
{
  public:
    enum Type 
    {  
       Unknown, See, Return, Author, Authors, Version, Since, Date,
       Note, Warning, Pre, Post, Invar, Remark, Attention, User
    };
    DocSimpleSect(DocNode *parent,Type t);
    virtual ~DocSimpleSect();
    int parse(bool userTitle);
    Kind kind() const { return Kind_SimpleSect; }
    Type type() const { return m_type; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v);

  private:
    DocNode *       m_parent;
    Type            m_type;
    DocTitle *      m_title;
};

/*! Parameter section */
class DocParamSect : public CompAccept<DocParamSect>, public DocNode
{
  public:
    enum Type 
    {  
       Unknown, Param, RetVal, Exception 
    };
    DocParamSect(DocNode *parent,Type t) : m_parent(parent), m_type(t) {}
    int parse(const QCString &cmdName);
    Kind kind() const { return Kind_ParamSect; }
    Type type() const { return m_type; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocParamSect>::accept(this,v); }

  private:
    DocNode *       m_parent;
    Type            m_type;
};

/*! Paragraph in the documentation tree */
class DocPara : public CompAccept<DocPara>, public DocNode
{
  public:
    DocPara(DocNode *parent) : m_parent(parent), 
             m_isFirst(FALSE), m_isLast(FALSE) {}
    int parse();
    Kind kind() const { return Kind_Para; }
    DocNode *parent() const { return m_parent; }
    bool isEmpty() const { return m_children.isEmpty(); }
    void accept(DocVisitor *v) { CompAccept<DocPara>::accept(this,v); }
    void markFirst(bool v=TRUE) { m_isFirst=v; }
    void markLast(bool v=TRUE)  { m_isLast=v; }
    bool isFirst() const { return m_isFirst; }
    bool isLast() const { return m_isLast; }

    int handleCommand(const QCString &cmdName);
    int handleHtmlStartTag(const QCString &tagName,const QList<Option> &tagOptions);
    int handleHtmlEndTag(const QCString &tagName);
    int handleSimpleSection(DocSimpleSect::Type t);
    int handleXRefItem(DocXRefItem::Type t);
    int handleParamSection(const QCString &cmdName,DocParamSect::Type t);
    void handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t);
    void handleImage(const QCString &cmdName);
    void handleDotFile(const QCString &cmdName);
    void handleInclude(const QCString &cmdName,DocInclude::Type t);
    void handleLink(const QCString &cmdName,bool isJavaLink);
    void handleRef(const QCString &cmdName);
    int handleLanguageSwitch();


  private:
    DocNode *m_parent;
    QCString m_sectionId;
    bool     m_isFirst;
    bool     m_isLast;
};

/*! Node representing a parameter list. */
class DocParamList : public DocNode
{
  public:
    DocParamList(DocNode *parent) : m_parent(parent) 
    { m_paragraph=new DocPara(this); }
    virtual ~DocParamList()
    { delete m_paragraph; }
    int parse(const QCString &cmdName);
    Kind kind() const { return Kind_ParamList; }
    DocNode *parent() const { return m_parent; }
    const QStrList &parameters() { return m_params; }
    void accept(DocVisitor *v)
    { 
      v->visitPre(this); 
      m_paragraph->accept(v);
      v->visitPost(this); 
    }

  private:
    DocNode   *m_parent;
    DocPara   *m_paragraph;
    QStrList   m_params;
};

/*! Node representing an item of a auto list */
class DocAutoListItem : public DocNode
{
  public:
    DocAutoListItem(DocNode *parent) : m_parent(parent) 
    { m_paragraph=new DocPara(this); }
    virtual ~DocAutoListItem() { delete m_paragraph; }
    int parse();
    Kind kind() const { return Kind_AutoListItem; }
    DocNode *parent() const { return m_parent; }
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

/*! Simple list item */
class DocSimpleListItem : public DocNode
{
  public:
    DocSimpleListItem(DocNode *parent) : m_parent(parent) 
    { m_paragraph=new DocPara(this); }
    int parse();
    virtual ~DocSimpleListItem() { delete m_paragraph; }
    Kind kind() const { return Kind_SimpleListItem; }
    DocNode *parent() const { return m_parent; }
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

/*! Html list item */
class DocHtmlListItem : public CompAccept<DocHtmlListItem>, public DocNode
{
  public:
    DocHtmlListItem(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlListItem; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlListItem>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! Html description data */
class DocHtmlDescData : public CompAccept<DocHtmlDescData>, public DocNode
{
  public:
    DocHtmlDescData(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlDescData; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlDescData>::accept(this,v); }

  private:
    DocNode *           m_parent;
};


/*! Html list item */
class DocHtmlPre : public CompAccept<DocHtmlPre>, public DocNode
{
  public:
    DocHtmlPre(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlPre; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlPre>::accept(this,v); }

  private:
    DocNode *m_parent;
};

/*! Html table cell */
class DocHtmlCell : public CompAccept<DocHtmlCell>, public DocNode
{
  public:
    DocHtmlCell(DocNode *parent,bool isHeading) : 
       m_parent(parent), m_isHeading(isHeading) {}
    int parse();
    bool isHeading() const { return m_isHeading; }
    Kind kind() const { return Kind_HtmlCell; }
    DocNode *parent() const { return m_parent; }
    uint numCells() const { return m_children.count(); }
    void accept(DocVisitor *v) { CompAccept<DocHtmlCell>::accept(this,v); }

  private:
    DocNode *      m_parent;
    bool           m_isHeading;
};

/*! Html table caption */
class DocHtmlCaption : public CompAccept<DocHtmlCaption>, public DocNode
{
  public:
    DocHtmlCaption(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlCaption; }
    DocNode *parent() const { return m_parent; }
    void accept(DocVisitor *v) { CompAccept<DocHtmlCaption>::accept(this,v); }

  private:
    DocNode * m_parent;
};

/*! Html table row */
class DocHtmlRow : public CompAccept<DocHtmlRow>, public DocNode
{
  public:
    DocHtmlRow(DocNode *parent) : m_parent(parent) {}
    int parse();
    Kind kind() const { return Kind_HtmlRow; }
    DocNode *parent() const { return m_parent; }
    uint numCells() const { return m_children.count(); }
    void accept(DocVisitor *v) { CompAccept<DocHtmlRow>::accept(this,v); }

  private:
    DocNode * m_parent;
};

/*! Html table */
class DocHtmlTable : public CompAccept<DocHtmlTable>, public DocNode
{
  public:
    DocHtmlTable(DocNode *parent) : m_parent(parent) { m_caption=0; }
    ~DocHtmlTable() { delete m_caption; }
    int parse();
    Kind kind() const { return Kind_HtmlTable; }
    DocNode *parent() const { return m_parent; }
    uint numRows() const { return m_children.count(); }
    uint numCols() const
    {
      uint cols=0;
      QListIterator<DocNode> cli(m_children);
      DocNode *n;
      for (cli.toFirst();(n=cli.current());++cli)
      {
        ASSERT(n->kind()==DocNode::Kind_HtmlRow);
        cols=QMAX(cols,((DocHtmlRow *)n)->numCells());
      }
      return cols;
    }
    void accept(DocVisitor *v) { CompAccept<DocHtmlTable>::accept(this,v); }

  private:
    DocNode *          m_parent;
    DocHtmlCaption    *m_caption;
};


/*! Root node of documentation tree */
class DocRoot : public CompAccept<DocRoot>, public DocNode
{
  public:
    DocRoot() {}
    void parse();
    Kind kind() const { return Kind_Root; }
    DocNode *parent() const { return 0; }
    void accept(DocVisitor *v) { CompAccept<DocRoot>::accept(this,v); }
};


#endif

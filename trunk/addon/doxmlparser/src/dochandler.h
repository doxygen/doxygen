/******************************************************************************
 *
 * $Id:$
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _DOCHANDLER_H
#define _DOCHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include <doxmlintf.h>
#include "stringimpl.h"
#include "basehandler.h"
#include "baseiterator.h"

class ParagraphHandler;
class DocInternalHandler;
class LinkedTextImpl;
class LinkedTextHandler;

//-----------------------------------------------------------------------------

class DocImpl : public IDoc { public: virtual ~DocImpl() {} };

#define DEFINE_CLS_IMPL(cls) \
  class cls##Impl : public I##cls, public DocImpl { public: virtual ~cls##Impl() {} }

DEFINE_CLS_IMPL(DocMarkup);
DEFINE_CLS_IMPL(DocPara);
DEFINE_CLS_IMPL(DocText);
DEFINE_CLS_IMPL(DocMarkupModifier);
DEFINE_CLS_IMPL(DocItemizedList);
DEFINE_CLS_IMPL(DocOrderedList);
DEFINE_CLS_IMPL(DocListItem);
DEFINE_CLS_IMPL(DocParameterList);
DEFINE_CLS_IMPL(DocParameterItem);
DEFINE_CLS_IMPL(DocParameter);
DEFINE_CLS_IMPL(DocTitle);
DEFINE_CLS_IMPL(DocSimpleSect);
DEFINE_CLS_IMPL(DocRef);
DEFINE_CLS_IMPL(DocVariableList);
DEFINE_CLS_IMPL(DocVariableListEntry);
DEFINE_CLS_IMPL(DocHRuler);
DEFINE_CLS_IMPL(DocLineBreak);
DEFINE_CLS_IMPL(DocULink);
DEFINE_CLS_IMPL(DocEMail);
DEFINE_CLS_IMPL(DocLink);
DEFINE_CLS_IMPL(DocProgramListing);
DEFINE_CLS_IMPL(DocCodeLine);
DEFINE_CLS_IMPL(DocHighlight);
DEFINE_CLS_IMPL(DocFormula);
DEFINE_CLS_IMPL(DocImage);
DEFINE_CLS_IMPL(DocDotFile);
DEFINE_CLS_IMPL(DocIndexEntry);
DEFINE_CLS_IMPL(DocTable);
DEFINE_CLS_IMPL(DocRow);
DEFINE_CLS_IMPL(DocEntry);
DEFINE_CLS_IMPL(DocSection);
DEFINE_CLS_IMPL(DocVerbatim);
DEFINE_CLS_IMPL(DocCopy);
DEFINE_CLS_IMPL(DocTocList);
DEFINE_CLS_IMPL(DocTocItem);
DEFINE_CLS_IMPL(DocAnchor);
DEFINE_CLS_IMPL(DocSymbol);
DEFINE_CLS_IMPL(DocInternal);
DEFINE_CLS_IMPL(DocRoot);

//-----------------------------------------------------------------------------


/*! \brief Node representing a piece of text.
 *
 */
class TextNode : public DocTextImpl
{
  public:
    TextNode(const QString &t,int markup,int level) 
      : m_text(t), m_markup(markup), m_headingLevel(level) {}
    virtual ~TextNode() {}

    // IDocText
    virtual Kind kind() const { return DocImpl::Text; }
    virtual const IString *text() const { return &m_text; }
    virtual int markup() const { return m_markup; }
    virtual int headingLevel() const { return m_headingLevel; }
  
  private:  
    StringImpl m_text;
    int m_markup;
    int m_headingLevel;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a change in the markup style.
 *
 */
class MarkupModifierNode : public DocMarkupModifierImpl
{
  public:
    MarkupModifierNode(int markup,bool enabled,int level=0) 
      : m_markup(markup), m_enabled(enabled), m_headingLevel(level) {}
    virtual ~MarkupModifierNode() {}
  
    // IDocMarkupModifier
    virtual Kind kind() const { return DocImpl::MarkupModifier; }
    virtual bool enabled() const { return m_enabled; }
    virtual int markup() const { return m_markup; }
    virtual int headingLevel() const { return m_headingLevel; }

  private:
    int m_markup;
    bool m_enabled;
    int m_headingLevel;
};


//-----------------------------------------------------------------------------

/*! \brief Handles markup commands in the XML input. 
 *
 */
class MarkupHandler : public BaseFallBackHandler<MarkupHandler>
{
  public:
    MarkupHandler(QList<DocImpl> &children,QString &curString);
    virtual ~MarkupHandler();
    int markup() const { return m_curMarkup; }
    int headingLevel() const { return m_headingLevel; }

    virtual void startBold(const QXmlAttributes &attrib);
    virtual void endBold();
    virtual void startEmphasis(const QXmlAttributes &attrib);
    virtual void endEmphasis();
    virtual void startComputerOutput(const QXmlAttributes &attrib);
    virtual void endComputerOutput();
    virtual void startCenter(const QXmlAttributes &attrib);
    virtual void endCenter();
    virtual void startSmallFont(const QXmlAttributes &attrib);
    virtual void endSmallFont();
    virtual void startSubscript(const QXmlAttributes &attrib);
    virtual void endSubscript();
    virtual void startSuperscript(const QXmlAttributes &attrib);
    virtual void endSuperscript();
    virtual void startPreformatted(const QXmlAttributes &attrib);
    virtual void endPreformatted();
    virtual void startHeading1(const QXmlAttributes &attrib);
    virtual void endHeading1();
    virtual void startHeading2(const QXmlAttributes &attrib);
    virtual void endHeading2();
    virtual void startHeading3(const QXmlAttributes &attrib);
    virtual void endHeading3();
    virtual void startHeading4(const QXmlAttributes &attrib);
    virtual void endHeading4();
    virtual void startHeading5(const QXmlAttributes &attrib);
    virtual void endHeading5();
    virtual void startHeading6(const QXmlAttributes &attrib);
    virtual void endHeading6();


  private:
    void addTextNode();

    QList<DocImpl>  &m_children;
    QString         &m_curString;
    int             m_curMarkup;
    int             m_headingLevel;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a paragraph of text and commands.
 *
 */
// children: itemizedlist, orderedlist, parameterlist, simplesect, ref,
//           variablelist, hruler, linebreak, ulink, email, link
//           programlisting, formula, image, dotfile, indexentry,
//           table
//
// children handled by MarkupHandler: 
//           bold, computeroutput, emphasis, center,
//           small, subscript, superscript. 
//
class ParagraphHandler : public DocParaImpl, 
                         public BaseHandler<ParagraphHandler>
{
    friend class ParagraphIterator;

  public:
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void endParagraph();
    virtual void startItemizedList(const QXmlAttributes& attrib);
    virtual void startOrderedList(const QXmlAttributes& attrib);
    virtual void startParameterList(const QXmlAttributes& attrib);
    virtual void startSimpleSect(const QXmlAttributes& attrib);
    virtual void startRef(const QXmlAttributes& attrib);
    virtual void startVariableList(const QXmlAttributes& attrib);
    virtual void startHRuler(const QXmlAttributes& attrib);
    virtual void startLineBreak(const QXmlAttributes& attrib);
    virtual void startULink(const QXmlAttributes& attrib);
    virtual void startEMail(const QXmlAttributes& attrib);
    virtual void startLink(const QXmlAttributes& attrib);
    virtual void startProgramListing(const QXmlAttributes& attrib);
    virtual void startFormula(const QXmlAttributes& attrib);
    virtual void startImage(const QXmlAttributes& attrib);
    virtual void startDotFile(const QXmlAttributes& attrib);
    virtual void startIndexEntry(const QXmlAttributes& attrib);
    virtual void startTable(const QXmlAttributes& attrib);
    virtual void startVerbatim(const QXmlAttributes& attrib);
    virtual void startHtmlOnly(const QXmlAttributes& attrib);
    virtual void startLatexOnly(const QXmlAttributes& attrib);
    virtual void startUmlaut(const QXmlAttributes& attrib);
    virtual void startAcute(const QXmlAttributes& attrib);
    virtual void startGrave(const QXmlAttributes& attrib);
    virtual void startCirc(const QXmlAttributes& attrib);
    virtual void startTilde(const QXmlAttributes& attrib);
    virtual void startSzlig(const QXmlAttributes& attrib);
    virtual void startCedil(const QXmlAttributes& attrib);
    virtual void startRing(const QXmlAttributes& attrib);
    virtual void startNbsp(const QXmlAttributes& attrib);
    virtual void startCopy(const QXmlAttributes& attrib);
    virtual void startAnchor(const QXmlAttributes& attrib);
    virtual void startCopyDoc(const QXmlAttributes& attrib);
    virtual void startTocList(const QXmlAttributes& attrib);

    ParagraphHandler(IBaseHandler *parent);
    virtual ~ParagraphHandler();

    // IDocPara
    virtual Kind kind() const { return DocImpl::Para; }
    virtual IDocIterator *contents() const;

  private:
    void addTextNode();
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
    MarkupHandler  *m_markupHandler;
};

class ParagraphIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    ParagraphIterator(const ParagraphHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a list item.
 *
 */
class ListItemHandler : public DocListItemImpl, public BaseHandler<ListItemHandler>
{
    friend class ListItemIterator;
  public:
    ListItemHandler(IBaseHandler *parent);
    virtual ~ListItemHandler();
    virtual void startListItem(const QXmlAttributes& attrib);
    virtual void endListItem();
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocItem
    virtual Kind kind() const { return DocImpl::ListItem; }
    virtual IDocIterator *contents() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class ListItemIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    ListItemIterator(const ListItemHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};


//-----------------------------------------------------------------------------

/*! \brief Node representing list of items.
 *
 */
class OrderedListHandler : public DocOrderedListImpl, public BaseHandler<OrderedListHandler>
{
    friend class OrderedListIterator;
  public:
    OrderedListHandler(IBaseHandler *parent);
    virtual ~OrderedListHandler();
    virtual void startOrderedList(const QXmlAttributes& attrib);
    virtual void endOrderedList();
    virtual void startOrderedListItem(const QXmlAttributes& attrib);

    // IDocOrderedList
    virtual Kind kind() const { return DocImpl::OrderedList; }
    virtual IDocIterator *elements() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class OrderedListIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    OrderedListIterator(const OrderedListHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing table of contents list.
 *
 */
class TocListHandler : public DocTocListImpl, public BaseHandler<TocListHandler>
{
    friend class TocListIterator;
  public:
    TocListHandler(IBaseHandler *parent);
    virtual ~TocListHandler();
    virtual void startTocList(const QXmlAttributes& attrib);
    virtual void endTocList();
    virtual void startTocItem(const QXmlAttributes& attrib);

    // IDocTocList
    virtual Kind kind() const { return DocImpl::TocList; }
    virtual IDocIterator *elements() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class TocListIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    TocListIterator(const TocListHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a table of contents item.
 *
 */
class TocItemHandler : public DocTocItemImpl, public BaseHandler<TocItemHandler>
{
    friend class TocItemIterator;
  public:
    TocItemHandler(IBaseHandler *parent);
    virtual ~TocItemHandler();
    virtual void startTocItem(const QXmlAttributes& attrib);
    virtual void endTocItem();

    // IDocItem
    virtual Kind kind() const { return DocImpl::TocItem; }
    virtual const IString *id() const { return &m_id; }
    virtual const IString *title() const { return &m_title; }

  private:
    IBaseHandler   *m_parent;
    StringImpl      m_id;
    StringImpl      m_title;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing list of items.
 *
 */
class ItemizedListHandler : public DocItemizedListImpl, public BaseHandler<ItemizedListHandler>
{
    friend class ItemizedListIterator;
  public:
    ItemizedListHandler(IBaseHandler *parent);
    virtual ~ItemizedListHandler();
    virtual void startItemizedList(const QXmlAttributes& attrib);
    virtual void endItemizedList();
    virtual void startItemizedListItem(const QXmlAttributes& attrib);

    // IDocItemizedList
    virtual Kind kind() const { return DocImpl::ItemizedList; }
    virtual IDocIterator *elements() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class ItemizedListIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    ItemizedListIterator(const ItemizedListHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};


//-----------------------------------------------------------------------------
/*! \brief Node representing a parameter.
 *
 */
class ParameterHandler : public DocParameterImpl, 
                         public BaseHandler<ParameterHandler>
{
  public:
    ParameterHandler(IBaseHandler *parent);
    virtual ~ParameterHandler();
    virtual void startParameterName(const QXmlAttributes& attrib);
    virtual void endParameterName();

    // IDocParameter
    virtual Kind kind() const { return DocImpl::Parameter; }
    virtual const IString *name() const { return &m_name; }

  private:
    IBaseHandler     *m_parent;
    StringImpl        m_name;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a list of param names with a single description.
 *
 */
class ParameterItemHandler : public DocParameterItemImpl, 
                             public BaseHandler<ParameterItemHandler>
{
    friend class ParameterItemIterator;
  public:
    ParameterItemHandler(IBaseHandler *parent);
    virtual ~ParameterItemHandler();
    virtual void startParameterItem(const QXmlAttributes& attrib);
    virtual void endParameterItem();
    virtual void startParameterName(const QXmlAttributes& attrib);
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocParameterItem
    virtual Kind kind() const { return DocImpl::ParameterItem; }
    virtual IDocIterator *paramNames() const;
    virtual IDocPara *description() const { return m_description; }

  private:
    IBaseHandler            *m_parent;
    QList<ParameterHandler>  m_parameters;
    ParagraphHandler *m_description;
};

class ParameterItemIterator : public BaseIteratorVia<IDocIterator,IDoc,ParameterHandler,DocImpl>
{
  public:
    ParameterItemIterator(const ParameterItemHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,ParameterHandler,DocImpl>(handler.m_parameters) {}
};

//-----------------------------------------------------------------------------

/* \brief Node representing a parameter section.
 *
 */
class ParameterListHandler : public DocParameterListImpl, 
                             public BaseHandler<ParameterListHandler>
{
    friend class ParameterListIterator;
  public:
    ParameterListHandler(IBaseHandler *parent);
    virtual ~ParameterListHandler();
    virtual void startParameterList(const QXmlAttributes& attrib);
    virtual void endParameterList();
    virtual void startParameterItem(const QXmlAttributes& attrib);

    // IDocParameterList
    virtual Kind kind() const { return DocImpl::ParameterList; }
    virtual Types sectType() const { return m_type; }
    virtual IDocIterator *params() const;

  private:
    IBaseHandler            *m_parent;
    QList<ParameterItemHandler>  m_paramItems;
    Types                    m_type;
};

class ParameterListIterator : public BaseIteratorVia<IDocIterator,IDoc,ParameterItemHandler,DocImpl>
{
  public:
    ParameterListIterator(const ParameterListHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,ParameterItemHandler,DocImpl>(handler.m_paramItems) {}
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class LineBreakHandler : public DocLineBreakImpl, public BaseHandler<LineBreakHandler>
{
  public:
    LineBreakHandler(IBaseHandler *parent);
    virtual ~LineBreakHandler();

    void startLineBreak(const QXmlAttributes& attrib);
    void endLineBreak();

    // IDocLineBreak
    virtual Kind kind() const { return DocImpl::LineBreak; }

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a link to section
 *
 */
class LinkHandler : public DocLinkImpl, public BaseHandler<LinkHandler>
{
  public:
    LinkHandler(IBaseHandler *parent);
    virtual ~LinkHandler();

    void startLink(const QXmlAttributes& attrib);
    void endLink();

    // IDocLink
    virtual Kind kind() const { return DocImpl::Link; }
    virtual const IString *refId() const { return &m_ref; }
    virtual const IString *text() const { return &m_text; }

  private:
    IBaseHandler   *m_parent;
    StringImpl      m_ref;
    StringImpl      m_text;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an email address
 *
 */
class EMailHandler : public DocEMailImpl, public BaseHandler<EMailHandler>
{
  public:
    EMailHandler(IBaseHandler *parent);
    virtual ~EMailHandler();

    void startEMail(const QXmlAttributes& attrib);
    void endEMail();

    // IDocEMail
    virtual Kind kind() const { return DocImpl::EMail; }
    virtual const IString *address() const { return &m_address; }

  private:
    IBaseHandler   *m_parent;
    StringImpl      m_address;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an URL
 *
 */
class ULinkHandler : public DocULinkImpl, public BaseHandler<ULinkHandler>
{
  public:
    ULinkHandler(IBaseHandler *parent);
    virtual ~ULinkHandler();

    void startULink(const QXmlAttributes& attrib);
    void endULink();

    // IDocULink
    virtual Kind kind() const { return DocImpl::ULink; }
    virtual const IString * url() const { return &m_url; }
    virtual const IString * text() const { return &m_text; }

  private:
    IBaseHandler   *m_parent;
    StringImpl      m_url;
    StringImpl      m_text;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class HRulerHandler : public DocHRulerImpl, public BaseHandler<HRulerHandler>
{
  public:
    HRulerHandler(IBaseHandler *parent);
    virtual ~HRulerHandler();

    void startHRuler(const QXmlAttributes& attrib);
    void endHRuler();

    // IDocHRuler
    virtual Kind kind() const { return DocImpl::HRuler; }

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a reference to another item
 *
 */
class RefHandler : public DocRefImpl, public BaseHandler<RefHandler>
{
  public:
    RefHandler(IBaseHandler *parent);
    virtual ~RefHandler();
    void startRef(const QXmlAttributes& attrib);
    void endRef();

    // IDocRef
    virtual Kind kind() const { return DocImpl::Ref; }
    virtual const IString *refId() const { return &m_refId; }
    virtual TargetKind targetKind() const { return m_targetKind; }
    virtual const IString *external() const { return &m_extId; }
    virtual const IString *text() const { return &m_linkText; }

  private:
    IBaseHandler   *m_parent;
    StringImpl      m_refId;
    StringImpl      m_extId;
    StringImpl      m_linkText;
    TargetKind      m_targetKind;
};

//-----------------------------------------------------------------------------

/* \brief Node representing the title of a section
 *
 */
// children: text, ref
// children handled by MarkupHandler: 
//           bold, computeroutput, emphasis, center,
//           small, subscript, superscript. 
class TitleHandler : public DocTitleImpl, public BaseHandler<TitleHandler>
{
    friend class TitleIterator;
  public:
    TitleHandler(IBaseHandler *parent);
    virtual ~TitleHandler();
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void endTitle();
    virtual void startRef(const QXmlAttributes& attrib);
    void addTextNode();

    // IDocTitle
    virtual Kind kind() const { return DocImpl::Title; }
    virtual IDocIterator *title() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
    MarkupHandler  *m_markupHandler;
};

class TitleIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    TitleIterator(const TitleHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/* \brief Node representing a simple section with an unnumbered header.
 *
 */
// children: title, para
class SimpleSectHandler : public DocSimpleSectImpl, 
                          public BaseHandler<SimpleSectHandler>
{
  public:
    SimpleSectHandler(IBaseHandler *parent);
    virtual ~SimpleSectHandler();
    virtual void startSimpleSect(const QXmlAttributes& attrib);
    virtual void endSimpleSect();
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocSimpleSect
    virtual Kind kind() const { return DocImpl::SimpleSect; }
    virtual Types type() const { return m_type; }
    virtual const IString *typeString() const { return &m_typeString; }
    virtual IDocTitle *title() const { return m_title; }
    virtual IDocPara *description() const { return m_paragraph; }

  private:
    IBaseHandler            *m_parent;
    ParagraphHandler        *m_paragraph;
    Types                    m_type;
    StringImpl               m_typeString;
    TitleHandler            *m_title;
};

//-----------------------------------------------------------------------------

/* \brief Node representing an named item of a VariableList.
 *
 */
class VariableListEntryHandler : public DocVariableListEntryImpl, 
                                 public BaseHandler<VariableListEntryHandler>
{
  public:
    virtual void startVarListEntry(const QXmlAttributes& attrib);
    virtual void endVarListEntry();
    virtual void startListItem(const QXmlAttributes& attrib);
    virtual void endListItem();
    virtual void startTerm(const QXmlAttributes& attrib);
    virtual void endTerm();
    virtual void startParagraph(const QXmlAttributes& attrib);

    VariableListEntryHandler(IBaseHandler *parent);
    virtual ~VariableListEntryHandler();

    // IDocVariableListEntry
    virtual Kind kind() const { return DocImpl::VariableListEntry; }
    virtual ILinkedTextIterator *term() const;
    virtual IDocPara *description() const { return m_description; }

  private:
    IBaseHandler*         m_parent;
    QList<LinkedTextImpl> m_term;
    ParagraphHandler*     m_description;
    LinkedTextHandler*    m_linkedTextHandler;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a list of named items.
 *
 */
// children: varlistentry, listitem
class VariableListHandler : public DocVariableListImpl, 
                            public BaseHandler<VariableListHandler>
{
    friend class VariableListIterator;
    
  public:
    virtual void startVariableList(const QXmlAttributes& attrib);
    virtual void endVariableList();
    virtual void startVarListEntry(const QXmlAttributes& attrib);
    virtual void startListItem(const QXmlAttributes& attrib);
    
    VariableListHandler(IBaseHandler *parent);
    virtual ~VariableListHandler();

    // IDocVariableList
    virtual Kind kind() const { return DocImpl::VariableList; }
    virtual IDocIterator *entries() const;

  private:
    IBaseHandler   *m_parent;
    QList<VariableListEntryHandler> m_entries;
    VariableListEntryHandler *m_curEntry;
};

class VariableListIterator : public BaseIteratorVia<IDocIterator,IDoc,VariableListEntryHandler,DocImpl>
{
  public:
    VariableListIterator(const VariableListHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,VariableListEntryHandler,DocImpl>(handler.m_entries) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a highlighted text fragment.
 *
 */
// children: ref
class HighlightHandler : public DocHighlightImpl, public BaseHandler<HighlightHandler>
{
    friend class HighlightIterator;
  public:
    HighlightHandler(IBaseHandler *parent);
    virtual ~HighlightHandler();
    void startHighlight(const QXmlAttributes& attrib);
    void endHighlight();
    virtual void startRef(const QXmlAttributes&);
    virtual void startSpace(const QXmlAttributes&);

    // IDocHighlight
    virtual Kind kind() const { return DocImpl::Highlight; }
    virtual HighlightKind highlightKind() const { return m_hl; }
    virtual IDocIterator *codeElements() const;

  private:
    void addTextNode();
    
    IBaseHandler   *m_parent;
    HighlightKind  m_hl;
    QString        m_hlString;
    QList<DocImpl> m_children;
};

class HighlightIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    HighlightIterator(const HighlightHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a line of code.
 *
 */
// children: linenumber, highlight, anchor, ref
class CodeLineHandler : public DocCodeLineImpl, public BaseHandler<CodeLineHandler>
{
    friend class CodeLineIterator;
  public:

    virtual void startCodeLine(const QXmlAttributes&);
    virtual void endCodeLine();
    virtual void startLineNumber(const QXmlAttributes&);
    virtual void endLineNumber();
    virtual void startHighlight(const QXmlAttributes&);
    virtual void startRef(const QXmlAttributes&);
    
    CodeLineHandler(IBaseHandler *parent);
    virtual ~CodeLineHandler();

    // IDocCodeLine
    virtual Kind kind() const { return DocImpl::CodeLine; }
    virtual int lineNumber() const { return m_lineNumber; }
    virtual const IString *refId() const { return &m_refId; }
    virtual IDocIterator *codeElements() const;

  private:
    void addTextNode();

    IBaseHandler   *m_parent;
    int            m_lineNumber;
    StringImpl     m_refId;
    QList<DocImpl> m_children;
};

class CodeLineIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    CodeLineIterator(const CodeLineHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a program listing
 *
 */
// children: codeline, linenumber
class ProgramListingHandler : public DocProgramListingImpl, public BaseHandler<ProgramListingHandler>
{
    friend class ProgramListingIterator;
  public:
    virtual void startProgramListing(const QXmlAttributes& attrib);
    virtual void endProgramListing();
    virtual void startCodeLine(const QXmlAttributes&);
    virtual void startLineNumber(const QXmlAttributes&);

    ProgramListingHandler(IBaseHandler *parent);
    virtual ~ProgramListingHandler();

    // IDocProgramListing
    virtual Kind kind() const { return DocImpl::ProgramListing; }
    virtual IDocIterator *codeLines() const;

  private:
    IBaseHandler           *m_parent;
    QList<CodeLineHandler>  m_children;
    bool m_hasLineNumber;
};

//-----------------------------------------------------------------------------

class ProgramListingIterator : public BaseIteratorVia<IDocIterator,IDoc,CodeLineHandler,DocImpl>
{
  public:
    ProgramListingIterator(const ProgramListingHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,CodeLineHandler,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a formula.
 *
 */
// children: -
class FormulaHandler : public DocFormulaImpl, public BaseHandler<FormulaHandler>
{
  public:
    FormulaHandler(IBaseHandler *parent);
    virtual ~FormulaHandler();
    void startFormula(const QXmlAttributes& attrib);
    void endFormula();

    // IDocFormula
    virtual Kind kind() const { return DocImpl::Formula; }
    virtual const IString *id() const { return &m_id; }
    virtual const IString *text() const { return &m_text; }

  private:
    IBaseHandler  *m_parent;
    StringImpl     m_id;
    StringImpl     m_text;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an image.
 *
 */
// children: -
class ImageHandler : public DocImageImpl, public BaseHandler<ImageHandler>
{
  public:
    ImageHandler(IBaseHandler *parent);
    virtual ~ImageHandler();
    void startImage(const QXmlAttributes& attrib);
    void endImage();

    // IDocImage
    virtual Kind kind() const { return DocImpl::Image; }
    virtual const IString *name() const { return &m_name; }
    virtual const IString *caption() const { return &m_caption; }

  private:
    IBaseHandler  *m_parent;
    StringImpl     m_name;
    StringImpl     m_caption;
};


//-----------------------------------------------------------------------------
/*! \brief Node representing an anchor.
 *
 */
// children: -
class AnchorHandler : public DocAnchorImpl, public BaseHandler<AnchorHandler>
{
  public:
    AnchorHandler(IBaseHandler *parent);
    virtual ~AnchorHandler();
    void startAnchor(const QXmlAttributes& attrib);
    void endAnchor();

    // IDocAnchor
    virtual Kind kind() const { return DocImpl::Anchor; }
    virtual const IString *id() const { return &m_id; }

  private:
    IBaseHandler  *m_parent;
    StringImpl     m_id;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a dot file.
 *
 */
// children: -
class DotFileHandler : public DocDotFileImpl, public BaseHandler<DotFileHandler>
{
  public:
    DotFileHandler(IBaseHandler *parent);
    virtual ~DotFileHandler();
    void startDotFile(const QXmlAttributes& attrib);
    void endDotFile();

    // IDocDotFile
    virtual Kind kind() const { return DocImpl::DotFile; }
    virtual const IString *name() const { return &m_name; }
    virtual const IString *caption() const { return &m_caption; }

  private:
    IBaseHandler  *m_parent;
    StringImpl     m_name;
    StringImpl     m_caption;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the index.
 *
 */
// children: -
class IndexEntryHandler : public DocIndexEntryImpl, public BaseHandler<IndexEntryHandler>
{
  public:
    IndexEntryHandler(IBaseHandler *parent);
    virtual ~IndexEntryHandler();
    void startIndexEntry(const QXmlAttributes& attrib);
    void endIndexEntry();
    void startPrimaryIE(const QXmlAttributes& attrib);
    void endPrimaryIE();
    void startSecondaryIE(const QXmlAttributes& attrib);
    void endSecondaryIE();

    // IDocIndexEntry
    virtual Kind kind() const { return DocImpl::IndexEntry; }
    virtual const IString *primary() const { return &m_primary; }
    virtual const IString *secondary() const { return &m_secondary; }

  private:
    IBaseHandler  *m_parent;
    StringImpl     m_primary;
    StringImpl     m_secondary;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table entry.
 *
 */
// children: para
class EntryHandler : public DocEntryImpl, public BaseHandler<EntryHandler>
{
    friend class EntryIterator;
  public:
    EntryHandler(IBaseHandler *parent);
    virtual ~EntryHandler();
    void startEntry(const QXmlAttributes& attrib);
    void endEntry();
    void startParagraph(const QXmlAttributes& attrib);

    // IDocEntry
    virtual Kind kind() const { return DocImpl::Entry; }
    virtual IDocIterator *contents() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class EntryIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    EntryIterator(const EntryHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table row.
 *
 */
// children: entry
class RowHandler : public DocRowImpl, public BaseHandler<RowHandler>
{
    friend class RowIterator;
  public:
    RowHandler(IBaseHandler *parent);
    virtual ~RowHandler();
    void startRow(const QXmlAttributes& attrib);
    void endRow();
    void startEntry(const QXmlAttributes& attrib);

    // IDocRow
    virtual Kind kind() const { return DocImpl::Row; }
    virtual IDocIterator *entries() const;

  private:
    IBaseHandler        *m_parent;
    QList<EntryHandler>  m_children;
};

class RowIterator : public BaseIteratorVia<IDocIterator,IDoc,EntryHandler,DocImpl>
{
  public:
    RowIterator(const RowHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,EntryHandler,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table.
 *
 */
// children: row, caption
class TableHandler : public DocTableImpl, public BaseHandler<TableHandler>
{
    friend class TableIterator;
  public:
    TableHandler(IBaseHandler *parent);
    virtual ~TableHandler();
    void startTable(const QXmlAttributes& attrib);
    void endTable();
    void startRow(const QXmlAttributes& attrib);
    void startCaption(const QXmlAttributes& attrib);
    void endCaption();

    // IDocTable
    virtual Kind kind() const { return DocImpl::Table; }
    virtual IDocIterator *rows() const;
    virtual int numColumns() const { return m_numColumns; }
    virtual const IString *caption() const { return &m_caption; }

  private:
    IBaseHandler      *m_parent;
    QList<RowHandler>  m_children;
    int                m_numColumns;
    StringImpl         m_caption;
};

class TableIterator : public BaseIteratorVia<IDocIterator,IDoc,RowHandler,DocImpl>
{
  public:
    TableIterator(const TableHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,RowHandler,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a copied piece of documentation.
 *
 */
class CopyHandler : public DocCopyImpl, public BaseHandler<CopyHandler>
{
    friend class CopyIterator;
  public:
    CopyHandler(IBaseHandler *parent);
    virtual ~CopyHandler();
    virtual void startCopy(const QXmlAttributes& attrib);
    virtual void endCopy();
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocCopy
    virtual Kind kind() const { return DocImpl::Copy; }
    virtual IDocIterator *contents() const;

  private:
    IBaseHandler   *m_parent;
    QList<DocImpl>  m_children;
};

class CopyIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    CopyIterator(const CopyHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an preformatted section
 */
class VerbatimHandler : public DocVerbatimImpl, 
                        public BaseHandler<VerbatimHandler>
{
  public:
    VerbatimHandler(IBaseHandler *parent);
    virtual ~VerbatimHandler();
    void startVerbatim(const QXmlAttributes& attrib,Types type);
    void endVerbatim();

    // IDocVerbatim
    virtual Kind kind() const { return DocImpl::Verbatim; }
    virtual const IString *text() const { return &m_text; }
    virtual Types type() const { return m_type; }

  private:
    IBaseHandler *m_parent;
    StringImpl    m_text;
    Types         m_type;
};


//-----------------------------------------------------------------------------

/*! \brief Node representing an special symbol.
 *
 */
// children: -
class SymbolHandler : public DocSymbolImpl, public BaseHandler<SymbolHandler>
{
  public:
    SymbolHandler(IBaseHandler *parent,Types type);
    virtual ~SymbolHandler();
    void startSymbol(const QXmlAttributes& attrib);
    void endSymbol();

    // IDocSymbol
    virtual Kind kind() const   { return DocImpl::Symbol; }
    virtual Types type() const  { return m_type; }
    virtual const IString *typeString() const { return &m_typeString; }
    virtual char letter() const { return m_letter; }

  private:
    IBaseHandler  *m_parent;
    char           m_letter;
    Types          m_type;
    StringImpl     m_typeString;
};


//-----------------------------------------------------------------------------

/*! \brief Node representing a section.
 *
 */
// children: title, para, sect(n+1)
class DocSectionHandler : public DocSectionImpl, public BaseHandler<DocSectionHandler>
{
    friend class DocSectionParaIterator;
    friend class DocSectionSubIterator;
  public:
    DocSectionHandler(IBaseHandler *parent,int level);
    virtual ~DocSectionHandler();
    virtual void startDocSection(const QXmlAttributes& attrib);
    virtual void endDocSection();
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void startSubSection(const QXmlAttributes& attrib);
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void startInternal(const QXmlAttributes& attrib);

    // IDocSection
    virtual Kind kind() const { return DocImpl::Section; }
    virtual const IString *id() const { return &m_id; }
    virtual int level() const { return m_level; }
    virtual IDocTitle *title() const { return m_title; }
    virtual IDocIterator *paragraphs() const;
    virtual IDocIterator *subSections() const;
    virtual IDocInternal *internal() const;

  private:
    IBaseHandler    *m_parent;
    QList<DocImpl>   m_paragraphs;
    QList<DocImpl>   m_subsections;
    DocInternalHandler *m_internal;
    StringImpl       m_id;
    int              m_level;
    TitleHandler    *m_title;
};

class DocSectionParaIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    DocSectionParaIterator(const DocSectionHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_paragraphs) {}
};

class DocSectionSubIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    DocSectionSubIterator(const DocSectionHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_subsections) {}
};

//-----------------------------------------------------------------------------

class DocInternalHandler : public DocInternalImpl, public BaseHandler<DocInternalHandler>
{
  public:
    friend class DocInternalParaIterator;
    friend class DocInternalSubIterator;
    DocInternalHandler(IBaseHandler *parent,int level);
    virtual ~DocInternalHandler();
    virtual void startInternal(const QXmlAttributes& attrib);
    virtual void endInternal();
    virtual void startSubSection(const QXmlAttributes& attrib);
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocInternal
    virtual Kind kind() const { return DocImpl::Internal; }
    virtual IDocIterator *paragraphs() const;
    virtual IDocIterator *subSections() const;

  private:
    IBaseHandler    *m_parent;
    QList<DocImpl>   m_paragraphs;
    QList<DocImpl>   m_subsections;
    int              m_level;
};

class DocInternalParaIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    DocInternalParaIterator(const DocInternalHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_paragraphs) {}
};

class DocInternalSubIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    DocInternalSubIterator(const DocInternalHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_subsections) {}
};


//-----------------------------------------------------------------------------

/*! \brief Node representing a documentation block.
 *
 */
// children: para, title, sect1, internal
class DocHandler : public DocRootImpl, public BaseHandler<DocHandler>
{
    friend class DocIterator;
  public:
    virtual void startDoc(const QXmlAttributes& attrib);
    virtual void endDoc();
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void startSect1(const QXmlAttributes& attrib);
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void startInternal(const QXmlAttributes& attrib);

    DocHandler(IBaseHandler *parent);
    virtual ~DocHandler();
    
    // IDocRoot
    virtual Kind kind() const { return DocImpl::Root; }
    virtual IDocIterator *contents() const;
    virtual IDocInternal *internal() const;

  private:
    IBaseHandler *m_parent;
    QList<DocImpl> m_children;
    DocInternalHandler *m_internal;
};

class DocIterator : public BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>
{
  public:
    DocIterator(const DocHandler &handler) : 
      BaseIteratorVia<IDocIterator,IDoc,DocImpl,DocImpl>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

void dochandler_init();
void dochandler_exit();

#endif

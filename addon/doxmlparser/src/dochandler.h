/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
#include "basehandler.h"
#include "baseiterator.h"

class ParagraphHandler;

//-----------------------------------------------------------------------------


/*! \brief Node representing a piece of text.
 *
 */
class TextNode : public IDocText
{
  public:
    TextNode(const QString &t,int markup) 
      : m_text(t), m_markup(markup) {}
    virtual ~TextNode() {}

    // IDocText
    virtual Kind kind() const { return Text; }
    virtual QString text() const { return m_text; }
    virtual int markup() const { return m_markup; }
  
  private:  
    QString m_text;
    int m_markup;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a change in the markup style.
 *
 */
class MarkupModifierNode : public IDocMarkupModifier
{
  public:
    MarkupModifierNode(int markup,bool enabled) 
      : m_markup(markup), m_enabled(enabled) {}
    virtual ~MarkupModifierNode() {}
  
    // IDocMarkupModifier
    virtual Kind kind() const { return MarkupModifier; }
    virtual bool enabled() const { return m_enabled; }
    virtual int markup() const { return m_markup; }

  private:
    int m_markup;
    bool m_enabled;
};


//-----------------------------------------------------------------------------

/*! \brief Handles markup commands in the XML input. 
 *
 */
class MarkupHandler : public BaseFallBackHandler<MarkupHandler>
{
  public:
    MarkupHandler(QList<IDoc> &children,QString &curString);
    virtual ~MarkupHandler();
    int markup() const { return m_curMarkup; }

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


  private:
    void addTextNode();

    QList<IDoc>    &m_children;
    QString        &m_curString;
    int             m_curMarkup;
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
class ParagraphHandler : public IDocPara, 
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

    ParagraphHandler(IBaseHandler *parent);
    virtual ~ParagraphHandler();

    // IDocPara
    virtual Kind kind() const { return Para; }
    virtual IDocIterator *contents() const;

  private:
    void addTextNode();
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
    MarkupHandler  *m_markupHandler;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a list item.
 *
 */
class ListItemHandler : public IDocListItem, public BaseHandler<ListItemHandler>
{
    friend class ListItemIterator;
  public:
    ListItemHandler(IBaseHandler *parent);
    virtual ~ListItemHandler();
    virtual void startListItem(const QXmlAttributes& attrib);
    virtual void endListItem();
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocItem
    virtual Kind kind() const { return ListItem; }
    virtual IDocIterator *contents() const;

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
};

class ListItemIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    ListItemIterator(const ListItemHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};


//-----------------------------------------------------------------------------

/*! \brief Node representing list of items.
 *
 */
class OrderedListHandler : public IDocOrderedList, public BaseHandler<OrderedListHandler>
{
    friend class OrderedListIterator;
  public:
    OrderedListHandler(IBaseHandler *parent);
    virtual ~OrderedListHandler();
    virtual void startOrderedList(const QXmlAttributes& attrib);
    virtual void endOrderedList();
    virtual void startOrderedListItem(const QXmlAttributes& attrib);

    // IDocOrderedList
    virtual Kind kind() const { return OrderedList; }
    virtual IDocIterator *elements() const;

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
};

class OrderedListIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    OrderedListIterator(const OrderedListHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};


//-----------------------------------------------------------------------------

/*! \brief Node representing list of items.
 *
 */
class ItemizedListHandler : public IDocItemizedList, public BaseHandler<ItemizedListHandler>
{
    friend class ItemizedListIterator;
  public:
    ItemizedListHandler(IBaseHandler *parent);
    virtual ~ItemizedListHandler();
    virtual void startItemizedList(const QXmlAttributes& attrib);
    virtual void endItemizedList();
    virtual void startItemizedListItem(const QXmlAttributes& attrib);

    // IDocItemizedList
    virtual Kind kind() const { return ItemizedList; }
    virtual IDocIterator *elements() const;

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
};

class ItemizedListIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    ItemizedListIterator(const ItemizedListHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};


//-----------------------------------------------------------------------------
/*! \brief Node representing a parameter.
 *
 */
class ParameterHandler : public IDocParameter, 
                         public BaseHandler<ParameterHandler>
{
  public:
    ParameterHandler(IBaseHandler *parent);
    virtual ~ParameterHandler();
    virtual void startParameterName(const QXmlAttributes& attrib);
    virtual void endParameterName();
    virtual void startParameterDescription(const QXmlAttributes& attrib);
    virtual void endParameterDescription();
    virtual void startParagraph(const QXmlAttributes& attrib);

    // IDocParameter
    virtual Kind kind() const { return Parameter; }
    virtual QString name() const { return m_name; }
    virtual IDocPara *description() const { return m_description; }

  private:
    IBaseHandler     *m_parent;
    QString           m_name;
    ParagraphHandler *m_description;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a parameter list.
 *
 */
class ParameterListHandler : public IDocParameterList, 
                             public BaseHandler<ParameterListHandler>
{
    friend class ParameterListIterator;
  public:
    ParameterListHandler(IBaseHandler *parent);
    virtual ~ParameterListHandler();
    virtual void startParameterList(const QXmlAttributes& attrib);
    virtual void endParameterList();
    virtual void startParameterName(const QXmlAttributes& attrib);
    virtual void startParameterDescription(const QXmlAttributes& attrib);

    // IDocParameterList
    virtual Kind kind() const { return ParameterList; }
    virtual Types listType() const { return m_type; }
    virtual IDocIterator *params() const;

  private:
    IBaseHandler            *m_parent;
    QList<ParameterHandler>  m_parameters;
    ParameterHandler        *m_curParam;
    Types                    m_type;
};

class ParameterListIterator : public BaseIterator<IDocIterator,IDoc,ParameterHandler>
{
  public:
    ParameterListIterator(const ParameterListHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,ParameterHandler>(handler.m_parameters) {}
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class LineBreakHandler : public IDocLineBreak, public BaseHandler<LineBreakHandler>
{
  public:
    LineBreakHandler(IBaseHandler *parent);
    virtual ~LineBreakHandler();

    void startLineBreak(const QXmlAttributes& attrib);
    void endLineBreak();

    // IDocLineBreak
    virtual Kind kind() const { return LineBreak; }

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a link to section
 *
 */
class LinkHandler : public IDocLink, public BaseHandler<LinkHandler>
{
  public:
    LinkHandler(IBaseHandler *parent);
    virtual ~LinkHandler();

    void startLink(const QXmlAttributes& attrib);
    void endLink();

    // IDocLink
    virtual Kind kind() const { return Link; }

  private:
    IBaseHandler   *m_parent;
    QString         m_ref;
    QString         m_text;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an email address
 *
 */
class EMailHandler : public IDocEMail, public BaseHandler<EMailHandler>
{
  public:
    EMailHandler(IBaseHandler *parent);
    virtual ~EMailHandler();

    void startEMail(const QXmlAttributes& attrib);
    void endEMail();

    // IDocEMail
    virtual Kind kind() const { return EMail; }

  private:
    IBaseHandler   *m_parent;
    QString         m_address;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an URL
 *
 */
class ULinkHandler : public IDocULink, public BaseHandler<ULinkHandler>
{
  public:
    ULinkHandler(IBaseHandler *parent);
    virtual ~ULinkHandler();

    void startULink(const QXmlAttributes& attrib);
    void endULink();

    // IDocULink
    virtual Kind kind() const { return ULink; }

  private:
    IBaseHandler   *m_parent;
    QString         m_url;
    QString         m_text;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class HRulerHandler : public IDocHRuler, public BaseHandler<HRulerHandler>
{
  public:
    HRulerHandler(IBaseHandler *parent);
    virtual ~HRulerHandler();

    void startHRuler(const QXmlAttributes& attrib);
    void endHRuler();

    // IDocHRuler
    virtual Kind kind() const { return HRuler; }

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a reference to another item
 *
 */
class RefHandler : public IDocRef, public BaseHandler<RefHandler>
{
  public:
    RefHandler(IBaseHandler *parent);
    virtual ~RefHandler();
    void startRef(const QXmlAttributes& attrib);
    void endRef();

    // IDocRef
    virtual Kind kind() const { return Ref; }
    virtual QString refId() const { return m_refId; }

  private:
    IBaseHandler   *m_parent;
    QString        m_refId;
    QString        m_linkText;
};

//-----------------------------------------------------------------------------

/* \brief Node representing the title of a section
 *
 */
// children: text, ref
// children handled by MarkupHandler: 
//           bold, computeroutput, emphasis, center,
//           small, subscript, superscript. 
class TitleHandler : public IDocTitle, public BaseHandler<TitleHandler>
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
    virtual Kind kind() const { return Title; }
    virtual IDocIterator *title() const;

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
    MarkupHandler  *m_markupHandler;
};

class TitleIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    TitleIterator(const TitleHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/* \brief Node representing a simple section with an unnumbered header.
 *
 */
// children: title, para
class SimpleSectHandler : public IDocSimpleSect, 
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
    virtual Kind kind() const { return SimpleSect; }
    virtual Types sectionType() const { return m_type; }
    virtual IDocTitle *title() const { return m_title; }
    virtual IDocPara *description() const { return m_paragraph; }

  private:
    IBaseHandler            *m_parent;
    ParagraphHandler        *m_paragraph;
    Types                    m_type;
    TitleHandler            *m_title;
};

//-----------------------------------------------------------------------------

/* \brief Node representing an named item of a VariableList.
 *
 */
class VariableListEntryHandler : public IDocVariableListEntry, public BaseHandler<VariableListEntryHandler>
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
    virtual Kind kind() const { return VariableListEntry; }

  private:
    IBaseHandler     *m_parent;
    QString           m_term;
    ParagraphHandler *m_description;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a list of named items.
 *
 */
// children: varlistentry, listitem
class VariableListHandler : public IDocVariableList, public BaseHandler<VariableListHandler>
{
  public:
    virtual void startVariableList(const QXmlAttributes& attrib);
    virtual void endVariableList();
    virtual void startVarListEntry(const QXmlAttributes& attrib);
    virtual void startListItem(const QXmlAttributes& attrib);
    
    VariableListHandler(IBaseHandler *parent);
    virtual ~VariableListHandler();

    // IDocVariableList
    virtual Kind kind() const { return VariableList; }

  private:
    IBaseHandler   *m_parent;
    QList<VariableListEntryHandler> m_entries;
    VariableListEntryHandler *m_curEntry;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a highlighted text fragment.
 *
 */
// children: -
class HighlightHandler : public IDocHighlight, public BaseHandler<HighlightHandler>
{
  public:
    HighlightHandler(IBaseHandler *parent);
    virtual ~HighlightHandler();
    void startHighlight(const QXmlAttributes& attrib);
    void endHighlight();

    // IDocHighlight
    virtual Kind kind() const { return Highlight; }

  private:
    IBaseHandler   *m_parent;
    QString        m_class;
    QString        m_text;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a line of code.
 *
 */
// children: linenumber, highlight, anchor, ref
class CodeLineHandler : public IDocCodeLine, public BaseHandler<CodeLineHandler>
{
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
    virtual Kind kind() const { return CodeLine; }

  private:
    void addTextNode();

    IBaseHandler   *m_parent;
    int            m_lineNumber;
    QString        m_refId;
    QList<IDoc> m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a program listing
 *
 */
// children: codeline, linenumber
class ProgramListingHandler : public IDocProgramListing, public BaseHandler<ProgramListingHandler>
{
  public:
    virtual void startProgramListing(const QXmlAttributes& attrib);
    virtual void endProgramListing();
    virtual void startCodeLine(const QXmlAttributes&);
    virtual void startLineNumber(const QXmlAttributes&);

    ProgramListingHandler(IBaseHandler *parent);
    virtual ~ProgramListingHandler();

    // IDocProgramListing
    virtual Kind kind() const { return ProgramListing; }

  private:
    IBaseHandler           *m_parent;
    QList<CodeLineHandler>  m_children;
    bool m_hasLineNumber;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a formula.
 *
 */
// children: -
class FormulaHandler : public IDocFormula, public BaseHandler<FormulaHandler>
{
  public:
    FormulaHandler(IBaseHandler *parent);
    virtual ~FormulaHandler();
    void startFormula(const QXmlAttributes& attrib);
    void endFormula();

    // IDocFormula
    virtual Kind kind() const { return Formula; }

  private:
    IBaseHandler  *m_parent;
    QString        m_id;
    QString        m_text;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an image.
 *
 */
// children: -
class ImageHandler : public IDocImage, public BaseHandler<ImageHandler>
{
  public:
    ImageHandler(IBaseHandler *parent);
    virtual ~ImageHandler();
    void startImage(const QXmlAttributes& attrib);
    void endImage();

    // IDocImage
    virtual Kind kind() const { return Image; }

  private:
    IBaseHandler  *m_parent;
    QString        m_name;
    QString        m_caption;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a dot file.
 *
 */
// children: -
class DotFileHandler : public IDocDotFile, public BaseHandler<DotFileHandler>
{
  public:
    DotFileHandler(IBaseHandler *parent);
    virtual ~DotFileHandler();
    void startDotFile(const QXmlAttributes& attrib);
    void endDotFile();

    // IDocDotFile
    virtual Kind kind() const { return DotFile; }

  private:
    IBaseHandler  *m_parent;
    QString        m_name;
    QString        m_caption;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the index.
 *
 */
// children: -
class IndexEntryHandler : public IDocIndexEntry, public BaseHandler<IndexEntryHandler>
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
    virtual Kind kind() const { return IndexEntry; }

  private:
    IBaseHandler  *m_parent;
    QString        m_primary;
    QString        m_secondary;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table entry.
 *
 */
// children: para
class EntryHandler : public IDocEntry, public BaseHandler<EntryHandler>
{
  public:
    EntryHandler(IBaseHandler *parent);
    virtual ~EntryHandler();
    void startEntry(const QXmlAttributes& attrib);
    void endEntry();
    void startParagraph(const QXmlAttributes& attrib);

    // IDocEntry
    virtual Kind kind() const { return Entry; }

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table row.
 *
 */
// children: entry
class RowHandler : public IDocRow, public BaseHandler<RowHandler>
{
  public:
    RowHandler(IBaseHandler *parent);
    virtual ~RowHandler();
    void startRow(const QXmlAttributes& attrib);
    void endRow();
    void startEntry(const QXmlAttributes& attrib);

    // IDocRow
    virtual Kind kind() const { return Row; }

  private:
    IBaseHandler        *m_parent;
    QList<EntryHandler>  m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table.
 *
 */
// children: row, caption
class TableHandler : public IDocTable, public BaseHandler<TableHandler>
{
  public:
    TableHandler(IBaseHandler *parent);
    virtual ~TableHandler();
    void startTable(const QXmlAttributes& attrib);
    void endTable();
    void startRow(const QXmlAttributes& attrib);
    void startCaption(const QXmlAttributes& attrib);
    void endCaption();

    // IDocTable
    virtual Kind kind() const { return Table; }

  private:
    IBaseHandler      *m_parent;
    QList<RowHandler>  m_children;
    int                m_numColumns;
    QString            m_caption;
};

//-----------------------------------------------------------------------------

class ParagraphIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    ParagraphIterator(const ParagraphHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};


//-----------------------------------------------------------------------------

/*! \brief Node representing a section.
 *
 */
// children: text, ref
// children handled by MarkupHandler: 
//           bold, computeroutput, emphasis, center,
//           small, subscript, superscript. 
class DocSectionHandler : public IDocSection, public BaseHandler<DocSectionHandler>
{
    friend class DocSectionIterator;
  public:
    DocSectionHandler(IBaseHandler *parent,int level);
    virtual ~DocSectionHandler();
    virtual void startDocSection(const QXmlAttributes& attrib);
    virtual void endDocSection();
    virtual void startRef(const QXmlAttributes& attrib);
    void addTextNode();

    // IDocSection
    virtual Kind kind() const { return Section; }
    virtual QString id() const { return m_id; }
    virtual int level() const { return m_level; }
    virtual IDocIterator *title() const;

  private:
    IBaseHandler   *m_parent;
    QList<IDoc>  m_children;
    MarkupHandler  *m_markupHandler;
    QString         m_id;
    int             m_level;
};

class DocSectionIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    DocSectionIterator(const DocSectionHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a documentation block.
 *
 */
// children: para, title, sect1, sect2, sect3
class DocHandler : public IDocRoot, public BaseHandler<DocHandler>
{
    friend class DocIterator;
  public:
    virtual void startDoc(const QXmlAttributes& attrib);
    virtual void endDoc();
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void startSect1(const QXmlAttributes& attrib);
    virtual void startSect2(const QXmlAttributes& attrib);
    virtual void startSect3(const QXmlAttributes& attrib);
    virtual void startTitle(const QXmlAttributes& attrib);

    DocHandler(IBaseHandler *parent);
    virtual ~DocHandler();
    
    // IDocRoot
    virtual Kind kind() const { return Root; }
    virtual IDocIterator *contents() const;

  private:
    IBaseHandler *m_parent;
    QList<IDoc> m_children;
};

class DocIterator : public BaseIterator<IDocIterator,IDoc,IDoc>
{
  public:
    DocIterator(const DocHandler &handler) : 
      BaseIterator<IDocIterator,IDoc,IDoc>(handler.m_children) {}
};

#endif

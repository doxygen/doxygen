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

#include "basehandler.h"

class ParagraphHandler;

//-----------------------------------------------------------------------------

/*! \brief Node of a structured documentation tree.
 *
 */
class DocNode
{
  public:
    enum Markup 
    { 
      Normal         = 0x00,
      Bold           = 0x01,
      Emphasis       = 0x02,
      ComputerOutput = 0x04,
      Subscript      = 0x08,
      Superscript    = 0x10,
      SmallFont      = 0x20,
      Center         = 0x40
    };
    enum NodeKind 
    { 
      Para,
      Text,
      MarkupModifier,
      ItemizedList,
      OrderedList,
      ListItem,
      ParameterList,
      Parameter,
      SimpleSect,
      Title,
      Ref,
      VariableList,
      VariableListEntry,
      HRuler,
      LineBreak,
      ULink,
      EMail,
      Link,
      ProgramListing,
      CodeLine,
      Highlight,
      Anchor,
      Formula,
      Image,
      DotFile,
      IndexEntry,
      Table,
      Row,
      Entry
    };
    DocNode(NodeKind k) : m_kind(k) {}
    virtual ~DocNode() {}
    
  private:
    NodeKind m_kind;
};

//-----------------------------------------------------------------------------


/*! \brief Node representing a piece of text.
 *
 */
class TextNode : public DocNode
{
  public:
    TextNode(const QString &t,int markup) 
      : DocNode(Text), m_text(t), m_markup(markup) {}
  
  private:  
    QString m_text;
    int m_markup;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a change in the markup style.
 *
 */
class MarkupModifierNode : public DocNode
{
  public:
    MarkupModifierNode(int markup,bool enabled) 
      : DocNode(MarkupModifier), m_markup(markup), m_enabled(enabled) {}
  
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
    MarkupHandler(QList<DocNode> &children,QString &curString);
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

    QList<DocNode> &m_children;
    QString        &m_curString;
    int             m_curMarkup;
};


//-----------------------------------------------------------------------------

/*! \brief Node representing a list item.
 *
 */
class ListItemHandler : public DocNode, public BaseHandler<ListItemHandler>
{
  public:
    ListItemHandler(IBaseHandler *parent);
    virtual ~ListItemHandler();
    virtual void startListItem(const QXmlAttributes& attrib);
    virtual void endListItem();
    virtual void startParagraph(const QXmlAttributes& attrib);

  private:
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
};


//-----------------------------------------------------------------------------

/*! \brief Node representing list of items.
 *
 */
class ListHandler : public DocNode, public BaseHandler<ListHandler>
{
  public:
    ListHandler(NodeKind k,IBaseHandler *parent);
    virtual ~ListHandler();
    virtual void startList(const QXmlAttributes& attrib);
    virtual void endList();
    virtual void startListItem(const QXmlAttributes& attrib);

  private:
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
};


//-----------------------------------------------------------------------------

/*! \brief Node representing a parameter.
 *
 */
class ParameterHandler : public DocNode, 
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

  private:
    IBaseHandler     *m_parent;
    QString           m_name;
    ParagraphHandler *m_description;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a parameter list.
 *
 */
class ParameterListHandler : public DocNode, 
                             public BaseHandler<ParameterListHandler>
{
  public:
    enum Types { Param, RetVal, Exception };
    ParameterListHandler(IBaseHandler *parent);
    virtual ~ParameterListHandler();
    virtual void startParameterList(const QXmlAttributes& attrib);
    virtual void endParameterList();
    virtual void startParameterName(const QXmlAttributes& attrib);
    virtual void startParameterDescription(const QXmlAttributes& attrib);

  private:
    IBaseHandler            *m_parent;
    QList<ParameterHandler>  m_parameters;
    ParameterHandler        *m_curParam;
    Types                    m_type;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class LineBreakHandler : public DocNode, public BaseHandler<LineBreakHandler>
{
  public:
    LineBreakHandler(IBaseHandler *parent);
    virtual ~LineBreakHandler();

    void startLineBreak(const QXmlAttributes& attrib);
    void endLineBreak();

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a link to section
 *
 */
class LinkHandler : public DocNode, public BaseHandler<LinkHandler>
{
  public:
    LinkHandler(IBaseHandler *parent);
    virtual ~LinkHandler();

    void startLink(const QXmlAttributes& attrib);
    void endLink();

  private:
    IBaseHandler   *m_parent;
    QString         m_ref;
    QString         m_text;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an email address
 *
 */
class EMailHandler : public DocNode, public BaseHandler<EMailHandler>
{
  public:
    EMailHandler(IBaseHandler *parent);
    virtual ~EMailHandler();

    void startEMail(const QXmlAttributes& attrib);
    void endEMail();

  private:
    IBaseHandler   *m_parent;
    QString         m_address;
};


//-----------------------------------------------------------------------------

/* \brief Node representing a link to an URL
 *
 */
class ULinkHandler : public DocNode, public BaseHandler<ULinkHandler>
{
  public:
    ULinkHandler(IBaseHandler *parent);
    virtual ~ULinkHandler();

    void startULink(const QXmlAttributes& attrib);
    void endULink();

  private:
    IBaseHandler   *m_parent;
    QString         m_url;
    QString         m_text;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a horizontal ruler
 *
 */
class HRulerHandler : public DocNode, public BaseHandler<HRulerHandler>
{
  public:
    HRulerHandler(IBaseHandler *parent);
    virtual ~HRulerHandler();

    void startHRuler(const QXmlAttributes& attrib);
    void endHRuler();

  private:
    IBaseHandler   *m_parent;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a reference to another item
 *
 */
class RefHandler : public DocNode, public BaseHandler<RefHandler>
{
  public:
    RefHandler(IBaseHandler *parent);
    virtual ~RefHandler();
    void startRef(const QXmlAttributes& attrib);
    void endRef();
  private:
    IBaseHandler   *m_parent;
    QString        m_refId;
    QString        m_anchor;
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
class TitleHandler : public DocNode, public BaseHandler<TitleHandler>
{
  public:
    TitleHandler(IBaseHandler *parent);
    virtual ~TitleHandler();
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void endTitle();
    virtual void startRef(const QXmlAttributes& attrib);
    void addTextNode();
  private:
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
    MarkupHandler  *m_markupHandler;
};

//-----------------------------------------------------------------------------

/* \brief Node representing a simple section with an unnumbered header.
 *
 */
// children: title, para
class SimpleSectHandler : public DocNode, 
                          public BaseHandler<SimpleSectHandler>
{
  public:
    enum Types { Invalid = 0,
                 See, Return, Author, Version, 
                 Since, Date, Bug, Note,
                 Warning, Par, Deprecated, Pre, 
                 Post, Invar, Remark, Attention,
                 Todo, Test, RCS, EnumValues, 
                 Examples
    };
    SimpleSectHandler(IBaseHandler *parent);
    virtual ~SimpleSectHandler();
    virtual void startSimpleSect(const QXmlAttributes& attrib);
    virtual void endSimpleSect();
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void startParagraph(const QXmlAttributes& attrib);

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
class VariableListEntryHandler : public DocNode, public BaseHandler<VariableListEntryHandler>
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
class VariableListHandler : public DocNode, public BaseHandler<VariableListHandler>
{
  public:
    virtual void startVariableList(const QXmlAttributes& attrib);
    virtual void endVariableList();
    virtual void startVarListEntry(const QXmlAttributes& attrib);
    virtual void startListItem(const QXmlAttributes& attrib);
    
    VariableListHandler(IBaseHandler *parent);
    virtual ~VariableListHandler();

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
class HighlightHandler : public DocNode, public BaseHandler<HighlightHandler>
{
  public:
    HighlightHandler(IBaseHandler *parent);
    virtual ~HighlightHandler();
    void startHighlight(const QXmlAttributes& attrib);
    void endHighlight();

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
class CodeLineHandler : public DocNode, public BaseHandler<CodeLineHandler>
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

  private:
    void addTextNode();

    IBaseHandler   *m_parent;
    int            m_lineNumber;
    QString        m_refId;
    QList<DocNode> m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a program listing
 *
 */
// children: codeline, linenumber
class ProgramListingHandler : public DocNode, public BaseHandler<ProgramListingHandler>
{
  public:
    virtual void startProgramListing(const QXmlAttributes& attrib);
    virtual void endProgramListing();
    virtual void startCodeLine(const QXmlAttributes&);
    virtual void startLineNumber(const QXmlAttributes&);

    ProgramListingHandler(IBaseHandler *parent);
    virtual ~ProgramListingHandler();
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
class FormulaHandler : public DocNode, public BaseHandler<FormulaHandler>
{
  public:
    FormulaHandler(IBaseHandler *parent);
    virtual ~FormulaHandler();
    void startFormula(const QXmlAttributes& attrib);
    void endFormula();

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
class ImageHandler : public DocNode, public BaseHandler<ImageHandler>
{
  public:
    ImageHandler(IBaseHandler *parent);
    virtual ~ImageHandler();
    void startImage(const QXmlAttributes& attrib);
    void endImage();

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
class DotFileHandler : public DocNode, public BaseHandler<DotFileHandler>
{
  public:
    DotFileHandler(IBaseHandler *parent);
    virtual ~DotFileHandler();
    void startDotFile(const QXmlAttributes& attrib);
    void endDotFile();

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
class IndexEntryHandler : public DocNode, public BaseHandler<IndexEntryHandler>
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
class EntryHandler : public DocNode, public BaseHandler<EntryHandler>
{
  public:
    EntryHandler(IBaseHandler *parent);
    virtual ~EntryHandler();
    void startEntry(const QXmlAttributes& attrib);
    void endEntry();
    void startParagraph(const QXmlAttributes& attrib);

  private:
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table row.
 *
 */
// children: entry
class RowHandler : public DocNode, public BaseHandler<RowHandler>
{
  public:
    RowHandler(IBaseHandler *parent);
    virtual ~RowHandler();
    void startRow(const QXmlAttributes& attrib);
    void endRow();
    void startEntry(const QXmlAttributes& attrib);

  private:
    IBaseHandler        *m_parent;
    QList<EntryHandler>  m_children;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing an entry in the table.
 *
 */
// children: row
class TableHandler : public DocNode, public BaseHandler<TableHandler>
{
  public:
    TableHandler(IBaseHandler *parent);
    virtual ~TableHandler();
    void startTable(const QXmlAttributes& attrib);
    void endTable();
    void startRow(const QXmlAttributes& attrib);

  private:
    IBaseHandler      *m_parent;
    QList<RowHandler>  m_children;
    int                m_numColumns;
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
class ParagraphHandler : public DocNode, public BaseHandler<ParagraphHandler>
{
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

  private:
    void addTextNode();
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
    MarkupHandler  *m_markupHandler;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a documentation block.
 *
 */
// children: para 
// TODO: title, sect1, sect2, sect3
class DocHandler : public BaseHandler<DocHandler>
{
  public:
    virtual void startDoc(const QXmlAttributes& attrib);
    virtual void endDoc();
    virtual void startParagraph(const QXmlAttributes& attrib);

    DocHandler(IBaseHandler *parent);
    virtual ~DocHandler();
    
  private:
    IBaseHandler *m_parent;
    QList<DocNode> m_children;
};

#endif

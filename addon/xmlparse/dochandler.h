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
      Parameter
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

/* \brief Node representing a simple section with an unnumbered header.
 *
 */
class SimpleSectHandler : public DocNode, 
                          public BaseHandler<SimpleSectHandler>
{
  public:
    enum Types { See, Return, Author, Version, 
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

  private:
    IBaseHandler            *m_parent;
    ParameterHandler        *m_curParam;
    Types                    m_type;
    QString                  m_title;
};

//-----------------------------------------------------------------------------

/*! \brief Node representing a paragraph of text and commands.
 *
 */
class ParagraphHandler : public DocNode, public BaseHandler<ParagraphHandler>
{
  public:
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void endParagraph();
    virtual void startItemizedList(const QXmlAttributes& attrib);
    virtual void startOrderedList(const QXmlAttributes& attrib);
    virtual void startParameterList(const QXmlAttributes& attrib);

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
    QList<ParagraphHandler> m_children;
};

#endif

#ifndef _DOCHANDLER_H
#define _DOCHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>

#include "basehandler.h"

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
      ListItem
    };
    DocNode(NodeKind k) : m_kind(k) {}
    virtual ~DocNode() {}
    
  private:
    NodeKind m_kind;
};

class TextNode : public DocNode
{
  public:
    TextNode(const QString &t,int markup) 
      : DocNode(Text), m_text(t), m_markup(markup) {}
  
  private:  
    QString m_text;
    int m_markup;
};

class MarkupModifierNode : public DocNode
{
  public:
    MarkupModifierNode(int markup,bool enabled) 
      : DocNode(MarkupModifier), m_markup(markup), m_enabled(enabled) {}
  
  private:
    int m_markup;
    bool m_enabled;
};

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

class ParagraphHandler : public DocNode, public BaseHandler<ParagraphHandler>
{
  public:
    virtual void startParagraph(const QXmlAttributes& attrib);
    virtual void endParagraph();
    virtual void startItemizedList(const QXmlAttributes& attrib);
    virtual void startOrderedList(const QXmlAttributes& attrib);

    ParagraphHandler(IBaseHandler *parent);
    virtual ~ParagraphHandler();

  private:
    void addTextNode();
    IBaseHandler   *m_parent;
    QList<DocNode>  m_children;
    MarkupHandler  *m_markupHandler;
};


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

/******************************************************************************
 *
 * $Id$
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
 */

#include <qmap.h>

#include "dochandler.h"
#include "debug.h"
#include "linkedtexthandler.h"


//----------------------------------------------------------------------

class TypeNameMapper
{
  public:
    TypeNameMapper()
    {
      m_map.insert("see",       SimpleSectHandler::See);
      m_map.insert("return",    SimpleSectHandler::Return);
      m_map.insert("author",    SimpleSectHandler::Author);
      m_map.insert("version",   SimpleSectHandler::Version);
      m_map.insert("since",     SimpleSectHandler::Since);
      m_map.insert("date",      SimpleSectHandler::Date);
      m_map.insert("bug",       SimpleSectHandler::Bug);
      m_map.insert("note",      SimpleSectHandler::Note);
      m_map.insert("warning",   SimpleSectHandler::Warning);
      m_map.insert("par",       SimpleSectHandler::Par);
      m_map.insert("deprecated",SimpleSectHandler::Deprecated);
      m_map.insert("pre",       SimpleSectHandler::Pre);
      m_map.insert("post",      SimpleSectHandler::Post);
      m_map.insert("invariant", SimpleSectHandler::Invar);
      m_map.insert("remark",    SimpleSectHandler::Remark);
      m_map.insert("attention", SimpleSectHandler::Attention);
      m_map.insert("todo",      SimpleSectHandler::Todo);
      m_map.insert("test",      SimpleSectHandler::Test);
      m_map.insert("rcs",       SimpleSectHandler::RCS);
      m_map.insert("enumvalues",SimpleSectHandler::EnumValues);
      m_map.insert("examples",  SimpleSectHandler::Examples);
    }
    SimpleSectHandler::Types stringToType(const QString &typeStr)
    {
      return m_map[typeStr];
    }
  private:
    QMap<QString,SimpleSectHandler::Types> m_map;
};

class HighlightMapper
{
  public:
    HighlightMapper()
    {
      m_map.insert("comment",        HighlightHandler::Comment);
      m_map.insert("keyword",        HighlightHandler::Keyword);
      m_map.insert("keywordtype",    HighlightHandler::KeywordType);
      m_map.insert("keywordflow",    HighlightHandler::KeywordFlow);
      m_map.insert("charliteral",    HighlightHandler::CharLiteral);
      m_map.insert("stringliteral",  HighlightHandler::StringLiteral);
      m_map.insert("preprocessor",   HighlightHandler::Preprocessor);
    }
    HighlightHandler::HighlightKind stringToKind(const QString &kindStr)
    {
      return m_map[kindStr];
    }
  private:
    QMap<QString,HighlightHandler::HighlightKind> m_map;
};

static TypeNameMapper *s_typeMapper;
static HighlightMapper *s_highlightMapper;

void dochandler_init()
{
  s_typeMapper = new TypeNameMapper;
  s_highlightMapper = new HighlightMapper;
}

void dochandler_exit()
{
  delete s_typeMapper;
  delete s_highlightMapper;
}
  
//----------------------------------------------------------------------
// MarkupHandler
//----------------------------------------------------------------------

MarkupHandler::MarkupHandler(QList<DocImpl> &children,QString &curString)
  : m_children(children), m_curString(curString), 
    m_curMarkup(IDocMarkup::Normal), m_headingLevel(0)
{
  addStartHandler("bold",this,&MarkupHandler::startBold);
  addEndHandler("bold",this,&MarkupHandler::endBold);
  
  addStartHandler("emphasis",this,&MarkupHandler::startEmphasis);
  addEndHandler("emphasis",this,&MarkupHandler::endEmphasis);
  
  addStartHandler("computeroutput",this,&MarkupHandler::startComputerOutput);
  addEndHandler("computeroutput",this,&MarkupHandler::endComputerOutput);

  addStartHandler("center",this,&MarkupHandler::startCenter);
  addEndHandler("center",this,&MarkupHandler::endCenter);

  addStartHandler("small",this,&MarkupHandler::startSmallFont);
  addEndHandler("small",this,&MarkupHandler::endSmallFont);

  addStartHandler("subscript",this,&MarkupHandler::startSubscript);
  addEndHandler("subscript",this,&MarkupHandler::endSubscript);

  addStartHandler("superscript",this,&MarkupHandler::startSuperscript);
  addEndHandler("superscript",this,&MarkupHandler::endSuperscript);

  addStartHandler("preformatted",this,&MarkupHandler::startPreformatted);
  addEndHandler("preformatted",this,&MarkupHandler::endPreformatted);

  addStartHandler("heading1",this,&MarkupHandler::startHeading1);
  addEndHandler("heading1",this,&MarkupHandler::endHeading1);
  
  addStartHandler("heading2",this,&MarkupHandler::startHeading2);
  addEndHandler("heading2",this,&MarkupHandler::endHeading2);

  addStartHandler("heading3",this,&MarkupHandler::startHeading3);
  addEndHandler("heading3",this,&MarkupHandler::endHeading3);

  addStartHandler("heading4",this,&MarkupHandler::startHeading4);
  addEndHandler("heading4",this,&MarkupHandler::endHeading4);

  addStartHandler("heading5",this,&MarkupHandler::startHeading5);
  addEndHandler("heading5",this,&MarkupHandler::endHeading5);

  addStartHandler("heading6",this,&MarkupHandler::startHeading6);
  addEndHandler("heading6",this,&MarkupHandler::endHeading6);
}

MarkupHandler::~MarkupHandler()
{
}

void MarkupHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_curMarkup,m_headingLevel));
    debug(2,"addTextNode() text=%s markup=%x\n",m_curString.data(),m_curMarkup);
    m_curString="";
  }
}

void MarkupHandler::startBold(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Bold,TRUE));
  m_curMarkup |= IDocMarkup::Bold;
}

void MarkupHandler::endBold()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Bold,FALSE));
  m_curMarkup &= ~IDocMarkup::Bold;
}

void MarkupHandler::startEmphasis(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Emphasis,TRUE));
  m_curMarkup |= IDocMarkup::Emphasis;
}

void MarkupHandler::endEmphasis()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Emphasis,FALSE));
  m_curMarkup &= ~IDocMarkup::Emphasis;
}

void MarkupHandler::startComputerOutput(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::ComputerOutput,TRUE));
  m_curMarkup |= IDocMarkup::ComputerOutput;
}

void MarkupHandler::endComputerOutput()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::ComputerOutput,FALSE));
  m_curMarkup &= ~IDocMarkup::ComputerOutput;
}

void MarkupHandler::startCenter(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Center,TRUE));
  m_curMarkup |= IDocMarkup::Center;
}

void MarkupHandler::endCenter()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Center,FALSE));
  m_curMarkup &= ~IDocMarkup::Center;
}

void MarkupHandler::startSmallFont(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::SmallFont,TRUE));
  m_curMarkup |= IDocMarkup::SmallFont;
}

void MarkupHandler::endSmallFont()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::SmallFont,FALSE));
  m_curMarkup &= ~IDocMarkup::SmallFont;
}

void MarkupHandler::startSubscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Subscript,TRUE));
  m_curMarkup |= IDocMarkup::Subscript;
}

void MarkupHandler::endSubscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Subscript,FALSE));
  m_curMarkup &= ~IDocMarkup::Subscript;
}

void MarkupHandler::startSuperscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Superscript,TRUE));
  m_curMarkup |= IDocMarkup::Superscript;
}

void MarkupHandler::endSuperscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Superscript,FALSE));
  m_curMarkup &= ~IDocMarkup::Superscript;
}

void MarkupHandler::startPreformatted(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Preformatted,TRUE));
  m_curMarkup |= IDocMarkup::Preformatted;
}

void MarkupHandler::endPreformatted()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Preformatted,FALSE));
  m_curMarkup &= ~IDocMarkup::Preformatted;
}

void MarkupHandler::startHeading1(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,1));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=1;
}

void MarkupHandler::endHeading1()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,1));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}

void MarkupHandler::startHeading2(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,2));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=2;
}

void MarkupHandler::endHeading2()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,2));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}

void MarkupHandler::startHeading3(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,3));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=3;
}

void MarkupHandler::endHeading3()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,3));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}

void MarkupHandler::startHeading4(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,4));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=4;
}

void MarkupHandler::endHeading4()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,4));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}

void MarkupHandler::startHeading5(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,5));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=5;
}

void MarkupHandler::endHeading5()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,5));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}

void MarkupHandler::startHeading6(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,TRUE,6));
  m_curMarkup |= IDocMarkup::Heading;
  m_headingLevel=6;
}

void MarkupHandler::endHeading6()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Heading,FALSE,6));
  m_curMarkup &= ~IDocMarkup::Heading;
  m_headingLevel=0;
}


//----------------------------------------------------------------------
// ListItemHandler
//----------------------------------------------------------------------

ListItemHandler::ListItemHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  
  addEndHandler("listitem",this,&ListItemHandler::endListItem);

  addStartHandler("para",this,&ListItemHandler::startParagraph);
}

ListItemHandler::~ListItemHandler()
{
}

void ListItemHandler::startListItem(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start list item handler\n");
}

void ListItemHandler::endListItem()
{
  debug(2,"end list item handler\n");
  m_parent->setDelegate(0);
}

void ListItemHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

IDocIterator *ListItemHandler::contents() const
{
  return new ListItemIterator(*this);
}

//----------------------------------------------------------------------
// OrderedListHandler
//----------------------------------------------------------------------

OrderedListHandler::OrderedListHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("orderedlist",this,&OrderedListHandler::endOrderedList);
  addStartHandler("listitem",this,&OrderedListHandler::startOrderedListItem);
}

OrderedListHandler::~OrderedListHandler()
{
}

void OrderedListHandler::startOrderedList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void OrderedListHandler::endOrderedList()
{
  m_parent->setDelegate(0);
}

void OrderedListHandler::startOrderedListItem(const QXmlAttributes& attrib)
{
  ListItemHandler *liHandler = new ListItemHandler(this);
  liHandler->startListItem(attrib);
  m_children.append(liHandler);
}

IDocIterator *OrderedListHandler::elements() const
{
  return new OrderedListIterator(*this);
}

//----------------------------------------------------------------------
// ItemizedListHandler
//----------------------------------------------------------------------

ItemizedListHandler::ItemizedListHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("itemizedlist",this,&ItemizedListHandler::endItemizedList);
  addStartHandler("listitem",this,&ItemizedListHandler::startItemizedListItem);
}

ItemizedListHandler::~ItemizedListHandler()
{
}

void ItemizedListHandler::startItemizedList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ItemizedListHandler::endItemizedList()
{
  m_parent->setDelegate(0);
}

void ItemizedListHandler::startItemizedListItem(const QXmlAttributes& attrib)
{
  ListItemHandler *liHandler = new ListItemHandler(this);
  liHandler->startListItem(attrib);
  m_children.append(liHandler);
}

IDocIterator *ItemizedListHandler::elements() const
{
  return new ItemizedListIterator(*this);
}

//----------------------------------------------------------------------
// TocListHandler
//----------------------------------------------------------------------

TocListHandler::TocListHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("toclist",this,&TocListHandler::endTocList);
  addStartHandler("tocitem",this,&TocListHandler::startTocItem);
}

TocListHandler::~TocListHandler()
{
}

void TocListHandler::startTocList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void TocListHandler::endTocList()
{
  m_parent->setDelegate(0);
}

void TocListHandler::startTocItem(const QXmlAttributes& attrib)
{
  TocItemHandler *tiHandler = new TocItemHandler(this);
  tiHandler->startTocItem(attrib);
  m_children.append(tiHandler);
}

IDocIterator *TocListHandler::elements() const
{
  return new TocListIterator(*this);
}

//----------------------------------------------------------------------
// TocItemHandler
//----------------------------------------------------------------------

TocItemHandler::TocItemHandler(IBaseHandler *parent) : m_parent(parent)
{
  addEndHandler("tocitem",this,&TocItemHandler::endTocItem);
}

TocItemHandler::~TocItemHandler()
{
}

void TocItemHandler::startTocItem(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_id = attrib.value("id");
  m_curString="";
}

void TocItemHandler::endTocItem()
{
  m_title = m_curString;
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// ParameterHandler
//----------------------------------------------------------------------

ParameterHandler::ParameterHandler(IBaseHandler *parent) : 
  m_parent(parent)
{
  addEndHandler("parametername",this,&ParameterHandler::endParameterName);
}

ParameterHandler::~ParameterHandler()
{
}

void ParameterHandler::startParameterName(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterHandler::endParameterName()
{
  m_name = m_curString;
  debug(2,"parameter %s\n",m_name.data());
  m_curString="";
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// ParameterListHandler
//----------------------------------------------------------------------

ParameterItemHandler::ParameterItemHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  addEndHandler("parameteritem",this,&ParameterItemHandler::endParameterItem);
  addStartHandler("parameternamelist");
  addEndHandler("parameternamelist");
  addStartHandler("parametername",this,&ParameterItemHandler::startParameterName);
  addStartHandler("parameterdescription");
  addEndHandler("parameterdescription");
  addStartHandler("para",this,&ParameterItemHandler::startParagraph);
  m_parameters.setAutoDelete(TRUE);
  m_description = 0;
}

ParameterItemHandler::~ParameterItemHandler()
{
  delete m_description;
}

void ParameterItemHandler::startParameterItem(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void ParameterItemHandler::endParameterItem()
{
  m_parent->setDelegate(0);
}

void ParameterItemHandler::startParameterName(const QXmlAttributes& attrib)
{
  ParameterHandler *param = new ParameterHandler(this);
  m_parameters.append(param);
  param->startParameterName(attrib);
}

void ParameterItemHandler::startParagraph(const QXmlAttributes& attrib)
{
  m_description = new ParagraphHandler(this);
  m_description->startParagraph(attrib);
}

IDocIterator *ParameterItemHandler::paramNames() const
{
  return new ParameterItemIterator(*this);
}

//----------------------------------------------------------------------
// ParameterListHandler
//----------------------------------------------------------------------

ParameterListHandler::ParameterListHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  addEndHandler("parameterlist",this,&ParameterListHandler::endParameterList);
  addStartHandler("parameteritem",this,&ParameterListHandler::startParameterItem);
  m_paramItems.setAutoDelete(TRUE);
}

ParameterListHandler::~ParameterListHandler()
{
}

void ParameterListHandler::startParameterList(const QXmlAttributes& attrib)
{
  QString kind = attrib.value("kind");
  if (kind=="retval")         m_type=RetVal;
  else if (kind=="exception") m_type=Exception;
  else if (kind=="param")     m_type=Param;
  else
  {
    debug(1,"Error: invalid parameterlist type: %s\n",kind.data());
  }
  debug(2,"parameterlist kind=%s\n",kind.data());
  m_parent->setDelegate(this);
}

void ParameterListHandler::endParameterList()
{
  m_parent->setDelegate(0);
}

void ParameterListHandler::startParameterItem(const QXmlAttributes& attrib)
{
  ParameterItemHandler *paramItem = new ParameterItemHandler(this);
  m_paramItems.append(paramItem);
  paramItem->startParameterItem(attrib);
}

IDocIterator *ParameterListHandler::params() const
{
  return new ParameterListIterator(*this);
}

//----------------------------------------------------------------------
// LinkHandler
//----------------------------------------------------------------------

LinkHandler::LinkHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("link",this,&LinkHandler::endLink);
}

LinkHandler::~LinkHandler()
{
}

void LinkHandler::startLink(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start link\n");
  m_ref = attrib.value("linkend");
  m_curString="";
}

void LinkHandler::endLink()
{
  m_text = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End link\n");
}

//----------------------------------------------------------------------
// EMailHandler
//----------------------------------------------------------------------

EMailHandler::EMailHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("email",this,&EMailHandler::endEMail);
}

EMailHandler::~EMailHandler()
{
}

void EMailHandler::startEMail(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start email\n");
  m_curString="";
}

void EMailHandler::endEMail()
{
  m_address = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End email\n");
}

//----------------------------------------------------------------------
// ULinkHandler
//----------------------------------------------------------------------

ULinkHandler::ULinkHandler(IBaseHandler *parent)
  :  m_parent(parent)
{
  addEndHandler("ulink",this,&ULinkHandler::endULink);
}

ULinkHandler::~ULinkHandler()
{
}

void ULinkHandler::startULink(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start ulink\n");
  m_url = attrib.value("url");
  m_curString="";
}

void ULinkHandler::endULink()
{
  m_text = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End ulink\n");
}

//----------------------------------------------------------------------
// LineBreakHandler
//----------------------------------------------------------------------

LineBreakHandler::LineBreakHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("linebreak",this,&LineBreakHandler::endLineBreak);
}

LineBreakHandler::~LineBreakHandler()
{
}

void LineBreakHandler::startLineBreak(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start linebreak\n");
}

void LineBreakHandler::endLineBreak()
{
  m_parent->setDelegate(0);
  debug(2,"End linebreak\n");
}

//----------------------------------------------------------------------
// HRulerHandler
//----------------------------------------------------------------------

HRulerHandler::HRulerHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("hruler",this,&HRulerHandler::endHRuler);
}

HRulerHandler::~HRulerHandler()
{
}

void HRulerHandler::startHRuler(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start hruler\n");
}

void HRulerHandler::endHRuler()
{
  m_parent->setDelegate(0);
  debug(2,"End hruler\n");
}

//----------------------------------------------------------------------
// RefHandler
//----------------------------------------------------------------------

RefHandler::RefHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("ref",this,&RefHandler::endRef);
}

RefHandler::~RefHandler()
{
}

void RefHandler::startRef(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_refId = attrib.value("refid");
  m_extId = attrib.value("external");
  ASSERT(attrib.value("kindref")=="compound" || 
         attrib.value("kindref")=="member");
  m_targetKind = attrib.value("kindref")=="compound" ?  Compound : Member;
  debug(2,"Start ref refId=%s\n",m_refId.data());
  m_curString="";
}

void RefHandler::endRef()
{
  m_linkText = m_curString;
  m_parent->setDelegate(0);
  debug(2,"End ref: text=`%s'\n",m_linkText.data());
}


//----------------------------------------------------------------------
// TitleHandler
//----------------------------------------------------------------------

TitleHandler::TitleHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  m_markupHandler = new MarkupHandler(m_children,m_curString);
  setFallBackHandler(m_markupHandler);
  addStartHandler("ref",this,&TitleHandler::startRef);
  addEndHandler("title",this,&TitleHandler::endTitle);
}

TitleHandler::~TitleHandler()
{
  delete m_markupHandler;
}

void TitleHandler::startTitle(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start title\n");
  m_curString="";
}

void TitleHandler::endTitle()
{
  addTextNode();
  m_parent->setDelegate(0);
  debug(2,"End title\n");
}

void TitleHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(
                      new TextNode(
                                   m_curString,
                                   m_markupHandler->markup(),
                                   m_markupHandler->headingLevel()
                                  )
                     );
    debug(2,"addTextNode() text=\"%s\" markup=%x headingLevel=%d\n",
        m_curString.data(),m_markupHandler->markup(),m_markupHandler->headingLevel());
    m_curString="";
  }
}

void TitleHandler::startRef(const QXmlAttributes& attrib)
{
  RefHandler *ref = new RefHandler(this);
  ref->startRef(attrib);
  m_children.append(ref);
}

IDocIterator *TitleHandler::title() const
{
  return new TitleIterator(*this);
}

//----------------------------------------------------------------------
// SimpleSectHandler
//----------------------------------------------------------------------

SimpleSectHandler::SimpleSectHandler(IBaseHandler *parent)
  : m_parent(parent), m_paragraph(0), m_title(0)
{
  addStartHandler("title",this,&SimpleSectHandler::startTitle);
  addStartHandler("para",this,&SimpleSectHandler::startParagraph);
  addEndHandler("simplesect",this,&SimpleSectHandler::endSimpleSect);
}

SimpleSectHandler::~SimpleSectHandler()
{
}

void SimpleSectHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  m_typeString = attrib.value("kind");
  m_type = s_typeMapper->stringToType(m_typeString);
  debug(2,"start simple section %s\n",m_typeString.data());
  m_parent->setDelegate(this);
}

void SimpleSectHandler::endSimpleSect()
{
  debug(2,"end simple section\n");
  m_parent->setDelegate(0);
}

void SimpleSectHandler::startTitle(const QXmlAttributes& attrib)
{
  ASSERT(m_title==0);
  m_title = new TitleHandler(this);
  m_title->startTitle(attrib);
}

void SimpleSectHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_paragraph==0);
  m_paragraph = new ParagraphHandler(this);
  m_paragraph->startParagraph(attrib);
}

//----------------------------------------------------------------------
// VariableListEntryHandler
//----------------------------------------------------------------------

VariableListEntryHandler::VariableListEntryHandler(IBaseHandler *parent)
   : m_parent(parent), m_description(0), m_linkedTextHandler(0)
{
  addStartHandler("term",this,&VariableListEntryHandler::startTerm);
  addEndHandler("term",this,&VariableListEntryHandler::endTerm);
  addStartHandler("para",this,&VariableListEntryHandler::startParagraph);
  addEndHandler("varlistentry",this,&VariableListEntryHandler::endVarListEntry);
  addEndHandler("listitem",this,&VariableListEntryHandler::endListItem);
}

VariableListEntryHandler::~VariableListEntryHandler()
{
  delete m_description;
}

void VariableListEntryHandler::startVarListEntry(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start varlistentry\n");
}

void VariableListEntryHandler::endVarListEntry()
{
  m_parent->setDelegate(0);
  debug(2,"end varlistentry\n");
}

void VariableListEntryHandler::startListItem(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start listitem\n");
}

void VariableListEntryHandler::endListItem()
{
  m_parent->setDelegate(0);
  debug(2,"end listitem\n");
}

void VariableListEntryHandler::startTerm(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
  m_linkedTextHandler = new LinkedTextHandler(this,m_term);
  m_linkedTextHandler->start("term");
}

void VariableListEntryHandler::endTerm()
{
  delete m_linkedTextHandler;
}

void VariableListEntryHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_description==0);
  m_description = new ParagraphHandler(this);
  m_description->startParagraph(attrib);
}

ILinkedTextIterator *VariableListEntryHandler::term() const 
{ 
  return new LinkedTextIterator(m_term); 
}


//----------------------------------------------------------------------
// VariableListHandler
//----------------------------------------------------------------------

VariableListHandler::VariableListHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_entries.setAutoDelete(TRUE);
  addStartHandler("varlistentry",this,&VariableListHandler::startVarListEntry);
  addStartHandler("listitem",this,&VariableListHandler::startListItem);
  addEndHandler("variablelist",this,&VariableListHandler::endVariableList);
}

VariableListHandler::~VariableListHandler()
{
}

void VariableListHandler::startVariableList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start variablelist\n");
}

void VariableListHandler::endVariableList()
{
  debug(2,"end variablelist\n");
  m_parent->setDelegate(0);
}

void VariableListHandler::startVarListEntry(const QXmlAttributes& attrib)
{
  VariableListEntryHandler *vle = new VariableListEntryHandler(this);
  vle->startVarListEntry(attrib);
  m_curEntry = vle;
  m_entries.append(vle);
}

void VariableListHandler::startListItem(const QXmlAttributes& attrib)
{
  ASSERT(m_curEntry!=0);
  m_curEntry->startListItem(attrib);
}

IDocIterator *VariableListHandler::entries() const
{
  return new VariableListIterator(*this);
}

//----------------------------------------------------------------------
// HighlightHandler
//----------------------------------------------------------------------

HighlightHandler::HighlightHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("highlight",this,&HighlightHandler::endHighlight);
  addStartHandler("ref",this,&HighlightHandler::startRef);
  addStartHandler("sp",this,&HighlightHandler::startSpace);
  m_hl = IDocHighlight::Invalid;
}

HighlightHandler::~HighlightHandler()
{
}

void HighlightHandler::startHighlight(const QXmlAttributes& attrib)
{
  m_hlString = attrib.value("class");
  m_hl = s_highlightMapper->stringToKind(m_hlString);
  m_curString="";
  m_parent->setDelegate(this);
  debug(2,"start highlight\n");
}

void HighlightHandler::endHighlight()
{
  addTextNode();
  debug(2,"end highlight class=`%s'\n",m_hlString.data());
  m_parent->setDelegate(0);
}

void HighlightHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *rh = new RefHandler(this);
  m_children.append(rh);
  rh->startRef(attrib);
}

void HighlightHandler::startSpace(const QXmlAttributes&)
{
  m_curString=" ";
  addTextNode();
}

void HighlightHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,IDocMarkup::Normal,0));
    debug(2,"addTextNode() text=\"%s\"\n",
        m_curString.data());
    m_curString="";
  }
}

IDocIterator *HighlightHandler::codeElements() const
{
  return new HighlightIterator(*this);
}

//----------------------------------------------------------------------
// CodeLineHandler
//----------------------------------------------------------------------

CodeLineHandler::CodeLineHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("codeline",this,&CodeLineHandler::endCodeLine);
  addEndHandler("linenumber",this,&CodeLineHandler::endLineNumber);
  addStartHandler("highlight",this,&CodeLineHandler::startHighlight);
  addStartHandler("ref",this,&CodeLineHandler::startRef);
  m_lineNumber = 0;
}

CodeLineHandler::~CodeLineHandler()
{
}

void CodeLineHandler::startCodeLine(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start codeline\n");
}

void CodeLineHandler::endCodeLine()
{
  addTextNode();
  debug(2,"end codeline\n");
  m_parent->setDelegate(0);
}

void CodeLineHandler::startLineNumber(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"start linenumber\n");
  m_lineNumber = attrib.value("line").toInt();
  m_refId = attrib.value("refid");
}

void CodeLineHandler::endLineNumber()
{
  m_parent->setDelegate(0);
}

void CodeLineHandler::startHighlight(const QXmlAttributes& attrib)
{
  addTextNode();
  HighlightHandler *hlh = new HighlightHandler(this);
  m_children.append(hlh);
  hlh->startHighlight(attrib);
}

void CodeLineHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *rh = new RefHandler(this);
  m_children.append(rh);
  rh->startRef(attrib);
}

void CodeLineHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,IDocMarkup::Normal,0));
    debug(2,"addTextNode() text=\"%s\"\n",
        m_curString.data());
    m_curString="";
  }
}

IDocIterator *CodeLineHandler::codeElements() const
{
  return new CodeLineIterator(*this);
}


//----------------------------------------------------------------------
// ProgramListingHandler
//----------------------------------------------------------------------

ProgramListingHandler::ProgramListingHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  m_hasLineNumber=FALSE;
  addEndHandler("programlisting",this,&ProgramListingHandler::endProgramListing);

  addStartHandler("linenumber",this,&ProgramListingHandler::startLineNumber);
  addStartHandler("codeline",this,&ProgramListingHandler::startCodeLine);
}

ProgramListingHandler::~ProgramListingHandler()
{
}

void ProgramListingHandler::startProgramListing(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start programlisting\n");
}

void ProgramListingHandler::endProgramListing()
{
  debug(2,"end programlisting\n");
  m_parent->setDelegate(0);
}

void ProgramListingHandler::startLineNumber(const QXmlAttributes& attrib)
{
  CodeLineHandler *clh = new CodeLineHandler(this);
  m_children.append(clh);
  m_hasLineNumber=TRUE;
  clh->startLineNumber(attrib);
}
  
void ProgramListingHandler::startCodeLine(const QXmlAttributes& attrib)
{
  CodeLineHandler *clh = 0;
  if (!m_hasLineNumber) 
  {
    clh = new CodeLineHandler(this);
    m_children.append(clh);
  }
  else
  {
    clh = m_children.getLast();
  }
  ASSERT(clh!=0);
  clh->startCodeLine(attrib);
  m_hasLineNumber=FALSE; 
}

IDocIterator *ProgramListingHandler::codeLines() const
{
  return new ProgramListingIterator(*this);
}



//----------------------------------------------------------------------
// FormulaHandler
//----------------------------------------------------------------------

FormulaHandler::FormulaHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("formula",this,&FormulaHandler::endFormula);
}

FormulaHandler::~FormulaHandler()
{
}

void FormulaHandler::startFormula(const QXmlAttributes& attrib)
{
  m_id = attrib.value("id");
  m_curString="";
  m_parent->setDelegate(this);
}

void FormulaHandler::endFormula()
{
  m_text = m_curString;
  debug(2,"formula id=`%s' text=`%s'\n",m_id.data(),m_text.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// AnchorHandler
//----------------------------------------------------------------------

AnchorHandler::AnchorHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("anchor",this,&AnchorHandler::endAnchor);
}

AnchorHandler::~AnchorHandler()
{
}

void AnchorHandler::startAnchor(const QXmlAttributes& attrib)
{
  m_id = attrib.value("id");
  m_parent->setDelegate(this);
}

void AnchorHandler::endAnchor()
{
  debug(2,"anchor id=`%s'\n",m_id.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// ImageHandler
//----------------------------------------------------------------------

ImageHandler::ImageHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("image",this,&ImageHandler::endImage);
}

ImageHandler::~ImageHandler()
{
}

void ImageHandler::startImage(const QXmlAttributes& attrib)
{
  m_name = attrib.value("name");
  m_curString="";
  m_parent->setDelegate(this);
}

void ImageHandler::endImage()
{
  m_caption = m_curString;
  debug(2,"image name=`%s' caption=`%s'\n",m_name.data(),m_caption.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// DotFileHandler
//----------------------------------------------------------------------

DotFileHandler::DotFileHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("dotfile",this,&DotFileHandler::endDotFile);
}

DotFileHandler::~DotFileHandler()
{
}

void DotFileHandler::startDotFile(const QXmlAttributes& attrib)
{
  m_name = attrib.value("name");
  m_curString="";
  m_parent->setDelegate(this);
}

void DotFileHandler::endDotFile()
{
  m_caption = m_curString;
  debug(2,"image name=`%s' caption=`%s'\n",m_name.data(),m_caption.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// IndexEntryHandler
//----------------------------------------------------------------------

IndexEntryHandler::IndexEntryHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("indexentry",this,&IndexEntryHandler::endIndexEntry);
  addStartHandler("primaryie",this,&IndexEntryHandler::startPrimaryIE);
  addEndHandler("primaryie",this,&IndexEntryHandler::endPrimaryIE);
  addStartHandler("secondaryie",this,&IndexEntryHandler::startSecondaryIE);
  addEndHandler("secondaryie",this,&IndexEntryHandler::endSecondaryIE);
}

IndexEntryHandler::~IndexEntryHandler()
{
}

void IndexEntryHandler::startIndexEntry(const QXmlAttributes& /*attrib*/)
{
  debug(2,"start index entry\n");
  m_parent->setDelegate(this);
}

void IndexEntryHandler::endIndexEntry()
{
  debug(2,"index entry primary=`%s' secondary=`%s'\n",
      m_primary.data(),m_secondary.data());
  m_parent->setDelegate(0);
}

void IndexEntryHandler::startPrimaryIE(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void IndexEntryHandler::endPrimaryIE()
{
  m_primary = m_curString;
}

void IndexEntryHandler::startSecondaryIE(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void IndexEntryHandler::endSecondaryIE()
{
  m_secondary = m_curString;
}

//----------------------------------------------------------------------
// EntryHandler
//----------------------------------------------------------------------

EntryHandler::EntryHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("entry",this,&EntryHandler::endEntry);
  addStartHandler("para",this,&EntryHandler::startParagraph);
}

EntryHandler::~EntryHandler()
{
}

void EntryHandler::startEntry(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void EntryHandler::endEntry()
{
  m_parent->setDelegate(0);
}

void EntryHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *ph = new ParagraphHandler(this);
  ph->startParagraph(attrib);
  m_children.append(ph);
}

IDocIterator *EntryHandler::contents() const
{
  return new EntryIterator(*this);
}

//----------------------------------------------------------------------
// RowHandler
//----------------------------------------------------------------------

RowHandler::RowHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("row",this,&RowHandler::endRow);
  addStartHandler("entry",this,&RowHandler::startEntry);
}

RowHandler::~RowHandler()
{
}

void RowHandler::startRow(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void RowHandler::endRow()
{
  m_parent->setDelegate(0);
}

void RowHandler::startEntry(const QXmlAttributes& attrib)
{
  EntryHandler *eh = new EntryHandler(this);
  eh->startEntry(attrib);
  m_children.append(eh);
}

IDocIterator *RowHandler::entries() const
{
  return new RowIterator(*this);
}

//----------------------------------------------------------------------
// TableHandler
//----------------------------------------------------------------------

TableHandler::TableHandler(IBaseHandler *parent)
  :  m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("table",this,&TableHandler::endTable);
  addStartHandler("row",this,&TableHandler::startRow);
  addStartHandler("caption",this,&TableHandler::startCaption);
  addEndHandler("caption",this,&TableHandler::endCaption);
}

TableHandler::~TableHandler()
{
}

void TableHandler::startTable(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_numColumns = attrib.value("cols").toInt();
  debug(2,"table cols=%d\n",m_numColumns);
}

void TableHandler::endTable()
{
  m_parent->setDelegate(0);
}

void TableHandler::startRow(const QXmlAttributes& attrib)
{
  RowHandler *rh = new RowHandler(this);
  rh->startRow(attrib);
  m_children.append(rh);
}

void TableHandler::startCaption(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void TableHandler::endCaption()
{
  m_caption = m_curString;
}

IDocIterator *TableHandler::rows() const
{
  return new TableIterator(*this);
}

//----------------------------------------------------------------------
// CopyHandler
//----------------------------------------------------------------------

CopyHandler::CopyHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  
  addEndHandler("copydoc",this,&CopyHandler::endCopy);

  addStartHandler("para",this,&CopyHandler::startParagraph);
}

CopyHandler::~CopyHandler()
{
}

void CopyHandler::startCopy(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start copy handler\n");
}

void CopyHandler::endCopy()
{
  debug(2,"end copy handler\n");
  m_parent->setDelegate(0);
}

void CopyHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

IDocIterator *CopyHandler::contents() const
{
  return new CopyIterator(*this);
}

//----------------------------------------------------------------------
// VerbatimHandler
//----------------------------------------------------------------------

VerbatimHandler::VerbatimHandler(IBaseHandler *parent)
  :  m_parent(parent), m_type(IDocVerbatim::Invalid)
{
  addEndHandler("verbatim",this,&VerbatimHandler::endVerbatim);
  addEndHandler("latexonly",this,&VerbatimHandler::endVerbatim);
  addEndHandler("htmlonly",this,&VerbatimHandler::endVerbatim);
}

VerbatimHandler::~VerbatimHandler()
{
}

void VerbatimHandler::startVerbatim(const QXmlAttributes&,Types type)
{
  m_type = type;
  m_parent->setDelegate(this);
  m_curString="";
}

void VerbatimHandler::endVerbatim()
{
  m_text = m_curString;
  m_parent->setDelegate(0);
}


//----------------------------------------------------------------------
// SymbolHandler
//----------------------------------------------------------------------

SymbolHandler::SymbolHandler(IBaseHandler *parent,Types type)
  :  m_parent(parent), m_letter('\0'), m_type(type)
{
  addEndHandler("symbol");
  switch (type)
  {
    case IDocSymbol::Invalid: m_typeString="invalid"; break;
    case IDocSymbol::Umlaut:  m_typeString="umlaut"; break;
    case IDocSymbol::Acute:   m_typeString="acute"; break;
    case IDocSymbol::Grave:   m_typeString="grave"; break;
    case IDocSymbol::Circ:    m_typeString="circ"; break;
    case IDocSymbol::Tilde:   m_typeString="tilde"; break;
    case IDocSymbol::Szlig:   m_typeString="szlig"; break;
    case IDocSymbol::Cedil:   m_typeString="cedil"; break;
    case IDocSymbol::Ring:    m_typeString="ring"; break;
    case IDocSymbol::Nbsp:    m_typeString="nbsp"; break;
    case IDocSymbol::Copy:    m_typeString="copy"; break;
  }
}

SymbolHandler::~SymbolHandler()
{
}

void SymbolHandler::startSymbol(const QXmlAttributes& attrib)
{
  QString ls = attrib.value("char");
  if (!ls.isEmpty()) m_letter = ls.latin1()[0];
}

//----------------------------------------------------------------------
// ParagraphHandler
//----------------------------------------------------------------------

ParagraphHandler::ParagraphHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  m_markupHandler = new MarkupHandler(m_children,m_curString);

  // preformatted
  setFallBackHandler(m_markupHandler);

  addEndHandler("para",this,&ParagraphHandler::endParagraph);

  addStartHandler("linebreak",this,&ParagraphHandler::startLineBreak);
  addStartHandler("hruler",this,&ParagraphHandler::startHRuler);
  addStartHandler("programlisting",this,&ParagraphHandler::startProgramListing);
  addStartHandler("verbatim",this,&ParagraphHandler::startVerbatim);
  addStartHandler("indexentry",this,&ParagraphHandler::startIndexEntry);
  addStartHandler("orderedlist",this,&ParagraphHandler::startOrderedList);
  addStartHandler("itemizedlist",this,&ParagraphHandler::startItemizedList);
  addStartHandler("simplesect",this,&ParagraphHandler::startSimpleSect);
  // TODO: title
  addStartHandler("variablelist",this,&ParagraphHandler::startVariableList);
  addStartHandler("table",this,&ParagraphHandler::startTable);
  // TODO: heading
  addStartHandler("image",this,&ParagraphHandler::startImage);
  addStartHandler("dotfile",this,&ParagraphHandler::startDotFile);
  addStartHandler("toclist",this,&ParagraphHandler::startTocList);
  // TODO: language???
  addStartHandler("parameterlist",this,&ParagraphHandler::startParameterList);
  // TODO: xrefsect
  addStartHandler("copydoc",this,&ParagraphHandler::startCopyDoc);

  addStartHandler("ref",this,&ParagraphHandler::startRef);
  addStartHandler("ulink",this,&ParagraphHandler::startULink);
  addStartHandler("email",this,&ParagraphHandler::startEMail);
  addStartHandler("link",this,&ParagraphHandler::startLink);
  addStartHandler("formula",this,&ParagraphHandler::startFormula);
  addStartHandler("latexonly",this,&ParagraphHandler::startHtmlOnly);
  addStartHandler("htmlonly",this,&ParagraphHandler::startLatexOnly);
  addStartHandler("umlaut",this,&ParagraphHandler::startUmlaut);
  addStartHandler("acute",this,&ParagraphHandler::startAcute);
  addStartHandler("grave",this,&ParagraphHandler::startGrave);
  addStartHandler("circ",this,&ParagraphHandler::startCirc);
  addStartHandler("tilde",this,&ParagraphHandler::startTilde);
  addStartHandler("szlig",this,&ParagraphHandler::startSzlig);
  addStartHandler("cedil",this,&ParagraphHandler::startCedil);
  addStartHandler("ring",this,&ParagraphHandler::startRing);
  addStartHandler("nbsp",this,&ParagraphHandler::startNbsp);
  addStartHandler("copy",this,&ParagraphHandler::startCopy);
  addStartHandler("anchor",this,&ParagraphHandler::startAnchor);
}

ParagraphHandler::~ParagraphHandler()
{
  delete m_markupHandler;
}

void ParagraphHandler::startParagraph(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"para\n");
}

void ParagraphHandler::endParagraph()
{
  addTextNode();
  debug(2,"end para\n");
  m_parent->setDelegate(0);
}

void ParagraphHandler::startItemizedList(const QXmlAttributes& attrib)
{
  addTextNode();
  ItemizedListHandler *listHandler = new ItemizedListHandler(this);
  listHandler->startItemizedList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startOrderedList(const QXmlAttributes& attrib)
{
  addTextNode();
  OrderedListHandler *listHandler = new OrderedListHandler(this);
  listHandler->startOrderedList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startParameterList(const QXmlAttributes& attrib)
{
  addTextNode();
  ParameterListHandler *listHandler = new ParameterListHandler(this);
  listHandler->startParameterList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  addTextNode();
  SimpleSectHandler *sectHandler = new SimpleSectHandler(this);
  sectHandler->startSimpleSect(attrib);
  m_children.append(sectHandler);
}

void ParagraphHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *ref = new RefHandler(this);
  ref->startRef(attrib);
  m_children.append(ref);
}

void ParagraphHandler::startVariableList(const QXmlAttributes& attrib)
{
  addTextNode();
  VariableListHandler *vl = new VariableListHandler(this);
  vl->startVariableList(attrib);
  m_children.append(vl);
}

void ParagraphHandler::startHRuler(const QXmlAttributes& attrib)
{
  addTextNode();
  HRulerHandler *hr = new HRulerHandler(this);
  hr->startHRuler(attrib);
  m_children.append(hr);
}

void ParagraphHandler::startLineBreak(const QXmlAttributes& attrib)
{
  addTextNode();
  LineBreakHandler *lb = new LineBreakHandler(this);
  lb->startLineBreak(attrib);
  m_children.append(lb);
}

void ParagraphHandler::startULink(const QXmlAttributes& attrib)
{
  addTextNode();
  ULinkHandler *uh = new ULinkHandler(this);
  uh->startULink(attrib);
  m_children.append(uh);
}

void ParagraphHandler::startEMail(const QXmlAttributes& attrib)
{
  addTextNode();
  EMailHandler *eh = new EMailHandler(this);
  eh->startEMail(attrib);
  m_children.append(eh);
}

void ParagraphHandler::startLink(const QXmlAttributes& attrib)
{
  addTextNode();
  LinkHandler *lh = new LinkHandler(this);
  lh->startLink(attrib);
  m_children.append(lh);
}

void ParagraphHandler::startProgramListing(const QXmlAttributes& attrib)
{
  addTextNode();
  ProgramListingHandler *pl = new ProgramListingHandler(this);
  pl->startProgramListing(attrib);
  m_children.append(pl);
}

void ParagraphHandler::startFormula(const QXmlAttributes& attrib)
{
  addTextNode();
  FormulaHandler *fh = new FormulaHandler(this);
  fh->startFormula(attrib);
  m_children.append(fh);
}

void ParagraphHandler::startImage(const QXmlAttributes& attrib)
{
  addTextNode();
  ImageHandler *ih = new ImageHandler(this);
  ih->startImage(attrib);
  m_children.append(ih);
}

void ParagraphHandler::startDotFile(const QXmlAttributes& attrib)
{
  addTextNode();
  DotFileHandler *df = new DotFileHandler(this);
  df->startDotFile(attrib);
  m_children.append(df);
}

void ParagraphHandler::startIndexEntry(const QXmlAttributes& attrib)
{
  addTextNode();
  IndexEntryHandler *df = new IndexEntryHandler(this);
  df->startIndexEntry(attrib);
  m_children.append(df);
}

void ParagraphHandler::startTable(const QXmlAttributes& attrib)
{
  addTextNode();
  TableHandler *th = new TableHandler(this);
  th->startTable(attrib);
  m_children.append(th);
}

void ParagraphHandler::startVerbatim(const QXmlAttributes& attrib)
{
  addTextNode();
  VerbatimHandler *vh = new VerbatimHandler(this);
  vh->startVerbatim(attrib,IDocVerbatim::Verbatim);
  m_children.append(vh);
}

void ParagraphHandler::startHtmlOnly(const QXmlAttributes& attrib)
{
  addTextNode();
  VerbatimHandler *vh = new VerbatimHandler(this);
  vh->startVerbatim(attrib,IDocVerbatim::HtmlOnly);
  m_children.append(vh);
}

void ParagraphHandler::startLatexOnly(const QXmlAttributes& attrib)
{
  addTextNode();
  VerbatimHandler *vh = new VerbatimHandler(this);
  vh->startVerbatim(attrib,IDocVerbatim::LatexOnly);
  m_children.append(vh);
}

void ParagraphHandler::startUmlaut(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Umlaut);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startAcute(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Acute);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startGrave(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Grave);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCirc(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Circ);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startTilde(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Tilde);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startSzlig(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Szlig);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCedil(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Cedil);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startRing(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Ring);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startNbsp(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Nbsp);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCopy(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Copy);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startAnchor(const QXmlAttributes& attrib)
{
  addTextNode();
  AnchorHandler *ah = new AnchorHandler(this);
  ah->startAnchor(attrib);
  m_children.append(ah);
}

void ParagraphHandler::startCopyDoc(const QXmlAttributes& attrib)
{
  addTextNode();
  CopyHandler *ch = new CopyHandler(this);
  ch->startCopy(attrib);
  m_children.append(ch);
}

void ParagraphHandler::startTocList(const QXmlAttributes& attrib)
{
  addTextNode();
  TocListHandler *th = new TocListHandler(this);
  th->startTocList(attrib);
  m_children.append(th);
}

void ParagraphHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(
                      new TextNode(
                                   m_curString,
                                   m_markupHandler->markup(),
                                   m_markupHandler->headingLevel()
                                  )
                     );
    debug(2,"addTextNode() text=\"%s\" markup=%x headingLevel=%d\n",
        m_curString.data(),m_markupHandler->markup(),m_markupHandler->headingLevel());
    m_curString="";
  }
}

IDocIterator *ParagraphHandler::contents() const
{
  return new ParagraphIterator(*this);
}

//----------------------------------------------------------------------
// DocSectionHandler
//----------------------------------------------------------------------

DocSectionHandler::DocSectionHandler(IBaseHandler *parent,int level)
  : m_parent(parent), m_internal(0), m_level(level), m_title(0)
{
  QString sectionKey;
  m_paragraphs.setAutoDelete(TRUE);
  m_subsections.setAutoDelete(TRUE);
  addStartHandler("title",this,&DocSectionHandler::startTitle);
  addStartHandler("para",this,&DocSectionHandler::startParagraph);
  if (level<6) 
  {
    sectionKey.sprintf("sect%d",level+1);
    addStartHandler(sectionKey.utf8(),this,&DocSectionHandler::startSubSection);
  }
  addStartHandler("internal",this,&DocSectionHandler::startInternal);
  sectionKey.sprintf("sect%d",level);
  addEndHandler(sectionKey.utf8(),this,&DocSectionHandler::endDocSection);
}

DocSectionHandler::~DocSectionHandler()
{
}

void DocSectionHandler::startDocSection(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start docsection\n");
  m_id = attrib.value("id");
}

void DocSectionHandler::endDocSection()
{
  m_parent->setDelegate(0);
  debug(2,"End docsection\n");
}

void DocSectionHandler::startSubSection(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,m_level+1);
  secHandler->startDocSection(attrib);
  m_subsections.append(secHandler);
}

void DocSectionHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_paragraphs.append(parHandler);
}

void DocSectionHandler::startInternal(const QXmlAttributes& attrib)
{
  m_internal = new DocInternalHandler(this,m_level);
  m_internal->startInternal(attrib);
}

void DocSectionHandler::startTitle(const QXmlAttributes& attrib)
{
  m_title = new TitleHandler(this);
  m_title->startTitle(attrib);
}

IDocIterator *DocSectionHandler::paragraphs() const
{
  return new DocSectionParaIterator(*this);
}

IDocIterator *DocSectionHandler::subSections() const
{
  return new DocSectionSubIterator(*this);
}

IDocInternal *DocSectionHandler::internal() const 
{ 
  return m_internal; 
}

//----------------------------------------------------------------------
// DocInternal
//----------------------------------------------------------------------

DocInternalHandler::DocInternalHandler(IBaseHandler *parent,int level)
  : m_parent(parent), m_level(level)
{
  m_paragraphs.setAutoDelete(TRUE);
  m_subsections.setAutoDelete(TRUE);
  addStartHandler("para",this,&DocInternalHandler::startParagraph);
  QString sectionKey;
  sectionKey.sprintf("sect%d",level+1);
  addStartHandler(sectionKey.utf8(),this,&DocInternalHandler::startSubSection);
  addEndHandler("internal",this,&DocInternalHandler::endInternal);
}

DocInternalHandler::~DocInternalHandler()
{
}

void DocInternalHandler::startInternal(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
  debug(2,"Start internal\n");
}

void DocInternalHandler::endInternal()
{
  m_parent->setDelegate(0);
  debug(2,"End internal\n");
}

void DocInternalHandler::startSubSection(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,m_level+1);
  secHandler->startDocSection(attrib);
  m_subsections.append(secHandler);
}

void DocInternalHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_paragraphs.append(parHandler);
}

IDocIterator *DocInternalHandler::paragraphs() const
{
  return new DocInternalParaIterator(*this);
}

IDocIterator *DocInternalHandler::subSections() const
{
  return new DocInternalSubIterator(*this);
}


//----------------------------------------------------------------------
// DocHandler
//----------------------------------------------------------------------

DocHandler::DocHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  addEndHandler("briefdescription",this,&DocHandler::endDoc);
  addEndHandler("detaileddescription",this,&DocHandler::endDoc);
  addEndHandler("inbodydescription",this,&DocHandler::endDoc);
  //addEndHandler("internal"); // TODO: implement this as a section
  addStartHandler("internal",this,&DocHandler::startInternal);

  addStartHandler("para",this,&DocHandler::startParagraph);
  addStartHandler("sect1",this,&DocHandler::startSect1);
  addStartHandler("title",this,&DocHandler::startTitle);
  //addStartHandler("internal");
}

DocHandler::~DocHandler()
{
}

void DocHandler::startDoc(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start dochandler\n");
}

void DocHandler::endDoc()
{
  debug(2,"end dochandler\n");
  m_parent->setDelegate(0);
}

void DocHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

void DocHandler::startSect1(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,1);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startTitle(const QXmlAttributes& attrib)
{
  TitleHandler *titleHandler = new TitleHandler(this);
  titleHandler->startTitle(attrib);
  m_children.append(titleHandler);
}

void DocHandler::startInternal(const QXmlAttributes& attrib)
{
  m_internal = new DocInternalHandler(this,1);
  m_internal->startInternal(attrib);
}

IDocIterator *DocHandler::contents() const
{
  return new DocIterator(*this);
}

IDocInternal *DocHandler::internal() const 
{ 
  return m_internal; 
}


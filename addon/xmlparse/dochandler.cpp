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

#include "dochandler.h"
#include <qmap.h>

class TypeNameMapper
{
  public:
    TypeNameMapper()
    {
      m_typeNameMap.insert("see",       SimpleSectHandler::See);
      m_typeNameMap.insert("return",    SimpleSectHandler::Return);
      m_typeNameMap.insert("author",    SimpleSectHandler::Author);
      m_typeNameMap.insert("version",   SimpleSectHandler::Version);
      m_typeNameMap.insert("since",     SimpleSectHandler::Since);
      m_typeNameMap.insert("date",      SimpleSectHandler::Date);
      m_typeNameMap.insert("bug",       SimpleSectHandler::Bug);
      m_typeNameMap.insert("note",      SimpleSectHandler::Note);
      m_typeNameMap.insert("warning",   SimpleSectHandler::Warning);
      m_typeNameMap.insert("par",       SimpleSectHandler::Par);
      m_typeNameMap.insert("deprecated",SimpleSectHandler::Deprecated);
      m_typeNameMap.insert("pre",       SimpleSectHandler::Pre);
      m_typeNameMap.insert("post",      SimpleSectHandler::Post);
      m_typeNameMap.insert("invariant", SimpleSectHandler::Invar);
      m_typeNameMap.insert("remark",    SimpleSectHandler::Remark);
      m_typeNameMap.insert("attention", SimpleSectHandler::Attention);
      m_typeNameMap.insert("todo",      SimpleSectHandler::Todo);
      m_typeNameMap.insert("test",      SimpleSectHandler::Test);
      m_typeNameMap.insert("rcs",       SimpleSectHandler::RCS);
      m_typeNameMap.insert("enumvalues",SimpleSectHandler::EnumValues);
      m_typeNameMap.insert("examples",  SimpleSectHandler::Examples);
    }
    SimpleSectHandler::Types stringToType(const QString &typeStr)
    {
      return m_typeNameMap[typeStr];
    }
  private:
    QMap<QString,SimpleSectHandler::Types> m_typeNameMap;
};

static TypeNameMapper g_typeMapper;

  
//----------------------------------------------------------------------
// MarkupHandler
//----------------------------------------------------------------------

MarkupHandler::MarkupHandler(QList<DocNode> &children,QString &curString)
  : m_children(children), m_curString(curString), m_curMarkup(DocNode::Normal)
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
}

MarkupHandler::~MarkupHandler()
{
}

void MarkupHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_curMarkup));
    printf("addTextNode() text=%s markup=%x\n",m_curString.data(),m_curMarkup);
    m_curString="";
  }
}

void MarkupHandler::startBold(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Bold,TRUE));
  m_curMarkup |= DocNode::Bold;
}

void MarkupHandler::endBold()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Bold,FALSE));
  m_curMarkup &= ~DocNode::Bold;
}

void MarkupHandler::startEmphasis(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Emphasis,TRUE));
  m_curMarkup |= DocNode::Emphasis;
}

void MarkupHandler::endEmphasis()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Emphasis,FALSE));
  m_curMarkup &= ~DocNode::Emphasis;
}

void MarkupHandler::startComputerOutput(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::ComputerOutput,TRUE));
  m_curMarkup |= DocNode::ComputerOutput;
}

void MarkupHandler::endComputerOutput()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::ComputerOutput,FALSE));
  m_curMarkup &= ~DocNode::ComputerOutput;
}

void MarkupHandler::startCenter(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Center,TRUE));
  m_curMarkup |= DocNode::Center;
}

void MarkupHandler::endCenter()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Center,FALSE));
  m_curMarkup &= ~DocNode::Center;
}

void MarkupHandler::startSmallFont(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::SmallFont,TRUE));
  m_curMarkup |= DocNode::SmallFont;
}

void MarkupHandler::endSmallFont()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::SmallFont,FALSE));
  m_curMarkup &= ~DocNode::SmallFont;
}

void MarkupHandler::startSubscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Subscript,TRUE));
  m_curMarkup |= DocNode::Subscript;
}

void MarkupHandler::endSubscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Subscript,FALSE));
  m_curMarkup &= ~DocNode::Subscript;
}

void MarkupHandler::startSuperscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Superscript,TRUE));
  m_curMarkup |= DocNode::Superscript;
}

void MarkupHandler::endSuperscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(DocNode::Superscript,FALSE));
  m_curMarkup &= ~DocNode::Superscript;
}

//----------------------------------------------------------------------
// ListItemHandler
//----------------------------------------------------------------------

ListItemHandler::ListItemHandler(IBaseHandler *parent) 
  : DocNode(ListItem), m_parent(parent)
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
  printf("start list item handler\n");
}

void ListItemHandler::endListItem()
{
  printf("end list item handler\n");
  m_parent->setDelegate(0);
}

void ListItemHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

//----------------------------------------------------------------------
// ListHandler
//----------------------------------------------------------------------

ListHandler::ListHandler(NodeKind k,IBaseHandler *parent) 
  : DocNode(k), m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  const char *endListName=0;
  switch(k)
  {
    case ItemizedList : endListName="itemizedlist"; break;
    case OrderedList :  endListName="orderedlist";  break;
    default: ASSERT(0);
  }
  addEndHandler(endListName,this,&ListHandler::endList);

  addStartHandler("listitem",this,&ListHandler::startListItem);
}

ListHandler::~ListHandler()
{
}

void ListHandler::startList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ListHandler::endList()
{
  m_parent->setDelegate(0);
}

void ListHandler::startListItem(const QXmlAttributes& attrib)
{
  ListItemHandler *liHandler = new ListItemHandler(this);
  liHandler->startListItem(attrib);
  m_children.append(liHandler);
}

//----------------------------------------------------------------------
// ParameterHandler
//----------------------------------------------------------------------

ParameterHandler::ParameterHandler(IBaseHandler *parent) : DocNode(Parameter),
  m_parent(parent), m_description(0)
{
  addEndHandler("parametername",this,&ParameterHandler::endParameterName);
  addEndHandler("parameterdescription",this,&ParameterHandler::endParameterDescription);
  addStartHandler("para",this,&ParameterHandler::startParagraph);
}

ParameterHandler::~ParameterHandler()
{
  delete m_description;
}

void ParameterHandler::startParameterName(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterHandler::endParameterName()
{
  m_name = m_curString;
  printf("parameter %s\n",m_name.data());
  m_curString="";
  m_parent->setDelegate(0);
}

void ParameterHandler::startParameterDescription(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterHandler::endParameterDescription()
{
  m_parent->setDelegate(0);
}

void ParameterHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_description==0);
  m_description = new ParagraphHandler(this);
  m_description->startParagraph(attrib);
}

//----------------------------------------------------------------------
// ParameterListHandler
//----------------------------------------------------------------------

ParameterListHandler::ParameterListHandler(IBaseHandler *parent) 
  : DocNode(ParameterList), m_parent(parent)
{
  addEndHandler("parameterlist",this,&ParameterListHandler::endParameterList);
  addStartHandler("parametername",this,&ParameterListHandler::startParameterName);
  addStartHandler("parameterdescription",this,&ParameterListHandler::startParameterDescription);
  addStartHandler("title");
  addEndHandler("title");
  m_parameters.setAutoDelete(TRUE);
  m_curParam=0;
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
    printf("Error: invalid parameterlist type: %s\n",kind.data());
  }
  printf("parameterlist kind=%s\n",kind.data());
  m_parent->setDelegate(this);
}

void ParameterListHandler::endParameterList()
{
  m_parent->setDelegate(0);
}

void ParameterListHandler::startParameterName(const QXmlAttributes& attrib)
{
  m_curParam = new ParameterHandler(this);
  m_parameters.append(m_curParam);
  m_curParam->startParameterName(attrib);
}

void ParameterListHandler::startParameterDescription(const QXmlAttributes& attrib)
{
  ASSERT(m_curParam!=0);
  m_curParam->startParameterDescription(attrib);
}

//----------------------------------------------------------------------
// SimpleSectHandler
//----------------------------------------------------------------------

SimpleSectHandler::SimpleSectHandler(IBaseHandler *parent)
  : DocNode(Para), m_parent(parent), m_paragraph(0)
{
  addStartHandler("title",this,&SimpleSectHandler::startTitle);
  addEndHandler("title",this,&SimpleSectHandler::endTitle);
  addStartHandler("para",this,&SimpleSectHandler::startParagraph);
}

SimpleSectHandler::~SimpleSectHandler()
{
}

void SimpleSectHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  m_type = g_typeMapper.stringToType(attrib.value("kind"));
  addEndHandler("simplesect",this,&SimpleSectHandler::endSimpleSect);
  printf("start simple section %s\n",attrib.value("kind").data());
  m_parent->setDelegate(this);
}

void SimpleSectHandler::endSimpleSect()
{
  printf("end simple section\n");
  m_parent->setDelegate(0);
}

void SimpleSectHandler::startTitle(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void SimpleSectHandler::endTitle()
{
  printf("simpleSect title=\"%s\"\n",m_curString.data());
  m_title = m_curString;
  m_curString="";
}


void SimpleSectHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_paragraph==0);
  m_paragraph = new ParagraphHandler(this);
  m_paragraph->startParagraph(attrib);
}

//----------------------------------------------------------------------
// ParagraphHandler
//----------------------------------------------------------------------

ParagraphHandler::ParagraphHandler(IBaseHandler *parent) 
  : DocNode(Para), m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  m_markupHandler = new MarkupHandler(m_children,m_curString);
  setFallBackHandler(m_markupHandler);

  addEndHandler("para",this,&ParagraphHandler::endParagraph);

  addStartHandler("itemizedlist",this,&ParagraphHandler::startItemizedList);
  addStartHandler("orderedlist",this,&ParagraphHandler::startOrderedList);
  addStartHandler("parameterlist",this,&ParagraphHandler::startParameterList);
  addStartHandler("simplesect",this,&ParagraphHandler::startSimpleSect);
}

ParagraphHandler::~ParagraphHandler()
{
  delete m_markupHandler;
}

void ParagraphHandler::startParagraph(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  printf("para\n");
}

void ParagraphHandler::endParagraph()
{
  addTextNode();
  printf("end para\n");
  m_parent->setDelegate(0);
}

void ParagraphHandler::startItemizedList(const QXmlAttributes& attrib)
{
  addTextNode();
  ListHandler *listHandler = new ListHandler(ItemizedList,this);
  listHandler->startList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startOrderedList(const QXmlAttributes& attrib)
{
  addTextNode();
  ListHandler *listHandler = new ListHandler(OrderedList,this);
  listHandler->startList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startParameterList(const QXmlAttributes& attrib)
{
  addTextNode();
  ParameterListHandler *parListHandler = new ParameterListHandler(this);
  parListHandler->startParameterList(attrib);
  m_children.append(parListHandler);
}

void ParagraphHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  addTextNode();
  SimpleSectHandler *sectHandler = new SimpleSectHandler(this);
  sectHandler->startSimpleSect(attrib);
  m_children.append(sectHandler);
}

void ParagraphHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_markupHandler->markup()));
    printf("addTextNode() text=%s markup=%x\n",
        m_curString.data(),m_markupHandler->markup());
    m_curString="";
  }
}

//----------------------------------------------------------------------
// DocHandler
//----------------------------------------------------------------------

DocHandler::DocHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  addEndHandler("briefdescription",this,&DocHandler::endDoc);
  addEndHandler("detaileddescription",this,&DocHandler::endDoc);

  addStartHandler("para",this,&DocHandler::startParagraph);
}

DocHandler::~DocHandler()
{
}

void DocHandler::startDoc(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  printf("start dochandler\n");
}

void DocHandler::endDoc()
{
  printf("end dochandler\n");
  m_parent->setDelegate(0);
}

void DocHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}


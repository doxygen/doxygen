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


//----------------------------------------------------------------------
// ParameterListHandler
//----------------------------------------------------------------------

ParameterListHandler::ParameterListHandler(IBaseHandler *parent) 
  : DocNode(ParameterList), m_parent(parent)
{
  addEndHandler("parameterlist",this,&ParameterListHandler::endParameterList);
  m_parameters.setAutoDelete(TRUE);
  m_curParam=0;
}

ParameterListHandler::~ParameterListHandler()
{
}

void ParameterListHandler::startParameterList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterListHandler::endParameterList()
{
  m_parent->setDelegate(0);
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
}

ParagraphHandler::~ParagraphHandler()
{
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


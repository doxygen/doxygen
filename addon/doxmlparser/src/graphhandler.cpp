#include "graphhandler.h"

GraphHandler::GraphHandler(IBaseHandler *parent,const char *endTag)
   : m_parent(parent)
{
  addEndHandler(endTag,this,&GraphHandler::endGraph);
  addStartHandler("node",this,&GraphHandler::startNode);
  m_nodes.setAutoDelete(TRUE);
}

GraphHandler::~GraphHandler()
{
}

void GraphHandler::startGraph(const QXmlAttributes &)
{
  m_parent->setDelegate(this);
}

void GraphHandler::endGraph()
{
  m_parent->setDelegate(0);
}

void GraphHandler::startNode(const QXmlAttributes &attrib)
{
  NodeHandler *n = new NodeHandler(this);
  n->startNode(attrib);
  m_nodes.append(n);
}

INodeIterator *GraphHandler::nodes() const
{
  return new NodeIterator(*this);
}

//------------------------------------------------------------------------

NodeHandler::NodeHandler(IBaseHandler *parent)
   : m_parent(parent)
{
  addEndHandler("node",this,&NodeHandler::endNode);
  addStartHandler("link",this,&NodeHandler::startLink);
  addEndHandler("link",this,&NodeHandler::endLink);
  addStartHandler("label",this,&NodeHandler::startLabel);
  addEndHandler("label",this,&NodeHandler::endLabel);
}

NodeHandler::~NodeHandler()
{
}

void NodeHandler::startNode(const QXmlAttributes &attrib)
{
  m_parent->setDelegate(this);
  m_id = attrib.value("id");
}

void NodeHandler::endNode()
{
  m_parent->setDelegate(0);
}

void NodeHandler::startLink(const QXmlAttributes &attrib)
{
  m_link = attrib.value("id");
}

void NodeHandler::endLink()
{
}

void NodeHandler::startLabel(const QXmlAttributes &/*attrib*/)
{
  m_curString="";
}

void NodeHandler::endLabel()
{
  m_label = m_curString;
}






#include "graphhandler.h"

class EdgeRelationMapper
{
  public:
    EdgeRelationMapper()
    {
      m_map.insert("public-inheritance",    IChildNode::PublicInheritance);
      m_map.insert("protected-inheritance", IChildNode::ProtectedInheritance);
      m_map.insert("private-inheritance",   IChildNode::PrivateInheritance);
      m_map.insert("usage",                 IChildNode::Usage);
      m_map.insert("template-instance",     IChildNode::TemplateInstance);
    }
    IChildNode::NodeRelation stringToNodeRelation(const QString &nrStr)
    {
      return m_map[nrStr];
    }
  private:
    QMap<QString,IChildNode::NodeRelation> m_map;
};

static EdgeRelationMapper *s_edgeRelationMapper;

void graphhandler_init()
{
  s_edgeRelationMapper = new EdgeRelationMapper;
}

void graphhandler_exit()
{
  delete s_edgeRelationMapper;
}

//------------------------------------------------------------------------

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
  addStartHandler("childnode",this,&NodeHandler::startChildNode);
  m_children.setAutoDelete(TRUE);
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

void NodeHandler::startChildNode(const QXmlAttributes &attrib)
{
  ChildNodeHandler *cnh = new ChildNodeHandler(this);
  cnh->startChildNode(attrib);
  m_children.append(cnh);
}
  
IChildNodeIterator *NodeHandler::children() const 
{ 
  return new ChildNodeIterator(*this); 
}

//------------------------------------------------------------------------

ChildNodeHandler::ChildNodeHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addStartHandler("edgelabel",this,&ChildNodeHandler::startEdgeLabel);
  m_edgeLabels.setAutoDelete(TRUE);
}

ChildNodeHandler::~ChildNodeHandler()
{
}

void ChildNodeHandler::startChildNode(const QXmlAttributes &attrib)
{
  m_id             = attrib.value("id");
  m_relationString = attrib.value("relation");
  m_relation       = s_edgeRelationMapper->stringToNodeRelation(m_relationString);
  m_parent->setDelegate(this);
}

void ChildNodeHandler::endChildNode()
{
  m_parent->setDelegate(0);
}


void ChildNodeHandler::startEdgeLabel(const QXmlAttributes &attrib)
{
  EdgeLabelHandler *elh = new EdgeLabelHandler(this);
  elh->startEdgeLabel(attrib);
  m_edgeLabels.append(elh);
}

IEdgeLabelIterator *ChildNodeHandler::edgeLabels() const
{
  return new EdgeLabelIterator(*this);
}


//-----------------------------------------------------------------------

EdgeLabelHandler::EdgeLabelHandler(IBaseHandler *parent)
  : m_parent(parent)
{
}

EdgeLabelHandler::~EdgeLabelHandler()
{
}

void EdgeLabelHandler::startEdgeLabel(const QXmlAttributes &)
{
  m_parent->setDelegate(this);
  m_curString="";
}

void EdgeLabelHandler::endEdgeLabel()
{
  m_label=m_curString;
  m_parent->setDelegate(0);
}






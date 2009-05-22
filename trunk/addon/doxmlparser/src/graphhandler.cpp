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
  m_nodeDict = new QDict<NodeHandler>(1009);
}

GraphHandler::~GraphHandler()
{
  delete m_nodeDict;
}

void GraphHandler::startGraph(const QXmlAttributes &)
{
  debug(2,"startGraph\n");
  m_parent->setDelegate(this);
}

void GraphHandler::endGraph()
{
  debug(2,"endGraph\n");
  m_parent->setDelegate(0);
}

void GraphHandler::startNode(const QXmlAttributes &attrib)
{
  NodeHandler *n = new NodeHandler(this);
  n->startNode(attrib);
  m_nodes.append(n);
  m_nodeDict->insert(attrib.value("id"),n);
}

INodeIterator *GraphHandler::nodes() const
{
  return new NodeIterator(*this);
}

NodeHandler *GraphHandler::getNodeById(const QString &id) const
{
  return m_nodeDict->find(id);
}

//------------------------------------------------------------------------

NodeHandler::NodeHandler(GraphHandler *gh)
   : m_parent(gh), m_graph(gh)
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
  debug(2,"startNode\n");
  m_parent->setDelegate(this);
  m_id = attrib.value("id");
}

void NodeHandler::endNode()
{
  debug(2,"endNode\n");
  m_parent->setDelegate(0);
}

void NodeHandler::startLink(const QXmlAttributes &attrib)
{
  m_link = attrib.value("refid");
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
  ChildNodeHandler *cnh = new ChildNodeHandler(this,m_graph);
  cnh->startChildNode(attrib);
  m_children.append(cnh);
}
  
IChildNodeIterator *NodeHandler::children() const 
{ 
  return new ChildNodeIterator(*this); 
}

//------------------------------------------------------------------------

ChildNodeHandler::ChildNodeHandler(IBaseHandler *parent,GraphHandler *gh)
  : m_parent(parent), m_graph(gh)
{
  addEndHandler("childnode",this,&ChildNodeHandler::endChildNode);
  addStartHandler("edgelabel",this,&ChildNodeHandler::startEdgeLabel);
  m_edgeLabels.setAutoDelete(TRUE);
}

ChildNodeHandler::~ChildNodeHandler()
{
}

void ChildNodeHandler::startChildNode(const QXmlAttributes &attrib)
{
  debug(2,"startChildNode\n");
  m_id             = attrib.value("refid");
  m_relationString = attrib.value("relation");
  m_relation       = s_edgeRelationMapper->stringToNodeRelation(m_relationString);
  m_parent->setDelegate(this);
}

void ChildNodeHandler::endChildNode()
{
  debug(2,"endChildNode\n");
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

INode *ChildNodeHandler::node() const
{
  return m_graph->getNodeById(m_id);
}

//-----------------------------------------------------------------------

EdgeLabelHandler::EdgeLabelHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("edgelabel",this,&EdgeLabelHandler::endEdgeLabel);
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






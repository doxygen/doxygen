/******************************************************************************
 *
 * $Id:$
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _GRAPHHANDLER_H
#define _GRAPHHANDLER_H

#include "stringimpl.h"
#include "doxmlintf.h"
#include "basehandler.h"
#include "baseiterator.h"

class NodeHandler;
class ChildNodeHandler;
class EdgeLabelHandler;

class GraphHandler : public IGraph, public BaseHandler<GraphHandler>
{
    friend class NodeIterator;
  public:
    GraphHandler(IBaseHandler *parent,const char *endTag);
    virtual ~GraphHandler();

    void startGraph(const QXmlAttributes &attrib);
    void endGraph();
    void startNode(const QXmlAttributes &attrib);
    NodeHandler *getNodeById(const QString &id) const;

    // IGraph
    virtual INodeIterator *nodes() const;

  private:
    IBaseHandler *m_parent;
    QList<NodeHandler> m_nodes;
    QDict<NodeHandler> *m_nodeDict;
};

//----------------------------------------------------------------------

class NodeHandler : public INode, public BaseHandler<NodeHandler>
{
    friend class ChildNodeIterator;
  public:
    NodeHandler(GraphHandler *gh);
    virtual ~NodeHandler();

    void startNode(const QXmlAttributes &attrib);
    void endNode();
    void startLabel(const QXmlAttributes &attrib);
    void endLabel();
    void startLink(const QXmlAttributes &attrib);
    void endLink();
    void startChildNode(const QXmlAttributes &attrib);

    // INode
    virtual const IString *id() const { return &m_id; }
    virtual const IString *label() const { return &m_label; }
    virtual const IString *linkId() const { return &m_link; }
    virtual IChildNodeIterator *children() const;

  private:
    IBaseHandler *m_parent;
    StringImpl m_id;
    StringImpl m_label;
    StringImpl m_link;
    QList<ChildNodeHandler> m_children;
    GraphHandler *m_graph;
};

class NodeIterator : public BaseIterator<INodeIterator,INode,NodeHandler>
{
  public:
    NodeIterator(const GraphHandler &handler) :
      BaseIterator<INodeIterator,INode,NodeHandler>(handler.m_nodes) {}
};

//----------------------------------------------------------------------

class ChildNodeHandler : public IChildNode, public BaseHandler<ChildNodeHandler>
{
    friend class EdgeLabelIterator;
  public:
    ChildNodeHandler(IBaseHandler *parent,GraphHandler *gh);
    virtual ~ChildNodeHandler();

    void startChildNode(const QXmlAttributes &attrib);
    void endChildNode();
    void startEdgeLabel(const QXmlAttributes &attrib);

    // IChildNode
    virtual INode *node() const;
    virtual NodeRelation relation() const { return m_relation; }
    virtual const IString * relationString() const { return &m_relationString; }
    virtual IEdgeLabelIterator *edgeLabels() const;

  private:
    IBaseHandler           *m_parent;
    QString                 m_id;
    NodeRelation            m_relation;
    StringImpl              m_relationString;
    QList<EdgeLabelHandler> m_edgeLabels;
    GraphHandler           *m_graph;
};

class ChildNodeIterator : public BaseIterator<IChildNodeIterator,IChildNode,ChildNodeHandler>
{
  public:
    ChildNodeIterator(const NodeHandler &handler) :
      BaseIterator<IChildNodeIterator,IChildNode,ChildNodeHandler>(handler.m_children) {}
};

//----------------------------------------------------------------------

class EdgeLabelHandler : public IEdgeLabel, public BaseHandler<EdgeLabelHandler>
{
    friend class EdgeLabelIterator;
  public:
    EdgeLabelHandler(IBaseHandler *parent);
    virtual ~EdgeLabelHandler();

    void startEdgeLabel(const QXmlAttributes &attrib);
    void endEdgeLabel();

    // IEdgeLabel
    virtual const IString *label() const { return &m_label; }

  private:
    IBaseHandler *m_parent;
    StringImpl    m_label;
};

class EdgeLabelIterator : public BaseIterator<IEdgeLabelIterator,IEdgeLabel,EdgeLabelHandler>
{
  public:
    EdgeLabelIterator(const ChildNodeHandler &handler) :
      BaseIterator<IEdgeLabelIterator,IEdgeLabel,EdgeLabelHandler>(handler.m_edgeLabels) {}
};

void graphhandler_init();
void graphhandler_exit();

#endif


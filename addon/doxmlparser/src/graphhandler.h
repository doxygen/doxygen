/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include "doxmlintf.h"
#include "basehandler.h"
#include "baseiterator.h"

class NodeHandler;
class ChildNodeHandler;

class GraphHandler : public IGraph, public BaseHandler<GraphHandler>
{
    friend class NodeIterator;
  public:
    GraphHandler(IBaseHandler *parent,const char *endTag);
    virtual ~GraphHandler();

    void startGraph(const QXmlAttributes &attrib);
    void endGraph();
    void startNode(const QXmlAttributes &attrib);

    // IGraph
    virtual INodeIterator *nodes() const;

  private:
    IBaseHandler *m_parent;
    QList<NodeHandler> m_nodes;
};

//----------------------------------------------------------------------

class NodeHandler : public INode, public BaseHandler<NodeHandler>
{
    friend class ChildNodeIterator;
  public:
    NodeHandler(IBaseHandler *parent);
    virtual ~NodeHandler();

    void startNode(const QXmlAttributes &attrib);
    void endNode();
    void startLabel(const QXmlAttributes &attrib);
    void endLabel();
    void startLink(const QXmlAttributes &attrib);
    void endLink();

    // INode
    virtual QString id() const { return m_id; }
    virtual QString label() const { return m_label; }
    virtual QString linkId() const { return m_link; }
    virtual IChildNodeIterator *children() const { return 0; } // TODO: implement

  private:
    IBaseHandler *m_parent;
    QString m_id;
    QString m_label;
    QString m_link;
    QList<ChildNodeHandler> m_children;
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
  public:
    ChildNodeHandler(IBaseHandler *parent);
    virtual ~ChildNodeHandler();

    void startChildNode(const QXmlAttributes &attrib);
    void endChildNode();

    // IChildNode
    virtual QString id() const { return m_id; }

  private:
    IBaseHandler *m_parent;
    QString m_id;
};

class ChildNodeIterator : public BaseIterator<IChildNodeIterator,IChildNode,ChildNodeHandler>
{
  public:
    ChildNodeIterator(const NodeHandler &handler) :
      BaseIterator<IChildNodeIterator,IChildNode,ChildNodeHandler>(handler.m_children) {}
};


#endif


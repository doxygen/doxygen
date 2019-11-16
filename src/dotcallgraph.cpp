/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation under the terms of the GNU General Public License is hereby
* granted. No representations are made about the suitability of this software
* for any purpose. It is provided "as is" without express or implied warranty.
* See the GNU General Public License for more details.
*
* Documents produced by Doxygen are derivative works derived from the
* input used in their production; they are not affected by this license.
*
*/

#include "dotcallgraph.h"

#include "dotnode.h"
#include "memberlist.h"
#include "config.h"
#include "util.h"

#define HIDE_SCOPE_NAMES      Config_getBool(HIDE_SCOPE_NAMES)

void DotCallGraph::buildGraph(DotNode *n,const MemberDef *md,int distance)
{
  MemberSDict *refs = m_inverse ? md->getReferencedByMembers() : md->getReferencesMembers();
  if (refs)
  {
    refs->sort();
    MemberSDict::Iterator mri(*refs);
    MemberDef *rmd;
    for (;(rmd=mri.current());++mri)
    {
      if (rmd->showInCallGraph())
      {
        QCString uniqueId;
        uniqueId=rmd->getReference()+"$"+
          rmd->getOutputFileBase()+"#"+rmd->anchor();
        DotNode *bn  = m_usedNodes->find(uniqueId);
        if (bn) // file is already a node in the graph
        {
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          bn->setDistance(distance);
        }
        else
        {
          QCString name;
          if (HIDE_SCOPE_NAMES)
          {
            name  = rmd->getOuterScope()==m_scope ? 
              rmd->name() : rmd->qualifiedName();
          }
          else
          {
            name = rmd->qualifiedName();
          }
          QCString tooltip = rmd->briefDescriptionAsTooltip();
          bn = new DotNode(
            getNextNodeNumber(),
            linkToText(rmd->getLanguage(),name,FALSE),
            tooltip,
            uniqueId,
            0 //distance
          );
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          bn->setDistance(distance);
          m_usedNodes->insert(uniqueId,bn);

          buildGraph(bn,rmd,distance+1);
        }
      }
    }
  }
}

void DotCallGraph::determineVisibleNodes(QList<DotNode> &queue, int &maxNodes)
{
  while (queue.count()>0 && maxNodes>0)
  {
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<=m_maxDepth) // not yet processed
    {
      n->markAsVisible();
      maxNodes--;
      // add direct children
      if (n->children())
      {
        QListIterator<DotNode> li(*n->children());
        DotNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          queue.append(dn);
        }
      }
    }
  }
}

void DotCallGraph::determineTruncatedNodes(QList<DotNode> &queue)
{
  while (queue.count()>0)
  {
    DotNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->children())
      {
        QListIterator<DotNode> li(*n->children());
        const DotNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          if (!dn->isVisible()) 
            truncated = TRUE;
          else 
            queue.append(dn);
        }
      }
      n->markAsTruncated(truncated);
    }
  }
}

DotCallGraph::DotCallGraph(const MemberDef *md,bool inverse)
{
  static int max_dot_graph_depth = Config_getBool(MAX_DOT_GRAPH_DEPTH);
  static int dot_graph_max_nodes = Config_getBool(DOT_GRAPH_MAX_NODES);

  m_inverse = inverse;
  m_diskName = md->getOutputFileBase()+"_"+md->anchor();
  m_scope    = md->getOuterScope();
  m_maxDepth = (m_inverse ? md->maxCallerGraphDepth() : md->maxCallGraphDepth());
  if (m_maxDepth <= 0 ) m_maxDepth = max_dot_graph_depth;
  m_maxNodes = (m_inverse ? md->maxCallerGraphNodes() : md->maxCallGraphNodes());
  if (m_maxNodes <= 0 ) m_maxNodes = dot_graph_max_nodes;

  QCString uniqueId;
  uniqueId = md->getReference()+"$"+
    md->getOutputFileBase()+"#"+md->anchor();
  QCString name;
  if (HIDE_SCOPE_NAMES)
  {
    name = md->name();
  }
  else
  {
    name = md->qualifiedName();
  }
  QCString tooltip = md->briefDescriptionAsTooltip();
  m_startNode = new DotNode(getNextNodeNumber(),
    linkToText(md->getLanguage(),name,FALSE),
    tooltip,
    uniqueId.data(),
    TRUE     // root node
  );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(uniqueId,m_startNode);
  buildGraph(m_startNode,md,1);

  int maxNodes = m_maxNodes;
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineVisibleNodes(openNodeQueue,maxNodes);
  openNodeQueue.clear();
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue);
}

DotCallGraph::~DotCallGraph()
{
  DotNode::deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotCallGraph::getBaseName() const
{
  return m_diskName + (m_inverse ? "_icgraph" : "_cgraph");
}

void DotCallGraph::computeTheGraph()
{
  computeGraph(
    m_startNode,
    CallGraph,
    m_graphFormat,
    m_inverse ? "RL" : "LR",
    FALSE,
    m_inverse,
    m_startNode->label(),
    m_theGraph);
}

QCString DotCallGraph::getMapLabel() const
{
  return m_baseName;
}

QCString DotCallGraph::writeGraph(
        FTextStream &out, 
        GraphOutputFormat graphFormat,
        EmbeddedOutputFormat textFormat,
        const char *path,
        const char *fileName,
        const char *relPath,bool generateImageMap,
        int graphId)
{
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

bool DotCallGraph::isTrivial() const
{
  return m_startNode->children()==0;
}

bool DotCallGraph::isTooBig() const
{
  int numNodes = m_startNode->children() ? m_startNode->children()->count() : 0;
  return numNodes>=m_maxNodes;
}

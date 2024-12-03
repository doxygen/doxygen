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

#include "dotincldepgraph.h"
#include "dotnode.h"
#include "util.h"
#include "config.h"
#include "textstream.h"

void DotInclDepGraph::buildGraph(DotNode *n,const FileDef *fd,int distance)
{
  const IncludeInfoList &includeFiles = m_inverse ? fd->includedByFileList() : fd->includeFileList();
  for (const auto &ii : includeFiles)
  {
    const FileDef *bfd = ii.fileDef;
    QCString in = ii.includeName;
    //printf(">>>> in='%s' bfd=%p\n",qPrint(ii->includeName),bfd);
    bool doc=TRUE,src=FALSE;
    if (bfd)
    {
      in  = bfd->absFilePath();
      doc = bfd->isLinkable() && !bfd->isHidden();
      src = bfd->generateSourceFile();
    }
    if (doc || src || !Config_getBool(HIDE_UNDOC_RELATIONS))
    {
      QCString url="";
      if (bfd) url=bfd->getOutputFileBase();
      if (!doc && src)
      {
        url=bfd->getSourceFileBase();
      }
      auto it = m_usedNodes.find(in.str());
      if (it!=m_usedNodes.end()) // file is already a node in the graph
      {
        DotNode *bn = it->second;
        n->addChild(bn,EdgeInfo::Blue,EdgeInfo::Solid);
        bn->addParent(n);
        bn->setDistance(distance);
      }
      else
      {
        QCString tmp_url;
        QCString tooltip;
        if (bfd)
        {
          tmp_url=doc || src ? bfd->getReference()+"$"+url : QCString();
          tooltip = bfd->briefDescriptionAsTooltip();
        }
        DotNode *bn = new DotNode(this,
                         ii.includeName,   // label
                         tooltip,           // tip
                         tmp_url,           // url
                         FALSE,             // rootNode
                         nullptr);                // cd
        n->addChild(bn,EdgeInfo::Blue,EdgeInfo::Solid);
        bn->addParent(n);
        m_usedNodes.emplace(in.str(),bn);
        bn->setDistance(distance);

        if (bfd) buildGraph(bn,bfd,distance+1);
      }
    }
  }
}

void DotInclDepGraph::determineVisibleNodes(DotNodeDeque &queue, int &maxNodes)
{
  while (!queue.empty() && maxNodes>0)
  {
    DotNode *n = queue.front();
    queue.pop_front();
    if (!n->isVisible() && n->distance()<=Config_getInt(MAX_DOT_GRAPH_DEPTH)) // not yet processed
    {
      n->markAsVisible();
      maxNodes--;
      // add direct children
      for (const auto &dn : n->children())
      {
        queue.push_back(dn);
      }
    }
  }
}

void DotInclDepGraph::determineTruncatedNodes(DotNodeDeque &queue)
{
  while (!queue.empty())
  {
    DotNode *n = queue.front();
    queue.pop_front();
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      for (const auto &dn : n->children())
      {
        if (!dn->isVisible())
        {
          truncated = TRUE;
        }
        else
        {
          queue.push_back(dn);
        }
      }
      n->markAsTruncated(truncated);
    }
  }
}

DotInclDepGraph::DotInclDepGraph(const FileDef *fd,bool inverse)
{
  m_inverse = inverse;
  ASSERT(fd!=nullptr);
  m_inclDepFileName   = fd->includeDependencyGraphFileName();
  m_inclByDepFileName = fd->includedByDependencyGraphFileName();
  QCString tmp_url=fd->getReference()+"$"+fd->getOutputFileBase();
  QCString tooltip = fd->briefDescriptionAsTooltip();
  m_startNode = new DotNode(this,
                            fd->docName(),
                            tooltip,
                            tmp_url,
                            TRUE);    // root node
  m_startNode->setDistance(0);
  m_usedNodes.emplace(fd->absFilePath().str(),m_startNode);
  buildGraph(m_startNode,fd,1);

  int maxNodes = Config_getInt(DOT_GRAPH_MAX_NODES);
  DotNodeDeque openNodeQueue;
  openNodeQueue.push_back(m_startNode);
  determineVisibleNodes(openNodeQueue,maxNodes);
  openNodeQueue.clear();
  openNodeQueue.push_back(m_startNode);
  determineTruncatedNodes(openNodeQueue);
}

DotInclDepGraph::~DotInclDepGraph()
{
  DotNode::deleteNodes(m_startNode);
}

QCString DotInclDepGraph::getBaseName() const
{
  if (m_inverse)
  {
    return m_inclByDepFileName;
  }
  else
  {
    return m_inclDepFileName;
  }
}

void DotInclDepGraph::computeTheGraph()
{
  computeGraph(m_startNode, GraphType::Dependency, m_graphFormat, "", FALSE,
               m_inverse, m_startNode->label(), m_theGraph);
}

QCString DotInclDepGraph::getMapLabel() const
{
  if (m_inverse)
  {
    return escapeCharsInString(m_startNode->label(),FALSE) + "dep";
  }
  else
  {
    return escapeCharsInString(m_startNode->label(),FALSE);
  }
}

QCString DotInclDepGraph::writeGraph(TextStream &out,
                                     GraphOutputFormat graphFormat,
                                     EmbeddedOutputFormat textFormat,
                                     const QCString &path,
                                     const QCString &fileName,
                                     const QCString &relPath,
                                     bool generateImageMap,
                                     int graphId)
{
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->children().empty();
}

bool DotInclDepGraph::isTooBig() const
{
  return numNodes()>=Config_getInt(DOT_GRAPH_MAX_NODES);
}

int DotInclDepGraph::numNodes() const
{
  return static_cast<int>(m_startNode->children().size());
}

void DotInclDepGraph::writeXML(TextStream &t)
{
  for (const auto &[name,node] : m_usedNodes)
  {
    node->writeXML(t,FALSE);
  }
}

void DotInclDepGraph::writeDocbook(TextStream &t)
{
  for (const auto &[name,node] : m_usedNodes)
  {
    node->writeDocbook(t,FALSE);
  }
}

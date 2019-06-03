
#include "dotincldepgraph.h"
#include "dotnode.h"

#include "util.h"
#include "config.h"

#define MAX_DOT_GRAPH_DEPTH   Config_getInt(MAX_DOT_GRAPH_DEPTH)
#define DOT_GRAPH_MAX_NODES   Config_getInt(DOT_GRAPH_MAX_NODES)

void DotInclDepGraph::buildGraph(DotNode *n,const FileDef *fd,int distance)
{
  QList<IncludeInfo> *includeFiles = 
    m_inverse ? fd->includedByFileList() : fd->includeFileList();
  if (includeFiles)
  {
    QListIterator<IncludeInfo> ili(*includeFiles);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      const FileDef *bfd = ii->fileDef;
      QCString in  = ii->includeName;
      //printf(">>>> in=`%s' bfd=%p\n",ii->includeName.data(),bfd);
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
        if (bfd) url=bfd->getOutputFileBase().copy();
        if (!doc && src)
        {
          url=bfd->getSourceFileBase();
        }
        DotNode *bn  = m_usedNodes->find(in);
        if (bn) // file is already a node in the graph
        {
          n->addChild(bn,0,0,0);
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
          bn = new DotNode(
            getNextNodeNumber(),// n
            ii->includeName,   // label
            tooltip,           // tip
            tmp_url,           // url
            FALSE,             // rootNode
            0                  // cd
          );
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          m_usedNodes->insert(in,bn);
          bn->setDistance(distance);

          if (bfd) buildGraph(bn,bfd,distance+1);
        }
      }
    }
  }
}

void DotInclDepGraph::determineVisibleNodes(QList<DotNode> &queue, int &maxNodes)
{
  while (queue.count()>0 && maxNodes>0)
  {
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<=MAX_DOT_GRAPH_DEPTH) // not yet processed
    {
      n->markAsVisible();
      maxNodes--;
      // add direct children
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          queue.append(dn);
        }
      }
    }
  }
}

void DotInclDepGraph::determineTruncatedNodes(QList<DotNode> &queue)
{
  while (queue.count()>0)
  {
    DotNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
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

DotInclDepGraph::DotInclDepGraph(const FileDef *fd,bool inverse)
{
  m_inverse = inverse;
  ASSERT(fd!=0);
  m_inclDepFileName   = fd->includeDependencyGraphFileName();
  m_inclByDepFileName = fd->includedByDependencyGraphFileName();
  QCString tmp_url=fd->getReference()+"$"+fd->getOutputFileBase();
  QCString tooltip = fd->briefDescriptionAsTooltip();
  m_startNode = new DotNode(getNextNodeNumber(),
    fd->docName(),
    tooltip,
    tmp_url.data(),
    TRUE     // root node
  );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(fd->absFilePath(),m_startNode);
  buildGraph(m_startNode,fd,1);

  int maxNodes = DOT_GRAPH_MAX_NODES;
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineVisibleNodes(openNodeQueue,maxNodes);
  openNodeQueue.clear();
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue);
}

DotInclDepGraph::~DotInclDepGraph()
{
  DotNode::deleteNodes(m_startNode);
  delete m_usedNodes;
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
  computeGraph(
    m_startNode,
    Dependency,
    m_graphFormat,
    "",
    FALSE,
    m_inverse,
    m_startNode->label(),
    m_theGraph
  );
}

QCString DotInclDepGraph::getMapLabel() const
{
  if (m_inverse)
  {
    return escapeCharsInString(m_startNode->m_label,FALSE) + "dep";
  }
  else
  {
    return escapeCharsInString(m_startNode->m_label,FALSE);
  }
}

QCString DotInclDepGraph::writeGraph(FTextStream &out,
  GraphOutputFormat graphFormat,
  EmbeddedOutputFormat textFormat,
  const char *path,
  const char *fileName,
  const char *relPath,
  bool generateImageMap,
  int graphId
)
{
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

bool DotInclDepGraph::isTooBig() const
{
  int numNodes = m_startNode->m_children ? m_startNode->m_children->count() : 0;
  return numNodes>=DOT_GRAPH_MAX_NODES;
}

void DotInclDepGraph::writeXML(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,FALSE);
  }
}

void DotInclDepGraph::writeDocbook(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDocbook(t,FALSE);
  }
}

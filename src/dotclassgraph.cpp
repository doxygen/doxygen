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

#include <algorithm>

#include "containers.h"
#include "dotclassgraph.h"
#include "dotnode.h"
#include "textstream.h"

#include "config.h"
#include "util.h"

void DotClassGraph::addClass(const ClassDef *cd,DotNode *n,EdgeInfo::Colors color,
  const QCString &label,const QCString &usedName,const QCString &templSpec,bool base,int distance)
{
  if (Config_getBool(HIDE_UNDOC_CLASSES) && !cd->isLinkable()) return;

  EdgeInfo::Styles edgeStyle = (!label.isEmpty() || color==EdgeInfo::Orange || color==EdgeInfo::Orange2) ? EdgeInfo::Dashed : EdgeInfo::Solid;
  QCString className;
  QCString fullName;
  if (cd->isAnonymous())
  {
    className="anonymous:";
    className+=label;
    fullName = className;
  }
  else if (!usedName.isEmpty()) // name is a typedef
  {
    className=usedName;
    fullName = className;
  }
  else if (!templSpec.isEmpty()) // name has a template part
  {
    className=insertTemplateSpecifierInScope(cd->displayName(),templSpec);
    fullName =insertTemplateSpecifierInScope(cd->name(),templSpec);
  }
  else // just a normal name
  {
    className=cd->displayName();
    fullName = cd->name();
  }
  //printf("DotClassGraph::addClass(class='%s',parent=%s,prot=%d,label=%s,dist=%d,usedName=%s,templSpec=%s,base=%d)\n",
  //                                 qPrint(className),qPrint(n->label()),prot,label,distance,usedName,templSpec,base);
  auto it = m_usedNodes.find(fullName.str());
  if (it!=m_usedNodes.end()) // class already inserted
  {
    DotNode *bn = it->second;
    if (base)
    {
      n->addChild(bn,color,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,color,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    //printf(" add exiting node %s of %s\n",qPrint(bn->label()),qPrint(n->label()));
  }
  else // new class
  {
    QCString displayName=className;
    if (Config_getBool(HIDE_SCOPE_NAMES)) displayName=stripScope(displayName);
    QCString tmp_url;
    if (cd->isLinkable() && !cd->isHidden())
    {
      tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
      if (!cd->anchor().isEmpty())
      {
        tmp_url+="#"+cd->anchor();
      }
    }
    QCString tooltip = cd->briefDescriptionAsTooltip();
    DotNode *bn = new DotNode(this,
      displayName,
      tooltip,
      tmp_url,
      FALSE,        // rootNode
      cd
    );
    if (base)
    {
      n->addChild(bn,color,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,color,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    m_usedNodes.emplace(fullName.str(),bn);
    //printf(" add new child node '%s' to %s hidden=%d url=%s\n",
    //    qPrint(className),qPrint(n->label()),cd->isHidden(),qPrint(tmp_url));

    buildGraph(cd,bn,base,distance+1);
  }
}

void DotClassGraph::determineTruncatedNodes(DotNodeDeque &queue,bool includeParents)
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
          truncated = TRUE;
        else
          queue.push_back(dn);
      }
      if (includeParents)
      {
        for (const auto &dn : n->parents())
        {
          if (!dn->isVisible())
            truncated = TRUE;
          else
            queue.push_back(dn);
        }
      }
      n->markAsTruncated(truncated);
    }
  }
}

bool DotClassGraph::determineVisibleNodes(DotNode *rootNode,
  int maxNodes,bool includeParents)
{
  DotNodeDeque childQueue;
  DotNodeDeque parentQueue;
  std::vector<size_t> childTreeWidth;
  std::vector<size_t> parentTreeWidth;
  childQueue.push_back(rootNode);
  if (includeParents) parentQueue.push_back(rootNode);
  bool firstNode=TRUE; // flag to force reprocessing rootNode in the parent loop
                       // despite being marked visible in the child loop
  while ((!childQueue.empty() || !parentQueue.empty()) && maxNodes>0)
  {
    if (!childQueue.empty())
    {
      DotNode *n = childQueue.front();
      childQueue.pop_front();
      size_t distance = n->distance();
      if (!n->isVisible() && distance<=static_cast<size_t>(Config_getInt(MAX_DOT_GRAPH_DEPTH))) // not yet processed
      {
        if (distance>0)
        {
          size_t oldSize=childTreeWidth.size();
          if (distance>oldSize)
          {
            childTreeWidth.resize(std::max(childTreeWidth.size(),distance));
            for (size_t i=oldSize;i<distance;i++) childTreeWidth[i]=0;
          }
          childTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct children
        for (const auto &dn : n->children())
        {
          childQueue.push_back(dn);
        }
      }
    }
    if (includeParents && !parentQueue.empty())
    {
      DotNode *n = parentQueue.front();
      parentQueue.pop_front();
      if ((!n->isVisible() || firstNode) && n->distance()<=Config_getInt(MAX_DOT_GRAPH_DEPTH)) // not yet processed
      {
        firstNode=FALSE;
        size_t distance = n->distance();
        if (distance>0)
        {
          size_t oldSize = parentTreeWidth.size();
          if (distance>oldSize)
          {
            parentTreeWidth.resize(std::max(parentTreeWidth.size(),distance));
            for (size_t i=oldSize;i<distance;i++) parentTreeWidth[i]=0;
          }
          parentTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct parents
        for (const auto &dn : n->parents())
        {
          parentQueue.push_back(dn);
        }
      }
    }
  }
  if (Config_getBool(UML_LOOK)) return FALSE; // UML graph are always top to bottom
  size_t maxWidth=0;
  size_t maxHeight=std::max(childTreeWidth.size(),parentTreeWidth.size());
  for (size_t i=0;i<childTreeWidth.size();i++)
  {
    if (childTreeWidth.at(i)>maxWidth) maxWidth=childTreeWidth.at(i);
  }
  for (size_t i=0;i<parentTreeWidth.size();i++)
  {
    if (parentTreeWidth.at(i)>maxWidth) maxWidth=parentTreeWidth.at(i);
  }
  //printf("max tree width=%d, max tree height=%d\n",maxWidth,maxHeight);
  return maxWidth>80 && maxHeight<12; // used metric to decide to render the tree
                                      // from left to right instead of top to bottom,
                                      // with the idea to render very wide trees in
                                      // left to right order.
}

static QCString joinLabels(const StringSet &ss)
{
  QCString label;
  int count=1;
  int maxLabels=10;
  auto it = std::begin(ss), e = std::end(ss);
  if (it!=e) // set not empty
  {
    label += (*it++).c_str();
    for (; it!=e && count < maxLabels ; ++it,++count)
    {
      label += '\n';
      label += (*it).c_str();
    }
    if (count==maxLabels) label+="\n...";
  }
  return label;
}

void DotClassGraph::buildGraph(const ClassDef *cd,DotNode *n,bool base,int distance)
{
  //printf("DocClassGraph::buildGraph(%s,distance=%d,base=%d)\n",
  //    qPrint(cd->name()),distance,base);
  // ---- Add inheritance relations

  if (m_graphType == GraphType::Inheritance || m_graphType==GraphType::Collaboration)
  {
    for (const auto &bcd : base ? cd->baseClasses() : cd->subClasses())
    {
      //printf("-------- inheritance relation %s->%s templ='%s'\n",
      //            qPrint(cd->name()),qPrint(bcd->classDef->name()),qPrint(bcd->templSpecifiers));
      addClass(bcd.classDef,n,EdgeInfo::protectionToColor(bcd.prot),QCString(),bcd.usedName,bcd.templSpecifiers,base,distance);
    }
  }
  if (m_graphType == GraphType::Collaboration)
  {
    // ---- Add usage relations

    const UsesClassList &list = base ? cd->usedImplementationClasses()   :
                                       cd->usedByImplementationClasses() ;
    for (const auto &ucd : list)
    {
      //printf("addClass: %s templSpec=%s\n",qPrint(ucd.classDef->name()),qPrint(ucd.templSpecifiers));
      addClass(ucd.classDef,n,EdgeInfo::Purple,joinLabels(ucd.accessors),QCString(),
          ucd.templSpecifiers,base,distance);
    }
  }
  if (Config_getBool(TEMPLATE_RELATIONS) && base)
  {
    for (const auto &ccd : cd->templateTypeConstraints())
    {
      //printf("addClass: %s\n",qPrint(ccd.classDef->name()));
      addClass(ccd.classDef,n,EdgeInfo::Orange2,joinLabels(ccd.accessors),QCString(),
        QCString(),TRUE,distance);
    }
  }

  // ---- Add template instantiation relations

  if (Config_getBool(TEMPLATE_RELATIONS))
  {
    if (base) // template relations for base classes
    {
      const ClassDef *templMaster=cd->templateMaster();
      if (templMaster)
      {
        for (const auto &ti : templMaster->getTemplateInstances())
        if (ti.classDef==cd)
        {
          addClass(templMaster,n,EdgeInfo::Orange,ti.templSpec,QCString(),QCString(),TRUE,distance);
        }
      }
    }
    else // template relations for super classes
    {
      for (const auto &ti : cd->getTemplateInstances())
      {
        addClass(ti.classDef,n,EdgeInfo::Orange,ti.templSpec,QCString(),QCString(),FALSE,distance);
      }
    }
  }
}

DotClassGraph::DotClassGraph(const ClassDef *cd,GraphType t)
{
  //printf("--------------- DotClassGraph::DotClassGraph '%s'\n",qPrint(cd->displayName()));
  m_graphType = t;
  QCString tmp_url="";
  if (cd->isLinkable() && !cd->isHidden())
  {
    tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
    if (!cd->anchor().isEmpty())
    {
      tmp_url+="#"+cd->anchor();
    }
  }
  QCString className = cd->displayName();
  QCString tooltip = cd->briefDescriptionAsTooltip();
  m_startNode = new DotNode(this,
    className,
    tooltip,
    tmp_url,
    TRUE,                      // is a root node
    cd
  );
  m_startNode->setDistance(0);
  m_usedNodes.emplace(className.str(),m_startNode);

  buildGraph(cd,m_startNode,TRUE,1);
  if (t==GraphType::Inheritance) buildGraph(cd,m_startNode,FALSE,1);

  m_lrRank = determineVisibleNodes(m_startNode,Config_getInt(DOT_GRAPH_MAX_NODES),t==GraphType::Inheritance);
  DotNodeDeque openNodeQueue;
  openNodeQueue.push_back(m_startNode);
  determineTruncatedNodes(openNodeQueue,t==GraphType::Inheritance);

  m_collabFileName = cd->collaborationGraphFileName();
  m_inheritFileName = cd->inheritanceGraphFileName();
}

bool DotClassGraph::isTrivial() const
{
  if (m_graphType==GraphType::Inheritance)
    return m_startNode->children().empty() && m_startNode->parents().empty();
  else
    return !Config_getBool(UML_LOOK) && m_startNode->children().empty();
}

bool DotClassGraph::isTooBig() const
{
  return numNodes()>=Config_getInt(DOT_GRAPH_MAX_NODES);
}

int DotClassGraph::numNodes() const
{
  size_t numNodes = 0;
  numNodes+= m_startNode->children().size();
  if (m_graphType==GraphType::Inheritance)
  {
    numNodes+= m_startNode->parents().size();
  }
  return static_cast<int>(numNodes);
}

DotClassGraph::~DotClassGraph()
{
  DotNode::deleteNodes(m_startNode);
}

QCString DotClassGraph::getBaseName() const
{
  switch (m_graphType)
  {
  case GraphType::Collaboration:
    return m_collabFileName;
    break;
  case GraphType::Inheritance:
    return m_inheritFileName;
    break;
  default:
    ASSERT(0);
    break;
  }
  return "";
}

void DotClassGraph::computeTheGraph()
{
  computeGraph(
    m_startNode,
    m_graphType,
    m_graphFormat,
    m_lrRank ? "LR" : "",
    m_graphType == GraphType::Inheritance,
    TRUE,
    m_startNode->label(),
    m_theGraph
  );
}

QCString DotClassGraph::getMapLabel() const
{
  QCString mapName;
  switch (m_graphType)
  {
  case GraphType::Collaboration:
    mapName="coll_map";
    break;
  case GraphType::Inheritance:
    mapName="inherit_map";
    break;
  default:
    ASSERT(0);
    break;
  }

  return escapeCharsInString(m_startNode->label(),FALSE)+"_"+escapeCharsInString(mapName,FALSE);
}

QCString DotClassGraph::getImgAltText() const
{
  switch (m_graphType)
  {
  case GraphType::Collaboration:
    return "Collaboration graph";
    break;
  case GraphType::Inheritance:
    return "Inheritance graph";
    break;
  default:
    ASSERT(0);
    break;
  }
  return "";
}

QCString DotClassGraph::writeGraph(TextStream &out,
  GraphOutputFormat graphFormat,
  EmbeddedOutputFormat textFormat,
  const QCString &path,
  const QCString &fileName,
  const QCString &relPath,
  bool /*isTBRank*/,
  bool generateImageMap,
  int graphId)
{
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

//--------------------------------------------------------------------

void DotClassGraph::writeXML(TextStream &t)
{
  for (const auto &[name,node] : m_usedNodes)
  {
    node->writeXML(t,TRUE);
  }
}

void DotClassGraph::writeDocbook(TextStream &t)
{
  for (const auto &[name,node] : m_usedNodes)
  {
    node->writeDocbook(t,TRUE);
  }
}

void DotClassGraph::writeDEF(TextStream &t)
{
  for (const auto &[name,node] : m_usedNodes)
  {
    node->writeDEF(t);
  }
}

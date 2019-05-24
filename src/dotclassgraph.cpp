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

#include "dotclassgraph.h"
#include "dotnode.h"

#include "config.h"
#include "util.h"

#define HIDE_SCOPE_NAMES      Config_getBool(HIDE_SCOPE_NAMES)
#define MAX_DOT_GRAPH_DEPTH   Config_getInt(MAX_DOT_GRAPH_DEPTH)
#define UML_LOOK              Config_getBool(UML_LOOK)
#define TEMPLATE_RELATIONS    Config_getBool(TEMPLATE_RELATIONS)
#define DOT_GRAPH_MAX_NODES   Config_getInt(DOT_GRAPH_MAX_NODES)

void DotClassGraph::addClass(const ClassDef *cd,DotNode *n,int prot,
  const char *label,const char *usedName,const char *templSpec,bool base,int distance)
{
  if (Config_getBool(HIDE_UNDOC_CLASSES) && !cd->isLinkable()) return;

  int edgeStyle = (label || prot==EdgeInfo::Orange || prot==EdgeInfo::Orange2) ? EdgeInfo::Dashed : EdgeInfo::Solid;
  QCString className;
  if (cd->isAnonymous())
  {
    className="anonymous:";
    className+=label;
  }
  else if (usedName) // name is a typedef
  {
    className=usedName;
  }
  else if (templSpec) // name has a template part
  {
    className=insertTemplateSpecifierInScope(cd->name(),templSpec);
  }
  else // just a normal name
  {
    className=cd->displayName();
  }
  //printf("DotClassGraph::addClass(class=`%s',parent=%s,prot=%d,label=%s,dist=%d,usedName=%s,templSpec=%s,base=%d)\n",
  //                                 className.data(),n->label().data(),prot,label,distance,usedName,templSpec,base);
  DotNode *bn = m_usedNodes->find(className);
  if (bn) // class already inserted
  {
    if (base)
    {
      n->addChild(bn,prot,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,prot,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    //printf(" add exiting node %s of %s\n",bn->label().data(),n->label().data());
  }
  else // new class
  {
    QCString displayName=className;
    if (HIDE_SCOPE_NAMES) displayName=stripScope(displayName);
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
    bn = new DotNode(getNextNodeNumber(),
      displayName,
      tooltip,
      tmp_url.data(),
      FALSE,        // rootNode
      cd
    );
    if (base)
    {
      n->addChild(bn,prot,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,prot,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    m_usedNodes->insert(className,bn);
    //printf(" add new child node `%s' to %s hidden=%d url=%s\n",
    //    className.data(),n->label().data(),cd->isHidden(),tmp_url.data());

    buildGraph(cd,bn,base,distance+1);
  }
}

void DotClassGraph::determineTruncatedNodes(QList<DotNode> &queue,bool includeParents)
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
      if (n->parents() && includeParents)
      {
        QListIterator<DotNode> li(*n->parents());
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

bool DotClassGraph::determineVisibleNodes(DotNode *rootNode,
  int maxNodes,bool includeParents)
{
  QList<DotNode> childQueue;
  QList<DotNode> parentQueue;
  QArray<int> childTreeWidth;
  QArray<int> parentTreeWidth;
  childQueue.append(rootNode);
  if (includeParents) parentQueue.append(rootNode);
  bool firstNode=TRUE; // flag to force reprocessing rootNode in the parent loop 
                       // despite being marked visible in the child loop
  while ((childQueue.count()>0 || parentQueue.count()>0) && maxNodes>0)
  {
    if (childQueue.count()>0)
    {
      DotNode *n = childQueue.take(0);
      int distance = n->distance();
      if (!n->isVisible() && distance<=MAX_DOT_GRAPH_DEPTH) // not yet processed
      {
        if (distance>0)
        {
          int oldSize=(int)childTreeWidth.size();
          if (distance>oldSize)
          {
            childTreeWidth.resize(QMAX(childTreeWidth.size(),(uint)distance));
            int i; for (i=oldSize;i<distance;i++) childTreeWidth[i]=0;
          }
          childTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct children
        if (n->children())
        {
          QListIterator<DotNode> li(*n->children());
          const DotNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            childQueue.append(dn);
          }
        }
      }
    }
    if (includeParents && parentQueue.count()>0)
    {
      DotNode *n = parentQueue.take(0);
      if ((!n->isVisible() || firstNode) && n->distance()<=MAX_DOT_GRAPH_DEPTH) // not yet processed
      {
        firstNode=FALSE;
        int distance = n->distance();
        if (distance>0)
        {
          int oldSize = (int)parentTreeWidth.size();
          if (distance>oldSize)
          {
            parentTreeWidth.resize(QMAX(parentTreeWidth.size(),(uint)distance));
            int i; for (i=oldSize;i<distance;i++) parentTreeWidth[i]=0;
          }
          parentTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct parents
        if (n->parents())
        {
          QListIterator<DotNode> li(*n->parents());
          const DotNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            parentQueue.append(dn);
          }
        }
      }
    }
  }
  if (UML_LOOK) return FALSE; // UML graph are always top to bottom
  int maxWidth=0;
  int maxHeight=(int)QMAX(childTreeWidth.size(),parentTreeWidth.size());
  uint i;
  for (i=0;i<childTreeWidth.size();i++)
  {
    if (childTreeWidth.at(i)>maxWidth) maxWidth=childTreeWidth.at(i);
  }
  for (i=0;i<parentTreeWidth.size();i++)
  {
    if (parentTreeWidth.at(i)>maxWidth) maxWidth=parentTreeWidth.at(i);
  }
  //printf("max tree width=%d, max tree height=%d\n",maxWidth,maxHeight);
  return maxWidth>80 && maxHeight<12; // used metric to decide to render the tree
                                      // from left to right instead of top to bottom,
                                      // with the idea to render very wide trees in
                                      // left to right order.
}

void DotClassGraph::buildGraph(const ClassDef *cd,DotNode *n,bool base,int distance)
{
  //printf("DocClassGraph::buildGraph(%s,distance=%d,base=%d)\n",
  //    cd->name().data(),distance,base);
  // ---- Add inheritance relations

  if (m_graphType == Inheritance || m_graphType==Collaboration)
  {
    BaseClassList *bcl = base ? cd->baseClasses() : cd->subClasses();
    if (bcl)
    {
      BaseClassListIterator bcli(*bcl);
      BaseClassDef *bcd;
      for ( ; (bcd=bcli.current()) ; ++bcli )
      {
        //printf("-------- inheritance relation %s->%s templ=`%s'\n",
        //            cd->name().data(),bcd->classDef->name().data(),bcd->templSpecifiers.data());
        addClass(bcd->classDef,n,bcd->prot,0,bcd->usedName,
          bcd->templSpecifiers,base,distance); 
      }
    }
  }
  if (m_graphType == Collaboration)
  {
    // ---- Add usage relations

    UsesClassDict *dict =
      base ? cd->usedImplementationClasses() :
      cd->usedByImplementationClasses()
      ;
    if (dict)
    {
      UsesClassDictIterator ucdi(*dict);
      UsesClassDef *ucd;
      for (;(ucd=ucdi.current());++ucdi)
      {
        QCString label;
        QDictIterator<void> dvi(*ucd->accessors);
        const char *s;
        bool first=TRUE;
        int count=0;
        int maxLabels=10;
        for (;(s=dvi.currentKey()) && count<maxLabels;++dvi,++count)
        {
          if (first) 
          {
            label=s;
            first=FALSE;
          }
          else
          {
            label+=QCString("\n")+s;
          }
        }
        if (count==maxLabels) label+="\n...";
        //printf("addClass: %s templSpec=%s\n",ucd->classDef->name().data(),ucd->templSpecifiers.data());
        addClass(ucd->classDef,n,EdgeInfo::Purple,label,0,
          ucd->templSpecifiers,base,distance);
      }
    }
  }
  if (TEMPLATE_RELATIONS && base)
  {
    ConstraintClassDict *dict = cd->templateTypeConstraints();
    if (dict)
    {
      ConstraintClassDictIterator ccdi(*dict);
      ConstraintClassDef *ccd;
      for (;(ccd=ccdi.current());++ccdi)
      {
        QCString label;
        QDictIterator<void> dvi(*ccd->accessors);
        const char *s;
        bool first=TRUE;
        int count=0;
        int maxLabels=10;
        for (;(s=dvi.currentKey()) && count<maxLabels;++dvi,++count)
        {
          if (first)
          {
            label=s;
            first=FALSE;
          }
          else
          {
            label+=QCString("\n")+s;
          }
        }
        if (count==maxLabels) label+="\n...";
        //printf("addClass: %s templSpec=%s\n",ucd->classDef->name().data(),ucd->templSpecifiers.data());
        addClass(ccd->classDef,n,EdgeInfo::Orange2,label,0,
          0,TRUE,distance);
      }
    }
  }

  // ---- Add template instantiation relations

  if (TEMPLATE_RELATIONS)
  {
    if (base) // template relations for base classes
    {
      const ClassDef *templMaster=cd->templateMaster();
      if (templMaster)
      {
        QDictIterator<ClassDef> cli(*templMaster->getTemplateInstances());
        const ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          if (templInstance==cd)
          {
            addClass(templMaster,n,EdgeInfo::Orange,cli.currentKey(),0,
              0,TRUE,distance);
          }
        }
      }
    }
    else // template relations for super classes
    {
      const QDict<ClassDef> *templInstances = cd->getTemplateInstances();
      if (templInstances)
      {
        QDictIterator<ClassDef> cli(*templInstances);
        const ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          addClass(templInstance,n,EdgeInfo::Orange,cli.currentKey(),0,
            0,FALSE,distance);
        }
      }
    }
  }
}

DotClassGraph::DotClassGraph(const ClassDef *cd,GraphType t)
{
  //printf("--------------- DotClassGraph::DotClassGraph `%s'\n",cd->displayName().data());
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
  m_startNode = new DotNode(getNextNodeNumber(),
    className,
    tooltip,
    tmp_url.data(),
    TRUE,                      // is a root node
    cd
  );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(className,m_startNode);

  buildGraph(cd,m_startNode,TRUE,1);
  if (t==Inheritance) buildGraph(cd,m_startNode,FALSE,1);

  m_lrRank = determineVisibleNodes(m_startNode,DOT_GRAPH_MAX_NODES,t==Inheritance);
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue,t==Inheritance);

  m_collabFileName = cd->collaborationGraphFileName();
  m_inheritFileName = cd->inheritanceGraphFileName();
}

bool DotClassGraph::isTrivial() const
{
  if (m_graphType==Inheritance)
    return m_startNode->children()==0 && m_startNode->parents()==0;
  else
    return !UML_LOOK && m_startNode->children()==0;
}

bool DotClassGraph::isTooBig() const
{
  int numNodes = 0;
  numNodes+= m_startNode->children() ? m_startNode->children()->count() : 0;
  if (m_graphType==Inheritance)
  {
    numNodes+= m_startNode->parents() ? m_startNode->parents()->count() : 0;
  }
  return numNodes>=DOT_GRAPH_MAX_NODES;
}

DotClassGraph::~DotClassGraph()
{
  DotNode::deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotClassGraph::getBaseName() const
{
  switch (m_graphType)
  {
  case Collaboration:
    return m_collabFileName;
    break;
  case Inheritance:
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
    m_graphType == Inheritance,
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
  case Collaboration:
    mapName="coll_map";
    break;
  case Inheritance:
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
  case Collaboration:
    return "Collaboration graph";
    break;
  case Inheritance:
    return "Inheritance graph";
    break;
  default:
    ASSERT(0);
    break;
  }
  return ""; 
}

QCString DotClassGraph::writeGraph(FTextStream &out,
  GraphOutputFormat graphFormat,
  EmbeddedOutputFormat textFormat,
  const char *path,
  const char *fileName,
  const char *relPath,
  bool /*isTBRank*/,
  bool generateImageMap,
  int graphId)
{
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

//--------------------------------------------------------------------

void DotClassGraph::writeXML(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,TRUE);
  }
}

void DotClassGraph::writeDocbook(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDocbook(t,TRUE);
  }
}

void DotClassGraph::writeDEF(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDEF(t);
  }
}

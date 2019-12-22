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

#include "dotgfxhierarchytable.h"

#include "language.h"
#include "util.h"
#include "message.h"
#include "doxygen.h"
#include "classlist.h"

#define OPTIMIZE_OUTPUT_SLICE Config_getBool(OPTIMIZE_OUTPUT_SLICE)

QCString DotGfxHierarchyTable::getBaseName() const
{
  QCString baseName;
  if (m_prefix.isEmpty())
    baseName.sprintf("inherit_graph_%d", m_graphId);
  else
    baseName.sprintf("%sinherit_graph_%d",m_prefix.data(), m_graphId);
  return baseName;
}

void DotGfxHierarchyTable::computeTheGraph()
{
  QListIterator<DotNode> dnli2(*m_rootNodes);
  DotNode *node;

  FTextStream md5stream(&m_theGraph);
  writeGraphHeader(md5stream,theTranslator->trGraphicalHierarchy());
  md5stream << "  rankdir=\"LR\";" << endl;
  for (dnli2.toFirst();(node=dnli2.current());++dnli2)
  {
    if (node->subgraphId()==m_rootSubgraphNode->subgraphId()) 
    {
      node->clearWriteFlag();
    }
  }
  for (dnli2.toFirst();(node=dnli2.current());++dnli2)
  {
    if (node->subgraphId()==m_rootSubgraphNode->subgraphId()) 
    {
      node->write(md5stream,Hierarchy,GOF_BITMAP,FALSE,TRUE,TRUE);
    }
  }
  writeGraphFooter(md5stream);

}

QCString DotGfxHierarchyTable::getMapLabel() const
{
  return escapeCharsInString(m_rootSubgraphNode->label(),FALSE);
}

void DotGfxHierarchyTable::createGraph(DotNode *n,FTextStream &out,
  const char *path,const char *fileName,int id)
{
  m_rootSubgraphNode = n;
  m_graphId = id;
  m_noDivTag = TRUE;
  m_zoomable = FALSE;
  DotGraph::writeGraph(out, GOF_BITMAP, EOF_Html, path, fileName, "", TRUE, 0);
}

void DotGfxHierarchyTable::writeGraph(FTextStream &out,
  const char *path,const char *fileName)
{
  //printf("DotGfxHierarchyTable::writeGraph(%s)\n",name);
  //printf("m_rootNodes=%p count=%d\n",m_rootNodes,m_rootNodes->count());

  if (m_rootSubgraphs->count()==0) return;

  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    term(1,"Output dir %s does not exist!\n",path);
  }

  // put each connected subgraph of the hierarchy in a row of the HTML output
  out << "<table border=\"0\" cellspacing=\"10\" cellpadding=\"0\">" << endl;

  QListIterator<DotNode> dnli(*m_rootSubgraphs);
  DotNode *n;
  int count=0;
  for (dnli.toFirst();(n=dnli.current());++dnli)
  {
    out << "<tr><td>";
    createGraph(n,out,path,fileName,count++);
    out << "</td></tr>" << endl;
  }
  out << "</table>" << endl;
}

void DotGfxHierarchyTable::addHierarchy(DotNode *n,const ClassDef *cd,bool hideSuper)
{
  //printf("addHierarchy '%s' baseClasses=%d\n",cd->name().data(),cd->baseClasses()->count());
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()) ; ++bcli )
    {
      ClassDef *bClass=bcd->classDef; 
      //printf("  Trying sub class='%s' usedNodes=%d\n",bClass->name().data(),m_usedNodes->count());
      if (bClass->isVisibleInHierarchy() && hasVisibleRoot(bClass->baseClasses()))
      {
        DotNode *bn;
        //printf("  Node '%s' Found visible class='%s'\n",n->label().data(),
        //                                              bClass->name().data());
        if ((bn=m_usedNodes->find(bClass->name()))) // node already present 
        {
          if (n->children()==0 || n->children()->findRef(bn)==-1) // no arrow yet
          {
            n->addChild(bn,bcd->prot);
            bn->addParent(n);
            //printf("  Adding node %s to existing base node %s (c=%d,p=%d)\n",
            //       n->label().data(),
            //       bn->label().data(),
            //       bn->children() ? bn->children()->count() : 0,
            //       bn->parents()  ? bn->parents()->count()  : 0
            //     );
          }
          //else
          //{
          //  printf("  Class already has an arrow!\n");
          //}
        }
        else 
        {
          QCString tmp_url="";
          if (bClass->isLinkable() && !bClass->isHidden())
          {
            tmp_url=bClass->getReference()+"$"+bClass->getOutputFileBase();
            if (!bClass->anchor().isEmpty())
            {
              tmp_url+="#"+bClass->anchor();
            }
          }
          QCString tooltip = bClass->briefDescriptionAsTooltip();
          bn = new DotNode(getNextNodeNumber(),
            bClass->displayName(),
            tooltip,
            tmp_url.data()
          );
          n->addChild(bn,bcd->prot);
          bn->addParent(n);
          //printf("  Adding node %s to new base node %s (c=%d,p=%d)\n",
          //   n->label().data(),
          //   bn->label().data(),
          //   bn->children() ? bn->children()->count() : 0,
          //   bn->parents()  ? bn->parents()->count()  : 0
          //  );
          //printf("  inserting %s (%p)\n",bClass->name().data(),bn);
          m_usedNodes->insert(bClass->name(),bn); // add node to the used list
        }
        if (!bClass->isVisited() && !hideSuper && bClass->subClasses())
        {
          bool wasVisited=bClass->isVisited();
          bClass->setVisited(TRUE);
          addHierarchy(bn,bClass,wasVisited);
        }
      }
    }
  }
  //printf("end addHierarchy\n");
}

void DotGfxHierarchyTable::addClassList(const ClassSDict *cl)
{
  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for (cli.toLast();(cd=cli.current());--cli)
  {
    //printf("Trying %s subClasses=%d\n",cd->name().data(),cd->subClasses()->count());
    if (cd->getLanguage()==SrcLangExt_VHDL &&
      (VhdlDocGen::VhdlClasses)cd->protection()!=VhdlDocGen::ENTITYCLASS
      )
    {
      continue;
    }
    if (OPTIMIZE_OUTPUT_SLICE && cd->compoundType() != m_classType)
    {
      continue;
    }
    if (!hasVisibleRoot(cd->baseClasses()) &&
      cd->isVisibleInHierarchy()
      ) // root node in the forest
    {
      QCString tmp_url="";
      if (cd->isLinkable() && !cd->isHidden()) 
      {
        tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
        if (!cd->anchor().isEmpty())
        {
          tmp_url+="#"+cd->anchor();
        }
      }
      //printf("Inserting root class %s\n",cd->name().data());
      QCString tooltip = cd->briefDescriptionAsTooltip();
      DotNode *n = new DotNode(getNextNodeNumber(),
        cd->displayName(),
        tooltip,
        tmp_url.data());

      //m_usedNodes->clear();
      m_usedNodes->insert(cd->name(),n);
      m_rootNodes->insert(0,n);   
      if (!cd->isVisited() && cd->subClasses())
      {
        addHierarchy(n,cd,cd->isVisited());
        cd->setVisited(TRUE);
      }
    }
  }
}

DotGfxHierarchyTable::DotGfxHierarchyTable(const char *prefix,ClassDef::CompoundType ct)
  : m_prefix(prefix)
  , m_classType(ct)
{
  m_rootNodes = new QList<DotNode>;
  m_usedNodes = new QDict<DotNode>(1009); 
  m_usedNodes->setAutoDelete(TRUE);
  m_rootSubgraphs = new DotNodeList;

  // build a graph with each class as a node and the inheritance relations
  // as edges
  initClassHierarchy(Doxygen::classSDict);
  initClassHierarchy(Doxygen::hiddenClasses);
  addClassList(Doxygen::classSDict);
  addClassList(Doxygen::hiddenClasses);
  // m_usedNodes now contains all nodes in the graph

  // color the graph into a set of independent subgraphs
  bool done=FALSE; 
  int curColor=0;
  QListIterator<DotNode> dnli(*m_rootNodes);
  while (!done) // there are still nodes to color
  {
    DotNode *n;
    done=TRUE; // we are done unless there are still uncolored nodes
    for (dnli.toLast();(n=dnli.current());--dnli)
    {
      if (n->subgraphId()==-1) // not yet colored
      {
        //printf("Starting at node %s (%p): %d\n",n->label().data(),n,curColor);
        done=FALSE; // still uncolored nodes
        n->setSubgraphId(curColor);
        n->markAsVisible();
        n->colorConnectedNodes(curColor);
        curColor++;
        const DotNode *dn=n->findDocNode();
        if (dn!=0) 
          m_rootSubgraphs->inSort(dn);
        else
          m_rootSubgraphs->inSort(n);
      }
    }
  }

  //printf("Number of independent subgraphs: %d\n",curColor);
  QListIterator<DotNode> dnli2(*m_rootSubgraphs);
  DotNode *n;
  for (dnli2.toFirst();(n=dnli2.current());++dnli2)
  {
    //printf("Node %s color=%d (c=%d,p=%d)\n",
    //    n->label().data(),n->m_subgraphId,
    //    n->children()?n->children()->count():0,
    //    n->parents()?n->parents()->count():0);
    int number=0;
    n->renumberNodes(number);
  }
}

DotGfxHierarchyTable::~DotGfxHierarchyTable()
{
  //printf("DotGfxHierarchyTable::~DotGfxHierarchyTable\n");

  //QDictIterator<DotNode> di(*m_usedNodes);
  //DotNode *n;
  //for (;(n=di.current());++di)
  //{
  //  printf("Node %p: %s\n",n,n->label().data());
  //}

  delete m_rootNodes;
  delete m_usedNodes;
  delete m_rootSubgraphs;
}

/*****************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include <stdlib.h>

#include "dot.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

//--------------------------------------------------------------------

const int maxImageWidth=1024;
const int maxImageHeight=1024;
  

/*! mapping from protection levels to color names */
static char *edgeColorMap[] =
{
  "midnightblue",  // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "darkorchid3",   // "use" relation
  "grey"           // Undocumented
};

static char *edgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

/*! converts the rectangles in a server site image map into a client 
 *  site image map.
 *  \param t the stream to which the result is written.
 *  \param mapName the name of the map file.
 *  \returns TRUE if succesful.
 */
static bool convertMapFile(QTextStream &t,const char *mapName)
{
  QFile f(mapName);
  if (!f.open(IO_ReadOnly)) return FALSE;
  const int maxLineLen=1024;
  char buf[maxLineLen];
  char url[maxLineLen];
  int x1,y1,x2,y2;
  while (!f.atEnd())
  {
    int numBytes = f.readLine(buf,maxLineLen);
    buf[numBytes-1]='\0';
    //printf("ReadLine `%s'\n",buf);
    if (strncmp(buf,"rect",4)==0)
    {
      sscanf(buf,"rect %s %d,%d %d,%d",url,&x1,&y2,&x2,&y1);
      char *refPtr = url;
      char *urlPtr = strchr(url,'$');
      //printf("url=`%s'\n",url);
      if (urlPtr)
      {
        QCString *dest;
        *urlPtr++='\0';
        //printf("refPtr=`%s' urlPtr=`%s'\n",refPtr,urlPtr);
        //printf("Found url=%s coords=%d,%d,%d,%d\n",file,x1,y1,x2,y2);
        t << "<area ";
        if (*refPtr!='\0')
        {
          t << "doxygen=\"" << refPtr << ":";
          if ((dest=tagDestinationDict[refPtr])) t << *dest << "/";
          t << "\" ";
        }
        t << "href=\""; 
        if (*refPtr!='\0')
        {
          if ((dest=tagDestinationDict[refPtr])) t << *dest << "/";
        }
        t << urlPtr << "\" shape=\"rect\" coords=\"" 
          << x1 << "," << y1 << "," << x2 << "," << y2 << "\">" << endl;
      }
    }
  }
  
  return TRUE;
}

static bool readBoundingBoxDot(const char *fileName,int *width,int *height)
{
  QFile f(fileName);
  if (!f.open(IO_ReadOnly)) return FALSE;
  const int maxLineLen=1024;
  char buf[maxLineLen];
  while (!f.atEnd())
  {
    int numBytes = f.readLine(buf,maxLineLen);
    buf[numBytes-1]='\0';
    if (strncmp(buf,"\tgraph [bb",10)==0)
    {
      int x,y;
      if (sscanf(buf,"\tgraph [bb= \"%d,%d,%d,%d\"];",&x,&y,width,height)!=4)
      {
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;
}

static bool readBoundingBoxEPS(const char *fileName,int *width,int *height)
{
  QFile f(fileName);
  if (!f.open(IO_ReadOnly)) return FALSE;
  const int maxLineLen=1024;
  char buf[maxLineLen];
  while (!f.atEnd())
  {
    int numBytes = f.readLine(buf,maxLineLen);
    buf[numBytes-1]='\0';
    if (strncmp(buf,"%%BoundingBox: ",15)==0)
    {
      int x,y;
      if (sscanf(buf,"%%%%BoundingBox: %d %d %d %d",&x,&y,width,height)!=4)
      {
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;
}

/*! returns TRUE if class cd is a leaf (i.e. has no visible children)
 */
static bool isLeaf(ClassDef *cd)
{
  BaseClassList *bcl = cd->superClasses();
  if (bcl->count()>0) // class has children, check their visibility
  {
    BaseClassListIterator bcli(*bcl);
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()); ++bcli )
    {
      ClassDef *bClass = bcd->classDef;
      //if (bClass->isLinkable() || !isLeaf(bClass)) return FALSE;
      
      // if class is not a leaf
      if (!isLeaf(bClass)) return FALSE;
      // or class is not documented in this project
      if (!Config::allExtFlag && !bClass->isLinkableInProject()) return FALSE;
      // or class is not documented and all ALLEXTERNALS = YES
      if (Config::allExtFlag && !bClass->isLinkable()) return FALSE;    
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------

class DotNodeList : public QList<DotNode>
{
  public:
    DotNodeList() : QList<DotNode>() {}
   ~DotNodeList() {}
   int compareItems(GCI item1,GCI item2)
   {
     return stricmp(((DotNode *)item1)->m_label,((DotNode *)item2)->m_label);
   }
};

//--------------------------------------------------------------------


/*! helper function that deletes all nodes in a connected graph, given
 *  one of the graph's nodes
 */
static void deleteNodes(DotNode *node)
{
  static DotNodeList deletedNodes;
  deletedNodes.setAutoDelete(TRUE);
  node->deleteNode(deletedNodes); // collect nodes to be deleted.
  deletedNodes.clear(); // actually remove the nodes.
}

DotNode::DotNode(int n,const char *lab,const char *url,int distance,bool isRoot)
  : m_number(n), m_label(lab), m_url(url), m_isRoot(isRoot)
{
  m_children = 0; 
  m_edgeInfo = 0;
  m_parents = 0;
  m_subgraphId=-1;
  m_deleted=FALSE;
  m_written=FALSE;
  m_hasDoc=FALSE;
  m_distance = distance;
}

DotNode::~DotNode()
{
  delete m_children;
  delete m_parents;
  delete m_edgeInfo;
}

void DotNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance=distance;
}

void DotNode::addChild(DotNode *n,
                       int edgeColor,
                       int edgeStyle,
                       const char *edgeLab,
                       const char *edgeURL,
                       int edgeLabCol
                      )
{
  if (m_children==0)
  {
    m_children = new QList<DotNode>;
    m_edgeInfo = new QList<EdgeInfo>;
    m_edgeInfo->setAutoDelete(TRUE);
  }
  m_children->append(n);
  EdgeInfo *ei = new EdgeInfo;
  ei->m_color = edgeColor;
  ei->m_style = edgeStyle; 
  ei->m_label = edgeLab;
  ei->m_url   = edgeURL;
  if (edgeLabCol==-1)
    ei->m_labColor=edgeColor;
  else
    ei->m_labColor=edgeLabCol;
  m_edgeInfo->append(ei);
}

void DotNode::addParent(DotNode *n)
{
  if (m_parents==0)
  {
    m_parents = new QList<DotNode>;
  }
  m_parents->append(n);
}

void DotNode::removeChild(DotNode *n)
{
  if (m_children) m_children->remove(n);
}

void DotNode::removeParent(DotNode *n)
{
  if (m_parents) m_parents->remove(n);
}

void DotNode::deleteNode(DotNodeList &deletedList)
{
  if (m_deleted) return; // avoid recursive loops in case the graph has cycles
  m_deleted=TRUE;
  if (m_parents!=0) // delete all parent nodes of this node
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      //pn->removeChild(this);
      pn->deleteNode(deletedList);
    }
  }
  if (m_children!=0) // delete all child nodes of this node
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      //cn->removeParent(this);
      cn->deleteNode(deletedList);
    }
  }
  // add this node to the list of deleted nodes.
  deletedList.append(this);
}

static QCString convertLabel(const QCString &l)
{
  QCString result;
  const char *p=l.data();
  char c;
  while ((c=*p++))
  {
    if (c=='\\') result+="\\\\";
    else result+=c;
  }
  return result;
}

void DotNode::writeBox(QTextStream &t,
                       GraphOutputFormat format,
                       bool hasNonReachableChildren)
{
  const char *labCol = 
          m_url.isEmpty() ? "grey" :  // non link
           (
            (hasNonReachableChildren) ? "red" : "black"
           );
  t << "  Node" << m_number << " [shape=\"box\",label=\""
    << convertLabel(m_label)    
    << "\",fontsize=10,height=0.2,width=0.4";
  if (format==GIF) t << ",fontname=\"doxfont\"";
  t << ",color=\"" << labCol << "\"";
  if (m_isRoot)
  {
    t << ",style=\"filled\" fontcolor=\"white\"";
  }
  else if (!m_url.isEmpty())
  {
    t << ",URL=\"" << m_url << ".html\"";
  }
  t << "];" << endl; 
}

void DotNode::writeArrow(QTextStream &t,
                         GraphOutputFormat format,
                         DotNode *cn,
                         EdgeInfo *ei,
                         bool topDown, 
                         bool pointBack
                        )
{
  t << "  Node";
  if (topDown) t << cn->number(); else t << m_number;
  t << " -> Node";
  if (topDown) t << m_number; else t << cn->number();
  t << " [";
  if (pointBack) t << "dir=back,";
  t << "color=\"" << edgeColorMap[ei->m_color] 
    << "\",fontsize=10,style=\"" << edgeStyleMap[ei->m_style] << "\"";
  if (!ei->m_label.isEmpty())
  {
    t << ",label=\"" << ei->m_label << "\"";
  }
  if (format==GIF) t << ",fontname=\"doxfont\"";
  t << "];" << endl; 
}

void DotNode::write(QTextStream &t,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    int distance
                   )
{
  //printf("DotNode::write(%d) name=%s\n",distance,m_label.data());
  if (m_written) return; // node already written to the output
  if (m_distance>distance) return;
  QList<DotNode> *nl = toChildren ? m_children : m_parents; 
  bool hasNonReachableChildren=FALSE;
  if (m_distance==distance && nl)
  {
    QListIterator<DotNode> dnli(*nl);
    DotNode *cn;
    for (dnli.toFirst();(cn=dnli.current());++dnli)
    {
      if (cn->m_distance>distance) hasNonReachableChildren=TRUE;
    }
  }
  writeBox(t,format,hasNonReachableChildren);
  m_written=TRUE;
  if (nl)
  {
    if (toChildren)
    {
      QListIterator<DotNode>  dnli1(*nl);
      QListIterator<EdgeInfo> dnli2(*m_edgeInfo);
      DotNode *cn;
      for (dnli1.toFirst();(cn=dnli1.current());++dnli1,++dnli2)
      {
        if (cn->m_distance<=distance) 
        {
          writeArrow(t,format,cn,dnli2.current(),topDown);
        }
        cn->write(t,format,topDown,toChildren,distance);
      }
    }
    else // render parents
    {
      QListIterator<DotNode> dnli(*nl);
      DotNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->m_distance<=distance) 
        {
          writeArrow(t,
                     format,
                     pn,
                     pn->m_edgeInfo->at(pn->m_children->findRef(this)),
                     FALSE
                    );
        }
        pn->write(t,format,TRUE,FALSE,distance);
      }
    }
  }
}

void DotNode::clearWriteFlag()
{
  m_written=FALSE;
  if (m_parents!=0)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->m_written)
      {
        pn->clearWriteFlag();
      }
    }
  }
  if (m_children!=0)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->m_written)
      {
        cn->clearWriteFlag();
      }
    }
  }
}

void DotNode::colorConnectedNodes(int curColor)
{ 
  if (m_children)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->m_subgraphId==-1) // uncolored child node
      {
        cn->m_subgraphId=curColor;
        cn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",cn->m_label.data(),cn,cn->m_subgraphId);
      }
    }
  }

  if (m_parents)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->m_subgraphId==-1) // uncolored parent node
      {
        pn->m_subgraphId=curColor;
        pn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",pn->m_label.data(),pn,pn->m_subgraphId);
      }
    }
  }
}

const DotNode *DotNode::findDocNode() const
{
  if (!m_url.isEmpty()) return this;
  //printf("findDocNode(): `%s'\n",m_label.data());
  if (m_parents)
  {
    QListIterator<DotNode> dnli(*m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (!pn->m_hasDoc)
      {
        pn->m_hasDoc=TRUE;
        const DotNode *dn = pn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> dnli(*m_children);
    DotNode *cn;
    for (dnli.toFirst();(cn=dnli.current());++dnli)
    {
      if (!cn->m_hasDoc)
      {
        cn->m_hasDoc=TRUE;
        const DotNode *dn = cn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  return 0;
}

//--------------------------------------------------------------------

int DotGfxHierarchyTable::m_curNodeNumber;

void DotGfxHierarchyTable::writeGraph(QTextStream &out,const char *path)
{
  //printf("DotGfxHierarchyTable::writeGraph(%s)\n",name);
  //printf("m_rootNodes=%p count=%d\n",m_rootNodes,m_rootNodes->count());
  if (m_rootSubgraphs->count()==0) return;

  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // goto the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  out << "<table border=0 cellspacing=10 cellpadding=0>" << endl;

  QListIterator<DotNode> dnli(*m_rootSubgraphs);
  DotNode *n;
  for (dnli.toFirst();(n=dnli.current());++dnli)
  {
    QCString baseName;
    QCString diskName=n->m_url.copy();
    int i=diskName.find('$'); 
    if (i!=-1)
    {
      diskName=diskName.right(diskName.length()-i-1);
    }
    else /* take the label name as the file name (and strip any template stuff) */
    {
      diskName=convertNameToFile(n->m_label);
    }
    baseName.sprintf("inherit_graph_%s",diskName.data());
    QCString dotName=baseName+".dot";
    QCString gifName=baseName+".gif";
    QCString mapName=baseName+".map";

    QFile f(dotName);
    if (!f.open(IO_WriteOnly)) return;
    QTextStream t(&f);
    t << "digraph inheritance" << endl;
    t << "{" << endl;
    t << "  rankdir=LR;" << endl;
    QListIterator<DotNode> dnli2(*m_rootNodes);
    DotNode *node;
    for (;(node=dnli2.current());++dnli2)
    {
      if (node->m_subgraphId==n->m_subgraphId) node->write(t,GIF,FALSE,TRUE);
    }
    t << "}" << endl;
    f.close();

    QCString dotCmd;
    dotCmd.sprintf("dot -Tgif \"%s\" -o \"%s\"",dotName.data(),gifName.data());
    //printf("Running: dot -Tgif %s -o %s\n",dotName.data(),gifName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      out << "</table>" << endl;
      return;
    }
    dotCmd.sprintf("dot -Timap \"%s\" -o \"%s\"",dotName.data(),mapName.data());
    //printf("Running: dot -Timap %s -o %s\n",dotName.data(),mapName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      out << "</table>" << endl;
      return;
    }
    out << "<tr><td><img src=\"" << gifName << "\" border=\"0\" usemap=\"#" 
      << n->m_label << "_map\"></td></tr>" << endl;
    out << "<map name=\"" << n->m_label << "_map\">" << endl;
    convertMapFile(out,mapName);
    out << "</map>" << endl;
    thisDir.remove(dotName);
    thisDir.remove(mapName);
  }
  out << "</table>" << endl;

  QDir::setCurrent(oldDir);
}

void DotGfxHierarchyTable::addHierarchy(DotNode *n,ClassDef *cd,bool hideSuper)
{
  //printf("addHierarchy `%s' baseClasses=%d\n",cd->name().data(),cd->baseClasses()->count());
  BaseClassListIterator bcli(*cd->superClasses());
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    ClassDef *bClass=bcd->classDef; 
    //printf("Trying super class=`%s'\n",bClass->name().data());
    if (bClass->isVisibleInHierarchy() && hasVisibleRoot(bClass->baseClasses()))
    {
      DotNode *bn;
      //printf("Node `%s' Found visible class=`%s'\n",n->m_label.data(),
      //                                              bClass->name().data());
      if ((bn=m_usedNodes->find(bClass->name()))) // node already present 
      {
        //printf("Base node `%s'\n",bn->m_label.data());
        //if (n->m_children)
        //{
        //  QListIterator<DotNode> dnli(*n->m_children);
        //  DotNode *cn;
        //  for (dnli.toFirst();(cn=dnli.current());++dnli)
        //  {
        //    printf("Child node `%s'\n",cn->m_label.data());
        //  }
        //  printf("ref node = %p\n",n->m_children->findRef(bn));
        //}

        if (n->m_children==0 || n->m_children->findRef(bn)==-1) // no arrow yet
        {
          n->addChild(bn,bcd->prot);
          bn->addParent(n);
          //printf("Adding node %s to existing base node %s (c=%d,p=%d)\n",
          //       n->m_label.data(),
          //       bn->m_label.data(),
          //       bn->m_children ? bn->m_children->count() : 0,
          //       bn->m_parents  ? bn->m_parents->count()  : 0
          //     );
        }
        //else
        //{
        //  printf("Class already has an arrow!\n");
        //}
      }
      else 
      {
        bn = new DotNode(m_curNodeNumber++,
             bClass->name(),
             bClass->isLinkable() ? 
              (bClass->getReference()+"$"+bClass->getOutputFileBase()).data() : 
              0
           );
        //printf("Adding node %s to new base node %s (c=%d,p=%d)\n",
        //   n->m_label.data(),
        //   bn->m_label.data(),
        //   bn->m_children ? bn->m_children->count() : 0,
        //   bn->m_parents  ? bn->m_parents->count()  : 0
        //  );
        n->addChild(bn,bcd->prot);
        bn->addParent(n);
        m_usedNodes->insert(bClass->name(),bn); // add node to the used list
      }
      if (!bClass->visited && !hideSuper && bClass->superClasses()->count()>0)
      {
        bool wasVisited=bClass->visited;
        bClass->visited=TRUE;
        addHierarchy(bn,bClass,wasVisited);
      }
    }
  }
}

DotGfxHierarchyTable::DotGfxHierarchyTable()
{
  m_curNodeNumber=0;
  m_rootNodes = new QList<DotNode>;
  //m_rootNodes->setAutoDelete(TRUE);    // rootNodes owns the nodes
  m_usedNodes = new QDict<DotNode>(1009); // virtualNodes only aliases nodes
  m_rootSubgraphs = new DotNodeList;
  
  // build a graph with each class as a node and the inheritance relations
  // as edges
  initClassHierarchy(&classList);
  ClassListIterator cli(classList);
  ClassDef *cd;
  for (cli.toLast();(cd=cli.current());--cli)
  {
    //printf("Trying %s superClasses=%d\n",cd->name().data(),cd->superClasses()->count());
    if (!hasVisibleRoot(cd->baseClasses()))
    {
      if (cd->isVisibleInHierarchy()) // root class in the graph
      {
        //printf("Inserting root class %s\n",cd->name().data());
        DotNode *n = new DotNode(m_curNodeNumber++,
                cd->name(),
                cd->isLinkable() ?
                  (cd->getReference()+"$"+cd->getOutputFileBase()).data() :
                  0
               );
        
        //m_usedNodes->clear();
        m_usedNodes->insert(cd->name(),n);
        m_rootNodes->insert(0,n);   
        if (!cd->visited && cd->superClasses()->count()>0)
        {
          addHierarchy(n,cd,cd->visited);
          cd->visited=TRUE;
        }
      }
    }
  }
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
      if (n->m_subgraphId==-1) // not yet colored
      {
        //printf("Starting at node %s (%p): %d\n",n->m_label.data(),n,curColor);
        done=FALSE; // still uncolored nodes
        n->m_subgraphId=curColor;
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
  //QListIterator<DotNode> dnli2(*m_rootSubgraphs);
  //DotNode *n;
  //for (dnli2.toFirst();(n=dnli2.current());++dnli2)
  //{
  //  printf("Node %s color=%d (c=%d,p=%d)\n",
  //      n->m_label.data(),n->m_subgraphId,
  //      n->m_children?n->m_children->count():0,
  //      n->m_parents?n->m_parents->count():0);
  //}
}

DotGfxHierarchyTable::~DotGfxHierarchyTable()
{
  DotNode *n = m_rootNodes->first();
  while (n)
  {
    DotNode *oldNode=n;
    n=m_rootNodes->next();
    deleteNodes(oldNode); 
  }
  delete m_rootNodes;
  delete m_usedNodes;
  delete m_rootSubgraphs;
}

//--------------------------------------------------------------------

int DotClassGraph::m_curNodeNumber;

void DotClassGraph::addClass(ClassDef *cd,DotNode *n,int prot,
    const char *label,int distance,const char *templSpec,bool base)
{
  //printf(":: DoxGfxUsageGraph::addClass(class=%s,parent=%s,prot=%d,label=%s,dist=%d)\n",
  //                                 cd->name().data(),n->m_label.data(),prot,label,distance);
  int edgeStyle = label ? EdgeInfo::Dashed : EdgeInfo::Solid;
  QCString className;
  if (templSpec)
    className=insertTemplateSpecifierInScope(cd->name(),templSpec);
  else
    className=cd->name().copy();
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
    //printf(" add exiting node %s of %s\n",bn->m_label.data(),n->m_label.data());
  }
  else // new class
  {
    bn = new DotNode(m_curNodeNumber++,
        className,
        cd->isLinkable() ? 
          (cd->getReference()+"$"+cd->getOutputFileBase()).data() : 0,
        distance
       );
    if (distance>m_maxDistance) m_maxDistance=distance;
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
    m_usedNodes->insert(className,bn);
    //printf(" add used node %s of %s\n",cd->name().data(),n->m_label.data());
    if (distance<m_recDepth) buildGraph(cd,bn,distance+1,base);
  }
}

void DotClassGraph::buildGraph(ClassDef *cd,DotNode *n,int distance,bool base)
{
  BaseClassListIterator bcli(base ? *cd->baseClasses() : *cd->superClasses());
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    addClass(bcd->classDef,n,bcd->prot,0,distance,bcd->templSpecifiers,base); 
  }
  if (m_graphType != Inheritance)
  {
    UsesClassDict *dict = 
      m_graphType==Implementation ? cd->usedImplementationClasses() :
                                    cd->usedInterfaceClasses();
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
        for (;(s=dvi.currentKey());++dvi)
        {
          if (first) 
          {
            label=s;
            first=FALSE;
          }
          else
          {
            label+=QCString("\\n")+s;
          }
        }
        //printf("Found label=`%s'\n",label.data());
        addClass(ucd->classDef,n,EdgeInfo::Black,label,distance,ucd->templSpecifiers,base);
      }
    }
  }
}

DotClassGraph::DotClassGraph(ClassDef *cd,GraphType t,int maxRecursionDepth)
{
  //printf("DotGfxUsage::DotGfxUsage %s\n",cd->name().data());
  m_graphType = t;
  m_maxDistance = 0;
  m_recDepth = maxRecursionDepth;
  m_startNode = new DotNode(m_curNodeNumber++,
                            cd->name(),
                            cd->isLinkable() ? 
                              (cd->getReference()+"$"+cd->getOutputFileBase()).data() : 0,
                            0,                         // distance
                            TRUE                       // is a root node
                           );
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(cd->name(),m_startNode);
  //printf("Root node %s\n",cd->name().data());
  if (m_recDepth>0) 
  {
    buildGraph(cd,m_startNode,1,TRUE);
    if (t==Inheritance) buildGraph(cd,m_startNode,1,FALSE);
  }
  m_diskName = cd->getOutputFileBase().copy();
}

bool DotClassGraph::isTrivial() const
{
  if (m_graphType==Inheritance)
    return m_startNode->m_children==0 && m_startNode->m_parents==0;
  else
    return m_startNode->m_children==0;
}

DotClassGraph::~DotClassGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

void writeDotGraph(DotNode *root,
                   GraphOutputFormat format,
                   const QCString &baseName,
                   bool lrRank,
                   bool renderParents,
                   int distance
                  )
{
  // generate the graph description for dot
  //printf("writeDotGraph(%s,%d)\n",baseName.data(),renderParents);
  QFile f;
  f.setName(baseName+".dot");
  if (f.open(IO_WriteOnly))
  {
    QTextStream t(&f);
    t << "digraph inheritance" << endl;
    t << "{" << endl;
    if (lrRank)
    {
      t << "  rankdir=LR;" << endl;
    }
    root->clearWriteFlag();
    root->write(t,format,TRUE,TRUE,distance);
    if (renderParents && root->m_parents) 
    {
      //printf("rendering parents!\n");
      QListIterator<DotNode>  dnli(*root->m_parents);
      DotNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->m_distance<=distance) 
        {
          root->writeArrow(t,
                           format,
                           pn,
                           pn->m_edgeInfo->at(pn->m_children->findRef(root)),
                           FALSE
                          );
        }
        pn->write(t,format,TRUE,FALSE,distance);
      }
    }
    t << "}" << endl;
    f.close();
  }
}

static void findMaximalDotGraph(DotNode *root,
                                int maxDist,
                                const QCString &baseName,
                                QDir &thisDir,
                                GraphOutputFormat format,
                                bool lrRank=FALSE,
                                bool renderParents=FALSE
                               )
{
  bool lastFit;
  int minDistance=1;
  int maxDistance=maxDist;
  int curDistance=maxDistance;
  int width=0;
  int height=0;

  // binary search for the maximal inheritance depth that fits in a reasonable
  // sized image (dimensions: maxImageWidth, maxImageHeight)
  do
  {
    writeDotGraph(root,format,baseName,lrRank,renderParents,curDistance);

    QCString dotCmd;
    // create annotated dot file
    dotCmd.sprintf("dot -Tdot \"%s.dot\" -o \"%s_tmp.dot\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      return;
    }

    // extract bounding box from the result
    readBoundingBoxDot(baseName+"_tmp.dot",&width,&height);
    width  = width *96/72; // 96 pixels/inch, 72 points/inch
    height = height*96/72; // 96 pixels/inch, 72 points/inch
    //printf("Found bounding box (%d,%d)\n",width,height);
    
    lastFit=(width<maxImageWidth && height<maxImageHeight);
    if (lastFit) // image is small enough
    {
      minDistance=curDistance;
      //printf("Image fits [%d-%d]\n",minDistance,maxDistance);
    }
    else
    {
      maxDistance=curDistance;
      //printf("Image does not fit [%d-%d]\n",minDistance,maxDistance);
    }
    curDistance=minDistance+(maxDistance-minDistance)/2;
    //printf("curDistance=%d\n",curDistance);
    
    // remove temporary dot file
    thisDir.remove(baseName+"_tmp.dot");
    
  } while (maxDistance-minDistance>1);

  if (!lastFit)
  {
    //printf("Using last fit %d\n",minDistance);
    writeDotGraph(root,
                  format,
                  baseName,
                  lrRank || (curDistance==1 && width>maxImageWidth),
                  renderParents,
                  minDistance
                 );
  }
}

QCString DotClassGraph::diskName() const
{
  return m_diskName + "_coll_graph"; 
}

void DotClassGraph::writeGraph(QTextStream &out,
                               GraphOutputFormat format,
                               const char *path,
                               bool isTBRank)
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // goto the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString baseName;
  QCString mapName;
  switch (m_graphType)
  {
    case Implementation:
      baseName.sprintf("%s_coll_graph",m_diskName.data());
      mapName="coll_map";
      break;
    case Interface:
      baseName.sprintf("%s_intf_graph",m_diskName.data());
      mapName="intf_map";
      break;
    case Inheritance:
      baseName.sprintf("%s_inherit_graph",m_diskName.data());
      mapName="inherit_map";
      break;
  }

  findMaximalDotGraph(m_startNode,m_maxDistance,baseName,
                      thisDir,format,!isTBRank,m_graphType==Inheritance);

  if (format==GIF) // run dot to create a .gif image
  {
    QCString dotCmd;
    dotCmd.sprintf("dot -Tgif \"%s.dot\" -o \"%s.gif\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
       err("Error: Problems running dot. Check your installation!\n");
       return;
    }
    // run dot again to create an image map
    dotCmd.sprintf("dot -Timap \"%s.dot\" -o \"%s.map\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
       err("Error: Problems running dot. Check your installation!\n");
       return;
    }
    out << "<p><center><img src=\"" << baseName << ".gif\" border=\"0\" usemap=\"#"
        << m_startNode->m_label << "_" << mapName << "\"></center>" << endl;
    out << "<map name=\"" << m_startNode->m_label << "_" << mapName << "\">" << endl;
    convertMapFile(out,baseName+".map");
    out << "</map><p>" << endl;
    thisDir.remove(baseName+".map");
  }
  else if (format==EPS) // run dot to create a .eps image
  {
    QCString dotCmd;
    dotCmd.sprintf("dot -Tps \"%s.dot\" -o \"%s.eps\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
       err("Error: Problems running dot. Check your installation!\n");
       return;
    }
    int width,height;
    if (!readBoundingBoxEPS(baseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      return;
    }
    int maxWidth = 420; /* approx. page width in points */
   
    out << "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\setlength{\\epsfxsize}{" << QMIN(width/2,maxWidth) << "pt}\n"
           "\\epsfbox{" << baseName << ".eps}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }
  thisDir.remove(baseName+".dot");

  QDir::setCurrent(oldDir);
}

//--------------------------------------------------------------------

int DotInclDepGraph::m_curNodeNumber;

void DotInclDepGraph::buildGraph(DotNode *n,FileDef *fd,int distance)
{
  QList<IncludeInfo> *includeFiles = fd->includeFileList();
  QListIterator<IncludeInfo> ili(*includeFiles);
  IncludeInfo *ii;
  for (;(ii=ili.current());++ili)
  {
    FileDef *bfd = ii->fileDef;
    QCString in  = ii->includeName;
    //printf(">>>> in=`%s' bfd=%p\n",ii->includeName.data(),bfd);
    bool doc=TRUE,src=FALSE;
    if (bfd)
    {
      in  = bfd->absFilePath();  
      doc = bfd->isLinkableInProject();
      src = bfd->generateSource() || (!bfd->isReference() && Config::sourceBrowseFlag);
    }
    if (doc || src)
    {
      QCString url=bfd ? bfd->getOutputFileBase().data() : "";
      if (!doc && src)
      {
        url+="-source";
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
        bn = new DotNode(
                          m_curNodeNumber++,
                          ii->includeName,
                          bfd ? (bfd->getReference()+"$"+url).data() : 0,
                          distance
                        );
        if (distance>m_maxDistance) m_maxDistance=distance;
        n->addChild(bn,0,0,0);
        bn->addParent(n);
        m_usedNodes->insert(in,bn);
        if (bfd) buildGraph(bn,bfd,distance+1);
      }
    }
  }
}

DotInclDepGraph::DotInclDepGraph(FileDef *fd)
{
  m_maxDistance = 0;
  ASSERT(fd!=0);
  m_diskName  = fd->getOutputFileBase().copy();
  m_startNode = new DotNode(m_curNodeNumber++,
                            fd->name(),
                            fd->getReference()+"$"+fd->getOutputFileBase(),
                            0,       // distance
                            TRUE     // root node
                           );
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(fd->absFilePath(),m_startNode);
  buildGraph(m_startNode,fd,1);
}

DotInclDepGraph::~DotInclDepGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotInclDepGraph::diskName() const
{
  return m_diskName + "_incldep"; 
}

void DotInclDepGraph::writeGraph(QTextStream &out,
                                 GraphOutputFormat format,
                                 const char *path
                                )
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // goto the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString baseName=m_diskName+"_incldep";

  findMaximalDotGraph(m_startNode,m_maxDistance,baseName,thisDir,format);

  if (format==GIF)
  {
    // run dot to create a .gif image
    QCString dotCmd;
    dotCmd.sprintf("dot -Tgif \"%s.dot\" -o \"%s.gif\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      return;
    }
    //printf("dot -Tgif %s.dot -o %s.gif",baseName.data(),baseName.data());

    // run dot again to create an image map
    dotCmd.sprintf("dot -Timap \"%s.dot\" -o \"%s.map\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      return;
    }

    out << "<p><center><img src=\"" << baseName << ".gif\" border=\"0\" usemap=\"#"
      << m_startNode->m_label << "_map\"></center>" << endl;
    out << "<map name=\"" << m_startNode->m_label << "_map\">" << endl;
    convertMapFile(out,baseName+".map");
    out << "</map><p>" << endl;
    thisDir.remove(baseName+".map");
  }
  else if (format==EPS)
  {
    // run dot to create a .eps image
    QCString dotCmd;

    dotCmd.sprintf("dot -Tps \"%s.dot\" -o \"%s.eps\"",baseName.data(),baseName.data());
    if (system(dotCmd)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      return;
    }
    int width,height;
    if (!readBoundingBoxEPS(baseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      return;
    }
    int maxWidth = 420; /* approx. page width in points */
   
    out << "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\setlength{\\epsfxsize}{" << QMIN(width/2,maxWidth) << "pt}\n"
           "\\epsfbox{" << baseName << ".eps}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }

  thisDir.remove(baseName+".dot");

  QDir::setCurrent(oldDir);
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

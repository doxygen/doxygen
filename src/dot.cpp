/*****************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#include "config.h"
#include "language.h"
#include "scanner.h"
#include "defargs.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

//--------------------------------------------------------------------

static const int maxCmdLine = 4096;

/*! mapping from protection levels to color names */
static const char *edgeColorMap[] =
{
  "midnightblue",  // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "darkorchid3",   // "use" relation
  "grey75",        // Undocumented
  "orange"         // template relation
};

static const char *edgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

/*! converts the rectangles in a server site image map into a client 
 *  site image map.
 *  \param t the stream to which the result is written.
 *  \param mapName the name of the map file.
 *  \param urlOnly if FALSE the url field in the map contains an external 
 *                 references followed by a $ and then the URL.  
 *  \returns TRUE if succesful.
 */
static bool convertMapFile(QTextStream &t,const char *mapName,
                           bool urlOnly=FALSE)
{
  QFile f(mapName);
  if (!f.open(IO_ReadOnly)) 
  {
    err("Error opening map file %s for inclusion in the docs!\n",mapName);
    return FALSE;
  }
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
      // obtain the url and the coordinates in the order used by graphviz-1.5
      sscanf(buf,"rect %s %d,%d %d,%d",url,&x1,&y1,&x2,&y2);
      // later versions of graphviz corrected the y coordinate order
      // the rule is that y2>=y1, so test and switch if needed
      if (y2<y1)
      {
        int temp=y2;
        y2=y1;
        y1=temp;
      }
      // there shouldn't be any need for this for known versions of graphviz
      // but it can't do any harm to check that x follows the rules as well
      if (x2<x1)
      {
        int temp=x2;
        x2=x1;
        x1=temp;
      }
      if (urlOnly)
      {
          t << "<area href=\"" << url << "\" shape=\"rect\" coords=\"" 
            << x1 << "," << y1 << "," << x2 << "," << y2 << "\""
            << " alt=\"\">" << endl;
      }
      else // name and external reference are separated by a $
      {
        char *refPtr = url;
        char *urlPtr = strchr(url,'$');
        //printf("url=`%s'\n",url);
        if (urlPtr)
        {
          QCString *dest;
          *urlPtr++='\0';
          //printf("refPtr=`%s' urlPtr=`%s'\n",refPtr,urlPtr);
          //printf("Found url=%s coords=%d,%d,%d,%d\n",url,x1,y1,x2,y2);
          t << "<area ";
          if (*refPtr!='\0')
          {
            t << "doxygen=\"" << refPtr << ":";
            if ((dest=Doxygen::tagDestinationDict[refPtr])) t << *dest << "/";
            t << "\" ";
          }
          t << "href=\""; 
          if (*refPtr!='\0')
          {
            if ((dest=Doxygen::tagDestinationDict[refPtr])) t << *dest << "/";
          }
          t << urlPtr << "\" shape=\"rect\" coords=\"" 
            << x1 << "," << y1 << "," << x2 << "," << y2 << "\""
            << " alt=\"\">" << endl;
        }
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
  BaseClassList *bcl = cd->subClasses();
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
      if (!Config_getBool("ALLEXTERNALS") && !bClass->isLinkableInProject()) return FALSE;
      // or class is not documented and all ALLEXTERNALS = YES
      if (Config_getBool("ALLEXTERNALS") && !bClass->isLinkable()) return FALSE;    
    }
  }
  return TRUE;
}

// since dot silently reproduces the input file when it does not
// support the PNG format, we need to check the result.
static void checkDotResult(const QCString &imgName)
{
  if (Config_getEnum("DOT_IMAGE_FORMAT")=="png")
  {
    QFile f(imgName);
    if (f.open(IO_ReadOnly))
    {
      char data[4];
      if (f.readBlock(data,4)==4)
      {
        if (!(data[1]=='P' && data[2]=='N' && data[3]=='G'))
        {
          err("Error! Image `%s' produced by dot is not a valid PNG!\n"
              "You should either select a different format "
              "(DOT_IMAGE_FORMAT in the config file) or install a more "
              "recent version of graphviz (1.7+)\n",imgName.data()
             );
        }
      }
      else
      {
        err("Error: Could not read image `%s' generated by dot!\n",imgName.data());
      }
    }
    else
    {
      err("Error: Could not open image `%s' generated by dot!\n",imgName.data());
    }
  }
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
static void deleteNodes(DotNode *node,SDict<DotNode> *skipNodes=0)
{
  //printf("deleteNodes skipNodes=%p\n",skipNodes);
  static DotNodeList deletedNodes;
  deletedNodes.setAutoDelete(TRUE);
  node->deleteNode(deletedNodes,skipNodes); // collect nodes to be deleted.
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
  //printf("DotNode::~DotNode() %s\n",m_label.data());
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

void DotNode::deleteNode(DotNodeList &deletedList,SDict<DotNode> *skipNodes)
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
      pn->deleteNode(deletedList,skipNodes);
    }
  }
  if (m_children!=0) // delete all child nodes of this node
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      //cn->removeParent(this);
      cn->deleteNode(deletedList,skipNodes);
    }
  }
  // add this node to the list of deleted nodes.
  //printf("skipNodes=%p find(%p)=%p\n",skipNodes,this,skipNodes ? skipNodes->find((int)this) : 0);
  if (skipNodes==0 || skipNodes->find((char*)this)==0)
  {
    //printf("deleting\n");
    deletedList.append(this);
  }
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
          m_url.isEmpty() ? "grey75" :  // non link
           (
            (hasNonReachableChildren) ? "red" : "black"
           );
  t << "  Node" << m_number << " [shape=\"box\",label=\""
    << convertLabel(m_label)    
    << "\",fontsize=10,height=0.2,width=0.4";
  if (format==BITMAP) t << ",fontname=\"Helvetica\"";
  t << ",color=\"" << labCol << "\"";
  if (m_isRoot)
  {
    t << ",style=\"filled\" fontcolor=\"white\"";
  }
  else if (!m_url.isEmpty())
  {
    t << ",URL=\"" << m_url << Doxygen::htmlFileExtension << "\"";
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
  if (format==BITMAP) t << ",fontname=\"Helvetica\"";
  t << "];" << endl; 
}

void DotNode::write(QTextStream &t,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    int distance,
                    bool backArrows
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
          writeArrow(t,format,cn,dnli2.current(),topDown,backArrows);
        }
        cn->write(t,format,topDown,toChildren,distance,backArrows);
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
                     FALSE,
                     backArrows
                    );
        }
        pn->write(t,format,TRUE,FALSE,distance,backArrows);
      }
    }
  }
}

void DotNode::writeXML(QTextStream &t,bool isClassGraph)
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    char *refPtr = url.data();
    char *urlPtr = strchr(url.data(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << "        <link id=\"" << urlPtr << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << refPtr << "\"";
      }
      t << "/>" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode id=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->m_color)
        {
          case EdgeInfo::Blue:    t << "public-inheritance"; break;
          case EdgeInfo::Green:   t << "protected-inheritance"; break;
          case EdgeInfo::Red:     t << "private-inheritance"; break;
          case EdgeInfo::Purple:  t << "usage"; break;
          case EdgeInfo::Orange:  t << "template-instance"; break;
          case EdgeInfo::Grey:    ASSERT(0); break;
        }
      }
      else // include graph
      {
        t << "include"; 
      }
      t << "\">" << endl;
      if (!edgeInfo->m_label.isEmpty()) 
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->m_label.find("\\n",p))!=-1)
        {
          t << "          <edgelabel>" 
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+2;
        }
        t << "          <edgelabel>" 
          << convertToXML(edgeInfo->m_label.right(edgeInfo->m_label.length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    } 
  }
  t << "      </node>" << endl;
}


void DotNode::writeDEF(QTextStream &t)
{
  char* nodePrefix = "        node-";

  t << "      node = {" << endl;
  t << nodePrefix << "id    = " << m_number << ';' << endl;
  t << nodePrefix << "label = '" << m_label << "';" << endl;

  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    char *refPtr = url.data();
    char *urlPtr = strchr(url.data(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << nodePrefix << "link = {" << endl << "  "
        << nodePrefix << "link-id = '" << urlPtr << "';" << endl;

      if (*refPtr!='\0')
      {
        t << "  " << nodePrefix << "link-external = '"
          << refPtr << "';" << endl;
      }
      t << "        };" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        node-child = {" << endl;
      t << "          child-id = '" << childNode->m_number << "';" << endl;
      t << "          relation = ";

      switch(edgeInfo->m_color)
      {
        case EdgeInfo::Blue:    t << "public-inheritance"; break;
        case EdgeInfo::Green:   t << "protected-inheritance"; break;
        case EdgeInfo::Red:     t << "private-inheritance"; break;
        case EdgeInfo::Purple:  t << "usage"; break;
        case EdgeInfo::Orange:  t << "template-instance"; break;
        case EdgeInfo::Grey:    ASSERT(0); break;
      }
      t << ';' << endl;

      if (!edgeInfo->m_label.isEmpty()) 
      {
        t << "          edgelabel = <<_EnD_oF_dEf_TeXt_" << endl
          << edgeInfo->m_label << endl
          << "_EnD_oF_dEf_TeXt_;" << endl;
      }
      t << "        }; /* node-child */" << endl;
    } /* for (;childNode...) */
  }
  t << "      }; /* node */" << endl;
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
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  out << "<table border=0 cellspacing=10 cellpadding=0>" << endl;

  QListIterator<DotNode> dnli(*m_rootSubgraphs);
  DotNode *n;
  int count=0;
  for (dnli.toFirst();(n=dnli.current());++dnli)
  {
    QCString baseName;
    QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
    baseName.sprintf("inherit_graph_%d",count++);
    baseName = convertNameToFile(baseName);
    QCString dotName=baseName+".dot";
    QCString imgName=baseName+"."+ imgExt;
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
      if (node->m_subgraphId==n->m_subgraphId) 
        node->write(t,BITMAP,FALSE,TRUE,1000,TRUE);
    }
    t << "}" << endl;
    f.close();

    QCString dotArgs(maxCmdLine);
    dotArgs.sprintf("-T%s \"%s\" -o \"%s\"",
           imgExt.data(), dotName.data(),imgName.data());
    //printf("Running: dot -T%s %s -o %s\n",imgExt.data(),dotName.data(),imgName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      out << "</table>" << endl;
      return;
    }
    checkDotResult(imgName);
    dotArgs.sprintf("-Timap \"%s\" -o \"%s\"",dotName.data(),mapName.data());
    //printf("Running: dot -Timap %s -o %s\n",dotName.data(),mapName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      out << "</table>" << endl;
      return;
    }
    QCString mapLabel = convertNameToFile(n->m_label);
    out << "<tr><td><img src=\"" << imgName << "\" border=\"0\" alt=\"\" usemap=\"#" 
      << mapLabel << "_map\">" << endl;
    out << "<map name=\"" << mapLabel << "_map\">" << endl;
    convertMapFile(out,mapName);
    out << "</map></td></tr>" << endl;
    if (Config_getBool("DOT_CLEANUP")) thisDir.remove(dotName);
    thisDir.remove(mapName);
  }
  out << "</table>" << endl;

  QDir::setCurrent(oldDir);
}

void DotGfxHierarchyTable::addHierarchy(DotNode *n,ClassDef *cd,bool hideSuper)
{
  //printf("addHierarchy `%s' baseClasses=%d\n",cd->name().data(),cd->baseClasses()->count());
  BaseClassListIterator bcli(*cd->subClasses());
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
        QCString tmp_url="";
        if (bClass->isLinkable()) 
          tmp_url=bClass->getReference()+"$"+bClass->getOutputFileBase();
        bn = new DotNode(m_curNodeNumber++,
             bClass->displayName(),
             tmp_url.data()
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
      if (!bClass->visited && !hideSuper && bClass->subClasses()->count()>0)
      {
        bool wasVisited=bClass->visited;
        bClass->visited=TRUE;
        addHierarchy(bn,bClass,wasVisited);
      }
    }
  }
}

void DotGfxHierarchyTable::addClassList(ClassSDict *cl)
{
  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for (cli.toLast();(cd=cli.current());--cli)
  {
    //printf("Trying %s subClasses=%d\n",cd->name().data(),cd->subClasses()->count());
    if (!hasVisibleRoot(cd->baseClasses()) &&
        cd->isVisibleInHierarchy()
       ) // root node in the forest
    {
      QCString tmp_url="";
      if (cd->isLinkable()) 
        tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
      //printf("Inserting root class %s\n",cd->name().data());
      DotNode *n = new DotNode(m_curNodeNumber++,
          cd->displayName(),
          tmp_url.data());

      //m_usedNodes->clear();
      m_usedNodes->insert(cd->name(),n);
      m_rootNodes->insert(0,n);   
      if (!cd->visited && cd->subClasses()->count()>0)
      {
        addHierarchy(n,cd,cd->visited);
        cd->visited=TRUE;
      }
    }
  }
}

DotGfxHierarchyTable::DotGfxHierarchyTable()
{
  m_curNodeNumber=0;
  m_rootNodes = new QList<DotNode>;
  m_usedNodes = new QDict<DotNode>(1009); 
  m_usedNodes->setAutoDelete(TRUE);
  m_rootSubgraphs = new DotNodeList;
  
  // build a graph with each class as a node and the inheritance relations
  // as edges
  initClassHierarchy(&Doxygen::classSDict);
  initClassHierarchy(&Doxygen::hiddenClasses);
  addClassList(&Doxygen::classSDict);
  addClassList(&Doxygen::hiddenClasses);
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

//--------------------------------------------------------------------

int DotClassGraph::m_curNodeNumber;

void DotClassGraph::addClass(ClassDef *cd,DotNode *n,int prot,
    const char *label,int distance,const char *usedName,const char *templSpec,bool base)
{
  if (Config_getBool("HIDE_UNDOC_CLASSES") && !cd->isLinkable()) return;

  int edgeStyle = (label || prot==EdgeInfo::Orange) ? EdgeInfo::Dashed : EdgeInfo::Solid;
  QCString className;
  if (usedName) // name is a typedef
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
  //                                 className.data(),n->m_label.data(),prot,label,distance,usedName,templSpec,base);
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
    QCString displayName=className;
    if (Config_getBool("HIDE_SCOPE_NAMES")) displayName=stripScope(displayName);
    QCString tmp_url;
    if (cd->isLinkable()) 
    {
      tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
    }
    bn = new DotNode(m_curNodeNumber++,
        displayName,
        tmp_url.data(),
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
    //printf(" add new child node `%s' to %s\n",className.data(),n->m_label.data());
    
    // we use <=, i.s.o < to cause one more level than intended which is used to 
    // detect truncated nodes
    if (distance<=m_recDepth) buildGraph(cd,bn,distance+1,base);
  }
}

void DotClassGraph::buildGraph(ClassDef *cd,DotNode *n,int distance,bool base)
{
  // ---- Add inheritance relations

  BaseClassListIterator bcli(base ? *cd->baseClasses() : *cd->subClasses());
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    //printf("-------- inheritance relation %s->%s templ=`%s'\n",
    //            cd->name().data(),bcd->classDef->name().data(),bcd->templSpecifiers.data());
    addClass(bcd->classDef,n,bcd->prot,0,distance,bcd->usedName,
               bcd->templSpecifiers,base); 
  }
  if (m_graphType != Inheritance)
  {

    // ---- Add usage relations
    
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
        addClass(ucd->classDef,n,EdgeInfo::Purple,label,distance,0,
                 ucd->templSpecifiers,base);
      }
    }
  }

  // ---- Add template instantiation relations

  if (Config_getBool("TEMPLATE_RELATIONS"))
  {
    if (base) // template relations for base classes
    {
      ClassDef *templMaster=cd->templateMaster();
      if (templMaster)
      {
        QDictIterator<ClassDef> cli(*templMaster->getTemplateInstances());
        ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          if (templInstance==cd)
          {
            addClass(templMaster,n,EdgeInfo::Orange,cli.currentKey(),distance,0,
                0,TRUE);
          }
        }
      }
    }
    else // template relations for super classes
    {
      QDict<ClassDef> *templInstances = cd->getTemplateInstances();
      if (templInstances)
      {
        QDictIterator<ClassDef> cli(*templInstances);
        ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          addClass(templInstance,n,EdgeInfo::Orange,cli.currentKey(),distance,0,
              0,FALSE);
        }
      }
    }
  }
}

DotClassGraph::DotClassGraph(ClassDef *cd,GraphType t,int maxRecursionDepth)
{
  //printf("--------------- DotClassGraph::DotClassGraph `%s'\n",cd->displayName().data());
  m_graphType = t;
  m_maxDistance = 0;
  m_recDepth = maxRecursionDepth;
  QCString tmp_url="";
  if (cd->isLinkable()) tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
  QCString className = cd->displayName();
  //if (cd->templateArguments())
  //{
  //  className+=tempArgListToString(cd->templateArguments());
  //}
  m_startNode = new DotNode(m_curNodeNumber++,
                            className,
                            tmp_url.data(),
                            0,                         // distance
                            TRUE                       // is a root node
                           );
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(className,m_startNode);

  //ClassSDict::Iterator cli(Doxygen::classSDict);
  //ClassDef *icd;
  //for (cli.toFirst();(icd=cli.current());++cli) icd->initTemplateMapping();

  //printf("Root node %s\n",cd->name().data());
  if (m_recDepth>0) 
  {
    buildGraph(cd,m_startNode,1,TRUE);
    if (t==Inheritance) buildGraph(cd,m_startNode,1,FALSE);
  }
  m_diskName = cd->getFileBase().copy();
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
                   int distance,
                   bool backArrows
                  )
{
  // generate the graph description for dot
  //printf("writeDotGraph(%s,%d)\n",baseName.data(),backArrows);
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
    root->write(t,format,TRUE,TRUE,distance,backArrows);
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
                           FALSE,
                           backArrows
                          );
        }
        pn->write(t,format,TRUE,FALSE,distance,backArrows);
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
                                bool renderParents=FALSE,
                                bool backArrows=TRUE
                               )
{
  int minDistance=1; // min distance that shows only direct children.
  int curDistance; //=QMIN(2,maxDist); // current distance to try
  int maxDistance=maxDist; // max distance that show whole graph
  int width=0;
  int height=0;
  int maxDotGraphWidth  = Config_getInt("MAX_DOT_GRAPH_WIDTH");
  int maxDotGraphHeight = Config_getInt("MAX_DOT_GRAPH_HEIGHT");
  int lastFit=minDistance;

  // binary search for the maximal inheritance depth that fits in a reasonable
  // sized image (dimensions: Config_getInt("MAX_DOT_GRAPH_WIDTH"), Config_getInt("MAX_DOT_GRAPH_HEIGHT"))
  while (minDistance<=maxDistance)
  {
    curDistance = (minDistance+maxDistance)/2;

    writeDotGraph(root,format,baseName,lrRank,renderParents,
        curDistance,backArrows);

    QCString dotArgs(maxCmdLine);
    // create annotated dot file
    dotArgs.sprintf("-Tdot \"%s.dot\" -o \"%s_tmp.dot\"",baseName.data(),baseName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      return;
    }

    // extract bounding box from the result
    readBoundingBoxDot(baseName+"_tmp.dot",&width,&height);
    width  = width *96/72; // 96 pixels/inch, 72 points/inch
    height = height*96/72; // 96 pixels/inch, 72 points/inch
    //printf("Found bounding box (%d,%d) max (%d,%d)\n",width,height,
    //    Config_getInt("MAX_DOT_GRAPH_WIDTH"),Config_getInt("MAX_DOT_GRAPH_HEIGHT"));

    // remove temporary dot file
    thisDir.remove(baseName+"_tmp.dot");

    bool graphFits=(width<maxDotGraphWidth && height<maxDotGraphHeight);
    if (graphFits) // graph is small enough
    {
      lastFit=curDistance;
      minDistance=curDistance+1;
      //printf("Image fits [%d-%d]\n",minDistance,maxDistance);
    }
    else // graph does not fit anymore with curDistance
    {
      //printf("Image does not fit [%d-%d]\n",minDistance,maxDistance);
      maxDistance=curDistance-1;
    }
  }
  //printf("lastFit=%d\n",lastFit);

  writeDotGraph(root,
                  format,
                  baseName,
                  lrRank || (minDistance==1 && width>Config_getInt("MAX_DOT_GRAPH_WIDTH")),
                  renderParents,
                  lastFit,
                  backArrows
                 );
}

QCString DotClassGraph::diskName() const
{
  QCString result=m_diskName.copy();
  switch (m_graphType)
  {
    case Implementation:
      result+="_coll_graph";
      break;
    case Interface:
      result+="_intf_graph";
      break;
    case Inheritance:
      result+="_inherit_graph";
      break;
  }
  return result;
}

QCString DotClassGraph::writeGraph(QTextStream &out,
                               GraphOutputFormat format,
                               const char *path,
                               bool isTBRank,
                               bool generateImageMap)
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString baseName;
  QCString mapName;
  switch (m_graphType)
  {
    case Implementation:
      mapName="coll_map";
      break;
    case Interface:
      mapName="intf_map";
      break;
    case Inheritance:
      mapName="inherit_map";
      break;
  }
  baseName = convertNameToFile(diskName());

  findMaximalDotGraph(m_startNode,QMIN(m_recDepth,m_maxDistance),baseName,
                      thisDir,format,!isTBRank,m_graphType==Inheritance);

  if (format==BITMAP) // run dot to create a bitmap image
  {
    QCString dotArgs(maxCmdLine);
    QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
    QCString imgName = baseName+"."+imgExt;
    dotArgs.sprintf("-T%s \"%s.dot\" -o \"%s\"",
                   imgExt.data(),baseName.data(),imgName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
       err("Error: Problems running dot. Check your installation!\n");
       QDir::setCurrent(oldDir);
       return baseName;
    }
    checkDotResult(imgName);
    if (generateImageMap)
    {
      // run dot again to create an image map
      dotArgs.sprintf("-Timap \"%s.dot\" -o \"%s.map\"",baseName.data(),baseName.data());
      if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
      {
        err("Error: Problems running dot. Check your installation!\n");
        QDir::setCurrent(oldDir);
        return baseName;
      }
      QCString mapLabel = convertNameToFile(m_startNode->m_label+"_"+mapName);
      out << "<p><center><img src=\"" << baseName << "." 
          << imgExt << "\" border=\"0\" usemap=\"#"
          << mapLabel << "\" alt=\"";
      switch (m_graphType)
      {
        case Implementation:
          out << "Collaboration graph";
          break;
        case Interface:
          out << "Interface dependency graph";
          break;
        case Inheritance:
          out << "Inheritance graph";
          break;
      }
      out << "\"></center>" << endl;
      QString tmpstr;
      QTextOStream tmpout(&tmpstr);
      convertMapFile(tmpout,baseName+".map");
      if (!tmpstr.isEmpty())
      {
        out << "<map name=\"" << mapLabel << "\">" << endl;
        out << tmpstr;
        out << "</map>" << endl;
      }
      thisDir.remove(baseName+".map");
    }
  }
  else if (format==EPS) // run dot to create a .eps image
  {
    QCString dotArgs(maxCmdLine);
    dotArgs.sprintf("-Tps \"%s.dot\" -o \"%s.eps\"",baseName.data(),baseName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
       err("Error: Problems running dot. Check your installation!\n");
       QDir::setCurrent(oldDir);
       return baseName;
    }
    int width,height;
    if (!readBoundingBoxEPS(baseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      QDir::setCurrent(oldDir);
      return baseName;
    }
    if (Config_getBool("USE_PDFLATEX"))
    {
      QCString epstopdfArgs(maxCmdLine);
      epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                     baseName.data(),baseName.data());
      if (iSystem("epstopdf",epstopdfArgs,TRUE)!=0)
      {
         err("Error: Problems running epstopdf. Check your TeX installation!\n");
         QDir::setCurrent(oldDir);
         return baseName;
      }
    }
    int maxWidth = 420; /* approx. page width in points */
   
    out << "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
                                      << "pt]{" << baseName << "}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }
  if (Config_getBool("DOT_CLEANUP")) thisDir.remove(baseName+".dot");

  QDir::setCurrent(oldDir);
  return baseName;
}

//--------------------------------------------------------------------

void DotClassGraph::writeXML(QTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,TRUE);
  }
}

void DotClassGraph::writeDEF(QTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDEF(t);
  }
}

//--------------------------------------------------------------------

int DotInclDepGraph::m_curNodeNumber;

void DotInclDepGraph::buildGraph(DotNode *n,FileDef *fd,int distance)
{
  QList<IncludeInfo> *includeFiles = 
     m_inverse ? fd->includedByFileList() : fd->includeFileList();
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
      doc = bfd->isLinkable();
      src = bfd->generateSourceFile();
    }
    if (doc || src)
    {
      QCString url="";
      if (bfd) url=bfd->getOutputFileBase().copy();
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
        QCString tmp_url="";
        if (bfd) tmp_url=bfd->getReference()+"$"+url;
        bn = new DotNode(
                          m_curNodeNumber++,
                          ii->includeName,
                          tmp_url,
                          distance
                        );
        if (distance>m_maxDistance) m_maxDistance=distance;
        n->addChild(bn,0,0,0);
        bn->addParent(n);
        m_usedNodes->insert(in,bn);

        // we use <=, i.s.o < to cause one more level than intended which is used to 
        // detect truncated nodes
        if (bfd && distance<=m_recDepth) buildGraph(bn,bfd,distance+1);
      }
    }
  }
}

DotInclDepGraph::DotInclDepGraph(FileDef *fd,int maxRecursionDepth,bool inverse)
{
  m_maxDistance = 0;
  m_recDepth = maxRecursionDepth;
  m_inverse = inverse;
  ASSERT(fd!=0);
  m_diskName  = fd->getFileBase().copy();
  QCString tmp_url=fd->getReference()+"$"+fd->getFileBase();
  m_startNode = new DotNode(m_curNodeNumber++,
                            fd->name(),
                            tmp_url.data(),
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
  QCString result=m_diskName.copy();
  if (m_inverse) result+="_dep";
  result+="_incl";
  return convertNameToFile(result); 
}

QCString DotInclDepGraph::writeGraph(QTextStream &out,
                                 GraphOutputFormat format,
                                 const char *path,
                                 bool generateImageMap
                                )
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString baseName=m_diskName;
  if (m_inverse) baseName+="_dep";
  baseName+="_incl";
  baseName=convertNameToFile(baseName);
  QCString mapName=m_startNode->m_label.copy();
  if (m_inverse) mapName+="dep";

  findMaximalDotGraph(m_startNode,QMIN(m_recDepth,m_maxDistance),baseName,thisDir,format,
                      FALSE,FALSE,!m_inverse);

  if (format==BITMAP)
  {
    // run dot to create a bitmap image
    QCString dotArgs(maxCmdLine);
    QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
    QCString imgName=baseName+"."+imgExt;
    dotArgs.sprintf("-T%s \"%s.dot\" -o \"%s\"",
                   imgExt.data(),baseName.data(),imgName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      QDir::setCurrent(oldDir);
      return baseName;
    }
    checkDotResult(imgName);

    if (generateImageMap)
    {
      // run dot again to create an image map
      dotArgs.sprintf("-Timap \"%s.dot\" -o \"%s.map\"",
                      baseName.data(),baseName.data());
      if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
      {
        err("Problems running dot. Check your installation!\n");
        QDir::setCurrent(oldDir);
        return baseName;
      }

      out << "<p><center><img src=\"" << baseName << "." 
          << imgExt << "\" border=\"0\" usemap=\"#"
          << mapName << "_map\" alt=\"";
      if (m_inverse) out << "Included by dependency graph"; else out << "Include dependency graph";
      out << "\">";
      out << "</center>" << endl;
      QString tmpstr;
      QTextOStream tmpout(&tmpstr);
      convertMapFile(tmpout,baseName+".map");
      if (!tmpstr.isEmpty())
      {
        out << "<map name=\"" << mapName << "_map\">" << endl;
        out << tmpstr;
        out << "</map>" << endl;
      }
      thisDir.remove(baseName+".map");
    }
  }
  else if (format==EPS)
  {
    // run dot to create a .eps image
    QCString dotArgs(maxCmdLine);
    dotArgs.sprintf("-Tps \"%s.dot\" -o \"%s.eps\"",
                   baseName.data(),baseName.data());
    if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
    {
      err("Problems running dot. Check your installation!\n");
      QDir::setCurrent(oldDir);
      return baseName;
    }
    int width,height;
    if (!readBoundingBoxEPS(baseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      QDir::setCurrent(oldDir);
      return baseName;
    }
    if (Config_getBool("USE_PDFLATEX"))
    {
      QCString epstopdfArgs(maxCmdLine);
      epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                     baseName.data(),baseName.data());
      if (iSystem("epstopdf",epstopdfArgs,TRUE)!=0)
      {
         err("Error: Problems running epstopdf. Check your TeX installation!\n");
         QDir::setCurrent(oldDir);
         return baseName;
      }
    }
    int maxWidth = 420; /* approx. page width in points */
   
    out << "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           //"\\setlength{\\epsfxsize}{" << QMIN(width/2,maxWidth) << "pt}\n"
           //"\\epsfbox{" << baseName << ".eps}\n"
           "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
                                      << "pt]{" << baseName << "}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }

  if (Config_getBool("DOT_CLEANUP")) thisDir.remove(baseName+".dot");

  QDir::setCurrent(oldDir);
  return baseName;
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

void DotInclDepGraph::writeXML(QTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,FALSE);
  }
}

//-------------------------------------------------------------

void generateGraphLegend(const char *path)
{
  QFile dotFile((QCString)path+"/graph_legend.dot");
  if (!dotFile.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",
        convertToQCString(dotFile.name()).data());
    return;
  }
  QTextStream dotText(&dotFile); 
  dotText << "digraph inheritance\n";
  dotText << "{\n";
  dotText << "  Node9 [shape=\"box\",label=\"Inherited\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",style=\"filled\" fontcolor=\"white\"];\n";
  dotText << "  Node10 -> Node9 [dir=back,color=\"midnightblue\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node10 [shape=\"box\",label=\"PublicBase\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classPublicBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node11 -> Node10 [dir=back,color=\"midnightblue\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node11 [shape=\"box\",label=\"Truncated\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"red\",URL=\"$classTruncated" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node13 -> Node9 [dir=back,color=\"darkgreen\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node13 [shape=\"box\",label=\"ProtectedBase\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classProtectedBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node14 -> Node9 [dir=back,color=\"firebrick4\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node14 [shape=\"box\",label=\"PrivateBase\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classPrivateBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node15 -> Node9 [dir=back,color=\"midnightblue\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node15 [shape=\"box\",label=\"Undocumented\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"grey75\"];\n";
  dotText << "  Node16 -> Node9 [dir=back,color=\"midnightblue\",fontsize=10,style=\"solid\",fontname=\"Helvetica\"];\n";
  dotText << "  Node16 [shape=\"box\",label=\"Templ< int >\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node17 -> Node16 [dir=back,color=\"orange\",fontsize=10,style=\"dashed\",label=\"< int >\",fontname=\"Helvetica\"];\n";
  dotText << "  Node17 [shape=\"box\",label=\"Templ< T >\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node18 -> Node9 [dir=back,color=\"darkorchid3\",fontsize=10,style=\"dashed\",label=\"m_usedClass\",fontname=\"Helvetica\"];\n";
  dotText << "  Node18 [shape=\"box\",label=\"Used\",fontsize=10,height=0.2,width=0.4,fontname=\"Helvetica\",color=\"black\",URL=\"$classUsed" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "}\n";
  dotFile.close();

  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());

  // run dot to generate the a bitmap image from the graph
  QCString dotArgs(maxCmdLine);
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = "graph_legend."+imgExt;
  dotArgs.sprintf("-T%s graph_legend.dot -o %s",imgExt.data(),imgName.data());
  if (iSystem(Config_getString("DOT_PATH")+"dot",dotArgs)!=0)
  {
      err("Problems running dot. Check your installation!\n");
      QDir::setCurrent(oldDir);
      return;
  }
  checkDotResult(imgName);
  QDir::setCurrent(oldDir);
}
  

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format)
{
  QCString absOutFile = outDir;
#ifdef _WIN32
  absOutFile+='\\';
#else
  absOutFile+='/';
#endif
  absOutFile+=outFile;

  // chdir to the output dir, so dot can find the font file.
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(outDir);
  //printf("Going to dir %s\n",QDir::currentDirPath().data());

  //{ // copy input file to output dir.
  //  QFile inf(inFile,IO_ReadOnly);
  //  QFileInfo infinfo(inf);
  //  uint s = infinfo.size();
  //  QByteArray a(s);
  //  inf.readBlock(a.data(),s);
  //  QFile outf(outDir,IO_WriteOnly);
  //  outf.writeBlock(a.data(),s);
  //}
  
  QCString dotArgs(maxCmdLine);
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = (QCString)outFile+"."+imgExt;
  if (format==BITMAP)
  {
    dotArgs.sprintf("-T%s \"%s\" -o \"%s\"",
        imgExt.data(),
        inFile,
        imgName.data());
  }
  else // format==EPS
  {
    dotArgs.sprintf("-Tps \"%s\" -o \"%s.eps\"",inFile,outFile);
  }
  QCString dotExe = Config_getString("DOT_PATH")+"dot";
  //printf("Running: %s %s\n",dotExe.data(),dotArgs.data());
  if (iSystem(dotExe,dotArgs)!=0)
  {
    err("Problems running dot. Check your installation!\n");
  }
  // Added by Nils Strom
  if ( (format==EPS) && (Config_getBool("USE_PDFLATEX")) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         outFile,outFile);
    if (iSystem("epstopdf",epstopdfArgs,TRUE)!=0)
    {
      err("Error: Problems running epstopdf. Check your TeX installation!\n");
    }
  }

  if (format==BITMAP) checkDotResult(imgName);

  QDir::setCurrent(oldDir);
}

 
/*! Marco Dalla Gasperina [marcodg@attbi.com] added this to allow
 *  dotfiles to generate image maps.
 *  \param inFile just the basename part of the filename
 *  \param outDir output directory
 *  \returns a string which is the HTML image map (without the \<map\>\</map\>)
 */
QString getDotImageMapFromFile(const QString& inFile, const QString& outDir)
{
  QString outFile = inFile + ".map";

  // chdir to the output dir, so dot can find the font file.
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(outDir);
  //printf("Going to dir %s\n",QDir::currentDirPath().data());

  QCString dotArgs(maxCmdLine);
  dotArgs.sprintf("-Timap \"%s\" -o \"%s\"", inFile.data(), outFile.data());

  QCString dotExe = Config_getString("DOT_PATH") + "dot";
  //printf("Running: %s %s\n",dotExe.data(),dotArgs.data());
  if (iSystem(dotExe,dotArgs)!=0)
  {
    err("Problems running dot. Check your installation!\n");
    QDir::setCurrent(oldDir);
    return "";
  }
  QString result;
  QTextOStream tmpout(&result);
  convertMapFile(tmpout, outFile, TRUE);
  QDir().remove(outFile);
  //printf("result=%s\n",result.data());

  QDir::setCurrent(oldDir);
  return result;
}
// end MDG mods



/*****************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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

#ifdef _WIN32
#include <windows.h>
#define BITMAP W_BITMAP
#endif

#include <stdlib.h>

#include "dot.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"
#include "config.h"
#include "language.h"
#include "defargs.h"
#include "docparser.h"
#include "debug.h"
#include "pagedef.h"
#include "portable.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include <qdir.h>
#include <qfile.h>
#include "ftextstream.h"
#include <md5.h>

#define MAP_CMD "cmapx"

//#define FONTNAME "FreeSans"
#define FONTNAME getDotFontName()
#define FONTSIZE getDotFontSize()

//--------------------------------------------------------------------

static const int maxCmdLine = 40960;

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

static const char *arrowStyle[] =
{
  "empty",         // Public
  "empty",         // Protected
  "empty",         // Private
  "open",          // "use" relation
  0,               // Undocumented
  0                // template relation
};

static const char *edgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

static QCString getDotFontName()
{
  static QCString dotFontName = Config_getString("DOT_FONTNAME");
  if (dotFontName.isEmpty()) dotFontName="FreeSans";
  return dotFontName;
}

static int getDotFontSize()
{
  static int dotFontSize = Config_getInt("DOT_FONTSIZE");
  if (dotFontSize<4) dotFontSize=4;
  return dotFontSize;
}

static void writeGraphHeader(FTextStream &t)
{
  t << "digraph G" << endl;
  t << "{" << endl;
  if (Config_getBool("DOT_TRANSPARENT"))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\","
       "labelfontname=\"" << FONTNAME << "\","
       "labelfontsize=\"" << FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\",shape=record];\n";
}

static void writeGraphFooter(FTextStream &t)
{
  t << "}" << endl;
}

/*! converts the rectangles in a client site image map into a stream
 *  \param t the stream to which the result is written.
 *  \param mapName the name of the map file.
 *  \param relPath the relative path to the root of the output directory
 *                 (used in case CREATE_SUBDIRS is enabled).
 *  \param urlOnly if FALSE the url field in the map contains an external
 *                 references followed by a $ and then the URL.
 *  \param context the context (file, class, or namespace) in which the
 *                 map file was found
 *  \returns TRUE if succesful.
 */
static bool convertMapFile(FTextStream &t,const char *mapName,
                           const QCString relPath, bool urlOnly=FALSE,
                           const QCString &context=QCString())
{
  QFile f(mapName);
  if (!f.open(IO_ReadOnly)) 
  {
    err("Error opening map file %s for inclusion in the docs!\n"
        "If you installed Graphviz/dot after a previous failing run, \n"
        "try deleting the output directory and rerun doxygen.\n",mapName);
    return FALSE;
  }
  const int maxLineLen=10240;
  while (!f.atEnd()) // foreach line
  {
    QCString buf(maxLineLen);
    int numBytes = f.readLine(buf.data(),maxLineLen);
    buf[numBytes-1]='\0';

    if (buf.left(5)=="<area")
    {
      // search for href="...", store ... part in link
      int indexS = buf.find("href=\""), indexE;
      if (indexS!=-1 && (indexE=buf.find('"',indexS+6))!=-1)
      {
        QCString link = buf.mid(indexS+6,indexE-indexS-6);
        QCString result;
        QCString *dest;
        if (urlOnly) // for user defined dot graphs
        {
          if (link.left(5)=="\\ref ") // \ref url
          {
            result="href=\"";
            // fake ref node to resolve the url
            DocRef *df = new DocRef( (DocNode*) 0, link.mid(5), context );
            if (!df->ref().isEmpty())
            {
              if ((dest=Doxygen::tagDestinationDict[df->ref()])) 
                result += *dest + "/";
            }
            else if (!relPath.isEmpty())
            {
              result += relPath;
            }
            if (!df->file().isEmpty())  
              result += df->file().data() + Doxygen::htmlFileExtension;
            if (!df->anchor().isEmpty()) 
              result += "#" + df->anchor();
            delete df;
            result += "\"";
          }
          else
          {
            result = "href=\"" + link + "\"";
          }
        }
        else // ref$url (external ref via tag file), or $url (local ref)
        {
          int marker = link.find('$');
          if (marker!=-1)
          {
            QCString ref = link.left(marker);
            QCString url = link.mid(marker+1);
            if (!ref.isEmpty())
            {
              result = "target=\"_blank\" doxygen=\"" + ref + ":";
              if ((dest=Doxygen::tagDestinationDict[ref])) result += *dest + "/";
              result += "\" ";
            }
            result+= "href=\"";
            if (!ref.isEmpty())
            {
              if ((dest=Doxygen::tagDestinationDict[ref])) result += *dest + "/";
            }
            else if (!relPath.isEmpty())
            {
              result += relPath;
            }
            result+= url + "\"";
          }
          else // should not happen, but handle properly anyway
          {
            result = "href=\"" + link + "\"";
          }
        }
        QCString leftPart = buf.left(indexS);
        QCString rightPart = buf.mid(indexE+1);
        buf = leftPart + result + rightPart;
      }
      t << buf;
    }
  }
  return TRUE;
}

static QArray<int> s_newNumber;
static int s_max_newNumber=0;

inline int reNumberNode(int number, bool doReNumbering)
{
  if (!doReNumbering) 
  {
    return number;
  } 
  else 
  {
    int s = s_newNumber.size();
    if (number>=s) 
    {
      int ns=0;
      ns = s * 3 / 2 + 5; // new size
      if (number>=ns) // number still doesn't fit
      {
        ns = number * 3 / 2 + 5;
      }
      s_newNumber.resize(ns);
      for (int i=s;i<ns;i++) // clear new part of the array
      {
        s_newNumber.at(i)=0;
      }
    }
    int i = s_newNumber.at(number);
    if (i == 0) // not yet mapped
    {
      i = ++s_max_newNumber; // start from 1
      s_newNumber.at(number) = i;
    }
    return i;
  }
}

static void resetReNumbering() 
{
  s_max_newNumber=0;
  s_newNumber.resize(s_max_newNumber);
}

static QCString g_dotFontPath;

static void setDotFontPath(const char *path)
{
  ASSERT(g_dotFontPath.isEmpty());
  g_dotFontPath = portable_getenv("DOTFONTPATH");
  QCString newFontPath = Config_getString("DOT_FONTPATH");
  QCString spath = path;
  if (!newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath.prepend(spath+portable_pathListSeparator());
  }
  else if (newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath=path;
  }
  else
  {
    portable_unsetenv("DOTFONTPATH");
    return;
  }
  portable_setenv("DOTFONTPATH",newFontPath);
}

static void unsetDotFontPath()
{
  if (g_dotFontPath.isEmpty())
  {
    portable_unsetenv("DOTFONTPATH");
  }
  else
  {
    portable_setenv("DOTFONTPATH",g_dotFontPath);
  }
  g_dotFontPath="";
}

static bool readBoundingBoxEPS(const char *fileName,int *width,int *height)
{
  QCString bb("%%PageBoundingBox:");
  QFile f(fileName);
  if (!f.open(IO_ReadOnly)) return FALSE;
  const int maxLineLen=1024;
  char buf[maxLineLen];
  while (!f.atEnd())
  {
    int numBytes = f.readLine(buf,maxLineLen-1); // read line
    buf[numBytes]='\0';
    if (strncmp(buf,bb.data(),bb.length()-1)==0) // found PageBoundingBox string
    {
      int x,y;
      if (sscanf(buf+bb.length(),"%d %d %d %d",&x,&y,width,height)!=4)
      {
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;
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

/*! Checks if a file "baseName".md5 exists. If so the contents
 *  are compared with \a md5. If equal FALSE is returned. If the .md5
 *  file does not exist or its contents are not equal to \a md5, 
 *  a new .md5 is generated with the \a md5 string as contents.
 */
static bool checkAndUpdateMd5Signature(const QCString &baseName,const QCString &md5)
{
  QFile f(baseName+".md5");
  if (f.open(IO_ReadOnly))
  {
    // read checksum
    QCString md5stored(33);
    int bytesRead=f.readBlock(md5stored.data(),32);
    md5stored[32]='\0';
    // compare checksum
    if (bytesRead==32 && md5==md5stored)
    {
      // bail out if equal
      return FALSE;
    }
  }
  f.close();
  // create checksum file
  if (f.open(IO_WriteOnly))
  {
    f.writeBlock(md5.data(),32); 
    f.close();
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

DotRunner::DotRunner(const char *file) : m_file(file)
{
  m_jobs.setAutoDelete(TRUE);
}

void DotRunner::addJob(const char *format,const char *output)
{
  QCString args = QCString("-T")+format+" -o \""+output+"\"";
  m_jobs.append(new QCString(args));
}

void DotRunner::addPostProcessing(const char *cmd,const char *args)
{
  m_postCmd = cmd;
  m_postArgs = args;
}

bool DotRunner::run()
{
  int exitCode=0;
  static QCString dotExe = Config_getString("DOT_PATH")+"dot";
  QCString dotArgs;
  QListIterator<QCString> li(m_jobs);
  QCString *s;
  if (Config_getBool("DOT_MULTI_TARGETS"))
  {
    dotArgs="\""+m_file+"\"";
    for (li.toFirst();(s=li.current());++li)
    {
      dotArgs+=' ';
      dotArgs+=*s;
    }
    if ((exitCode=portable_system(dotExe,dotArgs,FALSE))!=0)
    {
      goto error;
    }
  }
  else
  {
    for (li.toFirst();(s=li.current());++li)
    {
      dotArgs="\""+m_file+"\" "+*s;
      if ((exitCode=portable_system(dotExe,dotArgs,FALSE))!=0)
      {
        goto error;
      }
    }
  }
  if (!m_postCmd.isEmpty() && portable_system(m_postCmd,m_postArgs)!=0)
  {
    err("Error: Problems running '%s' as a post-processing step for dot output\n",m_postCmd.data());
    return FALSE;
  }
  return TRUE;
error:
  err("Problems running dot: exit code=%d, command='%s', arguments='%s'\n",
      exitCode,dotExe.data(),dotArgs.data());
  return FALSE;
}

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

DotNode::DotNode(int n,const char *lab,const char *tip, const char *url,
                 bool isRoot,ClassDef *cd)
  : m_subgraphId(-1)
  , m_number(n)
  , m_label(lab)
  , m_tooltip(tip)
  , m_url(url)
  , m_parents(0)
  , m_children(0)
  , m_edgeInfo(0)
  , m_deleted(FALSE)
  , m_written(FALSE)
  , m_hasDoc(FALSE)
  , m_isRoot(isRoot)
  , m_classDef(cd)
  , m_visible(FALSE)
  , m_truncated(Unknown)
  , m_distance(1000)
{
}

DotNode::~DotNode()
{
  delete m_children;
  delete m_parents;
  delete m_edgeInfo;
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

void DotNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance = distance;
}

static QCString convertLabel(const QCString &l)
{
  QCString result;
  const char *p=l.data();
  if (p==0) return result;
  char c;
  while ((c=*p++))
  {
    switch(c)
    {
      case '\\': result+="\\\\"; break;
      case '\n': result+="\\n"; break;
      case '<':  result+="\\<"; break;
      case '>':  result+="\\>"; break;
      case '|':  result+="\\|"; break;
      case '{':  result+="\\{"; break;
      case '}':  result+="\\}"; break;
      case '"':  result+="\\\""; break;
      default:   result+=c; break;
    }
  }
  return result;
}

static QCString escapeTooltip(const QCString &tooltip)
{
  QCString result;
  const char *p=tooltip.data();
  if (p==0) return result;
  char c;
  while ((c=*p++))
  {
    switch(c)
    {
      case '\\': result+="\\\\"; break;
      default:   result+=c; break;
    }
  }
  return result;
}

static void writeBoxMemberList(FTextStream &t,char prot,MemberList *ml,ClassDef *scope)
{
  if (ml)
  {
    MemberListIterator mlia(*ml);
    MemberDef *mma;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef() == scope)
      {
        t << prot << " " << convertLabel(mma->name());
        if (!mma->isObjCMethod() && 
            (mma->isFunction() || mma->isSlot() || mma->isSignal())) t << "()";
        t << "\\l";
      }
    }
    // write member groups within the memberlist
    MemberGroupList *mgl = ml->getMemberGroupList();
    if (mgl)
    {
      MemberGroupListIterator mgli(*mgl);
      MemberGroup *mg;
      for (mgli.toFirst();(mg=mgli.current());++mgli)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,prot,mg->members(),scope);
        }
      }
    }
  }
}

void DotNode::writeBox(FTextStream &t,
                       GraphType gt,
                       GraphOutputFormat /*format*/,
                       bool hasNonReachableChildren,
                       bool reNumber)
{
  const char *labCol = 
          m_url.isEmpty() ? "grey75" :  // non link
           (
            (hasNonReachableChildren) ? "red" : "black"
           );
  t << "  Node" << reNumberNode(m_number,reNumber) << " [label=\"";

  if (m_classDef && Config_getBool("UML_LOOK") && 
      (gt==Inheritance || gt==Collaboration))
  {
    t << "{" << convertLabel(m_label);
    t << "\\n|";
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::pubAttribs),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::pubStaticAttribs),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::properties),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberList::pacAttribs),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberList::pacStaticAttribs),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberList::proAttribs),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberList::proStaticAttribs),m_classDef);
    writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberList::priAttribs),m_classDef);
    writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberList::priStaticAttribs),m_classDef);
    t << "|";
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::pubMethods),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::pubStaticMethods),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberList::pubSlots),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberList::pacMethods),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberList::pacStaticMethods),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberList::proMethods),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberList::proStaticMethods),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberList::proSlots),m_classDef);
    writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberList::priMethods),m_classDef);
    writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberList::priStaticMethods),m_classDef);
    writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberList::priSlots),m_classDef);
    if (m_classDef->getMemberGroupSDict())
    {
      MemberGroupSDict::Iterator mgdi(*m_classDef->getMemberGroupSDict());
      MemberGroup *mg;
      for (mgdi.toFirst();(mg=mgdi.current());++mgdi)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,'*',mg->members(),m_classDef);
        }
      }
    }
    t << "}";
  }
  else // standard look
  {
    t << convertLabel(m_label);
  }
  t << "\",height=0.2,width=0.4";
  if (m_isRoot)
  {
    t << ",color=\"black\", fillcolor=\"grey75\", style=\"filled\" fontcolor=\"black\"";
  }
  else 
  {
    static bool dotTransparent = Config_getBool("DOT_TRANSPARENT");
    static bool vhdlOpt = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
    if (!dotTransparent)
    {
      ClassDef* ccd=this->m_classDef;

      t << ",color=\"" << labCol << "\", fillcolor=\"";
      if (ccd && vhdlOpt && (VhdlDocGen::VhdlClasses)ccd->protection()==VhdlDocGen::ARCHITECTURECLASS)
        t << "khaki";	
      else
        t << "white";
      t << "\", style=\"filled\"";
    }
    else
    {
      t << ",color=\"" << labCol << "\"";
    }
    if (!m_url.isEmpty())
    {
      int anchorPos = m_url.findRev('#');
      if (anchorPos==-1)
      {
        t << ",URL=\"" << m_url << Doxygen::htmlFileExtension << "\"";
      }
      else
      {
        t << ",URL=\"" << m_url.left(anchorPos) << Doxygen::htmlFileExtension
          << m_url.right(m_url.length()-anchorPos) << "\"";
      }
    }
    if (!m_tooltip.isEmpty())
    {
      t << ",tooltip=\"" << escapeTooltip(m_tooltip) << "\"";
    }
  }
  t << "];" << endl; 
}

void DotNode::writeArrow(FTextStream &t,
                         GraphType gt,
                         GraphOutputFormat format,
                         DotNode *cn,
                         EdgeInfo *ei,
                         bool topDown, 
                         bool pointBack,
                         bool reNumber
                        )
{
  t << "  Node";
  if (topDown) 
    t << reNumberNode(cn->number(),reNumber); 
  else 
    t << reNumberNode(m_number,reNumber);
  t << " -> Node";
  if (topDown) 
    t << reNumberNode(m_number,reNumber); 
  else 
    t << reNumberNode(cn->number(),reNumber);
  t << " [";
  if (pointBack) t << "dir=back,";
  t << "color=\"" << edgeColorMap[ei->m_color] 
    << "\",fontsize=\"" << FONTSIZE << "\",style=\"" << edgeStyleMap[ei->m_style] << "\"";
  if (!ei->m_label.isEmpty())
  {
    t << ",label=\"" << convertLabel(ei->m_label) << "\"";
  }
  if (Config_getBool("UML_LOOK") &&
      arrowStyle[ei->m_color] && 
      (gt==Inheritance || gt==Collaboration)
     )
  {
    if (pointBack) 
      t << ",arrowtail=\"" << arrowStyle[ei->m_color] << "\""; 
    else 
      t << ",arrowhead=\"" << arrowStyle[ei->m_color] << "\"";
  }

  if (format==BITMAP) t << ",fontname=\"" << FONTNAME << "\"";
  t << "];" << endl; 
}

void DotNode::write(FTextStream &t,
                    GraphType gt,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows,
                    bool reNumber
                   )
{
  //printf("DotNode::write(%d) name=%s this=%p written=%d\n",distance,m_label.data(),this,m_written);
  if (m_written) return; // node already written to the output
  if (!m_visible) return; // node is not visible
  writeBox(t,gt,format,m_truncated==Truncated,reNumber);
  m_written=TRUE;
  QList<DotNode> *nl = toChildren ? m_children : m_parents; 
  if (nl)
  {
    if (toChildren)
    {
      QListIterator<DotNode>  dnli1(*nl);
      QListIterator<EdgeInfo> dnli2(*m_edgeInfo);
      DotNode *cn;
      for (dnli1.toFirst();(cn=dnli1.current());++dnli1,++dnli2)
      {
        if (cn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",cn->label().data());
          writeArrow(t,gt,format,cn,dnli2.current(),topDown,backArrows,reNumber);
        }
        cn->write(t,gt,format,topDown,toChildren,backArrows,reNumber);
      }
    }
    else // render parents
    {
      QListIterator<DotNode> dnli(*nl);
      DotNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",pn->label().data());
          writeArrow(t,
              gt,
              format,
              pn,
              pn->m_edgeInfo->at(pn->m_children->findRef(this)),
              FALSE,
              backArrows,
              reNumber
              );
        }
        pn->write(t,gt,format,TRUE,FALSE,backArrows,reNumber);
      }
    }
  }
  //printf("end DotNode::write(%d) name=%s\n",distance,m_label.data());
}

void DotNode::writeXML(FTextStream &t,bool isClassGraph)
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
      t << "        <link refid=\"" << convertToXML(urlPtr) << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << convertToXML(refPtr) << "\"";
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
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
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
        while ((ni=edgeInfo->m_label.find('\n',p))!=-1)
        {
          t << "          <edgelabel>" 
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
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


void DotNode::writeDEF(FTextStream &t)
{
  const char* nodePrefix = "        node-";

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
        cn->markAsVisible();
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
        pn->markAsVisible();
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

void DotGfxHierarchyTable::writeGraph(FTextStream &out,const char *path) const
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
  setDotFontPath(d.absPath());
  //QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  //QDir::setCurrent(d.absPath());
  //QDir thisDir;

  // put each connected subgraph of the hierarchy in a row of the HTML output
  out << "<table border=\"0\" cellspacing=\"10\" cellpadding=\"0\">" << endl;

  QListIterator<DotNode> dnli(*m_rootSubgraphs);
  DotNode *n;
  int count=0;
  for (dnli.toFirst();(n=dnli.current());++dnli)
  {
    QCString baseName;
    QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
    baseName.sprintf("inherit_graph_%d",count++);
    baseName = convertNameToFile(baseName);
    QCString imgName = baseName+"."+ imgExt;
    QCString mapName = baseName+".map";
    QCString absImgName = QCString(d.absPath().data())+"/"+imgName;
    QCString absMapName = QCString(d.absPath().data())+"/"+mapName;
    QCString absBaseName = QCString(d.absPath().data())+"/"+baseName;
    QListIterator<DotNode> dnli2(*m_rootNodes);
    DotNode *node;

    // compute md5 checksum of the graph were are about to generate
    QGString theGraph;
    FTextStream md5stream(&theGraph);
    //md5stream.setEncoding(md5stream.UnicodeUTF8);
    writeGraphHeader(md5stream);
    md5stream << "  rankdir=LR;" << endl;
    for (dnli2.toFirst();(node=dnli2.current());++dnli2)
    {
      if (node->m_subgraphId==n->m_subgraphId) 
      {
        node->clearWriteFlag();
      }
    }
    for (dnli2.toFirst();(node=dnli2.current());++dnli2)
    {
      if (node->m_subgraphId==n->m_subgraphId) 
      {
        node->write(md5stream,DotNode::Hierarchy,BITMAP,FALSE,TRUE,TRUE,TRUE);
      }
    }
    writeGraphFooter(md5stream);
    resetReNumbering();
    uchar md5_sig[16];
    QCString sigStr(33);
    MD5Buffer((const unsigned char *)theGraph.data(),theGraph.length(),md5_sig);
    MD5SigToString(md5_sig,sigStr.data(),33);
    if (checkAndUpdateMd5Signature(absBaseName,sigStr) || 
        !QFileInfo(absMapName).exists())
    {
      // image was new or has changed
      QCString dotName=absBaseName+".dot";
      QFile f(dotName);
      if (!f.open(IO_WriteOnly)) return;
      FTextStream t(&f);
      //t.setEncoding(t.UnicodeUTF8);
      t << theGraph;
      f.close();
      resetReNumbering();

      DotRunner dotRun(dotName);
      dotRun.addJob(imgExt,absImgName);
      dotRun.addJob(MAP_CMD,absMapName);
      if (!dotRun.run())
      {
        out << "</table>" << endl;
        unsetDotFontPath();
        return;
      }
      checkDotResult(absImgName);
      if (Config_getBool("DOT_CLEANUP")) d.remove(dotName);
    }
    Doxygen::indexList.addImageFile(imgName);
    // write image and map in a table row
    QCString mapLabel = escapeCharsInString(n->m_label,FALSE);
    out << "<tr><td><img src=\"" << imgName << "\" border=\"0\" alt=\"\" usemap=\"#"
        << mapLabel << "_map\"/>" << endl;
    out << "<map name=\"" << mapLabel << "_map\" id=\"" << mapLabel << "\">" << endl;
    convertMapFile(out,absMapName,"");
    out << "</map></td></tr>" << endl;
    //thisDir.remove(mapName);
  }
  out << "</table>" << endl;

  unsetDotFontPath();
}

void DotGfxHierarchyTable::addHierarchy(DotNode *n,ClassDef *cd,bool hideSuper)
{
  //printf("addHierarchy `%s' baseClasses=%d\n",cd->name().data(),cd->baseClasses()->count());
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()) ; ++bcli )
    {
      ClassDef *bClass=bcd->classDef; 
      //printf("  Trying sub class=`%s' usedNodes=%d\n",bClass->name().data(),m_usedNodes->count());
      if (bClass->isVisibleInHierarchy() && hasVisibleRoot(bClass->baseClasses()))
      {
        DotNode *bn;
        //printf("  Node `%s' Found visible class=`%s'\n",n->m_label.data(),
        //                                              bClass->name().data());
        if ((bn=m_usedNodes->find(bClass->name()))) // node already present 
        {
          if (n->m_children==0 || n->m_children->findRef(bn)==-1) // no arrow yet
          {
            n->addChild(bn,bcd->prot);
            bn->addParent(n);
            //printf("  Adding node %s to existing base node %s (c=%d,p=%d)\n",
            //       n->m_label.data(),
            //       bn->m_label.data(),
            //       bn->m_children ? bn->m_children->count() : 0,
            //       bn->m_parents  ? bn->m_parents->count()  : 0
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
          }
          QCString tooltip = bClass->briefDescriptionAsTooltip();
          bn = new DotNode(m_curNodeNumber++,
              bClass->displayName(),
              tooltip,
              tmp_url.data()
              );
          n->addChild(bn,bcd->prot);
          bn->addParent(n);
          //printf("  Adding node %s to new base node %s (c=%d,p=%d)\n",
          //   n->m_label.data(),
          //   bn->m_label.data(),
          //   bn->m_children ? bn->m_children->count() : 0,
          //   bn->m_parents  ? bn->m_parents->count()  : 0
          //  );
          //printf("  inserting %s (%p)\n",bClass->name().data(),bn);
          m_usedNodes->insert(bClass->name(),bn); // add node to the used list
        }
        if (!bClass->visited && !hideSuper && bClass->subClasses())
        {
          bool wasVisited=bClass->visited;
          bClass->visited=TRUE;
          addHierarchy(bn,bClass,wasVisited);
        }
      }
    }
  }
  //printf("end addHierarchy\n");
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
      if (cd->isLinkable() && !cd->isHidden()) 
      {
        tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
      }
      //printf("Inserting root class %s\n",cd->name().data());
      QCString tooltip = cd->briefDescriptionAsTooltip();
      DotNode *n = new DotNode(m_curNodeNumber++,
          cd->displayName(),
          tooltip,
          tmp_url.data());

      //m_usedNodes->clear();
      m_usedNodes->insert(cd->name(),n);
      m_rootNodes->insert(0,n);   
      if (!cd->visited && cd->subClasses())
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
      if (n->m_subgraphId==-1) // not yet colored
      {
        //printf("Starting at node %s (%p): %d\n",n->m_label.data(),n,curColor);
        done=FALSE; // still uncolored nodes
        n->m_subgraphId=curColor;
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

int DotClassGraph::m_curNodeNumber = 0;

void DotClassGraph::addClass(ClassDef *cd,DotNode *n,int prot,
    const char *label,const char *usedName,const char *templSpec,bool base,int distance)
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
    if (cd->isLinkable() && !cd->isHidden()) 
    {
      tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
    }
    QCString tooltip = cd->briefDescriptionAsTooltip();
    bn = new DotNode(m_curNodeNumber++,
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
    //    className.data(),n->m_label.data(),cd->isHidden(),tmp_url.data());
    
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
      if (n->m_parents && includeParents)
      {
        QListIterator<DotNode> li(*n->m_parents);
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
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    if (childQueue.count()>0)
    {
      DotNode *n = childQueue.take(0);
      int distance = n->distance();
      if (!n->isVisible() && distance<maxDistance) // not yet processed
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
        if (n->m_children)
        {
          QListIterator<DotNode> li(*n->m_children);
          DotNode *dn;
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
      if ((!n->isVisible() || firstNode) && n->distance()<maxDistance) // not yet processed
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
        if (n->m_parents)
        {
          QListIterator<DotNode> li(*n->m_parents);
          DotNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            parentQueue.append(dn);
          }
        }
      }
    }
  }
  if (Config_getBool("UML_LOOK")) return FALSE; // UML graph are always top to bottom
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

void DotClassGraph::buildGraph(ClassDef *cd,DotNode *n,bool base,int distance)
{
  //printf("DocClassGraph::buildGraph(%s,distance=%d,base=%d)\n",
  //    cd->name().data(),distance,base);
  // ---- Add inheritance relations

  if (m_graphType == DotNode::Inheritance || m_graphType==DotNode::Collaboration)
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
  if (m_graphType == DotNode::Collaboration)
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

  // ---- Add template instantiation relations

  static bool templateRelations = Config_getBool("TEMPLATE_RELATIONS");
  if (templateRelations)
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
            addClass(templMaster,n,EdgeInfo::Orange,cli.currentKey(),0,
                0,TRUE,distance);
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
          addClass(templInstance,n,EdgeInfo::Orange,cli.currentKey(),0,
              0,FALSE,distance);
        }
      }
    }
  }
}

DotClassGraph::DotClassGraph(ClassDef *cd,DotNode::GraphType t)
{
  //printf("--------------- DotClassGraph::DotClassGraph `%s'\n",cd->displayName().data());
  m_graphType = t;
  QCString tmp_url="";
  if (cd->isLinkable() && !cd->isHidden()) 
  {
    tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
  }
  QCString className = cd->displayName();
  QCString tooltip = cd->briefDescriptionAsTooltip();
  m_startNode = new DotNode(m_curNodeNumber++,
                            className,
                            tooltip,
                            tmp_url.data(),
                            TRUE,                      // is a root node
                            cd
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(className,m_startNode);

  //printf("Root node %s\n",cd->name().data());
  //if (m_recDepth>0) 
  //{
    buildGraph(cd,m_startNode,TRUE,1);
    if (t==DotNode::Inheritance) buildGraph(cd,m_startNode,FALSE,1);
  //}

  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (t==DotNode::Inheritance && m_startNode->m_parents!=0)
  //  directChildNodes+=m_startNode->m_parents->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  //openNodeQueue.append(m_startNode);
  m_lrRank = determineVisibleNodes(m_startNode,maxNodes,t==DotNode::Inheritance);
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue,t==DotNode::Inheritance);

  m_diskName = cd->getFileBase().copy();
}

bool DotClassGraph::isTrivial() const
{
  if (m_graphType==DotNode::Inheritance)
    return m_startNode->m_children==0 && m_startNode->m_parents==0;
  else
    return m_startNode->m_children==0;
}

bool DotClassGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = 0;
  numNodes+= m_startNode->m_children ? m_startNode->m_children->count() : 0;
  if (m_graphType==DotNode::Inheritance)
  {
    numNodes+= m_startNode->m_parents ? m_startNode->m_parents->count() : 0;
  }
  return numNodes>=maxNodes;
}

DotClassGraph::~DotClassGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

/*! Computes a 16 byte md5 checksum for a given dot graph.
 *  The md5 checksum is returned as a 32 character ASCII string.
 */
QCString computeMd5Signature(DotNode *root,
                   DotNode::GraphType gt,
                   GraphOutputFormat format,
                   bool lrRank,
                   bool renderParents,
                   bool backArrows,
                   QCString &graphStr
                  )
{
  bool reNumber=TRUE;
    
  //printf("computeMd5Signature\n");
  QGString buf;
  FTextStream md5stream(&buf);
  //md5stream.setEncoding(md5stream.UnicodeUTF8);
  writeGraphHeader(md5stream);
  if (lrRank)
  {
    md5stream << "  rankdir=LR;" << endl;
  }
  root->clearWriteFlag();
  root->write(md5stream, 
      gt,
      format,
      gt!=DotNode::CallGraph && gt!=DotNode::Dependency,
      TRUE,
      backArrows,
      reNumber);
  if (renderParents && root->m_parents) 
  {
    QListIterator<DotNode>  dnli(*root->m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (pn->isVisible()) 
      {
        root->writeArrow(md5stream,                              // stream
            gt,                                                  // graph type
            format,                                              // output format
            pn,                                                  // child node
            pn->m_edgeInfo->at(pn->m_children->findRef(root)),   // edge info
            FALSE,                                               // topDown?
            backArrows,                                          // point back?
            reNumber                                             // renumber nodes
            );
      }
      pn->write(md5stream,      // stream
                gt,             // graph type
                format,         // output format
                TRUE,           // topDown?
                FALSE,          // toChildren?
                backArrows,     // backward pointing arrows?
                reNumber        // renumber nodes?
               );
    }
  }
  writeGraphFooter(md5stream);
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)buf.data(),buf.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  if (reNumber)
  {
    resetReNumbering();
  }
  graphStr=buf.data();
  //printf("md5: %s | file: %s\n",sigStr,baseName.data());
  return sigStr;
}

static bool updateDotGraph(DotNode *root,
                           DotNode::GraphType gt,
                           const QCString &baseName,
                           GraphOutputFormat format,
                           bool lrRank,
                           bool renderParents,
                           bool backArrows
                          )
{
  QCString theGraph;
  // TODO: write graph to theGraph, then compute md5 checksum
  QCString md5 = computeMd5Signature(
                   root,gt,format,lrRank,renderParents,backArrows,theGraph);
  if (checkAndUpdateMd5Signature(baseName,md5)) // graph needs to be regenerated
  {
    QFile f;
    f.setName(baseName+".dot");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      //t.setEncoding(t.UnicodeUTF8);
      t << theGraph;
    }
    return TRUE;
  }
  return FALSE;
}

QCString DotClassGraph::diskName() const
{
  QCString result=m_diskName.copy();
  switch (m_graphType)
  {
    case DotNode::Collaboration:
      result+="_coll_graph";
      break;
    //case Interface:
    //  result+="_intf_graph";
    //  break;
    case DotNode::Inheritance:
      result+="_inherit_graph";
      break;
    default:
      ASSERT(0);
      break;
  }
  return result;
}

QCString DotClassGraph::writeGraph(FTextStream &out,
                               GraphOutputFormat format,
                               const char *path,
                               const char *relPath,
                               bool /*isTBRank*/,
                               bool generateImageMap) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString baseName;
  QCString mapName;
  switch (m_graphType)
  {
    case DotNode::Collaboration:
      mapName="coll_map";
      break;
    //case Interface:
    //  mapName="intf_map";
    //  break;
    case DotNode::Inheritance:
      mapName="inherit_map";
      break;
    default:
      ASSERT(0);
      break;
  }
  baseName = convertNameToFile(diskName());
  QCString absBaseName = QCString(d.absPath().data())+"/"+baseName;

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");

  if (updateDotGraph(m_startNode,
                 m_graphType,
                 absBaseName,
                 format,
                 m_lrRank,
                 m_graphType==DotNode::Inheritance,
                 TRUE
                )
      )
  {
    if (format==BITMAP) // run dot to create a bitmap image
    {
      QCString dotArgs(maxCmdLine);
      QCString absImgName = absBaseName+"."+imgExt;

      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob(imgExt,absImgName);
      if (generateImageMap) dotRun.addJob(MAP_CMD,absBaseName+".map");
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
      checkDotResult(absImgName);
    }
    else if (format==EPS) // run dot to create a .eps image
    {
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob("ps",absBaseName+".eps");

      if (Config_getBool("USE_PDFLATEX"))
      {
        QCString epstopdfArgs(maxCmdLine);
        epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
            absBaseName.data(),absBaseName.data());
        dotRun.addPostProcessing("epstopdf",epstopdfArgs);
      }

      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
    }
    if (Config_getBool("DOT_CLEANUP")) d.remove(baseName+".dot");
  }
  Doxygen::indexList.addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap) // produce HTML to include the image
  {
    QCString mapLabel = escapeCharsInString(m_startNode->m_label,FALSE)+"_"+
                        escapeCharsInString(mapName,FALSE);
    out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
      << imgExt << "\" border=\"0\" usemap=\"#"
      << mapLabel << "\" alt=\"";
    switch (m_graphType)
    {
      case DotNode::Collaboration:
        out << "Collaboration graph";
        break;
      case DotNode::Inheritance:
        out << "Inheritance graph";
        break;
      default:
        ASSERT(0);
        break;
    }
    out << "\"/></div>" << endl;
    QGString tmpstr;
    FTextStream tmpout(&tmpstr);
    convertMapFile(tmpout,absBaseName+".map",relPath);
    if (!tmpstr.isEmpty())
    {
      out << "<map name=\"" << mapLabel << "\" id=\"" << mapLabel << "\">" << endl;
      out << tmpstr;
      out << "</map>" << endl;
    }
  }
  else if (format==EPS) // produce tex to include the .eps image
  {
      int width=420,height=600;
      if (!readBoundingBoxEPS(absBaseName+".eps",&width,&height))
      {
        err("Error: Could not extract bounding box from .eps!\n");
        unsetDotFontPath();
        return baseName;
      }
      //printf("Got EPS size %d,%d\n",width,height);
      int maxWidth  = 400;  /* approx. page width in points, excl. margins */
      int maxHeight = 400;  /* approx. page height in points, excl. margins */ 
      out << "\\nopagebreak\n"
        "\\begin{figure}[H]\n"
        "\\begin{center}\n"
        "\\leavevmode\n";
      if (width>maxWidth)
      {
        out << "\\includegraphics[width=" << maxWidth << "pt]";
      }
      else if (height>maxHeight)
      {
        out << "\\includegraphics[height=" << maxHeight << "pt]";
      }
      else
      {
        out << "\\includegraphics[width=" << width << "pt]";
      }
      out << "{" << baseName << "}\n"
        "\\end{center}\n"
        "\\end{figure}\n";
  }
  unsetDotFontPath();

  return baseName;
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

void DotClassGraph::writeDEF(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDEF(t);
  }
}

//--------------------------------------------------------------------

int DotInclDepGraph::m_curNodeNumber = 0;

void DotInclDepGraph::buildGraph(DotNode *n,FileDef *fd,int distance)
{
  QList<IncludeInfo> *includeFiles = 
     m_inverse ? fd->includedByFileList() : fd->includeFileList();
  if (includeFiles)
  {
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
        doc = bfd->isLinkable() && !bfd->isHidden();
        src = bfd->generateSourceFile();
      }
      if (doc || src || !Config_getBool("HIDE_UNDOC_RELATIONS"))
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
              m_curNodeNumber++, // n
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
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<maxDistance) // not yet processed
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


DotInclDepGraph::DotInclDepGraph(FileDef *fd,bool inverse)
{
  m_maxDistance = 0;
  m_inverse = inverse;
  ASSERT(fd!=0);
  m_diskName  = fd->getFileBase().copy();
  QCString tmp_url=fd->getReference()+"$"+fd->getFileBase();
  m_startNode = new DotNode(m_curNodeNumber++,
                            fd->docName(),
                            "",
                            tmp_url.data(),
                            TRUE     // root node
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(fd->absFilePath(),m_startNode);
  buildGraph(m_startNode,fd,1);

  static int nodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int maxNodes = nodes;
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

QCString DotInclDepGraph::writeGraph(FTextStream &out,
                                 GraphOutputFormat format,
                                 const char *path,
                                 const char *relPath,
                                 bool generateImageMap
                                ) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString baseName=m_diskName;
  if (m_inverse) baseName+="_dep";
  baseName+="_incl";
  baseName=convertNameToFile(baseName);
  QCString mapName=escapeCharsInString(m_startNode->m_label,FALSE);
  if (m_inverse) mapName+="dep";
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");

  QCString absBaseName = QCString(d.absPath())+"/"+baseName;
  QCString absMapName  = QCString(d.absPath())+"/"+mapName;

  if (updateDotGraph(m_startNode,
                 DotNode::Dependency,
                 absBaseName,
                 format,
                 FALSE,        // lrRank
                 FALSE,        // renderParents
                 m_inverse     // backArrows
                )
      )
  {
    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      QCString absImgName=absBaseName+"."+imgExt;
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob(imgExt,absImgName);
      if (generateImageMap) dotRun.addJob(MAP_CMD,absBaseName+".map");
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
      checkDotResult(absImgName);
    }
    else if (format==EPS)
    {
      // run dot to create a .eps image
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob("ps",absBaseName+".eps");
      if (Config_getBool("USE_PDFLATEX"))
      {
        QCString epstopdfArgs(maxCmdLine);
        epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
            absBaseName.data(),absBaseName.data());
        dotRun.addPostProcessing("epstopdf",epstopdfArgs);
      }
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
    }
  }
  Doxygen::indexList.addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
        << imgExt << "\" border=\"0\" usemap=\"#"
        << mapName << "_map\" alt=\"\"/>";
    out << "</div>" << endl;
    QGString tmpstr;
    FTextStream tmpout(&tmpstr);
    //tmpout.setEncoding(tmpout.UnicodeUTF8);
    convertMapFile(tmpout,absBaseName+".map",relPath);
    if (!tmpstr.isEmpty())
    {
      out << "<map name=\"" << mapName << "_map\" id=\"" << mapName << "\">" << endl;
      out << tmpstr;
      out << "</map>" << endl;
    }
  }
  else if (format==EPS)
  {
    int width,height;
    if (!readBoundingBoxEPS(absBaseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      unsetDotFontPath();
      return baseName;
    }
    int maxWidth = 420; /* approx. page width in points */

    out << "\\nopagebreak\n"
           "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
                                      << "pt]{" << baseName << "}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }

  if (Config_getBool("DOT_CLEANUP")) d.remove(baseName+".dot");

  unsetDotFontPath();
  return baseName;
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

bool DotInclDepGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = m_startNode->m_children ? m_startNode->m_children->count() : 0;
  return numNodes>=maxNodes;
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

//-------------------------------------------------------------

int DotCallGraph::m_curNodeNumber = 0;

void DotCallGraph::buildGraph(DotNode *n,MemberDef *md,int distance)
{
  LockingPtr<MemberSDict> refs = m_inverse ? md->getReferencedByMembers() : md->getReferencesMembers();
  if (!refs.isNull())
  {
    MemberSDict::Iterator mri(*refs);
    MemberDef *rmd;
    for (;(rmd=mri.current());++mri)
    {
      if (rmd->isFunction() || rmd->isSlot() || rmd->isSignal())
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
          if (Config_getBool("HIDE_SCOPE_NAMES"))
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
              m_curNodeNumber++,
              linkToText(name,FALSE),
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
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<maxDistance) // not yet processed
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

void DotCallGraph::determineTruncatedNodes(QList<DotNode> &queue)
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



DotCallGraph::DotCallGraph(MemberDef *md,bool inverse)
{
  m_maxDistance = 0;
  m_inverse = inverse;
  m_diskName = md->getOutputFileBase()+"_"+md->anchor();
  m_scope    = md->getOuterScope();
  QCString uniqueId;
  uniqueId = md->getReference()+"$"+
             md->getOutputFileBase()+"#"+md->anchor();
  QCString name;
  if (Config_getBool("HIDE_SCOPE_NAMES"))
  {
    name = md->name();
  }
  else
  {
    name = md->qualifiedName();
  }
  m_startNode = new DotNode(m_curNodeNumber++,
                            linkToText(name,FALSE),
                            "",
                            uniqueId.data(),
                            TRUE     // root node
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(uniqueId,m_startNode);
  buildGraph(m_startNode,md,1);

  static int nodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int maxNodes = nodes;
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

DotCallGraph::~DotCallGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotCallGraph::writeGraph(FTextStream &out, GraphOutputFormat format,
                        const char *path,const char *relPath,bool generateImageMap) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString baseName = m_diskName + (m_inverse ? "_icgraph" : "_cgraph");
  QCString mapName=baseName;
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");

  QCString absBaseName = QCString(d.absPath())+"/"+baseName;

  if (updateDotGraph(m_startNode,
                 DotNode::CallGraph,
                 absBaseName,
                 format,
                 TRUE,         // lrRank
                 FALSE,        // renderParents
                 m_inverse     // backArrows
                )
      )
  {
    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      QCString absImgName=absBaseName+"."+imgExt;
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob(imgExt,absImgName);
      if (generateImageMap) dotRun.addJob(MAP_CMD,absBaseName+".map");
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
      checkDotResult(absImgName);
    }
    else if (format==EPS)
    {
      // run dot to create a .eps image
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob("ps",absBaseName+".eps");
      if (Config_getBool("USE_PDFLATEX"))
      {
        QCString epstopdfArgs(maxCmdLine);
        epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
            absBaseName.data(),absBaseName.data());
        dotRun.addPostProcessing("epstopdf",epstopdfArgs);
      }
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
    }
  }
  Doxygen::indexList.addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
        << imgExt << "\" border=\"0\" usemap=\"#"
        << mapName << "_map\" alt=\"";
    out << "\"/>";
    out << "</div>" << endl;
    QGString tmpstr;
    FTextStream tmpout(&tmpstr);
    //tmpout.setEncoding(tmpout.UnicodeUTF8);
    convertMapFile(tmpout,absBaseName+".map",relPath);
    if (!tmpstr.isEmpty())
    {
      out << "<map name=\"" << mapName << "_map\" id=\"" << mapName << "\">" << endl;
      out << tmpstr;
      out << "</map>" << endl;
    }
  }
  else if (format==EPS)
  {
    int width,height;
    if (!readBoundingBoxEPS(absBaseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      unsetDotFontPath();
      return baseName;
    }
    int maxWidth = 420; /* approx. page width in points */

    out << "\\nopagebreak\n"
           "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
                                      << "pt]{" << baseName << "}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }

  if (Config_getBool("DOT_CLEANUP")) d.remove(baseName+".dot");

  unsetDotFontPath();
  return baseName;
}

bool DotCallGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

bool DotCallGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = m_startNode->m_children ? m_startNode->m_children->count() : 0;
  return numNodes>=maxNodes;
}

//-------------------------------------------------------------

DotDirDeps::DotDirDeps(DirDef *dir) : m_dir(dir)
{
}

DotDirDeps::~DotDirDeps()
{
}

QCString DotDirDeps::writeGraph(FTextStream &out,
                            GraphOutputFormat format,
                            const char *path,
                            const char *relPath,
                            bool generateImageMap) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString baseName=m_dir->getOutputFileBase()+"_dep";
  QCString mapName=escapeCharsInString(baseName,FALSE);
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");

  QCString absBaseName = QCString(d.absPath())+"/"+baseName;

  // TODO: create check, update md5 checksum
  {
    QFile f(absBaseName+".dot");
    if (!f.open(IO_WriteOnly))
    {
      err("Cannot create file %s.dot for writing!\n",baseName.data());
    }
    FTextStream t(&f);
    //t.setEncoding(t.UnicodeUTF8);
    m_dir->writeDepGraph(t);
    f.close();

    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      QCString absImgName=absBaseName+"."+imgExt;
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob(imgExt,absImgName);
      if (generateImageMap) dotRun.addJob(MAP_CMD,absBaseName+".map");
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
      checkDotResult(absImgName);
    }
    else if (format==EPS)
    {
      // run dot to create a .eps image
      DotRunner dotRun(absBaseName+".dot");
      dotRun.addJob("ps",absBaseName+".eps");
      if (Config_getBool("USE_PDFLATEX"))
      {
        QCString epstopdfArgs(maxCmdLine);
        epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
            absBaseName.data(),absBaseName.data());
        dotRun.addPostProcessing("epstopdf",epstopdfArgs);
      }
      if (!dotRun.run())
      {
        unsetDotFontPath();
        return baseName;
      }
    }
  }
  Doxygen::indexList.addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
        << imgExt << "\" border=\"0\" usemap=\"#"
        << mapName << "_map\" alt=\"";
    out << convertToXML(m_dir->displayName());
    out << "\"/>";
    out << "</div>" << endl;
    QGString tmpstr;
    FTextStream tmpout(&tmpstr);
    //tmpout.setEncoding(tmpout.UnicodeUTF8);
    convertMapFile(tmpout,absBaseName+".map",relPath,TRUE);
    if (!tmpstr.isEmpty())
    {
      out << "<map name=\"" << mapName << "_map\" id=\"" << mapName << "\">" << endl;
      out << tmpstr;
      out << "</map>" << endl;
    }
    else
    {
      //printf("Map is empty!\n");
    }
    //thisDir.remove(baseName+".map");
  }
  else if (format==EPS)
  {
    int width,height;
    if (!readBoundingBoxEPS(absBaseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      unsetDotFontPath();
      return baseName;
    }
    int maxWidth = 420; /* approx. page width in points */

    out << "\\nopagebreak\n"
           "\\begin{figure}[H]\n"
           "\\begin{center}\n"
           "\\leavevmode\n"
           "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
                                      << "pt]{" << baseName << "}\n"
           "\\end{center}\n"
           "\\end{figure}\n";
  }

  if (Config_getBool("DOT_CLEANUP")) d.remove(baseName+".dot");

  unsetDotFontPath();
  return baseName;
}

bool DotDirDeps::isTrivial() const
{
  return m_dir->depGraphIsTrivial();
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
  FTextStream dotText(&dotFile); 
  writeGraphHeader(dotText);
  dotText << "  Node9 [shape=\"box\",label=\"Inherited\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",fillcolor=\"grey75\",style=\"filled\" fontcolor=\"black\"];\n";
  dotText << "  Node10 -> Node9 [dir=back,color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node10 [shape=\"box\",label=\"PublicBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classPublicBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node11 -> Node10 [dir=back,color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node11 [shape=\"box\",label=\"Truncated\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"red\",URL=\"$classTruncated" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node13 -> Node9 [dir=back,color=\"darkgreen\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node13 [shape=\"box\",label=\"ProtectedBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classProtectedBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node14 -> Node9 [dir=back,color=\"firebrick4\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node14 [shape=\"box\",label=\"PrivateBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classPrivateBase" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node15 -> Node9 [dir=back,color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node15 [shape=\"box\",label=\"Undocumented\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"grey75\"];\n";
  dotText << "  Node16 -> Node9 [dir=back,color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node16 [shape=\"box\",label=\"Templ< int >\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node17 -> Node16 [dir=back,color=\"orange\",fontsize=\"" << FONTSIZE << "\",style=\"dashed\",label=\"< int >\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node17 [shape=\"box\",label=\"Templ< T >\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  dotText << "  Node18 -> Node9 [dir=back,color=\"darkorchid3\",fontsize=\"" << FONTSIZE << "\",style=\"dashed\",label=\"m_usedClass\",fontname=\"" << FONTNAME << "\"];\n";
  dotText << "  Node18 [shape=\"box\",label=\"Used\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classUsed" << Doxygen::htmlFileExtension << "\"];\n";
  writeGraphFooter(dotText);
  dotFile.close();

  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  // run dot to generate the a bitmap image from the graph
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = "graph_legend."+imgExt;
  QCString absImgName = QCString(d.absPath())+"/"+ imgName;

  DotRunner dotRun(d.absPath()+"/graph_legend.dot");
  dotRun.addJob(imgExt,absImgName);
  if (!dotRun.run())
  {
    unsetDotFontPath();
    return;
  }
  checkDotResult(absImgName);
  Doxygen::indexList.addImageFile(imgName);
  unsetDotFontPath();
}

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format)
{
  QDir d(outDir);
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",outDir); exit(1);
  }
  setDotFontPath("");

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = (QCString)outFile+"."+imgExt;
  QCString absImgName = QCString(d.absPath())+"/"+imgName;
  QCString absOutFile = QCString(d.absPath())+"/"+outFile;

  DotRunner dotRun(inFile);
  if (format==BITMAP)
    dotRun.addJob(imgExt,absImgName);
  else // format==EPS
    dotRun.addJob("ps",absOutFile+".eps");

  if ( (format==EPS) && (Config_getBool("USE_PDFLATEX")) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         absOutFile.data(),absOutFile.data());
    dotRun.addPostProcessing("epstopdf",epstopdfArgs);
  }

  if (!dotRun.run())
  {
     unsetDotFontPath();
     return;
  }

  if (format==BITMAP) checkDotResult(absImgName);
  Doxygen::indexList.addImageFile(imgName);

  unsetDotFontPath();
}

 
/*! Marco Dalla Gasperina [marcodg@attbi.com] added this to allow
 *  dotfiles to generate image maps.
 *  \param inFile just the basename part of the filename
 *  \param outDir output directory
 *  \param relPath relative path the to root of the output dir
 *  \param context the scope in which this graph is found (for resolving links)
 *  \returns a string which is the HTML image map (without the \<map\>\</map\>)
 */
QCString getDotImageMapFromFile(const QCString& inFile, const QCString& outDir,
                               const QCString &relPath,const QCString &context)
{
  QString outFile = inFile + ".map";

  QDir d(outDir);
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",outDir.data()); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString absInFile  = QCString(d.absPath())+"/"+inFile.data();
  QCString absOutFile = QCString(d.absPath())+"/"+outFile.data();

  DotRunner dotRun(absInFile);
  dotRun.addJob(MAP_CMD,absOutFile);
  if (!dotRun.run())
  {
    unsetDotFontPath();
    return "";
  }

  QGString result;
  FTextStream tmpout(&result);
  //tmpout.setEncoding(tmpout.UnicodeUTF8);
  convertMapFile(tmpout, absOutFile, relPath ,TRUE, context);
  d.remove(outFile);

  unsetDotFontPath();
  return result.data();
}
// end MDG mods

//-------------------------------------------------------------

DotGroupCollaboration::DotGroupCollaboration(GroupDef* gd)
{
    m_curNodeId = 0;
    QCString tmp_url = gd->getReference()+"$"+gd->getOutputFileBase();
    m_usedNodes = new QDict<DotNode>(1009);
    m_rootNode = new DotNode(m_curNodeId++, gd->groupTitle(), "", tmp_url, TRUE );
    m_rootNode->markAsVisible();
    m_usedNodes->insert(gd->name(), m_rootNode );
    m_edges.setAutoDelete(TRUE);

    m_diskName = gd->getOutputFileBase();

    buildGraph( gd );
}

DotGroupCollaboration::~DotGroupCollaboration()
{
  delete m_usedNodes;
}

void DotGroupCollaboration::buildGraph(GroupDef* gd)
{
  QCString tmp_url;
  //===========================
  // hierarchy.

  // Write parents
  LockingPtr<GroupList> groups = gd->partOfGroups();
  if ( groups!=0 )
  {
    GroupListIterator gli(*groups);
    GroupDef *d;
    for (gli.toFirst();(d=gli.current());++gli)
    {
      DotNode* nnode = m_usedNodes->find(d->name());
      if ( !nnode )
      { // add node
        tmp_url = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, d->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
      }
      tmp_url = "";
      addEdge( nnode, m_rootNode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  // Add subgroups
  if ( gd->getSubGroups() && gd->getSubGroups()->count() )
  {
    QListIterator<GroupDef> defli(*gd->getSubGroups());
    GroupDef *def;
    for (;(def=defli.current());++defli)
    {
      DotNode* nnode = m_usedNodes->find(def->name());
      if ( !nnode )
      { // add node
        tmp_url = def->getReference()+"$"+def->getOutputFileBase();
        QCString tooltip = def->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, def->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(def->name(), nnode );
      }
      tmp_url = "";
      addEdge( m_rootNode, nnode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  //=======================
  // Write collaboration

  // Add members
  addMemberList( gd->getMemberList(MemberList::allMembersList) );

  // Add classes
  if ( gd->getClasses() && gd->getClasses()->count() )
  {
    ClassSDict::Iterator defli(*gd->getClasses());
    ClassDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tclass );          
    }
  }

  // Add namespaces
  if ( gd->getNamespaces() && gd->getNamespaces()->count() )
  {
    NamespaceSDict::Iterator defli(*gd->getNamespaces());
    NamespaceDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tnamespace );          
    }
  }

  // Add files
  if ( gd->getFiles() && gd->getFiles()->count() )
  {
    QListIterator<FileDef> defli(*gd->getFiles());
    FileDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tfile );          
    }
  }

  // Add pages
  if ( gd->getPages() && gd->getPages()->count() )
  {
    PageSDict::Iterator defli(*gd->getPages());
    PageDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tpages );          
    }
  }

  // Add directories
  if ( gd->getDirs() && gd->getDirs()->count() )
  {
    QListIterator<DirDef> defli(*gd->getDirs());
    DirDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tdir );          
    }
  }
}

void DotGroupCollaboration::addMemberList( MemberList* ml )
{
  if ( !( ml && ml->count()) ) return;
  MemberListIterator defli(*ml);
  MemberDef *def;
  for (;(def=defli.current());++defli)
  {
    QCString tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension
      +"#"+def->anchor();
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tmember );
  }
}

DotGroupCollaboration::Edge* DotGroupCollaboration::addEdge( 
    DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
    const QCString& _label, const QCString& _url )
{
  // search a existing link.
  QListIterator<Edge> lli(m_edges);
  Edge* newEdge = 0;
  for ( lli.toFirst(); (newEdge=lli.current()); ++lli)
  {
    if ( newEdge->pNStart==_pNStart && 
         newEdge->pNEnd==_pNEnd &&
         newEdge->eType==_eType 
       )
    { // edge already found
      break;
    }
  }
  if ( newEdge==0 ) // new link
  {
    newEdge = new Edge(_pNStart,_pNEnd,_eType);
    m_edges.append( newEdge );
  } 

  if (!_label.isEmpty())
  {
    newEdge->links.append(new Link(_label,_url));
  }

  return newEdge;
}

void DotGroupCollaboration::addCollaborationMember( 
    Definition* def, QCString& url, EdgeType eType )
{
  // Create group nodes
  if ( !def->partOfGroups() )
    return;
  GroupListIterator gli(*def->partOfGroups());
  GroupDef *d;
  QCString tmp_str;
  for (;(d=gli.current());++gli)
  {
    DotNode* nnode = m_usedNodes->find(d->name());
    if ( nnode != m_rootNode )
    {
      if ( nnode==0 )
      { // add node
        tmp_str = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, d->groupTitle(), tooltip, tmp_str );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
      }
      tmp_str = def->qualifiedName();
      addEdge( m_rootNode, nnode, eType, tmp_str, url );
    }
  }
}


QCString DotGroupCollaboration::writeGraph( FTextStream &t, GraphOutputFormat format,
    const char *path, const char *relPath,
    bool writeImageMap) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Error: Output dir %s does not exist!\n",path); exit(1);
  }
  setDotFontPath(d.absPath());

  QCString baseName = m_diskName;
  QCString absBaseName = QCString(d.absPath())+"/"+baseName;

  QFile dotfile(absBaseName+".dot");
  if (dotfile.open(IO_WriteOnly))
  {
    FTextStream tdot(&dotfile);
    //tdot.setEncoding(tdot.UnicodeUTF8);
    writeGraphHeader(tdot);

    // clean write flags
    QDictIterator<DotNode> dni(*m_usedNodes);
    DotNode *pn;
    for (dni.toFirst();(pn=dni.current());++dni)
      pn->clearWriteFlag();

    // write other nodes.
    for (dni.toFirst();(pn=dni.current());++dni)
    {
      pn->write(tdot,DotNode::Inheritance,format,TRUE,FALSE,FALSE,FALSE);
    }

    // write edges
    QListIterator<Edge> eli(m_edges);
    Edge* edge;
    for (eli.toFirst();(edge=eli.current());++eli)
    {
      edge->write( tdot );
    }

    writeGraphFooter(tdot);
    dotfile.close();
  }

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  if (format==BITMAP) // run dot to create a bitmap image
  {
    QCString dotArgs(maxCmdLine);
    QCString imgName = baseName+"."+imgExt;
    QCString mapName=baseName+".map";

    QCString absImgName = QCString(d.absPath())+"/"+imgName;
    QCString absMapName = QCString(d.absPath())+"/"+mapName;

    DotRunner dotRun(absBaseName+".dot");
    dotRun.addJob(imgExt,absImgName);
    if (writeImageMap) dotRun.addJob(MAP_CMD,absMapName);
    if (!dotRun.run())
    {
      unsetDotFontPath();
      return baseName;
    }

    if (writeImageMap)
    {
      QCString mapLabel = escapeCharsInString(baseName,FALSE);
      t << "<center><table><tr><td><img src=\"" << relPath << imgName
        << "\" border=\"0\" alt=\"\" usemap=\"#" 
        << mapLabel << "_map\"/>" << endl;
      t << "<map name=\"" << mapLabel << "_map\" id=\"" << mapLabel << "\">" << endl;
      convertMapFile(t,absMapName,relPath);
      t << "</map></td></tr></table></center>" << endl;
    }
  }
  else if (format==EPS)
  {
    DotRunner dotRun(absBaseName+".dot");
    dotRun.addJob("ps",absBaseName+".eps");
    if (Config_getBool("USE_PDFLATEX"))
    {
      QCString epstopdfArgs(maxCmdLine);
      epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
          absBaseName.data(),absBaseName.data());
      dotRun.addPostProcessing("epstopdf",epstopdfArgs);
    }

    if (!dotRun.run())
    {
      unsetDotFontPath();
      return baseName;
    }

    int width,height;
    if (!readBoundingBoxEPS(absBaseName+".eps",&width,&height))
    {
      err("Error: Could not extract bounding box from .eps!\n");
      unsetDotFontPath();
      return baseName;
    }
    int maxWidth = 420; /* approx. page width in points */
    t << "\\nopagebreak\n"
         "\\begin{figure}[H]\n"
         "\\begin{center}\n"
         "\\leavevmode\n"
         "\\includegraphics[width=" << QMIN(width/2,maxWidth) 
          << "pt]{" << baseName << "}\n"
         "\\end{center}\n"
         "\\end{figure}\n";
  }
  if (Config_getBool("DOT_CLEANUP"))
  {
    d.remove(baseName+".dot");
  }

  unsetDotFontPath();
  return baseName;
}

void DotGroupCollaboration::Edge::write( FTextStream &t ) const
{
  const char* linkTypeColor[] = {
    "darkorchid3"
      ,"orange"
      ,"blueviolet"
      ,"darkgreen"   
      ,"firebrick4"  
      ,"grey75"
      ,"midnightblue"
  };
  QCString arrowStyle = "dir=\"none\", style=\"dashed\"";
  t << "  Node" << pNStart->number();
  t << "->";
  t << "Node" << pNEnd->number();

  t << " [shape=plaintext";
  if (links.count()>0) // there are links
  {
    t << ", ";
    // HTML-like edge labels crash on my Mac with Graphviz 2.0! and
    // are not supported by older version of dot.
    //
    //t << label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\">";
    //QListIterator<Link> lli(links);
    //Link *link;
    //for( lli.toFirst(); (link=lli.current()); ++lli)
    //{
    //  t << "<TR><TD";
    //  if ( !link->url.isEmpty() )
    //    t << " HREF=\"" << link->url << "\"";
    //  t << ">" << link->label << "</TD></TR>";
    //}
    //t << "</TABLE>>";

    t << "label=\"";
    QListIterator<Link> lli(links);
    Link *link;
    bool first=TRUE;
    int count=0;
    const int maxLabels = 10;
    for( lli.toFirst(); (link=lli.current()) && count<maxLabels; ++lli,++count)
    {
      if (first) first=FALSE; else t << "\\n"; 
      t << convertLabel(link->label);
    }
    if (count==maxLabels) t << "\\n...";
    t << "\"";

  }
  switch( eType )
  {
    case thierarchy :
      arrowStyle = "dir=\"back\", style=\"solid\"";
    default :
      t << ", color=\"" << linkTypeColor[(int)eType] << "\"";
      break;
  }
  t << ", " << arrowStyle;
  t << "];" << endl;
}

bool DotGroupCollaboration::isTrivial() const
{
  return m_usedNodes->count() <= 1;
}

void DotGroupCollaboration::writeGraphHeader(FTextStream &t) const
{
  t << "digraph structs" << endl;
  t << "{" << endl;
  if (Config_getBool("DOT_TRANSPARENT"))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << FONTNAME << "\",fontsize=\"" << FONTSIZE << "\","
    "labelfontname=\"" << FONTNAME << "\",labelfontsize=\"" << FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << FONTNAME << "\",fontsize=\"" << FONTSIZE << "\",shape=record];\n";
  t << "  rankdir=LR;\n";
}

void writeDotDirDepGraph(FTextStream &t,DirDef *dd)
{
    t << "digraph G {\n";
    if (Config_getBool("DOT_TRANSPARENT"))
    {
      t << "  bgcolor=transparent;\n";
    }
    t << "  compound=true\n";
    t << "  node [ fontsize=\"" << FONTSIZE << "\", fontname=\"" << FONTNAME << "\"];\n";
    t << "  edge [ labelfontsize=\"" << FONTSIZE << "\", labelfontname=\"" << FONTNAME << "\"];\n";

    QDict<DirDef> dirsInGraph(257);
    
    dirsInGraph.insert(dd->getOutputFileBase(),dd);
    if (dd->parent())
    {
      t << "  subgraph cluster" << dd->parent()->getOutputFileBase() << " {\n";
      t << "    graph [ bgcolor=\"#ddddee\", pencolor=\"black\", label=\"" 
        << dd->parent()->shortName() 
        << "\" fontname=\"" << FONTNAME << "\", fontsize=\"" << FONTSIZE << "\", URL=\"";
      t << dd->parent()->getOutputFileBase() << Doxygen::htmlFileExtension;
      t << "\"]\n";
    }
    if (dd->isCluster())
    {
      t << "  subgraph cluster" << dd->getOutputFileBase() << " {\n";
      t << "    graph [ bgcolor=\"#eeeeff\", pencolor=\"black\", label=\"\""
        << " URL=\"" << dd->getOutputFileBase() << Doxygen::htmlFileExtension 
        << "\"];\n";
      t << "    " << dd->getOutputFileBase() << " [shape=plaintext label=\"" 
        << dd->shortName() << "\"];\n";

      // add nodes for sub directories
      QListIterator<DirDef> sdi(dd->subDirs());
      DirDef *sdir;
      for (sdi.toFirst();(sdir=sdi.current());++sdi)
      {
        t << "    " << sdir->getOutputFileBase() << " [shape=box label=\"" 
          << sdir->shortName() << "\"";
        if (sdir->isCluster())
        {
          t << " color=\"red\"";
        }
        else
        {
          t << " color=\"black\"";
        }
        t << " fillcolor=\"white\" style=\"filled\"";
        t << " URL=\"" << sdir->getOutputFileBase() 
          << Doxygen::htmlFileExtension << "\"";
        t << "];\n";
        dirsInGraph.insert(sdir->getOutputFileBase(),sdir);
      }
      t << "  }\n";
    }
    else
    {
      t << "  " << dd->getOutputFileBase() << " [shape=box, label=\"" 
        << dd->shortName() << "\", style=\"filled\", fillcolor=\"#eeeeff\","
        << " pencolor=\"black\", URL=\"" << dd->getOutputFileBase() 
        << Doxygen::htmlFileExtension << "\"];\n";
    }
    if (dd->parent())
    {
      t << "  }\n";
    }

    // add nodes for other used directories
    QDictIterator<UsedDir> udi(*dd->usedDirs());
    UsedDir *udir;
    //printf("*** For dir %s\n",shortName().data());
    for (udi.toFirst();(udir=udi.current());++udi) 
      // for each used dir (=directly used or a parent of a directly used dir)
    {
      const DirDef *usedDir=udir->dir();
      DirDef *dir=dd;
      while (dir)
      {
        //printf("*** check relation %s->%s same_parent=%d !%s->isParentOf(%s)=%d\n",
        //    dir->shortName().data(),usedDir->shortName().data(),
        //    dir->parent()==usedDir->parent(),
        //    usedDir->shortName().data(),
        //    shortName().data(),
        //    !usedDir->isParentOf(this)
        //    );
        if (dir!=usedDir && dir->parent()==usedDir->parent() && 
            !usedDir->isParentOf(dd))
          // include if both have the same parent (or no parent)
        {
          t << "  " << usedDir->getOutputFileBase() << " [shape=box label=\"" 
            << usedDir->shortName() << "\"";
          if (usedDir->isCluster())
          {
            if (!Config_getBool("DOT_TRANSPARENT"))
            {
              t << " fillcolor=\"white\" style=\"filled\"";
            }
            t << " color=\"red\"";
          }
          t << " URL=\"" << usedDir->getOutputFileBase() 
            << Doxygen::htmlFileExtension << "\"];\n";
          dirsInGraph.insert(usedDir->getOutputFileBase(),usedDir);
          break;
        }
        dir=dir->parent();
      }
    }

    // add relations between all selected directories
    DirDef *dir;
    QDictIterator<DirDef> di(dirsInGraph);
    for (di.toFirst();(dir=di.current());++di) // foreach dir in the graph
    {
      QDictIterator<UsedDir> udi(*dir->usedDirs());
      UsedDir *udir;
      for (udi.toFirst();(udir=udi.current());++udi) // foreach used dir
      {
        const DirDef *usedDir=udir->dir();
        if ((dir!=dd || !udir->inherited()) &&     // only show direct dependendies for this dir
            (usedDir!=dd || !udir->inherited()) && // only show direct dependendies for this dir
            !usedDir->isParentOf(dir) &&             // don't point to own parent
            dirsInGraph.find(usedDir->getOutputFileBase())) // only point to nodes that are in the graph
        {
          QCString relationName;
          relationName.sprintf("dir_%06d_%06d",dir->dirCount(),usedDir->dirCount());
          if (Doxygen::dirRelations.find(relationName)==0)
          {
            // new relation
            Doxygen::dirRelations.append(relationName,
                new DirRelation(relationName,dir,udir));
          }
          int nrefs = udir->filePairs().count();
          t << "  " << dir->getOutputFileBase() << "->" 
                    << usedDir->getOutputFileBase();
          t << " [headlabel=\"" << nrefs << "\", labeldistance=1.5";
          t << " headhref=\"" << relationName << Doxygen::htmlFileExtension 
            << "\"];\n";
        }
      }
    }

    t << "}\n";
}

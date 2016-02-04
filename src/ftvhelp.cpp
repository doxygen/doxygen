/******************************************************************************
 * ftvhelp.cpp,v 1.0 2000/09/06 16:09:00
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
 * Original version contributed by Kenney Wong <kwong@ea.com>
 * Modified by Dimitri van Heesch
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 */

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>
#include <qfileinfo.h>

#include "ftvhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"
#include "htmlgen.h"
#include "layout.h"
#include "pagedef.h"
#include "docparser.h"
#include "htmldocvisitor.h"
#include "filedef.h"
#include "util.h"
#include "resourcemgr.h"

#define MAX_INDENT 1024

static int folderId=1;

struct FTVNode
{
  FTVNode(bool dir,const char *r,const char *f,const char *a,
          const char *n,bool sepIndex,bool navIndex,Definition *df)
    : isLast(TRUE), isDir(dir),ref(r),file(f),anchor(a),name(n), index(0),
      parent(0), separateIndex(sepIndex), addToNavIndex(navIndex),
      def(df) { children.setAutoDelete(TRUE); }
  int computeTreeDepth(int level) const;
  int numNodesAtLevel(int level,int maxLevel) const;
  bool isLast;
  bool isDir;
  QCString ref;
  QCString file;
  QCString anchor;
  QCString name;
  int index;
  QList<FTVNode> children;
  FTVNode *parent;
  bool separateIndex;
  bool addToNavIndex;
  Definition *def;
};

int FTVNode::computeTreeDepth(int level) const
{
  int maxDepth=level;
  QListIterator<FTVNode> li(children);
  FTVNode *n;
  for (;(n=li.current());++li)
  {
    if (n->children.count()>0)
    {
      int d = n->computeTreeDepth(level+1);
      if (d>maxDepth) maxDepth=d;
    }
  }
  return maxDepth;
}

int FTVNode::numNodesAtLevel(int level,int maxLevel) const
{
  int num=0;
  if (level<maxLevel)
  {
    num++; // this node
    QListIterator<FTVNode> li(children);
    FTVNode *n;
    for (;(n=li.current());++li)
    {
      num+=n->numNodesAtLevel(level+1,maxLevel);
    }
  }
  return num;
}

//----------------------------------------------------------------------------

/*! Constructs an ftv help object. 
 *  The object has to be \link initialize() initialized\endlink before it can 
 *  be used.
 */
FTVHelp::FTVHelp(bool TLI) 
{
  /* initial depth */
  m_indentNodes = new QList<FTVNode>[MAX_INDENT];
  m_indentNodes[0].setAutoDelete(TRUE);
  m_indent=0;
  m_topLevelIndex = TLI;
}

/*! Destroys the ftv help object. */
FTVHelp::~FTVHelp()
{
  delete[] m_indentNodes;
}

/*! This will create a folder tree view table of contents file (tree.js).
 *  \sa finalize()
 */
void FTVHelp::initialize()
{
}

/*! Finalizes the FTV help. This will finish and close the
 *  contents file (index.js).
 *  \sa initialize()
 */
void FTVHelp::finalize()
{
  generateTreeView();
}

/*! Increase the level of the contents hierarchy. 
 *  This will start a new sublist in contents file.
 *  \sa decContentsDepth()
 */
void FTVHelp::incContentsDepth()
{
  //printf("incContentsDepth() indent=%d\n",m_indent);
  m_indent++;
  ASSERT(m_indent<MAX_INDENT);
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the current sublist.
 *  \sa incContentsDepth()
 */
void FTVHelp::decContentsDepth()
{
  //printf("decContentsDepth() indent=%d\n",m_indent);
  ASSERT(m_indent>0);
  if (m_indent>0)
  {
    m_indent--;
    QList<FTVNode> *nl = &m_indentNodes[m_indent];
    FTVNode *parent = nl->getLast();
    if (parent)
    {
      QList<FTVNode> *children = &m_indentNodes[m_indent+1];
      while (!children->isEmpty())
      {
        parent->children.append(children->take(0));
      }
    }
  }
}

/*! Add a list item to the contents file.
 *  \param isDir TRUE if the item is a directory, FALSE if it is a text
 *  \param name The name of the item.
 *  \param ref  the URL of to the item.
 *  \param file the file containing the definition of the item
 *  \param anchor the anchor within the file.
 *  \param name the name of the item.
 *  \param separateIndex put the entries in a separate index file
 *  \param addToNavIndex add this entry to the quick navigation index
 *  \param def Definition corresponding to this entry
 */
void FTVHelp::addContentsItem(bool isDir,
                              const char *name,
                              const char *ref,
                              const char *file,
                              const char *anchor,
                              bool separateIndex,
                              bool addToNavIndex,
                              Definition *def
                              )
{
  //printf("%p: m_indent=%d addContentsItem(%s,%s,%s,%s)\n",this,m_indent,name,ref,file,anchor);
  QList<FTVNode> *nl = &m_indentNodes[m_indent];
  FTVNode *newNode = new FTVNode(isDir,ref,file,anchor,name,separateIndex,addToNavIndex,def);
  if (!nl->isEmpty())
  {
    nl->getLast()->isLast=FALSE;
  }
  nl->append(newNode);
  newNode->index = nl->count()-1;
  if (m_indent>0)
  {
    QList<FTVNode> *pnl = &m_indentNodes[m_indent-1];
    newNode->parent = pnl->getLast();
  }
  
}

static QCString node2URL(FTVNode *n,bool overruleFile=FALSE,bool srcLink=FALSE)
{
  QCString url = n->file;
  if (!url.isEmpty() && url.at(0)=='!')  // relative URL
  {
    // remove leading !
    url = url.mid(1);
  }
  else if (!url.isEmpty() && url.at(0)=='^') // absolute URL
  {
    // skip, keep ^ in the output
  }
  else // local file (with optional anchor)
  {
    if (overruleFile && n->def && n->def->definitionType()==Definition::TypeFile)
    {
      FileDef *fd = (FileDef*)n->def;
      if (srcLink)
      {
        url = fd->getSourceFileBase();
      }
      else
      {
        url = fd->getOutputFileBase();
      }
    }
    url+=Doxygen::htmlFileExtension;
    if (!n->anchor.isEmpty()) url+="#"+n->anchor;
  }
  return url;
}

QCString FTVHelp::generateIndentLabel(FTVNode *n,int level)
{
  QCString result;
  if (n->parent)
  {
    result=generateIndentLabel(n->parent,level+1);
  }
  result+=QCString().setNum(n->index)+"_";
  return result;
}

void FTVHelp::generateIndent(FTextStream &t, FTVNode *n,bool opened)
{
  int indent=0;
  FTVNode *p = n->parent;
  while (p) { indent++; p=p->parent; }
  if (n->isDir)
  {
    QCString dir = opened ? "&#9660;" : "&#9658;";
    t << "<span style=\"width:" << (indent*16) << "px;display:inline-block;\">&#160;</span>"
      << "<span id=\"arr_" << generateIndentLabel(n,0) << "\" class=\"arrow\" ";
    t << "onclick=\"toggleFolder('" << generateIndentLabel(n,0) << "')\"";
    t << ">" << dir
      << "</span>";
  }
  else
  {
    t << "<span style=\"width:" << ((indent+1)*16) << "px;display:inline-block;\">&#160;</span>";
  }
}

void FTVHelp::generateLink(FTextStream &t,FTVNode *n)
{
  //printf("FTVHelp::generateLink(ref=%s,file=%s,anchor=%s\n",
  //    n->ref.data(),n->file.data(),n->anchor.data());
  if (n->file.isEmpty()) // no link
  {
    t << "<b>" << convertToHtml(n->name) << "</b>";
  }
  else // link into other frame
  {
    if (!n->ref.isEmpty()) // link to entity imported via tag file
    {
      t << "<a class=\"elRef\" ";
      t << externalLinkTarget() << externalRef("",n->ref,FALSE);
    }
    else // local link
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    t << externalRef("",n->ref,TRUE);
    t << node2URL(n);
    if (m_topLevelIndex)
      t << "\" target=\"basefrm\">";
    else
      t << "\" target=\"_self\">";
    t << convertToHtml(n->name);
    t << "</a>";
    if (!n->ref.isEmpty())
    {
      t << "&#160;[external]";
    }
  }
}

static void generateBriefDoc(FTextStream &t,Definition *def)
{
  QCString brief = def->briefDescription(TRUE);
  //printf("*** %p: generateBriefDoc(%s)='%s'\n",def,def->name().data(),brief.data());
  if (!brief.isEmpty())
  {
    DocNode *root = validatingParseDoc(def->briefFile(),def->briefLine(),
        def,0,brief,FALSE,FALSE,0,TRUE,TRUE);
    QCString relPath = relativePathToRoot(def->getOutputFileBase());
    HtmlCodeGenerator htmlGen(t,relPath);
    HtmlDocVisitor *visitor = new HtmlDocVisitor(t,htmlGen,def);
    root->accept(visitor);
    delete visitor;
    delete root;
  }
}

void FTVHelp::generateTree(FTextStream &t, const QList<FTVNode> &nl,int level,int maxLevel,int &index)
{
  QListIterator<FTVNode> nli(nl);
  FTVNode *n;
  for (nli.toFirst();(n=nli.current());++nli)
  {
    t << "<tr id=\"row_" << generateIndentLabel(n,0) << "\"";
    if ((index&1)==0) // even row
      t << " class=\"even\"";
    if (level>=maxLevel) // item invisible by default
      t << " style=\"display:none;\"";
    else // item visible by default
      index++;
    t << "><td class=\"entry\">";
    bool nodeOpened = level+1<maxLevel;
    generateIndent(t,n,nodeOpened);
    if (n->isDir)
    {
      if (n->def && n->def->definitionType()==Definition::TypeGroup)
      {
        // no icon
      }
      else if (n->def && n->def->definitionType()==Definition::TypePage)
      {
        // no icon
      }
      else if (n->def && n->def->definitionType()==Definition::TypeNamespace)
      {
        t << "<span class=\"icona\"><span class=\"icon\">N</span></span>";
      }
      else if (n->def && n->def->definitionType()==Definition::TypeClass)
      {
        t << "<span class=\"icona\"><span class=\"icon\">C</span></span>";
      }
      else
      {
        t << "<span id=\"img_" << generateIndentLabel(n,0)
          << "\" class=\"iconf"
          << (nodeOpened?"open":"closed")
          << "\" onclick=\"toggleFolder('" << generateIndentLabel(n,0)
          << "')\">&#160;</span>";
      }
      generateLink(t,n);
      t << "</td><td class=\"desc\">";
      if (n->def)
      {
        generateBriefDoc(t,n->def);
      }
      t << "</td></tr>" << endl;
      folderId++;
      generateTree(t,n->children,level+1,maxLevel,index);
    }
    else // leaf node
    {
      FileDef *srcRef=0;
      if (n->def && n->def->definitionType()==Definition::TypeFile &&
          ((FileDef*)n->def)->generateSourceFile())
      {
        srcRef = (FileDef*)n->def;
      }
      if (srcRef)
      {
        t << "<a href=\"" << srcRef->getSourceFileBase()
          << Doxygen::htmlFileExtension 
          << "\">";
      }
      if (n->def && n->def->definitionType()==Definition::TypeGroup)
      {
        // no icon
      }
      else if (n->def && n->def->definitionType()==Definition::TypePage)
      {
        // no icon
      }
      else if (n->def && n->def->definitionType()==Definition::TypeNamespace)
      {
        t << "<span class=\"icona\"><span class=\"icon\">N</span></span>";
      }
      else if (n->def && n->def->definitionType()==Definition::TypeClass)
      {
        t << "<span class=\"icona\"><span class=\"icon\">C</span></span>";
      }
      else
      {
        t << "<span class=\"icondoc\"></span>";
      }
      if (srcRef)
      {
        t << "</a>";
      }
      generateLink(t,n);
      t << "</td><td class=\"desc\">";
      if (n->def)
      {
        generateBriefDoc(t,n->def);
      }
      t << "</td></tr>" << endl;
    }
  }
}

//-----------------------------------------------------------

struct NavIndexEntry
{
  NavIndexEntry(const QCString &u,const QCString &p) : url(u), path(p) {}
  QCString url;
  QCString path;
};

class NavIndexEntryList : public QList<NavIndexEntry>
{
  public:
    NavIndexEntryList() : QList<NavIndexEntry>() { setAutoDelete(TRUE); }
   ~NavIndexEntryList() {}
  private:
    int compareValues(const NavIndexEntry *item1,const NavIndexEntry *item2) const
    {
      // sort list based on url
      return qstrcmp(item1->url,item2->url);
    }
};

static QCString pathToNode(FTVNode *leaf,FTVNode *n)
{
  QCString result;
  if (n->parent)
  {
    result+=pathToNode(leaf,n->parent);
  }
  result+=QCString().setNum(n->index);
  if (leaf!=n) result+=",";
  return result;
}

static bool dupOfParent(const FTVNode *n)
{
  if (n->parent==0) return FALSE;
  if (n->file==n->parent->file) return TRUE;
  return FALSE;
}

static void generateJSLink(FTextStream &t,FTVNode *n)
{
  if (n->file.isEmpty()) // no link
  {
    t << "\"" << convertToJSString(n->name) << "\", null, ";
  }
  else // link into other page
  {
    t << "\"" << convertToJSString(n->name) << "\", \"";
    t << externalRef("",n->ref,TRUE);
    t << node2URL(n);
    t << "\", ";
  }
}

static QCString convertFileId2Var(const QCString &fileId)
{
  QCString varId = fileId;
  int i=varId.findRev('/');
  if (i>=0) varId = varId.mid(i+1);
  return substitute(varId,"-","_");
}

static bool generateJSTree(NavIndexEntryList &navIndex,FTextStream &t, 
                           const QList<FTVNode> &nl,int level,bool &first)
{
  static QCString htmlOutput = Config_getString(HTML_OUTPUT);
  QCString indentStr;
  indentStr.fill(' ',level*2);
  bool found=FALSE;
  QListIterator<FTVNode> nli(nl);
  FTVNode *n;
  for (nli.toFirst();(n=nli.current());++nli)
  {
    // terminate previous entry
    if (!first) t << "," << endl;
    first=FALSE;

    // start entry
    if (!found)
    {
      t << "[" << endl;
    }
    found=TRUE;

    if (n->addToNavIndex) // add entry to the navigation index
    {
      if (n->def && n->def->definitionType()==Definition::TypeFile)
      {
        FileDef *fd = (FileDef*)n->def;
        bool doc,src;
        doc = fileVisibleInIndex(fd,src);
        if (doc)
        {
          navIndex.append(new NavIndexEntry(node2URL(n,TRUE,FALSE),pathToNode(n,n)));
        }
        if (src)
        {
          navIndex.append(new NavIndexEntry(node2URL(n,TRUE,TRUE),pathToNode(n,n)));
        }
      }
      else
      {
        navIndex.append(new NavIndexEntry(node2URL(n),pathToNode(n,n)));
      }
    }

    if (n->separateIndex) // store items in a separate file for dynamic loading
    {
      bool firstChild=TRUE;
      t << indentStr << "  [ ";
      generateJSLink(t,n);
      if (n->children.count()>0) // write children to separate file for dynamic loading
      {
        QCString fileId = n->file;
        if (n->anchor)
        {
          fileId+="_"+n->anchor;
        }
        if (dupOfParent(n)) 
        {
          fileId+="_dup";
        }
        QFile f(htmlOutput+"/"+fileId+".js");
        if (f.open(IO_WriteOnly))
        {
          FTextStream tt(&f);
          tt << "var " << convertFileId2Var(fileId) << " =" << endl;
          generateJSTree(navIndex,tt,n->children,1,firstChild);
          tt << endl << "];"; 
        }
        t << "\"" << fileId << "\" ]";
      }
      else // no children
      {
        t << "null ]";
      }
    }
    else // show items in this file
    {
      bool firstChild=TRUE;
      t << indentStr << "  [ ";
      generateJSLink(t,n);
      bool emptySection = !generateJSTree(navIndex,t,n->children,level+1,firstChild);
      if (emptySection)
        t << "null ]";
      else
        t << endl << indentStr << "  ] ]"; 
    }
  }
  return found;
}

static void generateJSNavTree(const QList<FTVNode> &nodeList)
{
  QCString htmlOutput = Config_getString(HTML_OUTPUT);
  QFile f(htmlOutput+"/navtreedata.js");
  NavIndexEntryList navIndex;
  if (f.open(IO_WriteOnly) /*&& fidx.open(IO_WriteOnly)*/)
  {
    //FTextStream tidx(&fidx);
    //tidx << "var NAVTREEINDEX =" << endl;
    //tidx << "{" << endl;
    FTextStream t(&f);
    t << "var NAVTREE =" << endl;
    t << "[" << endl;
    t << "  [ ";
    QCString &projName = Config_getString(PROJECT_NAME);
    if (projName.isEmpty())
    {
      if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty()) // Use title of main page as root
      {
        t << "\"" << convertToJSString(Doxygen::mainPage->title()) << "\", ";
      }
      else // Use default section title as root
      {
        LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::MainPage);
        t << "\"" << convertToJSString(lne->title()) << "\", ";
      }
    }
    else // use PROJECT_NAME as root tree element
    {
      t << "\"" << convertToJSString(projName) << "\", ";
    }
    t << "\"index" << Doxygen::htmlFileExtension << "\", ";

    // add special entry for index page
    navIndex.append(new NavIndexEntry("index"+Doxygen::htmlFileExtension,""));
    // related page index is written as a child of index.html, so add this as well
    navIndex.append(new NavIndexEntry("pages"+Doxygen::htmlFileExtension,""));

    bool first=TRUE;
    generateJSTree(navIndex,t,nodeList,1,first);

    if (first) 
      t << "]" << endl;
    else 
      t << endl << "  ] ]" << endl;
    t << "];" << endl << endl;

    // write the navigation index (and sub-indices)
    navIndex.sort();
    int subIndex=0;
    int elemCount=0;
    const int maxElemCount=250;
    //QFile fidx(htmlOutput+"/navtreeindex.js");
    QFile fsidx(htmlOutput+"/navtreeindex0.js");
    if (/*fidx.open(IO_WriteOnly) &&*/ fsidx.open(IO_WriteOnly))
    {
      //FTextStream tidx(&fidx);
      FTextStream tsidx(&fsidx);
      t << "var NAVTREEINDEX =" << endl;
      t << "[" << endl;
      tsidx << "var NAVTREEINDEX" << subIndex << " =" << endl;
      tsidx << "{" << endl;
      QListIterator<NavIndexEntry> li(navIndex);
      NavIndexEntry *e;
      bool first=TRUE;
      for (li.toFirst();(e=li.current());) // for each entry
      {
        if (elemCount==0)
        {
          if (!first)
          {
            t << "," << endl;
          }
          else
          {
            first=FALSE;
          }
          t << "\"" << e->url << "\"";
        }
        tsidx << "\"" << e->url << "\":[" << e->path << "]";
        ++li;
        if (li.current() && elemCount<maxElemCount-1) tsidx << ","; // not last entry
        tsidx << endl;
  
        elemCount++;
        if (li.current() && elemCount>=maxElemCount) // switch to new sub-index
        {
          tsidx << "};" << endl;
          elemCount=0;
          fsidx.close();
          subIndex++;
          fsidx.setName(htmlOutput+"/navtreeindex"+QCString().setNum(subIndex)+".js");
          if (!fsidx.open(IO_WriteOnly)) break;
          tsidx.setDevice(&fsidx);
          tsidx << "var NAVTREEINDEX" << subIndex << " =" << endl;
          tsidx << "{" << endl;
        }
      }
      tsidx << "};" << endl;
      t << endl << "];" << endl;
    }
    t << endl << "var SYNCONMSG = '"  << theTranslator->trPanelSynchronisationTooltip(FALSE) << "';"; 
    t << endl << "var SYNCOFFMSG = '" << theTranslator->trPanelSynchronisationTooltip(TRUE)  << "';"; 
  }
  ResourceMgr::instance().copyResource("navtree.js",htmlOutput);
}

//-----------------------------------------------------------

// new style images
void FTVHelp::generateTreeViewImages()
{
  QCString dname=Config_getString(HTML_OUTPUT);
  const ResourceMgr &rm = ResourceMgr::instance();
  rm.copyResource("doc.luma",dname);
  rm.copyResource("folderopen.luma",dname);
  rm.copyResource("folderclosed.luma",dname);
  rm.copyResource("arrowdown.luma",dname);
  rm.copyResource("arrowright.luma",dname);
  rm.copyResource("splitbar.lum",dname);
}

// new style scripts
void FTVHelp::generateTreeViewScripts()
{
  QCString htmlOutput = Config_getString(HTML_OUTPUT);

  // generate navtree.js & navtreeindex.js
  generateJSNavTree(m_indentNodes[0]);

  // copy resize.js & navtree.css
  ResourceMgr::instance().copyResource("resize.js",htmlOutput);
  ResourceMgr::instance().copyResource("navtree.css",htmlOutput);
}

// write tree inside page
void FTVHelp::generateTreeViewInline(FTextStream &t)
{
  int preferredNumEntries = Config_getInt(HTML_INDEX_NUM_ENTRIES);
  t << "<div class=\"directory\">\n";
  QListIterator<FTVNode> li(m_indentNodes[0]);
  FTVNode *n;
  int d=1, depth=1;
  for (;(n=li.current());++li)
  {
    if (n->children.count()>0)
    {
      d = n->computeTreeDepth(2);
      if (d>depth) depth=d;
    }
  }
  int preferredDepth = depth;
  // write level selector
  if (depth>1)
  {
    t << "<div class=\"levels\">[";
    t << theTranslator->trDetailLevel(); 
    t << " ";
    int i;
    for (i=1;i<=depth;i++)
    {
      t << "<span onclick=\"javascript:toggleLevel(" << i << ");\">" << i << "</span>";
    }
    t << "]</div>";

    if (preferredNumEntries>0)
    {
      preferredDepth=1;
      for (int i=1;i<=depth;i++)
      {
        int num=0;
        QListIterator<FTVNode> li(m_indentNodes[0]);
        FTVNode *n;
        for (;(n=li.current());++li)
        {
          num+=n->numNodesAtLevel(0,i);
        }
        if (num<=preferredNumEntries)
        {
          preferredDepth=i;
        }
        else
        {
          break;
        }
      }
    }
  }
  //printf("preferred depth=%d\n",preferredDepth);

  t << "<table class=\"directory\">\n";
  int index=0;
  generateTree(t,m_indentNodes[0],0,preferredDepth,index);
  t << "</table>\n";

  t << "</div><!-- directory -->\n";
}

// write old style index.html and tree.html
void FTVHelp::generateTreeView()
{
  generateTreeViewImages();
  generateTreeViewScripts();
}


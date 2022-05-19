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
#include <algorithm>

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
#include "classdef.h"
#include "util.h"
#include "resourcemgr.h"

static int folderId=1;

const char *JAVASCRIPT_LICENSE_TEXT = R"LIC(/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
)LIC";

struct FTVNode;
using FTVNodePtr     = std::shared_ptr<FTVNode>;
using FTVNodeWeakPtr = std::weak_ptr<FTVNode>;
using FTVNodes       = std::vector<FTVNodePtr>;

struct FTVNode
{
  FTVNode(bool dir,const QCString &r,const QCString &f,const QCString &a,
          const QCString &n,bool sepIndex,bool navIndex,const Definition *df)
    : isLast(TRUE), isDir(dir), ref(r), file(f), anchor(a), name(n),
      separateIndex(sepIndex), addToNavIndex(navIndex),
      def(df) {}
 ~FTVNode() = default;
  int computeTreeDepth(int level) const;
  int numNodesAtLevel(int level,int maxLevel) const;
  bool isLast;
  bool isDir;
  QCString ref;
  QCString file;
  QCString anchor;
  QCString name;
  int index = 0;
  FTVNodes children;
  FTVNodeWeakPtr parent;
  bool separateIndex;
  bool addToNavIndex;
  const Definition *def;
};

int FTVNode::computeTreeDepth(int level) const
{
  int maxDepth=level;
  for (const auto &n : children)
  {
    if (!n->children.empty())
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
    for (const auto &n : children)
    {
      num+=n->numNodesAtLevel(level+1,maxLevel);
    }
  }
  return num;
}

//----------------------------------------------------------------------------

struct FTVHelp::Private
{
  Private(bool TLI) : topLevelIndex(TLI) { indentNodes.resize(1); }
  std::vector<FTVNodes> indentNodes;
  int indent = 0;
  bool topLevelIndex;

  void generateTree(TextStream &t,const FTVNodes &nl,int level,int maxLevel,int &index);
  void generateLink(TextStream &t,const FTVNodePtr &n);
};

/*! Constructs an ftv help object.
 *  The object has to be \link initialize() initialized\endlink before it can
 *  be used.
 */
FTVHelp::FTVHelp(bool TLI) : p(std::make_unique<Private>(TLI)) {}
FTVHelp::~FTVHelp() = default;
FTVHelp::FTVHelp(FTVHelp &&) = default;

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
  //printf("%p: incContentsDepth() indent=%d\n",this,p->indent);
  p->indent++;
  p->indentNodes.resize(p->indent+1);
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the current sublist.
 *  \sa incContentsDepth()
 */
void FTVHelp::decContentsDepth()
{
  //printf("%p: decContentsDepth() indent=%d\n",this,p->indent);
  ASSERT(p->indent>0);
  if (p->indent>0)
  {
    p->indent--;
    auto &nl = p->indentNodes[p->indent];
    if (!nl.empty())
    {
      auto &parent = nl.back();
      auto &children = p->indentNodes[p->indent+1];
      for (const auto &child : children)
      {
        parent->children.push_back(child);
      }
      children.clear();
    }
  }
}

/*! Add a list item to the contents file.
 *  \param isDir TRUE if the item is a directory, FALSE if it is a text
 *  \param name the name of the item.
 *  \param ref  the URL of to the item.
 *  \param file the file containing the definition of the item
 *  \param anchor the anchor within the file.
 *  \param separateIndex put the entries in a separate index file
 *  \param addToNavIndex add this entry to the quick navigation index
 *  \param def Definition corresponding to this entry
 */
void FTVHelp::addContentsItem(bool isDir,
                              const QCString &name,
                              const QCString &ref,
                              const QCString &file,
                              const QCString &anchor,
                              bool separateIndex,
                              bool addToNavIndex,
                              const Definition *def
                              )
{
  //printf("%p: p->indent=%d addContentsItem(%s,%s,%s,%s)\n",this,p->indent,name,ref,file,anchor);
  auto &nl = p->indentNodes[p->indent];
  if (!nl.empty())
  {
    nl.back()->isLast=FALSE;
  }
  auto newNode = std::make_shared<FTVNode>(isDir,ref,file,anchor,name,separateIndex,addToNavIndex,def);
  nl.push_back(newNode);
  newNode->index = static_cast<int>(nl.size()-1);
  if (p->indent>0)
  {
    auto &pnl = p->indentNodes[p->indent-1];
    if (!pnl.empty())
    {
      newNode->parent = pnl.back();
    }
  }
}

static QCString node2URL(const FTVNodePtr &n,bool overruleFile=FALSE,bool srcLink=FALSE)
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
      const FileDef *fd = toFileDef(n->def);
      if (srcLink)
      {
        url = fd->getSourceFileBase();
      }
      else
      {
        url = fd->getOutputFileBase();
      }
    }
    url = addHtmlExtensionIfMissing(url);
    if (!n->anchor.isEmpty()) url+="#"+n->anchor;
  }
  return url;
}

static QCString generateIndentLabel(const FTVNodePtr &n,int level)
{
  QCString result;
  auto parent = n->parent.lock();
  if (parent)
  {
    result=generateIndentLabel(parent,level+1);
  }
  result+=QCString().setNum(n->index)+"_";
  return result;
}

static void generateIndent(TextStream &t, const FTVNodePtr &n,bool opened)
{
  int indent=0;
  auto parent = n->parent.lock();
  while (parent) { indent++; parent=parent->parent.lock(); }
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

void FTVHelp::Private::generateLink(TextStream &t,const FTVNodePtr &n)
{
  //printf("FTVHelp::generateLink(ref=%s,file=%s,anchor=%s\n",
  //    qPrint(n->ref),qPrint(n->file),qPrint(n->anchor));
  bool setTarget = FALSE;
  if (n->file.isEmpty()) // no link
  {
    t << "<b>" << convertToHtml(n->name) << "</b>";
  }
  else // link into other frame
  {
    if (!n->ref.isEmpty()) // link to entity imported via tag file
    {
      t << "<a class=\"elRef\" ";
      QCString result = externalLinkTarget();
      if (result != "") setTarget = TRUE;
      t << result;
    }
    else // local link
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    t << externalRef("",n->ref,TRUE);
    t << node2URL(n);
    if (!setTarget)
    {
      if (topLevelIndex)
        t << "\" target=\"basefrm\">";
      else
        t << "\" target=\"_self\">";
    }
    else
    {
      t << "\">";
    }
    t << convertToHtml(n->name);
    t << "</a>";
    if (!n->ref.isEmpty())
    {
      t << "&#160;[external]";
    }
  }
}

static void generateBriefDoc(TextStream &t,const Definition *def)
{
  QCString brief = def->briefDescription(TRUE);
  //printf("*** %p: generateBriefDoc(%s)='%s'\n",def,qPrint(def->name()),qPrint(brief));
  if (!brief.isEmpty())
  {
    auto parser { createDocParser() };
    auto ast    { validatingParseDoc(*parser.get(),
                                     def->briefFile(),def->briefLine(),
                                     def,0,brief,FALSE,FALSE,
                                     QCString(),TRUE,TRUE,Config_getBool(MARKDOWN_SUPPORT)) };
    const DocNodeAST *astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
    if (astImpl)
    {
      QCString relPath = relativePathToRoot(def->getOutputFileBase());
      HtmlCodeGenerator htmlGen(t,relPath);
      HtmlDocVisitor visitor(t,htmlGen,def);
      std::visit(visitor,astImpl->root);
    }
  }
}

static char compoundIcon(const ClassDef *cd)
{
  char icon='C';
  if (cd->getLanguage() == SrcLangExt_Slice)
  {
    if (cd->compoundType()==ClassDef::Interface)
    {
      icon='I';
    }
    else if (cd->compoundType()==ClassDef::Struct)
    {
      icon='S';
    }
    else if (cd->compoundType()==ClassDef::Exception)
    {
      icon='E';
    }
  }
  return icon;
}

void FTVHelp::Private::generateTree(TextStream &t, const FTVNodes &nl,int level,int maxLevel,int &index)
{
  for (const auto &n : nl)
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
        if (n->def->getLanguage() == SrcLangExt_Slice)
        {
          t << "<span class=\"icona\"><span class=\"icon\">M</span></span>";
        }
        else
        {
          t << "<span class=\"icona\"><span class=\"icon\">N</span></span>";
        }
      }
      else if (n->def && n->def->definitionType()==Definition::TypeClass)
      {
        char icon=compoundIcon(toClassDef(n->def));
        t << "<span class=\"icona\"><span class=\"icon\">" << icon << "</span></span>";
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
      t << "</td></tr>\n";
      folderId++;
      generateTree(t,n->children,level+1,maxLevel,index);
    }
    else // leaf node
    {
      const FileDef *srcRef=0;
      if (n->def && n->def->definitionType()==Definition::TypeFile &&
          (toFileDef(n->def))->generateSourceFile())
      {
        srcRef = toFileDef(n->def);
      }
      if (srcRef)
      {
        t << "<a href=\"" << addHtmlExtensionIfMissing(srcRef->getSourceFileBase())
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
        if (n->def->getLanguage() == SrcLangExt_Slice)
        {
          t << "<span class=\"icona\"><span class=\"icon\">M</span></span>";
        }
        else
        {
          t << "<span class=\"icona\"><span class=\"icon\">N</span></span>";
        }
      }
      else if (n->def && n->def->definitionType()==Definition::TypeClass)
      {
        char icon=compoundIcon(toClassDef(n->def));
        t << "<span class=\"icona\"><span class=\"icon\">" << icon << "</span></span>";
      }
      else if (n->def && n->def->definitionType()==Definition::TypeConcept)
      {
        t << "<span class=\"icona\"><span class=\"icon\">R</span></span>";
      }
      else if (n->def && n->def->definitionType()==Definition::TypeDir)
      {
        t << "<span class=\"iconfclosed\"></span>";
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
      t << "</td></tr>\n";
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

class NavIndexEntryList : public std::vector<NavIndexEntry>
{
};

static QCString pathToNode(const FTVNodePtr &leaf,const FTVNodePtr &n)
{
  QCString result;
  auto parent = n->parent.lock();
  if (parent)
  {
    result+=pathToNode(leaf,parent);
  }
  result+=QCString().setNum(n->index);
  if (leaf!=n) result+=",";
  return result;
}

static bool dupOfParent(const FTVNodePtr &n)
{
  auto parent = n->parent.lock();
  if (!parent) return FALSE;
  if (n->file==parent->file) return TRUE;
  return FALSE;
}

static void generateJSLink(TextStream &t,const FTVNodePtr &n)
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

static bool generateJSTree(NavIndexEntryList &navIndex,TextStream &t,
                           const FTVNodes &nl,int level,bool &first)
{
  QCString htmlOutput = Config_getString(HTML_OUTPUT);
  QCString indentStr;
  indentStr.fill(' ',level*2);

  bool found=FALSE;
  for (const auto &n : nl)
  {
    // terminate previous entry
    if (!first) t << ",\n";
    first=FALSE;

    // start entry
    if (!found)
    {
      t << "[\n";
    }
    found=TRUE;

    if (n->addToNavIndex) // add entry to the navigation index
    {
      if (n->def && n->def->definitionType()==Definition::TypeFile)
      {
        const FileDef *fd = toFileDef(n->def);
        bool doc,src;
        doc = fileVisibleInIndex(fd,src);
        if (doc)
        {
          navIndex.emplace_back(node2URL(n,TRUE,FALSE),pathToNode(n,n));
        }
        if (src)
        {
          navIndex.emplace_back(node2URL(n,TRUE,TRUE),pathToNode(n,n));
        }
      }
      else
      {
        navIndex.emplace_back(node2URL(n),pathToNode(n,n));
      }
    }

    if (n->separateIndex) // store items in a separate file for dynamic loading
    {
      bool firstChild=TRUE;
      t << indentStr << "  [ ";
      generateJSLink(t,n);
      if (!n->children.empty()) // write children to separate file for dynamic loading
      {
        QCString fileId = n->file;
        if (!n->anchor.isEmpty())
        {
          fileId+="_"+n->anchor;
        }
        if (dupOfParent(n))
        {
          fileId+="_dup";
        }
        QCString fileName = htmlOutput+"/"+fileId+".js";
        std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
        if (f.is_open())
        {
          TextStream tt(&f);
          tt << "var " << convertFileId2Var(fileId) << " =\n";
          generateJSTree(navIndex,tt,n->children,1,firstChild);
          tt << "\n];";
        }
        f.close();
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
        t << "\n" << indentStr << "  ] ]";
    }
  }
  return found;
}

static void generateJSNavTree(const FTVNodes &nodeList)
{
  QCString htmlOutput = Config_getString(HTML_OUTPUT);
  std::ofstream f(htmlOutput.str()+"/navtreedata.js",std::ofstream::out | std::ofstream::binary);
  NavIndexEntryList navIndex;
  if (f.is_open())
  {
    TextStream t(&f);
    //TextStream tidx(&fidx);
    //tidx << "var NAVTREEINDEX =\n";
    //tidx << "{\n";
    t << JAVASCRIPT_LICENSE_TEXT;
    t << "var NAVTREE =\n";
    t << "[\n";
    t << "  [ ";
    QCString projName = Config_getString(PROJECT_NAME);
    if (projName.isEmpty())
    {
      if (mainPageHasTitle()) // Use title of main page as root
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
    navIndex.emplace_back("index"+Doxygen::htmlFileExtension,"");
    // related page index is written as a child of index.html, so add this as well
    navIndex.emplace_back("pages"+Doxygen::htmlFileExtension,"");

    bool first=TRUE;
    generateJSTree(navIndex,t,nodeList,1,first);

    if (first)
      t << "]\n";
    else
      t << "\n  ] ]\n";
    t << "];\n\n";

    // write the navigation index (and sub-indices)
    std::sort(navIndex.begin(),navIndex.end(),[](const auto &n1,const auto &n2)
        { return !n1.url.isEmpty() && (n2.url.isEmpty() || (n1.url<n2.url)); });

    int subIndex=0;
    int elemCount=0;
    const int maxElemCount=250;
    std::ofstream tsidx(htmlOutput.str()+"/navtreeindex0.js",std::ofstream::out | std::ofstream::binary);
    if (tsidx.is_open())
    {
      t << "var NAVTREEINDEX =\n";
      t << "[\n";
      tsidx << "var NAVTREEINDEX" << subIndex << " =\n";
      tsidx << "{\n";
      first=TRUE;
      auto it = navIndex.begin();
      while (it!=navIndex.end())
      {
        const NavIndexEntry &e = *it;
        if (elemCount==0)
        {
          if (!first)
          {
            t << ",\n";
          }
          else
          {
            first=FALSE;
          }
          t << "\"" << e.url << "\"";
        }
        tsidx << "\"" << e.url << "\":[" << e.path << "]";
        ++it;
        if (it!=navIndex.end() && elemCount<maxElemCount-1) tsidx << ","; // not last entry
        tsidx << "\n";

        elemCount++;
        if (it!=navIndex.end() && elemCount>=maxElemCount) // switch to new sub-index
        {
          tsidx << "};\n";
          elemCount=0;
          tsidx.close();
          subIndex++;
          QCString fileName = htmlOutput+"/navtreeindex"+QCString().setNum(subIndex)+".js";
          tsidx.open(fileName.str(),std::ofstream::out | std::ofstream::binary);
          if (!tsidx.is_open()) break;
          tsidx << "var NAVTREEINDEX" << subIndex << " =\n";
          tsidx << "{\n";
        }
      }
      tsidx << "};\n";
      t << "\n];\n";
    }
    t << "\nvar SYNCONMSG = '"  << theTranslator->trPanelSynchronisationTooltip(FALSE) << "';";
    t << "\nvar SYNCOFFMSG = '" << theTranslator->trPanelSynchronisationTooltip(TRUE)  << "';";
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
  rm.copyResource("splitbar.lum",dname);
}

// new style scripts
void FTVHelp::generateTreeViewScripts()
{
  QCString htmlOutput = Config_getString(HTML_OUTPUT);

  // generate navtree.js & navtreeindex.js
  generateJSNavTree(p->indentNodes[0]);

  // copy resize.js & navtree.css
  ResourceMgr::instance().copyResource("resize.js",htmlOutput);
  ResourceMgr::instance().copyResource("navtree.css",htmlOutput);
}

// write tree inside page
void FTVHelp::generateTreeViewInline(TextStream &t)
{
  int preferredNumEntries = Config_getInt(HTML_INDEX_NUM_ENTRIES);
  t << "<div class=\"directory\">\n";
  int d=1, depth=1;
  for (const auto &n : p->indentNodes[0])
  {
    if (!n->children.empty())
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
    for (int i=1;i<=depth;i++)
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
        for (const auto &n : p->indentNodes[0])
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

  if (!p->indentNodes[0].empty())
  {
    t << "<table class=\"directory\">\n";
    int index=0;
    p->generateTree(t,p->indentNodes[0],0,preferredDepth,index);
    t << "</table>\n";
  }

  t << "</div><!-- directory -->\n";
}

// write old style index.html and tree.html
void FTVHelp::generateTreeView()
{
  generateTreeViewImages();
  generateTreeViewScripts();
}

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

/** @file
 *  @brief This file contains functions for the various index pages.
 */

#include <stdlib.h>

#include <qtextstream.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qregexp.h>

#include "message.h"
#include "index.h"
#include "doxygen.h"
#include "config.h"
#include "filedef.h"
#include "outputlist.h"
#include "util.h"
#include "groupdef.h"
#include "language.h"
#include "htmlgen.h"
#include "htmlhelp.h"
#include "ftvhelp.h"
#include "dot.h"
#include "pagedef.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "memberlist.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"

#define MAX_ITEMS_BEFORE_MULTIPAGE_INDEX 200
#define MAX_ITEMS_BEFORE_QUICK_INDEX 30


int annotatedClasses;
int annotatedClassesPrinted;
int hierarchyClasses;
int documentedFiles;
int documentedGroups;
int documentedNamespaces;
int indexedPages;
int documentedClassMembers[CMHL_Total];
int documentedFileMembers[FMHL_Total];
int documentedNamespaceMembers[NMHL_Total];
int documentedHtmlFiles;
int documentedPages;
int documentedDirs;

static int countClassHierarchy();
static void countFiles(int &htmlFiles,int &files);
static int countGroups();
static int countDirs();
static int countNamespaces();
static int countAnnotatedClasses(int *cp);
static void countRelatedPages(int &docPages,int &indexPages);

void countDataStructures()
{
  annotatedClasses           = countAnnotatedClasses(&annotatedClassesPrinted); // "classes" + "annotated"
  hierarchyClasses           = countClassHierarchy();   // "hierarchy"
  countFiles(documentedHtmlFiles,documentedFiles);      // "files"
  countRelatedPages(documentedPages,indexedPages);      // "pages"
  documentedGroups           = countGroups();           // "modules"
  documentedNamespaces       = countNamespaces();       // "namespaces"
  documentedDirs             = countDirs();             // "dirs"
  // "globals"
  // "namespacemembers"
  // "functions"
}

static void startIndexHierarchy(OutputList &ol,int level)
{
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Html);
  if (level<6) ol.startIndexList();
  ol.enableAll();
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  ol.startItemList();
  ol.popGeneratorState();
}

static void endIndexHierarchy(OutputList &ol,int level)
{
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Html);
  if (level<6) ol.endIndexList();
  ol.enableAll();
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  ol.endItemList();
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

class MemberIndexList : public QList<MemberDef>
{
  public:
    MemberIndexList() : QList<MemberDef>() {}
    ~MemberIndexList() {}
    int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      MemberDef *md1=(MemberDef *)item1;
      MemberDef *md2=(MemberDef *)item2;
      return qstricmp(md1->name(),md2->name());
    }
};

#define MEMBER_INDEX_ENTRIES 256

static MemberIndexList g_memberIndexLetterUsed[CMHL_Total][MEMBER_INDEX_ENTRIES];
static MemberIndexList g_fileIndexLetterUsed[FMHL_Total][MEMBER_INDEX_ENTRIES];
static MemberIndexList g_namespaceIndexLetterUsed[NMHL_Total][MEMBER_INDEX_ENTRIES];

//static bool g_classIndexLetterUsed[CHL_Total][256];

const int maxItemsBeforeQuickIndex = MAX_ITEMS_BEFORE_QUICK_INDEX;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

static void startQuickIndexList(OutputList &ol,bool letterTabs=FALSE)
{
  bool fancyTabs = TRUE;
  if (fancyTabs)
  {
    if (letterTabs)
    {
      ol.writeString("  <div id=\"navrow4\" class=\"tabs3\">\n"); 
    }
    else
    {
      ol.writeString("  <div id=\"navrow3\" class=\"tabs2\">\n"); 
    }
    ol.writeString("    <ul class=\"tablist\">\n"); 
  }
  else
  {
    ol.writeString("  <div class=\"qindex\">"); 
  }
}

static void endQuickIndexList(OutputList &ol)
{
  bool fancyTabs = TRUE;
  if (fancyTabs)
  {
    ol.writeString("    </ul>\n");
  }
  ol.writeString("  </div>\n");
}

static void startQuickIndexItem(OutputList &ol,const char *l,
                                bool hl,bool compact,bool &first)
{
  bool fancyTabs = TRUE;
  if (!first && compact && !fancyTabs) ol.writeString(" | ");
  first=FALSE;
  if (fancyTabs)
  {
    ol.writeString("      <li"); 
    if (hl) ol.writeString(" class=\"current\"");
    ol.writeString("><a ");
  }
  else
  {
    if (!compact) ol.writeString("<li>");
    if (hl && compact)
    {
      ol.writeString("<a class=\"qindexHL\" ");
    }
    else
    {
      ol.writeString("<a class=\"qindex\" ");
    }
  }
  ol.writeString("href=\""); 
  ol.writeString(l);
  ol.writeString("\">");
  if (fancyTabs)
  {
    ol.writeString("<span>");
  }
}

static void endQuickIndexItem(OutputList &ol)
{
  bool fancyTabs=TRUE;
  if (fancyTabs) ol.writeString("</span>");
  ol.writeString("</a>");
  if (fancyTabs) ol.writeString("</li>\n");
}

// don't make this static as it is called from a template function and some
// old compilers don't support calls to static functions from a template.
QCString fixSpaces(const QCString &s)
{
  return substitute(s," ","&#160;");
}

void startTitle(OutputList &ol,const char *fileName,Definition *def)
{
  ol.startHeaderSection();
  if (def) def->writeSummaryLinks(ol);
  ol.startTitleHead(fileName);
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
}

void endTitle(OutputList &ol,const char *fileName,const char *name)
{
  ol.popGeneratorState();
  ol.endTitleHead(fileName,name);
  ol.endHeaderSection();
}

void startFile(OutputList &ol,const char *name,const char *manName,
               const char *title,HighlightedItem hli,bool additionalIndices,
               const char *altSidebarName)
{
  static bool disableIndex     = Config_getBool("DISABLE_INDEX");
  ol.startFile(name,manName,title);
  ol.startQuickIndices();
  if (!disableIndex)
  {
    ol.writeQuickLinks(TRUE,hli,name);
  }
  if (!additionalIndices)
  {
    ol.endQuickIndices();
  }
  ol.writeSplitBar(altSidebarName ? altSidebarName : name);
  ol.writeSearchInfo();
}

void endFile(OutputList &ol,bool skipNavIndex,bool skipEndContents,
             const QCString &navPath)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  if (!skipNavIndex)
  {
    if (!skipEndContents) ol.endContents();
    if (generateTreeView)
    {
      ol.writeString("</div><!-- doc-content -->\n");
    }
  }
  ol.writeFooter(navPath); // write the footer
  ol.popGeneratorState();
  ol.endFile();
}

void endFileWithNavPath(Definition *d,OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  QCString navPath;
  if (generateTreeView)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString("</div><!-- doc-content -->\n");
    ol.popGeneratorState();
    navPath = d->navigationPathAsString();
  }
  endFile(ol,generateTreeView,TRUE,navPath);
}

//----------------------------------------------------------------------
template<class T> 
void addMembersToIndex(T *def,LayoutDocManager::LayoutPart part,
                       const QCString &name,const QCString &anchor,
                       bool addToIndex=TRUE,bool preventSeparateIndex=FALSE)
{
  bool hasMembers = def->getMemberLists().count()>0 || def->getMemberGroupSDict()!=0;
  Doxygen::indexList->addContentsItem(hasMembers,name,
                                     def->getReference(),def->getOutputFileBase(),anchor,
                                     hasMembers && !preventSeparateIndex,
                                     addToIndex,
                                     def);
  int numClasses=0;
  ClassSDict *classes = def->getClassSDict();
  if (classes)
  {
     ClassDef *cd;
     ClassSDict::Iterator it(*classes);
     for (;(cd=it.current());++it)
     {
       if (cd->isLinkable()) numClasses++;
     }
  }
  //printf("addMembersToIndex(def=%s hasMembers=%d numClasses=%d)\n",def->name().data(),hasMembers,numClasses);
  if (hasMembers || numClasses>0)
  {
    Doxygen::indexList->incContentsDepth();
    QListIterator<LayoutDocEntry> eli(LayoutDocManager::instance().docEntries(part));
    LayoutDocEntry *lde;
    for (eli.toFirst();(lde=eli.current());++eli)
    {
      if (lde->kind()==LayoutDocEntry::MemberDef)
      {
        LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
        MemberList *ml = def->getMemberList(lmd->type);
        if (ml)
        {
          MemberListIterator mi(*ml);
          MemberDef *md;
          for (mi.toFirst();(md=mi.current());++mi)
          {
            LockingPtr<MemberList> enumList = md->enumFieldList();
            bool isDir = enumList!=0 && md->isEnumerate();
            bool isAnonymous = md->name().find('@')!=-1;
            static bool hideUndocMembers = Config_getBool("HIDE_UNDOC_MEMBERS");
            static bool extractStatic = Config_getBool("EXTRACT_STATIC");
            if (!isAnonymous && 
                (!hideUndocMembers || md->hasDocumentation()) &&
                (!md->isStatic() || extractStatic)
               )
            {
              if (md->getOuterScope()==def)
              {
                Doxygen::indexList->addContentsItem(isDir,
                  md->name(),md->getReference(),md->getOutputFileBase(),md->anchor(),FALSE,addToIndex);
              }
              else // inherited member
              {
                Doxygen::indexList->addContentsItem(isDir,
                  md->name(),def->getReference(),def->getOutputFileBase(),md->anchor(),FALSE,addToIndex);
              }
            }
            if (isDir)
            {
              if (!isAnonymous)
              {
                Doxygen::indexList->incContentsDepth();
              }
              MemberListIterator emli(*enumList);
              MemberDef *emd;
              for (emli.toFirst();(emd=emli.current());++emli)
              {
                if (emd->getOuterScope()==def)
                {
                  Doxygen::indexList->addContentsItem(FALSE,
                      emd->name(),emd->getReference(),emd->getOutputFileBase(),emd->anchor(),FALSE,addToIndex);
                }
                else // inherited member
                {
                  Doxygen::indexList->addContentsItem(FALSE,
                      emd->name(),def->getReference(),def->getOutputFileBase(),emd->anchor(),FALSE,addToIndex);
                }
              }
              if (!isAnonymous)
              {
                Doxygen::indexList->decContentsDepth();
              }
            }
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::NamespaceClasses || 
               lde->kind()==LayoutDocEntry::FileClasses || 
               lde->kind()==LayoutDocEntry::ClassNestedClasses
              )
      {
        if (classes)
        {
          ClassDef *cd;
          ClassSDict::Iterator it(*classes);
          for (;(cd=it.current());++it)
          {
            if (cd->isLinkable() && (cd->partOfGroups()==0 || def->definitionType()==Definition::TypeGroup))
            {
              bool isNestedClass = def->definitionType()==Definition::TypeClass;
              addMembersToIndex(cd,LayoutDocManager::Class,cd->displayName(FALSE),cd->anchor(),
                                addToIndex && isNestedClass,
                                preventSeparateIndex || cd->isEmbeddedInOuterScope());
            }
          }
        }
      }
    }

    Doxygen::indexList->decContentsDepth();
  }
}

//----------------------------------------------------------------------------

static bool classHasVisibleChildren(ClassDef *cd)
{
  BaseClassList *bcl;

  if (cd->getLanguage()==SrcLangExt_VHDL) // reverse baseClass/subClass relation
  {
    if (cd->baseClasses()==0) return FALSE;
    bcl=cd->baseClasses();
  }
  else 
  {
    if (cd->subClasses()==0) return FALSE;
    bcl=cd->subClasses();
  }

  BaseClassListIterator bcli(*bcl);
  for ( ; bcli.current() ; ++bcli)
  {
    if (bcli.current()->classDef->isVisibleInHierarchy())
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/*! Generates HTML Help tree of classes */

static void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper,int level,FTVHelp* ftv,bool addToIndex)
{
  if (bcl==0) return;
  BaseClassListIterator bcli(*bcl);
  bool started=FALSE;
  for ( ; bcli.current() ; ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->getLanguage()==SrcLangExt_VHDL && (VhdlDocGen::VhdlClasses)cd->protection()!=VhdlDocGen::ENTITYCLASS)
    {
      continue;
    }

    bool b;
    if (cd->getLanguage()==SrcLangExt_VHDL)
    {
      b=hasVisibleRoot(cd->subClasses());
    }
    else
    {
      b=hasVisibleRoot(cd->baseClasses());
    }

    if (cd->isVisibleInHierarchy() && b) // hasVisibleRoot(cd->baseClasses()))
    {
      if (!started)
      {
        startIndexHierarchy(ol,level);
        if (addToIndex)
        {
          Doxygen::indexList->incContentsDepth();
        }
        if (ftv)
        {
          ftv->incContentsDepth();
        }
        started=TRUE;
      }
      ol.startIndexListItem();
      //printf("Passed...\n");
      bool hasChildren = !cd->visited && !hideSuper && classHasVisibleChildren(cd);
      //printf("tree4: Has children %s: %d\n",cd->name().data(),hasChildren);
      if (cd->isLinkable())
      {
        //printf("Writing class %s\n",cd->displayName().data());
        ol.startIndexItem(cd->getReference(),cd->getOutputFileBase());
        ol.parseText(cd->displayName());
        ol.endIndexItem(cd->getReference(),cd->getOutputFileBase());
        if (cd->isReference()) 
        { 
          ol.startTypewriter(); 
          ol.docify(" [external]");
          ol.endTypewriter();
        }
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor());
        }
        if (ftv)
        {
          if (cd->getLanguage()==SrcLangExt_VHDL)
          {
            ftv->addContentsItem(hasChildren,bcli.current()->usedName,cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd);
          }
          else
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd);
          }
        }
      }
      else
      {
        ol.startIndexItem(0,0);
        ol.parseText(cd->name());
        ol.endIndexItem(0,0);
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),0,0,0);
        }
        if (ftv)
        {
          ftv->addContentsItem(hasChildren,cd->displayName(),0,0,0,FALSE,FALSE,cd);
        }
      }
      if (hasChildren)
      {
        //printf("Class %s at %p visited=%d\n",cd->name().data(),cd,cd->visited);
        bool wasVisited=cd->visited;
        cd->visited=TRUE;
        if (cd->getLanguage()==SrcLangExt_VHDL)	
        {
          writeClassTree(ol,cd->baseClasses(),wasVisited,level+1,ftv,addToIndex);
        }
        else       
        {
          writeClassTree(ol,cd->subClasses(),wasVisited,level+1,ftv,addToIndex);
        }
      }
      ol.endIndexListItem();
    }
  }
  if (started) 
  {
    endIndexHierarchy(ol,level);
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
    if (ftv)
    {
      ftv->decContentsDepth();
    }
  }
}

//----------------------------------------------------------------------------

static bool classVisibleInIndex(ClassDef *cd)
{
  static bool allExternals = Config_getBool("ALLEXTERNALS");
  return (allExternals && cd->isLinkable()) || cd->isLinkableInProject();
}

//----------------------------------------------------------------------------

static bool dirHasVisibleChildren(DirDef *dd)
{
  if (dd->hasDocumentation()) return TRUE;

  QListIterator<FileDef> fli(*dd->getFiles());
  FileDef *fd;
  for (fli.toFirst();(fd=fli.current());++fli)
  {
    bool genSourceFile;
    if (fileVisibleInIndex(fd,genSourceFile))
    {
      return TRUE;
    }
    if (genSourceFile)
    {
      return TRUE;
    }
  }

  QListIterator<DirDef> dli(dd->subDirs());
  DirDef *subdd;
  for (dli.toFirst();(subdd=dli.current());++dli)
  {
    if (dirHasVisibleChildren(subdd))
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
static void writeDirTreeNode(OutputList &ol, DirDef *dd, int level, FTVHelp* ftv,bool addToIndex)
{
  if (level>20)
  {
    warn(dd->getDefFileName(),dd->getDefLine(),
        "warning: maximum nesting level exceeded for directory %s: "
        "check for possible recursive directory relation!\n",dd->name().data()
        );
    return;
  }

  if (!dirHasVisibleChildren(dd))
  {
    return;
  }

  static bool tocExpand = TRUE; //Config_getBool("TOC_EXPAND");
  bool isDir = dd->subDirs().count()>0 || // there are subdirs
               (tocExpand &&              // or toc expand and
                dd->getFiles() && dd->getFiles()->count()>0 // there are files
               );
  //printf("gd=`%s': pageDict=%d\n",gd->name().data(),gd->pageDict->count());
  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(isDir,dd->shortName(),dd->getReference(),dd->getOutputFileBase(),0,TRUE,TRUE); 
    Doxygen::indexList->incContentsDepth();
  }
  if (ftv)
  {
    ftv->addContentsItem(isDir,dd->shortName(),dd->getReference(),
                         dd->getOutputFileBase(),0,FALSE,TRUE,dd); 
    ftv->incContentsDepth();
  }

  ol.startIndexListItem();
  ol.startIndexItem(dd->getReference(),dd->getOutputFileBase());
  ol.parseText(dd->shortName());
  ol.endIndexItem(dd->getReference(),dd->getOutputFileBase());
  if (dd->isReference()) 
  { 
    ol.startTypewriter(); 
    ol.docify(" [external]");
    ol.endTypewriter();
  }

  // write sub directories
  if (dd->subDirs().count()>0)
  {
    startIndexHierarchy(ol,level+1);
    QListIterator<DirDef> dli(dd->subDirs());
    DirDef *subdd = 0;
    for (dli.toFirst();(subdd=dli.current());++dli)
    {
      writeDirTreeNode(ol,subdd,level+1,ftv,addToIndex);
    }
    endIndexHierarchy(ol,level+1); 
  }

  FileList *fileList=dd->getFiles();
  int fileCount=0;
  if (fileList && fileList->count()>0)
  { 
    FileDef *fd=fileList->first();
    while (fd)
    {
      //static bool allExternals = Config_getBool("ALLEXTERNALS");
      //if ((allExternals && fd->isLinkable()) || fd->isLinkableInProject())
      //{
      //  fileCount++;
      //}
      bool genSourceFile;
      if (fileVisibleInIndex(fd,genSourceFile))
      {
        fileCount++;
      }
      else if (genSourceFile)
      {
        fileCount++;
      }
      fd=fileList->next();
    }
    if (fileCount>0)
    {
      startIndexHierarchy(ol,level+1);
      fd=fileList->first();
      while (fd)
      {
        bool doc,src;
        doc = fileVisibleInIndex(fd,src);
        QCString reference;
        QCString outputBase;
        if (doc) 
        {
          reference  = fd->getReference();
          outputBase = fd->getOutputFileBase();
        }
        if (doc || src)
        {
          ol.startIndexListItem();
          ol.startIndexItem(reference,outputBase);
          ol.parseText(fd->displayName());
          ol.endIndexItem(reference,outputBase);
          ol.endIndexListItem();
          if (ftv && (src || doc))
          {
            ftv->addContentsItem(FALSE,
                fd->displayName(),
                reference,outputBase,
                0,FALSE,FALSE,fd); 
          }
        }
        fd=fileList->next();
      }
      endIndexHierarchy(ol,level+1); 
    }
  }

  if (tocExpand && addToIndex)
  {
    // write files of this directory
    if (fileCount>0)
    {
      FileDef *fd=fileList->first();
      while (fd)
      {
        //static bool allExternals = Config_getBool("ALLEXTERNALS");
        //if ((allExternals && fd->isLinkable()) || fd->isLinkableInProject())
        bool doc,src;
        doc = fileVisibleInIndex(fd,src);
        if (doc)
        {
          addMembersToIndex(fd,LayoutDocManager::File,fd->displayName(),QCString(),TRUE);
        }
        else if (src)
        {
          Doxygen::indexList->addContentsItem(
               FALSE, convertToHtml(fd->name(),TRUE), 0, 
               fd->getSourceFileBase(), 0, FALSE, TRUE, fd);
        }
        fd=fileList->next();
      }
    }
  }
  ol.endIndexListItem();

  if (addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }
  if (ftv)
  {
    ftv->decContentsDepth();
  }
}

static void writeDirHierarchy(OutputList &ol, FTVHelp* ftv,bool addToIndex)
{
  if (ftv)
  {
    ol.pushGeneratorState(); 
    ol.disable(OutputGenerator::Html);
  }
  static bool fullPathNames = Config_getBool("FULL_PATH_NAMES");
  startIndexHierarchy(ol,0);
  if (fullPathNames)
  {
    SDict<DirDef>::Iterator dli(*Doxygen::directories);
    DirDef *dd;
    for (dli.toFirst();(dd=dli.current());++dli)
    {
      if (dd->getOuterScope()==Doxygen::globalScope) 
      {
        writeDirTreeNode(ol,dd,0,ftv,addToIndex);
      }
    }
  }
  if (ftv)
  {
    FileNameListIterator fnli(*Doxygen::inputNameList); 
    FileName *fn;
    for (fnli.toFirst();(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (;(fd=fni.current());++fni)
      {
        static bool fullPathNames = Config_getBool("FULL_PATH_NAMES");
        if (!fullPathNames || fd->getDirDef()==0) // top level file
        {
          bool doc,src;
          doc = fileVisibleInIndex(fd,src);
          QCString reference, outputBase;
          if (doc)
          {
            reference = fd->getReference();
            outputBase = fd->getOutputFileBase();
          }
          if (doc || src)
          {
            ftv->addContentsItem(FALSE,fd->displayName(),          
                                 reference, outputBase, 0,         
                                 FALSE,FALSE,fd);
          }
          if (addToIndex)
          {
            if (doc)
            {
              addMembersToIndex(fd,LayoutDocManager::File,fd->displayName(),QCString(),TRUE);
            }
            else if (src)
            {
              Doxygen::indexList->addContentsItem(
                  FALSE, convertToHtml(fd->name(),TRUE), 0, 
                  fd->getSourceFileBase(), 0, FALSE, TRUE, fd);
            }
          }
        }
      }
    }
  }
  endIndexHierarchy(ol,0); 
  if (ftv)
  {
    ol.popGeneratorState(); 
  }
}


//----------------------------------------------------------------------------

static void writeClassTreeForList(OutputList &ol,ClassSDict *cl,bool &started,FTVHelp* ftv,bool addToIndex)
{
  ClassSDict::Iterator cli(*cl);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    //printf("class %s hasVisibleRoot=%d isVisibleInHierarchy=%d\n",
    //             cd->name().data(),
    //              hasVisibleRoot(cd->baseClasses()),
    //              cd->isVisibleInHierarchy()
    //      );
    bool b;
    if (cd->getLanguage()==SrcLangExt_VHDL)
    {
      if (!(VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS)      
      {
        continue;
      }
      b=!hasVisibleRoot(cd->subClasses());
    }
    else
    {
      b=!hasVisibleRoot(cd->baseClasses());
    }

    if (b)  //filter on root classes
    {
      if (cd->isVisibleInHierarchy()) // should it be visible
      {
        if (!started)
        {
          startIndexHierarchy(ol,0);
          if (addToIndex)
          {
            Doxygen::indexList->incContentsDepth();
          }
          started=TRUE;
        }
        ol.startIndexListItem();
        bool hasChildren = !cd->visited && classHasVisibleChildren(cd); 
        //printf("list: Has children %s: %d\n",cd->name().data(),hasChildren);
        if (cd->isLinkable())
        {
          //printf("Writing class %s isLinkable()=%d isLinkableInProject()=%d cd->templateMaster()=%p\n",
          //    cd->displayName().data(),cd->isLinkable(),cd->isLinkableInProject(),cd->templateMaster());
          ol.startIndexItem(cd->getReference(),cd->getOutputFileBase());
          ol.parseText(cd->displayName());
          ol.endIndexItem(cd->getReference(),cd->getOutputFileBase());
          if (cd->isReference()) 
          {
            ol.startTypewriter(); 
            ol.docify(" [external]");
            ol.endTypewriter();
          }
          if (addToIndex)
          {
            if (cd->getLanguage()!=SrcLangExt_VHDL) // prevents double insertion in Design Unit List
            	  Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE);
          }
          if (ftv)
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd); 
          }
        }
        else
        {
          ol.startIndexItem(0,0);
          ol.parseText(cd->displayName());
          ol.endIndexItem(0,0);
          if (addToIndex)
          {
            Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),0,0,0,FALSE,FALSE);
          }
          if (ftv)
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),0,0,0,FALSE,FALSE,cd); 
          }
        }
        if (cd->getLanguage()==SrcLangExt_VHDL && hasChildren) 
        {
          writeClassTree(ol,cd->baseClasses(),cd->visited,1,ftv,addToIndex);
          cd->visited=TRUE;
        }
        else if (hasChildren)
        {
          writeClassTree(ol,cd->subClasses(),cd->visited,1,ftv,addToIndex);
          cd->visited=TRUE;
        }
        ol.endIndexListItem();
      }
    }
  }
}

static void writeClassHierarchy(OutputList &ol, FTVHelp* ftv,bool addToIndex)
{
  initClassHierarchy(Doxygen::classSDict);
  initClassHierarchy(Doxygen::hiddenClasses);
  if (ftv)
  {
    ol.pushGeneratorState(); 
    ol.disable(OutputGenerator::Html);
  }
  bool started=FALSE;
  writeClassTreeForList(ol,Doxygen::classSDict,started,ftv,addToIndex);
  writeClassTreeForList(ol,Doxygen::hiddenClasses,started,ftv,addToIndex);
  if (started) 
  {
    endIndexHierarchy(ol,0);
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
  }
  if (ftv)
  {
    ol.popGeneratorState(); 
  }
}

//----------------------------------------------------------------------------

static int countClassesInTreeList(const ClassSDict &cl)
{
  int count=0;
  ClassSDict::Iterator cli(cl);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    if (!hasVisibleRoot(cd->baseClasses())) // filter on root classes
    {
      if (cd->isVisibleInHierarchy()) // should it be visible
      {
        if (cd->subClasses()) // should have sub classes
        {
          count++;
        }
      }
    }
  }
  return count;
}

static int countClassHierarchy()
{
  int count=0;
  initClassHierarchy(Doxygen::classSDict);
  initClassHierarchy(Doxygen::hiddenClasses);
  count+=countClassesInTreeList(*Doxygen::classSDict);
  count+=countClassesInTreeList(*Doxygen::hiddenClasses);
  return count;
}

//----------------------------------------------------------------------------

static void writeHierarchicalIndex(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.pushGeneratorState();
  //1.{
  ol.disable(OutputGenerator::Man);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trClassHierarchy();
  bool addToIndex = lne==0 || lne->visible();

  startFile(ol,"hierarchy",0, title, HLI_Hierarchy);
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();

  if (Config_getBool("HAVE_DOT") && Config_getBool("GRAPHICAL_HIERARCHY"))
  {
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.startParagraph();
    ol.startTextLink("inherits",0);
    ol.parseText(theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.endParagraph();
    ol.enable(OutputGenerator::Latex);
    ol.enable(OutputGenerator::RTF);
  }
  ol.parseText(lne ? lne->intro() : theTranslator->trClassHierarchyDescription());
  ol.endTextBlock();

  // ---------------
  // Static class hierarchy for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputGenerator::Html);
  Doxygen::indexList->disable();

  writeClassHierarchy(ol,0,addToIndex);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  //2.}

  // ---------------
  // Dynamic class hierarchical index for HTML
  // ---------------
  ol.pushGeneratorState(); 
  //2.{
  ol.disableAllBut(OutputGenerator::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,0,"hierarchy",0,TRUE,TRUE); 
    }
    FTVHelp* ftv = new FTVHelp(FALSE);
    writeClassHierarchy(ol,ftv,addToIndex);
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.pushGeneratorState(); 
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString(outStr);
    ol.popGeneratorState();
    delete ftv;
  }
  ol.popGeneratorState();
  //2.}
  // ------

  endFile(ol);
  ol.popGeneratorState();
  //1.}
}

//----------------------------------------------------------------------------

static void writeGraphicalClassHierarchy(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.disableAllBut(OutputGenerator::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trClassHierarchy();
  startFile(ol,"inherits",0,title,HLI_Hierarchy,FALSE,"hierarchy");
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();
  ol.startParagraph();
  ol.startTextLink("hierarchy",0);
  ol.parseText(theTranslator->trGotoTextualHierarchy());
  ol.endTextLink();
  ol.endParagraph();
  ol.endTextBlock();
  DotGfxHierarchyTable g;
  ol.writeGraphicalHierarchy(g);
  endFile(ol);
  ol.enableAll();
}

//----------------------------------------------------------------------------

static void countFiles(int &htmlFiles,int &files)
{
  htmlFiles=0;
  files=0;
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      bool doc,src;
      doc = fileVisibleInIndex(fd,src);
      if (doc || src)
      {
        htmlFiles++;
      }
      if (doc)
      {
        files++;
      }
    }
  }
}

//----------------------------------------------------------------------------

static void writeFileIndex(OutputList &ol)
{
  if (documentedHtmlFiles==0) return;

  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  if (documentedFiles==0) ol.disableAllBut(OutputGenerator::Html);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileList);
  if (lne==0) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Files); // fall back
  QCString title = lne ? lne->title() : theTranslator->trFileList();
  bool addToIndex = lne==0 || lne->visible();

  startFile(ol,"files",0,title,HLI_Files);
  startTitle(ol,0);
  //if (!Config_getString("PROJECT_NAME").isEmpty()) 
  //{
  //  title.prepend(Config_getString("PROJECT_NAME")+" ");
  //}
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,title,0,"files",0,TRUE,TRUE); 
    Doxygen::indexList->incContentsDepth();
  }

  ol.parseText(lne ? lne->intro() : theTranslator->trFileListDescription(Config_getBool("EXTRACT_ALL")));
  ol.endTextBlock();

  // ---------------
  // Flat file index
  // ---------------

  // 1. {
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);

  OutputNameDict outputNameDict(1009);
  OutputNameList outputNameList;
  outputNameList.setAutoDelete(TRUE);
  
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    // re-sort input files in (dir,file) output order instead of (file,dir) input order 
    FileName *fn=Doxygen::inputNameList->first();
    while (fn)
    {
      FileDef *fd=fn->first();
      while (fd)
      {
        QCString path=fd->getPath();
        if (path.isEmpty()) path="[external]";
        FileList *fl = outputNameDict.find(path);
        if (fl)
        {
          fl->inSort(fd);
          //printf("+ inserting %s---%s\n",fd->getPath().data(),fd->name().data());
        }
        else
        {
          //printf("o inserting %s---%s\n",fd->getPath().data(),fd->name().data());
          fl = new FileList(path);
          fl->inSort(fd);
          outputNameList.inSort(fl);
          outputNameDict.insert(path,fl);
        }
        fd=fn->next();
      }
      fn=Doxygen::inputNameList->next();
    }
  }
  
  ol.startIndexList();
  FileList *fl=0;
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    fl = outputNameList.first();
  }
  else
  {
    fl = Doxygen::inputNameList->first();
  }
  while (fl)
  {
    FileDef *fd=fl->first();
    while (fd)
    {
      //printf("Found filedef %s\n",fd->name().data());
      bool doc = fd->isLinkableInProject();
      bool src = fd->generateSourceFile();
      bool nameOk = !fd->isDocumentationFile();
      if (nameOk && (doc || src) && 
              !fd->isReference())
      {
        QCString path;
        if (Config_getBool("FULL_PATH_NAMES")) 
        {
          path=stripFromPath(fd->getPath().copy());
        }
        QCString fullName=fd->name();
        if (!path.isEmpty()) 
        {
          if (path.at(path.length()-1)!='/') fullName.prepend("/");
          fullName.prepend(path);
        }

        ol.startIndexKey();
        ol.docify(path);
        if (doc)
        {
          ol.writeObjectLink(0,fd->getOutputFileBase(),0,fd->name());
          //if (addToIndex)
          //{
          //  addMembersToIndex(fd,LayoutDocManager::File,fullName,QCString());
          //}
        }
        else
        {
          ol.startBold();
          ol.docify(fd->name());
          ol.endBold();
          //if (addToIndex)
          //{
          //  Doxygen::indexList->addContentsItem(FALSE,fullName,0,0,0);
          //}
        }
        if (src)
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          ol.docify(" ");
          ol.startTextLink(fd->includeName(),0);
          ol.docify("[");
          ol.parseText(theTranslator->trCode());
          ol.docify("]");
          ol.endTextLink();
          ol.popGeneratorState();
        }
        ol.endIndexKey();
        bool hasBrief = !fd->briefDescription().isEmpty();
        ol.startIndexValue(hasBrief);
        if (hasBrief)
        {
          //ol.docify(" (");
          ol.parseDoc(
              fd->briefFile(),fd->briefLine(),
              fd,0,
              fd->briefDescription(TRUE),
              FALSE, // index words
              FALSE, // isExample
              0,     // example name
              TRUE,  // single line
              TRUE   // link from index
             );
          //ol.docify(")");
        }
        ol.endIndexValue(fd->getOutputFileBase(),hasBrief);
        //ol.popGeneratorState();
        // --------------------------------------------------------
      }
      fd=fl->next();
    }
    if (Config_getBool("FULL_PATH_NAMES"))
    {
      fl=outputNameList.next();
    }
    else
    {
      fl=Doxygen::inputNameList->next();
    }
  }
  ol.endIndexList();

  // 1. }
  ol.popGeneratorState();

  // ---------------
  // Hierarchical file index for HTML
  // ---------------
  ol.pushGeneratorState(); 
  ol.disableAllBut(OutputGenerator::Html);

  FTVHelp* ftv = new FTVHelp(FALSE);
  writeDirHierarchy(ol,ftv,addToIndex);
  QGString outStr;
  FTextStream t(&outStr);
  ftv->generateTreeViewInline(t);
  ol.writeString(outStr);
  delete ftv;

  ol.popGeneratorState();
  // ------

  if (addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
static int countNamespaces()
{
  int count=0;
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject()) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writeClassTree(ClassSDict *clDict,FTVHelp *ftv,bool addToIndex,bool globalOnly)
{
  if (clDict)
  {
    ClassSDict::Iterator cli(*clDict);
    ClassDef *cd;
    for (;(cd=cli.current());++cli)
    {
      if (cd->getLanguage()==SrcLangExt_VHDL) 
      {
        if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS || 
            (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS
           )// no architecture
        {
          continue;
        }
        if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS)
        {
          QCString n=cd->name();
          cd->setClassName(n.data());
        }         
      }

      if (!globalOnly || 
           cd->getOuterScope()==0 || 
           cd->getOuterScope()==Doxygen::globalScope 
         ) 
      {
        int count=0;
        if (cd->getClassSDict())
        {
          ClassSDict::Iterator ccit(*cd->getClassSDict());
          ClassDef *ccd;
          for (;(ccd=ccit.current());++ccit)
          {
            if (ccd->isLinkableInProject() && ccd->templateMaster()==0) 
            {
              count++;
            }
          }
        }
        if (classVisibleInIndex(cd) && cd->templateMaster()==0) 
        { 
          ftv->addContentsItem(count>0,cd->displayName(FALSE),cd->getReference(),
              cd->getOutputFileBase(),cd->anchor(),FALSE,TRUE,cd); 
          if (addToIndex && 
              /*cd->partOfGroups()==0 &&*/
              (cd->getOuterScope()==0 || 
               cd->getOuterScope()->definitionType()!=Definition::TypeClass
              )
             )
          {
            addMembersToIndex(cd,LayoutDocManager::Class,cd->displayName(FALSE),cd->anchor(),cd->partOfGroups()==0);
          }
          if (count>0)
          {
            ftv->incContentsDepth();
            writeClassTree(cd->getClassSDict(),ftv,addToIndex,FALSE);
            ftv->decContentsDepth();
          }
        }
      }
    }
  }
}

static bool containsVisibleChild(NamespaceDef *nd,bool includeClasses)
{
  if (nd->getNamespaceSDict())
  {
    NamespaceSDict::Iterator cnli(*nd->getNamespaceSDict());
    NamespaceDef *cnd;
    for (cnli.toFirst();(cnd=cnli.current());++cnli)
    {
      if (cnd->isLinkable() && cnd->localName().find('@')==-1)
      {
        return TRUE;
      }
      else if (containsVisibleChild(cnd,includeClasses))
      {
        return TRUE;
      }
    }
  }
  if (includeClasses && nd->getClassSDict())
  {
    ClassSDict::Iterator cli(*nd->getClassSDict());
    ClassDef *cd;
    for (;(cd=cli.current());++cli)
    {
      if (cd->isLinkableInProject() && cd->templateMaster()==0) 
      { 
        return TRUE;
      }
    }
  }
  return FALSE;
}

static void writeNamespaceTree(NamespaceSDict *nsDict,FTVHelp *ftv,
                               bool rootOnly,bool showClasses,bool addToIndex)
{
  if (nsDict)
  {
    NamespaceSDict::Iterator nli(*nsDict);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      if (nd->localName().find('@')==-1 && 
          (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
      {

        bool hasChildren = containsVisibleChild(nd,showClasses);
        bool isLinkable  = nd->isLinkableInProject();

        QCString ref; 
        QCString file; 
        if (isLinkable)
        {
          ref  = nd->getReference();
          file = nd->getOutputFileBase();
          if (nd->getLanguage()==SrcLangExt_VHDL) // UGLY HACK
          {
            file=file.replace(0,qstrlen("namespace"),"class");
          }
        }

        if (isLinkable || hasChildren)
        {
          ftv->addContentsItem(hasChildren,nd->localName(),ref,file,0,FALSE,TRUE,nd); 

          if (addToIndex)
          {
            Doxygen::indexList->addContentsItem(hasChildren,nd->localName(),ref,file,QCString(),
                hasChildren && !file.isEmpty(),addToIndex);
          }

          //printf("*** writeNamespaceTree count=%d addToIndex=%d showClasses=%d classCount=%d\n",
          //    count,addToIndex,showClasses,classCount);
          if (hasChildren)
          {
            if (addToIndex) Doxygen::indexList->incContentsDepth();
            ftv->incContentsDepth();
            writeNamespaceTree(nd->getNamespaceSDict(),ftv,FALSE,showClasses,addToIndex);
            if (showClasses) 
            {
              writeClassTree(nd->getClassSDict(),ftv,addToIndex,FALSE);
            }
            ftv->decContentsDepth();
            if (addToIndex) Doxygen::indexList->decContentsDepth();
          }
        }
      }
    }
  }
}


static void writeNamespaceIndex(OutputList &ol)
{
  if (documentedNamespaces==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceList);
  if (lne==0) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Namespaces); // fall back
  QCString title = lne ? lne->title() : theTranslator->trNamespaceList();
  bool addToIndex = lne==0 || lne->visible();
  startFile(ol,"namespaces",0,title,HLI_Namespaces);
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trNamespaceListDescription(Config_getBool("EXTRACT_ALL")));
  ol.endTextBlock();

  bool first=TRUE;

  // ---------------
  // Linear namespace index for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject())
    {
      if (first)
      {
        ol.startIndexList();
        first=FALSE;
      }
      //ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      ol.startIndexKey();
      if (nd->getLanguage()==SrcLangExt_VHDL)
      {
        ol.writeObjectLink(0, nd->getOutputFileBase().replace(0,qstrlen("namespace"),"class"),0,nd->displayName());
      }
      else
      {
        ol.writeObjectLink(0,nd->getOutputFileBase(),0,nd->displayName());
      }
      ol.endIndexKey();

      bool hasBrief = !nd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        //ol.docify(" (");
        ol.parseDoc(
                 nd->briefFile(),nd->briefLine(),
                 nd,0,
                 nd->briefDescription(TRUE),
                 FALSE, // index words
                 FALSE, // isExample
                 0,     // example name
                 TRUE,  // single line
                 TRUE   // link from index
                );
        //ol.docify(")");
      }
      ol.endIndexValue(nd->getOutputFileBase(),hasBrief);

    }
  }
  if (!first) ol.endIndexList();

  ol.popGeneratorState();

  // ---------------
  // Hierarchical namespace index for HTML
  // ---------------
  ol.pushGeneratorState(); 
  ol.disableAllBut(OutputGenerator::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,0,"namespaces",0,TRUE,TRUE); 
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp* ftv = new FTVHelp(FALSE);
    writeNamespaceTree(Doxygen::namespaceSDict,ftv,TRUE,FALSE,addToIndex);
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.writeString(outStr);
    delete ftv;
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
  }

  ol.popGeneratorState();
  // ------

  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static int countAnnotatedClasses(int *cp)
{
  int count=0;
  int countPrinted=0;
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0) 
    { 
      if (!cd->isEmbeddedInOuterScope())
      {
        countPrinted++;
      }
      count++; 
    }
  }
  *cp = countPrinted;
  return count;
}


static void writeAnnotatedClassList(OutputList &ol)
{
  //LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassList);
  //bool addToIndex = lne==0 || lne->visible();

  ol.startIndexList(); 
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (cd->getLanguage()==SrcLangExt_VHDL && 
        ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKAGECLASS || 
         (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
       ) // no architecture
    {
      continue;
    }
 
    ol.pushGeneratorState();
    if (cd->isEmbeddedInOuterScope())
    {
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::RTF);
    }
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      QCString type=cd->compoundTypeString();
      ol.startIndexKey();
      if (cd->getLanguage()==SrcLangExt_VHDL)
      {
        QCString prot= VhdlDocGen::getProtectionName((VhdlDocGen::VhdlClasses)cd->protection());
        ol.docify(prot.data());
        ol.writeString(" ");
      }
      ol.writeObjectLink(0,cd->getOutputFileBase(),cd->anchor(),cd->displayName());
      ol.endIndexKey();
      bool hasBrief = !cd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        ol.parseDoc(
                 cd->briefFile(),cd->briefLine(),
                 cd,0,
                 cd->briefDescription(TRUE),
                 FALSE,  // indexWords
                 FALSE,  // isExample
                 0,     // example name
                 TRUE,  // single line
                 TRUE   // link from index
                );
      }
      ol.endIndexValue(cd->getOutputFileBase(),hasBrief);

      //if (addToIndex)
      //{
      //  addMembersToIndex(cd,LayoutDocManager::Class,cd->displayName(),cd->anchor());
      //}
    }
    ol.popGeneratorState();
  }
  ol.endIndexList();
}

static QCString letterToLabel(char startLetter)
{
  QCString s(5); 
  if (isId(startLetter))
  {
    s[0]=startLetter; s[1]=0;
  }
  else
  {
    const char hex[]="0123456789abcdef";
    s[0]='0';
    s[1]='x';
    s[2]=hex[startLetter>>4];
    s[3]=hex[startLetter&0xF];
    s[4]=0;
  }
  return s;
}

//----------------------------------------------------------------------------

/** Special class list where sorting takes IGNORE_PREFIX into account. */
class PrefixIgnoreClassList : public ClassList
{
public:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
  {
    ClassDef *c1=(ClassDef *)item1;
    ClassDef *c2=(ClassDef *)item2;

    QCString n1 = c1->className();
    QCString n2 = c2->className();
    return qstricmp (n1.data()+getPrefixIndex(n1), n2.data()+getPrefixIndex(n2));
  }
};

/** Class representing a cell in the alphabetical class index. */
class AlphaIndexTableCell
{
  public:
    AlphaIndexTableCell(int row,int col,uchar letter,ClassDef *cd) : 
      m_letter(letter), m_class(cd), m_row(row), m_col(col) 
    { //printf("AlphaIndexTableCell(%d,%d,%c,%s)\n",row,col,letter!=0 ? letter: '-',
      //       cd!=(ClassDef*)0x8 ? cd->name().data() : "<null>"); 
    }

    ClassDef *classDef() const { return m_class; }
    uchar letter()       const { return m_letter; }
    int row()            const { return m_row; }
    int column()         const { return m_col; }

  private:
    uchar m_letter;
    ClassDef *m_class;
    int m_row;
    int m_col;
};

/** Class representing a row in the alphabetical class index. */
class AlphaIndexTableRows : public QList<AlphaIndexTableCell>
{
  public:
    AlphaIndexTableRows() { setAutoDelete(TRUE); }
};

/** Iterator for the cells in a row of the alphabetical class index. */
class AlphaIndexTableRowsIterator : public QListIterator<AlphaIndexTableCell>
{
  public:
    AlphaIndexTableRowsIterator(const AlphaIndexTableRows &list) : 
      QListIterator<AlphaIndexTableCell>(list) {}
};

/** Class representing the columns in the alphabetical class index. */
class AlphaIndexTableColumns : public QList<AlphaIndexTableRows>
{
  public:
    AlphaIndexTableColumns() { setAutoDelete(TRUE); }
};

// write an alphabetical index of all class with a header for each letter
static void writeAlphabeticalClassList(OutputList &ol)
{
  // What starting letters are used
  bool indexLetterUsed[256];
  memset (indexLetterUsed, 0, sizeof (indexLetterUsed));

  // first count the number of headers
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  uint startLetter=0;
  int headerItems=0;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      if (cd->getLanguage()==SrcLangExt_VHDL && !((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS ))// no architecture
        continue;
	     
      int index = getPrefixIndex(cd->className());
      //printf("name=%s index=%d %d\n",cd->className().data(),index,cd->protection());
      startLetter=toupper(cd->className().at(index))&0xFF;
      indexLetterUsed[startLetter] = true;
    }
  }

  // write quick link index (row of letters)
  QCString alphaLinks = "<div class=\"qindex\">";
  int l;
  for (l=0; l<256; l++)
  {
    if (indexLetterUsed[l])
    {
      if (headerItems) alphaLinks += "&#160;|&#160;";
      headerItems++;
      alphaLinks += (QCString)"<a class=\"qindex\" href=\"#letter_" + 
                    (char)l + "\">" + 
                    (char)l + "</a>";
    }
  }
  alphaLinks += "</div>\n";
  ol.writeString(alphaLinks);


  // the number of columns in the table
  const int columns = Config_getInt("COLS_IN_ALPHA_INDEX");

  int i,j;
  int totalItems = headerItems*2 + annotatedClasses;      // number of items in the table (headers span 2 items)
  int rows = (totalItems + columns - 1)/columns;          // number of rows in the table

  //printf("headerItems=%d totalItems=%d columns=%d rows=%d itemsInLastRow=%d\n",
  //    headerItems,totalItems,columns,rows,itemsInLastRow);

  // Keep a list of classes for each starting letter
  PrefixIgnoreClassList classesByLetter[256];
  AlphaIndexTableColumns tableColumns;

  // fill the columns with the class list (row elements in each column,
  // expect for the columns with number >= itemsInLastRow, which get one
  // item less.
  //int icount=0;
  startLetter=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (cd->getLanguage()==SrcLangExt_VHDL && !((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS ))// no architecture
      continue;
    
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      int index = getPrefixIndex(cd->className());
      startLetter=toupper(cd->className().at(index))&0xFF;
      // Do some sorting again, since the classes are sorted by name with 
      // prefix, which should be ignored really.
      if (cd->getLanguage()==SrcLangExt_VHDL)
      {
        if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS )// no architecture
          classesByLetter[startLetter].inSort(cd);
      }
      else
      classesByLetter[startLetter].inSort(cd);
    }
  }

  #define NEXT_ROW()                           \
    do                                         \
    {                                          \
      if (row>maxRows) maxRows=row;            \
      if (row>=rows && col<columns)            \
      {                                        \
        col++;                                 \
        row=0;                                 \
        tableRows = new AlphaIndexTableRows;   \
        tableColumns.append(tableRows);        \
      }                                        \
    }                                          \
    while(0)                                   \

  AlphaIndexTableRows *tableRows = new AlphaIndexTableRows;
  tableColumns.append(tableRows);
  int col=0,row=0,maxRows=0;
  for (l=0; l<256; l++)
  {
    if (classesByLetter[l].count()>0)
    {
      // add special header cell
      tableRows->append(new AlphaIndexTableCell(row,col,(uchar)l,(ClassDef*)0x8));
      row++;
      tableRows->append(new AlphaIndexTableCell(row,col,0,(ClassDef*)0x8));
      row++;
      tableRows->append(new AlphaIndexTableCell(row,col,0,classesByLetter[l].at(0)));
      row++; 
      NEXT_ROW();
      for (i=1; i<(int)classesByLetter[l].count(); i++)
      {
        // add normal cell
        tableRows->append(new AlphaIndexTableCell(row,col,0,classesByLetter[l].at(i)));
        row++;
        NEXT_ROW();
      }
    }
  }

  // create row iterators for each column
  AlphaIndexTableRowsIterator **colIterators = new AlphaIndexTableRowsIterator*[columns];
  for (i=0;i<columns;i++)
  {
    if (i<(int)tableColumns.count())
    {
      colIterators[i] = new AlphaIndexTableRowsIterator(*tableColumns.at(i));
    }
    else // empty column
    {
      colIterators[i] = 0;
    }
  }

  ol.writeString("<table style=\"margin: 10px; white-space: nowrap;\" align=\"center\" width=\"95%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
  // generate table
  for (i=0;i<=maxRows;i++) // foreach table row
  {
    //printf("writing row %d\n",i);
    //ol.nextTableRow();
    ol.writeString("<tr>");
    // the last column may contain less items then the others
    //int colsInRow = (i<rows-1) ? columns : itemsInLastRow; 
    //printf("row [%d]\n",i);
    for (j=0;j<columns;j++) // foreach table column
    {
      if (colIterators[j])
      {
        AlphaIndexTableCell *cell = colIterators[j]->current();
        if (cell)
        {
          if (cell->row()==i)
          {
            if (cell->letter()!=0)
            {
              QCString s = letterToLabel(cell->letter());
              ol.writeString("<td rowspan=\"2\" valign=\"bottom\">");
              ol.writeString("<a name=\"letter_");
              ol.writeString(s);
              ol.writeString("\"></a>");
              ol.writeString("<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">"
                  "<tr>"
                  "<td><div class=\"ah\">&#160;&#160;"); 
              ol.writeString(s);
              ol.writeString(         "&#160;&#160;</div>"
                  "</td>"
                  "</tr>"
                  "</table>\n");
            }
            else if (cell->classDef()!=(ClassDef*)0x8)
            {
              cd = cell->classDef();
              ol.writeString("<td valign=\"top\">");
              QCString namesp,cname;
              //if (cd->getNamespaceDef()) namesp=cd->getNamespaceDef()->displayName();
              //QCString cname=cd->className();
              extractNamespaceName(cd->name(),cname,namesp);
              QCString nsDispName;
              SrcLangExt lang = cd->getLanguage();
              QCString sep = getLanguageSpecificSeparator(lang);
              if (sep!="::")
              {
                nsDispName=substitute(namesp,"::",sep);
                cname=substitute(cname,"::",sep);
              }
              else
              {
                nsDispName=namesp;
              }

              ol.writeObjectLink(cd->getReference(),
                  cd->getOutputFileBase(),cd->anchor(),cname);
              if (!namesp.isEmpty())
              {
                ol.docify(" (");
                NamespaceDef *nd = getResolvedNamespace(namesp);
                if (nd && nd->isLinkable())
                {
                  ol.writeObjectLink(nd->getReference(),
                      nd->getOutputFileBase(),0,nsDispName);
                }
                else
                {
                  ol.docify(nsDispName);
                }
                ol.docify(")");
              }
              ol.writeNonBreakableSpace(3);
            }
            ++(*colIterators[j]);
            if (cell->letter()!=0 || cell->classDef()!=(ClassDef*)0x8)
            {
              ol.writeString("</td>");
            }
          }
        }
        else
        {
          ol.writeString("<td></td>");
        }
      }
    }
    ol.writeString("</tr>\n");
  }
  ol.writeString("</table>\n");
  
  ol.writeString(alphaLinks);

  // release the temporary memory
  for (i=0;i<columns;i++)
  {
    delete colIterators[i];
  }
  delete[] colIterators;
}

//----------------------------------------------------------------------------

static void writeAlphabeticalIndex(OutputList &ol)
{
  if (annotatedClasses==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassIndex);
  QCString title = lne ? lne->title() : theTranslator->trCompoundIndex();
  bool addToIndex = lne==0 || lne->visible();

  startFile(ol,"classes",0,title,HLI_Classes); 

  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,title,0,"classes",0,FALSE,TRUE); 
  }

  ol.startContents();
  writeAlphabeticalClassList(ol);
  endFile(ol); // contains ol.endContents()

  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static void writeAnnotatedIndex(OutputList &ol)
{
  //printf("writeAnnotatedIndex: count=%d printed=%d\n",
  //    annotatedClasses,annotatedClassesPrinted);
  if (annotatedClasses==0) return;
  
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  if (annotatedClassesPrinted==0)
  {
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
  }
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassList);
  if (lne==0) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Classes); // fall back
  QCString title = lne ? lne->title() : theTranslator->trCompoundList();
  bool addToIndex = lne==0 || lne->visible();

  if (Config_getBool("OPTIMIZE_OUTPUT_VHDL")) 
  {
    VhdlDocGen::findConstraintFile(lne);
  }
  
  startFile(ol,"annotated",0,title,HLI_Annotated);

  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);

  ol.startContents();

  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trCompoundListDescription());
  ol.endTextBlock();

  // ---------------
  // Linear class index for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  Doxygen::indexList->disable();

  writeAnnotatedClassList(ol);

  Doxygen::indexList->enable();
  ol.popGeneratorState();

  // ---------------
  // Hierarchical class index for HTML
  // ---------------
  ol.pushGeneratorState(); 
  ol.disableAllBut(OutputGenerator::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,0,"annotated",0,TRUE,TRUE); 
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp* ftv = new FTVHelp(FALSE);
    writeNamespaceTree(Doxygen::namespaceSDict,ftv,TRUE,TRUE,addToIndex);
    writeClassTree(Doxygen::classSDict,ftv,addToIndex,TRUE);
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.writeString(outStr);
    delete ftv;
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
  }

  ol.popGeneratorState();
  // ------
  
  endFile(ol); // contains ol.endContents()
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
static void writeClassLinkForMember(OutputList &ol,MemberDef *md,const char *separator,
                             QCString &prevClassName)
{
  ClassDef *cd=md->getClassDef();
  if ( cd && prevClassName!=cd->displayName())
  {
    ol.docify(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        cd->displayName());
    ol.writeString("\n");
    prevClassName = cd->displayName();
  }
}

static void writeFileLinkForMember(OutputList &ol,MemberDef *md,const char *separator,
                             QCString &prevFileName)
{
  FileDef *fd=md->getFileDef();
  if (fd && prevFileName!=fd->name())
  {
    ol.docify(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        fd->name());
    ol.writeString("\n");
    prevFileName = fd->name();
  }
}

static void writeNamespaceLinkForMember(OutputList &ol,MemberDef *md,const char *separator,
                             QCString &prevNamespaceName)
{
  NamespaceDef *nd=md->getNamespaceDef();
  if (nd && prevNamespaceName!=nd->name())
  {
    ol.docify(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        nd->name());
    ol.writeString("\n");
    prevNamespaceName = nd->name();
  }
}

static void writeMemberList(OutputList &ol,bool useSections,int page,
                            MemberIndexList memberLists[MEMBER_INDEX_ENTRIES],
                            DefinitionIntf::DefType type)
{
  int pi;
  // page==-1 => write all member indices to one page (used when total members is small)
  // page!=-1 => write all member for this page only (used when total member is large)
  int startIndex = page==-1 ? 0                      : page;
  int endIndex   = page==-1 ? MEMBER_INDEX_ENTRIES-1 : page;
  ASSERT((int)type<3);

  typedef void (*writeLinkForMember_t)(OutputList &ol,MemberDef *md,const char *separator,
                                   QCString &prevNamespaceName);

  // each index tab has its own write function
  static writeLinkForMember_t writeLinkForMemberMap[3] = 
  { 
    &writeClassLinkForMember, 
    &writeFileLinkForMember,
    &writeNamespaceLinkForMember
  };
  QCString prevName;
  QCString prevDefName;
  bool first=TRUE;
  bool firstSection=TRUE;
  bool firstItem=TRUE;
  for (pi=startIndex; pi<=endIndex; pi++) // page==-1 => pi=[0..127], page!=-1 => pi=page 
  {
    MemberIndexList *ml = &memberLists[pi];
    if (ml->count()==0) continue;
    ml->sort();
    QListIterator<MemberDef> mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      const char *sep;
      bool isFunc=!md->isObjCMethod() && 
        (md->isFunction() || md->isSlot() || md->isSignal()); 
      QCString name=md->name();
      int startIndex = getPrefixIndex(name);
      if (QCString(name.data()+startIndex)!=prevName) // new entry
      {
        if ((prevName.isEmpty() || 
            tolower(name.at(startIndex))!=tolower(prevName.at(0))) && 
            useSections) // new section
        {
          if (!firstItem)    ol.endItemListItem();
          if (!firstSection) ol.endItemList();
          char cl[2];
          cl[0] = tolower(name.at(startIndex));
          cl[1] = 0;
          QCString cs = letterToLabel(cl[0]);
          QCString anchor=(QCString)"index_"+cs;
          QCString title=(QCString)"- "+cl+" -";
          ol.startSection(anchor,title,SectionInfo::Subsection);
          ol.docify(title);
          ol.endSection(anchor,SectionInfo::Subsection);
          ol.startItemList();
          firstSection=FALSE;
          firstItem=TRUE;
        }
        else if (!useSections && first)
        {
          ol.startItemList();
          first=FALSE;
        }

        // member name
        if (!firstItem) ol.endItemListItem();
        ol.startItemListItem();
        firstItem=FALSE;
        ol.docify(name);
        if (isFunc) ol.docify("()");
        ol.writeString("\n");

        // link to class
        prevDefName="";
        sep = ": ";
        prevName = name.data()+startIndex;
      }
      else // same entry
      {
        sep = ", ";
        // link to class for other members with the same name
      }
      // write the link for the specific list type
      writeLinkForMemberMap[(int)type](ol,md,sep,prevDefName);
    }
  }
  if (!firstItem) ol.endItemListItem();
  ol.endItemList();
}

//----------------------------------------------------------------------------

void initClassMemberIndices()
{
  int i=0;
  int j=0;
  for (j=0;j<CMHL_Total;j++)
  {
    documentedClassMembers[j]=0;
    for (i=0;i<MEMBER_INDEX_ENTRIES;i++) 
    {
      g_memberIndexLetterUsed[j][i].clear();
    }
  }
}

void addClassMemberNameToIndex(MemberDef *md)
{
  static bool hideFriendCompounds = Config_getBool("HIDE_FRIEND_COMPOUNDS");
  ClassDef *cd=0;

 
  
  if (md->isLinkableInProject() && 
      (cd=md->getClassDef())    && 
      cd->isLinkableInProject() &&
      cd->templateMaster()==0)
  {
    QCString n = md->name();
    int index = getPrefixIndex(n);
    uchar charCode = (uchar)n.at(index);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (!n.isEmpty()) 
    {
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" || 
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        g_memberIndexLetterUsed[CMHL_All][letter].append(md);
        documentedClassMembers[CMHL_All]++;
      }
      if (md->isFunction()  || md->isSlot() || md->isSignal())
      {
        g_memberIndexLetterUsed[CMHL_Functions][letter].append(md);
        documentedClassMembers[CMHL_Functions]++;
      } 
      else if (md->isVariable())
      {
        g_memberIndexLetterUsed[CMHL_Variables][letter].append(md);
        documentedClassMembers[CMHL_Variables]++;
      }
      else if (md->isTypedef())
      {
        g_memberIndexLetterUsed[CMHL_Typedefs][letter].append(md);
        documentedClassMembers[CMHL_Typedefs]++;
      }
      else if (md->isEnumerate())
      {
        g_memberIndexLetterUsed[CMHL_Enums][letter].append(md);
        documentedClassMembers[CMHL_Enums]++;
      }
      else if (md->isEnumValue())
      {
        g_memberIndexLetterUsed[CMHL_EnumValues][letter].append(md);
        documentedClassMembers[CMHL_EnumValues]++;
      }
      else if (md->isProperty())
      {
        g_memberIndexLetterUsed[CMHL_Properties][letter].append(md);
        documentedClassMembers[CMHL_Properties]++;
      }
      else if (md->isEvent())
      {
        g_memberIndexLetterUsed[CMHL_Events][letter].append(md);
        documentedClassMembers[CMHL_Events]++;
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        g_memberIndexLetterUsed[CMHL_Related][letter].append(md);
        documentedClassMembers[CMHL_Related]++;
      }
    }
  }
}

//----------------------------------------------------------------------------

void initNamespaceMemberIndices()
{
  int i=0;
  int j=0;
  for (j=0;j<NMHL_Total;j++)
  {
    documentedNamespaceMembers[j]=0;
    for (i=0;i<MEMBER_INDEX_ENTRIES;i++) 
    {
      g_namespaceIndexLetterUsed[j][i].clear();
    }
  }
}

void addNamespaceMemberNameToIndex(MemberDef *md)
{
  NamespaceDef *nd=md->getNamespaceDef();
  if (nd && nd->isLinkableInProject() && md->isLinkableInProject())
  {
    QCString n = md->name();
    int index = getPrefixIndex(n);
    uchar charCode = (uchar)n.at(index);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (!n.isEmpty()) 
    {
      g_namespaceIndexLetterUsed[NMHL_All][letter].append(md);
      documentedNamespaceMembers[NMHL_All]++;

      if (md->isFunction()) 
      {
        g_namespaceIndexLetterUsed[NMHL_Functions][letter].append(md);
        documentedNamespaceMembers[NMHL_Functions]++;
      }
      else if (md->isVariable()) 
      {
        g_namespaceIndexLetterUsed[NMHL_Variables][letter].append(md);
        documentedNamespaceMembers[NMHL_Variables]++;
      }
      else if (md->isTypedef())
      {
        g_namespaceIndexLetterUsed[NMHL_Typedefs][letter].append(md);
        documentedNamespaceMembers[NMHL_Typedefs]++;
      }
      else if (md->isEnumerate())
      {
        g_namespaceIndexLetterUsed[NMHL_Enums][letter].append(md);
        documentedNamespaceMembers[NMHL_Enums]++;
      }
      else if (md->isEnumValue())
      {
        g_namespaceIndexLetterUsed[NMHL_EnumValues][letter].append(md);
        documentedNamespaceMembers[NMHL_EnumValues]++;
      }
    }
  }
}

//----------------------------------------------------------------------------

void initFileMemberIndices()
{
  int i=0;
  int j=0;
  for (j=0;j<NMHL_Total;j++)
  {
    documentedFileMembers[j]=0;
    for (i=0;i<MEMBER_INDEX_ENTRIES;i++) 
    {
      g_fileIndexLetterUsed[j][i].clear();
    }
  }
}

void addFileMemberNameToIndex(MemberDef *md)
{
  FileDef *fd=md->getFileDef();
  if (fd && fd->isLinkableInProject() && md->isLinkableInProject())
  {
    QCString n = md->name();
    int index = getPrefixIndex(n);
    uchar charCode = (uchar)n.at(index);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (!n.isEmpty()) 
    {
      g_fileIndexLetterUsed[FMHL_All][letter].append(md);
      documentedFileMembers[FMHL_All]++;

      if (md->isFunction()) 
      {
        g_fileIndexLetterUsed[FMHL_Functions][letter].append(md);
        documentedFileMembers[FMHL_Functions]++;
      }
      else if (md->isVariable()) 
      {
        g_fileIndexLetterUsed[FMHL_Variables][letter].append(md);
        documentedFileMembers[FMHL_Variables]++;
      }
      else if (md->isTypedef())
      {
        g_fileIndexLetterUsed[FMHL_Typedefs][letter].append(md);
        documentedFileMembers[FMHL_Typedefs]++;
      }
      else if (md->isEnumerate())
      {
        g_fileIndexLetterUsed[FMHL_Enums][letter].append(md);
        documentedFileMembers[FMHL_Enums]++;
      }
      else if (md->isEnumValue())
      {
        g_fileIndexLetterUsed[FMHL_EnumValues][letter].append(md);
        documentedFileMembers[FMHL_EnumValues]++;
      }
      else if (md->isDefine())
      {
        g_fileIndexLetterUsed[FMHL_Defines][letter].append(md);
        documentedFileMembers[FMHL_Defines]++;
      }
    }
  }
}

//----------------------------------------------------------------------------

static void writeQuickMemberIndex(OutputList &ol,
    MemberIndexList charUsed[MEMBER_INDEX_ENTRIES],int page,
    QCString fullName,bool multiPage)
{
  bool first=TRUE;
  int i;
  startQuickIndexList(ol,TRUE);
  for (i=33;i<127;i++)
  {
    char is[2];is[0]=(char)i;is[1]='\0';
    QCString ci = letterToLabel((char)i);
    if (charUsed[i].count()>0)
    {
      QCString anchor;
      QCString extension=Doxygen::htmlFileExtension;
      if (!multiPage)
        anchor="#index_";
      else if (first) 
        anchor=fullName+extension+"#index_";
      else 
        anchor=fullName+QCString().sprintf("_0x%02x",i)+extension+"#index_";
      startQuickIndexItem(ol,anchor+ci,i==page,TRUE,first);
      ol.writeString(is);
      endQuickIndexItem(ol);
      first=FALSE;
    }
  }
  endQuickIndexList(ol);
}

//----------------------------------------------------------------------------

/** Helper class representing a class member in the navigation menu. */
struct CmhlInfo
{
  CmhlInfo(const char *fn,const char *t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const CmhlInfo *getCmhlInfo(int hl)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static CmhlInfo cmhlInfo[] = 
  {
    CmhlInfo("functions",     theTranslator->trAll()),
    CmhlInfo("functions_func",
        fortranOpt ? theTranslator->trSubprograms() : 
        vhdlOpt    ? VhdlDocGen::trFunctionAndProc() :
                     theTranslator->trFunctions()),
    CmhlInfo("functions_vars",theTranslator->trVariables()),
    CmhlInfo("functions_type",theTranslator->trTypedefs()),
    CmhlInfo("functions_enum",theTranslator->trEnumerations()),
    CmhlInfo("functions_eval",theTranslator->trEnumerationValues()),
    CmhlInfo("functions_prop",theTranslator->trProperties()),
    CmhlInfo("functions_evnt",theTranslator->trEvents()),
    CmhlInfo("functions_rela",theTranslator->trRelatedFunctions())
  };
  return &cmhlInfo[hl];
}

static void writeClassMemberIndexFiltered(OutputList &ol, ClassMemberHighlight hl)
{
  if (documentedClassMembers[hl]==0) return;
  
  static bool disableIndex     = Config_getBool("DISABLE_INDEX");

  bool multiPageIndex=FALSE;
  int numPages=1;
  if (documentedClassMembers[hl]>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
    numPages=127;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassMembers);
  QCString title = lne ? lne->title() : theTranslator->trCompoundMembers();
  if (hl!=CMHL_All) title+=(QCString)" - "+getCmhlInfo(hl)->title;
  bool addToIndex = lne==0 || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getCmhlInfo(hl)->title,0,
                                       getCmhlInfo(hl)->fname,0,multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  int page;
  bool first=TRUE;
  for (page=0;page<numPages;page++)
  {
    if (!multiPageIndex || g_memberIndexLetterUsed[hl][page].count()>0)
    {
      QCString fileName = getCmhlInfo(hl)->fname;
      if (multiPageIndex)
      { 
        if (!first)
        {
          fileName+=QCString().sprintf("_0x%02x",page);
        }
        char cs[2];
        cs[0]=page;
        cs[1]=0;
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(FALSE,cs,0,fileName,0,FALSE,TRUE);
        }
      }
      bool quickIndex = documentedClassMembers[hl]>maxItemsBeforeQuickIndex;
      
      ol.startFile(fileName+extension,0,title);
      ol.startQuickIndices();
      if (!disableIndex)
      {
        ol.writeQuickLinks(TRUE,HLI_Functions,0);
        startQuickIndexList(ol);

        // index item for global member list
        startQuickIndexItem(ol,
            getCmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==CMHL_All,TRUE,first);
        ol.writeString(fixSpaces(getCmhlInfo(0)->title));
        endQuickIndexItem(ol);

        int i;
        // index items per category member lists
        for (i=1;i<CMHL_Total;i++)
        {
          if (documentedClassMembers[i]>0)
          {
            startQuickIndexItem(ol,getCmhlInfo(i)->fname+Doxygen::htmlFileExtension,hl==i,TRUE,first);
            ol.writeString(fixSpaces(getCmhlInfo(i)->title));
            //printf("multiPageIndex=%d first=%d fileName=%s file=%s title=%s\n",
            //    multiPageIndex,first,fileName.data(),getCmhlInfo(i)->fname,getCmhlInfo(i)->title.data());
            endQuickIndexItem(ol);
          }
        }

        endQuickIndexList(ol);

        // quick alphabetical index
        if (quickIndex)
        {
          writeQuickMemberIndex(ol,g_memberIndexLetterUsed[hl],page,
              getCmhlInfo(hl)->fname,multiPageIndex);
        }
      }
      ol.endQuickIndices();
      ol.writeSplitBar(fileName);
      ol.writeSearchInfo();

      ol.startContents();

      if (hl==CMHL_All)
      {
        ol.startTextBlock();
        ol.parseText(lne ? lne->intro() : theTranslator->trCompoundMembersDescription(Config_getBool("EXTRACT_ALL")));
        ol.endTextBlock();
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        ol.writeString("&#160;");
      }
      //ol.newParagraph();  // FIXME:PARA
      writeMemberList(ol,quickIndex,
                      multiPageIndex?page:-1,
                      g_memberIndexLetterUsed[hl],
                      Definition::TypeClass);
      endFile(ol);
      first=FALSE;
    }
  }

  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  
  ol.popGeneratorState();
}

static void writeClassMemberIndex(OutputList &ol)
{
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassMembers);
  bool addToIndex = lne==0 || lne->visible();

  if (documentedClassMembers[CMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,lne ? lne->title() : theTranslator->trCompoundMembers(),0,"functions",0); 
    Doxygen::indexList->incContentsDepth();
  }
  writeClassMemberIndexFiltered(ol,CMHL_All);
  writeClassMemberIndexFiltered(ol,CMHL_Functions);
  writeClassMemberIndexFiltered(ol,CMHL_Variables);
  writeClassMemberIndexFiltered(ol,CMHL_Typedefs);
  writeClassMemberIndexFiltered(ol,CMHL_Enums);
  writeClassMemberIndexFiltered(ol,CMHL_EnumValues);
  writeClassMemberIndexFiltered(ol,CMHL_Properties);
  writeClassMemberIndexFiltered(ol,CMHL_Events);
  writeClassMemberIndexFiltered(ol,CMHL_Related);
  if (documentedClassMembers[CMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

/** Helper class representing a file member in the navigation menu. */
struct FmhlInfo 
{
  FmhlInfo(const char *fn,const char *t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const FmhlInfo *getFmhlInfo(int hl)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static FmhlInfo fmhlInfo[] = 
  {
    FmhlInfo("globals",     theTranslator->trAll()),
    FmhlInfo("globals_func",
         fortranOpt ? theTranslator->trSubprograms()  : 
         vhdlOpt    ? VhdlDocGen::trFunctionAndProc() : 
                      theTranslator->trFunctions()),
    FmhlInfo("globals_vars",theTranslator->trVariables()),
    FmhlInfo("globals_type",theTranslator->trTypedefs()),
    FmhlInfo("globals_enum",theTranslator->trEnumerations()),
    FmhlInfo("globals_eval",theTranslator->trEnumerationValues()),
    FmhlInfo("globals_defs",theTranslator->trDefines())
  };
  return &fmhlInfo[hl];
}

static void writeFileMemberIndexFiltered(OutputList &ol, FileMemberHighlight hl)
{
  if (documentedFileMembers[hl]==0) return;

  static bool disableIndex     = Config_getBool("DISABLE_INDEX");

  bool multiPageIndex=FALSE;
  int numPages=1;
  if (documentedFileMembers[hl]>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
    numPages=127;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileGlobals);
  QCString title = lne ? lne->title() : theTranslator->trFileMembers();
  bool addToIndex = lne==0 || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getFmhlInfo(hl)->title,0,
                                       getFmhlInfo(hl)->fname,0,multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  int page;
  bool first=TRUE;
  for (page=0;page<numPages;page++)
  {
    if (!multiPageIndex || g_fileIndexLetterUsed[hl][page].count()>0)
    {
      QCString fileName = getFmhlInfo(hl)->fname;
      if (multiPageIndex)
      {
        if (!first)
        {
          fileName+=QCString().sprintf("_0x%02x",page);
        }
        char cs[2];
        cs[0]=page;
        cs[1]=0;
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(FALSE,cs,0,fileName,0,FALSE,TRUE);
        }
      }
      bool quickIndex = documentedFileMembers[hl]>maxItemsBeforeQuickIndex;
      
      ol.startFile(fileName+extension,0,title);
      ol.startQuickIndices();
      if (!disableIndex)
      {
        ol.writeQuickLinks(TRUE,HLI_Globals,0);
        startQuickIndexList(ol);

        // index item for all file member lists
        startQuickIndexItem(ol,
            getFmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==FMHL_All,TRUE,first);
        ol.writeString(fixSpaces(getFmhlInfo(0)->title));
        endQuickIndexItem(ol);

        int i;
        // index items for per category member lists
        for (i=1;i<FMHL_Total;i++)
        {
          if (documentedFileMembers[i]>0)
          {
            startQuickIndexItem(ol,
                getFmhlInfo(i)->fname+Doxygen::htmlFileExtension,hl==i,TRUE,first);
            ol.writeString(fixSpaces(getFmhlInfo(i)->title));
            endQuickIndexItem(ol);
          }
        }

        endQuickIndexList(ol);

        if (quickIndex)
        {
          writeQuickMemberIndex(ol,g_fileIndexLetterUsed[hl],page,
              getFmhlInfo(hl)->fname,multiPageIndex);
        }
      }
      ol.endQuickIndices();
      ol.writeSplitBar(fileName);
      ol.writeSearchInfo();

      ol.startContents();

      if (hl==FMHL_All)
      {
        ol.startTextBlock();
        ol.parseText(lne ? lne->intro() : theTranslator->trFileMembersDescription(Config_getBool("EXTRACT_ALL")));
        ol.endTextBlock();
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        ol.writeString("&#160;");
      }
      //ol.newParagraph();  // FIXME:PARA
      //writeFileMemberList(ol,quickIndex,hl,page);
      writeMemberList(ol,quickIndex,
          multiPageIndex?page:-1,
          g_fileIndexLetterUsed[hl],
          Definition::TypeFile);
      endFile(ol);
      first=FALSE;
    }
  }
  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  ol.popGeneratorState();
}

static void writeFileMemberIndex(OutputList &ol)
{
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileGlobals);
  bool addToIndex = lne==0 || lne->visible();
  if (documentedFileMembers[FMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,lne ? lne->title() : theTranslator->trFileMembers(),0,"globals",0); 
    Doxygen::indexList->incContentsDepth();
  }
  writeFileMemberIndexFiltered(ol,FMHL_All);
  writeFileMemberIndexFiltered(ol,FMHL_Functions);
  writeFileMemberIndexFiltered(ol,FMHL_Variables);
  writeFileMemberIndexFiltered(ol,FMHL_Typedefs);
  writeFileMemberIndexFiltered(ol,FMHL_Enums);
  writeFileMemberIndexFiltered(ol,FMHL_EnumValues);
  writeFileMemberIndexFiltered(ol,FMHL_Defines);
  if (documentedFileMembers[FMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

/** Helper class representing a namespace member in the navigation menu. */
struct NmhlInfo
{
  NmhlInfo(const char *fn,const char *t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const NmhlInfo *getNmhlInfo(int hl)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static NmhlInfo nmhlInfo[] = 
  {
    NmhlInfo("namespacemembers",     theTranslator->trAll()),
    NmhlInfo("namespacemembers_func",
        fortranOpt ? theTranslator->trSubprograms()  :
        vhdlOpt    ? VhdlDocGen::trFunctionAndProc() :
                     theTranslator->trFunctions()),
    NmhlInfo("namespacemembers_vars",theTranslator->trVariables()),
    NmhlInfo("namespacemembers_type",theTranslator->trTypedefs()),
    NmhlInfo("namespacemembers_enum",theTranslator->trEnumerations()),
    NmhlInfo("namespacemembers_eval",theTranslator->trEnumerationValues())
  };
  return &nmhlInfo[hl];
}

//----------------------------------------------------------------------------

static void writeNamespaceMemberIndexFiltered(OutputList &ol,
                                        NamespaceMemberHighlight hl)
{
  if (documentedNamespaceMembers[hl]==0) return;

  static bool disableIndex     = Config_getBool("DISABLE_INDEX");


  bool multiPageIndex=FALSE;
  int numPages=1;
  if (documentedNamespaceMembers[hl]>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
    numPages=127;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceMembers);
  QCString title = lne ? lne->title() : theTranslator->trNamespaceMembers();
  bool addToIndex = lne==0 || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getNmhlInfo(hl)->title,0,
                                       getNmhlInfo(hl)->fname,0,multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  int page;
  bool first=TRUE;
  for (page=0;page<numPages;page++)
  {
    if (!multiPageIndex || g_namespaceIndexLetterUsed[hl][page].count()>0)
    {
      QCString fileName = getNmhlInfo(hl)->fname;
      if (multiPageIndex)
      {
        if (!first)
        {
          fileName+=QCString().sprintf("_0x%02x",page);
        }
        char cs[2];
        cs[0]=page;
        cs[1]=0;
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(FALSE,cs,0,fileName,0,FALSE,TRUE);
        }
      }
      bool quickIndex = documentedNamespaceMembers[hl]>maxItemsBeforeQuickIndex;
      
      ol.startFile(fileName+extension,0,title);
      ol.startQuickIndices();
      if (!disableIndex)
      {
        ol.writeQuickLinks(TRUE,HLI_NamespaceMembers,0);
        startQuickIndexList(ol);

        // index item for all namespace member lists
        startQuickIndexItem(ol,
            getNmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==NMHL_All,TRUE,first);
        ol.writeString(fixSpaces(getNmhlInfo(0)->title));
        endQuickIndexItem(ol);

        int i;
        // index items per category member lists
        for (i=1;i<NMHL_Total;i++)
        {
          if (documentedNamespaceMembers[i]>0)
          {
            startQuickIndexItem(ol,
                getNmhlInfo(i)->fname+Doxygen::htmlFileExtension,hl==i,TRUE,first);
            ol.writeString(fixSpaces(getNmhlInfo(i)->title));
            endQuickIndexItem(ol);
          }
        }

        endQuickIndexList(ol);

        if (quickIndex)
        {
          writeQuickMemberIndex(ol,g_namespaceIndexLetterUsed[hl],page,
              getNmhlInfo(hl)->fname,multiPageIndex);
        }

      }
      ol.endQuickIndices();
      ol.writeSplitBar(fileName);
      ol.writeSearchInfo();

      ol.startContents();

      if (hl==NMHL_All)
      {
        ol.startTextBlock();
        ol.parseText(lne ? lne->intro() : theTranslator->trNamespaceMemberDescription(Config_getBool("EXTRACT_ALL")));
        ol.endTextBlock();
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        ol.writeString("&#160;");
      }
      //ol.newParagraph(); // FIXME:PARA

      //writeNamespaceMemberList(ol,quickIndex,hl,page);
      writeMemberList(ol,quickIndex,
                      multiPageIndex?page:-1,
                      g_namespaceIndexLetterUsed[hl],
                      Definition::TypeNamespace);
      endFile(ol);
    }
  }
  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  ol.popGeneratorState();
}

static void writeNamespaceMemberIndex(OutputList &ol)
{
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceMembers);
  bool addToIndex = lne==0 || lne->visible();
  if (documentedNamespaceMembers[NMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,lne ? lne->title() : theTranslator->trNamespaceMembers(),0,"namespacemembers",0); 
    Doxygen::indexList->incContentsDepth();
  }
  //bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  writeNamespaceMemberIndexFiltered(ol,NMHL_All);
  writeNamespaceMemberIndexFiltered(ol,NMHL_Functions);
  writeNamespaceMemberIndexFiltered(ol,NMHL_Variables);
  writeNamespaceMemberIndexFiltered(ol,NMHL_Typedefs);
  writeNamespaceMemberIndexFiltered(ol,NMHL_Enums);
  writeNamespaceMemberIndexFiltered(ol,NMHL_EnumValues);
  if (documentedNamespaceMembers[NMHL_All]>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

static void writeExampleIndex(OutputList &ol)
{
  if (Doxygen::exampleSDict->count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Examples);
  QCString title = lne ? lne->title() : theTranslator->trExamples();
  bool addToIndex = lne==0 || lne->visible();

  startFile(ol,"examples",0,title,HLI_Examples);

  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);

  ol.startContents();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,title,0,"examples",0,TRUE,TRUE); 
    Doxygen::indexList->incContentsDepth();
  }

  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trExamplesDescription());
  ol.endTextBlock();

  ol.startItemList();
  PageSDict::Iterator pdi(*Doxygen::exampleSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    ol.startItemListItem();
    QCString n=pd->getOutputFileBase();
    if (!pd->title().isEmpty())
    {
      ol.writeObjectLink(0,n,0,pd->title());
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,filterTitle(pd->title()),pd->getReference(),n,0,FALSE,TRUE);
      }
    }
    else
    {
      ol.writeObjectLink(0,n,0,pd->name());
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,pd->name(),pd->getReference(),n,0,FALSE,TRUE);
      }
    }
    ol.endItemListItem();
    ol.writeString("\n");
  }
  ol.endItemList();

  if (addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }
  endFile(ol);
  ol.popGeneratorState();
}


//----------------------------------------------------------------------------

template<typename T>
bool writeMemberNavIndex(FTextStream &t,
                         int indent,
                         int n,
                         int documentedMembers[],
                         MemberIndexList indexLetterUsed[][MEMBER_INDEX_ENTRIES],
                         const T *(*getInfo)(int),
                         bool &first
                        )

{
  bool found=FALSE;
  QCString indentStr;
  indentStr.fill(' ',indent*2);
  // index items per category member lists
  int i;
  for (i=0;i<n;i++)
  {
    bool hasIndex       = documentedMembers[i]>0;
    bool quickIndex     = documentedMembers[i]>maxItemsBeforeQuickIndex;
    bool multiIndexPage = documentedMembers[i]>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX;
    if (hasIndex)
    {
      // terminate previous entry
      if (!first) t << "," << endl;
      first = FALSE;

      // start entry
      if (!found)
      {
        t << "[" << endl;
      }
      found = TRUE;

      t << indentStr << "  [ ";
      t << "\"" << fixSpaces(getInfo(i)->title) << "\", ";
      t << "\"" << getInfo(i)->fname << Doxygen::htmlFileExtension << "\", ";
      bool firstPage=TRUE;
      if (quickIndex)
      {
        t << "[ " << endl;
        int j;
        for (j=33;j<127;j++)
        {
          if (indexLetterUsed[i][j].count()>0)
          {
            if (!firstPage) t << "," << endl;
            QCString fullName = getInfo(i)->fname;
            QCString extension = Doxygen::htmlFileExtension;
            QCString anchor;
            if (firstPage || !multiIndexPage) 
              anchor=fullName+extension+"#index_";
            else 
              anchor=fullName+QCString().sprintf("_0x%02x",j)+extension+"#index_";
            char is[2];is[0]=(char)j;is[1]='\0';
            QCString ci = letterToLabel((char)j);
            t << indentStr << "    [ ";
            t << "\"" << is << "\", ";
            t << "\"" << anchor << ci << "\", null ]";
            firstPage=FALSE;
          }
        }
        t << endl << indentStr << "  ] ]";
      }
      else
      {
        t << "null" << " ]";
      }
    }
  }
  return found;
}

//----------------------------------------------------------------------------

#if 0
static bool writeFullNavIndex(FTextStream &t, LayoutNavEntry *root,int indent,bool &first)
{
  static struct NavEntryCountMap 
  {
    LayoutNavEntry::Kind kind;
    bool hasItems;
  } navEntryCountMap[] =
  {
    { LayoutNavEntry::MainPage,         TRUE                                   },
    { LayoutNavEntry::Pages,            indexedPages>0                         },
    { LayoutNavEntry::Modules,          documentedGroups>0                     },
    { LayoutNavEntry::Namespaces,       documentedNamespaces>0                 },
    { LayoutNavEntry::NamespaceList,    documentedNamespaces>0                 },
    { LayoutNavEntry::NamespaceMembers, documentedNamespaceMembers[NMHL_All]>0 },
    { LayoutNavEntry::Classes,          annotatedClasses>0                     },
    { LayoutNavEntry::ClassList,        annotatedClasses>0                     },
    { LayoutNavEntry::ClassIndex,       annotatedClasses>0                     },
    { LayoutNavEntry::ClassHierarchy,   hierarchyClasses>0                     },
    { LayoutNavEntry::ClassMembers,     documentedClassMembers[CMHL_All]>0     },
    { LayoutNavEntry::Files,            documentedFiles>0                      },
    { LayoutNavEntry::FileList,         documentedFiles>0                      },
    { LayoutNavEntry::FileGlobals,      documentedFileMembers[FMHL_All]>0      },
    //{ LayoutNavEntry::Dirs,             documentedDirs>0                       },
    { LayoutNavEntry::Examples,         Doxygen::exampleSDict->count()>0       }
  };

  QCString indentStr;
  indentStr.fill(' ',indent*2);
  bool found=FALSE;
  if (root->children().count()>0)
  {
    QListIterator<LayoutNavEntry> li(root->children());
    LayoutNavEntry *entry;
    for (li.toFirst();(entry=li.current());++li)
    {
      if (navEntryCountMap[entry->kind()].hasItems && entry->visible())
      {
        // terminate previous entry
        if (!first) t << "," << endl;
        first = FALSE;

        // start entry
        if (!found)
        {
          t << "[" << endl;
        }
        found = TRUE;

        bool emptySection=TRUE;
        t << indentStr << "  [ ";
        t << "\"" << fixSpaces(entry->title()) << "\", ";
        t << "\"" << entry->baseFile() << Doxygen::htmlFileExtension << "\", ";

        // write children (if any)
        bool firstChild=TRUE;
        if (entry->kind()==LayoutNavEntry::ClassMembers)
        {
          emptySection = !writeMemberNavIndex(t,indent+1,CMHL_Total,documentedClassMembers,g_memberIndexLetterUsed,&getCmhlInfo,firstChild);
        }
        else if (entry->kind()==LayoutNavEntry::NamespaceMembers)
        {
          emptySection = !writeMemberNavIndex(t,indent+1,NMHL_Total,documentedNamespaceMembers,g_namespaceIndexLetterUsed,&getNmhlInfo,firstChild);
        }
        else if (entry->kind()==LayoutNavEntry::FileGlobals)
        {
          emptySection = !writeMemberNavIndex(t,indent+1,FMHL_Total,documentedFileMembers,g_fileIndexLetterUsed,&getFmhlInfo,firstChild);
        }
        else
        {
          emptySection = !writeFullNavIndex(t,entry,indent+1,firstChild);
        }
        // end entry
        if (emptySection) // entry without children
          t << "null ]";
        else // entry with children
          t << endl << indentStr << "  ] ]";
      }
    }
  }
  return found;
}
#endif

//----------------------------------------------------------------------------

static void countRelatedPages(int &docPages,int &indexPages)
{
  docPages=indexPages=0;
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if ( pd->visibleInIndex())
    {
      indexPages++; 
    }
    if ( pd->documentedPage())
    {
      docPages++;
    }
  }
}

//----------------------------------------------------------------------------

static bool mainPageHasTitle()
{
  if (Doxygen::mainPage==0) return FALSE;
  if (Doxygen::mainPage->title().isEmpty()) return FALSE;
  if (Doxygen::mainPage->title().lower()=="notitle") return FALSE;
  return TRUE;
}

//----------------------------------------------------------------------------

static void writePages(PageDef *pd,FTVHelp *ftv)
{
  //printf("writePages()=%s pd=%p mainpage=%p\n",pd->name().data(),pd,Doxygen::mainPage);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Pages);
  bool addToIndex = lne==0 || lne->visible();
  if (!addToIndex) return;

  bool hasSubPages = pd->hasSubPages();
  bool hasSections = pd->hasSections();

  if (pd->visibleInIndex())
  {
    QCString pageTitle;

    if (pd->title().isEmpty())
      pageTitle=pd->name();
    else
      pageTitle=pd->title();

    if (ftv)
    {
      //printf("*** adding %s\n",pageTitle.data());
      ftv->addContentsItem(
          hasSubPages,pageTitle,
          pd->getReference(),pd->getOutputFileBase(),
          0,hasSubPages,TRUE,pd); 
    }
    if (addToIndex && pd!=Doxygen::mainPage)
    {
      Doxygen::indexList->addContentsItem(
          hasSubPages,pageTitle,
          pd->getReference(),pd->getOutputFileBase(),
          0,hasSubPages,TRUE);
    }
  }
  if (hasSubPages && ftv) ftv->incContentsDepth();
  bool doIndent = (hasSections || hasSubPages) &&  
                  (pd!=Doxygen::mainPage || mainPageHasTitle());
  if (doIndent)
  {
    Doxygen::indexList->incContentsDepth();
  }
  if (hasSections)
  {
    pd->addSectionsToIndex();
  }
  PageSDict *subPages = pd->getSubPages();
  if (subPages)
  {
    PageSDict::Iterator pi(*subPages);
    PageDef *subPage;
    for (pi.toFirst();(subPage=pi.current());++pi)
    {
      writePages(subPage,ftv);
    }
  }
  if (hasSubPages && ftv) ftv->decContentsDepth();
  if (doIndent)
  {
    Doxygen::indexList->decContentsDepth();
  }
  //printf("end writePages()=%s\n",pd->title().data());
}

//----------------------------------------------------------------------------

static void writePageIndex(OutputList &ol)
{
  if (indexedPages==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Pages);
  QCString title = lne ? lne->title() : theTranslator->trRelatedPages();
  startFile(ol,"pages",0,title,HLI_Pages);
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trRelatedPagesDescription());
  ol.endTextBlock();

  {
    FTVHelp* ftv = new FTVHelp(FALSE);
    PageSDict::Iterator pdi(*Doxygen::pageSDict);
    PageDef *pd=0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      if (pd->getOuterScope()==0 || 
          pd->getOuterScope()->definitionType()!=Definition::TypePage
         )  // not a sub page
      {
        writePages(pd,ftv);
      }
    }
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.writeString(outStr);
    delete ftv;
  }

//  ol.popGeneratorState();
  // ------

  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static int countGroups()
{
  int count=0;
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    if (!gd->isReference())
    {
      gd->visited=FALSE;
      count++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------

static int countDirs()
{
  int count=0;
  SDict<DirDef>::Iterator dli(*Doxygen::directories);
  DirDef *dd;
  for (dli.toFirst();(dd=dli.current());++dli)
  {
    if (dd->isLinkableInProject())
    {
      dd->visited=FALSE;
      count++;
    }
  }
  return count;
}


//----------------------------------------------------------------------------

void writeGraphInfo(OutputList &ol)
{
  if (!Config_getBool("HAVE_DOT") || !Config_getBool("GENERATE_HTML")) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  generateGraphLegend(Config_getString("HTML_OUTPUT"));
  startFile(ol,"graph_legend",0,theTranslator->trLegendTitle().data());
  startTitle(ol,0);
  ol.parseText(theTranslator->trLegendTitle());
  endTitle(ol,0,0);
  ol.startContents();
  bool &stripCommentsStateRef = Config_getBool("STRIP_CODE_COMMENTS");
  bool oldStripCommentsState = stripCommentsStateRef;
  // temporarily disable the stripping of comments for our own code example!
  stripCommentsStateRef = FALSE;
  QCString legendDocs = theTranslator->trLegendDocs();
  int s = legendDocs.find("<center>");
  int e = legendDocs.find("</center>");
  if (Config_getEnum("DOT_IMAGE_FORMAT")=="svg" && s!=-1 && e!=-1)
  {
    legendDocs = legendDocs.left(s+8) + "[!-- SVG 0 --]\n" + legendDocs.mid(e); 
    //printf("legendDocs=%s\n",legendDocs.data());
  }
  FileDef fd("","graph_legend");
  ol.parseDoc("graph_legend",1,&fd,0,legendDocs,FALSE,FALSE);
  stripCommentsStateRef = oldStripCommentsState;
  endFile(ol);
  ol.popGeneratorState();
}



//----------------------------------------------------------------------------
/*!
 * write groups as hierarchical trees
 */
static void writeGroupTreeNode(OutputList &ol, GroupDef *gd, int level, FTVHelp* ftv, bool addToIndex)
{
  //bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  //bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");  
  if (level>20)
  {
    warn(gd->getDefFileName(),gd->getDefLine(),
        "warning: maximum nesting level exceeded for group %s: check for possible recursive group relation!\n",gd->name().data()
        );
    return;
  }

  /* Some groups should appear twice under different parent-groups.
   * That is why we should not check if it was visited 
   */
  if (/*!gd->visited &&*/ (!gd->isASubGroup() || level>0) &&
      gd->isVisible() &&
      (!gd->isReference() || Config_getBool("EXTERNAL_GROUPS")) // hide external groups by default
     )
  {
    //printf("gd->name()=%s #members=%d\n",gd->name().data(),gd->countMembers());
    // write group info
    bool hasSubGroups = gd->getSubGroups()->count()>0;
    bool hasSubPages = gd->getPages()->count()>0;
    int numSubItems = 0;
    if (1 /*Config_getBool("TOC_EXPAND")*/)
    {
      QListIterator<MemberList> mli(gd->getMemberLists());
      MemberList *ml;
      for (mli.toFirst();(ml=mli.current());++mli)
      {
        if (ml->listType()&MemberListType_documentationLists)
        {
          numSubItems += ml->count();
        }
      }
      numSubItems += gd->getNamespaces()->count();
      numSubItems += gd->getClasses()->count();
      numSubItems += gd->getFiles()->count();
      numSubItems += gd->getDirs()->count();
      numSubItems += gd->getPages()->count();
    }

    bool isDir = hasSubGroups || hasSubPages || numSubItems>0;
    //printf("gd=`%s': pageDict=%d\n",gd->name().data(),gd->pageDict->count());
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(isDir,gd->groupTitle(),gd->getReference(),gd->getOutputFileBase(),0,isDir,TRUE); 
      Doxygen::indexList->incContentsDepth();
    }
    if (ftv)
    {
      ftv->addContentsItem(hasSubGroups,gd->groupTitle(),
                           gd->getReference(),gd->getOutputFileBase(),0,
                           FALSE,FALSE,gd); 
      ftv->incContentsDepth();
    }
    
    //ol.writeListItem();
    //ol.startTextLink(gd->getOutputFileBase(),0);
    //parseText(ol,gd->groupTitle());
    //ol.endTextLink();

    ol.startIndexListItem();
    ol.startIndexItem(gd->getReference(),gd->getOutputFileBase());
    ol.parseText(gd->groupTitle());
    ol.endIndexItem(gd->getReference(),gd->getOutputFileBase());
    if (gd->isReference()) 
    { 
      ol.startTypewriter(); 
      ol.docify(" [external]");
      ol.endTypewriter();
    }

    QListIterator<LayoutDocEntry> eli(LayoutDocManager::instance().docEntries(LayoutDocManager::Group));
    LayoutDocEntry *lde;
    for (eli.toFirst();(lde=eli.current());++eli)
    {
      if (lde->kind()==LayoutDocEntry::MemberDef && addToIndex)
      {
        LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
        MemberList *ml = gd->getMemberList(lmd->type);
        if (ml)
        {
          MemberListIterator mi(*ml);
          MemberDef *md;
          for (mi.toFirst();(md=mi.current());++mi)
          {
            LockingPtr<MemberList> enumList = md->enumFieldList();
            bool isDir = enumList!=0 && md->isEnumerate();
            if (md->isVisible() && md->name().find('@')==-1)
            {
              Doxygen::indexList->addContentsItem(isDir,
                  md->name(),md->getReference(),
                  md->getOutputFileBase(),md->anchor(),FALSE,addToIndex);
            }
            if (isDir)
            {
              Doxygen::indexList->incContentsDepth();
              MemberListIterator emli(*enumList);
              MemberDef *emd;
              for (emli.toFirst();(emd=emli.current());++emli)
              {
                if (emd->isVisible())
                {
                  Doxygen::indexList->addContentsItem(FALSE,
                      emd->name(),emd->getReference(),emd->getOutputFileBase(),
                      emd->anchor(),FALSE,addToIndex);
                }
              }
              Doxygen::indexList->decContentsDepth();
            }
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupClasses && addToIndex)
      {
        ClassSDict::Iterator it(*gd->getClasses());
        ClassDef *cd;
        for (;(cd=it.current());++it)
        {
          //bool nestedClassInSameGroup = 
          //    cd->getOuterScope() && cd->getOuterScope()->definitionType()==Definition::TypeClass &&
          //    cd->getOuterScope()->partOfGroups()!=0 && cd->getOuterScope()->partOfGroups()->contains(gd);
          //printf("===== GroupClasses: %s visible=%d nestedClassInSameGroup=%d\n",cd->name().data(),cd->isVisible(),nestedClassInSameGroup);
          if (cd->isVisible() /*&& !nestedClassInSameGroup*/)
          {
            //if (cd->isEmbeddedInOuterScope())
            //{
              //printf("add class & members %d\n",addToIndex);
              addMembersToIndex(cd,LayoutDocManager::Class,cd->displayName(FALSE),cd->anchor(),addToIndex,TRUE);
            //}
            //else // only index the class, not its members
            //{
            //  printf("%s: add class only\n",cd->name().data());
            //  Doxygen::indexList->addContentsItem(FALSE,
            //    cd->displayName(TRUE),cd->getReference(),
            //    cd->getOutputFileBase(),cd->anchor(),addToIndex,TRUE);
            //}
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupNamespaces && addToIndex)
      {
        NamespaceSDict::Iterator it(*gd->getNamespaces());
        NamespaceDef *nd;
        for (;(nd=it.current());++it)
        {
          if (nd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                nd->localName(),nd->getReference(),
                nd->getOutputFileBase(),0,FALSE,FALSE);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupFiles && addToIndex)
      {
        QListIterator<FileDef> it(*gd->getFiles());
        FileDef *fd;
        for (;(fd=it.current());++it)
        {
          if (fd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                fd->displayName(),fd->getReference(),
                fd->getOutputFileBase(),0,FALSE,FALSE);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupDirs && addToIndex)
      {
        QListIterator<DirDef> it(*gd->getDirs());
        DirDef *dd;
        for (;(dd=it.current());++it)
        {
          if (dd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                dd->shortName(),dd->getReference(),
                dd->getOutputFileBase(),0,FALSE,FALSE);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupPageDocs && addToIndex)
      {
        SDict<PageDef>::Iterator it(*gd->getPages());
        PageDef *pd;
        for (;(pd=it.current());++it)
        {
          SectionInfo *si=0;
          if (!pd->name().isEmpty()) si=Doxygen::sectionDict->find(pd->name());
          bool hasSubPages = pd->hasSubPages();
          bool hasSections = pd->hasSections();
          Doxygen::indexList->addContentsItem(
              hasSubPages || hasSections,
              convertToHtml(pd->title(),TRUE),
              gd->getReference(),
              gd->getOutputFileBase(),
              si ? si->label.data() : 0,
              hasSubPages || hasSections,
              TRUE); // addToNavIndex
          if (hasSections || hasSubPages)
          {
            Doxygen::indexList->incContentsDepth();
          }
          if (hasSections)
          {
            pd->addSectionsToIndex();
          }
          writePages(pd,0);
          if (hasSections || hasSubPages)
          {
            Doxygen::indexList->decContentsDepth();
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupNestedGroups)
      {
        if (gd->getSubGroups()->count()>0)
        {
          startIndexHierarchy(ol,level+1);
          if (Config_getBool("SORT_GROUP_NAMES")) gd->sortSubGroups();
          QListIterator<GroupDef> gli(*gd->getSubGroups());
          GroupDef *subgd = 0;
          for (gli.toFirst();(subgd=gli.current());++gli)
          {
            writeGroupTreeNode(ol,subgd,level+1,ftv,addToIndex);
          }
          endIndexHierarchy(ol,level+1); 
        }
      }
    }

    ol.endIndexListItem();
    
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
    if (ftv)
    {
      ftv->decContentsDepth();
    }
    //gd->visited=TRUE;
  }
}

static void writeGroupHierarchy(OutputList &ol, FTVHelp* ftv,bool addToIndex)
{
  if (ftv)
  {
    ol.pushGeneratorState(); 
    ol.disable(OutputGenerator::Html);
  }
  startIndexHierarchy(ol,0);
  if (Config_getBool("SORT_GROUP_NAMES"))
  {
    Doxygen::groupSDict->sort();
  }
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    writeGroupTreeNode(ol,gd,0,ftv,addToIndex);
  }
  endIndexHierarchy(ol,0); 
  if (ftv)
  {
    ol.popGeneratorState(); 
  }
}

#if 0
static void writeGroupTree(GroupDef *gd,FTVHelp *ftv,int level,bool addToIndex)
{
  static bool externalGroups = Config_getBool("EXTERNAL_GROUPS");
  /* Some groups should appear twice under different parent-groups.
   * That is why we should not check if it was visited 
   */
  if ((!gd->isASubGroup() || level>0) &&
      gd->isVisible() &&
      (!gd->isReference() || externalGroups) // hide external groups by default
     )
  {
    if (ftv)
    {
      ftv->addContentsItem(hasSubGroups,gd->groupTitle(),gd->getReference(),gd->getOutputFileBase(),0); 
      ftv->incContentsDepth();
    }
    if (ftv)
    {
      ftv->decContentsDepth();
    }
  }
}

static void writeGroupTree(FTVHelp *ftv,bool addToIndex)
{
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    writeGroupTree(gd,ftv,0,addToIndex);
  }
}
#endif

//----------------------------------------------------------------------------

static void writeGroupIndex(OutputList &ol)
{
  if (documentedGroups==0) return; 
  ol.pushGeneratorState(); 
  // 1.{
  ol.disable(OutputGenerator::Man);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Modules);
  QCString title = lne ? lne->title() : theTranslator->trModules();
  bool addToIndex = lne==0 || lne->visible();

  startFile(ol,"modules",0,title,HLI_Modules);
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trModulesDescription());
  ol.endTextBlock();

  // ---------------
  // Normal group index for Latex/RTF
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  Doxygen::indexList->disable();

  writeGroupHierarchy(ol,0,FALSE);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  // 2.}

  // ---------------
  // interactive group index for HTML
  // ---------------
  // 2.{
  ol.pushGeneratorState(); 
  ol.disableAllBut(OutputGenerator::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,0,"modules",0,TRUE,TRUE); 
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp* ftv = new FTVHelp(FALSE);
    writeGroupHierarchy(ol,ftv,addToIndex);
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString(outStr);
    delete ftv;
    if (addToIndex)
    {
      Doxygen::indexList->decContentsDepth();
    }
  }
  ol.popGeneratorState();
  // 2.}

  endFile(ol);
  ol.popGeneratorState();
  // 1.}
}

//----------------------------------------------------------------------------

#if 0
static void writeDirIndex(OutputList &ol)
{
  if (documentedDirs==0) return; 
  ol.pushGeneratorState(); 
  ol.disable(OutputGenerator::Man);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Dirs);
  QCString title = lne ? lne->title() : theTranslator->trDirectories();
  bool addToIndex=FALSE; //lne==0 || lne->visible();

  startFile(ol,"dirs",0,title,HLI_Directories);
  startTitle(ol,0);
  ol.parseText(title);
  endTitle(ol,0,0);
  ol.startContents();
  ol.startTextBlock();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,title,0,"dirs",0,TRUE,TRUE); 
    Doxygen::indexList->incContentsDepth();
  }
  ol.parseText(lne ? lne->intro() : theTranslator->trDirDescription());
  ol.endTextBlock();

  FTVHelp* ftv = 0;
  bool treeView=Config_getBool("USE_INLINE_TREES");
  if (treeView)
  {
    ftv = new FTVHelp(FALSE);
  }

  writeDirHierarchy(ol,ftv,addToIndex);

  if (ftv)
  {
    QGString outStr;
    FTextStream t(&outStr);
    ftv->generateTreeViewInline(t);
    ol.pushGeneratorState(); 
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString(outStr);
    ol.popGeneratorState();
    delete ftv;
  }
  if (addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }
  endFile(ol);
  ol.popGeneratorState();
}
#endif

//----------------------------------------------------------------------------

static void writeUserGroupStubPage(OutputList &ol,LayoutNavEntry *lne)
{
  if (lne->baseFile().left(9)=="usergroup")
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    startFile(ol,lne->baseFile(),0,lne->title(),HLI_UserGroup);
    startTitle(ol,0);
    ol.parseText(lne->title());
    endTitle(ol,0,0);
    ol.startContents();
    QListIterator<LayoutNavEntry> li(lne->children());
    LayoutNavEntry *entry;
    int count=0;
    for (li.toFirst();(entry=li.current());++li)
    {
      if (entry->visible()) count++;
    }
    if (count>0)
    {
      ol.writeString("<ul>\n");
      for (li.toFirst();(entry=li.current());++li)
      {
        if (entry->visible())
        {
          ol.writeString("<li><a href=\""+entry->url()+"\"><span>"+
              fixSpaces(entry->title())+"</span></a></li>\n");
        }
      }
      ol.writeString("</ul>\n");
    }
    endFile(ol);
    ol.popGeneratorState();
  }
}

//----------------------------------------------------------------------------


static void writeIndex(OutputList &ol)
{
  static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static QCString projectName = Config_getString("PROJECT_NAME");
  // save old generator state
  ol.pushGeneratorState();

  QCString projPrefix;
  if (!projectName.isEmpty())
  {
    projPrefix=projectName+" ";
  }

  //--------------------------------------------------------------------
  // write HTML index
  //--------------------------------------------------------------------
  ol.disableAllBut(OutputGenerator::Html);

  QCString defFileName = 
    Doxygen::mainPage ? Doxygen::mainPage->docFile().data() : "[generated]";
  int defLine =
    Doxygen::mainPage ? Doxygen::mainPage->docLine() : -1;

  QCString title;
  if (!mainPageHasTitle())
  {
    title = theTranslator->trMainPage();
  }
  else 
  {
    title = filterTitle(Doxygen::mainPage->title());
  }

  QCString indexName="index";
  ol.startFile(indexName,0,title);
  
  if (Doxygen::mainPage)
  {
    if (
        (!projectName.isEmpty() && mainPageHasTitle() && qstricmp(title,projectName)!=0)
       ) // to avoid duplicate entries in the treeview
    {
      Doxygen::indexList->addContentsItem(Doxygen::mainPage->hasSubPages(),title,0,indexName,0,Doxygen::mainPage->hasSubPages(),TRUE); 
    }
    if (Doxygen::mainPage->hasSubPages() || Doxygen::mainPage->hasSections())
    {
      writePages(Doxygen::mainPage,0);
    }
  }

  ol.startQuickIndices();
  if (!Config_getBool("DISABLE_INDEX")) 
  {
    ol.writeQuickLinks(TRUE,HLI_Main,0);
  }
  ol.endQuickIndices();
  ol.writeSplitBar(indexName);
  ol.writeSearchInfo();
  bool headerWritten=FALSE;
  if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty())
  {
    if (Doxygen::mainPage->title().lower()!="notitle")
    {
      ol.startHeaderSection();
      ol.startTitleHead(0);
      ol.parseDoc(Doxygen::mainPage->docFile(),Doxygen::mainPage->docLine(),
                  Doxygen::mainPage,0,Doxygen::mainPage->title(),
                  TRUE,FALSE,0,TRUE,FALSE);
      headerWritten = TRUE;
    }
  }
  else
  {
    if (!projectName.isEmpty())
    {
      ol.startHeaderSection();
      ol.startTitleHead(0);
      ol.parseText(projPrefix+theTranslator->trDocumentation());
      headerWritten = TRUE;
    }
  }
  if (headerWritten)
  {
    ol.endTitleHead(0,0);
    ol.endHeaderSection();
  }

  ol.startContents();
  if (Config_getBool("DISABLE_INDEX") && Doxygen::mainPage==0) 
  {
    ol.writeQuickLinks(FALSE,HLI_Main,0);
  }

  if (Doxygen::mainPage)
  {
    Doxygen::insideMainPage=TRUE;
    if (Doxygen::mainPage->showToc() && Doxygen::mainPage->hasSections())
    {
      Doxygen::mainPage->writeToc(ol);
    }

    ol.startTextBlock();
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,
                Doxygen::mainPage->documentation(),TRUE,FALSE
                /*,Doxygen::mainPage->sectionDict*/);
    ol.endTextBlock();

    Doxygen::insideMainPage=FALSE;
  }
  
  endFile(ol);
  ol.disable(OutputGenerator::Html);
  
  //--------------------------------------------------------------------
  // write LaTeX/RTF index
  //--------------------------------------------------------------------
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::RTF);

  ol.startFile("refman",0,0);
  ol.startIndexSection(isTitlePageStart);
  if (!Config_getString("LATEX_HEADER").isEmpty()) 
  {
    ol.disable(OutputGenerator::Latex);
  }

  if (projPrefix.isEmpty())
  {
    ol.parseText(theTranslator->trReferenceManual());
  }
  else
  {
    ol.parseText(projPrefix);
  }

  if (!Config_getString("PROJECT_NUMBER").isEmpty())
  {
    ol.startProjectNumber(); 
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,Config_getString("PROJECT_NUMBER"),FALSE,FALSE);
    ol.endProjectNumber();
  }
  ol.endIndexSection(isTitlePageStart);
  ol.startIndexSection(isTitlePageAuthor);
  ol.parseText(theTranslator->trGeneratedBy());
  ol.endIndexSection(isTitlePageAuthor);
  ol.enable(OutputGenerator::Latex);

  ol.lastIndexPage();
  if (Doxygen::mainPage)
  {
    ol.startIndexSection(isMainPage);
    if (mainPageHasTitle())
    {
      ol.parseText(Doxygen::mainPage->title());
    }
    else
    {
      ol.parseText(/*projPrefix+*/theTranslator->trMainPage());
    }
    ol.endIndexSection(isMainPage);
  }
  if (documentedPages>0)
  {
    //ol.parseText(projPrefix+theTranslator->trPageDocumentation());
    //ol.endIndexSection(isPageDocumentation);
    PageSDict::Iterator pdi(*Doxygen::pageSDict);
    PageDef *pd=pdi.toFirst();
    bool first=Doxygen::mainPage==0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      if (!pd->getGroupDef() && !pd->isReference() && 
          (!pd->hasParentPage() ||                    // not inside other page
           (Doxygen::mainPage==pd->getOuterScope()))  // or inside main page
         )
      {
        bool isCitationPage = pd->name()=="citelist";
        if (isCitationPage)
        {
          // For LaTeX the bibliograph is already written by \bibliography
          ol.pushGeneratorState();
          ol.disable(OutputGenerator::Latex);
        }
        QCString title = pd->title();
        if (title.isEmpty()) title=pd->name();
        ol.startIndexSection(isPageDocumentation);
        ol.parseText(title);
        ol.endIndexSection(isPageDocumentation);
        ol.pushGeneratorState(); // write TOC title (RTF only)
          ol.disableAllBut(OutputGenerator::RTF);
          ol.startIndexSection(isPageDocumentation2);
          ol.parseText(title);
          ol.endIndexSection(isPageDocumentation2);
          ol.popGeneratorState();
        ol.writeAnchor(0,pd->name());

        ol.writePageLink(pd->getOutputFileBase(),first);
        first=FALSE;

        if (isCitationPage)
        {
          ol.popGeneratorState();
        }
      }
    }
  }

  if (!Config_getBool("LATEX_HIDE_INDICES"))
  {
    //if (indexedPages>0)
    //{
    //  ol.startIndexSection(isPageIndex);
    //  ol.parseText(/*projPrefix+*/ theTranslator->trPageIndex());
    //  ol.endIndexSection(isPageIndex);
    //}
    if (documentedGroups>0)
    {
      ol.startIndexSection(isModuleIndex);
      ol.parseText(/*projPrefix+*/ theTranslator->trModuleIndex());
      ol.endIndexSection(isModuleIndex);
    }
    if (documentedNamespaces>0)
    {
      ol.startIndexSection(isNamespaceIndex);
      ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trModulesIndex():theTranslator->trNamespaceIndex()));
      ol.endIndexSection(isNamespaceIndex);
    }
    if (hierarchyClasses>0)
    {
      ol.startIndexSection(isClassHierarchyIndex);
      ol.parseText(/*projPrefix+*/
          (fortranOpt ? theTranslator->trCompoundIndexFortran() : 
           vhdlOpt    ? VhdlDocGen::trDesignUnitIndex()         :
                        theTranslator->trHierarchicalIndex()
          ));
      ol.endIndexSection(isClassHierarchyIndex);
    }
    if (annotatedClassesPrinted>0)
    {
      ol.startIndexSection(isCompoundIndex);
      ol.parseText(/*projPrefix+*/
          (fortranOpt ? theTranslator->trCompoundIndexFortran() :
              vhdlOpt ? VhdlDocGen::trDesignUnitIndex()         : 
                        theTranslator->trCompoundIndex()
          ));
      ol.endIndexSection(isCompoundIndex);
    }
    if (documentedFiles>0)
    {
      ol.startIndexSection(isFileIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trFileIndex());
      ol.endIndexSection(isFileIndex);
    }
  }
  if (documentedGroups>0)
  {
    ol.startIndexSection(isModuleDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trModuleDocumentation());
    ol.endIndexSection(isModuleDocumentation);
  }
  if (documentedNamespaces>0)
  {
    ol.startIndexSection(isNamespaceDocumentation);
    ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trModuleDocumentation():theTranslator->trNamespaceDocumentation()));
    ol.endIndexSection(isNamespaceDocumentation);
  }
  if (annotatedClassesPrinted>0)
  {
    ol.startIndexSection(isClassDocumentation);
    ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trTypeDocumentation():theTranslator->trClassDocumentation()));
    ol.endIndexSection(isClassDocumentation);
  }
  if (documentedFiles>0)
  {
    ol.startIndexSection(isFileDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trFileDocumentation());
    ol.endIndexSection(isFileDocumentation);
  }
  if (Doxygen::exampleSDict->count()>0)
  {
    ol.startIndexSection(isExampleDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trExampleDocumentation());
    ol.endIndexSection(isExampleDocumentation);
  }
  ol.endIndexSection(isEndIndex);
  endFile(ol);

  if (Doxygen::mainPage)
  {
    Doxygen::insideMainPage=TRUE;
    ol.disable(OutputGenerator::Man);
    startFile(ol,Doxygen::mainPage->name(),0,Doxygen::mainPage->title());
    ol.startContents();
    ol.startTextBlock();
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,
                Doxygen::mainPage->documentation(),FALSE,FALSE
               );
    ol.endTextBlock();
    endFile(ol);
    ol.enable(OutputGenerator::Man);
    Doxygen::insideMainPage=FALSE;
  }

  ol.popGeneratorState();
}

static QArray<bool> indexWritten;

static void writeIndexHierarchyEntries(OutputList &ol,const QList<LayoutNavEntry> &entries)
{
  QListIterator<LayoutNavEntry> li(entries);
  LayoutNavEntry *lne;
  for (li.toFirst();(lne=li.current());++li)
  {
    LayoutNavEntry::Kind kind = lne->kind();
    uint index = (uint)kind;
    if (index>=indexWritten.size())
    {
      uint i;
      uint oldSize = indexWritten.size();
      uint newSize = index+1;
      indexWritten.resize(newSize);
      for (i=oldSize;i<newSize;i++) indexWritten.at(i)=FALSE;
    }
    //printf("starting %s kind=%d\n",lne->title().data(),lne->kind());
    bool addToIndex=lne==0 || lne->visible();
    bool needsClosing=FALSE;
    if (!indexWritten.at(index))
    {
      switch(kind)
      {
        case LayoutNavEntry::MainPage: 
          msg("Generating index page...\n");
          writeIndex(ol); 
          break;
        case LayoutNavEntry::Pages: 
          msg("Generating page index...\n");
          writePageIndex(ol);
          break;
        case LayoutNavEntry::Modules: 
          msg("Generating module index...\n");
          writeGroupIndex(ol);
          break;
        case LayoutNavEntry::Namespaces: 
          {
            static bool showNamespaces = Config_getBool("SHOW_NAMESPACES");
            if (showNamespaces)
            {
              if (documentedNamespaces>0 && addToIndex)
              {
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,0,0); 
                Doxygen::indexList->incContentsDepth();
                needsClosing=TRUE;
              }
              if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Namespaces)!=lne) // for backward compatibility with old layout file
              {
                msg("Generating namespace index...\n");
                writeNamespaceIndex(ol);
              }
            }
          }
          break;
        case LayoutNavEntry::NamespaceList: 
          {
            static bool showNamespaces = Config_getBool("SHOW_NAMESPACES");
            if (showNamespaces)
            {
              msg("Generating namespace index...\n");
              writeNamespaceIndex(ol);
            }
          }
          break;
        case LayoutNavEntry::NamespaceMembers: 
          msg("Generating namespace member index...\n");
          writeNamespaceMemberIndex(ol);
          break;
        case LayoutNavEntry::Classes: 
          if (annotatedClasses>0 && addToIndex)
          {
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,0,0); 
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Classes)!=lne) // for backward compatibility with old layout file
          {
            msg("Generating annotated compound index...\n");
            writeAnnotatedIndex(ol);
          }
          break;
        case LayoutNavEntry::ClassList: 
          msg("Generating annotated compound index...\n");
          writeAnnotatedIndex(ol);
          break;
        case LayoutNavEntry::ClassIndex:
          msg("Generating alphabetical compound index...\n");
          writeAlphabeticalIndex(ol);
          break;
        case LayoutNavEntry::ClassHierarchy: 
          msg("Generating hierarchical class index...\n");
          writeHierarchicalIndex(ol);
          if (Config_getBool("HAVE_DOT") && Config_getBool("GRAPHICAL_HIERARCHY"))
          {
            msg("Generating graphical class hierarchy...\n");
            writeGraphicalClassHierarchy(ol);
          }
          break;
        case LayoutNavEntry::ClassMembers: 
          msg("Generating member index...\n");
          writeClassMemberIndex(ol);
          break;
        case LayoutNavEntry::Files: 
          {
            static bool showFiles = Config_getBool("SHOW_FILES");
            if (showFiles)
            {
              if (documentedHtmlFiles>0 && addToIndex)
              {
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,0,0); 
                Doxygen::indexList->incContentsDepth();
                needsClosing=TRUE;
              }
              if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Files)!=lne) // for backward compatibility with old layout file
              {
                msg("Generating file index...\n");
                writeFileIndex(ol);
              }
            }
          }
          break;
        case LayoutNavEntry::FileList: 
          {
            static bool showFiles = Config_getBool("SHOW_FILES");
            if (showFiles)
            {
              msg("Generating file index...\n");
              writeFileIndex(ol);
            }
          }
          break;
        case LayoutNavEntry::FileGlobals: 
          msg("Generating file member index...\n");
          writeFileMemberIndex(ol);
          break;
        case LayoutNavEntry::Examples: 
          msg("Generating example index...\n");
          writeExampleIndex(ol);
          break;
        case LayoutNavEntry::User: 
          {
            // prepend a ! or ^ marker to the URL to avoid tampering with it
            QCString url = correctURL(lne->url(),"!"); // add ! to relative URL
            bool isRelative=url.at(0)=='!';
            if (!url.isEmpty() && !isRelative) // absolute URL
            {
              url.prepend("^"); // prepend ^ to absolute URL
            }
            bool isRef = lne->baseFile().left(4)=="@ref" || lne->baseFile().left(4)=="\\ref";
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,url,0,FALSE,isRef || isRelative); 
          }
          break;
        case LayoutNavEntry::UserGroup:
          if (addToIndex)
          {
            QCString url = correctURL(lne->url(),"!"); // add ! to relative URL
            if (!url.isEmpty())
            {
              if (url=="![none]")
              {
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,0,0,FALSE,FALSE); 
              }
              else
              {
                bool isRelative=url.at(0)=='!';
                if (!isRelative) // absolute URL
                {
                  url.prepend("^"); // prepend ^ to absolute URL
                }
                bool isRef = lne->baseFile().left(4)=="@ref" || lne->baseFile().left(4)=="\\ref";
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,url,0,FALSE,isRef || isRelative); 
              }
            }
            else 
            {
              Doxygen::indexList->addContentsItem(TRUE,lne->title(),0,lne->baseFile(),0,TRUE,TRUE); 
            }
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          writeUserGroupStubPage(ol,lne);
          break;
      }
      if (kind!=LayoutNavEntry::User && kind!=LayoutNavEntry::UserGroup) // User entry may appear multiple times
      {
        indexWritten.at(index)=TRUE;
      }
    }
    writeIndexHierarchyEntries(ol,lne->children());
    if (needsClosing)
    {
      switch(kind)
      {
        case LayoutNavEntry::Namespaces: 
        case LayoutNavEntry::Classes: 
        case LayoutNavEntry::Files: 
        case LayoutNavEntry::UserGroup: 
          Doxygen::indexList->decContentsDepth();
          break;
        default:
          break;
      }
    }
    //printf("ending %s kind=%d\n",lne->title().data(),lne->kind());
  }
}

void writeIndexHierarchy(OutputList &ol)
{
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry();
  if (lne)
  {
    writeIndexHierarchyEntries(ol,lne->children());
  }
}


/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include <cstdlib>
#include <array>

#include <assert.h>

#include "message.h"
#include "index.h"
#include "indexlist.h"
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
#include "dotgfxhierarchytable.h"
#include "dotlegendgraph.h"
#include "pagedef.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "memberlist.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"
#include "tooltip.h"
#include "utf8.h"
#include "portable.h"
#include "moduledef.h"
#include "sitemap.h"

#define MAX_ITEMS_BEFORE_MULTIPAGE_INDEX 200
#define MAX_ITEMS_BEFORE_QUICK_INDEX 30

// helpers
static int  countClassHierarchy(ClassDef::CompoundType ct);
static void countFiles(int &htmlFiles,int &files);
static int  countGroups();
static int  countDirs();
static int  countNamespaces();
static int  countConcepts();
static int  countAnnotatedClasses(int *cp,ClassDef::CompoundType ct);
static void countRelatedPages(int &docPages,int &indexPages);


struct Index::Private
{
  int annotatedClasses            = 0;
  int annotatedClassesPrinted     = 0;
  int hierarchyClasses            = 0;
  int annotatedInterfaces         = 0;
  int annotatedInterfacesPrinted  = 0;
  int hierarchyInterfaces         = 0;
  int annotatedStructs            = 0;
  int annotatedStructsPrinted     = 0;
  int annotatedExceptions         = 0;
  int annotatedExceptionsPrinted  = 0;
  int hierarchyExceptions         = 0;
  int documentedGroups            = 0;
  int documentedNamespaces        = 0;
  int documentedConcepts          = 0;
  int documentedModules           = 0;
  int indexedPages                = 0;
  int documentedFiles             = 0;
  int documentedPages             = 0;
  int documentedDirs              = 0;
  std::array<int,    ClassMemberHighlight::Total> documentedClassMembers     = {};
  std::array<int,     FileMemberHighlight::Total> documentedFileMembers      = {};
  std::array<int,NamespaceMemberHighlight::Total> documentedNamespaceMembers = {};
  std::array<int,   ModuleMemberHighlight::Total> documentedModuleMembers    = {};
  std::array<MemberIndexMap,ClassMemberHighlight::Total>     classIndexLetterUsed;
  std::array<MemberIndexMap,FileMemberHighlight::Total>      fileIndexLetterUsed;
  std::array<MemberIndexMap,NamespaceMemberHighlight::Total> namespaceIndexLetterUsed;
  std::array<MemberIndexMap,ModuleMemberHighlight::Total>    moduleIndexLetterUsed;
};

Index::Index() : p(std::make_unique<Private>())
{
}

Index::~Index() = default;

Index &Index::instance()
{
  static Index index;
  return index;
}

int Index::numAnnotatedClasses()           const { return p->annotatedClasses;        }
int Index::numAnnotatedClassesPrinted()    const { return p->annotatedClassesPrinted; }
int Index::numHierarchyClasses()           const { return p->hierarchyClasses;        }
int Index::numAnnotatedInterfaces()        const { return p->annotatedInterfaces; }
int Index::numAnnotatedInterfacesPrinted() const { return p->annotatedInterfacesPrinted; }
int Index::numHierarchyInterfaces()        const { return p->hierarchyInterfaces; }
int Index::numAnnotatedStructs()           const { return p->annotatedStructs; }
int Index::numAnnotatedStructsPrinted()    const { return p->annotatedStructsPrinted; }
int Index::numAnnotatedExceptions()        const { return p->annotatedExceptions; }
int Index::numAnnotatedExceptionsPrinted() const { return p->annotatedExceptionsPrinted; }
int Index::numHierarchyExceptions()        const { return p->hierarchyExceptions; }
int Index::numDocumentedGroups()           const { return p->documentedGroups; }
int Index::numDocumentedNamespaces()       const { return p->documentedNamespaces; }
int Index::numDocumentedConcepts()         const { return p->documentedConcepts; }
int Index::numDocumentedModules()          const { return p->documentedModules; }
int Index::numIndexedPages()               const { return p->indexedPages; }
int Index::numDocumentedFiles()            const { return p->documentedFiles; }
int Index::numDocumentedPages()            const { return p->documentedPages; }
int Index::numDocumentedDirs()             const { return p->documentedDirs; }
int Index::numDocumentedClassMembers(ClassMemberHighlight::Enum e)         const { return p->documentedClassMembers[e]; }
int Index::numDocumentedFileMembers(FileMemberHighlight::Enum e)           const { return p->documentedFileMembers[e];  }
int Index::numDocumentedNamespaceMembers(NamespaceMemberHighlight::Enum e) const { return p->documentedNamespaceMembers[e]; }
int Index::numDocumentedModuleMembers(ModuleMemberHighlight::Enum e)       const { return p->documentedModuleMembers[e]; }

Index::MemberIndexMap Index::isClassIndexLetterUsed(ClassMemberHighlight::Enum e) const
{
  return p->classIndexLetterUsed[static_cast<int>(e)];
}

Index::MemberIndexMap Index::isFileIndexLetterUsed(FileMemberHighlight::Enum e) const
{
  return p->fileIndexLetterUsed[static_cast<int>(e)];
}

Index::MemberIndexMap Index::isNamespaceIndexLetterUsed(NamespaceMemberHighlight::Enum e) const
{
  return p->namespaceIndexLetterUsed[static_cast<int>(e)];
}

Index::MemberIndexMap Index::isModuleIndexLetterUsed(ModuleMemberHighlight::Enum e) const
{
  return p->moduleIndexLetterUsed[static_cast<int>(e)];
}

void Index::resetDocumentedClassMembers(int i)
{
  p->documentedClassMembers[i]=0;
  p->classIndexLetterUsed[i].clear();
}

void Index::resetDocumentedFileMembers(int i)
{
  p->documentedFileMembers[i]=0;
  p->fileIndexLetterUsed[i].clear();
}

void Index::resetDocumentedNamespaceMembers(int i)
{
  p->documentedNamespaceMembers[i]=0;
  p->namespaceIndexLetterUsed[i].clear();
}

void Index::resetDocumentedModuleMembers(int i)
{
  p->documentedModuleMembers[i]=0;
  p->moduleIndexLetterUsed[i].clear();
}

static void MemberIndexMap_add(Index::MemberIndexMap &map,const std::string &letter,const MemberDef *md)
{
  auto it = map.find(letter);
  if (it!=map.end())
  {
    it->second.push_back(md);
  }
  else
  {
    map.emplace(letter,std::vector<const MemberDef*>({md}));
  }
}

void Index::incrementDocumentedClassMembers(int i,const std::string &letter,const MemberDef *md)
{
  p->documentedClassMembers[i]++;
  MemberIndexMap_add(p->classIndexLetterUsed[i],letter,md);
}

void Index::incrementDocumentedFileMembers(int i,const std::string &letter,const MemberDef *md)
{
  p->documentedFileMembers[i]++;
  MemberIndexMap_add(p->fileIndexLetterUsed[i],letter,md);
}

void Index::incrementDocumentedNamespaceMembers(int i,const std::string &letter,const MemberDef *md)
{
  p->documentedNamespaceMembers[i]++;
  MemberIndexMap_add(p->namespaceIndexLetterUsed[i],letter,md);
}

void Index::incrementDocumentedModuleMembers(int i,const std::string &letter,const MemberDef *md)
{
  p->documentedModuleMembers[i]++;
  MemberIndexMap_add(p->moduleIndexLetterUsed[i],letter,md);
}


void Index::sortMemberIndexLists()
{
  auto sortMemberIndexList = [](MemberIndexMap &map)
  {
    for (auto &[name,list] : map)
    {
      std::stable_sort(list.begin(),list.end(),
          [](const MemberDef *md1,const MemberDef *md2)
          {
            // consider candidates A::a, B::b, B::a, A::b, A::A, B::B,
            // want after sorting: A::A, A::a, B::a, B::B, A::b, B::b
            // so we can later group entries
            // - A: A
            // - a: A, B
            // - B: B
            // - b: A, B
            int result = qstricmp_sort(md1->name(),md2->name());
            if (result==0) // next compare with full scope
            {
              result = qstricmp_sort(md1->qualifiedName(),md2->qualifiedName());
            }
            return result<0;
          });
    }
  };

  for (auto &idx : p->classIndexLetterUsed)
  {
    sortMemberIndexList(idx);
  }
  for (auto &idx : p->fileIndexLetterUsed)
  {
    sortMemberIndexList(idx);
  }
  for (auto &idx : p->namespaceIndexLetterUsed)
  {
    sortMemberIndexList(idx);
  }
  for (auto &idx : p->moduleIndexLetterUsed)
  {
    sortMemberIndexList(idx);
  }
}

void Index::countDataStructures()
{
  for (int j=0;j<ClassMemberHighlight::Total;j++)
  {
    resetDocumentedClassMembers(j);
  }
  for (int j=0;j<NamespaceMemberHighlight::Total;j++)
  {
    resetDocumentedNamespaceMembers(j);
  }
  for (int j=0;j<FileMemberHighlight::Total;j++)
  {
    resetDocumentedFileMembers(j);
  }
  for (int j=0;j<ModuleMemberHighlight::Total;j++)
  {
    resetDocumentedModuleMembers(j);
  }

  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  p->annotatedClasses           = countAnnotatedClasses(&p->annotatedClassesPrinted, ClassDef::Class); // "classes" + "annotated"
  p->hierarchyClasses           = countClassHierarchy(ClassDef::Class);     // "hierarchy"
  // "interfaces" + "annotated"
  p->annotatedInterfaces        = sliceOpt ? countAnnotatedClasses(&p->annotatedInterfacesPrinted, ClassDef::Interface) : 0;
  // "interfacehierarchy"
  p->hierarchyInterfaces        = sliceOpt ? countClassHierarchy(ClassDef::Interface) : 0;
  // "structs" + "annotated"
  p->annotatedStructs           = sliceOpt ? countAnnotatedClasses(&p->annotatedStructsPrinted, ClassDef::Struct) : 0;
  // "exceptions" + "annotated"
  p->annotatedExceptions        = sliceOpt ? countAnnotatedClasses(&p->annotatedExceptionsPrinted, ClassDef::Exception) : 0;
  // "exceptionhierarchy"
  p->hierarchyExceptions        = sliceOpt ? countClassHierarchy(ClassDef::Exception) : 0;
  countFiles(p->documentedFiles,p->documentedFiles);         // "files"
  countRelatedPages(p->documentedPages,p->indexedPages);     // "pages"
  p->documentedGroups           = countGroups();             // "topics"
  p->documentedNamespaces       = countNamespaces();         // "namespaces"
  p->documentedConcepts         = countConcepts();           // "concepts"
  p->documentedDirs             = countDirs();               // "dirs"
  p->documentedModules          = ModuleManager::instance().numDocumentedModules();
  // "globals"
  // "namespacemembers"
  // "functions"
}


static void startIndexHierarchy(OutputList &ol,int level)
{
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Html);
  if (level<6) ol.startIndexList();
  ol.popGeneratorState();

  ol.pushGeneratorState();
  ol.disable(OutputType::Latex);
  ol.disable(OutputType::RTF);
  ol.disable(OutputType::Docbook);
  ol.startItemList();
  ol.popGeneratorState();
}

static void endIndexHierarchy(OutputList &ol,int level)
{
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Html);
  if (level<6) ol.endIndexList();
  ol.popGeneratorState();

  ol.pushGeneratorState();
  ol.disable(OutputType::Latex);
  ol.disable(OutputType::Docbook);
  ol.disable(OutputType::RTF);
  ol.endItemList();
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

const int maxItemsBeforeQuickIndex = MAX_ITEMS_BEFORE_QUICK_INDEX;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

static void startQuickIndexList(OutputList &ol,bool letterTabs=FALSE)
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

static void endQuickIndexList(OutputList &ol)
{
  ol.writeString("    </ul>\n");
  ol.writeString("  </div>\n");
}

static void startQuickIndexItem(OutputList &ol,const QCString &l,
                                bool hl,bool /* compact */,bool &first)
{
  first=FALSE;
  ol.writeString("      <li");
  if (hl) ol.writeString(" class=\"current\"");
  ol.writeString("><a ");
  ol.writeString("href=\"");
  ol.writeString(l);
  ol.writeString("\">");
  ol.writeString("<span>");
}

static void endQuickIndexItem(OutputList &ol)
{
  ol.writeString("</span>");
  ol.writeString("</a>");
  ol.writeString("</li>\n");
}

void startTitle(OutputList &ol,const QCString &fileName,const DefinitionMutable *def)
{
  ol.startHeaderSection();
  if (def) def->writeSummaryLinks(ol);
  ol.startTitleHead(fileName);
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
}

void endTitle(OutputList &ol,const QCString &fileName,const QCString &name)
{
  ol.popGeneratorState();
  ol.endTitleHead(fileName,name);
  ol.endHeaderSection();
}

void startFile(OutputList &ol,const QCString &name,const QCString &manName,
               const QCString &title,HighlightedItem hli,bool additionalIndices,
               const QCString &altSidebarName, int hierarchyLevel)
{
  bool disableIndex = Config_getBool(DISABLE_INDEX);
  ol.startFile(name,manName,title,hierarchyLevel);
  ol.startQuickIndices();
  if (!disableIndex)
  {
    ol.writeQuickLinks(hli,name);
  }
  if (!additionalIndices)
  {
    ol.endQuickIndices();
  }
  ol.writeSplitBar(!altSidebarName.isEmpty() ? altSidebarName : name);
  ol.writeSearchInfo();
}

void endFile(OutputList &ol,bool skipNavIndex,bool skipEndContents,
             const QCString &navPath)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
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

void endFileWithNavPath(OutputList &ol,const Definition *d)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString navPath;
  if (generateTreeView)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.writeString("</div><!-- doc-content -->\n");
    ol.popGeneratorState();
    navPath = d->navigationPathAsString();
  }
  endFile(ol,generateTreeView,TRUE,navPath);
}

//----------------------------------------------------------------------

static void writeMemberToIndex(const Definition *def,const MemberDef *md,bool addToIndex)
{
  bool isAnonymous = md->isAnonymous();
  bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  const MemberVector &enumList = md->enumFieldList();
  bool isDir = !enumList.empty() && md->isEnumerate();
  auto defType = def->definitionType();
  bool namespaceMemberInFileDocs = md->getNamespaceDef() && defType==Definition::TypeFile;
  bool lAddToIndex = addToIndex && !namespaceMemberInFileDocs;
  QCString name = namespaceMemberInFileDocs || defType==Definition::TypeModule ?
                  md->qualifiedName() : md->name();
  if (md->getOuterScope()==def ||
      (md->getNamespaceDef()!=nullptr && defType==Definition::TypeFile) ||
      md->getOuterScope()==Doxygen::globalScope)
  {
    Doxygen::indexList->addContentsItem(isDir,
        name,md->getReference(),md->getOutputFileBase(),md->anchor(),FALSE,lAddToIndex && md->getGroupDef()==nullptr);
  }
  else // inherited member
  {
    Doxygen::indexList->addContentsItem(isDir,
        name,def->getReference(),def->getOutputFileBase(),md->anchor(),FALSE,lAddToIndex && md->getGroupDef()==nullptr);
  }
  if (isDir)
  {
    if (!isAnonymous)
    {
      Doxygen::indexList->incContentsDepth();
    }
    for (const auto &emd : enumList)
    {
      if (!hideUndocMembers || emd->hasDocumentation())
      {
        namespaceMemberInFileDocs = emd->getNamespaceDef() && defType==Definition::TypeFile;
        lAddToIndex = addToIndex && !namespaceMemberInFileDocs;
        QCString ename = namespaceMemberInFileDocs || defType==Definition::TypeModule ?
                         emd->qualifiedName() : emd->name();
        if (emd->getOuterScope()==def ||
            (emd->getNamespaceDef()!=nullptr && defType==Definition::TypeFile) ||
            emd->getOuterScope()==Doxygen::globalScope)
        {
          Doxygen::indexList->addContentsItem(FALSE,
              ename,emd->getReference(),emd->getOutputFileBase(),emd->anchor(),FALSE,lAddToIndex && emd->getGroupDef()==nullptr);
        }
        else // inherited member
        {
          Doxygen::indexList->addContentsItem(FALSE,
              ename,def->getReference(),def->getOutputFileBase(),emd->anchor(),FALSE,lAddToIndex && emd->getGroupDef()==nullptr);
        }
      }
    }
    if (!isAnonymous)
    {
      Doxygen::indexList->decContentsDepth();
    }
  }
}

//----------------------------------------------------------------------
template<class T>
void addMembersToIndex(T *def,LayoutDocManager::LayoutPart part,
                       const QCString &name,const QCString &anchor,
                       bool addToIndex=TRUE,bool preventSeparateIndex=FALSE,
                       const ConceptLinkedRefMap *concepts = nullptr)

{
  int numClasses=0;
  for (const auto &cd : def->getClasses())
  {
    if (cd->isLinkable()) numClasses++;
  }
  int numConcepts=0;
  if (concepts)
  {
    for (const auto &cd : *concepts)
    {
      if (cd->isLinkable()) numConcepts++;
    }
  }
  bool hasMembers = !def->getMemberLists().empty() || !def->getMemberGroups().empty() || (numClasses>0) || (numConcepts>0);
  Doxygen::indexList->addContentsItem(hasMembers,name,
                                     def->getReference(),def->getOutputFileBase(),anchor,
                                     hasMembers && !preventSeparateIndex,
                                     addToIndex,
                                     def);
  //printf("addMembersToIndex(def=%s hasMembers=%d numClasses=%d)\n",qPrint(def->name()),hasMembers,numClasses);
  if (hasMembers || numClasses>0 || numConcepts>0)
  {
    Doxygen::indexList->incContentsDepth();
    for (const auto &lde : LayoutDocManager::instance().docEntries(part))
    {
      auto kind = lde->kind();
      if (kind==LayoutDocEntry::MemberDef)
      {
        const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
        if (lmd)
        {
          MemberList *ml = def->getMemberList(lmd->type);
          if (ml)
          {
            for (const auto &md : *ml)
            {
              if (md->visibleInIndex())
              {
                writeMemberToIndex(def,md,addToIndex);
              }
            }
          }
        }
      }
      else if (kind==LayoutDocEntry::NamespaceClasses ||
               kind==LayoutDocEntry::FileClasses ||
               kind==LayoutDocEntry::ClassNestedClasses ||
               kind==LayoutDocEntry::ModuleClasses
              )
      {
        for (const auto &cd : def->getClasses())
        {
          if (cd->isLinkable() && (cd->partOfGroups().empty() || def->definitionType()==Definition::TypeGroup))
          {
            bool inlineSimpleStructs = Config_getBool(INLINE_SIMPLE_STRUCTS);
            bool isNestedClass = def->definitionType()==Definition::TypeClass;
            addMembersToIndex(cd,LayoutDocManager::Class,cd->displayName(lde->kind()==LayoutDocEntry::FileClasses),cd->anchor(),
                              addToIndex && (isNestedClass || (cd->isSimple() && inlineSimpleStructs)),
                              preventSeparateIndex || cd->isEmbeddedInOuterScope());
          }
        }
      }
      else if ((kind==LayoutDocEntry::FileConcepts || kind==LayoutDocEntry::ModuleConcepts) && concepts)
      {
        for (const auto &cd : *concepts)
        {
          if (cd->isLinkable() && (cd->partOfGroups().empty() || def->definitionType()==Definition::TypeGroup))
          {
            Doxygen::indexList->addContentsItem(false,cd->displayName(),
                                     cd->getReference(),cd->getOutputFileBase(),QCString(),
                                     addToIndex,
                                     false,
                                     cd);
          }
        }
      }
    }

    Doxygen::indexList->decContentsDepth();
  }
}


//----------------------------------------------------------------------------
/*! Generates HTML Help tree of classes */

static void writeClassTreeToOutput(OutputList &ol,const BaseClassList &bcl,int level,FTVHelp* ftv,bool addToIndex,ClassDefSet &visitedClasses)
{
  if (bcl.empty()) return;
  bool started=FALSE;
  for (const auto &bcd : bcl)
  {
    ClassDef *cd=bcd.classDef;
    if (cd->getLanguage()==SrcLangExt::VHDL && VhdlDocGen::convert(cd->protection())!=VhdlDocGen::ENTITYCLASS)
    {
      continue;
    }

    bool b = cd->getLanguage()==SrcLangExt::VHDL ? classHasVisibleRoot(cd->subClasses()) : classHasVisibleRoot(cd->baseClasses());

    if (cd->isVisibleInHierarchy() && b) // classHasVisibleRoot(cd->baseClasses()))
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
      bool hasChildren = visitedClasses.find(cd)==visitedClasses.end() &&
                         classHasVisibleChildren(cd);
      //printf("tree4: Has children %s: %d\n",qPrint(cd->name()),hasChildren);
      if (cd->isLinkable())
      {
        //printf("Writing class %s\n",qPrint(cd->displayName()));
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
          if (cd->getLanguage()==SrcLangExt::VHDL)
          {
            ftv->addContentsItem(hasChildren,bcd.usedName,cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd);
          }
          else
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd);
          }
        }
      }
      else
      {
        ol.startIndexItem(QCString(),QCString());
        ol.parseText(cd->name());
        ol.endIndexItem(QCString(),QCString());
        if (addToIndex)
        {
          Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),QCString(),QCString(),QCString());
        }
        if (ftv)
        {
          ftv->addContentsItem(hasChildren,cd->displayName(),QCString(),QCString(),QCString(),FALSE,FALSE,cd);
        }
      }
      if (hasChildren)
      {
        //printf("Class %s at %p visited=%d\n",qPrint(cd->name()),cd,cd->visited);
        visitedClasses.insert(cd);
        if (cd->getLanguage()==SrcLangExt::VHDL)
        {
          writeClassTreeToOutput(ol,cd->baseClasses(),level+1,ftv,addToIndex,visitedClasses);
        }
        else
        {
          writeClassTreeToOutput(ol,cd->subClasses(),level+1,ftv,addToIndex,visitedClasses);
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

static bool dirHasVisibleChildren(const DirDef *dd)
{
  if (dd->hasDocumentation()) return TRUE;

  for (const auto &fd : dd->getFiles())
  {
    bool genSourceFile = false;
    if (fileVisibleInIndex(fd,genSourceFile))
    {
      return TRUE;
    }
    if (genSourceFile)
    {
      return TRUE;
    }
  }

  for(const auto &subdd : dd->subDirs())
  {
    if (dirHasVisibleChildren(subdd))
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
static void writeDirTreeNode(OutputList &ol, const DirDef *dd, int level, FTVHelp* ftv,bool addToIndex)
{
  if (level>20)
  {
    warn(dd->getDefFileName(),dd->getDefLine(),
        "maximum nesting level exceeded for directory %s: "
        "check for possible recursive directory relation!",qPrint(dd->name())
        );
    return;
  }

  if (!dirHasVisibleChildren(dd))
  {
    return;
  }

  bool tocExpand = TRUE; //Config_getBool(TOC_EXPAND);
  bool isDir = !dd->subDirs().empty() ||  // there are subdirs
               (tocExpand &&              // or toc expand and
                !dd->getFiles().empty()   // there are files
               );
  //printf("gd='%s': pageDict=%d\n",qPrint(gd->name()),gd->pageDict->count());
  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(isDir,dd->shortName(),dd->getReference(),dd->getOutputFileBase(),QCString(),TRUE,TRUE);
    Doxygen::indexList->incContentsDepth();
  }
  if (ftv)
  {
    ftv->addContentsItem(isDir,dd->shortName(),dd->getReference(),
                         dd->getOutputFileBase(),QCString(),FALSE,TRUE,dd);
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
  if (dd->subDirs().size()>0)
  {
    startIndexHierarchy(ol,level+1);
    for(const auto &subdd : dd->subDirs())
    {
      writeDirTreeNode(ol,subdd,level+1,ftv,addToIndex);
    }
    endIndexHierarchy(ol,level+1);
  }

  int fileCount=0;
  if (!dd->getFiles().empty())
  {
    for (const auto &fd : dd->getFiles())
    {
      //bool allExternals = Config_getBool(ALLEXTERNALS);
      //if ((allExternals && fd->isLinkable()) || fd->isLinkableInProject())
      //{
      //  fileCount++;
      //}
      bool genSourceFile = false;
      if (fileVisibleInIndex(fd,genSourceFile))
      {
        fileCount++;
      }
      else if (genSourceFile)
      {
        fileCount++;
      }
    }
    if (fileCount>0)
    {
      startIndexHierarchy(ol,level+1);
      for (const auto &fd : dd->getFiles())
      {
        bool src = false;
        bool doc = fileVisibleInIndex(fd,src);
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
                QCString(),FALSE,FALSE,fd);
          }
        }
      }
      endIndexHierarchy(ol,level+1);
    }
  }

  if (tocExpand && addToIndex)
  {
    // write files of this directory
    if (fileCount>0)
    {
      for (const auto &fd : dd->getFiles())
      {
        bool src = false;
        bool doc = fileVisibleInIndex(fd,src);
        if (doc)
        {
          addMembersToIndex(fd,LayoutDocManager::File,fd->displayName(),QCString(),
                            !fd->isLinkableViaGroup(),FALSE,&fd->getConcepts());
        }
        else if (src)
        {
          Doxygen::indexList->addContentsItem(
               FALSE, fd->name(), QCString(),
               fd->getSourceFileBase(), QCString(), FALSE, TRUE, fd);
        }
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
    ol.disable(OutputType::Html);
  }
  startIndexHierarchy(ol,0);
  for (const auto &dd : *Doxygen::dirLinkedMap)
  {
    if (dd->getOuterScope()==Doxygen::globalScope)
    {
      writeDirTreeNode(ol,dd.get(),0,ftv,addToIndex);
    }
  }
  if (ftv)
  {
    for (const auto &fn : *Doxygen::inputNameLinkedMap)
    {
      for (const auto &fd : *fn)
      {
        if (fd->getDirDef()==nullptr) // top level file
        {
          bool src = false;
          bool doc = fileVisibleInIndex(fd.get(),src);
          QCString reference, outputBase;
          if (doc)
          {
            reference = fd->getReference();
            outputBase = fd->getOutputFileBase();
          }
          if (doc || src)
          {
            ftv->addContentsItem(FALSE,fd->displayName(),
                                 reference, outputBase, QCString(),
                                 FALSE,FALSE,fd.get());
          }
          if (addToIndex)
          {
            if (doc)
            {
              addMembersToIndex(fd.get(),LayoutDocManager::File,fd->displayName(),QCString(),TRUE,FALSE,&fd->getConcepts());
            }
            else if (src)
            {
              Doxygen::indexList->addContentsItem(
                  FALSE, fd->displayName(), QCString(),
                  fd->getSourceFileBase(), QCString(), FALSE, TRUE, fd.get());
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

static void writeClassTreeForList(OutputList &ol,const ClassLinkedMap &cl,bool &started,FTVHelp* ftv,bool addToIndex,
                                  ClassDef::CompoundType ct,ClassDefSet &visitedClasses)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  for (const auto &cd : cl)
  {
    //printf("class %s classHasVisibleRoot=%d isVisibleInHierarchy=%d\n",
    //             qPrint(cd->name()),
    //              classHasVisibleRoot(cd->baseClasses()),
    //              cd->isVisibleInHierarchy()
    //      );
    bool b = false;
    if (cd->getLanguage()==SrcLangExt::VHDL)
    {
      if (VhdlDocGen::convert(cd->protection())!=VhdlDocGen::ENTITYCLASS)
      {
        continue;
      }
      b=!classHasVisibleRoot(cd->subClasses());
    }
    else if (sliceOpt && cd->compoundType() != ct)
    {
      continue;
    }
    else
    {
      b=!classHasVisibleRoot(cd->baseClasses());
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
        bool hasChildren = visitedClasses.find(cd.get())==visitedClasses.end() &&
                           classHasVisibleChildren(cd.get());
        //printf("list: Has children %s: %d\n",qPrint(cd->name()),hasChildren);
        if (cd->isLinkable())
        {
          //printf("Writing class %s isLinkable()=%d isLinkableInProject()=%d cd->isImplicitTemplateinstance()=%d\n",
          //    qPrint(cd->displayName()),cd->isLinkable(),cd->isLinkableInProject(),cd->isImplicitTemplateInstance());
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
            if (cd->getLanguage()!=SrcLangExt::VHDL) // prevents double insertion in Design Unit List
            	  Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE);
          }
          if (ftv)
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),cd->getReference(),cd->getOutputFileBase(),cd->anchor(),FALSE,FALSE,cd.get());
          }
        }
        else
        {
          ol.startIndexItem(QCString(),QCString());
          ol.parseText(cd->displayName());
          ol.endIndexItem(QCString(),QCString());
          if (addToIndex)
          {
            Doxygen::indexList->addContentsItem(hasChildren,cd->displayName(),QCString(),QCString(),QCString(),FALSE,FALSE);
          }
          if (ftv)
          {
            ftv->addContentsItem(hasChildren,cd->displayName(),QCString(),QCString(),QCString(),FALSE,FALSE,cd.get());
          }
        }
        if (cd->getLanguage()==SrcLangExt::VHDL && hasChildren)
        {
          writeClassTreeToOutput(ol,cd->baseClasses(),1,ftv,addToIndex,visitedClasses);
          visitedClasses.insert(cd.get());
        }
        else if (hasChildren)
        {
          writeClassTreeToOutput(ol,cd->subClasses(),1,ftv,addToIndex,visitedClasses);
          visitedClasses.insert(cd.get());
        }
        ol.endIndexListItem();
      }
    }
  }
}

static void writeClassHierarchy(OutputList &ol, FTVHelp* ftv,bool addToIndex,ClassDef::CompoundType ct)
{
  ClassDefSet visitedClasses;
  if (ftv)
  {
    ol.pushGeneratorState();
    ol.disable(OutputType::Html);
  }
  bool started=FALSE;
  writeClassTreeForList(ol,*Doxygen::classLinkedMap,started,ftv,addToIndex,ct,visitedClasses);
  writeClassTreeForList(ol,*Doxygen::hiddenClassLinkedMap,started,ftv,addToIndex,ct,visitedClasses);
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

static int countClassesInTreeList(const ClassLinkedMap &cl, ClassDef::CompoundType ct)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  int count=0;
  for (const auto &cd : cl)
  {
    if (sliceOpt && cd->compoundType() != ct)
    {
      continue;
    }
    if (!classHasVisibleRoot(cd->baseClasses())) // filter on root classes
    {
      if (cd->isVisibleInHierarchy()) // should it be visible
      {
        if (!cd->subClasses().empty()) // should have sub classes
        {
          count++;
        }
      }
    }
  }
  return count;
}

static int countClassHierarchy(ClassDef::CompoundType ct)
{
  int count=0;
  count+=countClassesInTreeList(*Doxygen::classLinkedMap, ct);
  count+=countClassesInTreeList(*Doxygen::hiddenClassLinkedMap, ct);
  return count;
}

//----------------------------------------------------------------------------

static void writeHierarchicalIndex(OutputList &ol)
{
  if (Index::instance().numHierarchyClasses()==0) return;
  ol.pushGeneratorState();
  //1.{
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trClassHierarchy();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"hierarchy",QCString(), title, HighlightedItem::ClassHierarchy);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();

  if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
  {
  ol.pushGeneratorState();
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::RTF);
    ol.disable(OutputType::Docbook);
    ol.startParagraph();
    ol.startTextLink("inherits",QCString());
    ol.parseText(theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.endParagraph();
  ol.popGeneratorState();
  }
  ol.parseText(lne ? lne->intro() : theTranslator->trClassHierarchyDescription());
  ol.endTextBlock();

  // ---------------
  // Static class hierarchy for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeClassHierarchy(ol,nullptr,addToIndex,ClassDef::Class);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  //2.}

  // ---------------
  // Dynamic class hierarchical index for HTML
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"hierarchy",QCString(),TRUE,TRUE);
    }
    FTVHelp ftv(false);
    writeClassHierarchy(ol,&ftv,addToIndex,ClassDef::Class);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.writeString(t.str().c_str());
    ol.popGeneratorState();
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
  if (Index::instance().numHierarchyClasses()==0) return;
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trClassHierarchy();
  startFile(ol,"inherits",QCString(),title,HighlightedItem::ClassHierarchy,FALSE,"hierarchy");
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.startParagraph();
  ol.startTextLink("hierarchy",QCString());
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

static void writeHierarchicalInterfaceIndex(OutputList &ol)
{
  if (Index::instance().numHierarchyInterfaces()==0) return;
  ol.pushGeneratorState();
  //1.{
  ol.disable(OutputType::Man);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::InterfaceHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trInterfaceHierarchy();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"interfacehierarchy",QCString(), title, HighlightedItem::InterfaceHierarchy);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();

  if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
  {
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::RTF);
    ol.startParagraph();
    ol.startTextLink("interfaceinherits",QCString());
    ol.parseText(theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.endParagraph();
    ol.enable(OutputType::Latex);
    ol.enable(OutputType::RTF);
  }
  ol.parseText(lne ? lne->intro() : theTranslator->trInterfaceHierarchyDescription());
  ol.endTextBlock();

  // ---------------
  // Static interface hierarchy for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeClassHierarchy(ol,nullptr,addToIndex,ClassDef::Interface);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  //2.}

  // ---------------
  // Dynamic interface hierarchical index for HTML
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"interfacehierarchy",QCString(),TRUE,TRUE);
    }
    FTVHelp ftv(false);
    writeClassHierarchy(ol,&ftv,addToIndex,ClassDef::Interface);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.writeString(t.str().c_str());
    ol.popGeneratorState();
  }
  ol.popGeneratorState();
  //2.}
  // ------

  endFile(ol);
  ol.popGeneratorState();
  //1.}
}

//----------------------------------------------------------------------------

static void writeGraphicalInterfaceHierarchy(OutputList &ol)
{
  if (Index::instance().numHierarchyInterfaces()==0) return;
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::InterfaceHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trInterfaceHierarchy();
  startFile(ol,"interfaceinherits",QCString(),title,HighlightedItem::InterfaceHierarchy,FALSE,"interfacehierarchy");
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.startParagraph();
  ol.startTextLink("interfacehierarchy",QCString());
  ol.parseText(theTranslator->trGotoTextualHierarchy());
  ol.endTextLink();
  ol.endParagraph();
  ol.endTextBlock();
  DotGfxHierarchyTable g("interface_",ClassDef::Interface);
  ol.writeGraphicalHierarchy(g);
  endFile(ol);
  ol.enableAll();
}

//----------------------------------------------------------------------------

static void writeHierarchicalExceptionIndex(OutputList &ol)
{
  if (Index::instance().numHierarchyExceptions()==0) return;
  ol.pushGeneratorState();
  //1.{
  ol.disable(OutputType::Man);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ExceptionHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trExceptionHierarchy();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"exceptionhierarchy",QCString(), title, HighlightedItem::ExceptionHierarchy);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();

  if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
  {
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::RTF);
    ol.startParagraph();
    ol.startTextLink("exceptioninherits",QCString());
    ol.parseText(theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.endParagraph();
    ol.enable(OutputType::Latex);
    ol.enable(OutputType::RTF);
  }
  ol.parseText(lne ? lne->intro() : theTranslator->trExceptionHierarchyDescription());
  ol.endTextBlock();

  // ---------------
  // Static exception hierarchy for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeClassHierarchy(ol,nullptr,addToIndex,ClassDef::Exception);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  //2.}

  // ---------------
  // Dynamic exception hierarchical index for HTML
  // ---------------
  ol.pushGeneratorState();
  //2.{
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"exceptionhierarchy",QCString(),TRUE,TRUE);
    }
    FTVHelp ftv(false);
    writeClassHierarchy(ol,&ftv,addToIndex,ClassDef::Exception);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.writeString(t.str().c_str());
    ol.popGeneratorState();
  }
  ol.popGeneratorState();
  //2.}
  // ------

  endFile(ol);
  ol.popGeneratorState();
  //1.}
}

//----------------------------------------------------------------------------

static void writeGraphicalExceptionHierarchy(OutputList &ol)
{
  if (Index::instance().numHierarchyExceptions()==0) return;
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ExceptionHierarchy);
  QCString title = lne ? lne->title() : theTranslator->trExceptionHierarchy();
  startFile(ol,"exceptioninherits",QCString(),title,HighlightedItem::ExceptionHierarchy,FALSE,"exceptionhierarchy");
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.startParagraph();
  ol.startTextLink("exceptionhierarchy",QCString());
  ol.parseText(theTranslator->trGotoTextualHierarchy());
  ol.endTextLink();
  ol.endParagraph();
  ol.endTextBlock();
  DotGfxHierarchyTable g("exception_",ClassDef::Exception);
  ol.writeGraphicalHierarchy(g);
  endFile(ol);
  ol.enableAll();
}

//----------------------------------------------------------------------------

static void countFiles(int &allFiles,int &docFiles)
{
  allFiles=0;
  docFiles=0;
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd: *fn)
    {
      bool src = false;
      bool doc = fileVisibleInIndex(fd.get(),src);
      if (doc || src)
      {
        allFiles++;
      }
      if (doc)
      {
        docFiles++;
      }
    }
  }
}

static void writeSingleFileIndex(OutputList &ol,const FileDef *fd)
{
  //printf("Found filedef %s\n",qPrint(fd->name()));
  bool doc = fd->isLinkableInProject();
  bool src = fd->generateSourceFile();
  bool nameOk = !fd->isDocumentationFile();
  if (nameOk && (doc || src) && !fd->isReference())
  {
    QCString path;
    if (Config_getBool(FULL_PATH_NAMES))
    {
      path=stripFromPath(fd->getPath());
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
      ol.writeObjectLink(QCString(),fd->getOutputFileBase(),QCString(),fd->name());
      //if (addToIndex)
      //{
      //  addMembersToIndex(fd,LayoutDocManager::File,fullName,QCString());
      //}
    }
    else if (src)
    {
      ol.writeObjectLink(QCString(),fd->getSourceFileBase(),QCString(),fd->name());
    }
    if (doc && src)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.docify(" ");
      ol.startTextLink(fd->includeName(),QCString());
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
      ol.generateDoc(
          fd->briefFile(),fd->briefLine(),
          fd,nullptr,
          fd->briefDescription(TRUE),
          FALSE, // index words
          FALSE, // isExample
          QCString(), // example name
          TRUE,  // single line
          TRUE,  // link from index
          Config_getBool(MARKDOWN_SUPPORT)
          );
      //ol.docify(")");
    }
    if (doc)
    {
      ol.endIndexValue(fd->getOutputFileBase(),hasBrief);
    }
    else // src
    {
      ol.endIndexValue(fd->getSourceFileBase(),hasBrief);
    }
    //ol.popGeneratorState();
    // --------------------------------------------------------
  }
}

//----------------------------------------------------------------------------

static void writeFileIndex(OutputList &ol)
{
  if (Index::instance().numDocumentedFiles()==0 || !Config_getBool(SHOW_FILES)) return;

  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);

  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileList);
  if (lne==nullptr) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Files); // fall back
  QCString title = lne ? lne->title() : theTranslator->trFileList();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"files",QCString(),title,HighlightedItem::Files);
  startTitle(ol,QCString());
  //if (!Config_getString(PROJECT_NAME).isEmpty())
  //{
  //  title.prepend(Config_getString(PROJECT_NAME)+" ");
  //}
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"files",QCString(),TRUE,TRUE);
    Doxygen::indexList->incContentsDepth();
  }

  ol.parseText(lne ? lne->intro() : theTranslator->trFileListDescription(Config_getBool(EXTRACT_ALL)));
  ol.endTextBlock();

  // ---------------
  // Flat file index
  // ---------------

  // 1. {
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);

  ol.startIndexList();
  if (Config_getBool(FULL_PATH_NAMES))
  {
    std::unordered_map<std::string,size_t> pathMap;
    std::vector<FilesInDir> outputFiles;

    // re-sort input files in (dir,file) output order instead of (file,dir) input order
    for (const auto &fn : *Doxygen::inputNameLinkedMap)
    {
      for (const auto &fd : *fn)
      {
        QCString path=fd->getPath();
        if (path.isEmpty()) path="[external]";
        auto it = pathMap.find(path.str());
        if (it!=pathMap.end()) // existing path -> append
        {
          outputFiles.at(it->second).files.push_back(fd.get());
        }
        else // new path -> create path entry + append
        {
          pathMap.emplace(path.str(),outputFiles.size());
          outputFiles.emplace_back(path);
          outputFiles.back().files.push_back(fd.get());
        }
      }
    }

    // sort the files by path
    std::stable_sort(outputFiles.begin(),
              outputFiles.end(),
              [](const auto &fp1,const auto &fp2) { return qstricmp_sort(fp1.path,fp2.path)<0; });
    // sort the files inside the directory by name
    for (auto &fp : outputFiles)
    {
      std::stable_sort(fp.files.begin(), fp.files.end(), compareFileDefs);
    }
    // write the results
    for (const auto &fp : outputFiles)
    {
      for (const auto &fd : fp.files)
      {
        writeSingleFileIndex(ol,fd);
      }
    }
  }
  else
  {
    for (const auto &fn : *Doxygen::inputNameLinkedMap)
    {
     for (const auto &fd : *fn)
      {
        writeSingleFileIndex(ol,fd.get());
      }
    }
  }
  ol.endIndexList();

  // 1. }
  ol.popGeneratorState();

  // ---------------
  // Hierarchical file index for HTML
  // ---------------
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  {
    FTVHelp ftv(false);
    writeDirHierarchy(ol,&ftv,addToIndex);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
  }

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
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    if (nd->isLinkableInProject()) count++;
  }
  return count;
}

//----------------------------------------------------------------------------
static int countConcepts()
{
  int count=0;
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if (cd->isLinkableInProject()) count++;
  }
  return count;
}


//----------------------------------------------------------------------------
template<typename Ptr> const ClassDef *get_pointer(const Ptr &p);
template<>             const ClassDef *get_pointer(const ClassLinkedMap::Ptr &p) { return p.get(); }
template<>             const ClassDef *get_pointer(const ClassLinkedRefMap::Ptr &p) { return p; }

template<class ListType>
static void writeClassTree(const ListType &cl,FTVHelp *ftv,bool addToIndex,bool globalOnly,ClassDef::CompoundType ct)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  for (const auto &cdi : cl)
  {
    const ClassDef *cd = get_pointer(cdi);
    if (cd->getLanguage()==SrcLangExt::VHDL)
    {
      if (VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKAGECLASS ||
          VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKBODYCLASS
         )// no architecture
      {
        continue;
      }
    }

    if (sliceOpt && cd->compoundType() != ct)
    {
      continue;
    }

    if (!globalOnly ||
         cd->getOuterScope()==nullptr ||
         cd->getOuterScope()==Doxygen::globalScope
       )
    {
      int count=0;
      for (const auto &ccd : cd->getClasses())
      {
        if (ccd->isLinkableInProject() && !ccd->isImplicitTemplateInstance())
        {
          count++;
        }
      }
      if (classVisibleInIndex(cd) && !cd->isImplicitTemplateInstance())
      {
        if (ftv)
        {
          ftv->addContentsItem(count>0,cd->displayName(FALSE),cd->getReference(),
                               cd->getOutputFileBase(),cd->anchor(),FALSE,TRUE,cd);
        }
        if (addToIndex &&
            (cd->getOuterScope()==nullptr ||
             cd->getOuterScope()->definitionType()!=Definition::TypeClass
            )
           )
        {
          addMembersToIndex(cd,LayoutDocManager::Class,
                            cd->displayName(FALSE),
                            cd->anchor(),
                            cd->partOfGroups().empty() && !cd->isSimple());
        }
        if (count>0)
        {
          if (ftv) ftv->incContentsDepth();
          writeClassTree(cd->getClasses(),ftv,addToIndex,FALSE,ct);
          if (ftv) ftv->decContentsDepth();
        }
      }
    }
  }
}

static void writeNamespaceMembers(const NamespaceDef *nd,bool addToIndex)
{
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Namespace))
  {
    if (lde->kind()==LayoutDocEntry::MemberDef)
    {
      const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
      if (lmd)
      {
        MemberList *ml = nd->getMemberList(lmd->type);
        if (ml)
        {
          for (const auto &md : *ml)
          {
            //printf("  member %s visible=%d\n",qPrint(md->name()),md->visibleInIndex());
            if (md->visibleInIndex())
            {
              writeMemberToIndex(nd,md,addToIndex);
            }
          }
        }
      }
    }
  }
}

static void writeModuleMembers(const ModuleDef *mod,bool addToIndex)
{
  for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Module))
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      const LayoutDocEntryMemberDecl *lmd = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get());
      if (lmd)
      {
        MemberList *ml = mod->getMemberList(lmd->type);
        if (ml)
        {
          for (const auto &md : *ml)
          {
            //printf("  member %s visible=%d\n",qPrint(md->name()),md->visibleInIndex());
            if (md->visibleInIndex())
            {
              writeMemberToIndex(mod,md,addToIndex);
            }
          }
        }
      }
    }
  }
}


static void writeConceptList(const ConceptLinkedRefMap &concepts, FTVHelp *ftv,bool addToIndex);
static void writeNamespaceTree(const NamespaceLinkedRefMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex);

static void writeNamespaceTreeElement(const NamespaceDef *nd,FTVHelp *ftv,
                                      bool rootOnly,bool addToIndex)
{
  if (!nd->isAnonymous() &&
      (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
  {

    bool hasNestedNamespace = namespaceHasNestedNamespace(nd);
    bool hasChildren = hasNestedNamespace ||
      namespaceHasNestedClass(nd,false,ClassDef::Class) ||
      namespaceHasNestedConcept(nd);
    bool isLinkable  = nd->isLinkable();
    int visibleMembers = nd->countVisibleMembers();

    //printf("namespace %s hasChildren=%d visibleMembers=%d\n",qPrint(nd->name()),hasChildren,visibleMembers);

    QCString ref;
    QCString file;
    if (isLinkable)
    {
      ref  = nd->getReference();
      file = nd->getOutputFileBase();
      if (nd->getLanguage()==SrcLangExt::VHDL) // UGLY HACK
      {
        file=file.replace(0,qstrlen("namespace"),"class");
      }
    }

    bool isDir = hasChildren || visibleMembers>0;
    if (isLinkable || isDir)
    {
      ftv->addContentsItem(hasNestedNamespace,nd->localName(),ref,file,QCString(),FALSE,nd->partOfGroups().empty(),nd);

      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(isDir,nd->localName(),ref,file,QCString(),
            hasChildren && !file.isEmpty(),nd->partOfGroups().empty());
      }
      if (addToIndex && isDir)
      {
        Doxygen::indexList->incContentsDepth();
      }

      if (isDir)
      {
        ftv->incContentsDepth();
        writeNamespaceTree(nd->getNamespaces(),ftv,FALSE,addToIndex);
        writeClassTree(nd->getClasses(),nullptr,addToIndex,FALSE,ClassDef::Class);
        writeConceptList(nd->getConcepts(),nullptr,addToIndex);
        writeNamespaceMembers(nd,addToIndex);
        ftv->decContentsDepth();
      }
      if (addToIndex && isDir)
      {
        Doxygen::indexList->decContentsDepth();
      }
    }
  }
}

static void writeNamespaceTree(const NamespaceLinkedRefMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex)
{
  for (const auto &nd : nsLinkedMap)
  {
    if (nd->isVisibleInHierarchy())
    {
      writeNamespaceTreeElement(nd,ftv,rootOnly,addToIndex);
    }
  }
}

static void writeNamespaceTree(const NamespaceLinkedMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex)
{
  for (const auto &nd : nsLinkedMap)
  {
    if (nd->isVisibleInHierarchy())
    {
      writeNamespaceTreeElement(nd.get(),ftv,rootOnly,addToIndex);
    }
  }
}

static void writeClassTreeInsideNamespace(const NamespaceLinkedRefMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex,ClassDef::CompoundType ct);

static void writeClassTreeInsideNamespaceElement(const NamespaceDef *nd,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex,ClassDef::CompoundType ct)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  if (!nd->isAnonymous() &&
      (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
  {
    bool isDir = namespaceHasNestedClass(nd,sliceOpt,ct);
    bool isLinkable  = nd->isLinkableInProject();

    //printf("writeClassTreeInsideNamespaceElement namespace %s isLinkable=%d isDir=%d\n",qPrint(nd->name()),isLinkable,isDir);

    QCString ref;
    QCString file;
    if (isLinkable)
    {
      ref  = nd->getReference();
      file = nd->getOutputFileBase();
      if (nd->getLanguage()==SrcLangExt::VHDL) // UGLY HACK
      {
        file=file.replace(0,qstrlen("namespace"),"class");
      }
    }

    if (isDir)
    {
      ftv->addContentsItem(isDir,nd->localName(),ref,file,QCString(),FALSE,TRUE,nd);

      if (addToIndex)
      {
        // the namespace entry is already shown under the namespace list so don't
        // add it to the nav index and don't create a separate index file for it otherwise
        // it will overwrite the one written for the namespace list.
        Doxygen::indexList->addContentsItem(isDir,nd->localName(),ref,file,QCString(),
            false, // separateIndex
            false  // addToNavIndex
            );
      }
      if (addToIndex)
      {
        Doxygen::indexList->incContentsDepth();
      }

      ftv->incContentsDepth();
      writeClassTreeInsideNamespace(nd->getNamespaces(),ftv,FALSE,addToIndex,ct);
      ClassLinkedRefMap d = nd->getClasses();
      if (sliceOpt)
      {
        if (ct == ClassDef::Interface)
        {
          d = nd->getInterfaces();
        }
        else if (ct == ClassDef::Struct)
        {
          d = nd->getStructs();
        }
        else if (ct == ClassDef::Exception)
        {
          d = nd->getExceptions();
        }
      }
      writeClassTree(d,ftv,addToIndex,FALSE,ct);
      ftv->decContentsDepth();

      if (addToIndex)
      {
        Doxygen::indexList->decContentsDepth();
      }
    }
  }
}

static void writeClassTreeInsideNamespace(const NamespaceLinkedRefMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex,ClassDef::CompoundType ct)
{
  for (const auto &nd : nsLinkedMap)
  {
    writeClassTreeInsideNamespaceElement(nd,ftv,rootOnly,addToIndex,ct);
  }
}

static void writeClassTreeInsideNamespace(const NamespaceLinkedMap &nsLinkedMap,FTVHelp *ftv,
                               bool rootOnly,bool addToIndex,ClassDef::CompoundType ct)
{
  for (const auto &nd : nsLinkedMap)
  {
    writeClassTreeInsideNamespaceElement(nd.get(),ftv,rootOnly,addToIndex,ct);
  }
}

static void writeNamespaceIndex(OutputList &ol)
{
  if (Index::instance().numDocumentedNamespaces()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceList);
  if (lne==nullptr) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Namespaces); // fall back
  QCString title = lne ? lne->title() : theTranslator->trNamespaceList();
  bool addToIndex = lne==nullptr || lne->visible();
  startFile(ol,"namespaces",QCString(),title,HighlightedItem::Namespaces);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trNamespaceListDescription(Config_getBool(EXTRACT_ALL)));
  ol.endTextBlock();

  bool first=TRUE;

  // ---------------
  // Linear namespace index for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);

  for (const auto &nd : *Doxygen::namespaceLinkedMap)
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
      if (nd->getLanguage()==SrcLangExt::VHDL)
      {
        ol.writeObjectLink(QCString(), nd->getOutputFileBase().replace(0,qstrlen("namespace"),"class"),QCString(),nd->displayName());
      }
      else
      {
        ol.writeObjectLink(QCString(),nd->getOutputFileBase(),QCString(),nd->displayName());
      }
      ol.endIndexKey();

      bool hasBrief = !nd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        //ol.docify(" (");
        ol.generateDoc(
                 nd->briefFile(),nd->briefLine(),
                 nd.get(),nullptr,
                 nd->briefDescription(TRUE),
                 FALSE, // index words
                 FALSE, // isExample
                 QCString(),     // example name
                 TRUE,  // single line
                 TRUE,  // link from index
                 Config_getBool(MARKDOWN_SUPPORT)
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
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"namespaces",QCString(),TRUE,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp ftv(false);
    writeNamespaceTree(*Doxygen::namespaceLinkedMap,&ftv,TRUE,addToIndex);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
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

static int countAnnotatedClasses(int *cp, ClassDef::CompoundType ct)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  int count=0;
  int countPrinted=0;
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (sliceOpt && cd->compoundType() != ct)
    {
      continue;
    }
    if (cd->isLinkableInProject() && !cd->isImplicitTemplateInstance())
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


static void writeAnnotatedClassList(OutputList &ol,ClassDef::CompoundType ct)
{
  //LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassList);
  //bool addToIndex = lne==nullptr || lne->visible();
  bool first=TRUE;

  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);

  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (cd->getLanguage()==SrcLangExt::VHDL &&
        (VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKAGECLASS ||
         VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKBODYCLASS)
       ) // no architecture
    {
      continue;
    }
    if (first)
    {
      ol.startIndexList();
      first=FALSE;
    }

    if (sliceOpt && cd->compoundType() != ct)
    {
      continue;
    }

    ol.pushGeneratorState();
    if (cd->isEmbeddedInOuterScope())
    {
      ol.disable(OutputType::Latex);
      ol.disable(OutputType::Docbook);
      ol.disable(OutputType::RTF);
    }
    if (cd->isLinkableInProject() && !cd->isImplicitTemplateInstance())
    {
      ol.startIndexKey();
      if (cd->getLanguage()==SrcLangExt::VHDL)
      {
        QCString prot= VhdlDocGen::getProtectionName(VhdlDocGen::convert(cd->protection()));
        ol.docify(prot);
        ol.writeString(" ");
      }
      ol.writeObjectLink(QCString(),cd->getOutputFileBase(),cd->anchor(),cd->displayName());
      ol.endIndexKey();
      bool hasBrief = !cd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        ol.generateDoc(
                 cd->briefFile(),cd->briefLine(),
                 cd.get(),nullptr,
                 cd->briefDescription(TRUE),
                 FALSE,  // indexWords
                 FALSE,  // isExample
                 QCString(),     // example name
                 TRUE,  // single line
                 TRUE,  // link from index
                 Config_getBool(MARKDOWN_SUPPORT)
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
  if (!first) ol.endIndexList();
}

inline bool isId1(int c)
{
  return (c<127 && c>31); // printable ASCII character
}

static QCString letterToLabel(const QCString &startLetter)
{
  if (startLetter.isEmpty()) return startLetter;
  const char *p = startLetter.data();
  char c = *p;
  QCString result;
  if (isId1(c))
  {
    result+=c;
  }
  else
  {
    result="0x";
    const char hex[]="0123456789abcdef";
    while ((c=*p++))
    {
      result+=hex[static_cast<unsigned char>(c)>>4];
      result+=hex[static_cast<unsigned char>(c)&0xf];
    }
  }
  return result;
}

//----------------------------------------------------------------------------

/** Class representing a cell in the alphabetical class index. */
class AlphaIndexTableCell
{
  public:
    AlphaIndexTableCell(int row,int col,const std::string &letter,const ClassDef *cd) :
      m_letter(letter), m_class(cd), m_row(row), m_col(col)
    {
    }

    const ClassDef *classDef() const { return m_class; }
    std::string letter()       const { return m_letter; }
    int row()                  const { return m_row; }
    int column()               const { return m_col; }

  private:
    std::string m_letter;
    const ClassDef *m_class;
    int m_row;
    int m_col;
};

using UsedIndexLetters = std::set<std::string>;

// write an alphabetical index of all class with a header for each letter
static void writeAlphabeticalClassList(OutputList &ol, ClassDef::CompoundType ct, int /* annotatedCount */)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);

  // What starting letters are used
  UsedIndexLetters indexLettersUsed;

  // first count the number of headers
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (sliceOpt && cd->compoundType() != ct)
      continue;
    if (cd->isLinkableInProject() && !cd->isImplicitTemplateInstance())
    {
      if (cd->getLanguage()==SrcLangExt::VHDL && !(VhdlDocGen::convert(cd->protection())==VhdlDocGen::ENTITYCLASS ))// no architecture
        continue;

      // get the first UTF8 character (after the part that should be ignored)
      int index = getPrefixIndex(cd->className());
      std::string letter = getUTF8CharAt(cd->className().str(),index);
      if (!letter.empty())
      {
        indexLettersUsed.insert(convertUTF8ToUpper(letter));
      }
    }
  }

  // write quick link index (row of letters)
  QCString alphaLinks = "<div class=\"qindex\">";
  bool first=true;
  for (const auto &letter : indexLettersUsed)
  {
    if (!first) alphaLinks += "&#160;|&#160;";
    first=false;
    QCString li = letterToLabel(letter.c_str());
    alphaLinks += "<a class=\"qindex\" href=\"#letter_" +
                  li + "\">" +
                  QCString(letter) + "</a>";
  }
  alphaLinks += "</div>\n";
  ol.writeString(alphaLinks);

  std::map<std::string, std::vector<const ClassDef*> > classesByLetter;

  // fill the columns with the class list (row elements in each column,
  // expect for the columns with number >= itemsInLastRow, which get one
  // item less.
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (sliceOpt && cd->compoundType() != ct)
      continue;
    if (cd->getLanguage()==SrcLangExt::VHDL && !(VhdlDocGen::convert(cd->protection())==VhdlDocGen::ENTITYCLASS ))// no architecture
      continue;

    if (cd->isLinkableInProject() && !cd->isImplicitTemplateInstance())
    {
      QCString className = cd->className();
      int index = getPrefixIndex(className);
      std::string letter = getUTF8CharAt(className.str(),index);
      if (!letter.empty())
      {
        letter = convertUTF8ToUpper(letter);
        auto it = classesByLetter.find(letter);
        if (it!=classesByLetter.end()) // add class to the existing list
        {
          it->second.push_back(cd.get());
        }
        else // new entry
        {
          classesByLetter.emplace(letter, std::vector<const ClassDef*>({ cd.get() }));
        }
      }
    }
  }

  // sort the class lists per letter while ignoring the prefix
  for (auto &[letter,list] : classesByLetter)
  {
    std::stable_sort(list.begin(), list.end(),
              [](const auto &c1,const auto &c2)
              {
                QCString n1 = c1->className();
                QCString n2 = c2->className();
                return qstricmp_sort(n1.data()+getPrefixIndex(n1), n2.data()+getPrefixIndex(n2))<0;
              });
  }

  // generate table
  if (!classesByLetter.empty())
  {
    ol.writeString("<div class=\"classindex\">\n");
    int counter=0;
    for (const auto &cl : classesByLetter)
    {
      QCString parity = (counter++%2)==0 ? "even" : "odd";
      ol.writeString("<dl class=\"classindex " + parity + "\">\n");

      // write character heading
      ol.writeString("<dt class=\"alphachar\">");
      QCString s = letterToLabel(cl.first.c_str());
      ol.writeString("<a id=\"letter_");
      ol.writeString(s);
      ol.writeString("\" name=\"letter_");
      ol.writeString(s);
      ol.writeString("\">");
      ol.writeString(cl.first.c_str());
      ol.writeString("</a>");
      ol.writeString("</dt>\n");

      // write class links
      for (const auto &cd : cl.second)
      {
        ol.writeString("<dd>");
        QCString namesp,cname;
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
          ol.writeString(" (");
          NamespaceDef *nd = getResolvedNamespace(namesp);
          if (nd && nd->isLinkable())
          {
            ol.writeObjectLink(nd->getReference(),
                nd->getOutputFileBase(),QCString(),nsDispName);
          }
          else
          {
            ol.docify(nsDispName);
          }
          ol.writeString(")");
        }
        ol.writeString("</dd>");
      }

      ol.writeString("</dl>\n");
    }
    ol.writeString("</div>\n");
  }
}

//----------------------------------------------------------------------------

static void writeAlphabeticalIndex(OutputList &ol)
{
  if (Index::instance().numAnnotatedClasses()==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassIndex);
  QCString title = lne ? lne->title() : theTranslator->trCompoundIndex();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"classes",QCString(),title,HighlightedItem::Classes);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,title,QCString(),"classes",QCString(),FALSE,TRUE);
  }

  ol.startContents();
  writeAlphabeticalClassList(ol, ClassDef::Class, Index::instance().numAnnotatedClasses());
  endFile(ol); // contains ol.endContents()

  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static void writeAlphabeticalInterfaceIndex(OutputList &ol)
{
  if (Index::instance().numAnnotatedInterfaces()==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::InterfaceIndex);
  QCString title = lne ? lne->title() : theTranslator->trInterfaceIndex();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"interfaces",QCString(),title,HighlightedItem::Interfaces);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,title,QCString(),"interfaces",QCString(),FALSE,TRUE);
  }

  ol.startContents();
  writeAlphabeticalClassList(ol, ClassDef::Interface, Index::instance().numAnnotatedInterfaces());
  endFile(ol); // contains ol.endContents()

  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static void writeAlphabeticalStructIndex(OutputList &ol)
{
  if (Index::instance().numAnnotatedStructs()==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::StructIndex);
  QCString title = lne ? lne->title() : theTranslator->trStructIndex();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"structs",QCString(),title,HighlightedItem::Structs);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,title,QCString(),"structs",QCString(),FALSE,TRUE);
  }

  ol.startContents();
  writeAlphabeticalClassList(ol, ClassDef::Struct, Index::instance().numAnnotatedStructs());
  endFile(ol); // contains ol.endContents()

  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static void writeAlphabeticalExceptionIndex(OutputList &ol)
{
  if (Index::instance().numAnnotatedExceptions()==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ExceptionIndex);
  QCString title = lne ? lne->title() : theTranslator->trExceptionIndex();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"exceptions",QCString(),title,HighlightedItem::Exceptions);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(FALSE,title,QCString(),"exceptions",QCString(),FALSE,TRUE);
  }

  ol.startContents();
  writeAlphabeticalClassList(ol, ClassDef::Exception, Index::instance().numAnnotatedExceptions());
  endFile(ol); // contains ol.endContents()

  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

struct AnnotatedIndexContext
{
  AnnotatedIndexContext(int numAnno,int numPrint,
                        LayoutNavEntry::Kind lk,LayoutNavEntry::Kind fk,
                        const QCString &title,const QCString &intro,
                        ClassDef::CompoundType ct,
                        const QCString &fn,
                        HighlightedItem hi) :
    numAnnotated(numAnno), numPrinted(numPrint),
    listKind(lk), fallbackKind(fk),
    listDefaultTitleText(title), listDefaultIntroText(intro),
    compoundType(ct),fileBaseName(fn),
    hiItem(hi) { }

  const int numAnnotated;
  const int numPrinted;
  const LayoutNavEntry::Kind listKind;
  const LayoutNavEntry::Kind fallbackKind;
  const QCString listDefaultTitleText;
  const QCString listDefaultIntroText;
  const ClassDef::CompoundType compoundType;
  const QCString fileBaseName;
  const HighlightedItem hiItem;
};

static void writeAnnotatedIndexGeneric(OutputList &ol,const AnnotatedIndexContext ctx)
{
  //printf("writeAnnotatedIndex: count=%d printed=%d\n",
  //    annotatedClasses,annotatedClassesPrinted);
  if (ctx.numAnnotated==0) return;

  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  if (ctx.numPrinted==0)
  {
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::RTF);
  }
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(ctx.listKind);
  if (lne==nullptr) lne = LayoutDocManager::instance().rootNavEntry()->find(ctx.fallbackKind); // fall back
  QCString title = lne ? lne->title() : ctx.listDefaultTitleText;
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,ctx.fileBaseName,QCString(),title,ctx.hiItem);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  ol.startContents();

  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : ctx.listDefaultIntroText);
  ol.endTextBlock();

  // ---------------
  // Linear class index for Latex/RTF
  // ---------------
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeAnnotatedClassList(ol, ctx.compoundType);

  Doxygen::indexList->enable();
  ol.popGeneratorState();

  // ---------------
  // Hierarchical class index for HTML
  // ---------------
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),ctx.fileBaseName,QCString(),TRUE,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp ftv(false);
    writeClassTreeInsideNamespace(*Doxygen::namespaceLinkedMap,&ftv,TRUE,addToIndex,ctx.compoundType);
    writeClassTree(*Doxygen::classLinkedMap,&ftv,addToIndex,TRUE,ctx.compoundType);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
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

static void writeAnnotatedIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  writeAnnotatedIndexGeneric(ol,
      AnnotatedIndexContext(index.numAnnotatedClasses(),index.numAnnotatedClassesPrinted(),
                            LayoutNavEntry::ClassList,LayoutNavEntry::Classes,
                            theTranslator->trCompoundList(),theTranslator->trCompoundListDescription(),
                            ClassDef::Class,
                            "annotated",
                            HighlightedItem::AnnotatedClasses));
}

//----------------------------------------------------------------------------

static void writeAnnotatedInterfaceIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  writeAnnotatedIndexGeneric(ol,
      AnnotatedIndexContext(index.numAnnotatedInterfaces(),index.numAnnotatedInterfacesPrinted(),
                            LayoutNavEntry::InterfaceList,LayoutNavEntry::Interfaces,
                            theTranslator->trInterfaceList(),theTranslator->trInterfaceListDescription(),
                            ClassDef::Interface,
                            "annotatedinterfaces",
                            HighlightedItem::AnnotatedInterfaces));
}

//----------------------------------------------------------------------------

static void writeAnnotatedStructIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  writeAnnotatedIndexGeneric(ol,
      AnnotatedIndexContext(index.numAnnotatedStructs(),index.numAnnotatedStructsPrinted(),
                            LayoutNavEntry::StructList,LayoutNavEntry::Structs,
                            theTranslator->trStructList(),theTranslator->trStructListDescription(),
                            ClassDef::Struct,
                            "annotatedstructs",
                            HighlightedItem::AnnotatedStructs));
}

//----------------------------------------------------------------------------

static void writeAnnotatedExceptionIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  writeAnnotatedIndexGeneric(ol,
      AnnotatedIndexContext(index.numAnnotatedExceptions(),index.numAnnotatedExceptionsPrinted(),
                            LayoutNavEntry::ExceptionList,LayoutNavEntry::Exceptions,
                            theTranslator->trExceptionList(),theTranslator->trExceptionListDescription(),
                            ClassDef::Exception,
                            "annotatedexceptions",
                            HighlightedItem::AnnotatedExceptions));
}

//----------------------------------------------------------------------------
static void writeClassLinkForMember(OutputList &ol,const MemberDef *md,const QCString &separator,
                             QCString &prevClassName)
{
  const ClassDef *cd=md->getClassDef();
  if ( cd && prevClassName!=cd->displayName())
  {
    ol.writeString(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        cd->displayName());
    prevClassName = cd->displayName();
  }
}

static void writeFileLinkForMember(OutputList &ol,const MemberDef *md,const QCString &separator,
                             QCString &prevFileName)
{
  const FileDef *fd=md->getFileDef();
  if (fd && prevFileName!=fd->name())
  {
    ol.writeString(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        fd->name());
    prevFileName = fd->name();
  }
}

static void writeNamespaceLinkForMember(OutputList &ol,const MemberDef *md,const QCString &separator,
                             QCString &prevNamespaceName)
{
  const NamespaceDef *nd=md->getNamespaceDef();
  if (nd && prevNamespaceName!=nd->displayName())
  {
    ol.writeString(separator);
    ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
        nd->displayName());
    prevNamespaceName = nd->displayName();
  }
}

static void writeModuleLinkForMember(OutputList &ol,const MemberDef *md,const QCString &separator,
                             QCString &prevModuleName)
{
  const ModuleDef *mod=md->getModuleDef();
  if (mod && prevModuleName!=mod->displayName())
  {
    ol.writeString(separator);
    // link to the member declaration in the module page
    ol.writeObjectLink(mod->getReference(),mod->getOutputFileBase(),"r_"+md->anchor(),
        mod->displayName());
    prevModuleName = mod->displayName();
  }
}


static void writeMemberList(OutputList &ol,bool useSections,const std::string &page,
                            const Index::MemberIndexMap &memberIndexMap,
                            Definition::DefType type)
{
  int index = static_cast<int>(type);
  const int numIndices = 4;
  ASSERT(index<numIndices);

  typedef void (*writeLinkForMember_t)(OutputList &ol,const MemberDef *md,const QCString &separator,
                                   QCString &prevNamespaceName);

  // each index tab has its own write function
  static writeLinkForMember_t writeLinkForMemberMap[numIndices] =
  {
    &writeClassLinkForMember,
    &writeFileLinkForMember,
    &writeNamespaceLinkForMember,
    &writeModuleLinkForMember
  };
  QCString prevName;
  QCString prevDefName;
  bool first=TRUE;
  bool firstSection=TRUE;
  bool firstItem=TRUE;
  const Index::MemberIndexList *mil = nullptr;
  std::string letter;
  for (const auto &kv : memberIndexMap)
  {
    if (!page.empty()) // specific page mode
    {
      auto it = memberIndexMap.find(page);
      if (it != memberIndexMap.end())
      {
        mil = &it->second;
        letter = page;
      }
    }
    else // do all pages
    {
      mil = &kv.second;
      letter = kv.first;
    }
    if (mil==nullptr || mil->empty()) continue;
    for (const auto &md : *mil)
    {
      const char *sep = nullptr;
      bool isFunc=!md->isObjCMethod() &&
        (md->isFunction() || md->isSlot() || md->isSignal());
      QCString name=type==Definition::TypeModule ? md->qualifiedName() : md->name();
      int startIndex = getPrefixIndex(name);
      if (name.data()+startIndex!=prevName) // new entry
      {
        if ((prevName.isEmpty() ||
            tolower(name.at(startIndex))!=tolower(prevName.at(0))) &&
            useSections) // new section
        {
          if (!firstItem)    ol.endItemListItem();
          if (!firstSection) ol.endItemList();
          QCString cs = letterToLabel(letter.c_str());
          QCString anchor=QCString("index_")+convertToId(cs);
          QCString title=QCString("- ")+letter.c_str()+" -";
          ol.startSection(anchor,title,SectionType::Subsection);
          ol.docify(title);
          ol.endSection(anchor,SectionType::Subsection);
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
        //ol.writeString("\n");

        // link to class
        prevDefName="";
        sep = "&#160;:&#160;";
        prevName = name.data()+startIndex;
      }
      else // same entry
      {
        sep = ", ";
        // link to class for other members with the same name
      }
      if (index<numIndices)
      {
        // write the link for the specific list type
        writeLinkForMemberMap[index](ol,md,sep,prevDefName);
      }
    }
    if (!page.empty())
    {
      break;
    }
  }
  if (!firstItem) ol.endItemListItem();
  ol.endItemList();
}

//----------------------------------------------------------------------------

void Index::addClassMemberNameToIndex(const MemberDef *md)
{
  bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  const ClassDef *cd=nullptr;

  if (md->isLinkableInProject() &&
      (cd=md->getClassDef())    &&
      cd->isLinkableInProject() &&
      !cd->isImplicitTemplateInstance())
  {
    QCString n = md->name();
    std::string letter = getUTF8CharAt(n.str(),getPrefixIndex(n));
    if (!letter.empty())
    {
      letter = convertUTF8ToLower(letter);
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide) &&
          (!md->isEnumValue() || (md->getEnumScope() && !md->getEnumScope()->isStrong()))
         )
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::All,letter,md);
      }
      if (md->isFunction()  || md->isSlot() || md->isSignal())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Functions,letter,md);
      }
      else if (md->isVariable())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Variables,letter,md);
      }
      else if (md->isTypedef())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Typedefs,letter,md);
      }
      else if (md->isEnumerate())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Enums,letter,md);
      }
      else if (md->isEnumValue() && md->getEnumScope() && !md->getEnumScope()->isStrong())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::EnumValues,letter,md);
      }
      else if (md->isProperty())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Properties,letter,md);
      }
      else if (md->isEvent())
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Events,letter,md);
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        incrementDocumentedClassMembers(ClassMemberHighlight::Related,letter,md);
      }
    }
  }
}

//----------------------------------------------------------------------------

void Index::addNamespaceMemberNameToIndex(const MemberDef *md)
{
  const NamespaceDef *nd=md->getNamespaceDef();
  if (nd && nd->isLinkableInProject() && md->isLinkableInProject())
  {
    QCString n = md->name();
    std::string letter = getUTF8CharAt(n.str(),getPrefixIndex(n));
    if (!letter.empty())
    {
      letter = convertUTF8ToLower(letter);
      if (!md->isEnumValue() || (md->getEnumScope() && !md->getEnumScope()->isStrong()))
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::All,letter,md);
      }
      if (md->isFunction())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Functions,letter,md);
      }
      else if (md->isVariable())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Variables,letter,md);
      }
      else if (md->isTypedef())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Typedefs,letter,md);
      }
      else if (md->isSequence())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Sequences,letter,md);
      }
      else if (md->isDictionary())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Dictionaries,letter,md);
      }
      else if (md->isEnumerate())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::Enums,letter,md);
      }
      else if (md->isEnumValue() && md->getEnumScope() && !md->getEnumScope()->isStrong())
      {
        incrementDocumentedNamespaceMembers(NamespaceMemberHighlight::EnumValues,letter,md);
      }
    }
  }
}

//----------------------------------------------------------------------------

void Index::addFileMemberNameToIndex(const MemberDef *md)
{
  const FileDef *fd=md->getFileDef();
  if (fd && fd->isLinkableInProject() && md->isLinkableInProject())
  {
    QCString n = md->name();
    std::string letter = getUTF8CharAt(n.str(),getPrefixIndex(n));
    if (!letter.empty())
    {
      letter = convertUTF8ToLower(letter);
      if (!md->isEnumValue() || (md->getEnumScope() && !md->getEnumScope()->isStrong()))
      {
        incrementDocumentedFileMembers(FileMemberHighlight::All,letter,md);
      }
      if (md->isFunction())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Functions,letter,md);
      }
      else if (md->isVariable())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Variables,letter,md);
      }
      else if (md->isTypedef())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Typedefs,letter,md);
      }
      else if (md->isSequence())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Sequences,letter,md);
      }
      else if (md->isDictionary())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Dictionaries,letter,md);
      }
      else if (md->isEnumerate())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Enums,letter,md);
      }
      else if (md->isEnumValue() && md->getEnumScope() && !md->getEnumScope()->isStrong())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::EnumValues,letter,md);
      }
      else if (md->isDefine())
      {
        incrementDocumentedFileMembers(FileMemberHighlight::Defines,letter,md);
      }
    }
  }
}

//----------------------------------------------------------------------------

void Index::addModuleMemberNameToIndex(const MemberDef *md)
{
  const ModuleDef *mod = md->getModuleDef();
  if (mod && mod->isPrimaryInterface() && mod->isLinkableInProject() && md->isLinkableInProject())
  {
    QCString n = md->name();
    std::string letter = getUTF8CharAt(n.str(),getPrefixIndex(n));
    if (!letter.empty())
    {
      letter = convertUTF8ToLower(letter);
      if (!md->isEnumValue() || (md->getEnumScope() && !md->getEnumScope()->isStrong()))
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::All,letter,md);
      }
      if (md->isFunction())
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::Functions,letter,md);
      }
      else if (md->isVariable())
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::Variables,letter,md);
      }
      else if (md->isTypedef())
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::Typedefs,letter,md);
      }
      else if (md->isEnumerate())
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::Enums,letter,md);
      }
      else if (md->isEnumValue() && md->getEnumScope() && !md->getEnumScope()->isStrong())
      {
        incrementDocumentedModuleMembers(ModuleMemberHighlight::EnumValues,letter,md);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void writeQuickMemberIndex(OutputList &ol,
    const Index::MemberIndexMap &map,const std::string &page,
    QCString fullName,bool multiPage)
{
  bool first=TRUE;
  startQuickIndexList(ol,TRUE);
  for (const auto &[letter,list] : map)
  {
    QCString ci(letter);
    QCString is = letterToLabel(ci);
    QCString anchor;
    QCString extension=Doxygen::htmlFileExtension;
    if (!multiPage)
      anchor="#index_";
    else if (first)
      anchor=fullName+extension+"#index_";
    else
      anchor=fullName+"_"+is+extension+"#index_";
    startQuickIndexItem(ol,anchor+convertToId(is),letter==page,TRUE,first);
    ol.writeString(ci);
    endQuickIndexItem(ol);
    first=FALSE;
  }
  endQuickIndexList(ol);
}

//----------------------------------------------------------------------------

/** Helper class representing a class member in the navigation menu. */
struct CmhlInfo
{
  CmhlInfo(const char *fn,const QCString &t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const CmhlInfo *getCmhlInfo(size_t hl)
{
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  static CmhlInfo cmhlInfo[] =
  {
    CmhlInfo("functions",     theTranslator->trAll()),
    CmhlInfo("functions_func",
        fortranOpt ? theTranslator->trSubprograms()     :
        vhdlOpt    ? theTranslator->trFunctionAndProc() :
                     theTranslator->trFunctions()),
    CmhlInfo("functions_vars",theTranslator->trVariables()),
    CmhlInfo("functions_type",theTranslator->trTypedefs()),
    CmhlInfo("functions_enum",theTranslator->trEnumerations()),
    CmhlInfo("functions_eval",theTranslator->trEnumerationValues()),
    CmhlInfo("functions_prop",theTranslator->trProperties()),
    CmhlInfo("functions_evnt",theTranslator->trEvents()),
    CmhlInfo("functions_rela",theTranslator->trRelatedSymbols())
  };
  return &cmhlInfo[hl];
}

static void writeClassMemberIndexFiltered(OutputList &ol, ClassMemberHighlight::Enum hl)
{
  const auto &index = Index::instance();
  if (index.numDocumentedClassMembers(hl)==0) return;

  bool disableIndex     = Config_getBool(DISABLE_INDEX);

  bool multiPageIndex=FALSE;
  if (index.numDocumentedClassMembers(hl)>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassMembers);
  QCString title = lne ? lne->title() : theTranslator->trCompoundMembers();
  if (hl!=ClassMemberHighlight::All) title+=QCString(" - ")+getCmhlInfo(hl)->title;
  bool addToIndex = lne==nullptr || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getCmhlInfo(hl)->title,QCString(),
        getCmhlInfo(hl)->fname,QCString(),multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  bool first=TRUE;
  for (const auto &[letter,list] : index.isClassIndexLetterUsed(hl))
  {
    QCString fileName = getCmhlInfo(hl)->fname;
    if (multiPageIndex)
    {
      QCString cs(letter);
      if (!first)
      {
        fileName+="_"+letterToLabel(cs);
      }
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,cs,QCString(),fileName,QCString(),FALSE,TRUE);
      }
    }
    bool quickIndex = index.numDocumentedClassMembers(hl)>maxItemsBeforeQuickIndex;

    ol.startFile(fileName+extension,QCString(),title);
    ol.startQuickIndices();
    if (!disableIndex)
    {
      ol.writeQuickLinks(HighlightedItem::Functions,QCString());
      if (!Config_getBool(HTML_DYNAMIC_MENUS))
      {
        startQuickIndexList(ol);

        // index item for global member list
        startQuickIndexItem(ol,
            getCmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==ClassMemberHighlight::All,TRUE,first);
        ol.writeString(fixSpaces(getCmhlInfo(0)->title));
        endQuickIndexItem(ol);

        // index items per category member lists
        for (int i=1;i<ClassMemberHighlight::Total;i++)
        {
          if (index.numDocumentedClassMembers(static_cast<ClassMemberHighlight::Enum>(i))>0)
          {
            startQuickIndexItem(ol,getCmhlInfo(i)->fname+Doxygen::htmlFileExtension,hl==i,TRUE,first);
            ol.writeString(fixSpaces(getCmhlInfo(i)->title));
            //printf("multiPageIndex=%d first=%d fileName=%s file=%s title=%s\n",
            //    multiPageIndex,first,qPrint(fileName),getCmhlInfo(i)->fname,qPrint(getCmhlInfo(i)->title));
            endQuickIndexItem(ol);
          }
        }

        endQuickIndexList(ol);

        // quick alphabetical index
        if (quickIndex)
        {
          writeQuickMemberIndex(ol,index.isClassIndexLetterUsed(hl),letter,
              getCmhlInfo(hl)->fname,multiPageIndex);
        }
      }
    }
    ol.endQuickIndices();
    ol.writeSplitBar(fileName);
    ol.writeSearchInfo();

    ol.startContents();

    ol.startTextBlock();
    ol.parseText(hl == ClassMemberHighlight::All && lne ? lne->intro() : theTranslator->trCompoundMembersDescriptionTotal(hl));
    ol.endTextBlock();

    writeMemberList(ol,quickIndex,
        multiPageIndex ? letter : std::string(),
        index.isClassIndexLetterUsed(hl),
        Definition::TypeClass);
    endFile(ol);
    first=FALSE;
  }

  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();

  ol.popGeneratorState();
}

static void writeClassMemberIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassMembers);
  bool addToIndex = lne==nullptr || lne->visible();

  if (index.numDocumentedClassMembers(ClassMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,lne ? lne->title() : theTranslator->trCompoundMembers(),QCString(),"functions",QCString());
    Doxygen::indexList->incContentsDepth();
  }
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::All);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Functions);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Variables);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Typedefs);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Enums);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::EnumValues);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Properties);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Events);
  writeClassMemberIndexFiltered(ol,ClassMemberHighlight::Related);
  if (index.numDocumentedClassMembers(ClassMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

/** Helper class representing a file member in the navigation menu. */
struct FmhlInfo
{
  FmhlInfo(const char *fn,const QCString &t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const FmhlInfo *getFmhlInfo(size_t hl)
{
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  static FmhlInfo fmhlInfo[] =
  {
    FmhlInfo("globals",     theTranslator->trAll()),
    FmhlInfo("globals_func",
         fortranOpt ? theTranslator->trSubprograms()     :
         vhdlOpt    ? theTranslator->trFunctionAndProc() :
                      theTranslator->trFunctions()),
    FmhlInfo("globals_vars",sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables()),
    FmhlInfo("globals_type",theTranslator->trTypedefs()),
    FmhlInfo("globals_sequ",theTranslator->trSequences()),
    FmhlInfo("globals_dict",theTranslator->trDictionaries()),
    FmhlInfo("globals_enum",theTranslator->trEnumerations()),
    FmhlInfo("globals_eval",theTranslator->trEnumerationValues()),
    FmhlInfo("globals_defs",theTranslator->trDefines())
  };
  return &fmhlInfo[hl];
}

static void writeFileMemberIndexFiltered(OutputList &ol, FileMemberHighlight::Enum hl)
{
  const auto &index = Index::instance();
  if (index.numDocumentedFileMembers(hl)==0) return;

  bool disableIndex     = Config_getBool(DISABLE_INDEX);

  bool multiPageIndex=FALSE;
  if (Index::instance().numDocumentedFileMembers(hl)>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileGlobals);
  QCString title = lne ? lne->title() : theTranslator->trFileMembers();
  bool addToIndex = lne==nullptr || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getFmhlInfo(hl)->title,QCString(),
        getFmhlInfo(hl)->fname,QCString(),multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  bool first=TRUE;
  for (const auto &[letter,list] : index.isFileIndexLetterUsed(hl))
  {
    QCString fileName = getFmhlInfo(hl)->fname;
    if (multiPageIndex)
    {
      QCString cs(letter);
      if (!first)
      {
        fileName+="_"+letterToLabel(cs);
      }
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,cs,QCString(),fileName,QCString(),FALSE,TRUE);
      }
    }
    bool quickIndex = index.numDocumentedFileMembers(hl)>maxItemsBeforeQuickIndex;

    ol.startFile(fileName+extension,QCString(),title);
    ol.startQuickIndices();
    if (!disableIndex)
    {
      ol.writeQuickLinks(HighlightedItem::Globals,QCString());
      if (!Config_getBool(HTML_DYNAMIC_MENUS))
      {
        startQuickIndexList(ol);

        // index item for all file member lists
        startQuickIndexItem(ol,
            getFmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==FileMemberHighlight::All,TRUE,first);
        ol.writeString(fixSpaces(getFmhlInfo(0)->title));
        endQuickIndexItem(ol);

        // index items for per category member lists
        for (int i=1;i<FileMemberHighlight::Total;i++)
        {
          if (Index::instance().numDocumentedFileMembers(static_cast<FileMemberHighlight::Enum>(i))>0)
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
          writeQuickMemberIndex(ol,index.isFileIndexLetterUsed(hl),letter,
              getFmhlInfo(hl)->fname,multiPageIndex);
        }
      }
    }
    ol.endQuickIndices();
    ol.writeSplitBar(fileName);
    ol.writeSearchInfo();

    ol.startContents();

    ol.startTextBlock();
    ol.parseText(hl == FileMemberHighlight::All && lne ? lne->intro() : theTranslator->trFileMembersDescriptionTotal(hl));
    ol.endTextBlock();

    writeMemberList(ol,quickIndex,
        multiPageIndex ? letter : std::string(),
        index.isFileIndexLetterUsed(hl),
        Definition::TypeFile);
    endFile(ol);
    first=FALSE;
  }
  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  ol.popGeneratorState();
}

static void writeFileMemberIndex(OutputList &ol)
{
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::FileGlobals);
  bool addToIndex = lne==nullptr || lne->visible();
  if (Index::instance().numDocumentedFileMembers(FileMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(true,lne ? lne->title() : theTranslator->trFileMembers(),QCString(),"globals",QCString());
    Doxygen::indexList->incContentsDepth();
  }
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::All);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Functions);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Variables);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Typedefs);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Sequences);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Dictionaries);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Enums);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::EnumValues);
  writeFileMemberIndexFiltered(ol,FileMemberHighlight::Defines);
  if (Index::instance().numDocumentedFileMembers(FileMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

/** Helper class representing a namespace member in the navigation menu. */
struct NmhlInfo
{
  NmhlInfo(const char *fn,const QCString &t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const NmhlInfo *getNmhlInfo(size_t hl)
{
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  bool sliceOpt   = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  static NmhlInfo nmhlInfo[] =
  {
    NmhlInfo("namespacemembers",     theTranslator->trAll()),
    NmhlInfo("namespacemembers_func",
        fortranOpt ? theTranslator->trSubprograms()     :
        vhdlOpt    ? theTranslator->trFunctionAndProc() :
                     theTranslator->trFunctions()),
    NmhlInfo("namespacemembers_vars",sliceOpt ? theTranslator->trConstants() : theTranslator->trVariables()),
    NmhlInfo("namespacemembers_type",theTranslator->trTypedefs()),
    NmhlInfo("namespacemembers_sequ",theTranslator->trSequences()),
    NmhlInfo("namespacemembers_dict",theTranslator->trDictionaries()),
    NmhlInfo("namespacemembers_enum",theTranslator->trEnumerations()),
    NmhlInfo("namespacemembers_eval",theTranslator->trEnumerationValues())
  };
  return &nmhlInfo[hl];
}

//----------------------------------------------------------------------------

static void writeNamespaceMemberIndexFiltered(OutputList &ol,
                                        NamespaceMemberHighlight::Enum hl)
{
  const auto &index = Index::instance();
  if (index.numDocumentedNamespaceMembers(hl)==0) return;

  bool disableIndex     = Config_getBool(DISABLE_INDEX);


  bool multiPageIndex=FALSE;
  if (index.numDocumentedNamespaceMembers(hl)>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceMembers);
  QCString title = lne ? lne->title() : theTranslator->trNamespaceMembers();
  bool addToIndex = lne==nullptr || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getNmhlInfo(hl)->title,QCString(),
        getNmhlInfo(hl)->fname,QCString(),multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  bool first=TRUE;
  for (const auto &[letter,list] : index.isNamespaceIndexLetterUsed(hl))
  {
    QCString fileName = getNmhlInfo(hl)->fname;
    if (multiPageIndex)
    {
      QCString cs(letter);
      if (!first)
      {
        fileName+="_"+letterToLabel(cs);
      }
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,cs,QCString(),fileName,QCString(),FALSE,TRUE);
      }
    }
    bool quickIndex = index.numDocumentedNamespaceMembers(hl)>maxItemsBeforeQuickIndex;

    ol.startFile(fileName+extension,QCString(),title);
    ol.startQuickIndices();
    if (!disableIndex)
    {
      ol.writeQuickLinks(HighlightedItem::NamespaceMembers,QCString());
      if (!Config_getBool(HTML_DYNAMIC_MENUS))
      {
        startQuickIndexList(ol);

        // index item for all namespace member lists
        startQuickIndexItem(ol,
            getNmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==NamespaceMemberHighlight::All,TRUE,first);
        ol.writeString(fixSpaces(getNmhlInfo(0)->title));
        endQuickIndexItem(ol);

        // index items per category member lists
        for (int i=1;i<NamespaceMemberHighlight::Total;i++)
        {
          if (index.numDocumentedNamespaceMembers(static_cast<NamespaceMemberHighlight::Enum>(i))>0)
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
          writeQuickMemberIndex(ol,index.isNamespaceIndexLetterUsed(hl),letter,
              getNmhlInfo(hl)->fname,multiPageIndex);
        }
      }
    }
    ol.endQuickIndices();
    ol.writeSplitBar(fileName);
    ol.writeSearchInfo();

    ol.startContents();

    ol.startTextBlock();
    ol.parseText(hl == NamespaceMemberHighlight::All && lne ? lne->intro() : theTranslator->trNamespaceMembersDescriptionTotal(hl));
    ol.endTextBlock();

    writeMemberList(ol,quickIndex,
        multiPageIndex ? letter : std::string(),
        index.isNamespaceIndexLetterUsed(hl),
        Definition::TypeNamespace);
    endFile(ol);
    first=FALSE;
  }
  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  ol.popGeneratorState();
}

static void writeNamespaceMemberIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::NamespaceMembers);
  bool addToIndex = lne==nullptr || lne->visible();
  if (index.numDocumentedNamespaceMembers(NamespaceMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(true,lne ? lne->title() : theTranslator->trNamespaceMembers(),QCString(),"namespacemembers",QCString());
    Doxygen::indexList->incContentsDepth();
  }
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::All);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Functions);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Variables);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Typedefs);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Sequences);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Dictionaries);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::Enums);
  writeNamespaceMemberIndexFiltered(ol,NamespaceMemberHighlight::EnumValues);
  if (index.numDocumentedNamespaceMembers(NamespaceMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }

}

//----------------------------------------------------------------------------

/** Helper class representing a module member in the navigation menu. */
struct MmhlInfo
{
  MmhlInfo(const char *fn,const QCString &t) : fname(fn), title(t) {}
  const char *fname;
  QCString title;
};

static const MmhlInfo *getMmhlInfo(size_t hl)
{
  static MmhlInfo nmhlInfo[] =
  {
    MmhlInfo("modulemembers",     theTranslator->trAll()),
    MmhlInfo("modulemembers_func",theTranslator->trFunctions()),
    MmhlInfo("modulemembers_vars",theTranslator->trVariables()),
    MmhlInfo("modulemembers_type",theTranslator->trTypedefs()),
    MmhlInfo("modulemembers_enum",theTranslator->trEnumerations()),
    MmhlInfo("modulemembers_eval",theTranslator->trEnumerationValues())
  };
  return &nmhlInfo[hl];
}

//----------------------------------------------------------------------------

static void writeModuleMemberIndexFiltered(OutputList &ol,
                                        ModuleMemberHighlight::Enum hl)
{
  const auto &index = Index::instance();
  if (index.numDocumentedModuleMembers(hl)==0) return;

  bool disableIndex     = Config_getBool(DISABLE_INDEX);


  bool multiPageIndex=FALSE;
  if (index.numDocumentedModuleMembers(hl)>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
  {
    multiPageIndex=TRUE;
  }

  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString extension=Doxygen::htmlFileExtension;
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ModuleMembers);
  QCString title = lne ? lne->title() : theTranslator->trModulesMembers();
  bool addToIndex = lne==nullptr || lne->visible();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(multiPageIndex,getMmhlInfo(hl)->title,QCString(),
        getMmhlInfo(hl)->fname,QCString(),multiPageIndex,TRUE);
    if (multiPageIndex) Doxygen::indexList->incContentsDepth();
  }

  bool first=TRUE;
  for (const auto &[letter,list] : index.isModuleIndexLetterUsed(hl))
  {
    QCString fileName = getMmhlInfo(hl)->fname;
    if (multiPageIndex)
    {
      QCString cs(letter);
      if (!first)
      {
        fileName+="_"+letterToLabel(cs);
      }
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,cs,QCString(),fileName,QCString(),FALSE,TRUE);
      }
    }
    bool quickIndex = index.numDocumentedModuleMembers(hl)>maxItemsBeforeQuickIndex;

    ol.startFile(fileName+extension,QCString(),title);
    ol.startQuickIndices();
    if (!disableIndex)
    {
      ol.writeQuickLinks(HighlightedItem::ModuleMembers,QCString());
      if (!Config_getBool(HTML_DYNAMIC_MENUS))
      {
        startQuickIndexList(ol);

        // index item for all namespace member lists
        startQuickIndexItem(ol,
            getMmhlInfo(0)->fname+Doxygen::htmlFileExtension,hl==ModuleMemberHighlight::All,TRUE,first);
        ol.writeString(fixSpaces(getMmhlInfo(0)->title));
        endQuickIndexItem(ol);

        // index items per category member lists
        for (int i=1;i<ModuleMemberHighlight::Total;i++)
        {
          if (index.numDocumentedModuleMembers(static_cast<ModuleMemberHighlight::Enum>(i))>0)
          {
            startQuickIndexItem(ol,
                getMmhlInfo(i)->fname+Doxygen::htmlFileExtension,hl==i,TRUE,first);
            ol.writeString(fixSpaces(getMmhlInfo(i)->title));
            endQuickIndexItem(ol);
          }
        }

        endQuickIndexList(ol);

        if (quickIndex)
        {
          writeQuickMemberIndex(ol,index.isModuleIndexLetterUsed(hl),letter,
              getMmhlInfo(hl)->fname,multiPageIndex);
        }
      }
    }
    ol.endQuickIndices();
    ol.writeSplitBar(fileName);
    ol.writeSearchInfo();

    ol.startContents();

    ol.startTextBlock();
    ol.parseText(hl == ModuleMemberHighlight::All && lne ? lne->intro() : theTranslator->trModuleMembersDescriptionTotal(hl));
    ol.endTextBlock();

    writeMemberList(ol,quickIndex,
        multiPageIndex ? letter : std::string(),
        index.isModuleIndexLetterUsed(hl),
        Definition::TypeModule);
    endFile(ol);
    first=FALSE;
  }
  if (multiPageIndex && addToIndex) Doxygen::indexList->decContentsDepth();
  ol.popGeneratorState();
}


//----------------------------------------------------------------------------

static void writeModuleMemberIndex(OutputList &ol)
{
  const auto &index = Index::instance();
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ModuleMembers);
  bool addToIndex = lne==nullptr || lne->visible();
  if (index.numDocumentedModuleMembers(ModuleMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->addContentsItem(true,lne ? lne->title() : theTranslator->trModulesMembers(),QCString(),"modulemembers",QCString());
    Doxygen::indexList->incContentsDepth();
  }
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::All);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::Functions);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::Variables);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::Typedefs);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::Enums);
  writeModuleMemberIndexFiltered(ol,ModuleMemberHighlight::EnumValues);
  if (index.numDocumentedModuleMembers(ModuleMemberHighlight::All)>0 && addToIndex)
  {
    Doxygen::indexList->decContentsDepth();
  }
}

//----------------------------------------------------------------------------

static void writeExampleIndex(OutputList &ol)
{
  if (Doxygen::exampleLinkedMap->empty()) return;
  ol.pushGeneratorState();
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Examples);
  QCString title = lne ? lne->title() : theTranslator->trExamples();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"examples",QCString(),title,HighlightedItem::Examples);

  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());

  ol.startContents();

  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"examples",QCString(),TRUE,TRUE);
    Doxygen::indexList->incContentsDepth();
  }

  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trExamplesDescription());
  ol.endTextBlock();

  ol.startItemList();
  for (const auto &pd : *Doxygen::exampleLinkedMap)
  {
    ol.startItemListItem();
    QCString n=pd->getOutputFileBase();
    if (!pd->title().isEmpty())
    {
      ol.writeObjectLink(QCString(),n,QCString(),pd->title());
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,filterTitle(pd->title()),pd->getReference(),n,QCString(),FALSE,TRUE);
      }
    }
    else
    {
      ol.writeObjectLink(QCString(),n,QCString(),pd->name());
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(FALSE,pd->name(),pd->getReference(),n,QCString(),FALSE,TRUE);
      }
    }
    ol.endItemListItem();
    //ol.writeString("\n");
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

static void countRelatedPages(int &docPages,int &indexPages)
{
  docPages=indexPages=0;
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->visibleInIndex() && !pd->hasParentPage())
    {
      indexPages++;
    }
    if (pd->documentedPage())
    {
      docPages++;
    }
  }
}

//----------------------------------------------------------------------------

static bool mainPageHasOwnTitle()
{
  QCString projectName = Config_getString(PROJECT_NAME);
  QCString title;
  if (Doxygen::mainPage)
  {
    title = filterTitle(Doxygen::mainPage->title());
  }
  return !projectName.isEmpty() && mainPageHasTitle() && qstricmp(title,projectName)!=0;
}

static void writePages(PageDef *pd,FTVHelp *ftv)
{
  //printf("writePages()=%s pd=%p mainpage=%p\n",qPrint(pd->name()),(void*)pd,(void*)Doxygen::mainPage.get());
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Pages);
  bool addToIndex = lne==nullptr || lne->visible();
  if (!addToIndex) return;

  bool hasSubPages = pd->hasSubPages();
  bool hasSections = pd->hasSections();

  if (pd->visibleInIndex())
  {
    QCString pageTitle;

    if (pd->title().isEmpty())
      pageTitle=pd->name();
    else
      pageTitle=filterTitle(pd->title());

    if (ftv)
    {
      //printf("*** adding %s hasSubPages=%d hasSections=%d\n",qPrint(pageTitle),hasSubPages,hasSections);
      ftv->addContentsItem(
          hasSubPages,pageTitle,
          pd->getReference(),pd->getOutputFileBase(),
          QCString(),hasSubPages,TRUE,pd);
    }
    if (addToIndex && pd!=Doxygen::mainPage.get())
    {
      Doxygen::indexList->addContentsItem(
          hasSubPages || hasSections,pageTitle,
          pd->getReference(),pd->getOutputFileBase(),
          QCString(),hasSubPages,TRUE,pd);
    }
  }
  if (hasSubPages && ftv) ftv->incContentsDepth();
  bool doIndent = (hasSections || hasSubPages) &&
                  (pd!=Doxygen::mainPage.get() || mainPageHasOwnTitle());
  if (doIndent)
  {
    Doxygen::indexList->incContentsDepth();
  }
  if (hasSections)
  {
    pd->addSectionsToIndex();
  }
  for (const auto &subPage : pd->getSubPages())
  {
    writePages(subPage,ftv);
  }
  if (hasSubPages && ftv) ftv->decContentsDepth();
  if (doIndent)
  {
    Doxygen::indexList->decContentsDepth();
  }
  //printf("end writePages()=%s\n",qPrint(pd->title()));
}

//----------------------------------------------------------------------------

static void writePageIndex(OutputList &ol)
{
  if (Index::instance().numIndexedPages()==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Pages);
  QCString title = lne ? lne->title() : theTranslator->trRelatedPages();
  startFile(ol,"pages",QCString(),title,HighlightedItem::Pages);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trRelatedPagesDescription());
  ol.endTextBlock();

  {
    FTVHelp ftv(false);
    for (const auto &pd : *Doxygen::pageLinkedMap)
    {
      if ((pd->getOuterScope()==nullptr ||
          pd->getOuterScope()->definitionType()!=Definition::TypePage) && // not a sub page
          pd->visibleInIndex()
         )
      {
        writePages(pd.get(),&ftv);
      }
    }
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
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
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (!gd->isReference())
    {
      //gd->visited=FALSE;
      count++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------

static int countDirs()
{
  int count=0;
  for (const auto &dd : *Doxygen::dirLinkedMap)
  {
    if (dd->isLinkableInProject())
    {
      count++;
    }
  }
  return count;
}


//----------------------------------------------------------------------------

void writeGraphInfo(OutputList &ol)
{
  if (!Config_getBool(HAVE_DOT) || !Config_getBool(GENERATE_HTML)) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  DotLegendGraph gd;
  gd.writeGraph(Config_getString(HTML_OUTPUT));

  bool oldStripCommentsState = Config_getBool(STRIP_CODE_COMMENTS);
  bool oldCreateSubdirs = Config_getBool(CREATE_SUBDIRS);
  // temporarily disable the stripping of comments for our own code example!
  Config_updateBool(STRIP_CODE_COMMENTS,FALSE);
  // temporarily disable create subdirs for linking to our example
  Config_updateBool(CREATE_SUBDIRS,FALSE);

  startFile(ol,"graph_legend",QCString(),theTranslator->trLegendTitle());
  startTitle(ol,QCString());
  ol.parseText(theTranslator->trLegendTitle());
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  QCString legendDocs = theTranslator->trLegendDocs();
  int s = legendDocs.find("<center>");
  int e = legendDocs.find("</center>");
  QCString imgExt = getDotImageExtension();
  if (imgExt=="svg" && s!=-1 && e!=-1)
  {
    legendDocs = legendDocs.left(s+8) + "[!-- " + "SVG 0 --]" + legendDocs.mid(e);
    //printf("legendDocs=%s\n",qPrint(legendDocs));
  }

  {
    auto fd = createFileDef("","graph_legend.dox");
    ol.generateDoc("graph_legend",1,fd.get(),nullptr,legendDocs,FALSE,FALSE,
        QCString(),FALSE,FALSE,FALSE);
  }

  // restore config settings
  Config_updateBool(STRIP_CODE_COMMENTS,oldStripCommentsState);
  Config_updateBool(CREATE_SUBDIRS,oldCreateSubdirs);

  endFile(ol);
  ol.popGeneratorState();
}



//----------------------------------------------------------------------------
/*!
 * write groups as hierarchical trees
 */
static void writeGroupTreeNode(OutputList &ol, const GroupDef *gd, int level, FTVHelp* ftv, bool addToIndex)
{
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  //bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  if (level>20)
  {
    warn(gd->getDefFileName(),gd->getDefLine(),
        "maximum nesting level exceeded for group %s: check for possible recursive group relation!",qPrint(gd->name())
        );
    return;
  }

  /* Some groups should appear twice under different parent-groups.
   * That is why we should not check if it was visited
   */
  if ((!gd->isASubGroup() || level>0) && gd->isVisible() && gd->isVisibleInHierarchy())
  {
    //printf("gd->name()=%s #members=%d\n",qPrint(gd->name()),gd->countMembers());
    // write group info
    bool hasSubGroups = !gd->getSubGroups().empty();
    bool hasSubPages  = !gd->getPages().empty();
    size_t numSubItems = 0;
    for (const auto &ml : gd->getMemberLists())
    {
      if (ml->listType().isDocumentation())
      {
        numSubItems += ml->size();
      }
    }
    numSubItems += gd->getNamespaces().size();
    numSubItems += gd->getClasses().size();
    numSubItems += gd->getFiles().size();
    numSubItems += gd->getConcepts().size();
    numSubItems += gd->getDirs().size();
    numSubItems += gd->getPages().size();

    bool isDir = hasSubGroups || hasSubPages || numSubItems>0;
    //printf("gd='%s': pageDict=%d\n",qPrint(gd->name()),gd->pageDict->count());
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(isDir,gd->groupTitle(),gd->getReference(),gd->getOutputFileBase(),QCString(),isDir,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    if (ftv)
    {
      ftv->addContentsItem(hasSubGroups,gd->groupTitle(),
                           gd->getReference(),gd->getOutputFileBase(),QCString(),
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

    for (const auto &lde : LayoutDocManager::instance().docEntries(LayoutDocManager::Group))
    {
      if (lde->kind()==LayoutDocEntry::MemberDef && addToIndex)
      {
        const LayoutDocEntryMemberDef *lmd = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get());
        if (lmd)
        {
          MemberList *ml = gd->getMemberList(lmd->type);
          if (ml)
          {
            for (const auto &md : *ml)
            {
              const MemberVector &enumList = md->enumFieldList();
              isDir = !enumList.empty() && md->isEnumerate();
              if (md->isVisible() && !md->isAnonymous())
              {
                Doxygen::indexList->addContentsItem(isDir,
                    md->qualifiedName(),md->getReference(),
                    md->getOutputFileBase(),md->anchor(),FALSE,addToIndex);
              }
              if (isDir)
              {
                Doxygen::indexList->incContentsDepth();
                for (const auto &emd : enumList)
                {
                  if (emd->isVisible())
                  {
                    Doxygen::indexList->addContentsItem(FALSE,
                        emd->qualifiedName(),emd->getReference(),emd->getOutputFileBase(),
                        emd->anchor(),FALSE,addToIndex);
                  }
                }
                Doxygen::indexList->decContentsDepth();
              }
            }
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupClasses && addToIndex)
      {
        for (const auto &cd : gd->getClasses())
        {
          //bool nestedClassInSameGroup =
          //    cd->getOuterScope() && cd->getOuterScope()->definitionType()==Definition::TypeClass &&
          //    cd->getOuterScope()->partOfGroups().empty() && cd->getOuterScope()->partOfGroups()->contains(gd);
          //printf("===== GroupClasses: %s visible=%d nestedClassInSameGroup=%d\n",qPrint(cd->name()),cd->isVisible(),nestedClassInSameGroup);
          if (cd->isVisible() /*&& !nestedClassInSameGroup*/)
          {
            addMembersToIndex(cd,
                              LayoutDocManager::Class,
                              cd->displayName(),
                              cd->anchor(),
                              addToIndex,
                              TRUE);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupNamespaces && addToIndex && Config_getBool(SHOW_NAMESPACES))
      {
        for (const auto &nd : gd->getNamespaces())
        {
          if (nd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                nd->displayName(),nd->getReference(),
                nd->getOutputFileBase(),QCString(),FALSE,Config_getBool(SHOW_NAMESPACES));
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupConcepts && addToIndex)
      {
        for (const auto &cd : gd->getConcepts())
        {
          if (cd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                cd->displayName(),cd->getReference(),
                cd->getOutputFileBase(),QCString(),FALSE,addToIndex);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupFiles && addToIndex)
      {
        for (const auto &fd : gd->getFiles())
        {
          if (fd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                fd->displayName(),fd->getReference(),
                fd->getOutputFileBase(),QCString(),FALSE,fd->isLinkableViaGroup());
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupDirs && addToIndex)
      {
        for (const auto &dd : gd->getDirs())
        {
          if (dd->isVisible())
          {
            Doxygen::indexList->addContentsItem(FALSE,
                dd->shortName(),dd->getReference(),
                dd->getOutputFileBase(),QCString(),FALSE,FALSE);
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupPageDocs && addToIndex)
      {
        for (const auto &pd : gd->getPages())
        {
          const SectionInfo *si=nullptr;
          if (!pd->name().isEmpty()) si=SectionManager::instance().find(pd->name());
          hasSubPages = pd->hasSubPages();
          bool hasSections = pd->hasSections();
          Doxygen::indexList->addContentsItem(
              hasSubPages || hasSections,
              pd->title(),
              gd->getReference(),
              gd->getOutputFileBase(),
              si ? si->label() : QCString(),
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
          writePages(pd,nullptr);
          if (hasSections || hasSubPages)
          {
            Doxygen::indexList->decContentsDepth();
          }
        }
      }
      else if (lde->kind()==LayoutDocEntry::GroupNestedGroups)
      {
        if (!gd->getSubGroups().empty())
        {
          startIndexHierarchy(ol,level+1);
          for (const auto &subgd : gd->getSubGroups())
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
    ol.disable(OutputType::Html);
  }
  startIndexHierarchy(ol,0);
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (gd->isVisibleInHierarchy())
    {
      writeGroupTreeNode(ol,gd.get(),0,ftv,addToIndex);
    }
  }
  endIndexHierarchy(ol,0);
  if (ftv)
  {
    ol.popGeneratorState();
  }
}

//----------------------------------------------------------------------------

static void writeTopicIndex(OutputList &ol)
{
  if (Index::instance().numDocumentedGroups()==0) return;
  ol.pushGeneratorState();
  // 1.{
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Topics);
  QCString title = lne ? lne->title() : theTranslator->trTopics();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"topics",QCString(),title,HighlightedItem::Topics);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trTopicListDescription());
  ol.endTextBlock();

  // ---------------
  // Normal group index for Latex/RTF
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeGroupHierarchy(ol,nullptr,FALSE);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  // 2.}

  // ---------------
  // interactive group index for HTML
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"topics",QCString(),TRUE,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp ftv(false);
    writeGroupHierarchy(ol,&ftv,addToIndex);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.disableAllBut(OutputType::Html);
    ol.writeString(t.str().c_str());
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

static void writeModuleTreeNode(OutputList &ol, const ModuleDef *mod,
                                FTVHelp* ftv, bool addToIndex)
{
  int visibleMembers = mod->countVisibleMembers();
  bool isDir=visibleMembers>0;
  if (addToIndex)
  {
    Doxygen::indexList->addContentsItem(isDir,mod->name(),
                              mod->getReference(),mod->getOutputFileBase(),QCString(),isDir,TRUE);
  }
  if (ftv)
  {
    ftv->addContentsItem(false,mod->name(),
                         mod->getReference(),mod->getOutputFileBase(),QCString(),
                         false,false,mod);
  }
  ol.startIndexListItem();
  ol.startIndexItem(mod->getReference(),mod->getOutputFileBase());
  ol.parseText(mod->qualifiedName());
  ol.endIndexItem(mod->getReference(),mod->getOutputFileBase());
  if (mod->isReference())
  {
    ol.startTypewriter();
    ol.docify(" [external]");
    ol.endTypewriter();
  }
  if (addToIndex && isDir)
  {
     Doxygen::indexList->incContentsDepth();
  }
  if (isDir)
  {
    //ftv->incContentsDepth();
    writeClassTree(mod->getClasses(),nullptr,addToIndex,FALSE,ClassDef::Class);
    writeConceptList(mod->getConcepts(),nullptr,addToIndex);
    writeModuleMembers(mod,addToIndex);
    //ftv->decContentsDepth();
  }
  if (addToIndex && isDir)
  {
     Doxygen::indexList->decContentsDepth();
  }
  ol.endIndexListItem();
}

//----------------------------------------------------------------------------

static void writeModuleList(OutputList &ol, FTVHelp *ftv,bool addToIndex)
{
  if (ftv)
  {
    ol.pushGeneratorState();
    ol.disable(OutputType::Html);
  }
  startIndexHierarchy(ol,0);
  for (const auto &mod : ModuleManager::instance().modules())
  {
    if (mod->isPrimaryInterface())
    {
      writeModuleTreeNode(ol,mod.get(),ftv,addToIndex);
    }
  }
  endIndexHierarchy(ol,0);
  if (ftv)
  {
    ol.popGeneratorState();
  }
}

//----------------------------------------------------------------------------

static void writeModuleIndex(OutputList &ol)
{
  if (ModuleManager::instance().numDocumentedModules()==0) return;
  ol.pushGeneratorState();
  // 1.{

  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ModuleList);
  if (lne==nullptr) lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Modules); // fall back
  QCString title = lne ? lne->title() : theTranslator->trModules();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"modules",QCString(),title,HighlightedItem::Modules);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trModulesListDescription(Config_getBool(EXTRACT_ALL)));
  ol.endTextBlock();

  // ---------------
  // Normal group index for Latex/RTF
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);
  Doxygen::indexList->disable();

  writeModuleList(ol,nullptr,FALSE);

  Doxygen::indexList->enable();
  ol.popGeneratorState();
  // 2.}

  // ---------------
  // interactive group index for HTML
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"modules",QCString(),TRUE,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp ftv(false);
    writeModuleList(ol,&ftv,addToIndex);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
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

static void writeConceptList(const ConceptLinkedRefMap &concepts, FTVHelp *ftv,bool addToIndex)
{
  for (const auto &cd : concepts)
  {
    if (cd->isLinkableInProject())
    {
      if (ftv)
      {
        ftv->addContentsItem(false,cd->displayName(FALSE),cd->getReference(),
                             cd->getOutputFileBase(),QCString(),false,cd->partOfGroups().empty(),cd);
      }
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(false,cd->displayName(FALSE),cd->getReference(),
            cd->getOutputFileBase(),QCString(),false,cd->partOfGroups().empty());
      }
    }
  }
}

static void writeConceptTreeInsideNamespaceElement(const NamespaceDef *nd,FTVHelp *ftv,
                                            bool rootOnly, bool addToIndex);

static void writeConceptTreeInsideNamespace(const NamespaceLinkedRefMap &nsLinkedMap,FTVHelp *ftv,
                                            bool rootOnly, bool addToIndex)
{
  for (const auto &nd : nsLinkedMap)
  {
    writeConceptTreeInsideNamespaceElement(nd,ftv,rootOnly,addToIndex);
  }
}


static void writeConceptTreeInsideNamespaceElement(const NamespaceDef *nd,FTVHelp *ftv,
                                            bool rootOnly, bool addToIndex)
{
  if (!nd->isAnonymous() &&
      (!rootOnly || nd->getOuterScope()==Doxygen::globalScope))
  {
    bool isDir = namespaceHasNestedConcept(nd);
    bool isLinkable  = nd->isLinkableInProject();

    //printf("writeConceptTreeInsideNamespaceElement namespace %s isLinkable=%d isDir=%d\n",qPrint(nd->name()),isLinkable,isDir);

    QCString ref;
    QCString file;
    if (isLinkable)
    {
      ref  = nd->getReference();
      file = nd->getOutputFileBase();
    }

    if (isDir)
    {
      ftv->addContentsItem(isDir,nd->localName(),ref,file,QCString(),FALSE,TRUE,nd);

      if (addToIndex)
      {
        // the namespace entry is already shown under the namespace list so don't
        // add it to the nav index and don't create a separate index file for it otherwise
        // it will overwrite the one written for the namespace list.
        Doxygen::indexList->addContentsItem(isDir,nd->localName(),ref,file,QCString(),
            false, // separateIndex
            false  // addToNavIndex
            );
      }
      if (addToIndex)
      {
        Doxygen::indexList->incContentsDepth();
      }

      ftv->incContentsDepth();
      writeConceptTreeInsideNamespace(nd->getNamespaces(),ftv,FALSE,addToIndex);
      writeConceptList(nd->getConcepts(),ftv,addToIndex);
      ftv->decContentsDepth();

      if (addToIndex)
      {
        Doxygen::indexList->decContentsDepth();
      }
    }
  }
}

static void writeConceptRootList(FTVHelp *ftv,bool addToIndex)
{
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if ((cd->getOuterScope()==nullptr ||
        cd->getOuterScope()==Doxygen::globalScope) && cd->isLinkableInProject()
       )
    {
      //printf("*** adding %s hasSubPages=%d hasSections=%d\n",qPrint(pageTitle),hasSubPages,hasSections);
      ftv->addContentsItem(
          false,cd->localName(),cd->getReference(),cd->getOutputFileBase(),
          QCString(),false,cd->partOfGroups().empty(),cd.get());
      if (addToIndex)
      {
        Doxygen::indexList->addContentsItem(
            false,cd->localName(),cd->getReference(),cd->getOutputFileBase(),
            QCString(),false,cd->partOfGroups().empty(),cd.get());
      }
    }
  }
}

static void writeConceptIndex(OutputList &ol)
{
  if (Index::instance().numDocumentedConcepts()==0) return;
  ol.pushGeneratorState();
  // 1.{
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Concepts);
  QCString title = lne ? lne->title() : theTranslator->trConceptList();
  bool addToIndex = lne==nullptr || lne->visible();

  startFile(ol,"concepts",QCString(),title,HighlightedItem::Concepts);
  startTitle(ol,QCString());
  ol.parseText(title);
  endTitle(ol,QCString(),QCString());
  ol.startContents();
  ol.startTextBlock();
  ol.parseText(lne ? lne->intro() : theTranslator->trConceptListDescription(Config_getBool(EXTRACT_ALL)));
  ol.endTextBlock();

  // ---------------
  // Normal group index for Latex/RTF
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disable(OutputType::Html);

  bool first=TRUE;
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if (cd->isLinkableInProject())
    {
      if (first)
      {
        ol.startIndexList();
        first=FALSE;
      }
      //ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      ol.startIndexKey();
      ol.writeObjectLink(QCString(),cd->getOutputFileBase(),QCString(),cd->displayName());
      ol.endIndexKey();

      bool hasBrief = !cd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        //ol.docify(" (");
        ol.generateDoc(
                 cd->briefFile(),cd->briefLine(),
                 cd.get(),nullptr,
                 cd->briefDescription(TRUE),
                 FALSE, // index words
                 FALSE, // isExample
                 QCString(),     // example name
                 TRUE,  // single line
                 TRUE,  // link from index
                 Config_getBool(MARKDOWN_SUPPORT)
                );
        //ol.docify(")");
      }
      ol.endIndexValue(cd->getOutputFileBase(),hasBrief);

    }
  }
  if (!first) ol.endIndexList();

  ol.popGeneratorState();
  // 2.}

  // ---------------
  // interactive group index for HTML
  // ---------------
  // 2.{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  {
    if (addToIndex)
    {
      Doxygen::indexList->addContentsItem(TRUE,title,QCString(),"concepts",QCString(),TRUE,TRUE);
      Doxygen::indexList->incContentsDepth();
    }
    FTVHelp ftv(false);
    for (const auto &nd : *Doxygen::namespaceLinkedMap)
    {
      writeConceptTreeInsideNamespaceElement(nd.get(),&ftv,true,addToIndex);
    }
    writeConceptRootList(&ftv,addToIndex);
    TextStream t;
    ftv.generateTreeViewInline(t);
    ol.writeString(t.str().c_str());
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

static void writeUserGroupStubPage(OutputList &ol,LayoutNavEntry *lne)
{
  if (lne->baseFile().startsWith("usergroup"))
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    startFile(ol,lne->baseFile(),QCString(),lne->title(),HighlightedItem::UserGroup);
    startTitle(ol,QCString());
    ol.parseText(lne->title());
    endTitle(ol,QCString(),QCString());
    ol.startContents();
    int count=0;
    for (const auto &entry: lne->children())
    {
      if (entry->visible()) count++;
    }
    if (count>0)
    {
      ol.writeString("<ul>\n");
      for (const auto &entry: lne->children())
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
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString projectName = Config_getString(PROJECT_NAME);
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
  ol.disableAllBut(OutputType::Html);

  QCString defFileName =
    Doxygen::mainPage ? Doxygen::mainPage->docFile() : QCString("[generated]");
  int defLine =
    Doxygen::mainPage ? Doxygen::mainPage->docLine() : -1;

  QCString title;
  if (!mainPageHasTitle())
  {
    title = theTranslator->trMainPage();
  }
  else if (Doxygen::mainPage)
  {
    title = filterTitle(Doxygen::mainPage->title());
  }

  QCString indexName="index";
  ol.startFile(indexName,QCString(),title);

  if (Doxygen::mainPage)
  {
    bool hasSubs = Doxygen::mainPage->hasSubPages() || Doxygen::mainPage->hasSections();
    bool hasTitle = !projectName.isEmpty() && mainPageHasTitle() && qstricmp(title,projectName)!=0;
    //printf("** mainPage title=%s hasTitle=%d hasSubs=%d\n",qPrint(title),hasTitle,hasSubs);
    if (hasTitle) // to avoid duplicate entries in the treeview
    {
      Doxygen::indexList->addContentsItem(hasSubs,
                                          title,
                                          QCString(),
                                          indexName,
                                          QCString(),
                                          hasSubs,
                                          TRUE);
    }
    if (hasSubs)
    {
      writePages(Doxygen::mainPage.get(),nullptr);
    }
  }

  ol.startQuickIndices();
  if (!Config_getBool(DISABLE_INDEX))
  {
    ol.writeQuickLinks(HighlightedItem::Main,QCString());
  }
  ol.endQuickIndices();
  ol.writeSplitBar(indexName);
  ol.writeSearchInfo();
  bool headerWritten=FALSE;
  if (Doxygen::mainPage)
  {
    if (!Doxygen::mainPage->title().isEmpty())
    {
      if (Doxygen::mainPage->title().lower() != "notitle")
        ol.startPageDoc(Doxygen::mainPage->title());
      else
        ol.startPageDoc("");
    }
    else
      ol.startPageDoc(projectName);
  }
  if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty())
  {
    if (Doxygen::mainPage->title().lower()!="notitle")
    {
      ol.startHeaderSection();
      ol.startTitleHead(QCString());
      ol.generateDoc(Doxygen::mainPage->docFile(),Doxygen::mainPage->getStartBodyLine(),
                  Doxygen::mainPage.get(),nullptr,Doxygen::mainPage->title(),false,false,
                  QCString(),true,false,Config_getBool(MARKDOWN_SUPPORT));
      headerWritten = TRUE;
    }
  }
  else
  {
    if (!projectName.isEmpty())
    {
      ol.startHeaderSection();
      ol.startTitleHead(QCString());
      ol.parseText(theTranslator->trDocumentation(projectName));
      headerWritten = TRUE;
    }
  }
  if (headerWritten)
  {
    ol.endTitleHead(QCString(),QCString());
    ol.endHeaderSection();
  }

  ol.startContents();
  if (Config_getBool(DISABLE_INDEX) && Doxygen::mainPage==nullptr)
  {
    ol.writeQuickLinks(HighlightedItem::Main,QCString());
  }

  if (Doxygen::mainPage)
  {
    if (Doxygen::mainPage->localToc().isHtmlEnabled() && Doxygen::mainPage->hasSections())
    {
      Doxygen::mainPage->writeToc(ol,Doxygen::mainPage->localToc());
    }

    ol.startTextBlock();
    ol.generateDoc(defFileName,defLine,Doxygen::mainPage.get(),nullptr,
                Doxygen::mainPage->documentation(),true,false,
                QCString(),false,false,Config_getBool(MARKDOWN_SUPPORT));
    ol.endTextBlock();
    ol.endPageDoc();
  }

  QCString fn = Crawlmap::crawlFileName;
  addHtmlExtensionIfMissing(fn);
  ol.writeString("<a href=\"" + fn + "\"></a>\n");
  Doxygen::indexList->addIndexFile(fn);

  endFile(ol);

  ol.disable(OutputType::Html);

  //--------------------------------------------------------------------
  // write LaTeX/RTF index
  //--------------------------------------------------------------------
  ol.enable(OutputType::Latex);
  ol.enable(OutputType::Docbook);
  ol.enable(OutputType::RTF);

  if (Doxygen::mainPage)
  {
    msg("Generating main page...\n");
    Doxygen::mainPage->writeDocumentation(ol);
  }

  ol.startFile("refman",QCString(),QCString());
  ol.startIndexSection(IndexSection::isTitlePageStart);
  ol.disable(OutputType::Latex);
  ol.disable(OutputType::Docbook);

  if (projPrefix.isEmpty())
  {
    ol.parseText(theTranslator->trReferenceManual());
  }
  else
  {
    ol.parseText(projPrefix);
  }

  if (!Config_getString(PROJECT_NUMBER).isEmpty())
  {
    ol.startProjectNumber();
    ol.generateDoc(defFileName,defLine,Doxygen::mainPage.get(),nullptr,Config_getString(PROJECT_NUMBER),false,false,
                   QCString(),false,false,Config_getBool(MARKDOWN_SUPPORT));
    ol.endProjectNumber();
  }
  ol.endIndexSection(IndexSection::isTitlePageStart);
  ol.startIndexSection(IndexSection::isTitlePageAuthor);
  ol.parseText(theTranslator->trGeneratedBy());
  ol.endIndexSection(IndexSection::isTitlePageAuthor);
  ol.enable(OutputType::Latex);
  ol.enable(OutputType::Docbook);

  ol.lastIndexPage();
  if (Doxygen::mainPage)
  {
    ol.startIndexSection(IndexSection::isMainPage);
    ol.endIndexSection(IndexSection::isMainPage);
  }
  const auto &index = Index::instance();
  if (index.numDocumentedPages()>0)
  {
    ol.startIndexSection(IndexSection::isPageDocumentation);
    ol.endIndexSection(IndexSection::isPageDocumentation);
  }

  ol.disable(OutputType::Docbook);
  if (!Config_getBool(LATEX_HIDE_INDICES))
  {
    //if (indexedPages>0)
    //{
    //  ol.startIndexSection(isPageIndex);
    //  ol.parseText(/*projPrefix+*/ theTranslator->trPageIndex());
    //  ol.endIndexSection(isPageIndex);
    //}
    if (index.numDocumentedModules()>0)
    {
      ol.startIndexSection(IndexSection::isModuleIndex);
      ol.parseText(/*projPrefix+*/ theTranslator->trModuleIndex());
      ol.endIndexSection(IndexSection::isModuleIndex);
    }
    if (index.numDocumentedGroups()>0)
    {
      ol.startIndexSection(IndexSection::isTopicIndex);
      ol.parseText(/*projPrefix+*/ theTranslator->trTopicIndex());
      ol.endIndexSection(IndexSection::isTopicIndex);
    }
    if (Config_getBool(SHOW_NAMESPACES) && (index.numDocumentedNamespaces()>0))
    {
      LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Namespaces);
      if (lne)
      {
        ol.startIndexSection(IndexSection::isNamespaceIndex);
        ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trModulesIndex():theTranslator->trNamespaceIndex()));
        ol.endIndexSection(IndexSection::isNamespaceIndex);
      }
    }
    if (index.numDocumentedConcepts()>0)
    {
      ol.startIndexSection(IndexSection::isConceptIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trConceptIndex());
      ol.endIndexSection(IndexSection::isConceptIndex);
    }
    if (index.numHierarchyInterfaces()>0)
    {
      ol.startIndexSection(IndexSection::isClassHierarchyIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trHierarchicalIndex());
      ol.endIndexSection(IndexSection::isClassHierarchyIndex);
    }
    if (index.numHierarchyClasses()>0)
    {
      LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::ClassHierarchy);
      if (lne)
      {
        ol.startIndexSection(IndexSection::isClassHierarchyIndex);
        ol.parseText(/*projPrefix+*/
            (fortranOpt ? theTranslator->trCompoundIndexFortran() :
             vhdlOpt    ? theTranslator->trHierarchicalIndex()    :
                          theTranslator->trHierarchicalIndex()
            ));
        ol.endIndexSection(IndexSection::isClassHierarchyIndex);
      }
    }
    if (index.numHierarchyExceptions()>0)
    {
      ol.startIndexSection(IndexSection::isClassHierarchyIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trHierarchicalIndex());
      ol.endIndexSection(IndexSection::isClassHierarchyIndex);
    }
    if (index.numAnnotatedInterfacesPrinted()>0)
    {
      ol.startIndexSection(IndexSection::isCompoundIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trInterfaceIndex());
      ol.endIndexSection(IndexSection::isCompoundIndex);
    }
    if (index.numAnnotatedClassesPrinted()>0)
    {
      ol.startIndexSection(IndexSection::isCompoundIndex);
      ol.parseText(/*projPrefix+*/
          (fortranOpt ? theTranslator->trCompoundIndexFortran() :
              vhdlOpt ? theTranslator->trDesignUnitIndex()      :
                        theTranslator->trCompoundIndex()
          ));
      ol.endIndexSection(IndexSection::isCompoundIndex);
    }
    if (index.numAnnotatedStructsPrinted()>0)
    {
      ol.startIndexSection(IndexSection::isCompoundIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trStructIndex());
      ol.endIndexSection(IndexSection::isCompoundIndex);
    }
    if (index.numAnnotatedExceptionsPrinted()>0)
    {
      ol.startIndexSection(IndexSection::isCompoundIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trExceptionIndex());
      ol.endIndexSection(IndexSection::isCompoundIndex);
    }
    if (Config_getBool(SHOW_FILES) && index.numDocumentedFiles()>0)
    {
      ol.startIndexSection(IndexSection::isFileIndex);
      ol.parseText(/*projPrefix+*/theTranslator->trFileIndex());
      ol.endIndexSection(IndexSection::isFileIndex);
    }
  }
  ol.enable(OutputType::Docbook);

  if (index.numDocumentedModules()>0)
  {
    ol.startIndexSection(IndexSection::isModuleDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trModuleDocumentation());
    ol.endIndexSection(IndexSection::isModuleDocumentation);
  }
  if (index.numDocumentedGroups()>0)
  {
    ol.startIndexSection(IndexSection::isTopicDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trTopicDocumentation());
    ol.endIndexSection(IndexSection::isTopicDocumentation);
  }
  if (index.numDocumentedNamespaces()>0)
  {
    ol.startIndexSection(IndexSection::isNamespaceDocumentation);
    ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trModuleDocumentation():theTranslator->trNamespaceDocumentation()));
    ol.endIndexSection(IndexSection::isNamespaceDocumentation);
  }
  if (index.numDocumentedConcepts()>0)
  {
    ol.startIndexSection(IndexSection::isConceptDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trConceptDocumentation());
    ol.endIndexSection(IndexSection::isConceptDocumentation);
  }
  if (index.numAnnotatedInterfacesPrinted()>0)
  {
    ol.startIndexSection(IndexSection::isClassDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trInterfaceDocumentation());
    ol.endIndexSection(IndexSection::isClassDocumentation);
  }
  if (index.numAnnotatedClassesPrinted()>0)
  {
    ol.startIndexSection(IndexSection::isClassDocumentation);
    ol.parseText(/*projPrefix+*/(fortranOpt?theTranslator->trTypeDocumentation():theTranslator->trClassDocumentation()));
    ol.endIndexSection(IndexSection::isClassDocumentation);
  }
  if (index.numAnnotatedStructsPrinted()>0)
  {
    ol.startIndexSection(IndexSection::isClassDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trStructDocumentation());
    ol.endIndexSection(IndexSection::isClassDocumentation);
  }
  if (index.numAnnotatedExceptionsPrinted()>0)
  {
    ol.startIndexSection(IndexSection::isClassDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trExceptionDocumentation());
    ol.endIndexSection(IndexSection::isClassDocumentation);
  }
  if (Config_getBool(SHOW_FILES) && index.numDocumentedFiles()>0)
  {
    ol.startIndexSection(IndexSection::isFileDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trFileDocumentation());
    ol.endIndexSection(IndexSection::isFileDocumentation);
  }
  if (!Doxygen::exampleLinkedMap->empty())
  {
    ol.startIndexSection(IndexSection::isExampleDocumentation);
    ol.parseText(/*projPrefix+*/theTranslator->trExamples());
    ol.endIndexSection(IndexSection::isExampleDocumentation);
  }
  ol.endIndexSection(IndexSection::isEndIndex);
  endFile(ol);

  ol.popGeneratorState();
}

static std::vector<bool> indexWritten;

static void writeIndexHierarchyEntries(OutputList &ol,const LayoutNavEntryList &entries)
{
  auto isRef = [](const QCString &s)
  {
    return s.startsWith("@ref") || s.startsWith("\\ref");
  };
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  const auto &index = Index::instance();
  for (const auto &lne : entries)
  {
    LayoutNavEntry::Kind kind = lne->kind();
    size_t idx = static_cast<size_t>(kind);
    if (idx>=indexWritten.size())
    {
      size_t oldSize = indexWritten.size();
      size_t newSize = idx+1;
      indexWritten.resize(newSize);
      for (size_t i=oldSize; i<newSize; i++) indexWritten.at(i)=FALSE;
    }
    //printf("starting %s kind=%d\n",qPrint(lne->title()),lne->kind());
    bool addToIndex=lne->visible();
    bool needsClosing=FALSE;
    if (!indexWritten.at(idx))
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
        case LayoutNavEntry::Topics:
          msg("Generating topic index...\n");
          writeTopicIndex(ol);
          break;
        case LayoutNavEntry::Modules:
          {
            if (index.numDocumentedModules()>0 && addToIndex)
            {
              Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
              Doxygen::indexList->incContentsDepth();
              needsClosing=TRUE;
            }
          }
          break;
        case LayoutNavEntry::ModuleList:
          msg("Generating module index...\n");
          writeModuleIndex(ol);
          break;
        case LayoutNavEntry::ModuleMembers:
          msg("Generating module member index...\n");
          writeModuleMemberIndex(ol);
          break;
        case LayoutNavEntry::Namespaces:
          {
            bool showNamespaces = Config_getBool(SHOW_NAMESPACES);
            if (showNamespaces)
            {
              if (index.numDocumentedNamespaces()>0 && addToIndex)
              {
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
                Doxygen::indexList->incContentsDepth();
                needsClosing=TRUE;
              }
              if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Namespaces)!=lne.get()) // for backward compatibility with old layout file
              {
                msg("Generating namespace index...\n");
                writeNamespaceIndex(ol);
              }
            }
          }
          break;
        case LayoutNavEntry::NamespaceList:
          {
            bool showNamespaces = Config_getBool(SHOW_NAMESPACES);
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
          if (index.numAnnotatedClasses()>0 && addToIndex)
          {
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Classes)!=lne.get()) // for backward compatibility with old layout file
          {
            msg("Generating annotated compound index...\n");
            writeAnnotatedIndex(ol);
          }
          break;
        case LayoutNavEntry::Concepts:
          msg("Generating concept index...\n");
          writeConceptIndex(ol);
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
          if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
          {
            msg("Generating graphical class hierarchy...\n");
            writeGraphicalClassHierarchy(ol);
          }
          break;
        case LayoutNavEntry::ClassMembers:
          if (!sliceOpt)
          {
            msg("Generating member index...\n");
            writeClassMemberIndex(ol);
          }
          break;
        case LayoutNavEntry::Interfaces:
          if (sliceOpt && index.numAnnotatedInterfaces()>0 && addToIndex)
          {
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          break;
        case LayoutNavEntry::InterfaceList:
          if (sliceOpt)
          {
            msg("Generating annotated interface index...\n");
            writeAnnotatedInterfaceIndex(ol);
          }
          break;
        case LayoutNavEntry::InterfaceIndex:
          if (sliceOpt)
          {
            msg("Generating alphabetical interface index...\n");
            writeAlphabeticalInterfaceIndex(ol);
          }
          break;
        case LayoutNavEntry::InterfaceHierarchy:
          if (sliceOpt)
          {
            msg("Generating hierarchical interface index...\n");
            writeHierarchicalInterfaceIndex(ol);
            if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
            {
              msg("Generating graphical interface hierarchy...\n");
              writeGraphicalInterfaceHierarchy(ol);
            }
          }
          break;
        case LayoutNavEntry::Structs:
          if (sliceOpt && index.numAnnotatedStructs()>0 && addToIndex)
          {
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          break;
        case LayoutNavEntry::StructList:
          if (sliceOpt)
          {
            msg("Generating annotated struct index...\n");
            writeAnnotatedStructIndex(ol);
          }
          break;
        case LayoutNavEntry::StructIndex:
          if (sliceOpt)
          {
            msg("Generating alphabetical struct index...\n");
            writeAlphabeticalStructIndex(ol);
          }
          break;
        case LayoutNavEntry::Exceptions:
          if (sliceOpt && index.numAnnotatedExceptions()>0 && addToIndex)
          {
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          break;
        case LayoutNavEntry::ExceptionList:
          if (sliceOpt)
          {
            msg("Generating annotated exception index...\n");
            writeAnnotatedExceptionIndex(ol);
          }
          break;
        case LayoutNavEntry::ExceptionIndex:
          if (sliceOpt)
          {
            msg("Generating alphabetical exception index...\n");
            writeAlphabeticalExceptionIndex(ol);
          }
          break;
        case LayoutNavEntry::ExceptionHierarchy:
          if (sliceOpt)
          {
            msg("Generating hierarchical exception index...\n");
            writeHierarchicalExceptionIndex(ol);
            if (Config_getBool(HAVE_DOT) && Config_getBool(GRAPHICAL_HIERARCHY))
            {
              msg("Generating graphical exception hierarchy...\n");
              writeGraphicalExceptionHierarchy(ol);
            }
          }
          break;
        case LayoutNavEntry::Files:
          {
            if (Config_getBool(SHOW_FILES) && index.numDocumentedFiles()>0 && addToIndex)
            {
              Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString());
              Doxygen::indexList->incContentsDepth();
              needsClosing=TRUE;
            }
            if (LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Files)!=lne.get()) // for backward compatibility with old layout file
            {
              msg("Generating file index...\n");
              writeFileIndex(ol);
            }
          }
          break;
        case LayoutNavEntry::FileList:
          msg("Generating file index...\n");
          writeFileIndex(ol);
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
          if (addToIndex)
          {
            // prepend a ! or ^ marker to the URL to avoid tampering with it
            QCString url = correctURL(lne->url(),"!"); // add ! to relative URL
            bool isRelative=url.at(0)=='!';
            if (!url.isEmpty() && !isRelative) // absolute URL
            {
              url.prepend("^"); // prepend ^ to absolute URL
            }
            Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),
                                                url,QCString(),FALSE,isRef(lne->baseFile()) || isRelative);
          }
          break;
        case LayoutNavEntry::UserGroup:
          if (addToIndex)
          {
            QCString url = correctURL(lne->url(),"!"); // add ! to relative URL
            if (!url.isEmpty())
            {
              if (url=="!") // result of a "[none]" url
              {
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),QCString(),QCString(),FALSE,FALSE);
              }
              else
              {
                bool isRelative=url.at(0)=='!';
                if (!isRelative) // absolute URL
                {
                  url.prepend("^"); // prepend ^ to absolute URL
                }
                Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),
                                                    url,QCString(),FALSE,isRef(lne->baseFile()) || isRelative);
              }
            }
            else
            {
              Doxygen::indexList->addContentsItem(TRUE,lne->title(),QCString(),lne->baseFile(),QCString(),TRUE,TRUE);
            }
            Doxygen::indexList->incContentsDepth();
            needsClosing=TRUE;
          }
          writeUserGroupStubPage(ol,lne.get());
          break;
        case LayoutNavEntry::None:
          assert(kind != LayoutNavEntry::None); // should never happen, means not properly initialized
          break;
      }
      if (kind!=LayoutNavEntry::User && kind!=LayoutNavEntry::UserGroup) // User entry may appear multiple times
      {
        indexWritten.at(idx)=TRUE;
      }
    }
    writeIndexHierarchyEntries(ol,lne->children());
    if (needsClosing)
    {
      switch(kind)
      {
        case LayoutNavEntry::Modules:
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
    //printf("ending %s kind=%d\n",qPrint(lne->title()),lne->kind());
  }
}

static bool quickLinkVisible(LayoutNavEntry::Kind kind)
{
  const auto &index = Index::instance();
  bool showNamespaces = Config_getBool(SHOW_NAMESPACES);
  bool showFiles = Config_getBool(SHOW_FILES);
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);
  switch (kind)
  {
    case LayoutNavEntry::MainPage:           return TRUE;
    case LayoutNavEntry::User:               return TRUE;
    case LayoutNavEntry::UserGroup:          return TRUE;
    case LayoutNavEntry::Pages:              return index.numIndexedPages()>0;
    case LayoutNavEntry::Topics:             return index.numDocumentedGroups()>0;
    case LayoutNavEntry::Modules:            return index.numDocumentedModules()>0;
    case LayoutNavEntry::ModuleList:         return index.numDocumentedModules()>0;
    case LayoutNavEntry::ModuleMembers:      return index.numDocumentedModuleMembers(ModuleMemberHighlight::All)>0;
    case LayoutNavEntry::Namespaces:         return index.numDocumentedNamespaces()>0 && showNamespaces;
    case LayoutNavEntry::NamespaceList:      return index.numDocumentedNamespaces()>0 && showNamespaces;
    case LayoutNavEntry::NamespaceMembers:   return index.numDocumentedNamespaceMembers(NamespaceMemberHighlight::All)>0;
    case LayoutNavEntry::Concepts:           return index.numDocumentedConcepts()>0;
    case LayoutNavEntry::Classes:            return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassList:          return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassIndex:         return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassHierarchy:     return index.numHierarchyClasses()>0;
    case LayoutNavEntry::ClassMembers:       return index.numDocumentedClassMembers(ClassMemberHighlight::All)>0 && !sliceOpt;
    case LayoutNavEntry::Interfaces:         return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceList:      return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceIndex:     return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceHierarchy: return index.numHierarchyInterfaces()>0;
    case LayoutNavEntry::Structs:            return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::StructList:         return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::StructIndex:        return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::Exceptions:         return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionList:      return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionIndex:     return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionHierarchy: return index.numHierarchyExceptions()>0;
    case LayoutNavEntry::Files:              return index.numDocumentedFiles()>0 && showFiles;
    case LayoutNavEntry::FileList:           return index.numDocumentedFiles()>0 && showFiles;
    case LayoutNavEntry::FileGlobals:        return index.numDocumentedFileMembers(FileMemberHighlight::All)>0;
    case LayoutNavEntry::Examples:           return !Doxygen::exampleLinkedMap->empty();
    case LayoutNavEntry::None:             // should never happen, means not properly initialized
      assert(kind != LayoutNavEntry::None);
      return FALSE;
  }
  return FALSE;
}

template<class T>
void renderMemberIndicesAsJs(std::ostream &t,
    std::function<std::size_t(std::size_t)> numDocumented,
    std::function<Index::MemberIndexMap(std::size_t)> getMemberList,
    const T *(*getInfo)(size_t hl),
    std::size_t total)
{
  // index items per category member lists
  bool firstMember=TRUE;
  for (std::size_t i=0;i<total;i++)
  {
    if (numDocumented(i)>0)
    {
      t << ",";
      if (firstMember)
      {
        t << "children:[";
        firstMember=FALSE;
      }
      t << "\n{text:\"" << convertToJSString(getInfo(i)->title) << "\",url:\""
        << convertToJSString(getInfo(i)->fname+Doxygen::htmlFileExtension) << "\"";

      // Check if we have many members, then add sub entries per letter...
      // quick alphabetical index
      bool quickIndex = numDocumented(i)>maxItemsBeforeQuickIndex;
      if (quickIndex)
      {
        bool multiPageIndex=FALSE;
        if (numDocumented(i)>MAX_ITEMS_BEFORE_MULTIPAGE_INDEX)
        {
          multiPageIndex=TRUE;
        }
        t << ",children:[\n";
        bool firstLetter=TRUE;
        for (const auto &[letter,list] : getMemberList(i))
        {
          if (!firstLetter) t << ",\n";
          QCString ci(letter);
          QCString is(letterToLabel(ci));
          QCString anchor;
          QCString extension=Doxygen::htmlFileExtension;
          QCString fullName = getInfo(i)->fname;
          if (!multiPageIndex || firstLetter)
            anchor=fullName+extension+"#index_";
          else // other pages of multi page index
            anchor=fullName+"_"+is+extension+"#index_";
          t << "{text:\"" << convertToJSString(ci) << "\",url:\""
            << convertToJSString(anchor+convertToId(is)) << "\"}";
          firstLetter=FALSE;
        }
        t << "]";
      }
      t << "}";
    }
  }
  if (!firstMember)
  {
    t << "]";
  }
}

static bool renderQuickLinksAsJs(std::ostream &t,LayoutNavEntry *root,bool first)
{
  int count=0;
  for (const auto &entry : root->children())
  {
    if (entry->visible() && quickLinkVisible(entry->kind())) count++;
  }
  if (count>0) // at least one item is visible
  {
    bool firstChild = TRUE;
    if (!first) t << ",";
    t << "children:[\n";
    for (const auto &entry : root->children())
    {
      if (entry->visible() && quickLinkVisible(entry->kind()))
      {
        if (!firstChild) t << ",\n";
        firstChild=FALSE;
        QCString url = entry->url();
        if (isURL(url)) url = "^" + url;
        t << "{text:\"" << convertToJSString(entry->title()) << "\",url:\""
          << convertToJSString(url) << "\"";
        bool hasChildren=FALSE;
        if (entry->kind()==LayoutNavEntry::ModuleMembers)
        {
          auto numDoc = [](std::size_t i) {
            return Index::instance().numDocumentedModuleMembers(static_cast<ModuleMemberHighlight::Enum>(i));
          };
          auto memList = [](std::size_t i) {
            return Index::instance().isModuleIndexLetterUsed(static_cast<ModuleMemberHighlight::Enum>(i));
          };
          renderMemberIndicesAsJs(t,numDoc,memList,getMmhlInfo,static_cast<std::size_t>(ModuleMemberHighlight::Total));
        }
        if (entry->kind()==LayoutNavEntry::NamespaceMembers)
        {
          auto numDoc = [](std::size_t i) {
            return Index::instance().numDocumentedNamespaceMembers(static_cast<NamespaceMemberHighlight::Enum>(i));
          };
          auto memList = [](std::size_t i) {
            return Index::instance().isNamespaceIndexLetterUsed(static_cast<NamespaceMemberHighlight::Enum>(i));
          };
          renderMemberIndicesAsJs(t,numDoc,memList,getNmhlInfo,static_cast<std::size_t>(NamespaceMemberHighlight::Total));
        }
        else if (entry->kind()==LayoutNavEntry::ClassMembers)
        {
          auto numDoc = [](std::size_t i) {
            return Index::instance().numDocumentedClassMembers(static_cast<ClassMemberHighlight::Enum>(i));
          };
          auto memList = [](std::size_t i) {
            return Index::instance().isClassIndexLetterUsed(static_cast<ClassMemberHighlight::Enum>(i));
          };
          renderMemberIndicesAsJs(t,numDoc,memList,getCmhlInfo,static_cast<std::size_t>(ClassMemberHighlight::Total));
        }
        else if (entry->kind()==LayoutNavEntry::FileGlobals)
        {
          auto numDoc = [](std::size_t i) {
            return Index::instance().numDocumentedFileMembers(static_cast<FileMemberHighlight::Enum>(i));
          };
          auto memList = [](std::size_t i) {
            return Index::instance().isFileIndexLetterUsed(static_cast<FileMemberHighlight::Enum>(i));
          };
          renderMemberIndicesAsJs(t,numDoc,memList,getFmhlInfo,static_cast<std::size_t>(FileMemberHighlight::Total));
        }
        else // recursive into child list
        {
          hasChildren = renderQuickLinksAsJs(t,entry.get(),FALSE);
        }
        if (hasChildren) t << "]";
        t << "}";
      }
    }
  }
  return count>0;
}

static void writeMenuData()
{
  if (!Config_getBool(GENERATE_HTML) || Config_getBool(DISABLE_INDEX)) return;
  QCString outputDir = Config_getBool(HTML_OUTPUT);
  LayoutNavEntry *root = LayoutDocManager::instance().rootNavEntry();
  std::ofstream t = Portable::openOutputStream(outputDir+"/menudata.js");
  if (t.is_open())
  {
    t << JAVASCRIPT_LICENSE_TEXT;
    t << "var menudata={";
    bool hasChildren = renderQuickLinksAsJs(t,root,TRUE);
    if (hasChildren) t << "]";
    t << "}\n";
  }
}

void writeIndexHierarchy(OutputList &ol)
{
  writeMenuData();
  LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry();
  if (lne)
  {
    writeIndexHierarchyEntries(ol,lne->children());
  }
}

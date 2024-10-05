/******************************************************************************
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
 */

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/stat.h>

#include <algorithm>
#include <unordered_map>
#include <memory>
#include <cinttypes>
#include <chrono>
#include <clocale>
#include <locale>

#include "version.h"
#include "doxygen.h"
#include "scanner.h"
#include "entry.h"
#include "index.h"
#include "indexlist.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "pre.h"
#include "tagreader.h"
#include "dot.h"
#include "msc.h"
#include "docparser.h"
#include "dirdef.h"
#include "outputlist.h"
#include "declinfo.h"
#include "htmlgen.h"
#include "latexgen.h"
#include "mangen.h"
#include "language.h"
#include "debug.h"
#include "htmlhelp.h"
#include "qhp.h"
#include "sitemap.h"
#include "ftvhelp.h"
#include "defargs.h"
#include "rtfgen.h"
#include "sqlite3gen.h"
#include "xmlgen.h"
#include "docbookgen.h"
#include "defgen.h"
#include "perlmodgen.h"
#include "reflist.h"
#include "pagedef.h"
#include "commentcnv.h"
#include "cmdmapper.h"
#include "searchindex_js.h"
#include "parserintf.h"
#include "htags.h"
#include "pycode.h"
#include "pyscanner.h"
#include "fortrancode.h"
#include "fortranscanner.h"
#include "xmlcode.h"
#include "sqlcode.h"
#include "lexcode.h"
#include "lexscanner.h"
#include "code.h"
#include "portable.h"
#include "vhdljjparser.h"
#include "vhdldocgen.h"
#include "vhdlcode.h"
#include "eclipsehelp.h"
#include "cite.h"
#include "markdown.h"
#include "arguments.h"
#include "memberlist.h"
#include "layout.h"
#include "groupdef.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"
#include "membername.h"
#include "membergroup.h"
#include "docsets.h"
#include "formula.h"
#include "settings.h"
#include "fileparser.h"
#include "emoji.h"
#include "plantuml.h"
#include "stlsupport.h"
#include "threadpool.h"
#include "clangparser.h"
#include "symbolresolver.h"
#include "regex.h"
#include "aliases.h"
#include "fileinfo.h"
#include "dir.h"
#include "conceptdef.h"
#include "trace.h"
#include "moduledef.h"
#include "stringutil.h"

#include <sqlite3.h>

#if USE_LIBCLANG
#include <clang/Basic/Version.h>
#endif

// provided by the generated file resources.cpp
extern void initResources();

#if !defined(_WIN32) || defined(__CYGWIN__)
#include <signal.h>
#define HAS_SIGNALS
#endif

// globally accessible variables
ClassLinkedMap       *Doxygen::classLinkedMap = nullptr;
ClassLinkedMap       *Doxygen::hiddenClassLinkedMap = nullptr;
ConceptLinkedMap     *Doxygen::conceptLinkedMap = nullptr;
NamespaceLinkedMap   *Doxygen::namespaceLinkedMap = nullptr;
MemberNameLinkedMap  *Doxygen::memberNameLinkedMap = nullptr;
MemberNameLinkedMap  *Doxygen::functionNameLinkedMap = nullptr;
FileNameLinkedMap    *Doxygen::inputNameLinkedMap = nullptr;
GroupLinkedMap       *Doxygen::groupLinkedMap = nullptr;
PageLinkedMap        *Doxygen::pageLinkedMap = nullptr;
PageLinkedMap        *Doxygen::exampleLinkedMap = nullptr;
StringUnorderedSet    Doxygen::inputPaths;
FileNameLinkedMap    *Doxygen::includeNameLinkedMap = nullptr;     // include names
FileNameLinkedMap    *Doxygen::exampleNameLinkedMap = nullptr;     // examples
FileNameLinkedMap    *Doxygen::imageNameLinkedMap = nullptr;       // images
FileNameLinkedMap    *Doxygen::dotFileNameLinkedMap = nullptr;     // dot files
FileNameLinkedMap    *Doxygen::mscFileNameLinkedMap = nullptr;     // msc files
FileNameLinkedMap    *Doxygen::diaFileNameLinkedMap = nullptr;     // dia files
FileNameLinkedMap    *Doxygen::plantUmlFileNameLinkedMap = nullptr;// plantuml files
NamespaceAliasInfoMap Doxygen::namespaceAliasMap;            // all namespace aliases
StringMap             Doxygen::tagDestinationMap;            // all tag locations
StringUnorderedSet    Doxygen::tagFileSet;                   // all tag file names
StringUnorderedSet    Doxygen::expandAsDefinedSet;           // all macros that should be expanded
MemberGroupInfoMap    Doxygen::memberGroupInfoMap;           // dictionary of the member groups heading
std::unique_ptr<PageDef> Doxygen::mainPage;
std::unique_ptr<NamespaceDef> Doxygen::globalNamespaceDef;
NamespaceDefMutable  *Doxygen::globalScope;
bool                  Doxygen::parseSourcesNeeded = FALSE;
SearchIndexIntf       Doxygen::searchIndex;
SymbolMap<Definition>*Doxygen::symbolMap;
ClangUsrMap          *Doxygen::clangUsrMap = nullptr;
Cache<std::string,LookupInfo> *Doxygen::typeLookupCache;
Cache<std::string,LookupInfo> *Doxygen::symbolLookupCache;
DirLinkedMap         *Doxygen::dirLinkedMap;
DirRelationLinkedMap  Doxygen::dirRelations;
ParserManager        *Doxygen::parserManager = nullptr;
QCString              Doxygen::htmlFileExtension;
bool                  Doxygen::suppressDocWarnings = FALSE;
QCString              Doxygen::filterDBFileName;
IndexList            *Doxygen::indexList;
QCString              Doxygen::spaces;
bool                  Doxygen::generatingXmlOutput = FALSE;
DefinesPerFileList    Doxygen::macroDefinitions;
bool                  Doxygen::clangAssistedParsing = FALSE;
QCString              Doxygen::verifiedDotPath;
InputFileEncodingList Doxygen::inputFileEncodingList;
std::mutex            Doxygen::countFlowKeywordsMutex;
std::mutex            Doxygen::addExampleMutex;
StaticInitMap         Doxygen::staticInitMap;

// locally accessible globals
static std::multimap< std::string, const Entry* > g_classEntries;
static StringVector     g_inputFiles;
static OutputList      *g_outputList = nullptr;          // list of output generating objects
static StringSet        g_usingDeclarations; // used classes
static bool             g_successfulRun = FALSE;
static bool             g_dumpSymbolMap = FALSE;

// keywords recognized as compounds
static const StringUnorderedSet g_compoundKeywords =
{ "template class", "template struct", "class", "struct", "union", "interface", "exception" };

void clearAll()
{
  g_inputFiles.clear();
  //g_excludeNameDict.clear();
  //delete g_outputList; g_outputList=nullptr;

  Doxygen::classLinkedMap->clear();
  Doxygen::hiddenClassLinkedMap->clear();
  Doxygen::conceptLinkedMap->clear();
  Doxygen::namespaceLinkedMap->clear();
  Doxygen::pageLinkedMap->clear();
  Doxygen::exampleLinkedMap->clear();
  Doxygen::inputNameLinkedMap->clear();
  Doxygen::includeNameLinkedMap->clear();
  Doxygen::exampleNameLinkedMap->clear();
  Doxygen::imageNameLinkedMap->clear();
  Doxygen::dotFileNameLinkedMap->clear();
  Doxygen::mscFileNameLinkedMap->clear();
  Doxygen::diaFileNameLinkedMap->clear();
  Doxygen::plantUmlFileNameLinkedMap->clear();
  Doxygen::tagDestinationMap.clear();
  SectionManager::instance().clear();
  CitationManager::instance().clear();
  Doxygen::mainPage.reset();
  FormulaManager::instance().clear();
}

class Statistics
{
  public:
    Statistics() {}
    void begin(const char *name)
    {
      msg("%s", name);
      stats.emplace_back(name,0);
      startTime = std::chrono::steady_clock::now();
    }
    void end()
    {
      std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
      stats.back().elapsed = static_cast<double>(std::chrono::duration_cast<
                                std::chrono::microseconds>(endTime - startTime).count())/1000000.0;
      warn_flush();
    }
    void print()
    {
      bool restore=FALSE;
      if (Debug::isFlagSet(Debug::Time))
      {
        Debug::clearFlag(Debug::Time);
        restore=TRUE;
      }
      msg("----------------------\n");
      for (const auto &s : stats)
      {
        msg("Spent %.6f seconds in %s",s.elapsed,s.name);
      }
      if (restore) Debug::setFlag(Debug::Time);
    }
  private:
    struct stat
    {
      const char *name;
      double elapsed;
      //stat() : name(nullptr),elapsed(0) {}
      stat(const char *n, double el) : name(n),elapsed(el) {}
    };
    std::vector<stat> stats;
    std::chrono::steady_clock::time_point startTime;
} g_s;


static void addMemberDocs(const Entry *root,MemberDefMutable *md, const QCString &funcDecl,
                   const ArgumentList *al,bool over_load,TypeSpecifier spec);
static void findMember(const Entry *root,
                       const QCString &relates,
                       const QCString &type,
                       const QCString &args,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      );

enum FindBaseClassRelation_Mode
{
  TemplateInstances,
  DocumentedOnly,
  Undocumented
};


static bool findClassRelation(
                           const Entry *root,
                           Definition *context,
                           ClassDefMutable *cd,
                           const BaseInfo *bi,
                           const TemplateNameMap &templateNames,
                           /*bool insertUndocumented*/
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          );

//----------------------------------------------------------------------------

static Definition *findScopeFromQualifiedName(NamespaceDefMutable *startScope,const QCString &n,
                                              FileDef *fileScope,const TagInfo *tagInfo);

static void addPageToContext(PageDef *pd,Entry *root)
{
  if (root->parent()) // add the page to it's scope
  {
    QCString scope = root->parent()->name;
    if (root->parent()->section.isPackageDoc())
    {
      scope=substitute(scope,".","::");
    }
    scope = stripAnonymousNamespaceScope(scope);
    scope+="::"+pd->name();
    Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,scope,nullptr,root->tagInfo());
    if (d)
    {
      pd->setPageScope(d);
    }
  }
}

static void addRelatedPage(Entry *root)
{
  GroupDef *gd=nullptr;
  for (const Grouping &g : root->groups)
  {
    if (!g.groupname.isEmpty() && (gd=Doxygen::groupLinkedMap->find(g.groupname))) break;
  }
  //printf("---> addRelatedPage() %s gd=%p\n",qPrint(root->name),gd);
  QCString doc=root->doc+root->inbodyDocs;

  PageDef *pd = addRelatedPage(root->name,root->args,doc,
      root->docFile,
      root->docLine,
      root->startLine,
      root->sli,
      gd,root->tagInfo(),
      FALSE,
      root->lang
     );
  if (pd)
  {
    pd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    pd->addSectionsToDefinition(root->anchors);
    pd->setLocalToc(root->localToc);
    addPageToContext(pd,root);
  }
}

static void buildGroupListFiltered(const Entry *root,bool additional, bool includeExternal)
{
  if (root->section.isGroupDoc() && !root->name.isEmpty() &&
        ((!includeExternal && root->tagInfo()==nullptr) ||
         ( includeExternal && root->tagInfo()!=nullptr))
     )
  {
    AUTO_TRACE("additional={} includeExternal={}",additional,includeExternal);
    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL &&  additional))
    {
      GroupDef *gd = Doxygen::groupLinkedMap->find(root->name);
      AUTO_TRACE_ADD("Processing group '{}':'{}' gd={}", root->type,root->name,(void*)gd);

      if (gd)
      {
        if ( !gd->hasGroupTitle() )
        {
          gd->setGroupTitle( root->type );
        }
        else if ( root->type.length() > 0 && root->name != root->type && gd->groupTitle() != root->type )
        {
          warn( root->fileName,root->startLine,
              "group %s: ignoring title \"%s\" that does not match old title \"%s\"",
              qPrint(root->name), qPrint(root->type), qPrint(gd->groupTitle()) );
        }
        gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        gd->setDocumentation( root->doc, root->docFile, root->docLine );
        gd->setInbodyDocumentation( root->inbodyDocs, root->inbodyFile, root->inbodyLine );
        gd->addSectionsToDefinition(root->anchors);
        gd->setRefItems(root->sli);
        gd->setLanguage(root->lang);
        if (root->groupDocType==Entry::GROUPDOC_NORMAL)
        {
          root->commandOverrides.apply_groupGraph([&](bool b) { gd->overrideGroupGraph(b); });
        }
      }
      else
      {
        if (root->tagInfo())
        {
          gd = Doxygen::groupLinkedMap->add(root->name,
               std::unique_ptr<GroupDef>(
                  createGroupDef(root->fileName,root->startLine,root->name,root->type,root->tagInfo()->fileName)));
          gd->setReference(root->tagInfo()->tagName);
        }
        else
        {
          gd = Doxygen::groupLinkedMap->add(root->name,
               std::unique_ptr<GroupDef>(
                  createGroupDef(root->fileName,root->startLine,root->name,root->type)));
        }
        gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        // allow empty docs for group
        gd->setDocumentation(!root->doc.isEmpty() ? root->doc : QCString(" "),root->docFile,root->docLine,FALSE);
        gd->setInbodyDocumentation( root->inbodyDocs, root->inbodyFile, root->inbodyLine );
        gd->addSectionsToDefinition(root->anchors);
        gd->setRefItems(root->sli);
        gd->setLanguage(root->lang);
        if (root->groupDocType==Entry::GROUPDOC_NORMAL)
        {
          root->commandOverrides.apply_groupGraph([&](bool b) { gd->overrideGroupGraph(b); });
        }
      }
    }
  }
  for (const auto &e : root->children()) buildGroupListFiltered(e.get(),additional,includeExternal);
}

static void buildGroupList(const Entry *root)
{
  // --- first process only local groups
  // first process the @defgroups blocks
  buildGroupListFiltered(root,FALSE,FALSE);
  // then process the @addtogroup, @weakgroup blocks
  buildGroupListFiltered(root,TRUE,FALSE);

  // --- then also process external groups
  // first process the @defgroups blocks
  buildGroupListFiltered(root,FALSE,TRUE);
  // then process the @addtogroup, @weakgroup blocks
  buildGroupListFiltered(root,TRUE,TRUE);
}

static void findGroupScope(const Entry *root)
{
  if (root->section.isGroupDoc() && !root->name.isEmpty() &&
      root->parent() && !root->parent()->name.isEmpty())
  {
    GroupDef *gd = Doxygen::groupLinkedMap->find(root->name);
    if (gd)
    {
      QCString scope = root->parent()->name;
      if (root->parent()->section.isPackageDoc())
      {
        scope=substitute(scope,".","::");
      }
      scope = stripAnonymousNamespaceScope(scope);
      scope+="::"+gd->name();
      Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,scope,nullptr,root->tagInfo());
      if (d)
      {
        gd->setGroupScope(d);
      }
    }
  }
  for (const auto &e : root->children()) findGroupScope(e.get());
}

static void organizeSubGroupsFiltered(const Entry *root,bool additional)
{
  if (root->section.isGroupDoc() && !root->name.isEmpty())
  {
    AUTO_TRACE("additional={}",additional);
    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL && additional))
    {
      GroupDef *gd = Doxygen::groupLinkedMap->find(root->name);
      if (gd)
      {
        AUTO_TRACE_ADD("adding {} to group {}",root->name,gd->name());
        addGroupToGroups(root,gd);
      }
    }
  }
  for (const auto &e : root->children()) organizeSubGroupsFiltered(e.get(),additional);
}

static void organizeSubGroups(const Entry *root)
{
  //printf("Defining groups\n");
  // first process the @defgroups blocks
  organizeSubGroupsFiltered(root,FALSE);
  //printf("Additional groups\n");
  // then process the @addtogroup, @weakgroup blocks
  organizeSubGroupsFiltered(root,TRUE);
}

//----------------------------------------------------------------------

static void buildFileList(const Entry *root)
{
  if ((root->section.isFileDoc() || (root->section.isFile() && Config_getBool(EXTRACT_ALL))) &&
      !root->name.isEmpty() && !root->tagInfo() // skip any file coming from tag files
     )
  {
    bool ambig = false;
    FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,root->name,ambig);
    if (!fd || ambig)
    {
      bool save_ambig = ambig;
      // use the directory of the file to see if the described file is in the same
      // directory as the describing file.
      QCString fn = root->fileName;
      int newIndex=fn.findRev('/');
      if (newIndex<0)
      {
        fn = root->name;
      }
      else
      {
        fn = fn.left(newIndex)+"/"+root->name;
      }
      fd=findFileDef(Doxygen::inputNameLinkedMap,fn,ambig);
      if (!fd) ambig = save_ambig;
    }
    //printf("**************** root->name=%s fd=%p\n",qPrint(root->name),(void*)fd);
    if (fd && !ambig)
    {
      //printf("Adding documentation!\n");
      // using FALSE in setDocumentation is small hack to make sure a file
      // is documented even if a \file command is used without further
      // documentation
      fd->setDocumentation(root->doc,root->docFile,root->docLine,FALSE);
      fd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      fd->addSectionsToDefinition(root->anchors);
      fd->setRefItems(root->sli);
      root->commandOverrides.apply_includeGraph   ([&](bool b) { fd->overrideIncludeGraph(b);    });
      root->commandOverrides.apply_includedByGraph([&](bool b) { fd->overrideIncludedByGraph(b); });
      for (const Grouping &g : root->groups)
      {
        GroupDef *gd=nullptr;
        if (!g.groupname.isEmpty() && (gd=Doxygen::groupLinkedMap->find(g.groupname)))
        {
          if (!gd->containsFile(fd))
          {
            gd->addFile(fd);
            fd->makePartOfGroup(gd);
            //printf("File %s: in group %s\n",qPrint(fd->name()),qPrint(gd->name()));
          }
        }
        else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
        {
          warn(root->fileName, root->startLine,
              "Found non-existing group '%s' for the command '%s', ignoring command",
              qPrint(g.groupname), Grouping::getGroupPriName( g.pri )
              );
    }
      }
    }
    else
    {
      QCString text(4096, QCString::ExplicitSize);
      text.sprintf("the name '%s' supplied as "
          "the argument in the \\file statement ",
          qPrint(root->name));
      if (ambig) // name is ambiguous
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameLinkedMap,root->name);
        text+="\n";
        text+="Please use a more specific name by "
          "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(root->fileName,root->startLine,"%s", qPrint(text));
    }
  }
  for (const auto &e : root->children()) buildFileList(e.get());
}

template<class DefMutable>
static void addIncludeFile(DefMutable *cd,FileDef *ifd,const Entry *root)
{
  if (
      (!root->doc.stripWhiteSpace().isEmpty() ||
       !root->brief.stripWhiteSpace().isEmpty() ||
       Config_getBool(EXTRACT_ALL)
      ) && root->protection!=Protection::Private
     )
  {
    //printf(">>>>>> includeFile=%s\n",qPrint(root->includeFile));

    bool local=Config_getBool(FORCE_LOCAL_INCLUDES);
    QCString includeFile = root->includeFile;
    if (!includeFile.isEmpty() && includeFile.at(0)=='"')
    {
      local = TRUE;
      includeFile=includeFile.mid(1,includeFile.length()-2);
    }
    else if (!includeFile.isEmpty() && includeFile.at(0)=='<')
    {
      local = FALSE;
      includeFile=includeFile.mid(1,includeFile.length()-2);
    }

    bool ambig = false;
    FileDef *fd=nullptr;
    // see if we need to include a verbatim copy of the header file
    //printf("root->includeFile=%s\n",qPrint(root->includeFile));
    if (!includeFile.isEmpty() &&
        (fd=findFileDef(Doxygen::inputNameLinkedMap,includeFile,ambig))==nullptr
       )
    { // explicit request
      QCString text;
      text.sprintf("the name '%s' supplied as "
                  "the argument of the \\class, \\struct, \\union, or \\include command ",
                  qPrint(includeFile)
                 );
      if (ambig) // name is ambiguous
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameLinkedMap,root->includeFile);
        text+="\n";
        text+="Please use a more specific name by "
            "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(root->fileName,root->startLine, "%s", qPrint(text));
    }
    else if (includeFile.isEmpty() && ifd &&
        // see if the file extension makes sense
        guessSection(ifd->name()).isHeader())
    { // implicit assumption
      fd=ifd;
    }

    // if a file is found, we mark it as a source file.
    if (fd)
    {
      QCString iName = !root->includeName.isEmpty() ?
                       root->includeName : includeFile;
      if (!iName.isEmpty()) // user specified include file
      {
        if (iName.at(0)=='<') local=FALSE; // explicit override
        else if (iName.at(0)=='"') local=TRUE;
        if (iName.at(0)=='"' || iName.at(0)=='<')
        {
          iName=iName.mid(1,iName.length()-2); // strip quotes or brackets
        }
        if (iName.isEmpty())
        {
          iName=fd->name();
        }
      }
      else if (!Config_getList(STRIP_FROM_INC_PATH).empty())
      {
        iName=stripFromIncludePath(fd->absFilePath());
      }
      else // use name of the file containing the class definition
      {
        iName=fd->name();
      }
      if (fd->generateSourceFile()) // generate code for header
      {
        cd->setIncludeFile(fd,iName,local,!root->includeName.isEmpty());
      }
      else // put #include in the class documentation without link
      {
        cd->setIncludeFile(nullptr,iName,local,TRUE);
      }
    }
  }
}


QCString stripTemplateSpecifiers(const QCString &s)
{
  size_t l = s.length();
  int count=0;
  int round=0;
  QCString result;
  for (size_t i=0;i<l;i++)
  {
    char c=s.at(i);
    if      (c=='(') round++;
    else if (c==')' && round>0) round--;
    else if (c=='<' && round==0) count++;
    if (count==0)
    {
      result+=c;
    }
    if (c=='>' && round==0 && count>0) count--;
  }
  //printf("stripTemplateSpecifiers(%s)=%s\n",qPrint(s),qPrint(result));
  return result;
}

/*! returns the Definition object belonging to the first \a level levels of
 *  full qualified name \a name. Creates an artificial scope if the scope is
 *  not found and set the parent/child scope relation if the scope is found.
 */
[[maybe_unused]]
static Definition *buildScopeFromQualifiedName(const QCString &name_,SrcLangExt lang,const TagInfo *tagInfo)
{
  QCString name = stripTemplateSpecifiers(name_);
  name.stripPrefix("::");
  int level = name.contains("::");
  //printf("buildScopeFromQualifiedName(%s) level=%d\n",qPrint(name),level);
  int i=0, p=0, l=0;
  Definition *prevScope=Doxygen::globalScope;
  QCString fullScope;
  while (i<level)
  {
    int idx=getScopeFragment(name,p,&l);
    if (idx==-1) return prevScope;
    QCString nsName = name.mid(idx,l);
    if (nsName.isEmpty()) return prevScope;
    if (!fullScope.isEmpty()) fullScope+="::";
    fullScope+=nsName;
    NamespaceDef *nd=Doxygen::namespaceLinkedMap->find(fullScope);
    DefinitionMutable *innerScope = toDefinitionMutable(nd);
    ClassDef *cd=nullptr;
    if (nd==nullptr) cd = getClass(fullScope);
    if (nd==nullptr && cd) // scope is a class
    {
      innerScope = toDefinitionMutable(cd);
    }
    else if (nd==nullptr && cd==nullptr && fullScope.find('<')==-1) // scope is not known and could be a namespace!
    {
      // introduce bogus namespace
      //printf("++ adding dummy namespace %s to %s tagInfo=%p\n",qPrint(nsName),qPrint(prevScope->name()),(void*)tagInfo);
      NamespaceDefMutable *newNd=
        toNamespaceDefMutable(
          Doxygen::namespaceLinkedMap->add(fullScope,
            createNamespaceDef(
              "[generated]",1,1,fullScope,
              tagInfo?tagInfo->tagName:QCString(),
              tagInfo?tagInfo->fileName:QCString())));
      if (newNd)
      {
        newNd->setLanguage(lang);
        newNd->setArtificial(TRUE);
        // add namespace to the list
        innerScope = newNd;
      }
    }
    else // scope is a namespace
    {
    }
    if (innerScope)
    {
      // make the parent/child scope relation
      DefinitionMutable *prevScopeMutable = toDefinitionMutable(prevScope);
      if (prevScopeMutable)
      {
        prevScopeMutable->addInnerCompound(toDefinition(innerScope));
      }
      innerScope->setOuterScope(prevScope);
    }
    else // current scope is a class, so return only the namespace part...
    {
      return prevScope;
    }
    // proceed to the next scope fragment
    p=idx+l+2;
    prevScope=toDefinition(innerScope);
    i++;
  }
  return prevScope;
}

static Definition *findScopeFromQualifiedName(NamespaceDefMutable *startScope,const QCString &n,
                                              FileDef *fileScope,const TagInfo *tagInfo)
{
  //printf("<findScopeFromQualifiedName(%s,%s)\n",startScope ? qPrint(startScope->name()) : 0, qPrint(n));
  Definition *resultScope=toDefinition(startScope);
  if (resultScope==nullptr) resultScope=Doxygen::globalScope;
  QCString scope=stripTemplateSpecifiersFromScope(n,FALSE);
  int l1 = 0;
  int i1 = getScopeFragment(scope,0,&l1);
  if (i1==-1)
  {
    //printf(">no fragments!\n");
    return resultScope;
  }
  int p=i1+l1,l2=0,i2=0;
  while ((i2=getScopeFragment(scope,p,&l2))!=-1)
  {
    QCString nestedNameSpecifier = scope.mid(i1,l1);
    Definition *orgScope = resultScope;
    //printf("  nestedNameSpecifier=%s\n",qPrint(nestedNameSpecifier));
    resultScope = const_cast<Definition*>(resultScope->findInnerCompound(nestedNameSpecifier));
    //printf("  resultScope=%p\n",resultScope);
    if (resultScope==nullptr)
    {
      if (orgScope==Doxygen::globalScope && fileScope && !fileScope->getUsedNamespaces().empty())
        // also search for used namespaces
      {
        for (const auto &nd : fileScope->getUsedNamespaces())
        {
          NamespaceDef *mnd = toNamespaceDefMutable(nd);
          if (mnd)
          {
            resultScope = findScopeFromQualifiedName(toNamespaceDefMutable(nd),n,fileScope,tagInfo);
            if (resultScope!=nullptr) break;
          }
        }
        if (resultScope)
        {
          // for a nested class A::I in used namespace N, we get
          // N::A::I while looking for A, so we should compare
          // resultScope->name() against scope.left(i2+l2)
          //printf("  -> result=%s scope=%s\n",qPrint(resultScope->name()),qPrint(scope));
          if (rightScopeMatch(resultScope->name(),scope.left(i2+l2)))
          {
            break;
          }
          goto nextFragment;
        }
      }

      // also search for used classes. Complication: we haven't been able
      // to put them in the right scope yet, because we are still resolving
      // the scope relations!
      // Therefore loop through all used classes and see if there is a right
      // scope match between the used class and nestedNameSpecifier.
      for (const auto &usedName : g_usingDeclarations)
      {
        //printf("Checking using class %s\n",usedName.c_str());
        if (rightScopeMatch(usedName.c_str(),nestedNameSpecifier))
        {
          // ui.currentKey() is the fully qualified name of nestedNameSpecifier
          // so use this instead.
          QCString fqn = QCString(usedName) + scope.right(scope.length()-p);
          resultScope = buildScopeFromQualifiedName(fqn,startScope->getLanguage(),nullptr);
          //printf("Creating scope from fqn=%s result %p\n",qPrint(fqn),resultScope);
          if (resultScope)
          {
            //printf("> Match! resultScope=%s\n",qPrint(resultScope->name()));
            return resultScope;
          }
        }
      }

      //printf("> name %s not found in scope %s\n",qPrint(nestedNameSpecifier),qPrint(orgScope->name()));
      return nullptr;
    }
 nextFragment:
    i1=i2;
    l1=l2;
    p=i2+l2;
  }
  //printf(">findScopeFromQualifiedName scope %s\n",qPrint(resultScope->name()));
  return resultScope;
}

std::unique_ptr<ArgumentList> getTemplateArgumentsFromName(
                  const QCString &name,
                  const ArgumentLists &tArgLists)
{
  // for each scope fragment, check if it is a template and advance through
  // the list if so.
  int i=0, p=0;
  auto alIt = tArgLists.begin();
  while ((i=name.find("::",p))!=-1 && alIt!=tArgLists.end())
  {
    NamespaceDef *nd = Doxygen::namespaceLinkedMap->find(name.left(i));
    if (nd==nullptr)
    {
      ClassDef *cd = getClass(name.left(i));
      if (cd)
      {
        if (!cd->templateArguments().empty())
        {
          ++alIt;
        }
      }
    }
    p=i+2;
  }
  return alIt!=tArgLists.end() ?
         std::make_unique<ArgumentList>(*alIt) :
         std::unique_ptr<ArgumentList>();
}

static
ClassDef::CompoundType convertToCompoundType(EntryType section,TypeSpecifier specifier)
{
  ClassDef::CompoundType sec=ClassDef::Class;

  if (specifier.isStruct())
    sec=ClassDef::Struct;
  else if (specifier.isUnion())
    sec=ClassDef::Union;
  else if (specifier.isCategory())
    sec=ClassDef::Category;
  else if (specifier.isInterface())
    sec=ClassDef::Interface;
  else if (specifier.isProtocol())
    sec=ClassDef::Protocol;
  else if (specifier.isException())
    sec=ClassDef::Exception;
  else if (specifier.isService())
    sec=ClassDef::Service;
  else if (specifier.isSingleton())
    sec=ClassDef::Singleton;

  if (section.isUnionDoc())
    sec=ClassDef::Union;
  else if (section.isStructDoc())
    sec=ClassDef::Struct;
  else if (section.isInterfaceDoc())
    sec=ClassDef::Interface;
  else if (section.isProtocolDoc())
    sec=ClassDef::Protocol;
  else if (section.isCategoryDoc())
    sec=ClassDef::Category;
  else if (section.isExceptionDoc())
    sec=ClassDef::Exception;
  else if (section.isServiceDoc())
    sec=ClassDef::Service;
  else if (section.isSingletonDoc())
    sec=ClassDef::Singleton;

  return sec;
}


static void addClassToContext(const Entry *root)
{
  AUTO_TRACE("name={}",root->name);
  FileDef *fd = root->fileDef();

  QCString scName;
  if (root->parent()->section.isScope())
  {
     scName=root->parent()->name;
  }
  // name without parent's scope
  QCString fullName = root->name;

  // strip off any template parameters (but not those for specializations)
  fullName=stripTemplateSpecifiersFromScope(fullName);

  // name with scope (if not present already)
  QCString qualifiedName = fullName;
  if (!scName.isEmpty() && !leftScopeMatch(fullName,scName))
  {
    qualifiedName.prepend(scName+"::");
  }

  // see if we already found the class before
  ClassDefMutable *cd = getClassMutable(qualifiedName);

  AUTO_TRACE_ADD("Found class with name '{}', qualifiedName '{}'", cd ? cd->name() : root->name, qualifiedName);

  if (cd)
  {
    fullName=cd->name();
    AUTO_TRACE_ADD("Existing class '{}'",cd->name());
    //if (cd->templateArguments()==0)
    //{
    //  //printf("existing ClassDef tempArgList=%p specScope=%s\n",root->tArgList,qPrint(root->scopeSpec));
    //  cd->setTemplateArguments(tArgList);
    //}

    cd->setDocumentation(root->doc,root->docFile,root->docLine);
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    root->commandOverrides.apply_collaborationGraph([&](bool b          ) { cd->overrideCollaborationGraph(b); });
    root->commandOverrides.apply_inheritanceGraph  ([&](CLASS_GRAPH_t gt) { cd->overrideInheritanceGraph(gt); });

    if (!root->spec.isForwardDecl() && cd->isForwardDeclared())
    {
      cd->setDefFile(root->fileName,root->startLine,root->startColumn);
      if (root->bodyLine!=-1)
      {
        cd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
        cd->setBodyDef(fd);
      }
    }

    if (cd->templateArguments().empty() || (cd->isForwardDeclared() && !root->spec.isForwardDecl()))
    {
      // this happens if a template class declared with @class is found
      // before the actual definition or if a forward declaration has different template
      // parameter names.
      std::unique_ptr<ArgumentList> tArgList = getTemplateArgumentsFromName(cd->name(),root->tArgLists);
      if (tArgList)
      {
        cd->setTemplateArguments(*tArgList);
      }
    }
    if (cd->requiresClause().isEmpty() && !root->req.isEmpty())
    {
      cd->setRequiresClause(root->req);
    }

    cd->setCompoundType(convertToCompoundType(root->section,root->spec));

    cd->setMetaData(root->metaData);
  }
  else // new class
  {
    ClassDef::CompoundType sec = convertToCompoundType(root->section,root->spec);

    QCString className;
    QCString namespaceName;
    extractNamespaceName(fullName,className,namespaceName);

    AUTO_TRACE_ADD("New class: fullname '{}' namespace '{}' name='{}' brief='{}' docs='{}'",
        fullName, namespaceName, className, Trace::trunc(root->brief), Trace::trunc(root->doc));

    QCString tagName;
    QCString refFileName;
    const TagInfo *tagInfo = root->tagInfo();
    if (tagInfo)
    {
      tagName     = tagInfo->tagName;
      refFileName = tagInfo->fileName;
      if (fullName.find("::")!=-1)
        // symbols imported via tag files may come without the parent scope,
        // so we artificially create it here
      {
        buildScopeFromQualifiedName(fullName,root->lang,tagInfo);
      }
    }
    std::unique_ptr<ArgumentList> tArgList;
    int i=0;
    if ((root->lang==SrcLangExt::CSharp || root->lang==SrcLangExt::Java) && (i=fullName.findRev('<'))!=-1)
    {
      // a Java/C# generic class looks like a C++ specialization, so we need to split the
      // name and template arguments here
      tArgList = stringToArgumentList(root->lang,fullName.mid(i));
      fullName=fullName.left(i);
    }
    else
    {
      tArgList = getTemplateArgumentsFromName(fullName,root->tArgLists);
    }
    // add class to the list
    cd = toClassDefMutable(
        Doxygen::classLinkedMap->add(fullName,
          createClassDef(tagInfo?tagName:root->fileName,root->startLine,root->startColumn,
             fullName,sec,tagName,refFileName,TRUE,root->spec.isEnum()) ));
    if (cd)
    {
      AUTO_TRACE_ADD("New class '{}' type={} #tArgLists={} tagInfo={} hidden={} artificial={}",
              fullName,cd->compoundTypeString(),root->tArgLists.size(),
              fmt::ptr(tagInfo),root->hidden,root->artificial);
      cd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
      cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      cd->setLanguage(root->lang);
      cd->setId(root->id);
      cd->setHidden(root->hidden);
      cd->setArtificial(root->artificial);
      cd->setClassSpecifier(root->spec);
      cd->addQualifiers(root->qualifiers);
      cd->setTypeConstraints(root->typeConstr);
      root->commandOverrides.apply_collaborationGraph([&](bool b          ) { cd->overrideCollaborationGraph(b); });
      root->commandOverrides.apply_inheritanceGraph  ([&](CLASS_GRAPH_t gt) { cd->overrideInheritanceGraph(gt); });

      if (tArgList)
      {
        cd->setTemplateArguments(*tArgList);
      }
      cd->setRequiresClause(root->req);
      cd->setProtection(root->protection);
      cd->setIsStatic(root->isStatic);

      // file definition containing the class cd
      cd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
      cd->setBodyDef(fd);

      cd->setMetaData(root->metaData);

      cd->insertUsedFile(fd);
    }
    else
    {
      AUTO_TRACE_ADD("Class {} not added, already exists as alias", fullName);
    }
  }

  if (cd)
  {
    cd->addSectionsToDefinition(root->anchors);
    if (!root->subGrouping) cd->setSubGrouping(FALSE);
    if (!root->spec.isForwardDecl())
    {
      if (cd->hasDocumentation())
      {
        addIncludeFile(cd,fd,root);
      }
      if (fd && root->section.isCompound())
      {
        AUTO_TRACE_ADD("Inserting class {} in file {} (root->fileName='{}')", cd->name(), fd->name(), root->fileName);
        cd->setFileDef(fd);
        fd->insertClass(cd);
      }
    }
    addClassToGroups(root,cd);
    ModuleManager::instance().addClassToModule(root,cd);
    cd->setRefItems(root->sli);
  }
}

//----------------------------------------------------------------------
// build a list of all classes mentioned in the documentation
// and all classes that have a documentation block before their definition.
static void buildClassList(const Entry *root)
{
  if ((root->section.isCompound() || root->section.isObjcImpl()) && !root->name.isEmpty())
  {
    AUTO_TRACE();
    addClassToContext(root);
  }
  for (const auto &e : root->children()) buildClassList(e.get());
}

static void buildClassDocList(const Entry *root)
{
  if ((root->section.isCompoundDoc()) && !root->name.isEmpty())
  {
    AUTO_TRACE();
    addClassToContext(root);
  }
  for (const auto &e : root->children()) buildClassDocList(e.get());
}

//----------------------------------------------------------------------
// build a list of all classes mentioned in the documentation
// and all classes that have a documentation block before their definition.

static void addConceptToContext(const Entry *root)
{
  AUTO_TRACE();
  FileDef *fd = root->fileDef();

  QCString scName;
  if (root->parent()->section.isScope())
  {
     scName=root->parent()->name;
  }

  // name with scope (if not present already)
  QCString qualifiedName = root->name;
  if (!scName.isEmpty() && !leftScopeMatch(qualifiedName,scName))
  {
    qualifiedName.prepend(scName+"::");
  }

  // see if we already found the concept before
  ConceptDefMutable *cd = getConceptMutable(qualifiedName);

  AUTO_TRACE_ADD("Found concept with name '{}' (qualifiedName='{}')", cd ? cd->name() : root->name, qualifiedName);

  if (cd)
  {
    qualifiedName=cd->name();
    AUTO_TRACE_ADD("Existing concept '{}'",cd->name());

    cd->setDocumentation(root->doc,root->docFile,root->docLine);
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);

    addIncludeFile(cd,fd,root);
  }
  else // new concept
  {
    QCString className;
    QCString namespaceName;
    extractNamespaceName(qualifiedName,className,namespaceName);

    AUTO_TRACE_ADD("New concept: fullname '{}' namespace '{}' name='{}' brief='{}' docs='{}'",
        qualifiedName,namespaceName,className,root->brief,root->doc);

    QCString tagName;
    QCString refFileName;
    const TagInfo *tagInfo = root->tagInfo();
    if (tagInfo)
    {
      tagName     = tagInfo->tagName;
      refFileName = tagInfo->fileName;
      if (qualifiedName.find("::")!=-1)
        // symbols imported via tag files may come without the parent scope,
        // so we artificially create it here
      {
        buildScopeFromQualifiedName(qualifiedName,root->lang,tagInfo);
      }
    }
    std::unique_ptr<ArgumentList> tArgList = getTemplateArgumentsFromName(qualifiedName,root->tArgLists);
    // add concept to the list
    cd = toConceptDefMutable(
        Doxygen::conceptLinkedMap->add(qualifiedName,
            createConceptDef(tagInfo?tagName:root->fileName,root->startLine,root->startColumn,
               qualifiedName,tagName,refFileName)));
    if (cd)
    {
      AUTO_TRACE_ADD("New concept '{}' #tArgLists={} tagInfo={}",
          qualifiedName,root->tArgLists.size(),fmt::ptr(tagInfo));
      cd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
      cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      cd->setLanguage(root->lang);
      cd->setId(root->id);
      cd->setHidden(root->hidden);
      cd->setGroupId(root->mGrpId);
      if (tArgList)
      {
        cd->setTemplateArguments(*tArgList);
      }
      cd->setInitializer(root->initializer.str().c_str());
      // file definition containing the class cd
      cd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
      cd->setBodyDef(fd);
      addIncludeFile(cd,fd,root);

      // also add namespace to the correct structural context
      Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,qualifiedName,nullptr,tagInfo);
      if (d && d->definitionType()==Definition::TypeNamespace)
      {
        DefinitionMutable *dm = toDefinitionMutable(d);
        if (dm)
        {
          dm->addInnerCompound(cd);
        }
        cd->setOuterScope(d);
      }
    }
    else
    {
      AUTO_TRACE_ADD("Concept '{}' not added, already exists (as alias)", qualifiedName);
    }
  }

  if (cd)
  {
    cd->addSectionsToDefinition(root->anchors);
    if (fd)
    {
      AUTO_TRACE_ADD("Inserting concept '{}' in file '{}' (root->fileName='{}')", cd->name(), fd->name(), root->fileName);
      cd->setFileDef(fd);
      fd->insertConcept(cd);
    }
    addConceptToGroups(root,cd);
    ModuleManager::instance().addConceptToModule(root,cd);
    cd->setRefItems(root->sli);
  }
}

static void findModuleDocumentation(const Entry *root)
{
  if (root->section.isModuleDoc())
  {
    AUTO_TRACE();
    ModuleManager::instance().addDocs(root);
  }
  for (const auto &e : root->children()) findModuleDocumentation(e.get());
}

static void buildConceptList(const Entry *root)
{
  if (root->section.isConcept())
  {
    AUTO_TRACE();
    addConceptToContext(root);
  }
  for (const auto &e : root->children()) buildConceptList(e.get());
}

static void buildConceptDocList(const Entry *root)
{
  if (root->section.isConceptDoc())
  {
    AUTO_TRACE();
    addConceptToContext(root);
  }
  for (const auto &e : root->children()) buildConceptDocList(e.get());
}

// This routine is to allow @ingroup X @{ concept A; concept B; @} to work
// (same also works for variable and functions because of logic in MemberGroup::insertMember)
static void distributeConceptGroups()
{
  AUTO_TRACE();
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if (cd->groupId()!=DOX_NOGROUP)
    {
      for (const auto &ocd : *Doxygen::conceptLinkedMap)
      {
        if (cd!=ocd && cd->groupId()==ocd->groupId() &&
            !cd->partOfGroups().empty() && ocd->partOfGroups().empty())
        {
          ConceptDefMutable *ocdm = toConceptDefMutable(ocd.get());
          if (ocdm)
          {
            for (const auto &gd : cd->partOfGroups())
            {
              if (gd)
              {
                AUTO_TRACE_ADD("making concept '{}' part of group '{}'",ocdm->name(),gd->name());
                ocdm->makePartOfGroup(gd);
                gd->addConcept(ocd.get());
              }
            }
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void resolveClassNestingRelations()
{
  ClassDefSet visitedClasses;

  bool done=FALSE;
  //int iteration=0;
  while (!done)
  {
    done=TRUE;
    //++iteration;
    struct ClassAlias
    {
      ClassAlias(const QCString &name,std::unique_ptr<ClassDef> cd,DefinitionMutable *ctx) :
        aliasFullName(name),aliasCd(std::move(cd)), aliasContext(ctx) {}
      QCString aliasFullName;
      std::unique_ptr<ClassDef> aliasCd;
      DefinitionMutable *aliasContext;
    };
    std::vector<ClassAlias> aliases;
    for (const auto &icd : *Doxygen::classLinkedMap)
    {
      ClassDefMutable *cd = toClassDefMutable(icd.get());
      if (cd && visitedClasses.find(icd.get())==visitedClasses.end())
      {
        QCString name = stripAnonymousNamespaceScope(icd->name());
        //printf("processing=%s, iteration=%d\n",qPrint(cd->name()),iteration);
        // also add class to the correct structural context
        Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,
                                                 name,icd->getFileDef(),nullptr);
        if (d)
        {
          //printf("****** adding %s to scope %s in iteration %d\n",qPrint(cd->name()),qPrint(d->name()),iteration);
          DefinitionMutable *dm = toDefinitionMutable(d);
          if (dm)
          {
            dm->addInnerCompound(cd);
          }
          cd->setOuterScope(d);

          // for inline namespace add an alias of the class to the outer scope
          while (d->definitionType()==Definition::TypeNamespace)
          {
            NamespaceDef *nd = toNamespaceDef(d);
            //printf("nd->isInline()=%d\n",nd->isInline());
            if (nd && nd->isInline())
            {
              d = d->getOuterScope();
              if (d)
              {
                dm = toDefinitionMutable(d);
                if (dm)
                {
                  auto aliasCd = createClassDefAlias(d,cd);
                  QCString aliasFullName = d->qualifiedName()+"::"+aliasCd->localName();
                  aliases.emplace_back(aliasFullName,std::move(aliasCd),dm);
                  //printf("adding %s to %s as %s\n",qPrint(aliasCd->name()),qPrint(d->name()),qPrint(aliasFullName));
                }
              }
            }
            else
            {
              break;
            }
          }

          visitedClasses.insert(icd.get());
          done=FALSE;
        }
        //else
        //{
        //  printf("****** ignoring %s: scope not (yet) found in iteration %d\n",qPrint(cd->name()),iteration);
        //}
      }
    }
    // add aliases
    for (auto &alias : aliases)
    {
       ClassDef *aliasCd = Doxygen::classLinkedMap->add(alias.aliasFullName,std::move(alias.aliasCd));
       if (aliasCd)
       {
         alias.aliasContext->addInnerCompound(aliasCd);
       }
    }
  }

  //give warnings for unresolved compounds
  for (const auto &icd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cd = toClassDefMutable(icd.get());
    if (cd && visitedClasses.find(icd.get())==visitedClasses.end())
    {
      QCString name = stripAnonymousNamespaceScope(cd->name());
      //printf("processing unresolved=%s, iteration=%d\n",qPrint(cd->name()),iteration);
      /// create the scope artificially
      // anyway, so we can at least relate scopes properly.
      Definition *d = buildScopeFromQualifiedName(name,cd->getLanguage(),nullptr);
      if (d && d!=cd && !cd->getDefFileName().isEmpty())
                 // avoid recursion in case of redundant scopes, i.e: namespace N { class N::C {}; }
                 // for this case doxygen assumes the existence of a namespace N::N in which C is to be found!
                 // also avoid warning for stuff imported via a tagfile.
      {
        DefinitionMutable *dm = toDefinitionMutable(d);
        if (dm)
        {
          dm->addInnerCompound(cd);
        }
        cd->setOuterScope(d);
        warn(cd->getDefFileName(),cd->getDefLine(),
            "Internal inconsistency: scope for class %s not "
            "found!",qPrint(name)
            );
      }
    }
  }
}

void distributeClassGroupRelations()
{
  //bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  //if (!inlineGroupedClasses) return;
  //printf("** distributeClassGroupRelations()\n");

  ClassDefSet visitedClasses;
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    //printf("Checking %s\n",qPrint(cd->name()));
    // distribute the group to nested classes as well
    if (visitedClasses.find(cd.get())==visitedClasses.end() && !cd->partOfGroups().empty())
    {
      //printf("  Candidate for merging\n");
      GroupDef *gd = cd->partOfGroups().front();
      for (auto &ncd : cd->getClasses())
      {
        ClassDefMutable *ncdm = toClassDefMutable(ncd);
        if (ncdm && ncdm->partOfGroups().empty())
        {
          //printf("  Adding %s to group '%s'\n",qPrint(ncd->name()),
          //    gd->groupTitle());
          ncdm->makePartOfGroup(gd);
          gd->addClass(ncdm);
        }
      }
      visitedClasses.insert(cd.get()); // only visit every class once
    }
  }
}

//----------------------------

static ClassDefMutable *createTagLessInstance(const ClassDef *rootCd,const ClassDef *templ,const QCString &fieldName)
{
  QCString fullName = removeAnonymousScopes(templ->name());
  if (fullName.endsWith("::")) fullName=fullName.left(fullName.length()-2);
  fullName+="."+fieldName;

  //printf("** adding class %s based on %s\n",qPrint(fullName),qPrint(templ->name()));
  ClassDefMutable *cd = toClassDefMutable(
      Doxygen::classLinkedMap->add(fullName,
         createClassDef(templ->getDefFileName(),
                            templ->getDefLine(),
                            templ->getDefColumn(),
                            fullName,
                            templ->compoundType())));
  if (cd)
  {
    cd->setDocumentation(templ->documentation(),templ->docFile(),templ->docLine()); // copy docs to definition
    cd->setBriefDescription(templ->briefDescription(),templ->briefFile(),templ->briefLine());
    cd->setLanguage(templ->getLanguage());
    cd->setBodySegment(templ->getDefLine(),templ->getStartBodyLine(),templ->getEndBodyLine());
    cd->setBodyDef(templ->getBodyDef());

    cd->setOuterScope(rootCd->getOuterScope());
    if (rootCd->getOuterScope()!=Doxygen::globalScope)
    {
      DefinitionMutable *outerScope = toDefinitionMutable(rootCd->getOuterScope());
      if (outerScope)
      {
        outerScope->addInnerCompound(cd);
      }
    }

    FileDef *fd = templ->getFileDef();
    if (fd)
    {
      cd->setFileDef(fd);
      fd->insertClass(cd);
    }
    for (auto &gd : rootCd->partOfGroups())
    {
      cd->makePartOfGroup(gd);
      gd->addClass(cd);
    }

    MemberList *ml = templ->getMemberList(MemberListType::PubAttribs());
    if (ml)
    {
      for (const auto &md : *ml)
      {
        //printf("    Member %s type=%s\n",qPrint(md->name()),md->typeString());
        auto newMd = createMemberDef(md->getDefFileName(),md->getDefLine(),md->getDefColumn(),
            md->typeString(),md->name(),md->argsString(),md->excpString(),
            md->protection(),md->virtualness(),md->isStatic(),Relationship::Member,
            md->memberType(),
            ArgumentList(),ArgumentList(),"");
        MemberDefMutable *imd = toMemberDefMutable(newMd.get());
        imd->setMemberClass(cd);
        imd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
        imd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
        imd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
        imd->setMemberSpecifiers(md->getMemberSpecifiers());
        imd->setVhdlSpecifiers(md->getVhdlSpecifiers());
        imd->setMemberGroupId(md->getMemberGroupId());
        imd->setInitializer(md->initializer());
        imd->setRequiresClause(md->requiresClause());
        imd->setMaxInitLines(md->initializerLines());
        imd->setBitfields(md->bitfieldString());
        imd->setLanguage(md->getLanguage());
        cd->insertMember(imd);
        MemberName *mn = Doxygen::memberNameLinkedMap->add(md->name());
        mn->push_back(std::move(newMd));
      }
    }
  }
  return cd;
}

/** Look through the members of class \a cd and its public members.
 *  If there is a member m of a tag less struct/union,
 *  then we create a duplicate of the struct/union with the name of the
 *  member to identify it.
 *  So if cd has name S, then the tag less struct/union will get name S.m
 *  Since tag less structs can be nested we need to call this function
 *  recursively. Later on we need to patch the member types so we keep
 *  track of the hierarchy of classes we create.
 */
static void processTagLessClasses(const ClassDef *rootCd,
                                  const ClassDef *cd,
                                  ClassDefMutable *tagParentCd,
                                  const QCString &prefix,int count)
{
  //printf("%d: processTagLessClasses %s\n",count,qPrint(cd->name()));
  //printf("checking members for %s\n",qPrint(cd->name()));
  if (tagParentCd && !cd->getClasses().empty())
  {
    MemberList *ml = cd->getMemberList(MemberListType::PubAttribs());
    if (ml)
    {
      int pos=0;
      for (const auto &md : *ml)
      {
        QCString type = md->typeString();
        if (type.find("::@")!=-1) // member of tag less struct/union
        {
          for (const auto &icd : cd->getClasses())
          {
            //printf("  member %s: type='%s'\n",qPrint(md->name()),qPrint(type));
            //printf("  comparing '%s'<->'%s'\n",qPrint(type),qPrint(icd->name()));
            if (type.find(icd->name())!=-1) // matching tag less struct/union
            {
              QCString name = md->name();
              if (md->isAnonymous()) name = "__unnamed" + QCString().setNum(pos++)+"__";
              if (!prefix.isEmpty()) name.prepend(prefix+".");
              //printf("    found %s for class %s\n",qPrint(name),qPrint(cd->name()));
              ClassDefMutable *ncd = createTagLessInstance(rootCd,icd,name);
              if (ncd)
              {
                processTagLessClasses(rootCd,icd,ncd,name,count+1);
                //printf("    addTagged %s to %s\n",qPrint(ncd->name()),qPrint(tagParentCd->name()));
                ncd->setTagLessReference(icd);

                // replace tag-less type for generated/original member
                // by newly created class name.
                // note the difference between changing cd and tagParentCd.
                // for the initial call this is the same pointer, but for
                // recursive calls cd is the original tag-less struct (of which
                // there is only one instance) and tagParentCd is the newly
                // generated tagged struct of which there can be multiple instances!
                MemberList *pml = tagParentCd->getMemberList(MemberListType::PubAttribs());
                if (pml)
                {
                  for (const auto &pmd : *pml)
                  {
                    MemberDefMutable *pmdm = toMemberDefMutable(pmd);
                    if (pmdm && pmd->name()==md->name())
                    {
                      pmdm->setAccessorType(ncd,substitute(pmd->typeString(),icd->name(),ncd->name()));
                      //pmd->setType(substitute(pmd->typeString(),icd->name(),ncd->name()));
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

static void findTagLessClasses(std::vector<ClassDefMutable*> &candidates,ClassDef *cd)
{
  for (const auto &icd : cd->getClasses())
  {
    if (icd->name().find("@")==-1) // process all non-anonymous inner classes
    {
      findTagLessClasses(candidates,icd);
    }
  }

  ClassDefMutable *cdm = toClassDefMutable(cd);
  if (cdm)
  {
    candidates.push_back(cdm);
  }
}

static void findTagLessClasses()
{
  std::vector<ClassDefMutable *> candidates;
  for (auto &cd : *Doxygen::classLinkedMap)
  {
    Definition *scope = cd->getOuterScope();
    if (scope && scope->definitionType()!=Definition::TypeClass) // that is not nested
    {
      findTagLessClasses(candidates,cd.get());
    }
  }

  // since processTagLessClasses is potentially adding classes to Doxygen::classLinkedMap
  // we need to call it outside of the loop above, otherwise the iterator gets invalidated!
  for (auto &cd : candidates)
  {
    processTagLessClasses(cd,cd,cd,"",0); // process tag less inner struct/classes
  }
}


//----------------------------------------------------------------------
// build a list of all namespaces mentioned in the documentation
// and all namespaces that have a documentation block before their definition.
static void buildNamespaceList(const Entry *root)
{
  if (
       (root->section.isNamespace() ||
        root->section.isNamespaceDoc() ||
        root->section.isPackageDoc()
       ) &&
       !root->name.isEmpty()
     )
  {
    AUTO_TRACE("name={}",root->name);

    QCString fName = root->name;
    if (root->section.isPackageDoc())
    {
      fName=substitute(fName,".","::");
    }

    QCString fullName = stripAnonymousNamespaceScope(fName);
    if (!fullName.isEmpty())
    {
      AUTO_TRACE_ADD("Found namespace {} in {} at line {}",root->name,root->fileName,root->startLine);
      NamespaceDef *ndi = Doxygen::namespaceLinkedMap->find(fullName);
      if (ndi) // existing namespace
      {
        NamespaceDefMutable *nd = toNamespaceDefMutable(ndi);
        if (nd) // non-inline namespace
        {
          AUTO_TRACE_ADD("Existing namespace");
          nd->setDocumentation(root->doc,root->docFile,root->docLine);
          nd->setName(fullName); // change name to match docs
          nd->addSectionsToDefinition(root->anchors);
          nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          if (nd->getLanguage()==SrcLangExt::Unknown)
          {
            nd->setLanguage(root->lang);
          }
          if (root->tagInfo()==nullptr && nd->isReference() && !(root->doc.isEmpty() && root->brief.isEmpty()))
            // if we previously found namespace nd in a tag file and now we find a
            // documented namespace with the same name in the project, then remove
            // the tag file reference
          {
            nd->setReference("");
            nd->setFileName(fullName);
          }
          nd->setMetaData(root->metaData);

          // file definition containing the namespace nd
          FileDef *fd=root->fileDef();
          if (nd->isArtificial())
          {
            nd->setArtificial(FALSE); // found namespace explicitly, so cannot be artificial
            nd->setDefFile(root->fileName,root->startLine,root->startColumn);
          }
          // insert the namespace in the file definition
          if (fd) fd->insertNamespace(nd);
          addNamespaceToGroups(root,nd);
          nd->setRefItems(root->sli);
        }
      }
      else // fresh namespace
      {
        QCString tagName;
        QCString tagFileName;
        const TagInfo *tagInfo = root->tagInfo();
        if (tagInfo)
        {
          tagName     = tagInfo->tagName;
          tagFileName = tagInfo->fileName;
        }
        AUTO_TRACE_ADD("new namespace {} lang={} tagName={}",fullName,langToString(root->lang),tagName);
        // add namespace to the list
        NamespaceDefMutable *nd = toNamespaceDefMutable(
            Doxygen::namespaceLinkedMap->add(fullName,
              createNamespaceDef(tagInfo?tagName:root->fileName,root->startLine,
                root->startColumn,fullName,tagName,tagFileName,
                root->type,root->spec.isPublished())));
        if (nd)
        {
          nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
          nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          nd->addSectionsToDefinition(root->anchors);
          nd->setHidden(root->hidden);
          nd->setArtificial(root->artificial);
          nd->setLanguage(root->lang);
          nd->setId(root->id);
          nd->setMetaData(root->metaData);
          nd->setInline(root->spec.isInline());
          nd->setExported(root->exported);

          addNamespaceToGroups(root,nd);
          nd->setRefItems(root->sli);

          // file definition containing the namespace nd
          FileDef *fd=root->fileDef();
          // insert the namespace in the file definition
          if (fd) fd->insertNamespace(nd);

          // the empty string test is needed for extract all case
          nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          nd->insertUsedFile(fd);
          nd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
          nd->setBodyDef(fd);

          // also add namespace to the correct structural context
          Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,fullName,nullptr,tagInfo);
          AUTO_TRACE_ADD("adding namespace {} to context {}",nd->name(),d ? d->name() : QCString("<none>"));
          if (d==nullptr) // we didn't find anything, create the scope artificially
            // anyway, so we can at least relate scopes properly.
          {
            d = buildScopeFromQualifiedName(fullName,nd->getLanguage(),tagInfo);
            DefinitionMutable *dm = toDefinitionMutable(d);
            if (dm)
            {
              dm->addInnerCompound(nd);
            }
            nd->setOuterScope(d);
            // TODO: Due to the order in which the tag file is written
            // a nested class can be found before its parent!
          }
          else
          {
            DefinitionMutable *dm = toDefinitionMutable(d);
            if (dm)
            {
              dm->addInnerCompound(nd);
            }
            nd->setOuterScope(d);
            // in case of d is an inline namespace, alias insert nd in the part scope of d.
            while (d->definitionType()==Definition::TypeNamespace)
            {
              NamespaceDef *pnd = toNamespaceDef(d);
              if (pnd && pnd->isInline())
              {
                d = d->getOuterScope();
                if (d)
                {
                  dm = toDefinitionMutable(d);
                  if (dm)
                  {
                    auto aliasNd = createNamespaceDefAlias(d,nd);
                    dm->addInnerCompound(aliasNd.get());
                    QCString aliasName = aliasNd->name();
                    AUTO_TRACE_ADD("adding alias {} to {}",aliasName,d->name());
                    Doxygen::namespaceLinkedMap->add(aliasName,std::move(aliasNd));
                  }
                }
                else
                {
                  break;
                }
              }
              else
              {
                break;
              }
            }
          }
        }
      }
    }
  }
  for (const auto &e : root->children()) buildNamespaceList(e.get());
}

//----------------------------------------------------------------------

static NamespaceDef *findUsedNamespace(const LinkedRefMap<NamespaceDef> &unl,
                              const QCString &name)
{
  NamespaceDef *usingNd =nullptr;
  for (auto &und : unl)
  {
    QCString uScope=und->name()+"::";
    usingNd = getResolvedNamespace(uScope+name);
    if (usingNd!=nullptr) break;
  }
  return usingNd;
}

static void findUsingDirectives(const Entry *root)
{
  if (root->section.isUsingDir())
  {
    AUTO_TRACE("Found using directive {} at line {} of {}",root->name,root->startLine,root->fileName);
    QCString name=substitute(root->name,".","::");
    if (name.endsWith("::"))
    {
      name=name.left(name.length()-2);
    }
    if (!name.isEmpty())
    {
      NamespaceDef *usingNd = nullptr;
      NamespaceDefMutable *nd = nullptr;
      FileDef      *fd = root->fileDef();
      QCString nsName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (root->parent() && root->parent()->section.isNamespace() &&
          (fd==nullptr || fd->getLanguage()!=SrcLangExt::Java) // not a .java file
         )
      {
        nsName=stripAnonymousNamespaceScope(root->parent()->name);
        if (!nsName.isEmpty())
        {
          nd = getResolvedNamespaceMutable(nsName);
        }
      }

      // find the scope in which the 'using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e.
      // file scope).
      int scopeOffset = static_cast<int>(nsName.length());
      do
      {
        QCString scope=scopeOffset>0 ?
                      nsName.left(scopeOffset)+"::" : QCString();
        usingNd = getResolvedNamespace(scope+name);
        //printf("Trying with scope='%s' usingNd=%p\n",(scope+qPrint(name)),usingNd);
        if (scopeOffset==0)
        {
          scopeOffset=-1;
        }
        else if ((scopeOffset=nsName.findRev("::",scopeOffset-1))==-1)
        {
          scopeOffset=0;
        }
      } while (scopeOffset>=0 && usingNd==nullptr);

      if (usingNd==nullptr && nd) // not found, try used namespaces in this scope
                            // or in one of the parent namespace scopes
      {
        const NamespaceDefMutable *pnd = nd;
        while (pnd && usingNd==nullptr)
        {
          // also try with one of the used namespaces found earlier
          usingNd = toNamespaceDefMutable(findUsedNamespace(pnd->getUsedNamespaces(),name));

          // goto the parent
          Definition *s = pnd->getOuterScope();
          if (s && s->definitionType()==Definition::TypeNamespace)
          {
            pnd = toNamespaceDefMutable(toNamespaceDef(s));
          }
          else
          {
            pnd = nullptr;
          }
        }
      }
      if (usingNd==nullptr && fd) // still nothing, also try used namespace in the
                            // global scope
      {
        usingNd = findUsedNamespace(fd->getUsedNamespaces(),name);
      }

      //printf("%s -> %s\n",qPrint(name),usingNd?qPrint(usingNd->name()):"<none>");

      // add the namespace the correct scope
      if (usingNd)
      {
        //printf("using fd=%p nd=%p\n",fd,nd);
        if (nd)
        {
          //printf("Inside namespace %s\n",qPrint(nd->name()));
          nd->addUsingDirective(usingNd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",qPrint(fd->name()));
          fd->addUsingDirective(usingNd);
        }
      }
      else // unknown namespace, but add it anyway.
      {
        AUTO_TRACE_ADD("new unknown namespace {} lang={} hidden={}",name,langToString(root->lang),root->hidden);
        // add namespace to the list
        nd = toNamespaceDefMutable(
            Doxygen::namespaceLinkedMap->add(name,
               createNamespaceDef(root->fileName,root->startLine,root->startColumn,name)));
        if (nd)
        {
          nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
          nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          nd->addSectionsToDefinition(root->anchors);
          nd->setHidden(root->hidden);
          nd->setArtificial(TRUE);
          nd->setLanguage(root->lang);
          nd->setId(root->id);
          nd->setMetaData(root->metaData);
          nd->setInline(root->spec.isInline());
          nd->setExported(root->exported);

          for (const Grouping &g : root->groups)
          {
            GroupDef *gd=nullptr;
            if (!g.groupname.isEmpty() && (gd=Doxygen::groupLinkedMap->find(g.groupname)))
              gd->addNamespace(nd);
          }

          // insert the namespace in the file definition
          if (fd)
          {
            fd->insertNamespace(nd);
            fd->addUsingDirective(nd);
          }

          // the empty string test is needed for extract all case
          nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          nd->insertUsedFile(fd);
          nd->setRefItems(root->sli);
        }
      }
    }
  }
  for (const auto &e : root->children()) findUsingDirectives(e.get());
}

//----------------------------------------------------------------------

static void buildListOfUsingDecls(const Entry *root)
{
  if (root->section.isUsingDecl() &&
      !root->parent()->section.isCompound() // not a class/struct member
     )
  {
    QCString name = substitute(root->name,".","::");
    g_usingDeclarations.insert(name.str());
  }
  for (const auto &e : root->children()) buildListOfUsingDecls(e.get());
}


static void findUsingDeclarations(const Entry *root,bool filterPythonPackages)
{
  if (root->section.isUsingDecl() &&
      !root->parent()->section.isCompound() && // not a class/struct member
      (!filterPythonPackages || (root->lang==SrcLangExt::Python && root->fileName.endsWith("__init__.py")))
     )
  {
    AUTO_TRACE("Found using declaration '{}' at line {} of {} inside section {}",
       root->name,root->startLine,root->fileName,root->parent()->section);
    if (!root->name.isEmpty())
    {
      const Definition *usingDef = nullptr;
      NamespaceDefMutable *nd = nullptr;
      FileDef      *fd = root->fileDef();
      QCString scName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (root->parent()->section.isNamespace())
      {
        scName=root->parent()->name;
        if (!scName.isEmpty())
        {
          nd = getResolvedNamespaceMutable(scName);
        }
      }

      // Assume the using statement was used to import a class.
      // Find the scope in which the 'using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e.
      // file scope).

      QCString name = substitute(root->name,".","::"); //Java/C# scope->internal

      SymbolResolver resolver;
      const Definition *scope = nd;
      if (nd==nullptr) scope = fd;
      usingDef = resolver.resolveSymbol(scope,name);

      //printf("usingDef(scope=%s,name=%s)=%s\n",qPrint(nd?nd->qualifiedName():""),qPrint(name),usingDef?qPrint(usingDef->qualifiedName()):"nullptr");

      if (!usingDef)
      {
        usingDef = getClass(name); // try direct lookup, this is needed to get
                                   // builtin STL classes to properly resolve, e.g.
                                   // vector -> std::vector
      }
      if (!usingDef)
      {
        usingDef = Doxygen::hiddenClassLinkedMap->find(name); // check if it is already hidden
      }
#if 0
      if (!usingDef)
      {
        AUTO_TRACE_ADD("New using class '{}' (sec={})! #tArgLists={}",
             name,root->section,root->tArgLists.size());
        ClassDefMutable *usingCd = toClassDefMutable(
             Doxygen::hiddenClassLinkedMap->add(name,
               createClassDef( "<using>",1,1, name, ClassDef::Class)));
        if (usingCd)
        {
          usingCd->setArtificial(TRUE);
          usingCd->setLanguage(root->lang);
          usingDef = usingCd;
        }
      }
#endif
      else
      {
        AUTO_TRACE_ADD("Found used type '{}' in scope='{}'",
            usingDef->name(), nd ? nd->name(): fd ? fd->name() : QCString("<unknown>"));
      }

      if (usingDef)
      {
        if (nd)
        {
          nd->addUsingDeclaration(usingDef);
        }
        else if (fd)
        {
          fd->addUsingDeclaration(usingDef);
        }
      }
    }
  }
  for (const auto &e : root->children()) findUsingDeclarations(e.get(),filterPythonPackages);
}

//----------------------------------------------------------------------

static void applyMemberOverrideOptions(const Entry *root,MemberDefMutable *md)
{
  root->commandOverrides.apply_callGraph           ([&](bool b) { md->overrideCallGraph(b);            });
  root->commandOverrides.apply_callerGraph         ([&](bool b) { md->overrideCallerGraph(b);          });
  root->commandOverrides.apply_referencedByRelation([&](bool b) { md->overrideReferencedByRelation(b); });
  root->commandOverrides.apply_referencesRelation  ([&](bool b) { md->overrideReferencesRelation(b);   });
  root->commandOverrides.apply_inlineSource        ([&](bool b) { md->overrideInlineSource(b);         });
  root->commandOverrides.apply_enumValues          ([&](bool b) { md->overrideEnumValues(b);           });
}

//----------------------------------------------------------------------

static void findUsingDeclImports(const Entry *root)
{
  if (root->section.isUsingDecl() &&
      root->parent()->section.isCompound() // in a class/struct member
     )
  {
    AUTO_TRACE("Found using declaration '{}' inside section {}", root->name, root->parent()->section);
    QCString fullName=removeRedundantWhiteSpace(root->parent()->name);
    fullName=stripAnonymousNamespaceScope(fullName);
    fullName=stripTemplateSpecifiersFromScope(fullName);
    ClassDefMutable *cd = getClassMutable(fullName);
    if (cd)
    {
      AUTO_TRACE_ADD("found class '{}'",cd->name());
      int i=root->name.findRev("::");
      if (i!=-1)
      {
        QCString scope=root->name.left(i);
        QCString memName=root->name.right(root->name.length()-i-2);
        SymbolResolver resolver;
        const ClassDef *bcd = resolver.resolveClass(cd,scope); // todo: file in fileScope parameter
        AUTO_TRACE_ADD("name={} scope={} bcd={}",scope,cd?cd->name():"<none>",bcd?bcd->name():"<none>");
        if (bcd && bcd!=cd)
        {
          AUTO_TRACE_ADD("found class '{}' memName='{}'",bcd->name(),memName);
          const MemberNameInfoLinkedMap &mnlm=bcd->memberNameInfoLinkedMap();
          const MemberNameInfo *mni = mnlm.find(memName);
          if (mni)
          {
            for (auto &mi : *mni)
            {
              const MemberDef *md = mi->memberDef();
              if (md && md->protection()!=Protection::Private)
              {
                AUTO_TRACE_ADD("found member '{}'",mni->memberName());
                QCString fileName = root->fileName;
                if (fileName.isEmpty() && root->tagInfo())
                {
                  fileName = root->tagInfo()->tagName;
                }
                const ArgumentList &templAl = md->templateArguments();
                const ArgumentList &al = md->argumentList();

                if (!cd->containsOverload(md))
                {
                  AUTO_TRACE_ADD("creating new member {} for class {}",memName,cd->name());
                  auto newMd = createMemberDef(
                      fileName,root->startLine,root->startColumn,
                      md->typeString(),memName,md->argsString(),
                      md->excpString(),root->protection,root->virt,
                      md->isStatic(),Relationship::Member,md->memberType(),
                      templAl,al,root->metaData
                      );
                  auto newMmd = toMemberDefMutable(newMd.get());
                  newMmd->setMemberClass(cd);
                  cd->insertMember(newMd.get());
                  if (!root->doc.isEmpty() || !root->brief.isEmpty())
                  {
                    newMmd->setDocumentation(root->doc,root->docFile,root->docLine);
                    newMmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
                    newMmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                  }
                  else
                  {
                    newMmd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
                    newMmd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
                    newMmd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
                  }
                  newMmd->setDefinition(md->definition());
                  applyMemberOverrideOptions(root,newMmd);
                  newMmd->addQualifiers(root->qualifiers);
                  newMmd->setBitfields(md->bitfieldString());
                  newMmd->addSectionsToDefinition(root->anchors);
                  newMmd->setBodySegment(md->getDefLine(),md->getStartBodyLine(),md->getEndBodyLine());
                  newMmd->setBodyDef(md->getBodyDef());
                  newMmd->setInitializer(md->initializer());
                  newMmd->setRequiresClause(md->requiresClause());
                  newMmd->setMaxInitLines(md->initializerLines());
                  newMmd->setMemberGroupId(root->mGrpId);
                  newMmd->setMemberSpecifiers(md->getMemberSpecifiers());
                  newMmd->setVhdlSpecifiers(md->getVhdlSpecifiers());
                  newMmd->setLanguage(root->lang);
                  newMmd->setId(root->id);
                  MemberName *mn = Doxygen::memberNameLinkedMap->add(memName);
                  mn->push_back(std::move(newMd));
                }
              }
            }
          }
        }
      }
    }
  }
  else if (root->section.isUsingDecl() &&
           (root->parent()->section.isNamespace() || root->parent()->section.isEmpty()) && // namespace or global member
           root->lang==SrcLangExt::Cpp                                                     // do we also want this for e.g. Fortran? (see test case 095)
          )
  {
    AUTO_TRACE("Found using declaration '{}' inside section {}", root->name, root->parent()->section);
    Definition *scope = nullptr;
    NamespaceDefMutable *nd = nullptr;
    FileDef *fd = root->parent()->fileDef();
    if (!root->parent()->name.isEmpty())
    {
      QCString fullName=removeRedundantWhiteSpace(root->parent()->name);
      fullName=stripAnonymousNamespaceScope(fullName);
      nd = toNamespaceDefMutable(getResolvedNamespace(fullName));
      scope = nd;
    }
    else
    {
      scope = fd;
    }
    if (scope)
    {
      AUTO_TRACE_ADD("found scope '{}'",scope->name());
      SymbolResolver resolver;
      const Definition *def = resolver.resolveSymbol(root->name.startsWith("::") ? nullptr : scope,root->name);
      if (def && def->definitionType()==Definition::TypeMember)
      {
        int i=root->name.find("::");
        QCString memName;
        if (i!=-1)
        {
          memName = root->name.right(root->name.length()-i-2);
        }
        else
        {
          memName = root->name;
        }
        const MemberDef *md = toMemberDef(def);
        AUTO_TRACE_ADD("found member '{}' for name '{}'",md->qualifiedName(),root->name);
        QCString fileName = root->fileName;
        if (fileName.isEmpty() && root->tagInfo())
        {
          fileName = root->tagInfo()->tagName;
        }
        const ArgumentList &templAl = md->templateArguments();
        const ArgumentList &al = md->argumentList();

        auto newMd = createMemberDef(
            fileName,root->startLine,root->startColumn,
            md->typeString(),memName,md->argsString(),
            md->excpString(),root->protection,root->virt,
            md->isStatic(),Relationship::Member,md->memberType(),
            templAl,al,root->metaData
            );
        auto newMmd = toMemberDefMutable(newMd.get());
        if (nd)
        {
          newMmd->setNamespace(nd);
          nd->insertMember(newMd.get());
        }
        if (fd)
        {
          newMmd->setFileDef(fd);
          fd->insertMember(newMd.get());
        }
        if (!root->doc.isEmpty() || !root->brief.isEmpty())
        {
          newMmd->setDocumentation(root->doc,root->docFile,root->docLine);
          newMmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          newMmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
        }
        else
        {
          newMmd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
          newMmd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
          newMmd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
        }
        newMmd->setDefinition(md->definition());
        applyMemberOverrideOptions(root,newMmd);
        newMmd->addQualifiers(root->qualifiers);
        newMmd->setBitfields(md->bitfieldString());
        newMmd->addSectionsToDefinition(root->anchors);
        newMmd->setBodySegment(md->getDefLine(),md->getStartBodyLine(),md->getEndBodyLine());
        newMmd->setBodyDef(md->getBodyDef());
        newMmd->setInitializer(md->initializer());
        newMmd->setRequiresClause(md->requiresClause());
        newMmd->setMaxInitLines(md->initializerLines());
        newMmd->setMemberGroupId(root->mGrpId);
        newMmd->setMemberSpecifiers(md->getMemberSpecifiers());
        newMmd->setVhdlSpecifiers(md->getVhdlSpecifiers());
        newMmd->setLanguage(root->lang);
        newMmd->setId(root->id);
        MemberName *mn = Doxygen::functionNameLinkedMap->add(memName);
        mn->push_back(std::move(newMd));
#if 0 // insert an alias instead of a copy
        const MemberDef *md = toMemberDef(def);
        AUTO_TRACE_ADD("found member '{}' for name '{}'",md->qualifiedName(),root->name);
        auto aliasMd = createMemberDefAlias(nd,md);
        QCString aliasFullName = nd->qualifiedName()+"::"+aliasMd->localName();
        if (nd && aliasMd.get())
        {
          nd->insertMember(aliasMd.get());
        }
        if (fd && aliasMd.get())
        {
          fd->insertMember(aliasMd.get());
        }
        MemberName *mn = Doxygen::memberNameLinkedMap->add(aliasFullName);
        mn->push_back(std::move(aliasMd));
#endif
      }
      else if (def && def->definitionType()==Definition::TypeClass)
      {
        const ClassDef *cd = toClassDef(def);
        QCString copyFullName;
        if (nd==nullptr)
        {
          copyFullName = cd->localName();
        }
        else
        {
          copyFullName = nd->qualifiedName()+"::"+cd->localName();
        }
        if (Doxygen::classLinkedMap->find(copyFullName)==nullptr)
        {
          ClassDefMutable *ncdm = toClassDefMutable(
              Doxygen::classLinkedMap->add(copyFullName,
                cd->deepCopy(copyFullName)));
          AUTO_TRACE_ADD("found class '{}' for name '{}' copy '{}' obj={}",cd->qualifiedName(),root->name,copyFullName,(void*)ncdm);
          if (ncdm)
          {
            if (nd) ncdm->moveTo(nd);
            if ((!root->doc.isEmpty() || !root->brief.isEmpty())) // use docs at using statement
            {
              ncdm->setDocumentation(root->doc,root->docFile,root->docLine);
              ncdm->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            }
            else // use docs from used class
            {
              ncdm->setDocumentation(cd->documentation(),cd->docFile(),cd->docLine());
              ncdm->setBriefDescription(cd->briefDescription(),cd->briefFile(),cd->briefLine());
            }
            if (nd)
            {
              nd->addInnerCompound(ncdm);
              nd->addUsingDeclaration(ncdm);
            }
            if (fd)
            {
              if (ncdm) ncdm->setFileDef(fd);
              fd->insertClass(ncdm);
              fd->addUsingDeclaration(ncdm);
            }
          }
        }
#if 0 // insert an alias instead of a copy
        auto aliasCd = createClassDefAlias(nd,cd);
        QCString aliasFullName;
        if (nd==nullptr)
        {
          aliasFullName = aliasCd->localName();
        }
        else
        {
          aliasFullName = nd->qualifiedName()+"::"+aliasCd->localName();
        }
        AUTO_TRACE_ADD("found class '{}' for name '{}' aliasFullName='{}'",cd->qualifiedName(),root->name,aliasFullName);
        auto acd = Doxygen::classLinkedMap->add(aliasFullName,std::move(aliasCd));
        if (nd && acd)
        {
          nd->addInnerCompound(acd);
        }
        if (fd && acd)
        {
          fd->insertClass(acd);
        }
#endif
      }
      else if (scope)
      {
        AUTO_TRACE_ADD("no symbol with name '{}' in scope {}",root->name,scope->name());
      }
    }
  }
  for (const auto &e : root->children()) findUsingDeclImports(e.get());
}

//----------------------------------------------------------------------

static void findIncludedUsingDirectives()
{
  FileDefSet visitedFiles;
  // then recursively add using directives found in #include files
  // to files that have not been visited.
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      //printf("----- adding using directives for file %s\n",qPrint(fd->name()));
      fd->addIncludedUsingDirectives(visitedFiles);
    }
  }
}

//----------------------------------------------------------------------

static MemberDef *addVariableToClass(
    const Entry *root,
    ClassDefMutable *cd,
    MemberType mtype,
    const QCString &type,
    const QCString &name,
    const QCString &args,
    bool fromAnnScope,
    MemberDef *fromAnnMemb,
    Protection prot,
    Relationship related)
{
  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  QCString scopeSeparator="::";
  SrcLangExt lang = cd->getLanguage();
  if (lang==SrcLangExt::Java || lang==SrcLangExt::CSharp)
  {
    qualScope = substitute(qualScope,"::",".");
    scopeSeparator=".";
  }
  AUTO_TRACE("class variable: file='{}' type='{}' scope='{}' name='{}' args='{}' prot={} mtype={} lang={} ann={} init='{}'",
      root->fileName, type, qualScope, name, args, root->protection, mtype, lang, fromAnnScope, root->initializer.str());

  QCString def;
  if (!type.isEmpty())
  {
    if (related!=Relationship::Member || mtype==MemberType::Friend || Config_getBool(HIDE_SCOPE_NAMES))
    {
      if (root->spec.isAlias()) // turn 'typedef B A' into 'using A'
      {
        def="using "+name;
      }
      else
      {
        def=type+" "+name+args;
      }
    }
    else
    {
      if (root->spec.isAlias()) // turn 'typedef B C::A' into 'using C::A'
      {
        def="using "+qualScope+scopeSeparator+name;
      }
      else
      {
        def=type+" "+qualScope+scopeSeparator+name+args;
      }
    }
  }
  else
  {
    if (Config_getBool(HIDE_SCOPE_NAMES))
    {
      def=name+args;
    }
    else
    {
      def=qualScope+scopeSeparator+name+args;
    }
  }
  def.stripPrefix("static ");

  // see if the member is already found in the same scope
  // (this may be the case for a static member that is initialized
  //  outside the class)
  MemberName *mn=Doxygen::memberNameLinkedMap->find(name);
  if (mn)
  {
    for (const auto &imd : *mn)
    {
      //printf("md->getClassDef()=%p cd=%p type=[%s] md->typeString()=[%s]\n",
      //    md->getClassDef(),cd,qPrint(type),md->typeString());
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md &&
          md->getClassDef()==cd &&
          ((lang==SrcLangExt::Python && type.isEmpty() && !md->typeString().isEmpty()) ||
          removeRedundantWhiteSpace(type)==md->typeString()))
        // member already in the scope
      {

        if (root->lang==SrcLangExt::ObjC &&
            root->mtype==MethodTypes::Property &&
            md->memberType()==MemberType::Variable)
        { // Objective-C 2.0 property
          // turn variable into a property
          md->setProtection(root->protection);
          cd->reclassifyMember(md,MemberType::Property);
        }
        addMemberDocs(root,md,def,nullptr,FALSE,root->spec);
        AUTO_TRACE_ADD("Member already found!");
        return md;
      }
    }
  }

  QCString fileName = root->fileName;
  if (fileName.isEmpty() && root->tagInfo())
  {
    fileName = root->tagInfo()->tagName;
  }

  // new member variable, typedef or enum value
  auto md = createMemberDef(
      fileName,root->startLine,root->startColumn,
      type,name,args,root->exception,
      prot,Specifier::Normal,root->isStatic,related,
      mtype,!root->tArgLists.empty() ? root->tArgLists.back() : ArgumentList(),
      ArgumentList(), root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  mmd->setTagInfo(root->tagInfo());
  mmd->setMemberClass(cd); // also sets outer scope (i.e. getOuterScope())
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->setDefinition(def);
  mmd->setBitfields(root->bitfields);
  mmd->addSectionsToDefinition(root->anchors);
  mmd->setFromAnonymousScope(fromAnnScope);
  mmd->setFromAnonymousMember(fromAnnMemb);
  //md->setIndentDepth(indentDepth);
  mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
  std::string init = root->initializer.str();
  mmd->setInitializer(init.c_str());
  mmd->setMaxInitLines(root->initLines);
  mmd->setMemberGroupId(root->mGrpId);
  mmd->setMemberSpecifiers(root->spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setReadAccessor(root->read);
  mmd->setWriteAccessor(root->write);
  applyMemberOverrideOptions(root,mmd);
  mmd->setHidden(root->hidden);
  mmd->setArtificial(root->artificial);
  mmd->setLanguage(root->lang);
  mmd->setId(root->id);
  addMemberToGroups(root,md.get());
  ModuleManager::instance().addMemberToModule(root,md.get());
  mmd->setBodyDef(root->fileDef());
  mmd->addQualifiers(root->qualifiers);

  AUTO_TRACE_ADD("Adding new member to class '{}'",cd->name());
  cd->insertMember(md.get());
  mmd->setRefItems(root->sli);

  cd->insertUsedFile(root->fileDef());
  root->markAsProcessed();

  // add the member to the global list
  MemberDef *result = md.get();
  mn = Doxygen::memberNameLinkedMap->add(name);
  mn->push_back(std::move(md));

  return result;
}

//----------------------------------------------------------------------

static MemberDef *addVariableToFile(
    const Entry *root,
    MemberType mtype,
    const QCString &scope,
    const QCString &type,
    const QCString &name,
    const QCString &args,
    bool fromAnnScope,
    MemberDef *fromAnnMemb)
{
  AUTO_TRACE("global variable: file='{}' type='{}' scope='{}' name='{}' args='{}' prot={} mtype={} lang={} init='{}'",
      root->fileName, type, scope, name, args, root->protection, mtype, root->lang, root->initializer.str());

  FileDef *fd = root->fileDef();

  // see if we have a typedef that should hide a struct or union
  if (mtype==MemberType::Typedef && Config_getBool(TYPEDEF_HIDES_STRUCT))
  {
    QCString ttype = type;
    ttype.stripPrefix("typedef ");
    if (ttype.stripPrefix("struct ") || ttype.stripPrefix("union "))
    {
      static const reg::Ex re(R"(\a\w*)");
      reg::Match match;
      std::string typ = ttype.str();
      if (reg::search(typ,match,re))
      {
        QCString typeValue = match.str();
        ClassDefMutable *cd = getClassMutable(typeValue);
        if (cd)
        {
          // this typedef should hide compound name cd, so we
          // change the name that is displayed from cd.
          cd->setClassName(name);
          cd->setDocumentation(root->doc,root->docFile,root->docLine);
          cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          return nullptr;
        }
      }
    }
  }

  // see if the function is inside a namespace
  NamespaceDefMutable *nd = nullptr;
  if (!scope.isEmpty())
  {
    if (scope.find('@')!=-1) return nullptr; // anonymous scope!
    nd = getResolvedNamespaceMutable(scope);
  }
  QCString def;

  // determine the definition of the global variable
  if (nd && !nd->isAnonymous() &&
      !Config_getBool(HIDE_SCOPE_NAMES)
     )
    // variable is inside a namespace, so put the scope before the name
  {
    SrcLangExt lang = nd->getLanguage();
    QCString sep=getLanguageSpecificSeparator(lang);

    if (!type.isEmpty())
    {
      if (root->spec.isAlias()) // turn 'typedef B NS::A' into 'using NS::A'
      {
        def="using "+nd->name()+sep+name;
      }
      else // normal member
      {
        def=type+" "+nd->name()+sep+name+args;
      }
    }
    else
    {
      def=nd->name()+sep+name+args;
    }
  }
  else
  {
    if (!type.isEmpty() && !root->name.isEmpty())
    {
      if (name.at(0)=='@') // dummy variable representing anonymous union
      {
        def=type;
      }
      else
      {
        if (root->spec.isAlias()) // turn 'typedef B A' into 'using A'
        {
          def="using "+root->name;
        }
        else // normal member
        {
          def=type+" "+name+args;
        }
      }
    }
    else
    {
      def=name+args;
    }
  }
  def.stripPrefix("static ");

  MemberName *mn=Doxygen::functionNameLinkedMap->find(name);
  if (mn)
  {
    //QCString nscope=removeAnonymousScopes(scope);
    //NamespaceDef *nd=nullptr;
    //if (!nscope.isEmpty())
    if (!scope.isEmpty())
    {
      nd = getResolvedNamespaceMutable(scope);
    }
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md &&
          ((nd==nullptr && md->getNamespaceDef()==nullptr && md->getFileDef() &&
            root->fileName==md->getFileDef()->absFilePath()
           ) // both variable names in the same file
           || (nd!=nullptr && md->getNamespaceDef()==nd) // both in same namespace
          )
          && !md->isDefine() // function style #define's can be "overloaded" by typedefs or variables
          && !md->isEnumerate() // in C# an enum value and enum can have the same name
         )
        // variable already in the scope
      {
        bool isPHPArray = md->getLanguage()==SrcLangExt::PHP &&
                          md->argsString()!=args &&
                          args.find('[')!=-1;
        bool staticsInDifferentFiles =
                          root->isStatic && md->isStatic() &&
                          root->fileName!=md->getDefFileName();

        if (md->getFileDef() &&
            !isPHPArray && // not a php array
            !staticsInDifferentFiles
           )
          // not a php array variable
        {
          AUTO_TRACE_ADD("variable already found: scope='{}'",md->getOuterScope()->name());
          addMemberDocs(root,md,def,nullptr,FALSE,root->spec);
          md->setRefItems(root->sli);
          // if md is a variable forward declaration and root is the definition that
          // turn md into the definition
          if (!root->explicitExternal && md->isExternal())
          {
            md->setDeclFile(md->getDefFileName(),md->getDefLine(),md->getDefColumn());
            md->setExplicitExternal(FALSE,root->fileName,root->startLine,root->startColumn);
          }
          // if md is the definition and root point at a declaration, then add the
          // declaration info
          else if (root->explicitExternal && !md->isExternal())
          {
            md->setDeclFile(root->fileName,root->startLine,root->startColumn);
          }
          return md;
        }
      }
    }
  }

  QCString fileName = root->fileName;
  if (fileName.isEmpty() && root->tagInfo())
  {
    fileName = root->tagInfo()->tagName;
  }

  AUTO_TRACE_ADD("new variable, namespace='{}'",nd?nd->name():QCString("<global>"));
  // new global variable, enum value or typedef
  auto md =  createMemberDef(
      fileName,root->startLine,root->startColumn,
      type,name,args,QCString(),
      root->protection, Specifier::Normal,root->isStatic,Relationship::Member,
      mtype,!root->tArgLists.empty() ? root->tArgLists.back() : ArgumentList(),
      root->argList, root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  mmd->setTagInfo(root->tagInfo());
  mmd->setMemberSpecifiers(root->spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->addSectionsToDefinition(root->anchors);
  mmd->setFromAnonymousScope(fromAnnScope);
  mmd->setFromAnonymousMember(fromAnnMemb);
  std::string init = root->initializer.str();
  mmd->setInitializer(init.c_str());
  mmd->setMaxInitLines(root->initLines);
  mmd->setMemberGroupId(root->mGrpId);
  mmd->setDefinition(def);
  mmd->setLanguage(root->lang);
  mmd->setId(root->id);
  applyMemberOverrideOptions(root,mmd);
  mmd->setExplicitExternal(root->explicitExternal,fileName,root->startLine,root->startColumn);
  mmd->addQualifiers(root->qualifiers);
  //md->setOuterScope(fd);
  if (!root->explicitExternal)
  {
    mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
    mmd->setBodyDef(fd);
  }
  addMemberToGroups(root,md.get());
  ModuleManager::instance().addMemberToModule(root,md.get());

  mmd->setRefItems(root->sli);
  if (nd && !nd->isAnonymous())
  {
    mmd->setNamespace(nd);
    nd->insertMember(md.get());
  }

  // add member to the file (we do this even if we have already inserted
  // it into the namespace.
  if (fd)
  {
    mmd->setFileDef(fd);
    fd->insertMember(md.get());
  }

  root->markAsProcessed();

  // add member definition to the list of globals
  MemberDef *result = md.get();
  mn = Doxygen::functionNameLinkedMap->add(name);
  mn->push_back(std::move(md));

  return result;
}

/*! See if the return type string \a type is that of a function pointer
 *  \returns -1 if this is not a function pointer variable or
 *           the index at which the closing brace of (...*name) was found.
 */
static int findFunctionPtr(const std::string &type,SrcLangExt lang, int *pLength=nullptr)
{
  AUTO_TRACE("type='{}' lang={}",type,lang);
  if (lang == SrcLangExt::Fortran || lang == SrcLangExt::VHDL)
  {
    return -1; // Fortran and VHDL do not have function pointers
  }

  static const reg::Ex re(R"(\([^)]*[*&^][^)]*\))");
  reg::Match match;
  size_t i=std::string::npos;
  size_t l=0;
  if (reg::search(type,match,re)) // contains (...*...) or (...&...) or (...^...)
  {
    i = match.position();
    l = match.length();
  }
  if (i!=std::string::npos)
  {
    size_t di = type.find("decltype(");
    if (di!=std::string::npos && di<i)
    {
      i = std::string::npos;
    }
  }
  size_t bb=type.find('<');
  size_t be=type.rfind('>');
  bool templFp = false;
  if (be!=std::string::npos) {
    size_t cc_ast = type.find("::*");
    size_t cc_amp = type.find("::&");
    templFp = (cc_ast != std::string::npos && cc_ast>be) || (cc_amp != std::string::npos && cc_amp>be); // hack to find, e.g 'B<X>(A<int>::*)'
  }

  if (!type.empty()                            &&  // return type is non-empty
      i!=std::string::npos                     &&   // contains (...*...)
      type.find("operator")==std::string::npos &&   // not an operator
      (type.find(")(")==std::string::npos || type.find("typedef ")!=std::string::npos) &&
                                                    // not a function pointer return type
      (!(bb<i && i<be) || templFp) // bug665855: avoid treating "typedef A<void (T*)> type" as a function pointer
     )
  {
    if (pLength) *pLength=static_cast<int>(l);
    //printf("findFunctionPtr=%d\n",(int)i);
    AUTO_TRACE_EXIT("result={}",i);
    return static_cast<int>(i);
  }
  else
  {
    //printf("findFunctionPtr=%d\n",-1);
    AUTO_TRACE_EXIT("result=-1");
    return -1;
  }
}


/*! Returns TRUE iff \a type is a class within scope \a context.
 *  Used to detect variable declarations that look like function prototypes.
 */
static bool isVarWithConstructor(const Entry *root)
{
  bool result      = false;
  bool typeIsClass = false;
  bool typePtrType = false;
  QCString type;
  Definition *ctx = nullptr;
  FileDef *fd = root->fileDef();
  SymbolResolver resolver(fd);

  AUTO_TRACE("isVarWithConstructor({})",root->name);
  if (root->parent()->section.isCompound())
  { // inside a class
    result=FALSE;
    AUTO_TRACE_EXIT("inside class: result={}",result);
    return result;
  }
  else if ((fd != nullptr) && (fd->name().endsWith(".c") || fd->name().endsWith(".h")))
  { // inside a .c file
    result=FALSE;
    AUTO_TRACE_EXIT("inside C file: result={}",result);
    return result;
  }
  if (root->type.isEmpty())
  {
    result=FALSE;
    AUTO_TRACE_EXIT("no type: result={}",result);
    return result;
  }
  if (!root->parent()->name.isEmpty())
  {
    ctx=Doxygen::namespaceLinkedMap->find(root->parent()->name);
  }
  type = root->type;
  // remove qualifiers
  findAndRemoveWord(type,"const");
  findAndRemoveWord(type,"static");
  findAndRemoveWord(type,"volatile");
  typePtrType = type.find('*')!=-1 || type.find('&')!=-1;
  if (!typePtrType)
  {
    typeIsClass = resolver.resolveClass(ctx,type)!=nullptr;
    int ti=0;
    if (!typeIsClass && (ti=type.find('<'))!=-1)
    {
      typeIsClass=resolver.resolveClass(ctx,type.left(ti))!=nullptr;
    }
  }
  if (typeIsClass) // now we still have to check if the arguments are
                   // types or values. Since we do not have complete type info
                   // we need to rely on heuristics :-(
  {
    if (root->argList.empty())
    {
      result=FALSE; // empty arg list -> function prototype.
      AUTO_TRACE_EXIT("empty arg list: result={}",result);
      return result;
    }
    for (const Argument &a : root->argList)
    {
      static const reg::Ex initChars(R"([\d"'&*!^]+)");
      reg::Match match;
      if (!a.name.isEmpty() || !a.defval.isEmpty())
      {
        std::string name = a.name.str();
        if (reg::search(name,match,initChars) && match.position()==0)
        {
          result=TRUE;
        }
        else
        {
          result=FALSE; // arg has (type,name) pair -> function prototype
        }
        AUTO_TRACE_EXIT("function prototype: result={}",result);
        return result;
      }
      if (!a.type.isEmpty() &&
          (a.type.at(a.type.length()-1)=='*' ||
           a.type.at(a.type.length()-1)=='&'))
           // type ends with * or & => pointer or reference
      {
        result=FALSE;
        AUTO_TRACE_EXIT("pointer or reference: result={}",result);
        return result;
      }
      if (a.type.isEmpty() || resolver.resolveClass(ctx,a.type)!=nullptr)
      {
        result=FALSE; // arg type is a known type
        AUTO_TRACE_EXIT("known type: result={}",result);
        return result;
      }
      if (checkIfTypedef(ctx,fd,a.type))
      {
        result=FALSE; // argument is a typedef
        AUTO_TRACE_EXIT("typedef: result={}",result);
        return result;
      }
      std::string atype = a.type.str();
      if (reg::search(atype,match,initChars) && match.position()==0)
      {
        result=TRUE; // argument type starts with typical initializer char
        AUTO_TRACE_EXIT("argument with init char: result={}",result);
        return result;
      }
      std::string resType=resolveTypeDef(ctx,a.type).str();
      if (resType.empty()) resType=atype;
      static const reg::Ex idChars(R"(\a\w*)");
      if (reg::search(resType,match,idChars) && match.position()==0) // resType starts with identifier
      {
        resType=match.str();
        if (resType=="int"    || resType=="long"     ||
            resType=="float"  || resType=="double"   ||
            resType=="char"   || resType=="void"     ||
            resType=="signed" || resType=="unsigned" ||
            resType=="const"  || resType=="volatile" )
        {
          result=FALSE; // type keyword -> function prototype
          AUTO_TRACE_EXIT("type keyword: result={}",result);
          return result;
        }
      }
    }
    result=TRUE;
  }

  AUTO_TRACE_EXIT("end: result={}",result);
  return result;
}

static void addVariable(const Entry *root,int isFuncPtr=-1)
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);

  AUTO_TRACE("VARIABLE_SEC: type='{}' name='{}' args='{}' bodyLine={} endBodyLine={} mGrpId={} relates='{}'",
      root->type, root->name, root->args, root->bodyLine, root->endBodyLine, root->mGrpId, root->relates);
  //printf("root->parent->name=%s\n",qPrint(root->parent->name));

  QCString type = root->type;
  QCString name = root->name;
  QCString args = root->args;
  if (type.isEmpty() && name.find("operator")==-1 &&
      (name.find('*')!=-1 || name.find('&')!=-1))
  {
    // recover from parse error caused by redundant braces
    // like in "int *(var[10]);", which is parsed as
    // type="" name="int *" args="(var[10])"

    type=name;
    std::string sargs = args.str();
    static const reg::Ex reName(R"(\a\w*)");
    reg::Match match;
    if (reg::search(sargs,match,reName))
    {
      name  = match.str();           // e.g. 'var'  in '(var[10])'
      sargs = match.suffix().str();  // e.g. '[10]) in '(var[10])'
      size_t j = sargs.find(')');
      if (j!=std::string::npos) args=sargs.substr(0,j); // extract, e.g '[10]' from '[10])'
    }
  }
  else
  {
    int i=isFuncPtr;
    if (i==-1 && (root->spec.isAlias())==0) i=findFunctionPtr(type.str(),root->lang); // for typedefs isFuncPtr is not yet set
    AUTO_TRACE_ADD("functionPtr={}",i!=-1?"yes":"no");
    if (i>=0) // function pointer
    {
      int ai = type.find('[',i);
      if (ai>i) // function pointer array
      {
        args.prepend(type.right(type.length()-ai));
        type=type.left(ai);
      }
      else if (type.find(')',i)!=-1) // function ptr, not variable like "int (*bla)[10]"
      {
        type=type.left(type.length()-1);
        args.prepend(") ");
      }
    }
  }
  AUTO_TRACE_ADD("after correction: type='{}' name='{}' args='{}'",type,name,args);

  QCString scope;
  name=removeRedundantWhiteSpace(name);

  // find the scope of this variable
  int index = computeQualifiedIndex(name);
  if (index!=-1 && root->parent()->section.isGroupDoc() && root->parent()->tagInfo())
    // grouped members are stored with full scope
  {
    buildScopeFromQualifiedName(name.left(index+2),root->lang,root->tagInfo());
    scope=name.left(index);
    name=name.mid(index+2);
  }
  else
  {
    Entry *p = root->parent();
    while (p->section.isScope())
    {
      QCString scopeName = p->name;
      if (!scopeName.isEmpty())
      {
        scope.prepend(scopeName);
        break;
      }
      p=p->parent();
    }
  }

  type=type.stripWhiteSpace();
  ClassDefMutable *cd=nullptr;
  bool isRelated=FALSE;
  bool isMemberOf=FALSE;

  QCString classScope=stripAnonymousNamespaceScope(scope);
  classScope=stripTemplateSpecifiersFromScope(classScope,FALSE);
  QCString annScopePrefix=scope.left(scope.length()-classScope.length());

  if (name.findRev("::")!=-1)
  {
    if (type=="friend class" || type=="friend struct" ||
        type=="friend union")
    {
      cd=getClassMutable(scope);
      if (cd)
      {
        addVariableToClass(root,  // entry
            cd,    // class to add member to
            MemberType::Friend, // type of member
            type,   // type value as string
            name,   // name of the member
            args,   // arguments as string
            FALSE,  // from Anonymous scope
            nullptr,      // anonymous member
            Protection::Public,   // protection
            Relationship::Member  // related to a class
            );
      }
    }
    if (root->bodyLine!=-1 && root->endBodyLine!=-1) // store the body location for later use
    {
      Doxygen::staticInitMap.emplace(name.str(),BodyInfo{root->startLine,root->bodyLine,root->endBodyLine});
    }


    AUTO_TRACE_ADD("static variable {} body=[{}..{}]",name,root->bodyLine,root->endBodyLine);
    return;  /* skip this member, because it is a
              * static variable definition (always?), which will be
              * found in a class scope as well, but then we know the
              * correct protection level, so only then it will be
              * inserted in the correct list!
              */
  }

  MemberType mtype = MemberType::Variable;
  if (type=="@")
    mtype=MemberType::EnumValue;
  else if (type.startsWith("typedef "))
    mtype=MemberType::Typedef;
  else if (type.startsWith("friend "))
    mtype=MemberType::Friend;
  else if (root->mtype==MethodTypes::Property)
    mtype=MemberType::Property;
  else if (root->mtype==MethodTypes::Event)
    mtype=MemberType::Event;
  else if (type.find("sequence<") != -1)
    mtype=sliceOpt ? MemberType::Sequence : MemberType::Typedef;
  else if (type.find("dictionary<") != -1)
    mtype=sliceOpt ? MemberType::Dictionary : MemberType::Typedef;

  if (!root->relates.isEmpty()) // related variable
  {
    isRelated=TRUE;
    isMemberOf=(root->relatesType==RelatesType::MemberOf);
    if (getClass(root->relates)==nullptr && !scope.isEmpty())
      scope=mergeScopes(scope,root->relates);
    else
      scope=root->relates;
  }

  cd=getClassMutable(scope);
  if (cd==nullptr && classScope!=scope) cd=getClassMutable(classScope);
  if (cd)
  {
    MemberDef *md=nullptr;

    // if cd is an anonymous (=tag less) scope we insert the member
    // into a non-anonymous parent scope as well. This is needed to
    // be able to refer to it using \var or \fn

    //int indentDepth=0;
    int si=scope.find('@');
    //int anonyScopes = 0;
    //bool added=FALSE;

    bool inlineSimpleStructs = Config_getBool(INLINE_SIMPLE_STRUCTS);
    Relationship relationship = isMemberOf ? Relationship::Foreign :
      isRelated  ? Relationship::Related :
      Relationship::Member  ;
    if (si!=-1 && !inlineSimpleStructs) // anonymous scope or type
    {
      QCString pScope;
      ClassDefMutable *pcd=nullptr;
      pScope = scope.left(std::max(si-2,0)); // scope without tag less parts
      if (!pScope.isEmpty())
        pScope.prepend(annScopePrefix);
      else if (annScopePrefix.length()>2)
        pScope=annScopePrefix.left(annScopePrefix.length()-2);
      if (name.at(0)!='@')
      {
        if (!pScope.isEmpty() && (pcd=getClassMutable(pScope)))
        {
          AUTO_TRACE_ADD("Adding anonymous member to scope '{}'",pScope);
          md=addVariableToClass(root,  // entry
              pcd,   // class to add member to
              mtype, // member type
              type,  // type value as string
              name,  // member name
              args,  // arguments as string
              TRUE,  // from anonymous scope
              nullptr,     // from anonymous member
              root->protection,
              relationship
              );
          //added=TRUE;
        }
        else // anonymous scope inside namespace or file => put variable in the global scope
        {
          if (mtype==MemberType::Variable)
          {
            AUTO_TRACE_ADD("Adding anonymous member to global scope '{}'");
            md=addVariableToFile(root,mtype,pScope,type,name,args,TRUE,nullptr);
          }
          //added=TRUE;
        }
      }
    }

    addVariableToClass(root,   // entry
        cd,     // class to add member to
        mtype,  // member type
        type,   // type value as string
        name,   // name of the member
        args,   // arguments as string
        FALSE,  // from anonymous scope
        md,     // from anonymous member
        root->protection,
        relationship
        );
  }
  else if (!name.isEmpty()) // global variable
  {
    addVariableToFile(root,mtype,scope,type,name,args,FALSE,/*nullptr,*/nullptr);
  }

}

//----------------------------------------------------------------------
// Searches the Entry tree for typedef documentation sections.
// If found they are stored in their class or in the global list.
static void buildTypedefList(const Entry *root)
{
  //printf("buildVarList(%s)\n",qPrint(rootNav->name()));
  if (!root->name.isEmpty() &&
      root->section.isVariable() &&
      root->type.find("typedef ")!=-1 // its a typedef
     )
  {
    AUTO_TRACE();
    QCString rname = removeRedundantWhiteSpace(root->name);
    QCString scope;
    int index = computeQualifiedIndex(rname);
    if (index!=-1 && root->parent()->section.isGroupDoc() && root->parent()->tagInfo())
      // grouped members are stored with full scope
    {
      buildScopeFromQualifiedName(rname.left(index+2),root->lang,root->tagInfo());
      scope=rname.left(index);
      rname=rname.mid(index+2);
    }
    else
    {
      scope=root->parent()->name; //stripAnonymousNamespaceScope(root->parent->name);
    }
    ClassDefMutable *cd=getClassMutable(scope);
    NamespaceDef *nd=getResolvedNamespace(scope);
    MemberName *mn = Doxygen::functionNameLinkedMap->find(rname);
    bool found=false;
    if (mn) // symbol with the same name already found
    {
      for (auto &imd : *mn)
      {
        if (!imd->isTypedef())
          continue;

        QCString rtype = root->type;
        rtype.stripPrefix("typedef ");

        // merge the typedefs only if they're not both grouped, and both are
        // either part of the same class, part of the same namespace, or both
        // are global (i.e., neither in a class or a namespace)
        bool notBothGrouped = root->groups.empty() || imd->getGroupDef()==nullptr; // see example #100
        bool bothSameScope = (!cd && !nd) || (cd && imd->getClassDef() == cd) || (nd && imd->getNamespaceDef() == nd);
        //printf("imd->isTypedef()=%d imd->typeString()=%s root->type=%s\n",imd->isTypedef(),
        //    qPrint(imd->typeString()),qPrint(root->type));
        if (notBothGrouped && bothSameScope && imd->typeString()==rtype)
        {
          MemberDefMutable *md = toMemberDefMutable(imd.get());
          if (md)
          {
            md->setDocumentation(root->doc,root->docFile,root->docLine);
            md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
            md->setDocsForDefinition(!root->proto);
            md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            md->addSectionsToDefinition(root->anchors);
            md->setRefItems(root->sli);
            md->addQualifiers(root->qualifiers);

            // merge ingroup specifiers
            if (md->getGroupDef()==nullptr && !root->groups.empty())
            {
              addMemberToGroups(root,md);
            }
            else if (md->getGroupDef()!=nullptr && root->groups.empty())
            {
              //printf("existing member is grouped, new member not\n");
            }
            else if (md->getGroupDef()!=nullptr && !root->groups.empty())
            {
              //printf("both members are grouped\n");
            }
            found=true;
            break;
          }
        }
      }
    }
    if (found)
    {
      AUTO_TRACE_ADD("typedef '{}' already found",rname);
      // mark the entry as processed, as we copied everything from it elsewhere
      // also, otherwise, due to containing `typedef` it may later get treated
      // as a function typedef in filterMemberDocumentation, which is incorrect
      root->markAsProcessed();
    }
    else
    {
      AUTO_TRACE_ADD("new typedef '{}'",rname);
      addVariable(root);
    }

  }
  for (const auto &e : root->children())
    if (!e->section.isEnum())
      buildTypedefList(e.get());
}

//----------------------------------------------------------------------
// Searches the Entry tree for sequence documentation sections.
// If found they are stored in the global list.
static void buildSequenceList(const Entry *root)
{
  if (!root->name.isEmpty() &&
      root->section.isVariable() &&
      root->type.find("sequence<")!=-1 // it's a sequence
     )
  {
    AUTO_TRACE();
    addVariable(root);
  }
  for (const auto &e : root->children())
    if (!e->section.isEnum())
      buildSequenceList(e.get());
}

//----------------------------------------------------------------------
// Searches the Entry tree for dictionary documentation sections.
// If found they are stored in the global list.
static void buildDictionaryList(const Entry *root)
{
  if (!root->name.isEmpty() &&
      root->section.isVariable() &&
      root->type.find("dictionary<")!=-1 // it's a dictionary
     )
  {
    AUTO_TRACE();
    addVariable(root);
  }
  for (const auto &e : root->children())
    if (!e->section.isEnum())
      buildDictionaryList(e.get());
}

//----------------------------------------------------------------------
// Searches the Entry tree for Variable documentation sections.
// If found they are stored in their class or in the global list.

static void buildVarList(const Entry *root)
{
  //printf("buildVarList(%s) section=%08x\n",qPrint(rootNav->name()),rootNav->section());
  int isFuncPtr=-1;
  if (!root->name.isEmpty() &&
      (root->type.isEmpty() || g_compoundKeywords.find(root->type.str())==g_compoundKeywords.end()) &&
      (
       (root->section.isVariable() &&   // it's a variable
        root->type.find("typedef ")==-1 // and not a typedef
       ) ||
       (root->section.isFunction() && // or maybe a function pointer variable
        (isFuncPtr=findFunctionPtr(root->type.str(),root->lang))!=-1
       ) ||
       (root->section.isFunction() && // class variable initialized by constructor
        isVarWithConstructor(root)
       )
      )
     ) // documented variable
  {
    AUTO_TRACE();
    addVariable(root,isFuncPtr);
  }
  for (const auto &e : root->children())
    if (!e->section.isEnum())
      buildVarList(e.get());
}

//----------------------------------------------------------------------
// Searches the Entry tree for Interface sections (UNO IDL only).
// If found they are stored in their service or in the global list.
//

static void addInterfaceOrServiceToServiceOrSingleton(
        const Entry *root,
        ClassDefMutable *cd,
        QCString const& rname)
{
  FileDef *fd = root->fileDef();
  enum MemberType type = root->section.isExportedInterface() ? MemberType::Interface : MemberType::Service;
  QCString fileName = root->fileName;
  if (fileName.isEmpty() && root->tagInfo())
  {
    fileName = root->tagInfo()->tagName;
  }
  auto md = createMemberDef(
      fileName, root->startLine, root->startColumn, root->type, rname,
      "", "", root->protection, root->virt, root->isStatic, Relationship::Member,
      type, ArgumentList(), root->argList, root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  mmd->setTagInfo(root->tagInfo());
  mmd->setMemberClass(cd);
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setDocsForDefinition(false);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
  mmd->setMemberSpecifiers(root->spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setMemberGroupId(root->mGrpId);
  mmd->setTypeConstraints(root->typeConstr);
  mmd->setLanguage(root->lang);
  mmd->setBodyDef(fd);
  mmd->setFileDef(fd);
  mmd->addSectionsToDefinition(root->anchors);
  QCString const def = root->type + " " + rname;
  mmd->setDefinition(def);
  applyMemberOverrideOptions(root,mmd);
  mmd->addQualifiers(root->qualifiers);

  AUTO_TRACE("Interface member: fileName='{}' type='{}' name='{}' mtype='{}' prot={} virt={} state={} proto={} def='{}'",
      fileName,root->type,rname,type,root->protection,root->virt,root->isStatic,root->proto,def);

  // add member to the class cd
  cd->insertMember(md.get());
  // also add the member as a "base" (to get nicer diagrams)
  // "optional" interface/service get Protected which turns into dashed line
  BaseInfo base(rname,
          root->spec.isOptional() ? Protection::Protected : Protection::Public, Specifier::Normal);
  TemplateNameMap templateNames;
  findClassRelation(root,cd,cd,&base,templateNames,DocumentedOnly,true) ||
       findClassRelation(root,cd,cd,&base,templateNames,Undocumented,true);
  // add file to list of used files
  cd->insertUsedFile(fd);

  addMemberToGroups(root,md.get());
  ModuleManager::instance().addMemberToModule(root,md.get());
  root->markAsProcessed();
  mmd->setRefItems(root->sli);

  // add member to the global list of all members
  MemberName *mn = Doxygen::memberNameLinkedMap->add(rname);
  mn->push_back(std::move(md));
}

static void buildInterfaceAndServiceList(const Entry *root)
{
  if (root->section.isExportedInterface() || root->section.isIncludedService())
  {
    AUTO_TRACE("Exported interface/included service: type='{}' scope='{}' name='{}' args='{}'"
               " relates='{}' relatesType='{}' file='{}' line={} bodyLine={} #tArgLists={}"
               " mGrpId={} spec={} proto={} docFile='{}'",
                 root->type, root->parent()->name, root->name, root->args,
                 root->relates, root->relatesType, root->fileName, root->startLine, root->bodyLine, root->tArgLists.size(),
                 root->mGrpId, root->spec, root->proto, root->docFile);

    QCString rname = removeRedundantWhiteSpace(root->name);

    if (!rname.isEmpty())
    {
      QCString scope = root->parent()->name;
      ClassDefMutable *cd = getClassMutable(scope);
      assert(cd);
      if (cd && ((ClassDef::Interface == cd->compoundType()) ||
                 (ClassDef::Service   == cd->compoundType()) ||
                 (ClassDef::Singleton == cd->compoundType())))
      {
        addInterfaceOrServiceToServiceOrSingleton(root,cd,rname);
      }
      else
      {
        assert(false); // was checked by scanner.l
      }
    }
    else if (rname.isEmpty())
    {
      warn(root->fileName,root->startLine,
           "Illegal member name found.");
    }
  }
  // can only have these in IDL anyway
  switch (root->lang)
  {
    case SrcLangExt::Unknown: // fall through (root node always is Unknown)
    case SrcLangExt::IDL:
        for (const auto &e : root->children()) buildInterfaceAndServiceList(e.get());
        break;
    default:
        return; // nothing to do here
  }
}


//----------------------------------------------------------------------
// Searches the Entry tree for Function sections.
// If found they are stored in their class or in the global list.

static void addMethodToClass(const Entry *root,ClassDefMutable *cd,
                  const QCString &rtype,const QCString &rname,const QCString &rargs,
                  bool isFriend,
                  Protection protection,bool stat,Specifier virt,TypeSpecifier spec,
                  const QCString &relates
                  )
{
  FileDef *fd=root->fileDef();

  QCString type = rtype;
  QCString args = rargs;

  QCString name=removeRedundantWhiteSpace(rname);
  name.stripPrefix("::");

  MemberType mtype = MemberType::Function;
  if (isFriend)                              mtype=MemberType::Friend;
  else if (root->mtype==MethodTypes::Signal) mtype=MemberType::Signal;
  else if (root->mtype==MethodTypes::Slot)   mtype=MemberType::Slot;
  else if (root->mtype==MethodTypes::DCOP)   mtype=MemberType::DCOP;

  // strip redundant template specifier for constructors
  int i = -1;
  int j = -1;
  if ((fd==nullptr || fd->getLanguage()==SrcLangExt::Cpp) &&
      !name.startsWith("operator ") &&   // not operator
      (i=name.find('<'))!=-1    &&   // containing <
      (j=name.find('>'))!=-1    &&   // or >
      (j!=i+2 || name.at(i+1)!='=')  // but not the C++20 spaceship operator <=>
     )
  {
    name=name.left(i);
  }

  QCString fileName = root->fileName;
  if (fileName.isEmpty() && root->tagInfo())
  {
    fileName = root->tagInfo()->tagName;
  }

  //printf("root->name='%s; args='%s' root->argList='%s'\n",
  //    qPrint(root->name),qPrint(args),qPrint(argListToString(root->argList))
  //   );

  // adding class member
  Relationship relationship = relates.isEmpty() ?                        Relationship::Member  :
                              root->relatesType==RelatesType::MemberOf ? Relationship::Foreign :
                                                                         Relationship::Related ;
  auto md = createMemberDef(
      fileName,root->startLine,root->startColumn,
      type,name,args,root->exception,
      protection,virt,
      stat && root->relatesType!=RelatesType::MemberOf,
      relationship,
      mtype,!root->tArgLists.empty() ? root->tArgLists.back() : ArgumentList(),
      root->argList, root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  mmd->setTagInfo(root->tagInfo());
  mmd->setMemberClass(cd);
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setDocsForDefinition(!root->proto);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
  mmd->setMemberSpecifiers(spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setMemberGroupId(root->mGrpId);
  mmd->setTypeConstraints(root->typeConstr);
  mmd->setLanguage(root->lang);
  mmd->setRequiresClause(root->req);
  mmd->setId(root->id);
  mmd->setBodyDef(fd);
  mmd->setFileDef(fd);
  mmd->addSectionsToDefinition(root->anchors);
  QCString def;
  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  SrcLangExt lang = cd->getLanguage();
  QCString scopeSeparator=getLanguageSpecificSeparator(lang);
  if (scopeSeparator!="::")
  {
    qualScope = substitute(qualScope,"::",scopeSeparator);
  }
  if (lang==SrcLangExt::PHP)
  {
    // for PHP we use Class::method and Namespace\method
    scopeSeparator="::";
  }
//  QCString optArgs = root->argList.empty() ? args : QCString();
  if (!relates.isEmpty() || isFriend || Config_getBool(HIDE_SCOPE_NAMES))
  {
    if (!type.isEmpty())
    {
      def=type+" "+name; //+optArgs;
    }
    else
    {
      def=name; //+optArgs;
    }
  }
  else
  {
    if (!type.isEmpty())
    {
      def=type+" "+qualScope+scopeSeparator+name; //+optArgs;
    }
    else
    {
      def=qualScope+scopeSeparator+name; //+optArgs;
    }
  }
  def.stripPrefix("friend ");
  mmd->setDefinition(def);
  applyMemberOverrideOptions(root,mmd);
  mmd->addQualifiers(root->qualifiers);

  AUTO_TRACE("function member: type='{}' scope='{}' name='{}' args='{}' proto={} def='{}'",
             type, qualScope, rname, args, root->proto, def);

  // add member to the class cd
  cd->insertMember(md.get());
  // add file to list of used files
  cd->insertUsedFile(fd);

  addMemberToGroups(root,md.get());
  ModuleManager::instance().addMemberToModule(root,md.get());
  root->markAsProcessed();
  mmd->setRefItems(root->sli);

  // add member to the global list of all members
  //printf("Adding member=%s class=%s\n",qPrint(md->name()),qPrint(cd->name()));
  MemberName *mn = Doxygen::memberNameLinkedMap->add(name);
  mn->push_back(std::move(md));
}

//------------------------------------------------------------------------------------------

static void addGlobalFunction(const Entry *root,const QCString &rname,const QCString &sc)
{
  QCString scope = sc;

  // new global function
  QCString name=removeRedundantWhiteSpace(rname);
  auto md = createMemberDef(
      root->fileName,root->startLine,root->startColumn,
      root->type,name,root->args,root->exception,
      root->protection,root->virt,root->isStatic,Relationship::Member,
      MemberType::Function,
      !root->tArgLists.empty() ? root->tArgLists.back() : ArgumentList(),
      root->argList,root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  mmd->setTagInfo(root->tagInfo());
  mmd->setLanguage(root->lang);
  mmd->setId(root->id);
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->setPrototype(root->proto,root->fileName,root->startLine,root->startColumn);
  mmd->setDocsForDefinition(!root->proto);
  mmd->setTypeConstraints(root->typeConstr);
  //md->setBody(root->body);
  mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
  FileDef *fd=root->fileDef();
  mmd->setBodyDef(fd);
  mmd->addSectionsToDefinition(root->anchors);
  mmd->setMemberSpecifiers(root->spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setMemberGroupId(root->mGrpId);
  mmd->setRequiresClause(root->req);
  mmd->setExplicitExternal(root->explicitExternal,root->fileName,root->startLine,root->startColumn);

  NamespaceDefMutable *nd = nullptr;
  // see if the function is inside a namespace that was not part of
  // the name already (in that case nd should be non-zero already)
  if (root->parent()->section.isNamespace())
  {
    //QCString nscope=removeAnonymousScopes(root->parent()->name);
    QCString nscope=root->parent()->name;
    if (!nscope.isEmpty())
    {
      nd = getResolvedNamespaceMutable(nscope);
    }
  }
  else if (root->parent()->section.isGroupDoc() && !scope.isEmpty())
  {
    nd = getResolvedNamespaceMutable(sc);
  }

  if (!scope.isEmpty())
  {
    QCString sep = getLanguageSpecificSeparator(root->lang);
    if (sep!="::")
    {
      scope = substitute(scope,"::",sep);
    }
    scope+=sep;
  }

  if (Config_getBool(HIDE_SCOPE_NAMES)) scope = "";
  QCString def;
  //QCString optArgs = root->argList.empty() ? QCString() : root->args;
  if (!root->type.isEmpty())
  {
    def=root->type+" "+scope+name; //+optArgs;
  }
  else
  {
    def=scope+name; //+optArgs;
  }
  AUTO_TRACE("new non-member function type='{}' scope='{}' name='{}' args='{}' proto={} def='{}'",
              root->type,scope,rname,root->args,root->proto,def);
  mmd->setDefinition(def);
  applyMemberOverrideOptions(root,mmd);
  mmd->addQualifiers(root->qualifiers);

  mmd->setRefItems(root->sli);
  if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
  {
    // add member to namespace
    mmd->setNamespace(nd);
    nd->insertMember(md.get());
  }
  if (fd)
  {
    // add member to the file (we do this even if we have already
    // inserted it into the namespace)
    mmd->setFileDef(fd);
    fd->insertMember(md.get());
  }

  addMemberToGroups(root,md.get());
  ModuleManager::instance().addMemberToModule(root,md.get());
  if (root->relatesType == RelatesType::Simple) // if this is a relatesalso command,
                                                // allow find Member to pick it up
  {
    root->markAsProcessed(); // Otherwise we have finished with this entry.
  }

  // add member to the list of file members
  MemberName *mn = Doxygen::functionNameLinkedMap->add(name);
  mn->push_back(std::move(md));
}

//------------------------------------------------------------------------------------------

static void buildFunctionList(const Entry *root)
{
  if (root->section.isFunction())
  {
    AUTO_TRACE("member function: type='{}' scope='{}' name='{}' args='{}' relates='{}' relatesType='{}'"
               " file='{}' line={} bodyLine={} #tArgLists={} mGrpId={}"
               " spec={} proto={} docFile='{}'",
               root->type, root->parent()->name, root->name, root->args, root->relates, root->relatesType,
               root->fileName, root->startLine, root->bodyLine, root->tArgLists.size(), root->mGrpId,
               root->spec, root->proto, root->docFile);

    bool isFriend=root->type.find("friend ")!=-1;
    QCString rname = removeRedundantWhiteSpace(root->name);
    //printf("rname=%s\n",qPrint(rname));

    QCString scope;
    int index = computeQualifiedIndex(rname);
    if (index!=-1 && root->parent()->section.isGroupDoc() && root->parent()->tagInfo())
      // grouped members are stored with full scope
    {
      buildScopeFromQualifiedName(rname.left(index+2),root->lang,root->tagInfo());
      scope=rname.left(index);
      rname=rname.mid(index+2);
    }
    else
    {
      scope=root->parent()->name; //stripAnonymousNamespaceScope(root->parent->name);
    }
    if (!rname.isEmpty() && scope.find('@')==-1)
    {
      // check if this function's parent is a class
      scope=stripTemplateSpecifiersFromScope(scope,FALSE);

      FileDef *rfd=root->fileDef();

      int memIndex=rname.findRev("::");

      ClassDefMutable *cd=getClassMutable(scope);
      if (cd && scope+"::"==rname.left(scope.length()+2)) // found A::f inside A
      {
        // strip scope from name
        rname=rname.right(rname.length()-root->parent()->name.length()-2);
      }

      bool isMember=FALSE;
      if (memIndex!=-1)
      {
        int ts=rname.find('<');
        int te=rname.find('>');
        if (memIndex>0 && (ts==-1 || te==-1))
        {
          // note: the following code was replaced by inMember=TRUE to deal with a
          // function rname='X::foo' of class X inside a namespace also called X...
          // bug id 548175
          //nd = Doxygen::namespaceLinkedMap->find(rname.left(memIndex));
          //isMember = nd==nullptr;
          //if (nd)
          //{
          //  // strip namespace scope from name
          //  scope=rname.left(memIndex);
          //  rname=rname.right(rname.length()-memIndex-2);
          //}
          isMember = TRUE;
        }
        else
        {
          isMember=memIndex<ts || memIndex>te;
        }
      }

      if (!root->parent()->name.isEmpty() && root->parent()->section.isCompound() && cd)
      {
        AUTO_TRACE_ADD("member '{}' of class '{}'", rname,cd->name());
        addMethodToClass(root,cd,root->type,rname,root->args,isFriend,
                         root->protection,root->isStatic,root->virt,root->spec,root->relates);
      }
      else if (root->parent()->section.isObjcImpl() && cd)
      {
        const MemberDef *md = cd->getMemberByName(rname);
        if (md)
        {
          MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(md));
          if (mdm)
          {
            mdm->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
            mdm->setBodyDef(root->fileDef());
          }
        }
      }
      else if (!root->parent()->section.isCompound() && !root->parent()->section.isObjcImpl() &&
               !isMember &&
               (root->relates.isEmpty() || root->relatesType==RelatesType::Duplicate) &&
               !root->type.startsWith("extern ") && !root->type.startsWith("typedef ")
              )
      // no member => unrelated function
      {
        /* check the uniqueness of the function name in the file.
         * A file could contain a function prototype and a function definition
         * or even multiple function prototypes.
         */
        bool found=FALSE;
        MemberDef *md_found=nullptr;
        MemberName *mn = Doxygen::functionNameLinkedMap->find(rname);
        if (mn)
        {
          AUTO_TRACE_ADD("function '{}' already found",rname);
          for (const auto &imd : *mn)
          {
            MemberDefMutable *md = toMemberDefMutable(imd.get());
            if (md)
            {
              const NamespaceDef *mnd = md->getNamespaceDef();
              NamespaceDef *rnd = nullptr;
              //printf("root namespace=%s\n",qPrint(rootNav->parent()->name()));
              QCString fullScope = scope;
              QCString parentScope = root->parent()->name;
              if (!parentScope.isEmpty() && !leftScopeMatch(parentScope,scope))
              {
                if (!scope.isEmpty()) fullScope.prepend("::");
                fullScope.prepend(parentScope);
              }
              //printf("fullScope=%s\n",qPrint(fullScope));
              rnd = getResolvedNamespace(fullScope);
              const FileDef *mfd = md->getFileDef();
              QCString nsName,rnsName;
              if (mnd)  nsName = mnd->name();
              if (rnd) rnsName = rnd->name();
              //printf("matching arguments for %s%s %s%s\n",
              //    qPrint(md->name()),md->argsString(),qPrint(rname),qPrint(argListToString(root->argList)));
              const ArgumentList &mdAl = md->argumentList();
              const ArgumentList &mdTempl = md->templateArguments();

              // in case of template functions, we need to check if the
              // functions have the same number of template parameters
              bool sameNumTemplateArgs = TRUE;
              bool matchingReturnTypes = TRUE;
              bool sameRequiresClause = TRUE;
              if (!mdTempl.empty() && !root->tArgLists.empty())
              {
                if (mdTempl.size()!=root->tArgLists.back().size())
                {
                  sameNumTemplateArgs = FALSE;
                }
                if (md->typeString()!=removeRedundantWhiteSpace(root->type))
                {
                  matchingReturnTypes = FALSE;
                }
                if (md->requiresClause()!=root->req)
                {
                  sameRequiresClause = FALSE;
                }
              }
              else if (!mdTempl.empty() || !root->tArgLists.empty())
              { // if one has template parameters and the other doesn't then that also counts as a
                // difference
                sameNumTemplateArgs = FALSE;
              }

              bool staticsInDifferentFiles =
                root->isStatic && md->isStatic() && root->fileName!=md->getDefFileName();

              if (
                  matchArguments2(md->getOuterScope(),mfd,&mdAl,
                    rnd ? rnd : Doxygen::globalScope,rfd,&root->argList,
                    FALSE,root->lang) &&
                  sameNumTemplateArgs &&
                  matchingReturnTypes &&
                  sameRequiresClause &&
                  !staticsInDifferentFiles
                 )
              {
                GroupDef *gd=nullptr;
                if (!root->groups.empty() && !root->groups.front().groupname.isEmpty())
                {
                  gd = Doxygen::groupLinkedMap->find(root->groups.front().groupname);
                }
                //printf("match!\n");
                //printf("mnd=%p rnd=%p nsName=%s rnsName=%s\n",mnd,rnd,qPrint(nsName),qPrint(rnsName));
                // see if we need to create a new member
                found=(mnd && rnd && nsName==rnsName) ||   // members are in the same namespace
                  ((mnd==nullptr && rnd==nullptr && mfd!=nullptr &&       // no external reference and
                    mfd->absFilePath()==root->fileName // prototype in the same file
                   )
                  );
                // otherwise, allow a duplicate global member with the same argument list
                if (!found && gd && gd==md->getGroupDef() && nsName==rnsName)
                {
                  // member is already in the group, so we don't want to add it again.
                  found=TRUE;
                }

                AUTO_TRACE_ADD("combining function with prototype found={} in namespace '{}'",found,nsName);

                if (found)
                {
                  // merge argument lists
                  ArgumentList mergedArgList = root->argList;
                  mergeArguments(const_cast<ArgumentList&>(mdAl),mergedArgList,!root->doc.isEmpty());
                  // merge documentation
                  if (md->documentation().isEmpty() && !root->doc.isEmpty())
                  {
                    if (root->proto)
                    {
                      md->moveDeclArgumentList(stringToArgumentList(root->lang,root->args));
                    }
                    else
                    {
                      md->moveArgumentList(stringToArgumentList(root->lang,root->args));
                    }
                  }

                  md->setDocumentation(root->doc,root->docFile,root->docLine);
                  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                  md->setDocsForDefinition(!root->proto);
                  if (md->getStartBodyLine()==-1 && root->bodyLine!=-1)
                  {
                    md->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
                    md->setBodyDef(rfd);
                  }

                  if (md->briefDescription().isEmpty() && !root->brief.isEmpty())
                  {
                    md->setArgsString(root->args);
                  }
                  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);

                  md->addSectionsToDefinition(root->anchors);

                  applyMemberOverrideOptions(root,md);
                  md->addQualifiers(root->qualifiers);

                  // merge ingroup specifiers
                  if (md->getGroupDef()==nullptr && !root->groups.empty())
                  {
                    addMemberToGroups(root,md);
                  }
                  else if (md->getGroupDef()!=nullptr && root->groups.empty())
                  {
                    //printf("existing member is grouped, new member not\n");
                  }
                  else if (md->getGroupDef()!=nullptr && !root->groups.empty())
                  {
                    //printf("both members are grouped\n");
                  }
                  ModuleManager::instance().addMemberToModule(root,md);

                  // if md is a declaration and root is the corresponding
                  // definition, then turn md into a definition.
                  if (md->isPrototype() && !root->proto)
                  {
                    md->setDeclFile(md->getDefFileName(),md->getDefLine(),md->getDefColumn());
                    md->setPrototype(FALSE,root->fileName,root->startLine,root->startColumn);
                  }
                  // if md is already the definition, then add the declaration info
                  else if (!md->isPrototype() && root->proto)
                  {
                    md->setDeclFile(root->fileName,root->startLine,root->startColumn);
                  }
                }
              }
            }
            if (found)
            {
              md_found = md;
              break;
            }
          }
        }
        if (!found) /* global function is unique with respect to the file */
        {
          addGlobalFunction(root,rname,scope);
        }
        else
        {
          FileDef *fd=root->fileDef();
          if (fd)
          {
            // add member to the file (we do this even if we have already
            // inserted it into the namespace)
            fd->insertMember(md_found);
          }
        }

        AUTO_TRACE_ADD("unrelated function type='{}' name='{}' args='{}'",root->type,rname,root->args);
      }
      else
      {
        AUTO_TRACE_ADD("function '{}' is not processed",rname);
      }
    }
    else if (rname.isEmpty())
    {
        warn(root->fileName,root->startLine,
             "Illegal member name found."
            );
    }
  }
  for (const auto &e : root->children()) buildFunctionList(e.get());
}

//----------------------------------------------------------------------

static void findFriends()
{
  AUTO_TRACE();
  for (const auto &fn : *Doxygen::functionNameLinkedMap) // for each global function name
  {
    MemberName *mn = Doxygen::memberNameLinkedMap->find(fn->memberName());
    if (mn)
    { // there are members with the same name
      // for each function with that name
      for (const auto &ifmd : *fn)
      {
        MemberDefMutable *fmd = toMemberDefMutable(ifmd.get());
        // for each member with that name
        for (const auto &immd : *mn)
        {
           MemberDefMutable *mmd = toMemberDefMutable(immd.get());
          //printf("Checking for matching arguments
          //        mmd->isRelated()=%d mmd->isFriend()=%d mmd->isFunction()=%d\n",
          //    mmd->isRelated(),mmd->isFriend(),mmd->isFunction());
          if (fmd && mmd &&
              (mmd->isFriend() || (mmd->isRelated() && mmd->isFunction())) &&
              matchArguments2(mmd->getOuterScope(), mmd->getFileDef(), &mmd->argumentList(),
                              fmd->getOuterScope(), fmd->getFileDef(), &fmd->argumentList(),
                              TRUE,mmd->getLanguage()
                             )

             ) // if the member is related and the arguments match then the
               // function is actually a friend.
          {
            AUTO_TRACE_ADD("Merging related global and member '{}' isFriend={} isRelated={} isFunction={}",
                mmd->name(),mmd->isFriend(),mmd->isRelated(),mmd->isFunction());
            const ArgumentList &mmdAl = mmd->argumentList();
            const ArgumentList &fmdAl = fmd->argumentList();
            mergeArguments(const_cast<ArgumentList&>(fmdAl),const_cast<ArgumentList&>(mmdAl));

            // reset argument lists to add missing default parameters
            QCString mmdAlStr = argListToString(mmdAl);
            QCString fmdAlStr = argListToString(fmdAl);
            mmd->setArgsString(mmdAlStr);
            fmd->setArgsString(fmdAlStr);
            mmd->moveDeclArgumentList(std::make_unique<ArgumentList>(mmdAl));
            fmd->moveDeclArgumentList(std::make_unique<ArgumentList>(fmdAl));
            AUTO_TRACE_ADD("friend args='{}' member args='{}'",argListToString(fmd->argumentList()),argListToString(mmd->argumentList()));

            if (!fmd->documentation().isEmpty())
            {
              mmd->setDocumentation(fmd->documentation(),fmd->docFile(),fmd->docLine());
            }
            else if (!mmd->documentation().isEmpty())
            {
              fmd->setDocumentation(mmd->documentation(),mmd->docFile(),mmd->docLine());
            }
            if (mmd->briefDescription().isEmpty() && !fmd->briefDescription().isEmpty())
            {
              mmd->setBriefDescription(fmd->briefDescription(),fmd->briefFile(),fmd->briefLine());
            }
            else if (!mmd->briefDescription().isEmpty() && !fmd->briefDescription().isEmpty())
            {
              fmd->setBriefDescription(mmd->briefDescription(),mmd->briefFile(),mmd->briefLine());
            }
            if (!fmd->inbodyDocumentation().isEmpty())
            {
              mmd->setInbodyDocumentation(fmd->inbodyDocumentation(),fmd->inbodyFile(),fmd->inbodyLine());
            }
            else if (!mmd->inbodyDocumentation().isEmpty())
            {
              fmd->setInbodyDocumentation(mmd->inbodyDocumentation(),mmd->inbodyFile(),mmd->inbodyLine());
            }
            //printf("body mmd %d fmd %d\n",mmd->getStartBodyLine(),fmd->getStartBodyLine());
            if (mmd->getStartBodyLine()==-1 && fmd->getStartBodyLine()!=-1)
            {
              mmd->setBodySegment(fmd->getDefLine(),fmd->getStartBodyLine(),fmd->getEndBodyLine());
              mmd->setBodyDef(fmd->getBodyDef());
              //mmd->setBodyMember(fmd);
            }
            else if (mmd->getStartBodyLine()!=-1 && fmd->getStartBodyLine()==-1)
            {
              fmd->setBodySegment(mmd->getDefLine(),mmd->getStartBodyLine(),mmd->getEndBodyLine());
              fmd->setBodyDef(mmd->getBodyDef());
              //fmd->setBodyMember(mmd);
            }
            mmd->setDocsForDefinition(fmd->isDocsForDefinition());

            mergeMemberOverrideOptions(mmd,fmd);

            mmd->addQualifiers(fmd->getQualifiers());
            fmd->addQualifiers(mmd->getQualifiers());

          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferFunctionDocumentation()
{
  AUTO_TRACE();

  // find matching function declaration and definitions.
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    //printf("memberName=%s count=%zu\n",qPrint(mn->memberName()),mn->size());
    /* find a matching function declaration and definition for this function */
    for (const auto &imdec : *mn)
    {
      MemberDefMutable *mdec = toMemberDefMutable(imdec.get());
      if (mdec &&
          (mdec->isPrototype() ||
           (mdec->isVariable() && mdec->isExternal())
         ))
      {
        for (const auto &imdef : *mn)
        {
          MemberDefMutable *mdef = toMemberDefMutable(imdef.get());
          if (mdef && mdec!=mdef &&
              mdec->getNamespaceDef()==mdef->getNamespaceDef())
          {
            combineDeclarationAndDefinition(mdec,mdef);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferFunctionReferences()
{
  AUTO_TRACE();
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    MemberDefMutable *mdef=nullptr,*mdec=nullptr;
    /* find a matching function declaration and definition for this function */
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md)
      {
        if (md->isPrototype())
          mdec=md;
        else if (md->isVariable() && md->isExternal())
          mdec=md;

        if (md->isFunction() && !md->isStatic() && !md->isPrototype())
          mdef=md;
        else if (md->isVariable() && !md->isExternal() && !md->isStatic())
          mdef=md;
      }

      if (mdef && mdec) break;
    }
    if (mdef && mdec)
    {
      const ArgumentList &mdefAl = mdef->argumentList();
      const ArgumentList &mdecAl = mdec->argumentList();
      if (
          matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),const_cast<ArgumentList*>(&mdefAl),
                          mdec->getOuterScope(),mdec->getFileDef(),const_cast<ArgumentList*>(&mdecAl),
                          TRUE,mdef->getLanguage()
            )
         ) /* match found */
      {
        AUTO_TRACE_ADD("merging references for mdec={} mdef={}",mdec->name(),mdef->name());
        mdef->mergeReferences(mdec);
        mdec->mergeReferences(mdef);
        mdef->mergeReferencedBy(mdec);
        mdec->mergeReferencedBy(mdef);
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferRelatedFunctionDocumentation()
{
  AUTO_TRACE();
  // find match between function declaration and definition for
  // related functions
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    /* find a matching function declaration and definition for this function */
    // for each global function
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md)
      {
        //printf("  Function '%s'\n",qPrint(md->name()));
        MemberName *rmn = Doxygen::memberNameLinkedMap->find(md->name());
        if (rmn) // check if there is a member with the same name
        {
          //printf("  Member name found\n");
          // for each member with the same name
          for (const auto &irmd : *rmn)
          {
            MemberDefMutable *rmd = toMemberDefMutable(irmd.get());
            //printf("  Member found: related='%d'\n",rmd->isRelated());
            if (rmd &&
                (rmd->isRelated() || rmd->isForeign()) && // related function
                matchArguments2( md->getOuterScope(), md->getFileDef(), &md->argumentList(),
                  rmd->getOuterScope(),rmd->getFileDef(),&rmd->argumentList(),
                  TRUE,md->getLanguage()
                  )
               )
            {
              AUTO_TRACE_ADD("Found related member '{}'",md->name());
              if (rmd->relatedAlso())
                md->setRelatedAlso(rmd->relatedAlso());
              else if (rmd->isForeign())
                md->makeForeign();
              else
                md->makeRelated();
            }
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

void transferStaticInstanceInitializers()
{
  AUTO_TRACE();
  for (const auto &[qualifiedName,bodyInfo] : Doxygen::staticInitMap)
  {
    size_t i=qualifiedName.rfind("::");
    if (i!=std::string::npos)
    {
      QCString scope = qualifiedName.substr(0,i);
      QCString name  = qualifiedName.substr(i+2);
      MemberName *mn = Doxygen::memberNameLinkedMap->find(name);
      if (mn)
      {
        for (const auto &imd : *mn)
        {
          MemberDefMutable *md = toMemberDefMutable(imd.get());
          if (md && md->qualifiedName().str()==qualifiedName && md->isVariable())
          {
            AUTO_TRACE_ADD("found static member {} body [{}..{}]\n",
              md->qualifiedName(),bodyInfo.startLine,bodyInfo.endLine);
            md->setBodySegment(bodyInfo.defLine,
                               bodyInfo.startLine,
                               bodyInfo.endLine);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

/*! make a dictionary of all template arguments of class cd
 * that are part of the base class name.
 * Example: A template class A with template arguments <R,S,T>
 * that inherits from B<T,T,S> will have T and S in the dictionary.
 */
static TemplateNameMap getTemplateArgumentsInName(const ArgumentList &templateArguments,const std::string &name)
{
  std::map<std::string,int> templateNames;
  int count=0;
  for (const Argument &arg : templateArguments)
  {
    static const reg::Ex re(R"(\a[\w:]*)");
    reg::Iterator it(name,re);
    reg::Iterator end;
    for (; it!=end ; ++it)
    {
      const auto &match = *it;
      std::string n = match.str();
      if (n==arg.name.str())
      {
        if (templateNames.find(n)==templateNames.end())
        {
          templateNames.emplace(n,count);
        }
      }
    }
  }
  return templateNames;
}

/*! Searches a class from within \a context and \a cd and returns its
 *  definition if found (otherwise nullptr is returned).
 */
static ClassDef *findClassWithinClassContext(Definition *context,ClassDef *cd,const QCString &name)
{
  ClassDef *result=nullptr;
  if (cd==nullptr)
  {
    return result;
  }
  FileDef *fd=cd->getFileDef();
  SymbolResolver resolver(fd);
  if (context && cd!=context)
  {
    result = const_cast<ClassDef*>(resolver.resolveClass(context,name,true,true));
  }
  //printf("1. result=%p\n",result);
  if (result==nullptr)
  {
    result = const_cast<ClassDef*>(resolver.resolveClass(cd,name,true,true));
  }
  //printf("2. result=%p\n",result);
  if (result==nullptr) // try direct class, needed for namespaced classes imported via tag files (see bug624095)
  {
    result = getClass(name);
  }
  //printf("3. result=%p\n",result);
  //printf("** Trying to find %s within context %s class %s result=%s lookup=%p\n",
  //       qPrint(name),
  //       context ? qPrint(context->name()) : "<none>",
  //       cd      ? qPrint(cd->name())      : "<none>",
  //       result  ? qPrint(result->name())  : "<none>",
  //       Doxygen::classLinkedMap->find(name)
  //      );
  return result;
}


static void findUsedClassesForClass(const Entry *root,
                           Definition *context,
                           ClassDefMutable *masterCd,
                           ClassDefMutable *instanceCd,
                           bool isArtificial,
                           const ArgumentList *actualArgs = nullptr,
                           const TemplateNameMap &templateNames = TemplateNameMap()
                           )
{
  AUTO_TRACE();
  const ArgumentList &formalArgs = masterCd->templateArguments();
  for (auto &mni : masterCd->memberNameInfoLinkedMap())
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      if (md->isVariable() || md->isObjCProperty()) // for each member variable in this class
      {
        AUTO_TRACE_ADD("Found variable '{}' in class '{}'",md->name(),masterCd->name());
        QCString type = normalizeNonTemplateArgumentsInString(md->typeString(),masterCd,formalArgs);
        QCString typedefValue = md->getLanguage()==SrcLangExt::Java ? type : resolveTypeDef(masterCd,type);
        if (!typedefValue.isEmpty())
        {
          type = typedefValue;
        }
        int pos=0;
        QCString usedClassName;
        QCString templSpec;
        bool found=FALSE;
        // the type can contain template variables, replace them if present
        type = substituteTemplateArgumentsInString(type,formalArgs,actualArgs);

        //printf("      template substitution gives=%s\n",qPrint(type));
        while (!found && extractClassNameFromType(type,pos,usedClassName,templSpec,root->lang)!=-1)
        {
          // find the type (if any) that matches usedClassName
          SymbolResolver resolver(masterCd->getFileDef());
          const ClassDefMutable *typeCd = resolver.resolveClassMutable(masterCd,usedClassName,false,true);
          //printf("====>  usedClassName=%s -> typeCd=%s\n",
          //     qPrint(usedClassName),typeCd?qPrint(typeCd->name()):"<none>");
          if (typeCd)
          {
            usedClassName = typeCd->name();
          }

          int sp=usedClassName.find('<');
          if (sp==-1) sp=0;
          // replace any namespace aliases
          replaceNamespaceAliases(usedClassName);
          // add any template arguments to the class
          QCString usedName = removeRedundantWhiteSpace(usedClassName+templSpec);
          //printf("    usedName=%s usedClassName=%s templSpec=%s\n",qPrint(usedName),qPrint(usedClassName),qPrint(templSpec));

          TemplateNameMap formTemplateNames;
          if (templateNames.empty())
          {
            formTemplateNames = getTemplateArgumentsInName(formalArgs,usedName.str());
          }
          BaseInfo bi(usedName,Protection::Public,Specifier::Normal);
          findClassRelation(root,context,instanceCd,&bi,formTemplateNames,TemplateInstances,isArtificial);

          for (const Argument &arg : masterCd->templateArguments())
          {
            if (arg.name==usedName) // type is a template argument
            {
              ClassDef *usedCd = Doxygen::hiddenClassLinkedMap->find(usedName);
              ClassDefMutable *usedCdm = toClassDefMutable(usedCd);
              if (usedCd==nullptr)
              {
                usedCdm = toClassDefMutable(
                  Doxygen::hiddenClassLinkedMap->add(usedName,
                    createClassDef(
                      masterCd->getDefFileName(),masterCd->getDefLine(),
                      masterCd->getDefColumn(),
                      usedName,
                      ClassDef::Class)));
                if (usedCdm)
                {
                  //printf("making %s a template argument!!!\n",qPrint(usedCd->name()));
                  usedCdm->makeTemplateArgument();
                  usedCdm->setUsedOnly(TRUE);
                  usedCdm->setLanguage(masterCd->getLanguage());
                  usedCd = usedCdm;
                }
              }
              if (usedCd)
              {
                found=TRUE;
                AUTO_TRACE_ADD("case 1: adding used class '{}'", usedCd->name());
                instanceCd->addUsedClass(usedCd,md->name(),md->protection());
                if (usedCdm)
                {
                  if (isArtificial) usedCdm->setArtificial(TRUE);
                  usedCdm->addUsedByClass(instanceCd,md->name(),md->protection());
                }
              }
            }
          }

          if (!found)
          {
            ClassDef *usedCd=findClassWithinClassContext(context,masterCd,usedName);
            //printf("Looking for used class %s: result=%s master=%s\n",
            //    qPrint(usedName),usedCd?qPrint(usedCd->name()):"<none>",masterCd?qPrint(masterCd->name()):"<none>");

            if (usedCd)
            {
              found=TRUE;
              AUTO_TRACE_ADD("case 2: adding used class '{}'", usedCd->name());
              instanceCd->addUsedClass(usedCd,md->name(),md->protection()); // class exists
              ClassDefMutable *usedCdm = toClassDefMutable(usedCd);
              if (usedCdm)
              {
                usedCdm->addUsedByClass(instanceCd,md->name(),md->protection());
              }
            }
          }
        }
        if (!found && !type.isEmpty()) // used class is not documented in any scope
        {
          ClassDef *usedCd = Doxygen::hiddenClassLinkedMap->find(type);
          ClassDefMutable *usedCdm = toClassDefMutable(usedCd);
          if (usedCd==nullptr && !Config_getBool(HIDE_UNDOC_RELATIONS))
          {
            if (type.endsWith("(*") || type.endsWith("(^")) // type is a function pointer
            {
              type+=md->argsString();
            }
            AUTO_TRACE_ADD("New undocumented used class '{}'", type);
            usedCdm = toClassDefMutable(
                        Doxygen::hiddenClassLinkedMap->add(type,
                          createClassDef(
                            masterCd->getDefFileName(),masterCd->getDefLine(),
                            masterCd->getDefColumn(),
                            type,ClassDef::Class)));
            if (usedCdm)
            {
              usedCdm->setUsedOnly(TRUE);
              usedCdm->setLanguage(masterCd->getLanguage());
              usedCd = usedCdm;
            }
          }
          if (usedCd)
          {
            AUTO_TRACE_ADD("case 3: adding used class '{}'", usedCd->name());
            instanceCd->addUsedClass(usedCd,md->name(),md->protection());
            if (usedCdm)
            {
              if (isArtificial) usedCdm->setArtificial(TRUE);
              usedCdm->addUsedByClass(instanceCd,md->name(),md->protection());
            }
          }
        }
      }
    }
  }
}

static void findBaseClassesForClass(
      const Entry *root,
      Definition *context,
      ClassDefMutable *masterCd,
      ClassDefMutable *instanceCd,
      FindBaseClassRelation_Mode mode,
      bool isArtificial,
      const ArgumentList *actualArgs = nullptr,
      const TemplateNameMap &templateNames=TemplateNameMap()
    )
{
  AUTO_TRACE("name={}",root->name);
  // The base class could ofcouse also be a non-nested class
  const ArgumentList &formalArgs = masterCd->templateArguments();
  for (const BaseInfo &bi : root->extends)
  {
    //printf("masterCd=%s bi.name='%s' #actualArgs=%d\n",
    //    qPrint(masterCd->localName()),qPrint(bi.name),actualArgs ? (int)actualArgs->size() : -1);
    TemplateNameMap formTemplateNames;
    if (templateNames.empty())
    {
      formTemplateNames = getTemplateArgumentsInName(formalArgs,bi.name.str());
    }
    BaseInfo tbi = bi;
    tbi.name = substituteTemplateArgumentsInString(bi.name,formalArgs,actualArgs);
    //printf("masterCd=%p instanceCd=%p bi->name=%s tbi.name=%s\n",(void*)masterCd,(void*)instanceCd,qPrint(bi.name),qPrint(tbi.name));

    if (mode==DocumentedOnly)
    {
      // find a documented base class in the correct scope
      if (!findClassRelation(root,context,instanceCd,&tbi,formTemplateNames,DocumentedOnly,isArtificial))
      {
        // 1.8.2: decided to show inheritance relations even if not documented,
        //        we do make them artificial, so they do not appear in the index
        //if (!Config_getBool(HIDE_UNDOC_RELATIONS))
        bool b = Config_getBool(HIDE_UNDOC_RELATIONS) ? TRUE : isArtificial;
        //{
          // no documented base class -> try to find an undocumented one
          findClassRelation(root,context,instanceCd,&tbi,formTemplateNames,Undocumented,b);
        //}
      }
    }
    else if (mode==TemplateInstances)
    {
      findClassRelation(root,context,instanceCd,&tbi,formTemplateNames,TemplateInstances,isArtificial);
    }
  }
}

//----------------------------------------------------------------------

static void findTemplateInstanceRelation(const Entry *root,
            Definition *context,
            ClassDefMutable *templateClass,const QCString &templSpec,
            const TemplateNameMap &templateNames,
            bool isArtificial)
{
  AUTO_TRACE("Derived from template '{}' with parameters '{}' isArtificial={}",
         templateClass->name(),templSpec,isArtificial);

  QCString tempArgsStr = tempArgListToString(templateClass->templateArguments(),root->lang,false);
  bool existingClass = templSpec==tempArgsStr;
  if (existingClass) return; // avoid recursion

  bool freshInstance=FALSE;
  ClassDefMutable *instanceClass = toClassDefMutable(
                     templateClass->insertTemplateInstance(
                     root->fileName,root->startLine,root->startColumn,templSpec,freshInstance));
  if (instanceClass)
  {
    instanceClass->setArtificial(TRUE);
    instanceClass->setLanguage(root->lang);

    if (freshInstance)
    {
      AUTO_TRACE_ADD("found fresh instance '{}'",instanceClass->name());
      instanceClass->setTemplateBaseClassNames(templateNames);

      // search for new template instances caused by base classes of
      // instanceClass
      auto it_pair = g_classEntries.equal_range(templateClass->name().str());
      for (auto it=it_pair.first ; it!=it_pair.second ; ++it)
      {
        const Entry *templateRoot = it->second;
        AUTO_TRACE_ADD("template root found '{}' templSpec='{}'",templateRoot->name,templSpec);
        std::unique_ptr<ArgumentList> templArgs = stringToArgumentList(root->lang,templSpec);
        findBaseClassesForClass(templateRoot,context,templateClass,instanceClass,
            TemplateInstances,isArtificial,templArgs.get(),templateNames);

        findUsedClassesForClass(templateRoot,context,templateClass,instanceClass,
            isArtificial,templArgs.get(),templateNames);
      }
    }
    else
    {
      AUTO_TRACE_ADD("instance already exists");
    }
  }
}

static bool isRecursiveBaseClass(const QCString &scope,const QCString &name)
{
  QCString n=name;
  int index=n.find('<');
  if (index!=-1)
  {
    n=n.left(index);
  }
  bool result = rightScopeMatch(scope,n);
  return result;
}

/*! Searches for the end of a template in prototype \a s starting from
 *  character position \a startPos. If the end was found the position
 *  of the closing \> is returned, otherwise -1 is returned.
 *
 *  Handles exotic cases such as
 *  \code
 *    Class<(id<0)>
 *    Class<bits<<2>
 *    Class<"<">
 *    Class<'<'>
 *    Class<(")<")>
 *  \endcode
 */
static int findEndOfTemplate(const QCString &s,size_t startPos)
{
  // locate end of template
  size_t e=startPos;
  int brCount=1;
  int roundCount=0;
  size_t len = s.length();
  bool insideString=FALSE;
  bool insideChar=FALSE;
  char pc = 0;
  while (e<len && brCount!=0)
  {
    char c=s.at(e);
    switch(c)
    {
      case '<':
        if (!insideString && !insideChar)
        {
          if (e<len-1 && s.at(e+1)=='<')
            e++;
          else if (roundCount==0)
            brCount++;
        }
        break;
      case '>':
        if (!insideString && !insideChar)
        {
          if (e<len-1 && s.at(e+1)=='>')
            e++;
          else if (roundCount==0)
            brCount--;
        }
        break;
      case '(':
        if (!insideString && !insideChar)
          roundCount++;
        break;
      case ')':
        if (!insideString && !insideChar)
          roundCount--;
        break;
      case '"':
        if (!insideChar)
        {
          if (insideString && pc!='\\')
            insideString=FALSE;
          else
            insideString=TRUE;
        }
        break;
      case '\'':
        if (!insideString)
        {
          if (insideChar && pc!='\\')
            insideChar=FALSE;
          else
            insideChar=TRUE;
        }
        break;
    }
    pc = c;
    e++;
  }
  return brCount==0 ? static_cast<int>(e) : -1;
}

static int findTemplateSpecializationPosition(const QCString &name)
{
  if (name.isEmpty()) return 0;
  int l = static_cast<int>(name.length());
  if (name[l-1]=='>') // search backward to find the matching <, allowing nested <...> and strings.
  {
    int count=1;
    int i=l-2;
    char insideQuote=0;
    while (count>0 && i>=0)
    {
      char c = name[i--];
      switch (c)
      {
        case '>':  if (!insideQuote) count++; break;
        case '<':  if (!insideQuote) count--; break;
        case '\'': if (!insideQuote) insideQuote=c;
                   else if (insideQuote==c && (i<0 || name[i]!='\\')) insideQuote=0;
                   break;
        case '"':  if (!insideQuote) insideQuote=c;
                   else if (insideQuote==c && (i<0 || name[i]!='\\')) insideQuote=0;
                   break;
        default: break;
      }
    }
    if (i>=0) l=i+1;
  }
  return l;
}

static bool findClassRelation(
                           const Entry *root,
                           Definition *context,
                           ClassDefMutable *cd,
                           const BaseInfo *bi,
                           const TemplateNameMap &templateNames,
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          )
{
  AUTO_TRACE("name={} base={} isArtificial={}",cd->name(),bi->name,isArtificial);

  QCString biName=bi->name;
  bool explicitGlobalScope=FALSE;
  if (biName.startsWith("::")) // explicit global scope
  {
     biName=biName.right(biName.length()-2);
     explicitGlobalScope=TRUE;
  }

  Entry *parentNode=root->parent();
  bool lastParent=FALSE;
  do // for each parent scope, starting with the largest scope
     // (in case of nested classes)
  {
    QCString scopeName= parentNode ? parentNode->name : QCString();
    int scopeOffset=explicitGlobalScope ? 0 : static_cast<int>(scopeName.length());
    do // try all parent scope prefixes, starting with the largest scope
    {
      //printf("scopePrefix='%s' biName='%s'\n",
      //    qPrint(scopeName.left(scopeOffset)),qPrint(biName));

      QCString baseClassName=biName;
      if (scopeOffset>0)
      {
        baseClassName.prepend(scopeName.left(scopeOffset)+"::");
      }
      //QCString stripped;
      //baseClassName=stripTemplateSpecifiersFromScope
      //                    (removeRedundantWhiteSpace(baseClassName),TRUE,
      //                    &stripped);
      SymbolResolver resolver(cd->getFileDef());
      ClassDefMutable *baseClass = resolver.resolveClassMutable(explicitGlobalScope ? Doxygen::globalScope : context,
                                           baseClassName,
                                           mode==Undocumented,
                                           true
                                          );
      const MemberDef *baseClassTypeDef = resolver.getTypedef();
      QCString templSpec = resolver.getTemplateSpec();
      //printf("baseClassName=%s baseClass=%p cd=%p explicitGlobalScope=%d\n",
      //    qPrint(baseClassName),baseClass,cd,explicitGlobalScope);
      //printf("    scope='%s' baseClassName='%s' baseClass=%s templSpec=%s\n",
      //                    cd ? qPrint(cd->name()):"<none>",
      //                    qPrint(baseClassName),
      //                    baseClass?qPrint(baseClass->name()):"<none>",
      //                    qPrint(templSpec)
      //      );
      //if (baseClassName.left(root->name.length())!=root->name ||
      //    baseClassName.at(root->name.length())!='<'
      //   ) // Check for base class with the same name.
      //     // If found then look in the outer scope for a match
      //     // and prevent recursion.
      if (!isRecursiveBaseClass(root->name,baseClassName)
          || explicitGlobalScope
          // sadly isRecursiveBaseClass always true for UNO IDL ifc/svc members
          // (i.e. this is needed for addInterfaceOrServiceToServiceOrSingleton)
          || (root->lang==SrcLangExt::IDL &&
              (root->section.isExportedInterface() ||
               root->section.isIncludedService()))
         )
      {
        AUTO_TRACE_ADD("class relation '{}' inherited/used by '{}' found prot={} virt={} templSpec='{}'",
            baseClassName, root->name, bi->prot, bi->virt, templSpec);

        int i=findTemplateSpecializationPosition(baseClassName);
        int si=baseClassName.findRev("::",i);
        if (si==-1) si=0;
        if (baseClass==nullptr && static_cast<size_t>(i)!=baseClassName.length())
          // base class has template specifiers
        {
          // TODO: here we should try to find the correct template specialization
          // but for now, we only look for the unspecialized base class.
          int e=findEndOfTemplate(baseClassName,i+1);
          //printf("baseClass==0 i=%d e=%d\n",i,e);
          if (e!=-1) // end of template was found at e
          {
            templSpec = removeRedundantWhiteSpace(baseClassName.mid(i,e-i));
            baseClassName = baseClassName.left(i)+baseClassName.right(baseClassName.length()-e);
            baseClass = resolver.resolveClassMutable(explicitGlobalScope ? Doxygen::globalScope : context,
                  baseClassName,
                  mode==Undocumented,
                  true
                  );
            baseClassTypeDef = resolver.getTypedef();
            //printf("baseClass=%p -> baseClass=%s templSpec=%s\n",
            //      baseClass,qPrint(baseClassName),qPrint(templSpec));
          }
        }
        else if (baseClass && !templSpec.isEmpty()) // we have a known class, but also
                                                    // know it is a template, so see if
                                                    // we can also link to the explicit
                                                    // instance (for instance if a class
                                                    // derived from a template argument)
        {
          //printf("baseClass=%s templSpec=%s\n",qPrint(baseClass->name()),qPrint(templSpec));
          ClassDefMutable *templClass=getClassMutable(baseClass->name()+templSpec);
          if (templClass)
          {
            // use the template instance instead of the template base.
            baseClass = templClass;
            templSpec.clear();
          }
        }

        //printf("cd=%p baseClass=%p\n",cd,baseClass);
        bool found=baseClass!=nullptr && (baseClass!=cd || mode==TemplateInstances);
        //printf("1. found=%d\n",found);
        if (!found && si!=-1)
        {
          // replace any namespace aliases
          replaceNamespaceAliases(baseClassName);
          baseClass = resolver.resolveClassMutable(explicitGlobalScope ? Doxygen::globalScope : context,
                                     baseClassName,
                                     mode==Undocumented,
                                     true
                                    );
          baseClassTypeDef = resolver.getTypedef();
          found=baseClass!=nullptr && baseClass!=cd;
          if (found) templSpec = resolver.getTemplateSpec();
        }
        //printf("2. found=%d\n",found);

        if (!found)
        {
          baseClass=toClassDefMutable(findClassWithinClassContext(context,cd,baseClassName));
          //printf("findClassWithinClassContext(%s,%s)=%p\n",
          //    qPrint(cd->name()),qPrint(baseClassName),baseClass);
          found = baseClass!=nullptr && baseClass!=cd;

        }
        //printf("3. found=%d\n",found);
        if (!found)
        {
          // for PHP the "use A\B as C" construct map class C to A::B, so we lookup
          // the class name also in the alias mapping.
          auto it = Doxygen::namespaceAliasMap.find(baseClassName.str());
          if (it!=Doxygen::namespaceAliasMap.end()) // see if it is indeed a class.
          {
            baseClass=getClassMutable(QCString(it->second.alias));
            found = baseClass!=nullptr && baseClass!=cd;
          }
        }
        bool isATemplateArgument = templateNames.find(biName.str())!=templateNames.end();

        //printf("4. found=%d\n",found);
        if (found)
        {
          AUTO_TRACE_ADD("Documented base class '{}' templSpec='{}'",biName,templSpec);
          // add base class to this class

          // if templSpec is not empty then we should "instantiate"
          // the template baseClass. A new ClassDef should be created
          // to represent the instance. To be able to add the (instantiated)
          // members and documentation of a template class
          // (inserted in that template class at a later stage),
          // the template should know about its instances.
          // the instantiation process, should be done in a recursive way,
          // since instantiating a template may introduce new inheritance
          // relations.
          if (!templSpec.isEmpty() && mode==TemplateInstances)
          {
            // if baseClass is actually a typedef then we should not
            // instantiate it, since typedefs are in a different namespace
            // see bug531637 for an example where this would otherwise hang
            // Doxygen
            if (baseClassTypeDef==nullptr)
            {
              //printf("       => findTemplateInstanceRelation: %s\n",qPrint(baseClass->name()));
              findTemplateInstanceRelation(root,context,baseClass,templSpec,templateNames,baseClass->isArtificial());
            }
          }
          else if (mode==DocumentedOnly || mode==Undocumented)
          {
            //printf("       => insert base class\n");
            QCString usedName;
            if (baseClassTypeDef || cd->isCSharp())
            {
              usedName=biName;
              //printf("***** usedName=%s templSpec=%s\n",qPrint(usedName),qPrint(templSpec));
            }
            Protection prot = bi->prot;
            if (Config_getBool(SIP_SUPPORT)) prot=Protection::Public;
            if (cd!=baseClass && !cd->isSubClass(baseClass) && baseClass->isBaseClass(cd,true,templSpec)==0) // check for recursion, see bug690787
            {
              cd->insertBaseClass(baseClass,usedName,prot,bi->virt,templSpec);
              // add this class as super class to the base class
              baseClass->insertSubClass(cd,prot,bi->virt,templSpec);
            }
            else
            {
              warn(root->fileName,root->startLine,
                  "Detected potential recursive class relation "
                  "between class %s and base class %s!",
                  qPrint(cd->name()),qPrint(baseClass->name())
                  );
            }
          }
          return TRUE;
        }
        else if (mode==Undocumented && (scopeOffset==0 || isATemplateArgument))
        {
          AUTO_TRACE_ADD("New undocumented base class '{}' baseClassName='{}' templSpec='{}' isArtificial={}",
                       biName,baseClassName,templSpec,isArtificial);
          baseClass=nullptr;
          if (isATemplateArgument)
          {
            baseClass = toClassDefMutable(Doxygen::hiddenClassLinkedMap->find(baseClassName));
            if (baseClass==nullptr) // not found (or alias)
            {
              baseClass= toClassDefMutable(
                Doxygen::hiddenClassLinkedMap->add(baseClassName,
                  createClassDef(root->fileName,root->startLine,root->startColumn,
                               baseClassName,
                               ClassDef::Class)));
              if (baseClass) // really added (not alias)
              {
                if (isArtificial) baseClass->setArtificial(TRUE);
                baseClass->setLanguage(root->lang);
              }
            }
          }
          else
          {
            baseClass = toClassDefMutable(Doxygen::classLinkedMap->find(baseClassName));
            //printf("*** classDDict->find(%s)=%p biName=%s templSpec=%s\n",
            //    qPrint(baseClassName),baseClass,qPrint(biName),qPrint(templSpec));
            if (baseClass==nullptr) // not found (or alias)
            {
              baseClass = toClassDefMutable(
                  Doxygen::classLinkedMap->add(baseClassName,
                    createClassDef(root->fileName,root->startLine,root->startColumn,
                      baseClassName,
                      ClassDef::Class)));
              if (baseClass) // really added (not alias)
              {
                if (isArtificial) baseClass->setArtificial(TRUE);
                baseClass->setLanguage(root->lang);
                si = baseClassName.findRev("::");
                if (si!=-1) // class is nested
                {
                  Definition *sd = findScopeFromQualifiedName(Doxygen::globalScope,baseClassName.left(si),nullptr,root->tagInfo());
                  if (sd==nullptr || sd==Doxygen::globalScope) // outer scope not found
                  {
                    baseClass->setArtificial(TRUE); // see bug678139
                  }
                }
              }
            }
          }
          if (baseClass)
          {
            if (biName.endsWith("-p"))
            {
              biName="<"+biName.left(biName.length()-2)+">";
            }
            if (!cd->isSubClass(baseClass) && cd!=baseClass && cd->isBaseClass(baseClass,true,templSpec)==0) // check for recursion
            {
              // add base class to this class
              cd->insertBaseClass(baseClass,biName,bi->prot,bi->virt,templSpec);
              // add this class as super class to the base class
              baseClass->insertSubClass(cd,bi->prot,bi->virt,templSpec);
            }
            // the undocumented base was found in this file
            baseClass->insertUsedFile(root->fileDef());

            Definition *scope = buildScopeFromQualifiedName(baseClass->name(),root->lang,nullptr);
            if (scope!=baseClass)
            {
              baseClass->setOuterScope(scope);
            }

            if (baseClassName.endsWith("-p"))
            {
              baseClass->setCompoundType(ClassDef::Protocol);
            }
            return TRUE;
          }
          else
          {
            AUTO_TRACE_ADD("Base class '{}' not created (alias?)",biName);
          }
        }
        else
        {
          AUTO_TRACE_ADD("Base class '{}' not found",biName);
        }
      }
      else
      {
        if (mode!=TemplateInstances)
        {
          warn(root->fileName,root->startLine,
              "Detected potential recursive class relation "
              "between class %s and base class %s!",
              qPrint(root->name),qPrint(baseClassName)
              );
        }
        // for mode==TemplateInstance this case is quite common and
        // indicates a relation between a template class and a template
        // instance with the same name.
      }
      if (scopeOffset==0)
      {
        scopeOffset=-1;
      }
      else if ((scopeOffset=scopeName.findRev("::",scopeOffset-1))==-1)
      {
        scopeOffset=0;
      }
      //printf("new scopeOffset='%d'",scopeOffset);
    } while (scopeOffset>=0);

    if (parentNode==nullptr)
    {
      lastParent=TRUE;
    }
    else
    {
      parentNode=parentNode->parent();
    }
  } while (lastParent);

  return FALSE;
}

//----------------------------------------------------------------------
// Computes the base and super classes for each class in the tree

static bool isClassSection(const Entry *root)
{
  if ( !root->name.isEmpty() )
  {
    if (root->section.isCompound())
         // is it a compound (class, struct, union, interface ...)
    {
      return TRUE;
    }
    else if (root->section.isCompoundDoc())
         // is it a documentation block with inheritance info.
    {
      bool hasExtends = !root->extends.empty();
      if (hasExtends) return TRUE;
    }
  }
  return FALSE;
}


/*! Builds a dictionary of all entry nodes in the tree starting with \a root
 */
static void findClassEntries(const Entry *root)
{
  if (isClassSection(root))
  {
    g_classEntries.emplace(root->name.str(),root);
  }
  for (const auto &e : root->children()) findClassEntries(e.get());
}

static QCString extractClassName(const Entry *root)
{
  // strip any anonymous scopes first
  QCString bName=stripAnonymousNamespaceScope(root->name);
  bName=stripTemplateSpecifiersFromScope(bName);
  int i=0;
  if ((root->lang==SrcLangExt::CSharp || root->lang==SrcLangExt::Java) &&
      (i=bName.find('<'))!=-1)
  {
    // a Java/C# generic class looks like a C++ specialization, so we need to strip the
    // template part before looking for matches
    bName=bName.left(i);
  }
  return bName;
}

/*! Using the dictionary build by findClassEntries(), this
 *  function will look for additional template specialization that
 *  exists as inheritance relations only. These instances will be
 *  added to the template they are derived from.
 */
static void findInheritedTemplateInstances()
{
  AUTO_TRACE();
  ClassDefSet visitedClasses;
  for (const auto &[name,root] : g_classEntries)
  {
    QCString bName = extractClassName(root);
    ClassDefMutable *cdm = getClassMutable(bName);
    if (cdm)
    {
      findBaseClassesForClass(root,cdm,cdm,cdm,TemplateInstances,FALSE);
    }
  }
}

static void makeTemplateInstanceRelation(const Entry *root,ClassDefMutable *cd)
{
  AUTO_TRACE("root->name={} cd={}",root->name,cd->name());
  int i = root->name.find('<');
  if (i!=-1)
  {
    ClassDefMutable *master = getClassMutable(root->name.left(i));
    if (master && master!=cd && !cd->templateMaster())
    {
      AUTO_TRACE_ADD("class={} master={}",cd->name(),cd->templateMaster()?cd->templateMaster()->name():"<none>",master->name());
      cd->setTemplateMaster(master);
      master->insertExplicitTemplateInstance(cd,root->name.mid(i));
    }
  }
}

static void findUsedTemplateInstances()
{
  AUTO_TRACE();
  for (const auto &[name,root] : g_classEntries)
  {
    QCString bName = extractClassName(root);
    ClassDefMutable *cdm = getClassMutable(bName);
    if (cdm)
    {
      findUsedClassesForClass(root,cdm,cdm,cdm,TRUE);
      makeTemplateInstanceRelation(root,cdm);
      cdm->addTypeConstraints();
    }
  }
}

static void warnUndocumentedNamespaces()
{
  AUTO_TRACE();
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    if (!nd->hasDocumentation())
    {
      if ((guessSection(nd->getDefFileName()).isHeader() ||
           nd->getLanguage() == SrcLangExt::Fortran) &&     // Fortran doesn't have header files.
          !Config_getBool(HIDE_UNDOC_NAMESPACES)            // undocumented namespaces are visible
         )
      {
        warn_undoc(nd->getDefFileName(),nd->getDefLine(), "%s %s is not documented.",
                   nd->getLanguage() == SrcLangExt::Fortran ? "Module" : "Namespace",
                   qPrint(nd->name()));
      }
    }
  }
}

static void computeClassRelations()
{
  AUTO_TRACE();
  for (const auto &[name,root] : g_classEntries)
  {
    QCString bName = extractClassName(root);
    ClassDefMutable *cd = getClassMutable(bName);
    if (cd)
    {
      findBaseClassesForClass(root,cd,cd,cd,DocumentedOnly,FALSE);
    }
    size_t numMembers = cd ? cd->memberNameInfoLinkedMap().size() : 0;
    if ((cd==nullptr || (!cd->hasDocumentation() && !cd->isReference())) && numMembers>0 && !bName.endsWith("::"))
    {
      if (!root->name.isEmpty() && root->name.find('@')==-1 && // normal name
          (guessSection(root->fileName).isHeader() ||
           Config_getBool(EXTRACT_LOCAL_CLASSES)) && // not defined in source file
           protectionLevelVisible(root->protection) && // hidden by protection
           !Config_getBool(HIDE_UNDOC_CLASSES) // undocumented class are visible
         )
        warn_undoc(
                   root->fileName,root->startLine,
                   "Compound %s is not documented.",
                   qPrint(root->name)
             );
    }
  }
}

static void computeTemplateClassRelations()
{
  AUTO_TRACE();
  for (const auto &[name,root] : g_classEntries)
  {
    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    ClassDefMutable *cd=getClassMutable(bName);
    // strip any anonymous scopes first
    if (cd && !cd->getTemplateInstances().empty())
    {
      AUTO_TRACE_ADD("Template class '{}'",cd->name());
      for (const auto &ti : cd->getTemplateInstances()) // for each template instance
      {
        ClassDefMutable *tcd=toClassDefMutable(ti.classDef);
        if (tcd)
        {
          AUTO_TRACE_ADD("Template instance '{}'",tcd->name());
          QCString templSpec = ti.templSpec;
          std::unique_ptr<ArgumentList> templArgs = stringToArgumentList(tcd->getLanguage(),templSpec);
          for (const BaseInfo &bi : root->extends)
          {
            // check if the base class is a template argument
            BaseInfo tbi = bi;
            const ArgumentList &tl = cd->templateArguments();
            if (!tl.empty())
            {
              TemplateNameMap baseClassNames = tcd->getTemplateBaseClassNames();
              TemplateNameMap templateNames = getTemplateArgumentsInName(tl,bi.name.str());
              // for each template name that we inherit from we need to
              // substitute the formal with the actual arguments
              TemplateNameMap actualTemplateNames;
              for (const auto &tn_kv : templateNames)
              {
                size_t templIndex = tn_kv.second;
                Argument actArg;
                bool hasActArg=FALSE;
                if (templIndex<templArgs->size())
                {
                  actArg=templArgs->at(templIndex);
                  hasActArg=TRUE;
                }
                if (hasActArg &&
                    baseClassNames.find(actArg.type.str())!=baseClassNames.end() &&
                    actualTemplateNames.find(actArg.type.str())==actualTemplateNames.end()
                   )
                {
                  actualTemplateNames.emplace(actArg.type.str(),static_cast<int>(templIndex));
                }
              }

              tbi.name = substituteTemplateArgumentsInString(bi.name,tl,templArgs.get());
              // find a documented base class in the correct scope
              if (!findClassRelation(root,cd,tcd,&tbi,actualTemplateNames,DocumentedOnly,FALSE))
              {
                // no documented base class -> try to find an undocumented one
                findClassRelation(root,cd,tcd,&tbi,actualTemplateNames,Undocumented,TRUE);
              }
            }
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------
// compute the references (anchors in HTML) for each function in the file

static void computeMemberReferences()
{
  AUTO_TRACE();
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->computeAnchors();
    }
  }
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->computeAnchors();
    }
  }
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->computeAnchors();
    }
  }
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->computeAnchors();
  }
}

//----------------------------------------------------------------------

static void addListReferences()
{
  AUTO_TRACE();
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->addListReferences();
    }
  }

  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->addListReferences();
    }
  }

  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->addListReferences();
    }
  }

  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->addListReferences();
  }

  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    QCString name = pd->getOutputFileBase();
    if (pd->getGroupDef())
    {
      name = pd->getGroupDef()->getOutputFileBase();
    }
    {
      const RefItemVector &xrefItems = pd->xrefListItems();
      addRefItem(xrefItems,
          name,
          theTranslator->trPage(TRUE,TRUE),
          name,pd->title(),QCString(),nullptr);
    }
  }

  for (const auto &dd : *Doxygen::dirLinkedMap)
  {
    QCString name = dd->getOutputFileBase();
    //if (dd->getGroupDef())
    //{
    //  name = dd->getGroupDef()->getOutputFileBase();
    //}
    const RefItemVector &xrefItems = dd->xrefListItems();
    addRefItem(xrefItems,
        name,
        theTranslator->trDir(TRUE,TRUE),
        name,dd->displayName(),QCString(),nullptr);
  }

  ModuleManager::instance().addListReferences();
}

//----------------------------------------------------------------------

static void generateXRefPages()
{
  AUTO_TRACE();
  for (RefListManager::Ptr &rl : RefListManager::instance())
  {
    rl->generatePage();
  }
}

//----------------------------------------------------------------------
// Copy the documentation in entry 'root' to member definition 'md' and
// set the function declaration of the member to 'funcDecl'. If the boolean
// over_load is set the standard overload text is added.

static void addMemberDocs(const Entry *root,
                   MemberDefMutable *md, const QCString &funcDecl,
                   const ArgumentList *al,
                   bool over_load,
                   TypeSpecifier spec
                  )
{
  if (md==nullptr) return;
  AUTO_TRACE("scope='{}' name='{}' args='{}' funcDecl='{}' mSpec={}",
       root->parent()->name,md->name(),md->argsString(),funcDecl,spec);
  if (!root->section.isDoc()) // @fn or @var does not need to specify the complete definition, so don't overwrite it
  {
    QCString fDecl=funcDecl;
    // strip extern specifier
    fDecl.stripPrefix("extern ");
    md->setDefinition(fDecl);
  }
  applyMemberOverrideOptions(root,md);
  md->addQualifiers(root->qualifiers);
  ClassDefMutable *cd=md->getClassDefMutable();
  const NamespaceDef *nd=md->getNamespaceDef();
  QCString fullName;
  if (cd)
    fullName = cd->name();
  else if (nd)
    fullName = nd->name();

  if (!fullName.isEmpty()) fullName+="::";
  fullName+=md->name();
  FileDef *rfd=root->fileDef();

  // TODO determine scope based on root not md
  Definition *rscope = md->getOuterScope();

  const ArgumentList &mdAl = md->argumentList();
  if (al)
  {
    ArgumentList mergedAl = *al;
    //printf("merging arguments (1) docs=%d\n",root->doc.isEmpty());
    mergeArguments(const_cast<ArgumentList&>(mdAl),mergedAl,!root->doc.isEmpty());
  }
  else
  {
    if (
          matchArguments2( md->getOuterScope(), md->getFileDef(),const_cast<ArgumentList*>(&mdAl),
                           rscope,rfd,&root->argList,
                           TRUE, root->lang
                         )
       )
    {
      //printf("merging arguments (2)\n");
      ArgumentList mergedArgList = root->argList;
      mergeArguments(const_cast<ArgumentList&>(mdAl),mergedArgList,!root->doc.isEmpty());
    }
  }
  if (over_load)  // the \overload keyword was used
  {
    QCString doc=getOverloadDocs();
    if (!root->doc.isEmpty())
    {
      doc+="<p>";
      doc+=root->doc;
    }
    md->setDocumentation(doc,root->docFile,root->docLine);
    md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    md->setDocsForDefinition(!root->proto);
  }
  else
  {
    //printf("overwrite!\n");
    md->setDocumentation(root->doc,root->docFile,root->docLine);
    md->setDocsForDefinition(!root->proto);

    //printf("overwrite!\n");
    md->setBriefDescription(root->brief,root->briefFile,root->briefLine);

    if (
        (md->inbodyDocumentation().isEmpty() ||
         !root->parent()->name.isEmpty()
        ) && !root->inbodyDocs.isEmpty()
       )
    {
      md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    }
  }

  //printf("initializer: '%s'(isEmpty=%d) '%s'(isEmpty=%d)\n",
  //    qPrint(md->initializer()),md->initializer().isEmpty(),
  //    qPrint(root->initializer),root->initializer.isEmpty()
  //   );
  std::string rootInit = root->initializer.str();
  if (md->initializer().isEmpty() && !rootInit.empty())
  {
    //printf("setInitializer\n");
    md->setInitializer(rootInit.c_str());
  }
  if (md->requiresClause().isEmpty() && !root->req.isEmpty())
  {
    md->setRequiresClause(root->req);
  }

  md->setMaxInitLines(root->initLines);

  if (rfd)
  {
    if ((md->getStartBodyLine()==-1 && root->bodyLine!=-1)
       )
    {
      //printf("Setting new body segment [%d,%d]\n",root->bodyLine,root->endBodyLine);
      md->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
      md->setBodyDef(rfd);
    }

    md->setRefItems(root->sli);
  }

  applyMemberOverrideOptions(root,md);
  md->addQualifiers(root->qualifiers);

  md->mergeMemberSpecifiers(spec);
  md->addSectionsToDefinition(root->anchors);
  addMemberToGroups(root,md);
  ModuleManager::instance().addMemberToModule(root,md);
  if (cd) cd->insertUsedFile(rfd);
  //printf("root->mGrpId=%d\n",root->mGrpId);
  if (root->mGrpId!=-1)
  {
    if (md->getMemberGroupId()!=-1)
    {
      if (md->getMemberGroupId()!=root->mGrpId)
      {
        warn(
             root->fileName,root->startLine,
             "member %s belongs to two different groups. The second "
             "one found here will be ignored.",
             qPrint(md->name())
            );
      }
    }
    else // set group id
    {
      //printf("setMemberGroupId=%d md=%s\n",root->mGrpId,qPrint(md->name()));
      md->setMemberGroupId(root->mGrpId);
    }
  }
  md->addQualifiers(root->qualifiers);
}

//----------------------------------------------------------------------
// find a class definition given the scope name and (optionally) a
// template list specifier

static const ClassDef *findClassDefinition(FileDef *fd,NamespaceDef *nd,
                         const QCString &scopeName)
{
  SymbolResolver resolver(fd);
  const ClassDef *tcd = resolver.resolveClass(nd,scopeName,true,true);
  return tcd;
}

//----------------------------------------------------------------------------
// Returns TRUE, if the entry belongs to the group of the member definition,
// otherwise FALSE.

static bool isEntryInGroupOfMember(const Entry *root,const MemberDef *md,bool allowNoGroup=false)
{
  const GroupDef *gd = md->getGroupDef();
  if (!gd)
  {
    return allowNoGroup;
  }

  for (const auto &g : root->groups)
  {
    if (g.groupname == gd->name())
    {
      return true; // matching group
    }
  }

  return false;
}

//----------------------------------------------------------------------
// Adds the documentation contained in 'root' to a global function
// with name 'name' and argument list 'args' (for overloading) and
// function declaration 'decl' to the corresponding member definition.

static bool findGlobalMember(const Entry *root,
                           const QCString &namespaceName,
                           const QCString &type,
                           const QCString &name,
                           const QCString &tempArg,
                           const QCString &,
                           const QCString &decl,
                           TypeSpecifier /* spec */)
{
  AUTO_TRACE("namespace='{}' type='{}' name='{}' tempArg='{}' decl='{}'",namespaceName,type,name,tempArg,decl);
  QCString n=name;
  if (n.isEmpty()) return FALSE;
  if (n.find("::")!=-1) return FALSE; // skip undefined class members
  MemberName *mn=Doxygen::functionNameLinkedMap->find(n+tempArg); // look in function dictionary
  if (mn==nullptr)
  {
    mn=Doxygen::functionNameLinkedMap->find(n); // try without template arguments
  }
  if (mn) // function name defined
  {
    AUTO_TRACE_ADD("Found symbol name");
    //int count=0;
    bool found=FALSE;
    for (const auto &md : *mn)
    {
      // If the entry has groups, then restrict the search to members which are
      // in one of the groups of the entry. If md is not associated with a group yet,
      // allow this documentation entry to add the group info.
      if (!root->groups.empty() && !isEntryInGroupOfMember(root, md.get(), true))
      {
        continue;
      }

      const NamespaceDef *nd=nullptr;
      if (md->isAlias() && md->getOuterScope() &&
          md->getOuterScope()->definitionType()==Definition::TypeNamespace)
      {
        nd = toNamespaceDef(md->getOuterScope());
      }
      else
      {
        nd = md->getNamespaceDef();
      }

      // special case for strong enums
      int enumNamePos=0;
      if (nd && md->isEnumValue() && (enumNamePos=namespaceName.findRev("::"))!=-1)
      { // md part of a strong enum in a namespace?
        QCString enumName = namespaceName.mid(enumNamePos+2);
        if (namespaceName.left(enumNamePos)==nd->name())
        {
          MemberName *enumMn=Doxygen::functionNameLinkedMap->find(enumName);
          if (enumMn)
          {
            for (const auto &emd : *enumMn)
            {
              found = emd->isStrong() && md->getEnumScope()==emd.get();
              if (found)
              {
                addMemberDocs(root,toMemberDefMutable(md->resolveAlias()),decl,nullptr,FALSE,root->spec);
                break;
              }
            }
          }
        }
        if (found)
        {
          break;
        }
      }
      else if (nd==nullptr && md->isEnumValue()) // md part of global strong enum?
      {
        MemberName *enumMn=Doxygen::functionNameLinkedMap->find(namespaceName);
        if (enumMn)
        {
          for (const auto &emd : *enumMn)
          {
            found = emd->isStrong() && md->getEnumScope()==emd.get();
            if (found)
            {
              addMemberDocs(root,toMemberDefMutable(md->resolveAlias()),decl,nullptr,FALSE,root->spec);
              break;
            }
          }
        }
      }

      const FileDef *fd=root->fileDef();
      //printf("File %s\n",fd ? qPrint(fd->name()) : "<none>");
      LinkedRefMap<NamespaceDef> nl;
      if (fd)
      {
        nl = fd->getUsedNamespaces();
      }
      //printf("NamespaceList %p\n",nl);

      // search in the list of namespaces that are imported via a
      // using declaration
      bool viaUsingDirective = nd && nl.find(nd->qualifiedName())!=nullptr;

      if ((namespaceName.isEmpty() && nd==nullptr) ||  // not in a namespace
          (nd && nd->name()==namespaceName) ||   // or in the same namespace
          viaUsingDirective                      // member in 'using' namespace
         )
      {
        AUTO_TRACE_ADD("Try to add member '{}' to scope '{}'",md->name(),namespaceName);

        NamespaceDef *rnd = nullptr;
        if (!namespaceName.isEmpty()) rnd = Doxygen::namespaceLinkedMap->find(namespaceName);

        const ArgumentList &mdAl = md.get()->argumentList();
        bool matching=
          (mdAl.empty() && root->argList.empty()) ||
          md->isVariable() || md->isTypedef() || /* in case of function pointers */
          matchArguments2(md->getOuterScope(),md.get()->getFileDef(),&mdAl,
                          rnd ? rnd : Doxygen::globalScope,fd,&root->argList,
                          FALSE,root->lang);

        // for template members we need to check if the number of
        // template arguments is the same, otherwise we are dealing with
        // different functions.
        if (matching && !root->tArgLists.empty())
        {
          const ArgumentList &mdTempl = md->templateArguments();
          if (root->tArgLists.back().size()!=mdTempl.size())
          {
            matching=FALSE;
          }
        }

        //printf("%s<->%s\n",
        //    qPrint(argListToString(md->argumentList())),
        //    qPrint(argListToString(root->argList)));

        // For static members we also check if the comment block was found in
        // the same file. This is needed because static members with the same
        // name can be in different files. Thus it would be wrong to just
        // put the comment block at the first syntactically matching member. If
        // the comment block belongs to a group of the static member, then add
        // the documentation even if it is in a different file.
        if (matching && md->isStatic() &&
            md->getDefFileName()!=root->fileName &&
            mn->size()>1 &&
            !isEntryInGroupOfMember(root,md.get()))
        {
          matching = FALSE;
        }

        // for template member we also need to check the return type and requires
        if (!md->templateArguments().empty() && !root->tArgLists.empty())
        {
          //printf("Comparing return types '%s'<->'%s'\n",
          //    md->typeString(),type);
          if (md->templateArguments().size()!=root->tArgLists.back().size() ||
              md->typeString()!=type ||
              md->requiresClause()!=root->req)
          {
            //printf(" ---> no matching\n");
            matching = FALSE;
          }
        }

        if (matching) // add docs to the member
        {
          AUTO_TRACE_ADD("Match found");
          addMemberDocs(root,toMemberDefMutable(md->resolveAlias()),decl,&root->argList,FALSE,root->spec);
          found=TRUE;
          break;
        }
      }
    }
    if (!found && root->relatesType!=RelatesType::Duplicate && root->section.isFunction()) // no match
    {
      QCString fullFuncDecl=decl;
      if (!root->argList.empty()) fullFuncDecl+=argListToString(root->argList,TRUE);
      QCString warnMsg =
         QCString("no matching file member found for \n")+substitute(fullFuncDecl,"%","%%");
      if (mn->size()>0)
      {
        warnMsg+="\nPossible candidates:";
        for (const auto &md : *mn)
        {
          warnMsg+="\n  '";
          warnMsg+=substitute(md->declaration(),"%","%%");
          warnMsg+="' " + warn_line(md->getDefFileName(),md->getDefLine());
        }
      }
      warn(root->fileName,root->startLine, "%s", qPrint(warnMsg));
    }
  }
  else // got docs for an undefined member!
  {
    if (root->type!="friend class" &&
        root->type!="friend struct" &&
        root->type!="friend union" &&
        root->type!="friend" &&
        (!Config_getBool(TYPEDEF_HIDES_STRUCT) ||
         root->type.find("typedef ")==-1)
       )
    {
      warn(root->fileName,root->startLine,
           "documented symbol '%s' was not declared or defined.",qPrint(decl)
          );
    }
  }
  return TRUE;
}

static bool isSpecialization(
                  const ArgumentLists &srcTempArgLists,
                  const ArgumentLists &dstTempArgLists
    )
{
    auto srcIt = srcTempArgLists.begin();
    auto dstIt = dstTempArgLists.begin();
    while (srcIt!=srcTempArgLists.end() && dstIt!=dstTempArgLists.end())
    {
      if ((*srcIt).size()!=(*dstIt).size()) return TRUE;
      ++srcIt;
      ++dstIt;
    }
    return FALSE;
}

static bool scopeIsTemplate(const Definition *d)
{
  bool result=FALSE;
  if (d && d->definitionType()==Definition::TypeClass)
  {
    result = !(toClassDef(d))->templateArguments().empty() ||
             scopeIsTemplate(d->getOuterScope());
  }
  return result;
}

static QCString substituteTemplatesInString(
    const ArgumentLists &srcTempArgLists,
    const ArgumentLists &dstTempArgLists,
    const std::string &src
    )
{
  std::string dst;
  static const reg::Ex re(R"(\a\w*)");
  reg::Iterator it(src,re);
  reg::Iterator end;
  //printf("type=%s\n",qPrint(sa->type));
  size_t p=0;
  for (; it!=end ; ++it) // for each word in srcType
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    bool found=FALSE;
    dst+=src.substr(p,i-p);
    std::string name=match.str();

    auto srcIt = srcTempArgLists.begin();
    auto dstIt = dstTempArgLists.begin();
    while (srcIt!=srcTempArgLists.end() && !found)
    {
      const ArgumentList *tdAli = nullptr;
      std::vector<Argument>::const_iterator tdaIt;
      if (dstIt!=dstTempArgLists.end())
      {
        tdAli = &(*dstIt);
        tdaIt = tdAli->begin();
        ++dstIt;
      }

      const ArgumentList &tsaLi = *srcIt;
      for (auto tsaIt = tsaLi.begin(); tsaIt!=tsaLi.end() && !found; ++tsaIt)
      {
        Argument tsa = *tsaIt;
        const Argument *tda = nullptr;
        if (tdAli && tdaIt!=tdAli->end())
        {
          tda = &(*tdaIt);
          ++tdaIt;
        }
        //if (tda) printf("tsa=%s|%s tda=%s|%s\n",
        //    qPrint(tsa.type),qPrint(tsa.name),
        //    qPrint(tda->type),qPrint(tda->name));
        if (name==tsa.name.str())
        {
          if (tda && tda->name.isEmpty())
          {
            QCString tdaName = tda->name;
            QCString tdaType = tda->type;
            int vc=0;
            if      (tdaType.startsWith("class "))    vc=6;
            else if (tdaType.startsWith("typename ")) vc=9;
            if (vc>0) // convert type=="class T" to type=="class" name=="T"
            {
              tdaName = tdaType.mid(vc);
            }
            if (!tdaName.isEmpty())
            {
              name=tdaName.str(); // substitute
              found=TRUE;
            }
          }
        }
      }

      //printf("   srcList='%s' dstList='%s faList='%s'\n",
      //  qPrint(argListToString(srclali.current())),
      //  qPrint(argListToString(dstlali.current())),
      //  funcTempArgList ? qPrint(argListToString(funcTempArgList)) : "<none>");
      ++srcIt;
    }
    dst+=name;
    p=i+l;
  }
  dst+=src.substr(p);
  //printf("  substituteTemplatesInString(%s)=%s\n",
  //    qPrint(src),qPrint(dst));
  return QCString(dst);
}

static void substituteTemplatesInArgList(
                  const ArgumentLists &srcTempArgLists,
                  const ArgumentLists &dstTempArgLists,
                  const ArgumentList &src,
                  ArgumentList &dst
                 )
{
  auto dstIt = dst.begin();
  for (const Argument &sa : src)
  {
    QCString dstType =  substituteTemplatesInString(srcTempArgLists,dstTempArgLists,sa.type.str());
    QCString dstArray = substituteTemplatesInString(srcTempArgLists,dstTempArgLists,sa.array.str());
    if (dstIt == dst.end())
    {
      Argument da = sa;
      da.type  = dstType;
      da.array = dstArray;
      dst.push_back(da);
      dstIt = dst.end();
    }
    else
    {
      Argument da = *dstIt;
      da.type  = dstType;
      da.array = dstArray;
      ++dstIt;
    }
  }
  dst.setConstSpecifier(src.constSpecifier());
  dst.setVolatileSpecifier(src.volatileSpecifier());
  dst.setPureSpecifier(src.pureSpecifier());
  dst.setTrailingReturnType(substituteTemplatesInString(
                             srcTempArgLists,dstTempArgLists,
                             src.trailingReturnType().str()));
  dst.setIsDeleted(src.isDeleted());
  dst.setRefQualifier(src.refQualifier());
  dst.setNoParameters(src.noParameters());
  //printf("substituteTemplatesInArgList: replacing %s with %s\n",
  //    qPrint(argListToString(src)),qPrint(argListToString(dst))
  //    );
}

//-------------------------------------------------------------------------------------------

static void addLocalObjCMethod(const Entry *root,
                        const QCString &scopeName,
                        const QCString &funcType,const QCString &funcName,const QCString &funcArgs,
                        const QCString &exceptions,const QCString &funcDecl,
                        TypeSpecifier spec)
{
  AUTO_TRACE();
  //printf("scopeName='%s' className='%s'\n",qPrint(scopeName),qPrint(className));
  ClassDefMutable *cd=nullptr;
  if (Config_getBool(EXTRACT_LOCAL_METHODS) && (cd=getClassMutable(scopeName)))
  {
    AUTO_TRACE_ADD("Local objective C method '{}' scopeName='{}'",root->name,scopeName);
    auto md = createMemberDef(
        root->fileName,root->startLine,root->startColumn,
        funcType,funcName,funcArgs,exceptions,
        root->protection,root->virt,root->isStatic,Relationship::Member,
        MemberType::Function,ArgumentList(),root->argList,root->metaData);
    auto mmd = toMemberDefMutable(md.get());
    mmd->setTagInfo(root->tagInfo());
    mmd->setLanguage(root->lang);
    mmd->setId(root->id);
    mmd->makeImplementationDetail();
    mmd->setMemberClass(cd);
    mmd->setDefinition(funcDecl);
    applyMemberOverrideOptions(root,mmd);
    mmd->addQualifiers(root->qualifiers);
    mmd->setDocumentation(root->doc,root->docFile,root->docLine);
    mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    mmd->setDocsForDefinition(!root->proto);
    mmd->setPrototype(root->proto,root->fileName,root->startLine,root->startColumn);
    mmd->addSectionsToDefinition(root->anchors);
    mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
    FileDef *fd=root->fileDef();
    mmd->setBodyDef(fd);
    mmd->setMemberSpecifiers(spec);
    mmd->setVhdlSpecifiers(root->vhdlSpec);
    mmd->setMemberGroupId(root->mGrpId);
    cd->insertMember(md.get());
    cd->insertUsedFile(fd);
    mmd->setRefItems(root->sli);

    MemberName *mn = Doxygen::memberNameLinkedMap->add(root->name);
    mn->push_back(std::move(md));
  }
  else
  {
    // local objective C method found for class without interface
  }
}

//-------------------------------------------------------------------------------------------

static void addMemberFunction(const Entry *root,
                       MemberName *mn,
                       const QCString &scopeName,
                       const QCString &namespaceName,
                       const QCString &className,
                       const QCString &funcTyp,
                       const QCString &funcName,
                       const QCString &funcArgs,
                       const QCString &funcTempList,
                       const QCString &exceptions,
                       const QCString &type,
                       const QCString &args,
                       bool isFriend,
                       TypeSpecifier spec,
                       const QCString &relates,
                       const QCString &funcDecl,
                       bool overloaded,
                       bool isFunc)
{
  AUTO_TRACE();
  QCString funcType = funcTyp;
  int count=0;
  int noMatchCount=0;
  bool memFound=FALSE;
  for (const auto &imd : *mn)
  {
    MemberDefMutable *md = toMemberDefMutable(imd.get());
    if (md==nullptr) continue;
    ClassDefMutable *cd=md->getClassDefMutable();
    if (cd==nullptr) continue;
    //AUTO_TRACE_ADD("member definition found, scope needed='{}' scope='{}' args='{}' fileName='{}'",
    //    scopeName, cd->name(), md->argsString(), root->fileName);
    FileDef *fd=root->fileDef();
    NamespaceDef *nd=nullptr;
    if (!namespaceName.isEmpty()) nd=getResolvedNamespace(namespaceName);

    //printf("scopeName %s->%s\n",qPrint(scopeName),
    //       qPrint(stripTemplateSpecifiersFromScope(scopeName,FALSE)));

    // if the member we are searching for is an enum value that is part of
    // a "strong" enum, we need to look into the fields of the enum for a match
    int enumNamePos=0;
    if (md->isEnumValue() && (enumNamePos=className.findRev("::"))!=-1)
    {
      QCString enumName = className.mid(enumNamePos+2);
      QCString fullScope = className.left(enumNamePos);
      if (!namespaceName.isEmpty()) fullScope.prepend(namespaceName+"::");
      if (fullScope==cd->name())
      {
        MemberName *enumMn=Doxygen::memberNameLinkedMap->find(enumName);
        //printf("enumMn(%s)=%p\n",qPrint(className),(void*)enumMn);
        if (enumMn)
        {
          for (const auto &emd : *enumMn)
          {
            memFound = emd->isStrong() && md->getEnumScope()==emd.get();
            if (memFound)
            {
              addMemberDocs(root,md,funcDecl,nullptr,overloaded,spec);
              count++;
            }
            if (memFound) break;
          }
        }
      }
    }
    if (memFound) break;

    const ClassDef *tcd=findClassDefinition(fd,nd,scopeName);
    if (tcd==nullptr && cd && stripAnonymousNamespaceScope(cd->name())==scopeName)
    {
      // don't be fooled by anonymous scopes
      tcd=cd;
    }
    //printf("Looking for %s inside nd=%s result=%p (%s) cd=%p\n",
    //    qPrint(scopeName),nd?qPrint(nd->name()):"<none>",tcd,tcd?qPrint(tcd->name()):"",cd);

    if (cd && tcd==cd) // member's classes match
    {
      AUTO_TRACE_ADD("class definition '{}' found",cd->name());

      // get the template parameter lists found at the member declaration
      ArgumentLists declTemplArgs = cd->getTemplateParameterLists();
      const ArgumentList &templAl = md->templateArguments();
      if (!templAl.empty())
      {
        declTemplArgs.push_back(templAl);
      }

      // get the template parameter lists found at the member definition
      const ArgumentLists &defTemplArgs = root->tArgLists;
      //printf("defTemplArgs=%p\n",defTemplArgs);

      // do we replace the decl argument lists with the def argument lists?
      bool substDone=FALSE;
      ArgumentList argList;

      /* substitute the occurrences of class template names in the
       * argument list before matching
       */
      const ArgumentList &mdAl = md->argumentList();
      if (declTemplArgs.size()>0 && declTemplArgs.size()==defTemplArgs.size())
      {
        /* the function definition has template arguments
         * and the class definition also has template arguments, so
         * we must substitute the template names of the class by that
         * of the function definition before matching.
         */
        substituteTemplatesInArgList(declTemplArgs,defTemplArgs,mdAl,argList);

        substDone=TRUE;
      }
      else /* no template arguments, compare argument lists directly */
      {
        argList = mdAl;
      }

      AUTO_TRACE_ADD("matching '{}'<=>'{}' className='{}' namespaceName='{}'",
          argListToString(argList,TRUE),argListToString(root->argList,TRUE),className,namespaceName);

      bool matching=
        md->isVariable() || md->isTypedef() || // needed for function pointers
        matchArguments2(
            md->getClassDef(),md->getFileDef(),&argList,
            cd,fd,&root->argList,
            TRUE,root->lang);

      if (md->getLanguage()==SrcLangExt::ObjC && md->isVariable() && root->section.isFunction())
      {
        matching = FALSE; // don't match methods and attributes with the same name
      }

      // for template member we also need to check the return type
      if (!md->templateArguments().empty() && !root->tArgLists.empty())
      {
        QCString memType = md->typeString();
        memType.stripPrefix("static "); // see bug700696
        funcType=substitute(stripTemplateSpecifiersFromScope(funcType,TRUE),
            className+"::",""); // see bug700693 & bug732594
        memType=substitute(stripTemplateSpecifiersFromScope(memType,TRUE),
            className+"::",""); // see bug758900
        AUTO_TRACE_ADD("Comparing return types '{}'<->'{}' #args {}<->{}",
            md->typeString(),funcType,md->templateArguments().size(),root->tArgLists.back().size());
        if (md->templateArguments().size()!=root->tArgLists.back().size() || memType!=funcType)
        {
          //printf(" ---> no matching\n");
          matching = FALSE;
        }
      }
      else if (defTemplArgs.size()>declTemplArgs.size())
      {
        // avoid matching a non-template function in a template class against a
        // template function with the same name and parameters, see issue #10184
        substDone = false;
        matching  = false;
      }
      bool rootIsUserDoc   = root->section.isMemberDoc();
      bool classIsTemplate = scopeIsTemplate(md->getClassDef());
      bool mdIsTemplate    = md->templateArguments().hasParameters();
      bool classOrMdIsTemplate = mdIsTemplate || classIsTemplate;
      bool rootIsTemplate  = !root->tArgLists.empty();
      //printf("classIsTemplate=%d mdIsTemplate=%d rootIsTemplate=%d\n",classIsTemplate,mdIsTemplate,rootIsTemplate);
      if (!rootIsUserDoc && // don't check out-of-line @fn references, see bug722457
          (mdIsTemplate || rootIsTemplate) && // either md or root is a template
          ((classOrMdIsTemplate && !rootIsTemplate) || (!classOrMdIsTemplate && rootIsTemplate))
         )
      {
        // Method with template return type does not match method without return type
        // even if the parameters are the same. See also bug709052
        AUTO_TRACE_ADD("Comparing return types: template v.s. non-template");
        matching = FALSE;
      }

      AUTO_TRACE_ADD("Match results of matchArguments2='{}' substDone='{}'",matching,substDone);

      if (substDone) // found a new argument list
      {
        if (matching) // replace member's argument list
        {
          md->setDefinitionTemplateParameterLists(root->tArgLists);
          md->moveArgumentList(std::make_unique<ArgumentList>(argList));
        }
        else // no match
        {
          if (!funcTempList.isEmpty() &&
              isSpecialization(declTemplArgs,defTemplArgs))
          {
            // check if we are dealing with a partial template
            // specialization. In this case we add it to the class
            // even though the member arguments do not match.

            addMethodToClass(root,cd,type,md->name(),args,isFriend,
                md->protection(),md->isStatic(),md->virtualness(),spec,relates);
            return;
          }
        }
      }
      if (matching)
      {
        addMemberDocs(root,md,funcDecl,nullptr,overloaded,spec);
        count++;
        memFound=TRUE;
      }
    }
    else if (cd && cd!=tcd) // we did find a class with the same name as cd
      // but in a different namespace
    {
      noMatchCount++;
    }

    if (memFound) break;
  }
  if (count==0 && root->parent() && root->parent()->section.isObjcImpl())
  {
    addLocalObjCMethod(root,scopeName,funcType,funcName,funcArgs,exceptions,funcDecl,spec);
    return;
  }
  if (count==0 && !(isFriend && funcType=="class"))
  {
    int candidates=0;
    const ClassDef *ecd = nullptr, *ucd = nullptr;
    MemberDef *emd = nullptr, *umd = nullptr;
    //printf("Assume template class\n");
    for (const auto &md : *mn)
    {
      MemberDef *cmd=md.get();
      MemberDefMutable *cdmdm = toMemberDefMutable(cmd);
      ClassDefMutable *ccd=cdmdm ? cdmdm->getClassDefMutable() : nullptr;
      //printf("ccd->name()==%s className=%s\n",qPrint(ccd->name()),qPrint(className));
      if (ccd!=nullptr && rightScopeMatch(ccd->name(),className))
      {
        const ArgumentList &templAl = md->templateArguments();
        if (!root->tArgLists.empty() && !templAl.empty() &&
            root->tArgLists.back().size()<=templAl.size())
        {
          AUTO_TRACE_ADD("add template specialization");
          addMethodToClass(root,ccd,type,md->name(),args,isFriend,
              root->protection,root->isStatic,root->virt,spec,relates);
          return;
        }
        if (argListToString(md->argumentList(),FALSE,FALSE) ==
            argListToString(root->argList,FALSE,FALSE))
        { // exact argument list match -> remember
          ucd = ecd = ccd;
          umd = emd = cmd;
          AUTO_TRACE_ADD("new candidate className='{}' scope='{}' args='{}': exact match",
              className,ccd->name(),md->argsString());
        }
        else // arguments do not match, but member name and scope do -> remember
        {
          ucd = ccd;
          umd = cmd;
          AUTO_TRACE_ADD("new candidate className='{}' scope='{}' args='{}': no match",
              className,ccd->name(),md->argsString());
        }
        candidates++;
      }
    }
    bool strictProtoMatching = Config_getBool(STRICT_PROTO_MATCHING);
    if (!strictProtoMatching)
    {
      if (candidates==1 && ucd && umd)
      {
        // we didn't find an actual match on argument lists, but there is only 1 member with this
        // name in the same scope, so that has to be the one.
        addMemberDocs(root,toMemberDefMutable(umd),funcDecl,nullptr,overloaded,spec);
        return;
      }
      else if (candidates>1 && ecd && emd)
      {
        // we didn't find a unique match using type resolution,
        // but one of the matches has the exact same signature so
        // we take that one.
        addMemberDocs(root,toMemberDefMutable(emd),funcDecl,nullptr,overloaded,spec);
        return;
      }
    }

    QCString warnMsg = "no ";
    if (noMatchCount>1) warnMsg+="uniquely ";
    warnMsg+="matching class member found for \n";

    for (const ArgumentList &al : root->tArgLists)
    {
      warnMsg+="  template ";
      warnMsg+=tempArgListToString(al,root->lang);
      warnMsg+='\n';
    }

    QCString fullFuncDecl=funcDecl;
    if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);

    warnMsg+="  ";
    warnMsg+=fullFuncDecl;

    if (candidates>0 || noMatchCount>=1)
    {
      warnMsg+="\nPossible candidates:";

      NamespaceDef *nd=nullptr;
      if (!namespaceName.isEmpty()) nd=getResolvedNamespace(namespaceName);
      FileDef *fd=root->fileDef();

      for (const auto &md : *mn)
      {
        const ClassDef *cd=md->getClassDef();
        const ClassDef *tcd=findClassDefinition(fd,nd,scopeName);
        if (tcd==nullptr && cd && stripAnonymousNamespaceScope(cd->name())==scopeName)
        {
          // don't be fooled by anonymous scopes
          tcd=cd;
        }
        if (cd!=nullptr && (rightScopeMatch(cd->name(),className) || (cd!=tcd)))
        {
          warnMsg+='\n';
          const ArgumentList &templAl = md->templateArguments();
          warnMsg+="  '";
          if (templAl.hasParameters())
          {
            warnMsg+="template ";
            warnMsg+=tempArgListToString(templAl,root->lang);
            warnMsg+='\n';
            warnMsg+="  ";
          }
          if (!md->typeString().isEmpty())
          {
            warnMsg+=md->typeString();
            warnMsg+=' ';
          }
          QCString qScope = cd->qualifiedNameWithTemplateParameters();
          if (!qScope.isEmpty())
            warnMsg+=qScope+"::"+md->name();
          warnMsg+=md->argsString();
          warnMsg+="' " + warn_line(md->getDefFileName(),md->getDefLine());
        }
      }
    }
    warn(root->fileName,root->startLine,"%s",qPrint(warnMsg));
  }
}

//-------------------------------------------------------------------------------------------

static void addMemberSpecialization(const Entry *root,
                             MemberName *mn,
                             ClassDefMutable *cd,
                             const QCString &funcType,
                             const QCString &funcName,
                             const QCString &funcArgs,
                             const QCString &funcDecl,
                             const QCString &exceptions,
                             TypeSpecifier spec
                            )
{
  AUTO_TRACE("funcType={} funcName={} funcArgs={} funcDecl={} spec={}",funcType,funcName,funcArgs,funcDecl,spec);
  MemberDef *declMd=nullptr;
  for (const auto &md : *mn)
  {
    if (md->getClassDef()==cd)
    {
      // TODO: we should probably also check for matching arguments
      declMd = md.get();
      break;
    }
  }
  MemberType mtype=MemberType::Function;
  ArgumentList tArgList;
  //  getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
  auto md = createMemberDef(
      root->fileName,root->startLine,root->startColumn,
      funcType,funcName,funcArgs,exceptions,
      declMd ? declMd->protection() : root->protection,
      root->virt,root->isStatic,Relationship::Member,
      mtype,tArgList,root->argList,root->metaData);
  auto mmd = toMemberDefMutable(md.get());
  //printf("new specialized member %s args='%s'\n",qPrint(md->name()),qPrint(funcArgs));
  mmd->setTagInfo(root->tagInfo());
  mmd->setLanguage(root->lang);
  mmd->setId(root->id);
  mmd->setMemberClass(cd);
  mmd->setTemplateSpecialization(TRUE);
  mmd->setTypeConstraints(root->typeConstr);
  mmd->setDefinition(funcDecl);
  applyMemberOverrideOptions(root,mmd);
  mmd->addQualifiers(root->qualifiers);
  mmd->setDocumentation(root->doc,root->docFile,root->docLine);
  mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  mmd->setDocsForDefinition(!root->proto);
  mmd->setPrototype(root->proto,root->fileName,root->startLine,root->startColumn);
  mmd->addSectionsToDefinition(root->anchors);
  mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
  FileDef *fd=root->fileDef();
  mmd->setBodyDef(fd);
  mmd->setMemberSpecifiers(spec);
  mmd->setVhdlSpecifiers(root->vhdlSpec);
  mmd->setMemberGroupId(root->mGrpId);
  cd->insertMember(md.get());
  mmd->setRefItems(root->sli);

  mn->push_back(std::move(md));
}

//-------------------------------------------------------------------------------------------

static void addOverloaded(const Entry *root,MemberName *mn,
                          const QCString &funcType,const QCString &funcName,const QCString &funcArgs,
                          const QCString &funcDecl,const QCString &exceptions,TypeSpecifier spec)
{
  // for unique overloaded member we allow the class to be
  // omitted, this is to be Qt compatible. Using this should
  // however be avoided, because it is error prone
  bool sameClass=false;
  if (mn->size()>0)
  {
    // check if all members with the same name are also in the same class
    sameClass = std::equal(mn->begin()+1,mn->end(),mn->begin(),
                  [](const auto &md1,const auto &md2)
                  { return md1->getClassDef()->name()==md2->getClassDef()->name(); });
  }
  if (sameClass)
  {
    MemberDefMutable *mdm = toMemberDefMutable(mn->front().get());
    ClassDefMutable *cd = mdm ? mdm->getClassDefMutable() : nullptr;
    if (cd==nullptr) return;

    MemberType mtype = MemberType::Function;
    if      (root->mtype==MethodTypes::Signal)  mtype=MemberType::Signal;
    else if (root->mtype==MethodTypes::Slot)    mtype=MemberType::Slot;
    else if (root->mtype==MethodTypes::DCOP)    mtype=MemberType::DCOP;

    // new overloaded member function
    std::unique_ptr<ArgumentList> tArgList =
      getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
    //printf("new related member %s args='%s'\n",qPrint(md->name()),qPrint(funcArgs));
    auto md = createMemberDef(
        root->fileName,root->startLine,root->startColumn,
        funcType,funcName,funcArgs,exceptions,
        root->protection,root->virt,root->isStatic,Relationship::Related,
        mtype,tArgList ? *tArgList : ArgumentList(),root->argList,root->metaData);
    auto mmd = toMemberDefMutable(md.get());
    mmd->setTagInfo(root->tagInfo());
    mmd->setLanguage(root->lang);
    mmd->setId(root->id);
    mmd->setTypeConstraints(root->typeConstr);
    mmd->setMemberClass(cd);
    mmd->setDefinition(funcDecl);
    applyMemberOverrideOptions(root,mmd);
    mmd->addQualifiers(root->qualifiers);
    QCString doc=getOverloadDocs();
    doc+="<p>";
    doc+=root->doc;
    mmd->setDocumentation(doc,root->docFile,root->docLine);
    mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    mmd->setDocsForDefinition(!root->proto);
    mmd->setPrototype(root->proto,root->fileName,root->startLine,root->startColumn);
    mmd->addSectionsToDefinition(root->anchors);
    mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
    FileDef *fd=root->fileDef();
    mmd->setBodyDef(fd);
    mmd->setMemberSpecifiers(spec);
    mmd->setVhdlSpecifiers(root->vhdlSpec);
    mmd->setMemberGroupId(root->mGrpId);
    cd->insertMember(md.get());
    cd->insertUsedFile(fd);
    mmd->setRefItems(root->sli);

    mn->push_back(std::move(md));
  }
}

static void insertMemberAlias(Definition *outerScope,const MemberDef *md)
{
  if (outerScope && outerScope!=Doxygen::globalScope)
  {
    auto aliasMd = createMemberDefAlias(outerScope,md);
    if (outerScope->definitionType()==Definition::TypeClass)
    {
      ClassDefMutable *cdm = toClassDefMutable(outerScope);
      if (cdm)
      {
        cdm->insertMember(aliasMd.get());
      }
    }
    else if (outerScope->definitionType()==Definition::TypeNamespace)
    {
      NamespaceDefMutable *ndm = toNamespaceDefMutable(outerScope);
      if (ndm)
      {
        ndm->insertMember(aliasMd.get());
      }
    }
    else if (outerScope->definitionType()==Definition::TypeFile)
    {
      toFileDef(outerScope)->insertMember(aliasMd.get());
    }
    if (aliasMd)
    {
      Doxygen::functionNameLinkedMap->add(md->name())->push_back(std::move(aliasMd));
    }
  }
}

//-------------------------------------------------------------------------------------------

/*! This function tries to find a member (in a documented class/file/namespace)
 * that corresponds to the function/variable declaration given in \a funcDecl.
 *
 * The boolean \a overloaded is used to specify whether or not a standard
 * overload documentation line should be generated.
 *
 * The boolean \a isFunc is a hint that indicates that this is a function
 * instead of a variable or typedef.
 */
static void findMember(const Entry *root,
                       const QCString &relates,
                       const QCString &type,
                       const QCString &args,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      )
{
  AUTO_TRACE("root='{}' funcDecl='{}' related='{}' overload={} isFunc={} mGrpId={} #tArgList={} spec={} lang={}",
               root->name, funcDecl, relates, overloaded, isFunc, root->mGrpId, root->tArgLists.size(),
               root->spec, root->lang);

  QCString scopeName;
  QCString className;
  QCString namespaceName;
  QCString funcType;
  QCString funcName;
  QCString funcArgs;
  QCString funcTempList;
  QCString exceptions;
  QCString funcSpec;
  bool isRelated=false;
  bool isMemberOf=false;
  bool isFriend=false;
  bool done=false;
  TypeSpecifier spec = root->spec;
  while (!done)
  {
    done=true;
    if (funcDecl.stripPrefix("friend ")) // treat friends as related members
    {
      isFriend=true;
      done=false;
    }
    if (funcDecl.stripPrefix("inline "))
    {
      spec.setInline(true);
      done=false;
    }
    if (funcDecl.stripPrefix("explicit "))
    {
      spec.setExplicit(true);
      done=false;
    }
    if (funcDecl.stripPrefix("mutable "))
    {
      spec.setMutable(true);
      done=false;
    }
    if (funcDecl.stripPrefix("virtual "))
    {
      done=false;
    }
  }

  // delete any ; from the function declaration
  int sep=0;
  while ((sep=funcDecl.find(';'))!=-1)
  {
    funcDecl=(funcDecl.left(sep)+funcDecl.right(funcDecl.length()-sep-1)).stripWhiteSpace();
  }

  // make sure the first character is a space to simplify searching.
  if (!funcDecl.isEmpty() && funcDecl[0]!=' ') funcDecl.prepend(" ");

  // remove some superfluous spaces
  funcDecl= substitute(
              substitute(
                substitute(funcDecl,"~ ","~"),
                ":: ","::"
              ),
              " ::","::"
            ).stripWhiteSpace();

  //printf("funcDecl='%s'\n",qPrint(funcDecl));
  if (isFriend && funcDecl.startsWith("class "))
  {
    //printf("friend class\n");
    funcDecl=funcDecl.right(funcDecl.length()-6);
    funcName = funcDecl;
  }
  else if (isFriend && funcDecl.startsWith("struct "))
  {
    funcDecl=funcDecl.right(funcDecl.length()-7);
    funcName = funcDecl;
  }
  else
  {
    // extract information from the declarations
    parseFuncDecl(funcDecl,root->lang,scopeName,funcType,funcName,
                funcArgs,funcTempList,exceptions
               );
  }

  // the class name can also be a namespace name, we decide this later.
  // if a related class name is specified and the class name could
  // not be derived from the function declaration, then use the
  // related field.
  AUTO_TRACE_ADD("scopeName='{}' className='{}' namespaceName='{}' funcType='{}' funcName='{}' funcArgs='{}'",
                 scopeName,className,namespaceName,funcType,funcName,funcArgs);
  if (!relates.isEmpty())
  {                             // related member, prefix user specified scope
    isRelated=TRUE;
    isMemberOf=(root->relatesType == RelatesType::MemberOf);
    if (getClass(relates)==nullptr && !scopeName.isEmpty())
    {
      scopeName= mergeScopes(scopeName,relates);
    }
    else
    {
      scopeName = relates;
    }
  }

  if (relates.isEmpty() && root->parent() &&
      (root->parent()->section.isScope() || root->parent()->section.isObjcImpl()) &&
      !root->parent()->name.isEmpty()) // see if we can combine scopeName
                                       // with the scope in which it was found
  {
    QCString joinedName = root->parent()->name+"::"+scopeName;
    if (!scopeName.isEmpty() &&
        (getClass(joinedName) || Doxygen::namespaceLinkedMap->find(joinedName)))
    {
      scopeName = joinedName;
    }
    else
    {
      scopeName = mergeScopes(root->parent()->name,scopeName);
    }
  }
  else // see if we can prefix a namespace or class that is used from the file
  {
     FileDef *fd=root->fileDef();
     if (fd)
     {
       for (const auto &fnd : fd->getUsedNamespaces())
       {
         QCString joinedName = fnd->name()+"::"+scopeName;
         if (Doxygen::namespaceLinkedMap->find(joinedName))
         {
           scopeName=joinedName;
           break;
         }
       }
     }
  }
  scopeName=stripTemplateSpecifiersFromScope(
      removeRedundantWhiteSpace(scopeName),false,&funcSpec,QCString(),false);

  // funcSpec contains the last template specifiers of the given scope.
  // If this method does not have any template arguments or they are
  // empty while funcSpec is not empty we assume this is a
  // specialization of a method. If not, we clear the funcSpec and treat
  // this as a normal method of a template class.
  if (!(root->tArgLists.size()>0 &&
        root->tArgLists.front().size()==0
       )
     )
  {
    funcSpec.clear();
  }

  // split scope into a namespace and a class part
  extractNamespaceName(scopeName,className,namespaceName,TRUE);
  AUTO_TRACE_ADD("scopeName='{}' className='{}' namespaceName='{}'",scopeName,className,namespaceName);

  //namespaceName=removeAnonymousScopes(namespaceName);
  if (namespaceName.find('@')!=-1) return; // skip stuff in anonymous namespace...

  //printf("namespaceName='%s' className='%s'\n",qPrint(namespaceName),qPrint(className));
  // merge class and namespace scopes again
  scopeName.clear();
  if (!namespaceName.isEmpty())
  {
    if (className.isEmpty())
    {
      scopeName=namespaceName;
    }
    else if (!relates.isEmpty() || // relates command with explicit scope
             !getClass(className)) // class name only exists in a namespace
    {
      scopeName=namespaceName+"::"+className;
    }
    else
    {
      scopeName=className;
    }
  }
  else if (!className.isEmpty())
  {
    scopeName=className;
  }
  //printf("new scope='%s'\n",qPrint(scopeName));

  QCString tempScopeName=scopeName;
  ClassDefMutable *cd=getClassMutable(scopeName);
  if (cd)
  {
    if (funcSpec.isEmpty())
    {
      uint32_t argListIndex=0;
      tempScopeName=cd->qualifiedNameWithTemplateParameters(&root->tArgLists,&argListIndex);
    }
    else
    {
      tempScopeName=scopeName+funcSpec;
    }
  }
  //printf("scopeName=%s cd=%p root->tArgLists=%p result=%s\n",
  //    qPrint(scopeName),cd,root->tArgLists,qPrint(tempScopeName));

  //printf("scopeName='%s' className='%s'\n",qPrint(scopeName),qPrint(className));
  // rebuild the function declaration (needed to get the scope right).
  if (!scopeName.isEmpty() && !isRelated && !isFriend && !Config_getBool(HIDE_SCOPE_NAMES))
  {
    if (!funcType.isEmpty())
    {
      if (isFunc) // a function -> we use argList for the arguments
      {
        funcDecl=funcType+" "+tempScopeName+"::"+funcName+funcTempList;
      }
      else
      {
        funcDecl=funcType+" "+tempScopeName+"::"+funcName+funcArgs;
      }
    }
    else
    {
      if (isFunc) // a function => we use argList for the arguments
      {
        funcDecl=tempScopeName+"::"+funcName+funcTempList;
      }
      else // variable => add 'argument' list
      {
        funcDecl=tempScopeName+"::"+funcName+funcArgs;
      }
    }
  }
  else // build declaration without scope
  {
    if (!funcType.isEmpty()) // but with a type
    {
      if (isFunc) // function => omit argument list
      {
        funcDecl=funcType+" "+funcName+funcTempList;
      }
      else // variable => add 'argument' list
      {
        funcDecl=funcType+" "+funcName+funcArgs;
      }
    }
    else // no type
    {
      if (isFunc)
      {
        funcDecl=funcName+funcTempList;
      }
      else
      {
        funcDecl=funcName+funcArgs;
      }
    }
  }

  if (funcType=="template class" && !funcTempList.isEmpty())
    return;   // ignore explicit template instantiations

  AUTO_TRACE_ADD("Parse results: namespaceName='{}' className=`{}` funcType='{}' funcSpec='{}' "
           " funcName='{}' funcArgs='{}' funcTempList='{}' funcDecl='{}' relates='{}'"
           " exceptions='{}' isRelated={} isMemberOf={} isFriend={} isFunc={}",
           namespaceName, className, funcType, funcSpec,
           funcName, funcArgs, funcTempList, funcDecl, relates,
           exceptions, isRelated, isMemberOf, isFriend, isFunc);

  if (!funcName.isEmpty()) // function name is valid
  {
    // check if 'className' is actually a scoped enum, in which case we need to
    // process it as a global, see issue #6471
    bool strongEnum = false;
    MemberName *mn=nullptr;
    if (!className.isEmpty() && (mn=Doxygen::functionNameLinkedMap->find(className)))
    {
      for (const auto &imd : *mn)
      {
        MemberDefMutable *md = toMemberDefMutable(imd.get());
        Definition *mdScope = nullptr;
        if (md && md->isEnumerate() && md->isStrong() && (mdScope=md->getOuterScope()) &&
            // need filter for the correct scope, see issue #9668
            ((namespaceName.isEmpty() && mdScope==Doxygen::globalScope) || (mdScope->name()==namespaceName)))
        {
          AUTO_TRACE_ADD("'{}' is a strong enum! (namespace={} md->getOuterScope()->name()={})",md->name(),namespaceName,md->getOuterScope()->name());
          strongEnum = true;
          // pass the scope name name as a 'namespace' to the findGlobalMember function
          if (!namespaceName.isEmpty())
          {
            namespaceName+="::"+className;
          }
          else
          {
            namespaceName=className;
          }
        }
      }
    }

    if (funcName.startsWith("operator ")) // strip class scope from cast operator
    {
      funcName = substitute(funcName,className+"::","");
    }
    mn = nullptr;
    if (!funcTempList.isEmpty()) // try with member specialization
    {
      mn=Doxygen::memberNameLinkedMap->find(funcName+funcTempList);
    }
    if (mn==nullptr) // try without specialization
    {
      mn=Doxygen::memberNameLinkedMap->find(funcName);
    }
    if (!isRelated && !strongEnum && mn) // function name already found
    {
      AUTO_TRACE_ADD("member name exists ({} members with this name)",mn->size());
      if (!className.isEmpty()) // class name is valid
      {
        if (funcSpec.isEmpty()) // not a member specialization
        {
          addMemberFunction(root,mn,scopeName,namespaceName,className,funcType,funcName,
                            funcArgs,funcTempList,exceptions,
                            type,args,isFriend,spec,relates,funcDecl,overloaded,isFunc);
        }
        else if (cd) // member specialization
        {
          addMemberSpecialization(root,mn,cd,funcType,funcName,funcArgs,funcDecl,exceptions,spec);
        }
        else
        {
          //printf("*** Specialized member %s of unknown scope %s%s found!\n",
          //        qPrint(scopeName),qPrint(funcName),qPrint(funcArgs));
        }
      }
      else if (overloaded) // check if the function belongs to only one class
      {
        addOverloaded(root,mn,funcType,funcName,funcArgs,funcDecl,exceptions,spec);
      }
      else // unrelated function with the same name as a member
      {
        if (!findGlobalMember(root,namespaceName,funcType,funcName,funcTempList,funcArgs,funcDecl,spec))
        {
          QCString fullFuncDecl=funcDecl;
          if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
          warn(root->fileName,root->startLine,
               "Cannot determine class for function\n%s",
               qPrint(fullFuncDecl)
              );
        }
      }
    }
    else if (isRelated && !relates.isEmpty())
    {
      AUTO_TRACE_ADD("related function scopeName='{}' className='{}'",scopeName,className);
      if (className.isEmpty()) className=relates;
      //printf("scopeName='%s' className='%s'\n",qPrint(scopeName),qPrint(className));
      if ((cd=getClassMutable(scopeName)))
      {
        bool newMember=TRUE; // assume we have a new member
        MemberDefMutable *mdDefine=nullptr;
        {
          mn = Doxygen::functionNameLinkedMap->find(funcName);
          if (mn)
          {
            for (const auto &imd : *mn)
            {
              MemberDefMutable *md = toMemberDefMutable(imd.get());
              if (md && md->isDefine())
              {
                mdDefine = md;
                break;
              }
            }
          }
        }

        if (mdDefine) // macro definition is already created by the preprocessor and inserted as a file member
        {
          //printf("moving #define %s into class %s\n",qPrint(mdDefine->name()),qPrint(cd->name()));

          // take mdDefine from the Doxygen::functionNameLinkedMap (without deleting the data)
          auto mdDefineTaken = Doxygen::functionNameLinkedMap->take(funcName,mdDefine);
          // insert it as a class member
          if ((mn=Doxygen::memberNameLinkedMap->find(funcName))==nullptr)
          {
            mn=Doxygen::memberNameLinkedMap->add(funcName);
          }

          if (mdDefine->getFileDef())
          {
            mdDefine->getFileDef()->removeMember(mdDefine);
          }
          mdDefine->makeRelated();
          mdDefine->setMemberClass(cd);
          mdDefine->moveTo(cd);
          cd->insertMember(mdDefine);
          // also insert the member as an alias in the parent's scope, so it can be referenced also without cd's scope
          insertMemberAlias(cd->getOuterScope(),mdDefine);
          mn->push_back(std::move(mdDefineTaken));
        }
        else // normal member, needs to be created and added to the class
        {
          FileDef *fd=root->fileDef();

          if ((mn=Doxygen::memberNameLinkedMap->find(funcName))==nullptr)
          {
            mn=Doxygen::memberNameLinkedMap->add(funcName);
          }
          else
          {
            // see if we got another member with matching arguments
            MemberDefMutable *rmd_found = nullptr;
            for (const auto &irmd : *mn)
            {
              MemberDefMutable *rmd = toMemberDefMutable(irmd.get());
              if (rmd)
              {
                const ArgumentList &rmdAl = rmd->argumentList();

                newMember=
                  className!=rmd->getOuterScope()->name() ||
                  !matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),&rmdAl,
                      cd,fd,&root->argList,
                      TRUE,root->lang);
                if (!newMember)
                {
                  rmd_found = rmd;
                }
              }
            }
            if (rmd_found) // member already exists as rmd -> add docs
            {
              AUTO_TRACE_ADD("addMemberDocs for related member {}",root->name);
              addMemberDocs(root,rmd_found,funcDecl,nullptr,overloaded,spec);
              newMember=false;
            }
          }

          if (newMember) // need to create a new member
          {
            MemberType mtype = MemberType::Function;
            switch (root->mtype)
            {
              case MethodTypes::Method:   mtype = MemberType::Function; break;
              case MethodTypes::Signal:   mtype = MemberType::Signal;   break;
              case MethodTypes::Slot:     mtype = MemberType::Slot;     break;
              case MethodTypes::DCOP:     mtype = MemberType::DCOP;     break;
              case MethodTypes::Property: mtype = MemberType::Property; break;
              case MethodTypes::Event:    mtype = MemberType::Event;    break;
            }

            //printf("New related name '%s' '%d'\n",qPrint(funcName),
            //    root->argList ? (int)root->argList->count() : -1);

            // first note that we pass:
            //   (root->tArgLists ? root->tArgLists->last() : nullptr)
            // for the template arguments for the new "member."
            // this accurately reflects the template arguments of
            // the related function, which don't have to do with
            // those of the related class.
            auto md = createMemberDef(
                root->fileName,root->startLine,root->startColumn,
                funcType,funcName,funcArgs,exceptions,
                root->protection,root->virt,
                root->isStatic,
                isMemberOf ? Relationship::Foreign : Relationship::Related,
                mtype,
                (!root->tArgLists.empty() ? root->tArgLists.back() : ArgumentList()),
                funcArgs.isEmpty() ? ArgumentList() : root->argList,
                root->metaData);
            auto mmd = toMemberDefMutable(md.get());

            // also insert the member as an alias in the parent's scope, so it can be referenced also without cd's scope
            insertMemberAlias(cd->getOuterScope(),md.get());

            // we still have the problem that
            // MemberDef::writeDocumentation() in memberdef.cpp
            // writes the template argument list for the class,
            // as if this member is a member of the class.
            // fortunately, MemberDef::writeDocumentation() has
            // a special mechanism that allows us to totally
            // override the set of template argument lists that
            // are printed.  We use that and set it to the
            // template argument lists of the related function.
            //
            mmd->setDefinitionTemplateParameterLists(root->tArgLists);

            mmd->setTagInfo(root->tagInfo());

            //printf("Related member name='%s' decl='%s' bodyLine='%d'\n",
            //       qPrint(funcName),qPrint(funcDecl),root->bodyLine);

            // try to find the matching line number of the body from the
            // global function list
            bool found=FALSE;
            if (root->bodyLine==-1)
            {
              MemberName *rmn=Doxygen::functionNameLinkedMap->find(funcName);
              if (rmn)
              {
                const MemberDefMutable *rmd_found=nullptr;
                for (const auto &irmd : *rmn)
                {
                  MemberDefMutable *rmd = toMemberDefMutable(irmd.get());
                  if (rmd)
                  {
                    const ArgumentList &rmdAl = rmd->argumentList();
                    // check for matching argument lists
                    if (
                        matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),&rmdAl,
                          cd,fd,&root->argList,
                          TRUE,root->lang)
                       )
                    {
                      found=TRUE;
                      rmd_found = rmd;
                      break;
                    }
                  }
                }
                if (rmd_found) // member found -> copy line number info
                {
                  mmd->setBodySegment(rmd_found->getDefLine(),rmd_found->getStartBodyLine(),rmd_found->getEndBodyLine());
                  mmd->setBodyDef(rmd_found->getBodyDef());
                  //md->setBodyMember(rmd);
                }
              }
            }
            if (!found) // line number could not be found or is available in this
                        // entry
            {
              mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
              mmd->setBodyDef(fd);
            }

            //if (root->mGrpId!=-1)
            //{
            //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
            //}
            mmd->setMemberClass(cd);
            mmd->setMemberSpecifiers(spec);
            mmd->setVhdlSpecifiers(root->vhdlSpec);
            mmd->setDefinition(funcDecl);
            applyMemberOverrideOptions(root,mmd);
            mmd->addQualifiers(root->qualifiers);
            mmd->setDocumentation(root->doc,root->docFile,root->docLine);
            mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
            mmd->setDocsForDefinition(!root->proto);
            mmd->setPrototype(root->proto,root->fileName,root->startLine,root->startColumn);
            mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            mmd->addSectionsToDefinition(root->anchors);
            mmd->setMemberGroupId(root->mGrpId);
            mmd->setLanguage(root->lang);
            mmd->setId(root->id);
            //md->setMemberDefTemplateArguments(root->mtArgList);
            cd->insertMember(md.get());
            cd->insertUsedFile(fd);
            mmd->setRefItems(root->sli);
            if (root->relatesType==RelatesType::Duplicate) mmd->setRelatedAlso(cd);
            addMemberToGroups(root,md.get());
            ModuleManager::instance().addMemberToModule(root,md.get());
            //printf("Adding member=%s\n",qPrint(md->name()));
            mn->push_back(std::move(md));
          }
          if (root->relatesType==RelatesType::Duplicate)
          {
            if (!findGlobalMember(root,namespaceName,funcType,funcName,funcTempList,funcArgs,funcDecl,spec))
            {
              QCString fullFuncDecl=funcDecl;
              if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
              warn(root->fileName,root->startLine,
                  "Cannot determine file/namespace for relatedalso function\n%s",
                  qPrint(fullFuncDecl)
                  );
            }
          }
        }
      }
      else
      {
        warn_undoc(root->fileName,root->startLine,
                   "class '%s' for related function '%s' is not "
                   "documented.",
                   qPrint(className),qPrint(funcName)
                  );
      }
    }
    else if (root->parent() && root->parent()->section.isObjcImpl())
    {
      addLocalObjCMethod(root,scopeName,funcType,funcName,funcArgs,exceptions,funcDecl,spec);
    }
    else // unrelated not overloaded member found
    {
      bool globMem = findGlobalMember(root,namespaceName,funcType,funcName,funcTempList,funcArgs,funcDecl,spec);
      if (className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "class for member '%s' cannot "
             "be found.", qPrint(funcName)
            );
      }
      else if (!className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "member '%s' of class '%s' cannot be found",
             qPrint(funcName),qPrint(className));
      }
    }
  }
  else
  {
    // this should not be called
    warn(root->fileName,root->startLine,
         "member with no name found.");
  }
  return;
}

//----------------------------------------------------------------------
// find the members corresponding to the different documentation blocks
// that are extracted from the sources.

static void filterMemberDocumentation(const Entry *root,const QCString &relates)
{
  AUTO_TRACE("root->type='{}' root->inside='{}' root->name='{}' root->args='{}' section={} root->spec={} root->mGrpId={}",
      root->type,root->inside,root->name,root->args,root->section,root->spec,root->mGrpId);
  //printf("root->parent()->name=%s\n",qPrint(root->parent()->name));
  bool isFunc=TRUE;

  QCString type = root->type;
  QCString args = root->args;
  int i=-1, l=0;
  if ( // detect func variable/typedef to func ptr
      (i=findFunctionPtr(type.str(),root->lang,&l))!=-1
     )
  {
    //printf("Fixing function pointer!\n");
    // fix type and argument
    args.prepend(type.right(type.length()-i-l));
    type=type.left(i+l);
    //printf("Results type=%s,name=%s,args=%s\n",qPrint(type),qPrint(root->name),qPrint(args));
    isFunc=FALSE;
  }
  else if ((type.startsWith("typedef ") && args.find('(')!=-1))
    // detect function types marked as functions
  {
    isFunc=FALSE;
  }

  //printf("Member %s isFunc=%d\n",qPrint(root->name),isFunc);
  if (root->section.isMemberDoc())
  {
    //printf("Documentation for inline member '%s' found args='%s'\n",
    //    qPrint(root->name),qPrint(args));
    //if (relates.length()) printf("  Relates %s\n",qPrint(relates));
    if (type.isEmpty())
    {
      findMember(root,
                 relates,
                 type,
                 args,
                 root->name + args + root->exception,
                 FALSE,
                 isFunc);
    }
    else
    {
      findMember(root,
                 relates,
                 type,
                 args,
                 type + " " + root->name + args + root->exception,
                 FALSE,
                 isFunc);
    }
  }
  else if (root->section.isOverloadDoc())
  {
    //printf("Overloaded member %s found\n",qPrint(root->name));
    findMember(root,
               relates,
               type,
               args,
               root->name,
               TRUE,
               isFunc);
  }
  else if
    ((root->section.isFunction()      // function
      ||
      (root->section.isVariable() &&  // variable
       !type.isEmpty() &&                // with a type
       g_compoundKeywords.find(type.str())==g_compoundKeywords.end() // that is not a keyword
       // (to skip forward declaration of class etc.)
      )
     )
    )
    {
      //printf("Documentation for member '%s' found args='%s' excp='%s'\n",
      //    qPrint(root->name),qPrint(args),qPrint(root->exception));
      //if (relates.length()) printf("  Relates %s\n",qPrint(relates));
      //printf("Inside=%s\n Relates=%s\n",qPrint(root->inside),qPrint(relates));
      if (type=="friend class" || type=="friend struct" ||
          type=="friend union")
      {
        findMember(root,
            relates,
            type,
            args,
            type+" "+root->name,
            FALSE,FALSE);

      }
      else if (!type.isEmpty())
      {
        findMember(root,
            relates,
            type,
            args,
            type+" "+ root->inside + root->name + args + root->exception,
            FALSE,isFunc);
      }
      else
      {
        findMember(root,
            relates,
            type,
            args,
            root->inside + root->name + args + root->exception,
            FALSE,isFunc);
      }
    }
  else if (root->section.isDefine() && !relates.isEmpty())
  {
    findMember(root,
               relates,
               type,
               args,
               root->name + args,
               FALSE,
               !args.isEmpty());
  }
  else if (root->section.isVariableDoc())
  {
    //printf("Documentation for variable %s found\n",qPrint(root->name));
    //if (!relates.isEmpty()) printf("  Relates %s\n",qPrint(relates));
    findMember(root,
               relates,
               type,
               args,
               root->name,
               FALSE,
               FALSE);
  }
  else if (root->section.isExportedInterface() ||
           root->section.isIncludedService())
  {
    findMember(root,
               relates,
               type,
               args,
               type + " " + root->name,
               FALSE,
               FALSE);
  }
  else
  {
    // skip section
    //printf("skip section\n");
  }
}

static void findMemberDocumentation(const Entry *root)
{
  if (root->section.isMemberDoc() ||
      root->section.isOverloadDoc() ||
      root->section.isFunction() ||
      root->section.isVariable() ||
      root->section.isVariableDoc() ||
      root->section.isDefine() ||
      root->section.isIncludedService() ||
      root->section.isExportedInterface()
     )
  {
    AUTO_TRACE();
    if (root->relatesType==RelatesType::Duplicate && !root->relates.isEmpty())
    {
      filterMemberDocumentation(root,"");
    }
    filterMemberDocumentation(root,root->relates);
  }
  for (const auto &e : root->children())
  {
    if (!e->section.isEnum())
    {
      findMemberDocumentation(e.get());
    }
  }
}

//----------------------------------------------------------------------

static void findObjCMethodDefinitions(const Entry *root)
{
  AUTO_TRACE();
  for (const auto &objCImpl : root->children())
  {
    if (objCImpl->section.isObjcImpl())
    {
      for (const auto &objCMethod : objCImpl->children())
      {
        if (objCMethod->section.isFunction())
        {
          //Printf("  Found ObjC method definition %s\n",qPrint(objCMethod->name));
          findMember(objCMethod.get(),
                     objCMethod->relates,
                     objCMethod->type,
                     objCMethod->args,
                     objCMethod->type+" "+objCImpl->name+"::"+objCMethod->name+" "+objCMethod->args,
                     FALSE,TRUE);
          objCMethod->section=EntryType::makeEmpty();
        }
      }
    }
  }
}

//----------------------------------------------------------------------
// find and add the enumeration to their classes, namespaces or files

static void findEnums(const Entry *root)
{
  if (root->section.isEnum())
  {
    AUTO_TRACE("name={}",root->name);
    ClassDefMutable     *cd        = nullptr;
    FileDef             *fd        = nullptr;
    NamespaceDefMutable *nd        = nullptr;
    MemberNameLinkedMap *mnsd      = nullptr;
    bool                isGlobal   = false;
    bool                isRelated  = false;
    bool                isMemberOf = false;
    //printf("Found enum with name '%s' relates=%s\n",qPrint(root->name),qPrint(root->relates));

    QCString name;
    QCString scope;

    int i = root->name.findRev("::");
    if (i!=-1) // scope is specified
    {
      scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClassMutable(scope))==nullptr)
      {
        nd=toNamespaceDefMutable(buildScopeFromQualifiedName(root->name.left(i+2),root->lang,root->tagInfo()));
      }
    }
    else // no scope, check the scope in which the docs where found
    {
      if (root->parent()->section.isScope() && !root->parent()->name.isEmpty()) // found enum docs inside a compound
      {
        scope=root->parent()->name;
        if ((cd=getClassMutable(scope))==nullptr) nd=getResolvedNamespaceMutable(scope);
      }
      name=root->name;
    }

    if (!root->relates.isEmpty())
    {   // related member, prefix user specified scope
      isRelated=TRUE;
      isMemberOf=(root->relatesType==RelatesType::MemberOf);
      if (getClass(root->relates)==nullptr && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else
        scope=root->relates;
      if ((cd=getClassMutable(scope))==nullptr) nd=getResolvedNamespaceMutable(scope);
    }

    if (cd && !name.isEmpty()) // found a enum inside a compound
    {
      //printf("Enum '%s'::'%s'\n",qPrint(cd->name()),qPrint(name));
      fd=nullptr;
      mnsd=Doxygen::memberNameLinkedMap;
      isGlobal=false;
    }
    else if (nd) // found enum inside namespace
    {
      mnsd=Doxygen::functionNameLinkedMap;
      isGlobal=true;
    }
    else // found a global enum
    {
      fd=root->fileDef();
      mnsd=Doxygen::functionNameLinkedMap;
      isGlobal=true;
    }

    if (!name.isEmpty())
    {
      // new enum type
      AUTO_TRACE_ADD("new enum {} at line {} of {}",name,root->bodyLine,root->fileName);
      auto md = createMemberDef(
          root->fileName,root->startLine,root->startColumn,
          QCString(),name,QCString(),QCString(),
          root->protection,Specifier::Normal,FALSE,
          isMemberOf ? Relationship::Foreign : isRelated ? Relationship::Related : Relationship::Member,
          MemberType::Enumeration,
          ArgumentList(),ArgumentList(),root->metaData);
      auto mmd = toMemberDefMutable(md.get());
      mmd->setTagInfo(root->tagInfo());
      mmd->setLanguage(root->lang);
      mmd->setId(root->id);
      if (!isGlobal) mmd->setMemberClass(cd); else mmd->setFileDef(fd);
      mmd->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
      mmd->setBodyDef(root->fileDef());
      mmd->setMemberSpecifiers(root->spec);
      mmd->setVhdlSpecifiers(root->vhdlSpec);
      mmd->setEnumBaseType(root->args);
      //printf("Enum %s definition at line %d of %s: protection=%d scope=%s\n",
      //    qPrint(root->name),root->bodyLine,qPrint(root->fileName),root->protection,cd?qPrint(cd->name()):"<none>");
      mmd->addSectionsToDefinition(root->anchors);
      mmd->setMemberGroupId(root->mGrpId);
      applyMemberOverrideOptions(root,mmd);
      mmd->addQualifiers(root->qualifiers);
      //printf("%s::setRefItems(%zu)\n",qPrint(md->name()),root->sli.size());
      mmd->setRefItems(root->sli);
      //printf("found enum %s nd=%p\n",qPrint(md->name()),nd);
      bool defSet=FALSE;

      QCString baseType = root->args;
      if (!baseType.isEmpty())
      {
        baseType.prepend(" : ");
      }

      if (nd)
      {
        if (isRelated || Config_getBool(HIDE_SCOPE_NAMES))
        {
          mmd->setDefinition(name+baseType);
        }
        else
        {
          mmd->setDefinition(nd->name()+"::"+name+baseType);
        }
        //printf("definition=%s\n",md->definition());
        defSet=TRUE;
        mmd->setNamespace(nd);
        nd->insertMember(md.get());
      }

      // even if we have already added the enum to a namespace, we still
      // also want to add it to other appropriate places such as file
      // or class.
      if (isGlobal && (nd==nullptr || !nd->isAnonymous()))
      {
        if (!defSet) mmd->setDefinition(name+baseType);
        if (fd==nullptr && root->parent())
        {
          fd=root->parent()->fileDef();
        }
        if (fd)
        {
          mmd->setFileDef(fd);
          fd->insertMember(md.get());
        }
      }
      else if (cd)
      {
        if (isRelated || Config_getBool(HIDE_SCOPE_NAMES))
        {
          mmd->setDefinition(name+baseType);
        }
        else
        {
          mmd->setDefinition(cd->name()+"::"+name+baseType);
        }
        cd->insertMember(md.get());
        cd->insertUsedFile(fd);
      }
      mmd->setDocumentation(root->doc,root->docFile,root->docLine);
      mmd->setDocsForDefinition(!root->proto);
      mmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      mmd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);

      //printf("Adding member=%s\n",qPrint(md->name()));
      addMemberToGroups(root,md.get());
      ModuleManager::instance().addMemberToModule(root,md.get());

      MemberName *mn = mnsd->add(name);
      mn->push_back(std::move(md));
    }
  }
  else
  {
    for (const auto &e : root->children()) findEnums(e.get());
  }
}

//----------------------------------------------------------------------

static void addEnumValuesToEnums(const Entry *root)
{
  if (root->section.isEnum())
    // non anonymous enumeration
  {
    AUTO_TRACE("name={}",root->name);
    ClassDefMutable     *cd        = nullptr;
    FileDef             *fd        = nullptr;
    NamespaceDefMutable *nd        = nullptr;
    MemberNameLinkedMap *mnsd      = nullptr;
    bool                 isGlobal  = false;
    bool                 isRelated = false;
    //printf("Found enum with name '%s' relates=%s\n",qPrint(root->name),qPrint(root->relates));

    QCString name;
    QCString scope;

    int i = root->name.findRev("::");
    if (i!=-1) // scope is specified
    {
      scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClassMutable(scope))==nullptr)
      {
        nd=toNamespaceDefMutable(buildScopeFromQualifiedName(root->name.left(i+2),root->lang,root->tagInfo()));
      }
    }
    else // no scope, check the scope in which the docs where found
    {
      if (root->parent()->section.isScope() && !root->parent()->name.isEmpty()) // found enum docs inside a compound
      {
        scope=root->parent()->name;
        if ((cd=getClassMutable(scope))==nullptr) nd=getResolvedNamespaceMutable(scope);
      }
      name=root->name;
    }

    if (!root->relates.isEmpty())
    {   // related member, prefix user specified scope
      isRelated=TRUE;
      if (getClassMutable(root->relates)==nullptr && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else
        scope=root->relates;
      if ((cd=getClassMutable(scope))==nullptr) nd=getResolvedNamespaceMutable(scope);
    }

    if (cd && !name.isEmpty()) // found a enum inside a compound
    {
      //printf("Enum in class '%s'::'%s'\n",qPrint(cd->name()),qPrint(name));
      fd=nullptr;
      mnsd=Doxygen::memberNameLinkedMap;
      isGlobal=false;
    }
    else if (nd && !nd->isAnonymous()) // found enum inside namespace
    {
      //printf("Enum in namespace '%s'::'%s'\n",qPrint(nd->name()),qPrint(name));
      mnsd=Doxygen::functionNameLinkedMap;
      isGlobal=true;
    }
    else // found a global enum
    {
      fd=root->fileDef();
      //printf("Enum in file '%s': '%s'\n",qPrint(fd->name()),qPrint(name));
      mnsd=Doxygen::functionNameLinkedMap;
      isGlobal=true;
    }

    if (!name.isEmpty())
    {
      //printf("** name=%s\n",qPrint(name));
      MemberName *mn = mnsd->find(name); // for all members with this name
      if (mn)
      {
        struct EnumValueInfo
        {
          EnumValueInfo(const QCString &n,std::unique_ptr<MemberDef> &&md) :
            name(n), member(std::move(md)) {}
          QCString name;
          std::unique_ptr<MemberDef> member;
        };
        std::vector< EnumValueInfo > extraMembers;
        // for each enum in this list
        for (const auto &imd : *mn)
        {
          MemberDefMutable *md = toMemberDefMutable(imd.get());
          // use raw pointer in this loop, since we modify mn and can then invalidate mdp.
          if (md && md->isEnumerate() && !root->children().empty())
          {
            AUTO_TRACE_ADD("enum {} with {} children",md->name(),root->children().size());
            for (const auto &e : root->children())
            {
              SrcLangExt sle = root->lang;
              bool isJavaLike = sle==SrcLangExt::CSharp || sle==SrcLangExt::Java || sle==SrcLangExt::XML;
              if ( isJavaLike || root->spec.isStrong())
              {
                // Unlike classic C/C++ enums, for C++11, C# & Java enum
                // values are only visible inside the enum scope, so we must create
                // them here and only add them to the enum
                //printf("md->qualifiedName()=%s e->name=%s tagInfo=%p name=%s\n",
                //    qPrint(md->qualifiedName()),qPrint(e->name),(void*)e->tagInfo(),qPrint(e->name));
                QCString qualifiedName = root->name;
                if (isJavaLike)
                {
                  qualifiedName=substitute(qualifiedName,"::",".");
                }
                if (md->qualifiedName()==qualifiedName)       // enum value scope matches that of the enum
                {
                  QCString fileName = e->fileName;
                  if (fileName.isEmpty() && e->tagInfo())
                  {
                    fileName = e->tagInfo()->tagName;
                  }
                  AUTO_TRACE_ADD("strong enum value {}",e->name);
                  auto fmd = createMemberDef(
                      fileName,e->startLine,e->startColumn,
                      e->type,e->name,e->args,QCString(),
                      e->protection, Specifier::Normal,e->isStatic,Relationship::Member,
                      MemberType::EnumValue,ArgumentList(),ArgumentList(),e->metaData);
                  auto fmmd = toMemberDefMutable(fmd.get());
                  NamespaceDef *mnd = md->getNamespaceDef();
                  if      (md->getClassDef())
                    fmmd->setMemberClass(md->getClassDef());
                  else if (mnd && (mnd->isLinkable() || mnd->isAnonymous()))
                    fmmd->setNamespace(mnd);
                  else if (md->getFileDef())
                    fmmd->setFileDef(md->getFileDef());
                  fmmd->setOuterScope(md->getOuterScope());
                  fmmd->setTagInfo(e->tagInfo());
                  fmmd->setLanguage(e->lang);
                  fmmd->setBodySegment(e->startLine,e->bodyLine,e->endBodyLine);
                  fmmd->setBodyDef(e->fileDef());
                  fmmd->setId(e->id);
                  fmmd->setDocumentation(e->doc,e->docFile,e->docLine);
                  fmmd->setBriefDescription(e->brief,e->briefFile,e->briefLine);
                  fmmd->addSectionsToDefinition(e->anchors);
                  std::string init = e->initializer.str();
                  fmmd->setInitializer(init.c_str());
                  fmmd->setMaxInitLines(e->initLines);
                  fmmd->setMemberGroupId(e->mGrpId);
                  fmmd->setExplicitExternal(e->explicitExternal,fileName,e->startLine,e->startColumn);
                  fmmd->setRefItems(e->sli);
                  fmmd->setAnchor();
                  md->insertEnumField(fmd.get());
                  fmmd->setEnumScope(md,TRUE);
                  extraMembers.emplace_back(e->name,std::move(fmd));
                }
              }
              else
              {
                AUTO_TRACE_ADD("enum value {}",e->name);
                //printf("e->name=%s isRelated=%d\n",qPrint(e->name),isRelated);
                MemberName *fmn=nullptr;
                MemberNameLinkedMap *emnsd = isRelated ? Doxygen::functionNameLinkedMap : mnsd;
                if (!e->name.isEmpty() && (fmn=emnsd->find(e->name)))
                  // get list of members with the same name as the field
                {
                  for (const auto &ifmd : *fmn)
                  {
                    MemberDefMutable *fmd = toMemberDefMutable(ifmd.get());
                    if (fmd && fmd->isEnumValue() && fmd->getOuterScope()==md->getOuterScope()) // in same scope
                    {
                      //printf("found enum value with same name %s in scope %s\n",
                      //    qPrint(fmd->name()),qPrint(fmd->getOuterScope()->name()));
                      if (nd && !nd->isAnonymous())
                      {
                        if (!fmd->isStrongEnumValue()) // only non strong enum values can be globally added
                        {
                          const NamespaceDef *fnd=fmd->getNamespaceDef();
                          if (fnd==nd) // enum value is inside a namespace
                          {
                            md->insertEnumField(fmd);
                            fmd->setEnumScope(md);
                          }
                        }
                      }
                      else if (isGlobal)
                      {
                        if (!fmd->isStrongEnumValue()) // only non strong enum values can be globally added
                        {
                          const FileDef *ffd=fmd->getFileDef();
                          if (ffd==fd && ffd==md->getFileDef()) // enum value has file scope
                          {
                            md->insertEnumField(fmd);
                            fmd->setEnumScope(md);
                          }
                        }
                      }
                      else if (isRelated && cd) // reparent enum value to
                                                // match the enum's scope
                      {
                        md->insertEnumField(fmd);   // add field def to list
                        fmd->setEnumScope(md);      // cross ref with enum name
                        fmd->setEnumClassScope(cd); // cross ref with enum name
                        fmd->setOuterScope(cd);
                        fmd->makeRelated();
                        cd->insertMember(fmd);
                      }
                      else
                      {
                        if (!fmd->isStrongEnumValue()) // only non strong enum values can be globally added
                        {
                          const ClassDef *fcd=fmd->getClassDef();
                          if (fcd==cd) // enum value is inside a class
                          {
                            //printf("Inserting enum field %s in enum scope %s\n",
                            //    qPrint(fmd->name()),qPrint(md->name()));
                            md->insertEnumField(fmd); // add field def to list
                            fmd->setEnumScope(md);    // cross ref with enum name
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        // move the newly added members into mn
        for (auto &e : extraMembers)
        {
          MemberName *emn=mnsd->add(e.name);
          emn->push_back(std::move(e.member));
        }
      }
    }
  }
  else
  {
    for (const auto &e : root->children()) addEnumValuesToEnums(e.get());
  }
}

//----------------------------------------------------------------------

static void addEnumDocs(const Entry *root,MemberDefMutable *md)
{
  AUTO_TRACE();
  // documentation outside a compound overrides the documentation inside it
  {
    md->setDocumentation(root->doc,root->docFile,root->docLine);
    md->setDocsForDefinition(!root->proto);
  }

  // brief descriptions inside a compound override the documentation
  // outside it
  {
    md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  }

  if (md->inbodyDocumentation().isEmpty() || !root->parent()->name.isEmpty())
  {
    md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  }

  if (root->mGrpId!=-1 && md->getMemberGroupId()==-1)
  {
    md->setMemberGroupId(root->mGrpId);
  }

  md->addSectionsToDefinition(root->anchors);
  md->setRefItems(root->sli);

  const GroupDef *gd=md->getGroupDef();
  if (gd==nullptr && !root->groups.empty()) // member not grouped but out-of-line documentation is
  {
    addMemberToGroups(root,md);
  }
  ModuleManager::instance().addMemberToModule(root,md);
}

//----------------------------------------------------------------------
// Search for the name in the associated groups.  If a matching member
// definition exists, then add the documentation to it and return TRUE,
// otherwise FALSE.

static bool tryAddEnumDocsToGroupMember(const Entry *root,const QCString &name)
{
  for (const auto &g : root->groups)
  {
    const GroupDef *gd = Doxygen::groupLinkedMap->find(g.groupname);
    if (gd)
    {
      MemberList *ml = gd->getMemberList(MemberListType::DecEnumMembers());
      if (ml)
      {
        MemberDefMutable *md = toMemberDefMutable(ml->find(name));
        if (md)
        {
          addEnumDocs(root,md);
          return TRUE;
        }
      }
    }
    else if (!gd && g.pri == Grouping::GROUPING_INGROUP)
    {
      warn(root->fileName, root->startLine,
          "Found non-existing group '%s' for the command '%s', ignoring command",
          qPrint(g.groupname), Grouping::getGroupPriName( g.pri )
          );
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------
// find the documentation blocks for the enumerations

static void findEnumDocumentation(const Entry *root)
{
  if (root->section.isEnumDoc() &&
     !root->name.isEmpty() &&
      root->name.at(0)!='@'        // skip anonymous enums
     )
  {
    QCString name;
    QCString scope;
    int i = root->name.findRev("::");
    if (i!=-1) // scope is specified as part of the name
    {
      name=root->name.right(root->name.length()-i-2); // extract name
      scope=root->name.left(i); // extract scope
      //printf("Scope='%s' Name='%s'\n",qPrint(scope),qPrint(name));
    }
    else // just the name
    {
      name=root->name;
    }
    if (root->parent()->section.isScope() && !root->parent()->name.isEmpty()) // found enum docs inside a compound
    {
      if (!scope.isEmpty()) scope.prepend("::");
      scope.prepend(root->parent()->name);
    }
    const ClassDef *cd = getClass(scope);
    const NamespaceDef *nd=Doxygen::namespaceLinkedMap->find(scope);
    const FileDef *fd = root->fileDef();
    AUTO_TRACE("Found docs for enum with name '{}' and scope '{}' in context '{}' cd='{}', nd='{}' fd='{}'",
                 name,scope,root->parent()->name,
                 cd ? cd->name() : QCString("<none>"),
                 nd ? nd->name() : QCString("<none>"),
                 fd ? fd->name() : QCString("<none>"));

    if (!name.isEmpty())
    {
      bool found = tryAddEnumDocsToGroupMember(root, name);
      if (!found)
      {
        MemberName *mn = cd ?  Doxygen::memberNameLinkedMap->find(name) : Doxygen::functionNameLinkedMap->find(name);
        if (mn)
        {
          for (const auto &imd : *mn)
          {
            MemberDefMutable *md = toMemberDefMutable(imd.get());
            if (md && md->isEnumerate())
            {
              const ClassDef *mcd = md->getClassDef();
              const NamespaceDef *mnd = md->getNamespaceDef();
              const FileDef *mfd = md->getFileDef();
              if (cd && mcd==cd)
              {
                AUTO_TRACE_ADD("Match found for class scope");
                addEnumDocs(root,md);
                found = TRUE;
                break;
              }
              else if (cd==nullptr && mcd==nullptr && nd!=nullptr && mnd==nd)
              {
                AUTO_TRACE_ADD("Match found for namespace scope");
                addEnumDocs(root,md);
                found = TRUE;
                break;
              }
              else if (cd==nullptr && nd==nullptr && mcd==nullptr && mnd==nullptr && fd==mfd)
              {
                AUTO_TRACE_ADD("Match found for global scope");
                addEnumDocs(root,md);
                found = TRUE;
                break;
              }
            }
          }
        }
      }
      if (!found)
      {
        warn(root->fileName,root->startLine,
             "Documentation for undefined enum '%s' found.",
             qPrint(name)
            );
      }
    }
  }
  for (const auto &e : root->children()) findEnumDocumentation(e.get());
}

// search for each enum (member or function) in mnl if it has documented
// enum values.
static void findDEV(const MemberNameLinkedMap &mnsd)
{
  // for each member name
  for (const auto &mn : mnsd)
  {
    // for each member definition
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md && md->isEnumerate()) // member is an enum
      {
        int documentedEnumValues=0;
        // for each enum value
        for (const auto &fmd : md->enumFieldList())
        {
          if (fmd->isLinkableInProject()) documentedEnumValues++;
        }
        // at least one enum value is documented
        if (documentedEnumValues>0) md->setDocumentedEnumValues(TRUE);
      }
    }
  }
}

// search for each enum (member or function) if it has documented enum
// values.
static void findDocumentedEnumValues()
{
  findDEV(*Doxygen::memberNameLinkedMap);
  findDEV(*Doxygen::functionNameLinkedMap);
}

//----------------------------------------------------------------------

static void addMembersToIndex()
{
  auto &index = Index::instance();
  // for each class member name
  for (const auto &mn : *Doxygen::memberNameLinkedMap)
  {
    // for each member definition
    for (const auto &md : *mn)
    {
      index.addClassMemberNameToIndex(md.get());
      if (md->getModuleDef())
      {
        index.addModuleMemberNameToIndex(md.get());
      }
    }
  }
  // for each file/namespace function name
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    // for each member definition
    for (const auto &md : *mn)
    {
      if (md->getNamespaceDef())
      {
        index.addNamespaceMemberNameToIndex(md.get());
      }
      else
      {
        index.addFileMemberNameToIndex(md.get());
      }
      if (md->getModuleDef())
      {
        index.addModuleMemberNameToIndex(md.get());
      }
    }
  }

  index.sortMemberIndexLists();
}

//----------------------------------------------------------------------

static void addToIndices()
{
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (cd->isLinkableInProject())
    {
      Doxygen::indexList->addIndexItem(cd.get(),nullptr);
      if (Doxygen::searchIndex.enabled())
      {
        Doxygen::searchIndex.setCurrentDoc(cd.get(),cd->anchor(),FALSE);
        Doxygen::searchIndex.addWord(cd->localName(),TRUE);
      }
    }
  }

  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if (cd->isLinkableInProject())
    {
      Doxygen::indexList->addIndexItem(cd.get(),nullptr);
      if (Doxygen::searchIndex.enabled())
      {
        Doxygen::searchIndex.setCurrentDoc(cd.get(),cd->anchor(),FALSE);
        Doxygen::searchIndex.addWord(cd->localName(),TRUE);
      }
    }
  }

  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    if (nd->isLinkableInProject())
    {
      Doxygen::indexList->addIndexItem(nd.get(),nullptr);
      if (Doxygen::searchIndex.enabled())
      {
        Doxygen::searchIndex.setCurrentDoc(nd.get(),nd->anchor(),FALSE);
        Doxygen::searchIndex.addWord(nd->localName(),TRUE);
      }
    }
  }

  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      if (Doxygen::searchIndex.enabled() && fd->isLinkableInProject())
      {
        Doxygen::searchIndex.setCurrentDoc(fd.get(),fd->anchor(),FALSE);
        Doxygen::searchIndex.addWord(fd->localName(),TRUE);
      }
    }
  }

  auto addWordsForTitle = [](const Definition *d,const QCString &anchor,const QCString &title)
  {
      Doxygen::indexList->addIndexItem(d,nullptr,QCString(),filterTitle(title));
      if (Doxygen::searchIndex.enabled())
      {
        Doxygen::searchIndex.setCurrentDoc(d,anchor,false);
        std::string s = title.str();
        static const reg::Ex re(R"(\a[\w-]*)");
        reg::Iterator it(s,re);
        reg::Iterator end;
        for (; it!=end ; ++it)
        {
          const auto &match = *it;
          std::string matchStr = match.str();
          Doxygen::searchIndex.addWord(matchStr.c_str(),true);
        }
      }
  };

  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (gd->isLinkableInProject())
    {
      addWordsForTitle(gd.get(),gd->anchor(),gd->groupTitle());
    }
  }

  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->isLinkableInProject())
    {
      addWordsForTitle(pd.get(),pd->anchor(),pd->title());
    }
  }

  if (Doxygen::mainPage)
  {
    addWordsForTitle(Doxygen::mainPage.get(),Doxygen::mainPage->anchor(),Doxygen::mainPage->title());
  }

  auto addMemberToSearchIndex = [](const MemberDef *md)
  {
    if (Doxygen::searchIndex.enabled())
    {
      Doxygen::searchIndex.setCurrentDoc(md,md->anchor(),FALSE);
      QCString ln=md->localName();
      QCString qn=md->qualifiedName();
      Doxygen::searchIndex.addWord(ln,TRUE);
      if (ln!=qn)
      {
        Doxygen::searchIndex.addWord(qn,TRUE);
        if (md->getClassDef())
        {
          Doxygen::searchIndex.addWord(md->getClassDef()->displayName(),TRUE);
        }
        if (md->getNamespaceDef())
        {
          Doxygen::searchIndex.addWord(md->getNamespaceDef()->displayName(),TRUE);
        }
      }
    }
  };

  auto getScope = [](const MemberDef *md)
  {
    const Definition *scope = nullptr;
    if (md->getGroupDef())          scope = md->getGroupDef();
    else if (md->getClassDef())     scope = md->getClassDef();
    else if (md->getNamespaceDef()) scope = md->getNamespaceDef();
    else if (md->getFileDef())      scope = md->getFileDef();
    return scope;
  };

  auto addMemberToIndices = [addMemberToSearchIndex,getScope](const MemberDef *md)
  {
    if (md->isLinkableInProject())
    {
      if (!(md->isEnumerate() && md->isAnonymous()))
      {
        Doxygen::indexList->addIndexItem(getScope(md),md);
        addMemberToSearchIndex(md);
      }
      if (md->isEnumerate())
      {
        for (const auto &fmd : md->enumFieldList())
        {
          Doxygen::indexList->addIndexItem(getScope(fmd),fmd);
          addMemberToSearchIndex(fmd);
        }
      }
    }
  };

  // for each class member name
  for (const auto &mn : *Doxygen::memberNameLinkedMap)
  {
    // for each member definition
    for (const auto &md : *mn)
    {
      addMemberToIndices(md.get());
    }
  }
  // for each file/namespace function name
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    // for each member definition
    for (const auto &md : *mn)
    {
      addMemberToIndices(md.get());
    }
  }
}

//----------------------------------------------------------------------

static void vhdlCorrectMemberProperties()
{
  // for each member name
  for (const auto &mn : *Doxygen::memberNameLinkedMap)
  {
    // for each member definition
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md)
      {
        VhdlDocGen::correctMemberProperties(md);
      }
    }
  }
  // for each member name
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    // for each member definition
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md)
      {
        VhdlDocGen::correctMemberProperties(md);
      }
    }
  }
}

// recursive helper function looking for reimplements/implemented
// by relations between class cd and direct or indirect base class bcd
static void computeMemberRelationsForBaseClass(const ClassDef *cd,const BaseClassDef *bcd)
{
  for (const auto &mn : cd->memberNameInfoLinkedMap()) // for each member in class cd with a unique name
  {
    for (const auto &imd : *mn) // for each member with a given name
    {
      MemberDefMutable *md = toMemberDefMutable(imd->memberDef());
      if (md && (md->isFunction() || md->isCSharpProperty())) // filter on reimplementable members
      {
        ClassDef *mbcd = bcd->classDef;
        if (mbcd && mbcd->isLinkable()) // filter on linkable classes
        {
          const auto &bmn = mbcd->memberNameInfoLinkedMap();
          const auto &bmni = bmn.find(mn->memberName());
          if (bmni) // there are base class members with the same name
          {
            for (const auto &ibmd : *bmni) // for base class member with that name
            {
              MemberDefMutable *bmd = toMemberDefMutable(ibmd->memberDef());
              if (bmd) // not part of an inline namespace
              {
                if (bmd->virtualness()!=Specifier::Normal     ||
                    bmd->getLanguage()==SrcLangExt::Python     ||
                    bmd->getLanguage()==SrcLangExt::Java       ||
                    bmd->getLanguage()==SrcLangExt::PHP        ||
                    mbcd->compoundType()==ClassDef::Interface ||
                    mbcd->compoundType()==ClassDef::Protocol)
                {
                  const ArgumentList &bmdAl = bmd->argumentList();
                  const ArgumentList &mdAl =  md->argumentList();
                  //printf(" Base argList='%s'\n Super argList='%s'\n",
                  //        qPrint(argListToString(bmdAl)),
                  //        qPrint(argListToString(mdAl))
                  //      );
                  if (
                      bmd->getLanguage()==SrcLangExt::Python ||
                      matchArguments2(bmd->getOuterScope(),bmd->getFileDef(),&bmdAl,
                        md->getOuterScope(), md->getFileDef(), &mdAl,
                        TRUE,bmd->getLanguage()
                        )
                     )
                  {
                    //printf("match!\n");
                    const MemberDef *rmd = md->reimplements();
                    if (rmd==nullptr) // not already assigned
                    {
                      //printf("%s: setting (new) reimplements member %s\n",qPrint(md->qualifiedName()),qPrint(bmd->qualifiedName()));
                      md->setReimplements(bmd);
                    }
                    //printf("%s: add reimplementedBy member %s\n",qPrint(bmd->qualifiedName()),qPrint(md->qualifiedName()));
                    bmd->insertReimplementedBy(md);
                  }
                  else
                  {
                    //printf("no match!\n");
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // do also for indirect base classes
  for (const auto &bbcd : bcd->classDef->baseClasses())
  {
    computeMemberRelationsForBaseClass(cd,&bbcd);
  }
}

//----------------------------------------------------------------------
// computes the relation between all members. For each member 'm'
// the members that override the implementation of 'm' are searched and
// the member that 'm' overrides is searched.

static void computeMemberRelations()
{
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (cd->isLinkable())
    {
      for (const auto &bcd : cd->baseClasses())
      {
        computeMemberRelationsForBaseClass(cd.get(),&bcd);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void createTemplateInstanceMembers()
{
  // for each class
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    // that is a template
    for (const auto &ti : cd->getTemplateInstances())
    {
      ClassDefMutable *tcdm = toClassDefMutable(ti.classDef);
      if (tcdm)
      {
        tcdm->addMembersToTemplateInstance(cd.get(),cd->templateArguments(),ti.templSpec);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void mergeCategories()
{
  AUTO_TRACE();
  // merge members of categories into the class they extend
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    int i=cd->name().find('(');
    if (i!=-1) // it is an Objective-C category
    {
      QCString baseName=cd->name().left(i);
      ClassDefMutable *baseClass=toClassDefMutable(Doxygen::classLinkedMap->find(baseName));
      if (baseClass)
      {
        AUTO_TRACE_ADD("merging members of category {} into {}",cd->name(),baseClass->name());
        baseClass->mergeCategory(cd.get());
      }
    }
  }
}

// builds the list of all members for each class

static void buildCompleteMemberLists()
{
  // merge the member list of base classes into the inherited classes.
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    if (// !cd->isReference() && // not an external class
         cd->subClasses().empty() && // is a root of the hierarchy
         !cd->baseClasses().empty()) // and has at least one base class
    {
      ClassDefMutable *cdm = toClassDefMutable(cd.get());
      if (cdm)
      {
        //printf("*** merging members for %s\n",qPrint(cd->name()));
        cdm->mergeMembers();
      }
    }
  }
  // now sort the member list of all members for all classes.
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->sortAllMembersList();
    }
  }
}

//----------------------------------------------------------------------------

static void generateFileSources()
{
  auto processSourceFile = [](FileDef *fd,OutputList &ol,ClangTUParser *parser)
  {
    bool showSources  = fd->generateSourceFile() && !Htags::useHtags; // sources need to be shown in the output
    bool parseSources = !fd->isReference() && Doxygen::parseSourcesNeeded; // we needed to parse the sources even if we do not show them
    if (showSources)
    {
      msg("Generating code for file %s...\n",qPrint(fd->docName()));
      fd->writeSourceHeader(ol);
      fd->writeSourceBody(ol,parser);
      fd->writeSourceFooter(ol);
    }
    else if (parseSources)
    {
      msg("Parsing code for file %s...\n",qPrint(fd->docName()));
      fd->parseSource(parser);
    }
  };
  if (!Doxygen::inputNameLinkedMap->empty())
  {
#if USE_LIBCLANG
    if (Doxygen::clangAssistedParsing)
    {
      StringUnorderedSet processedFiles;

      // create a dictionary with files to process
      StringUnorderedSet filesToProcess;

      for (const auto &fn : *Doxygen::inputNameLinkedMap)
      {
        for (const auto &fd : *fn)
        {
          filesToProcess.insert(fd->absFilePath().str());
        }
      }
      // process source files (and their include dependencies)
      for (const auto &fn : *Doxygen::inputNameLinkedMap)
      {
        for (const auto &fd : *fn)
        {
          if (fd->isSource() && !fd->isReference() && fd->getLanguage()==SrcLangExt::Cpp &&
              (fd->generateSourceFile() ||
               (!fd->isReference() && Doxygen::parseSourcesNeeded)
              )
             )
          {
            auto clangParser = ClangParser::instance()->createTUParser(fd.get());
            clangParser->parse();
            processSourceFile(fd.get(),*g_outputList,clangParser.get());

            for (auto incFile : clangParser->filesInSameTU())
            {
              if (filesToProcess.find(incFile)!=filesToProcess.end() &&  // part of input
                  fd->absFilePath()!=QCString(incFile) &&                // not same file
                  processedFiles.find(incFile)==processedFiles.end())    // not yet marked as processed
              {
                StringVector moreFiles;
                bool ambig = false;
                FileDef *ifd=findFileDef(Doxygen::inputNameLinkedMap,incFile.c_str(),ambig);
                if (ifd && !ifd->isReference())
                {
                  processSourceFile(ifd,*g_outputList,clangParser.get());
                  processedFiles.insert(incFile);
                }
              }
            }
            processedFiles.insert(fd->absFilePath().str());
          }
        }
      }
      // process remaining files
      for (const auto &fn : *Doxygen::inputNameLinkedMap)
      {
        for (const auto &fd : *fn)
        {
          if (processedFiles.find(fd->absFilePath().str())==processedFiles.end()) // not yet processed
          {
            if (fd->getLanguage()==SrcLangExt::Cpp) // C/C++ file, use clang parser
            {
              auto clangParser = ClangParser::instance()->createTUParser(fd.get());
              clangParser->parse();
              processSourceFile(fd.get(),*g_outputList,clangParser.get());
            }
            else // non C/C++ file, use built-in parser
            {
              processSourceFile(fd.get(),*g_outputList,nullptr);
            }
          }
        }
      }
    }
    else
#endif
    {
      std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
      if (numThreads>1)
      {
        msg("Generating code files using %zu threads.\n",numThreads);
        struct SourceContext
        {
          SourceContext(FileDef *fd_,bool gen_,const OutputList &ol_)
            : fd(fd_), generateSourceFile(gen_), ol(ol_) {}
          FileDef *fd;
          bool generateSourceFile;
          OutputList ol;
        };
        ThreadPool threadPool(numThreads);
        std::vector< std::future< std::shared_ptr<SourceContext> > > results;
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            bool generateSourceFile = fd->generateSourceFile() && !Htags::useHtags;
            auto ctx = std::make_shared<SourceContext>(fd.get(),generateSourceFile,*g_outputList);
            auto processFile = [ctx]()
            {
              if (ctx->generateSourceFile)
              {
                msg("Generating code for file %s...\n",qPrint(ctx->fd->docName()));
              }
              else
              {
                msg("Parsing code for file %s...\n",qPrint(ctx->fd->docName()));
              }
              StringVector filesInSameTu;
              ctx->fd->getAllIncludeFilesRecursively(filesInSameTu);
              if (ctx->generateSourceFile) // sources need to be shown in the output
              {
                ctx->fd->writeSourceHeader(ctx->ol);
                ctx->fd->writeSourceBody(ctx->ol,nullptr);
                ctx->fd->writeSourceFooter(ctx->ol);
              }
              else if (!ctx->fd->isReference() && Doxygen::parseSourcesNeeded)
                // we needed to parse the sources even if we do not show them
              {
                ctx->fd->parseSource(nullptr);
              }
              return ctx;
            };
            results.emplace_back(threadPool.queue(processFile));
          }
        }
        for (auto &f : results)
        {
          auto ctx = f.get();
        }
      }
      else // single threaded version
      {
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            StringVector filesInSameTu;
            fd->getAllIncludeFilesRecursively(filesInSameTu);
            processSourceFile(fd.get(),*g_outputList,nullptr);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void generateFileDocs()
{
  if (Index::instance().numDocumentedFiles()==0) return;

  if (!Doxygen::inputNameLinkedMap->empty())
  {
    std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
    if (numThreads>1) // multi threaded processing
    {
      struct DocContext
      {
        DocContext(FileDef *fd_,const OutputList &ol_)
          : fd(fd_), ol(ol_) {}
        FileDef *fd;
        OutputList ol;
      };
      ThreadPool threadPool(numThreads);
      std::vector< std::future< std::shared_ptr<DocContext> > > results;
      for (const auto &fn : *Doxygen::inputNameLinkedMap)
      {
        for (const auto &fd : *fn)
        {
          bool doc = fd->isLinkableInProject();
          if (doc)
          {
            auto ctx = std::make_shared<DocContext>(fd.get(),*g_outputList);
            auto processFile = [ctx]() {
              msg("Generating docs for file %s...\n",qPrint(ctx->fd->docName()));
              ctx->fd->writeDocumentation(ctx->ol);
              return ctx;
            };
            results.emplace_back(threadPool.queue(processFile));
          }
        }
      }
      for (auto &f : results)
      {
        auto ctx = f.get();
      }
    }
    else // single threaded processing
    {
      for (const auto &fn : *Doxygen::inputNameLinkedMap)
      {
        for (const auto &fd : *fn)
        {
          bool doc = fd->isLinkableInProject();
          if (doc)
          {
            msg("Generating docs for file %s...\n",qPrint(fd->docName()));
            fd->writeDocumentation(*g_outputList);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void addSourceReferences()
{
  // add source references for class definitions
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    const FileDef *fd=cd->getBodyDef();
    if (fd && cd->isLinkableInProject() && cd->getStartDefLine()!=-1)
    {
      const_cast<FileDef*>(fd)->addSourceRef(cd->getStartDefLine(),cd.get(),nullptr);
    }
  }
  // add source references for concept definitions
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    const FileDef *fd=cd->getBodyDef();
    if (fd && cd->isLinkableInProject() && cd->getStartDefLine()!=-1)
    {
      const_cast<FileDef*>(fd)->addSourceRef(cd->getStartDefLine(),cd.get(),nullptr);
    }
  }
  // add source references for namespace definitions
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    const FileDef *fd=nd->getBodyDef();
    if (fd && nd->isLinkableInProject() && nd->getStartDefLine()!=-1)
    {
      const_cast<FileDef*>(fd)->addSourceRef(nd->getStartDefLine(),nd.get(),nullptr);
    }
  }

  // add source references for member names
  for (const auto &mn : *Doxygen::memberNameLinkedMap)
  {
    for (const auto &md : *mn)
    {
      //printf("class member %s: def=%s body=%d link?=%d\n",
      //    qPrint(md->name()),
      //    md->getBodyDef()?qPrint(md->getBodyDef()->name()):"<none>",
      //    md->getStartBodyLine(),md->isLinkableInProject());
      const FileDef *fd=md->getBodyDef();
      if (fd &&
          md->getStartDefLine()!=-1 &&
          md->isLinkableInProject() &&
          (fd->generateSourceFile() || Doxygen::parseSourcesNeeded)
         )
      {
        //printf("Found member '%s' in file '%s' at line '%d' def=%s\n",
        //    qPrint(md->name()),qPrint(fd->name()),md->getStartBodyLine(),qPrint(md->getOuterScope()->name()));
        const_cast<FileDef*>(fd)->addSourceRef(md->getStartDefLine(),md->getOuterScope(),md.get());
      }
    }
  }
  for (const auto &mn : *Doxygen::functionNameLinkedMap)
  {
    for (const auto &md : *mn)
    {
      const FileDef *fd=md->getBodyDef();
      //printf("member %s body=[%d,%d] fd=%p link=%d parseSources=%d\n",
      //    qPrint(md->name()),
      //    md->getStartBodyLine(),md->getEndBodyLine(),fd,
      //    md->isLinkableInProject(),
      //    Doxygen::parseSourcesNeeded);
      if (fd &&
          md->getStartDefLine()!=-1 &&
          md->isLinkableInProject() &&
          (fd->generateSourceFile() || Doxygen::parseSourcesNeeded)
         )
      {
        //printf("Found member '%s' in file '%s' at line '%d' def=%s\n",
        //    qPrint(md->name()),qPrint(fd->name()),md->getStartBodyLine(),qPrint(md->getOuterScope()->name()));
        const_cast<FileDef*>(fd)->addSourceRef(md->getStartDefLine(),md->getOuterScope(),md.get());
      }
    }
  }
}

//----------------------------------------------------------------------------

// add the macro definitions found during preprocessing as file members
static void buildDefineList()
{
  AUTO_TRACE();
  for (const auto &s : g_inputFiles)
  {
    auto it = Doxygen::macroDefinitions.find(s);
    if (it!=Doxygen::macroDefinitions.end())
    {
      for (const auto &def : it->second)
      {
        auto md = createMemberDef(
            def.fileName,def.lineNr,def.columnNr,
            "#define",def.name,def.args,QCString(),
            Protection::Public,Specifier::Normal,FALSE,Relationship::Member,MemberType::Define,
            ArgumentList(),ArgumentList(),"");
        auto mmd = toMemberDefMutable(md.get());

        if (!def.args.isEmpty())
        {
          mmd->moveArgumentList(stringToArgumentList(SrcLangExt::Cpp, def.args));
        }
        mmd->setInitializer(def.definition);
        mmd->setFileDef(def.fileDef);
        mmd->setDefinition("#define "+def.name);

        MemberName *mn=Doxygen::functionNameLinkedMap->add(def.name);
        if (def.fileDef)
        {
          def.fileDef->insertMember(md.get());
        }
        AUTO_TRACE_ADD("adding macro {} with definition {}",def.name,def.definition);
        mn->push_back(std::move(md));
      }
    }
  }
}

//----------------------------------------------------------------------------

static void sortMemberLists()
{
  // sort class member lists
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->sortMemberLists();
    }
  }

  // sort namespace member lists
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->sortMemberLists();
    }
  }

  // sort file member lists
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->sortMemberLists();
    }
  }

  // sort group member lists
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->sortMemberLists();
  }

  ModuleManager::instance().sortMemberLists();
}

//----------------------------------------------------------------------------

static bool isSymbolHidden(const Definition *d)
{
  bool hidden = d->isHidden();
  const Definition *parent = d->getOuterScope();
  return parent ? hidden || isSymbolHidden(parent) : hidden;
}

static void computeTooltipTexts()
{
  std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
  if (numThreads>1)
  {
    ThreadPool threadPool(numThreads);
    std::vector < std::future< void > > results;
    // queue the work
    for (const auto &[name,symList] : *Doxygen::symbolMap)
    {
      for (const auto &def : symList)
      {
        DefinitionMutable *dm = toDefinitionMutable(def);
        if (dm && !isSymbolHidden(def) && !def->isArtificial() && def->isLinkableInProject())
        {
          auto processTooltip = [dm]() {
            dm->computeTooltip();
          };
          results.emplace_back(threadPool.queue(processTooltip));
        }
      }
    }
    // wait for the results
    for (auto &f : results)
    {
      f.get();
    }
  }
  else
  {
    for (const auto &[name,symList] : *Doxygen::symbolMap)
    {
      for (const auto &def : symList)
      {
        DefinitionMutable *dm = toDefinitionMutable(def);
        if (dm && !isSymbolHidden(def) && !def->isArtificial() && def->isLinkableInProject())
        {
          dm->computeTooltip();
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void setAnonymousEnumType()
{
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->setAnonymousEnumType();
    }
  }
}

//----------------------------------------------------------------------------

static void countMembers()
{
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->countMembers();
    }
  }

  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->countMembers();
    }
  }

  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->countMembers();
    }
  }

  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->countMembers();
  }

  auto &mm = ModuleManager::instance();
  mm.countMembers();
}


//----------------------------------------------------------------------------
// generate the documentation for all classes

static void generateDocsForClassList(const std::vector<ClassDefMutable*> &classList)
{
  std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
  if (numThreads>1) // multi threaded processing
  {
    struct DocContext
    {
      DocContext(ClassDefMutable *cd_,const OutputList &ol_)
        : cd(cd_), ol(ol_) {}
      ClassDefMutable *cd;
      OutputList ol;
    };
    ThreadPool threadPool(numThreads);
    std::vector< std::future< std::shared_ptr<DocContext> > > results;
    for (const auto &cd : classList)
    {
      //printf("cd=%s getOuterScope=%p global=%p\n",qPrint(cd->name()),cd->getOuterScope(),Doxygen::globalScope);
      if (cd->getOuterScope()==nullptr || // <-- should not happen, but can if we read an old tag file
           cd->getOuterScope()==Doxygen::globalScope // only look at global classes
         )
      {
        auto ctx = std::make_shared<DocContext>(cd,*g_outputList);
        auto processFile = [ctx]()
        {
          msg("Generating docs for compound %s...\n",qPrint(ctx->cd->displayName()));

          // skip external references, anonymous compounds and
          // template instances
          if (!ctx->cd->isHidden() && !ctx->cd->isEmbeddedInOuterScope() &&
              ctx->cd->isLinkableInProject() && ctx->cd->templateMaster()==nullptr)
          {
            ctx->cd->writeDocumentation(ctx->ol);
            ctx->cd->writeMemberList(ctx->ol);
          }

          // even for undocumented classes, the inner classes can be documented.
          ctx->cd->writeDocumentationForInnerClasses(ctx->ol);
          return ctx;
        };
        results.emplace_back(threadPool.queue(processFile));
      }
    }
    for (auto &f : results)
    {
      auto ctx = f.get();
    }
  }
  else // single threaded processing
  {
    for (const auto &cd : classList)
    {
      //printf("cd=%s getOuterScope=%p global=%p hidden=%d embeddedInOuterScope=%d\n",
      //    qPrint(cd->name()),cd->getOuterScope(),Doxygen::globalScope,cd->isHidden(),cd->isEmbeddedInOuterScope());
      if (cd->getOuterScope()==nullptr || // <-- should not happen, but can if we read an old tag file
          cd->getOuterScope()==Doxygen::globalScope // only look at global classes
         )
      {
        // skip external references, anonymous compounds and
        // template instances
        if ( !cd->isHidden() && !cd->isEmbeddedInOuterScope() &&
              cd->isLinkableInProject() && cd->templateMaster()==nullptr)
        {
          msg("Generating docs for compound %s...\n",qPrint(cd->displayName()));

          cd->writeDocumentation(*g_outputList);
          cd->writeMemberList(*g_outputList);
        }
        // even for undocumented classes, the inner classes can be documented.
        cd->writeDocumentationForInnerClasses(*g_outputList);
      }
    }
  }
}

static void addClassAndNestedClasses(std::vector<ClassDefMutable*> &list,ClassDefMutable *cd)
{
  list.push_back(cd);
  for (const auto &innerCdi : cd->getClasses())
  {
    ClassDefMutable *innerCd = toClassDefMutable(innerCdi);
    if (innerCd && innerCd->isLinkableInProject() && innerCd->templateMaster()==nullptr &&
        protectionLevelVisible(innerCd->protection()) &&
        !innerCd->isEmbeddedInOuterScope()
       )
    {
      list.push_back(innerCd);
      addClassAndNestedClasses(list,innerCd);
    }
  }
}

static void generateClassDocs()
{
  std::vector<ClassDefMutable*> classList;
  for (const auto &cdi : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cd = toClassDefMutable(cdi.get());
    if (cd && (cd->getOuterScope()==nullptr ||
               cd->getOuterScope()->definitionType()!=Definition::TypeClass))
    {
      addClassAndNestedClasses(classList,cd);
    }
  }
  for (const auto &cdi : *Doxygen::hiddenClassLinkedMap)
  {
    ClassDefMutable *cd = toClassDefMutable(cdi.get());
    if (cd && (cd->getOuterScope()==nullptr ||
               cd->getOuterScope()->definitionType()!=Definition::TypeClass))
    {
      addClassAndNestedClasses(classList,cd);
    }
  }
  generateDocsForClassList(classList);
}

//----------------------------------------------------------------------------

static void generateConceptDocs()
{
  for (const auto &cdi : *Doxygen::conceptLinkedMap)
  {
    ConceptDefMutable *cd=toConceptDefMutable(cdi.get());

    //printf("cd=%s getOuterScope=%p global=%p\n",qPrint(cd->name()),cd->getOuterScope(),Doxygen::globalScope);
    if (cd &&
        (cd->getOuterScope()==nullptr || // <-- should not happen, but can if we read an old tag file
         cd->getOuterScope()==Doxygen::globalScope // only look at global concepts
        ) && !cd->isHidden() && cd->isLinkableInProject()
       )
    {
      msg("Generating docs for concept %s...\n",qPrint(cd->displayName()));
      cd->writeDocumentation(*g_outputList);
    }
  }
}

//----------------------------------------------------------------------------

static void inheritDocumentation()
{
  for (const auto &mn : *Doxygen::memberNameLinkedMap)
  {
    for (const auto &imd : *mn)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      //static int count=0;
      //printf("%04d Member '%s'\n",count++,qPrint(md->qualifiedName()));
      if (md && md->documentation().isEmpty() && md->briefDescription().isEmpty())
      { // no documentation yet
        const MemberDef *bmd = md->reimplements();
        while (bmd && bmd->documentation().isEmpty() &&
                      bmd->briefDescription().isEmpty()
              )
        { // search up the inheritance tree for a documentation member
          //printf("bmd=%s class=%s\n",qPrint(bmd->name()),qPrint(bmd->getClassDef()->name()));
          bmd = bmd->reimplements();
        }
        if (bmd) // copy the documentation from the reimplemented member
        {
          md->setInheritsDocsFrom(bmd);
          md->setDocumentation(bmd->documentation(),bmd->docFile(),bmd->docLine());
          md->setDocsForDefinition(bmd->isDocsForDefinition());
          md->setBriefDescription(bmd->briefDescription(),bmd->briefFile(),bmd->briefLine());
          md->copyArgumentNames(bmd);
          md->setInbodyDocumentation(bmd->inbodyDocumentation(),bmd->inbodyFile(),bmd->inbodyLine());
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void combineUsingRelations()
{
  // for each file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->combineUsingRelations();
    }
  }

  // for each namespace
  NamespaceDefSet visitedNamespaces;
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->combineUsingRelations(visitedNamespaces);
    }
  }
}

//----------------------------------------------------------------------------

static void addMembersToMemberGroup()
{
  // for each class
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->addMembersToMemberGroup();
    }
  }
  // for each file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->addMembersToMemberGroup();
    }
  }
  // for each namespace
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->addMembersToMemberGroup();
    }
  }
  // for each group
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->addMembersToMemberGroup();
  }
  ModuleManager::instance().addMembersToMemberGroup();
}

//----------------------------------------------------------------------------

static void distributeMemberGroupDocumentation()
{
  // for each class
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->distributeMemberGroupDocumentation();
    }
  }
  // for each file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->distributeMemberGroupDocumentation();
    }
  }
  // for each namespace
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->distributeMemberGroupDocumentation();
    }
  }
  // for each group
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->distributeMemberGroupDocumentation();
  }
  ModuleManager::instance().distributeMemberGroupDocumentation();
}

//----------------------------------------------------------------------------

static void findSectionsInDocumentation()
{
  // for each class
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm)
    {
      cdm->findSectionsInDocumentation();
    }
  }
  // for each concept
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    ConceptDefMutable *cdm = toConceptDefMutable(cd.get());
    if (cdm)
    {
      cdm->findSectionsInDocumentation();
    }
  }
  // for each file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      fd->findSectionsInDocumentation();
    }
  }
  // for each namespace
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm)
    {
      ndm->findSectionsInDocumentation();
    }
  }
  // for each group
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    gd->findSectionsInDocumentation();
  }
  // for each page
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    pd->findSectionsInDocumentation();
  }
  ModuleManager::instance().findSectionsInDocumentation();
  if (Doxygen::mainPage) Doxygen::mainPage->findSectionsInDocumentation();
}

//----------------------------------------------------------------------


static void flushCachedTemplateRelations()
{
  // remove all references to classes from the cache
  // as there can be new template instances in the inheritance path
  // to this class. Optimization: only remove those classes that
  // have inheritance instances as direct or indirect sub classes.
  StringVector elementsToRemove;
  for (const auto &ci : *Doxygen::typeLookupCache)
  {
    const LookupInfo &li = ci.second;
    if (li.definition)
    {
      elementsToRemove.push_back(ci.first);
    }
  }
  for (const auto &k : elementsToRemove)
  {
    Doxygen::typeLookupCache->remove(k);
  }

  // remove all cached typedef resolutions whose target is a
  // template class as this may now be a template instance
  // for each global function name
  for (const auto &fn : *Doxygen::functionNameLinkedMap)
  {
    // for each function with that name
    for (const auto &ifmd : *fn)
    {
      MemberDefMutable *fmd = toMemberDefMutable(ifmd.get());
      if (fmd && fmd->isTypedefValCached())
      {
        const ClassDef *cd = fmd->getCachedTypedefVal();
        if (cd->isTemplate()) fmd->invalidateTypedefValCache();
      }
    }
  }
  // for each class method name
  for (const auto &nm : *Doxygen::memberNameLinkedMap)
  {
    // for each function with that name
    for (const auto &imd : *nm)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md && md->isTypedefValCached())
      {
        const ClassDef *cd = md->getCachedTypedefVal();
        if (cd->isTemplate()) md->invalidateTypedefValCache();
      }
    }
  }
}

//----------------------------------------------------------------------------

static void flushUnresolvedRelations()
{
  // Remove all unresolved references to classes from the cache.
  // This is needed before resolving the inheritance relations, since
  // it would otherwise not find the inheritance relation
  // for C in the example below, as B::I was already found to be unresolvable
  // (which is correct if you ignore the inheritance relation between A and B).
  //
  // class A { class I {} };
  // class B : public A {};
  // class C : public B::I {};

  StringVector elementsToRemove;
  for (const auto &ci : *Doxygen::typeLookupCache)
  {
    const LookupInfo &li = ci.second;
    if (li.definition==nullptr && li.typeDef==nullptr)
    {
      elementsToRemove.push_back(ci.first);
    }
  }
  for (const auto &k : elementsToRemove)
  {
    Doxygen::typeLookupCache->remove(k);
  }

  // for each global function name
  for (const auto &fn : *Doxygen::functionNameLinkedMap)
  {
    // for each function with that name
    for (const auto &ifmd : *fn)
    {
      MemberDefMutable *fmd = toMemberDefMutable(ifmd.get());
      if (fmd)
      {
        fmd->invalidateCachedArgumentTypes();
      }
    }
  }
  // for each class method name
  for (const auto &nm : *Doxygen::memberNameLinkedMap)
  {
    // for each function with that name
    for (const auto &imd : *nm)
    {
      MemberDefMutable *md = toMemberDefMutable(imd.get());
      if (md)
      {
        md->invalidateCachedArgumentTypes();
      }
    }
  }

}

//----------------------------------------------------------------------------
// Returns TRUE if the entry and member definition have equal file names,
// otherwise FALSE.

static bool haveEqualFileNames(const Entry *root,const MemberDef *md)
{
  const FileDef *fd = md->getFileDef();
  if (!fd)
  {
    return FALSE;
  }

  return fd->absFilePath() == root->fileName;
}

//----------------------------------------------------------------------------

static void addDefineDoc(const Entry *root, MemberDefMutable *md)
{
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setDocsForDefinition(!root->proto);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  if (md->inbodyDocumentation().isEmpty())
  {
    md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  }
  if (md->getStartBodyLine()==-1 && root->bodyLine!=-1)
  {
    md->setBodySegment(root->startLine,root->bodyLine,root->endBodyLine);
    md->setBodyDef(root->fileDef());
  }
  md->addSectionsToDefinition(root->anchors);
  md->setMaxInitLines(root->initLines);
  applyMemberOverrideOptions(root,md);
  md->setRefItems(root->sli);
  if (root->mGrpId!=-1) md->setMemberGroupId(root->mGrpId);
  addMemberToGroups(root,md);
  ModuleManager::instance().addMemberToModule(root,md);
}

//----------------------------------------------------------------------------

static void findDefineDocumentation(Entry *root)
{
  if ((root->section.isDefineDoc() || root->section.isDefine()) && !root->name.isEmpty())
  {
    //printf("found define '%s' '%s' brief='%s' doc='%s'\n",
    //       qPrint(root->name),qPrint(root->args),qPrint(root->brief),qPrint(root->doc));

    if (root->tagInfo() && !root->name.isEmpty()) // define read from a tag file
    {
      auto md = createMemberDef(root->tagInfo()->tagName,1,1,
                    "#define",root->name,root->args,QCString(),
                    Protection::Public,Specifier::Normal,FALSE,Relationship::Member,MemberType::Define,
                    ArgumentList(),ArgumentList(),"");
      auto mmd = toMemberDefMutable(md.get());
      mmd->setTagInfo(root->tagInfo());
      mmd->setLanguage(root->lang);
      //printf("Searching for '%s' fd=%p\n",qPrint(filePathName),fd);
      mmd->setFileDef(root->parent()->fileDef());
      //printf("Adding member=%s\n",qPrint(md->name()));
      MemberName *mn = Doxygen::functionNameLinkedMap->add(root->name);
      mn->push_back(std::move(md));
    }
    MemberName *mn=Doxygen::functionNameLinkedMap->find(root->name);
    if (mn)
    {
      int count=0;
      for (const auto &md : *mn)
      {
        if (md->memberType()==MemberType::Define) count++;
      }
      if (count==1)
      {
        for (const auto &imd : *mn)
        {
          MemberDefMutable *md = toMemberDefMutable(imd.get());
          if (md && md->memberType()==MemberType::Define)
          {
            addDefineDoc(root,md);
          }
        }
      }
      else if (count>1 &&
               (!root->doc.isEmpty() ||
                !root->brief.isEmpty() ||
                root->bodyLine!=-1
               )
              )
        // multiple defines don't know where to add docs
        // but maybe they are in different files together with their documentation
      {
        for (const auto &imd : *mn)
        {
          MemberDefMutable *md = toMemberDefMutable(imd.get());
          if (md && md->memberType()==MemberType::Define)
          {
            if (haveEqualFileNames(root, md) || isEntryInGroupOfMember(root, md))
              // doc and define in the same file or group assume they belong together.
            {
              addDefineDoc(root,md);
            }
          }
        }
        //warn("define %s found in the following files:\n",qPrint(root->name));
        //warn("Cannot determine where to add the documentation found "
        //     "at line %d of file %s. \n",
        //     root->startLine,qPrint(root->fileName));
      }
    }
    else if (!root->doc.isEmpty() || !root->brief.isEmpty()) // define not found
    {
      bool preEnabled = Config_getBool(ENABLE_PREPROCESSING);
      if (preEnabled)
      {
        warn(root->fileName,root->startLine,
             "documentation for unknown define %s found.",
             qPrint(root->name)
            );
      }
      else
      {
        warn(root->fileName,root->startLine,
             "found documented #define %s but ignoring it because "
             "ENABLE_PREPROCESSING is NO.",
             qPrint(root->name)
            );
      }
    }
  }
  for (const auto &e : root->children()) findDefineDocumentation(e.get());
}

//----------------------------------------------------------------------------

static void findDirDocumentation(const Entry *root)
{
  if (root->section.isDirDoc())
  {
    QCString normalizedName = root->name;
    normalizedName = substitute(normalizedName,"\\","/");
    //printf("root->docFile=%s normalizedName=%s\n",
    //    qPrint(root->docFile),qPrint(normalizedName));
    if (root->docFile==normalizedName) // current dir?
    {
      int lastSlashPos=normalizedName.findRev('/');
      if (lastSlashPos!=-1) // strip file name
      {
        normalizedName=normalizedName.left(lastSlashPos);
      }
    }
    if (normalizedName.at(normalizedName.length()-1)!='/')
    {
      normalizedName+='/';
    }
    DirDef *matchingDir=nullptr;
    for (const auto &dir : *Doxygen::dirLinkedMap)
    {
      //printf("Dir: %s<->%s\n",qPrint(dir->name()),qPrint(normalizedName));
      if (dir->name().right(normalizedName.length())==normalizedName)
      {
        if (matchingDir)
        {
           warn(root->fileName,root->startLine,
             "\\dir command matches multiple directories.\n"
             "  Applying the command for directory %s\n"
             "  Ignoring the command for directory %s",
             qPrint(matchingDir->name()),qPrint(dir->name())
           );
        }
        else
        {
          matchingDir=dir.get();
        }
      }
    }
    if (matchingDir)
    {
      //printf("Match for with dir %s\n",qPrint(matchingDir->name()));
      matchingDir->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      matchingDir->setDocumentation(root->doc,root->docFile,root->docLine);
      matchingDir->setRefItems(root->sli);
      root->commandOverrides.apply_directoryGraph([&](bool b) { matchingDir->overrideDirectoryGraph(b); });
      addDirToGroups(root,matchingDir);
    }
    else
    {
      warn(root->fileName,root->startLine,"No matching "
          "directory found for command \\dir %s",qPrint(normalizedName));
    }
  }
  for (const auto &e : root->children()) findDirDocumentation(e.get());
}


//----------------------------------------------------------------------------
// create a (sorted) list of separate documentation pages

static void buildPageList(Entry *root)
{
  if (root->section.isPageDoc())
  {
    if (!root->name.isEmpty())
    {
      addRelatedPage(root);
    }
  }
  else if (root->section.isMainpageDoc())
  {
    QCString title=root->args.stripWhiteSpace();
    if (title.isEmpty()) title=theTranslator->trMainPage();
    //QCString name = Config_getBool(GENERATE_TREEVIEW)?"main":"index";
    QCString name = "index";
    addRefItem(root->sli,
               name,
               "page",
               name,
               title,
               QCString(),nullptr
               );
  }
  for (const auto &e : root->children()) buildPageList(e.get());
}

// search for the main page defined in this project
static void findMainPage(Entry *root)
{
  if (root->section.isMainpageDoc())
  {
    if (Doxygen::mainPage==nullptr && root->tagInfo()==nullptr)
    {
      //printf("mainpage: docLine=%d startLine=%d\n",root->docLine,root->startLine);
      //printf("Found main page! \n======\n%s\n=======\n",qPrint(root->doc));
      QCString title=root->args.stripWhiteSpace();
      if (title.isEmpty()) title = Config_getString(PROJECT_NAME);
      //QCString indexName=Config_getBool(GENERATE_TREEVIEW)?"main":"index";
      QCString indexName="index";
      Doxygen::mainPage = createPageDef(root->docFile,root->docLine,
                              indexName, root->brief+root->doc+root->inbodyDocs,title);
      //setFileNameForSections(root->anchors,"index",Doxygen::mainPage);
      Doxygen::mainPage->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      Doxygen::mainPage->setBodySegment(root->startLine,root->startLine,-1);
      Doxygen::mainPage->setFileName(indexName);
      Doxygen::mainPage->setLocalToc(root->localToc);
      addPageToContext(Doxygen::mainPage.get(),root);

      const SectionInfo *si = SectionManager::instance().find(Doxygen::mainPage->name());
      if (si)
      {
        if (!si->ref().isEmpty()) // we are from a tag file
        {
          // a page name is a label as well! but should no be double either
          SectionManager::instance().replace(
            Doxygen::mainPage->name(),
            indexName,
            root->startLine,
            Doxygen::mainPage->title(),
            SectionType::Page,
            0); // level 0
        }
        else if (si->lineNr() != -1)
        {
          warn(root->fileName,root->startLine,"multiple use of section label '%s' for main page, (first occurrence: %s, line %d)",
               qPrint(Doxygen::mainPage->name()),qPrint(si->fileName()),si->lineNr());
        }
        else
        {
          warn(root->fileName,root->startLine,"multiple use of section label '%s' for main page, (first occurrence: %s)",
               qPrint(Doxygen::mainPage->name()),qPrint(si->fileName()));
        }
      }
      else
      {
        // a page name is a label as well! but should no be double either
        SectionManager::instance().add(
          Doxygen::mainPage->name(),
          indexName,
          root->startLine,
          Doxygen::mainPage->title(),
          SectionType::Page,
          0); // level 0
      }
      Doxygen::mainPage->addSectionsToDefinition(root->anchors);
    }
    else if (root->tagInfo()==nullptr)
    {
      warn(root->fileName,root->startLine,
           "found more than one \\mainpage comment block! (first occurrence: %s, line %d), Skipping current block!",
           qPrint(Doxygen::mainPage->docFile()),Doxygen::mainPage->getStartBodyLine());
    }
  }
  for (const auto &e : root->children()) findMainPage(e.get());
}

// search for the main page imported via tag files and add only the section labels
static void findMainPageTagFiles(Entry *root)
{
  if (root->section.isMainpageDoc())
  {
    if (Doxygen::mainPage && root->tagInfo())
    {
      Doxygen::mainPage->addSectionsToDefinition(root->anchors);
    }
  }
  for (const auto &e : root->children()) findMainPageTagFiles(e.get());
}

static void computePageRelations(Entry *root)
{
  if ((root->section.isPageDoc() || root->section.isMainpageDoc()) && !root->name.isEmpty())
  {
    PageDef *pd = root->section.isPageDoc() ?
                    Doxygen::pageLinkedMap->find(root->name) :
                    Doxygen::mainPage.get();
    if (pd)
    {
      for (const BaseInfo &bi : root->extends)
      {
        PageDef *subPd = Doxygen::pageLinkedMap->find(bi.name);
        if (pd==subPd)
        {
         term("page defined %s with label %s is a direct "
             "subpage of itself! Please remove this cyclic dependency.\n",
              qPrint(warn_line(pd->docFile(),pd->docLine())),qPrint(pd->name()));
        }
        else if (subPd)
        {
          pd->addInnerCompound(subPd);
          //printf("*** Added subpage relation: %s->%s\n",
          //    qPrint(pd->name()),qPrint(subPd->name()));
        }
      }
    }
  }
  for (const auto &e : root->children()) computePageRelations(e.get());
}

static void checkPageRelations()
{
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    Definition *ppd = pd->getOuterScope();
    while (ppd)
    {
      if (ppd==pd.get())
      {
        term("page defined %s with label %s is a subpage "
             "of itself! Please remove this cyclic dependency.\n",
              qPrint(warn_line(pd->docFile(),pd->docLine())),qPrint(pd->name()));
      }
      ppd=ppd->getOuterScope();
    }
  }
}

//----------------------------------------------------------------------------

static void resolveUserReferences()
{
  for (const auto &si : SectionManager::instance())
  {
    //printf("si->label='%s' si->definition=%s si->fileName='%s'\n",
    //        qPrint(si->label),si->definition?qPrint(si->definition->name()):"<none>",
    //        qPrint(si->fileName));
    PageDef *pd=nullptr;

    // hack: the items of a todo/test/bug/deprecated list are all fragments from
    // different files, so the resulting section's all have the wrong file
    // name (not from the todo/test/bug/deprecated list, but from the file in
    // which they are defined). We correct this here by looking at the
    // generated section labels!
    for (const RefListManager::Ptr &rl : RefListManager::instance())
    {
      QCString label="_"+rl->listName(); // "_todo", "_test", ...
      if (si->label().left(label.length())==label)
      {
        si->setFileName(rl->listName());
        si->setGenerated(TRUE);
        break;
      }
    }

    //printf("start: si->label=%s si->fileName=%s\n",qPrint(si->label),qPrint(si->fileName));
    if (!si->generated())
    {
      // if this section is in a page and the page is in a group, then we
      // have to adjust the link file name to point to the group.
      if (!si->fileName().isEmpty() &&
          (pd=Doxygen::pageLinkedMap->find(si->fileName())) &&
          pd->getGroupDef())
      {
        si->setFileName(pd->getGroupDef()->getOutputFileBase());
      }

      if (si->definition())
      {
        // TODO: there should be one function in Definition that returns
        // the file to link to, so we can avoid the following tests.
        const GroupDef *gd=nullptr;
        if (si->definition()->definitionType()==Definition::TypeMember)
        {
          gd = (toMemberDef(si->definition()))->getGroupDef();
        }

        if (gd)
        {
          si->setFileName(gd->getOutputFileBase());
        }
        else
        {
          //si->fileName=si->definition->getOutputFileBase();
          //printf("Setting si->fileName to %s\n",qPrint(si->fileName));
        }
      }
    }
    //printf("end: si->label=%s si->fileName=%s\n",qPrint(si->label),qPrint(si->fileName));
  }
}



//----------------------------------------------------------------------------
// generate all separate documentation pages


static void generatePageDocs()
{
  //printf("documentedPages=%d real=%d\n",documentedPages,Doxygen::pageLinkedMap->count());
  if (Index::instance().numDocumentedPages()==0) return;
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (!pd->getGroupDef() && !pd->isReference())
    {
      msg("Generating docs for page %s...\n",qPrint(pd->name()));
      pd->writeDocumentation(*g_outputList);
    }
  }
}

//----------------------------------------------------------------------------
// create a (sorted) list & dictionary of example pages

static void buildExampleList(Entry *root)
{
  if ((root->section.isExample() || root->section.isExampleLineno()) && !root->name.isEmpty())
  {
    if (Doxygen::exampleLinkedMap->find(root->name))
    {
      warn(root->fileName,root->startLine,
          "Example %s was already documented. Ignoring "
          "documentation found here.",
          qPrint(root->name)
          );
    }
    else
    {
      PageDef *pd = Doxygen::exampleLinkedMap->add(root->name,
               createPageDef(root->fileName,root->startLine,
                 root->name,root->brief+root->doc+root->inbodyDocs,root->args));
      pd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      pd->setFileName(convertNameToFile(pd->name()+"-example",FALSE,TRUE));
      pd->addSectionsToDefinition(root->anchors);
      pd->setLanguage(root->lang);
      pd->setShowLineNo(root->section.isExampleLineno());

      //we don't add example to groups
      //addExampleToGroups(root,pd);
    }
  }
  for (const auto &e : root->children()) buildExampleList(e.get());
}

//----------------------------------------------------------------------------
// prints the Entry tree (for debugging)

void printNavTree(Entry *root,int indent)
{
  if (Debug::isFlagSet(Debug::Entries))
  {
    QCString indentStr;
    indentStr.fill(' ',indent);
    Debug::print(Debug::Entries,0,"%s%s at %s:%d (sec=%s, spec=%s)\n",
        indentStr.isEmpty()?"":qPrint(indentStr),
        root->name.isEmpty()?"<empty>":qPrint(root->name),
        qPrint(root->fileName),root->startLine,
        root->section.to_string().c_str(),
        root->spec.to_string().c_str());
    for (const auto &e : root->children())
    {
      printNavTree(e.get(),indent+2);
    }
  }
}

void printNavLayout(LayoutNavEntry *root,int indent)
{
  if (Debug::isFlagSet(Debug::Layout))
  {
    QCString indentStr;
    indentStr.fill(' ',indent);
    Debug::print(Debug::Layout,0,"%skind=%s visible=%d title='%s'\n",
        indentStr.isEmpty()?"":qPrint(indentStr),
        qPrint(root->navToString()),
        root->visible(),
        qPrint(root->title())
        );
    for (const auto &e : root->children())
    {
      printNavLayout(e.get(),indent+2);
    }
  }
}

void printLayout()
{
  bool extraIndent = false;
  Debug::print(Debug::Layout,0,"Part: Navigation index\n");
  for (const auto &e : LayoutDocManager::instance().rootNavEntry()->children())
  {
    printNavLayout(e.get(),2);
  }

  for (int i = 0; i < LayoutDocManager::NrParts; i++)
  {
     Debug::print(Debug::Layout,0,"\nPart: %s\n", qPrint(LayoutDocManager::partToString(i)));
     for (const auto &lde : LayoutDocManager::instance().docEntries(static_cast<LayoutDocManager::LayoutPart>(i)))
     {
       if (const LayoutDocEntrySimple *ldes = dynamic_cast<const LayoutDocEntrySimple*>(lde.get()))
       {
         if (lde->kind() == LayoutDocEntry::MemberDeclEnd || lde->kind() == LayoutDocEntry::MemberDefEnd) extraIndent = false;
         Debug::print(Debug::Layout,0,"  %skind: %s, visible=%d\n",
           extraIndent? "  " : "",qPrint(lde->entryToString()), ldes->visible());
         if (lde->kind() == LayoutDocEntry::MemberDeclStart || lde->kind() == LayoutDocEntry::MemberDefStart) extraIndent = true;
       }
       else if (const LayoutDocEntryMemberDecl *lmdecl = dynamic_cast<const LayoutDocEntryMemberDecl*>(lde.get()))
       {
         Debug::print(Debug::Layout,0,"  %scomplex kind: %s, type: %s\n",
           extraIndent? "  " : "",qPrint(lde->entryToString()),qPrint(lmdecl->type.to_string()));
       }
       else if (const LayoutDocEntryMemberDef *lmdef = dynamic_cast<const LayoutDocEntryMemberDef*>(lde.get()))
       {
         Debug::print(Debug::Layout,0,"  %scomplex kind: %s, type: %s\n",
           extraIndent? "  " : "",qPrint(lde->entryToString()),qPrint(lmdef->type.to_string()));
       }
       else
       {
         // should not happen
         Debug::print(Debug::Layout,0,"  %snot handled kind: %s\n",extraIndent? "  " : "",qPrint(lde->entryToString()));
       }
     }
  }
}

//----------------------------------------------------------------------------
// prints the Sections tree (for debugging)

void printSectionsTree()
{
  if (Debug::isFlagSet(Debug::Sections))
  {
    for (const auto &si : SectionManager::instance())
    {
      Debug::print(Debug::Sections,0,"Section = %s, file = %s, title = %s, type = %d, ref = %s\n",
            qPrint(si->label()),qPrint(si->fileName()),qPrint(si->title()),
            si->type(),qPrint(si->ref()));
    }
  }
}


//----------------------------------------------------------------------------
// generate the example documentation

static void generateExampleDocs()
{
  g_outputList->disable(OutputType::Man);
  for (const auto &pd : *Doxygen::exampleLinkedMap)
  {
    msg("Generating docs for example %s...\n",qPrint(pd->name()));
    SrcLangExt lang = getLanguageFromFileName(pd->name(), SrcLangExt::Unknown);
    if (lang != SrcLangExt::Unknown)
    {
      QCString ext = getFileNameExtension(pd->name());
      auto intf = Doxygen::parserManager->getCodeParser(ext);
      intf->resetCodeParserState();
    }
    QCString n=pd->getOutputFileBase();
    startFile(*g_outputList,n,n,pd->name());
    startTitle(*g_outputList,n);
    g_outputList->docify(pd->name());
    endTitle(*g_outputList,n,QCString());
    g_outputList->startContents();
    QCString lineNoOptStr;
    if (pd->showLineNo())
    {
      lineNoOptStr="{lineno}";
    }
    g_outputList->generateDoc(pd->docFile(),                       // file
                         pd->docLine(),                            // startLine
                         pd.get(),                                 // context
                         nullptr,                                  // memberDef
                         (pd->briefDescription().isEmpty()?"":pd->briefDescription()+"\n\n")+
                         pd->documentation()+"\n\n\\include"+lineNoOptStr+" "+pd->name(), // docs
                         TRUE,                                     // index words
                         TRUE,                                     // is example
                         pd->name(),
                         FALSE,
                         FALSE,
                         Config_getBool(MARKDOWN_SUPPORT)
                        );
    endFile(*g_outputList); // contains g_outputList->endContents()
  }
  g_outputList->enable(OutputType::Man);
}

//----------------------------------------------------------------------------
// generate module pages

static void generateGroupDocs()
{
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (!gd->isReference())
    {
      gd->writeDocumentation(*g_outputList);
    }
  }
}

//----------------------------------------------------------------------------
// generate module pages

static void generateNamespaceClassDocs(const ClassLinkedRefMap &classList)
{
  std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
  if (numThreads>1) // multi threaded processing
  {
    struct DocContext
    {
      DocContext(ClassDefMutable *cdm_,const OutputList &ol_)
        : cdm(cdm_), ol(ol_) {}
      ClassDefMutable *cdm;
      OutputList ol;
    };
    ThreadPool threadPool(numThreads);
    std::vector< std::future< std::shared_ptr<DocContext> > > results;
    // for each class in the namespace...
    for (const auto &cd : classList)
    {
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        auto ctx = std::make_shared<DocContext>(cdm,*g_outputList);
        auto processFile = [ctx]()
        {
          if ( ( ctx->cdm->isLinkableInProject() &&
                ctx->cdm->templateMaster()==nullptr
               ) // skip external references, anonymous compounds and
              // template instances and nested classes
              && !ctx->cdm->isHidden() && !ctx->cdm->isEmbeddedInOuterScope()
             )
          {
            msg("Generating docs for compound %s...\n",qPrint(ctx->cdm->displayName()));
            ctx->cdm->writeDocumentation(ctx->ol);
            ctx->cdm->writeMemberList(ctx->ol);
          }
          ctx->cdm->writeDocumentationForInnerClasses(ctx->ol);
          return ctx;
        };
        results.emplace_back(threadPool.queue(processFile));
      }
    }
    // wait for the results
    for (auto &f : results)
    {
      auto ctx = f.get();
    }
  }
  else // single threaded processing
  {
    // for each class in the namespace...
    for (const auto &cd : classList)
    {
      ClassDefMutable *cdm = toClassDefMutable(cd);
      if (cdm)
      {
        if ( ( cd->isLinkableInProject() &&
              cd->templateMaster()==nullptr
             ) // skip external references, anonymous compounds and
            // template instances and nested classes
            && !cd->isHidden() && !cd->isEmbeddedInOuterScope()
           )
        {
          msg("Generating docs for compound %s...\n",qPrint(cd->displayName()));

          cdm->writeDocumentation(*g_outputList);
          cdm->writeMemberList(*g_outputList);
        }
        cdm->writeDocumentationForInnerClasses(*g_outputList);
      }
    }
  }
}

static void generateNamespaceConceptDocs(const ConceptLinkedRefMap &conceptList)
{
  // for each concept in the namespace...
  for (const auto &cd : conceptList)
  {
    ConceptDefMutable *cdm = toConceptDefMutable(cd);
    if ( cdm && cd->isLinkableInProject() && !cd->isHidden())
    {
      msg("Generating docs for concept %s...\n",qPrint(cd->name()));
      cdm->writeDocumentation(*g_outputList);
    }
  }
}

static void generateNamespaceDocs()
{
  bool sliceOpt = Config_getBool(OPTIMIZE_OUTPUT_SLICE);

  //writeNamespaceIndex(*g_outputList);

  // for each namespace...
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    if (nd->isLinkableInProject())
    {
      NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
      if (ndm)
      {
        msg("Generating docs for namespace %s\n",qPrint(nd->displayName()));
        ndm->writeDocumentation(*g_outputList);
      }
    }

    generateNamespaceClassDocs(nd->getClasses());
    if (sliceOpt)
    {
      generateNamespaceClassDocs(nd->getInterfaces());
      generateNamespaceClassDocs(nd->getStructs());
      generateNamespaceClassDocs(nd->getExceptions());
    }
    generateNamespaceConceptDocs(nd->getConcepts());
  }
}

static void runHtmlHelpCompiler()
{
  std::string oldDir = Dir::currentDirPath();
  Dir::setCurrent(Config_getString(HTML_OUTPUT).str());
  Portable::setShortDir();
  if (Portable::system(Config_getString(HHC_LOCATION).data(), qPrint(HtmlHelp::hhpFileName), Debug::isFlagSet(Debug::ExtCmd))!=1)
  {
    err("failed to run html help compiler on %s\n", qPrint(HtmlHelp::hhpFileName));
  }
  Dir::setCurrent(oldDir);
}

static void runQHelpGenerator()
{
  QCString args = Qhp::qhpFileName + " -o \"" + Qhp::getQchFileName() + "\"";
  std::string oldDir = Dir::currentDirPath();
  Dir::setCurrent(Config_getString(HTML_OUTPUT).str());

  QCString qhgLocation=Config_getString(QHG_LOCATION);
  if (Debug::isFlagSet(Debug::Qhp)) // produce info for debugging
  {
    // run qhelpgenerator -v and extract the Qt version used
    QCString cmd=qhgLocation+ " -v 2>&1";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
    FILE *f=Portable::popen(cmd,"r");
    if (!f)
    {
      err("could not execute %s\n",qPrint(qhgLocation));
    }
    else
    {
      const size_t bufSize = 1024;
      char inBuf[bufSize+1];
      size_t numRead=fread(inBuf,1,bufSize,f);
      inBuf[numRead] = '\0';
      Debug::print(Debug::Qhp,0,inBuf);
      Portable::pclose(f);

      int qtVersion=0;
      static const reg::Ex versionReg(R"(Qt (\d+)\.(\d+)\.(\d+))");
      reg::Match match;
      std::string s = inBuf;
      if (reg::search(inBuf,match,versionReg))
      {
        qtVersion = 10000*QCString(match[1].str()).toInt() +
                      100*QCString(match[2].str()).toInt() +
                          QCString(match[3].str()).toInt();
      }
      if (qtVersion>0 && (qtVersion<60000 || qtVersion >= 60205))
      {
        // dump the output of qhelpgenerator -c file.qhp
        // Qt<6 or Qt>=6.2.5 or higher, see https://bugreports.qt.io/browse/QTBUG-101070
        cmd=qhgLocation+ " -c " + Qhp::qhpFileName + " 2>&1";
        Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
        f=Portable::popen(cmd,"r");
        if (!f)
        {
          err("could not execute %s\n",qPrint(qhgLocation));
        }
        else
        {
          std::string output;
          while ((numRead=fread(inBuf,1,bufSize,f))>0)
          {
            inBuf[numRead] = '\0';
            output += inBuf;
          }
          Portable::pclose(f);
          Debug::print(Debug::Qhp,0,output.c_str());
        }
      }
    }
  }

  if (Portable::system(qhgLocation, args, FALSE))
  {
    err("failed to run qhelpgenerator on %s\n",qPrint(Qhp::qhpFileName));
  }
  Dir::setCurrent(oldDir);
}

//----------------------------------------------------------------------------

static void computeVerifiedDotPath()
{
  // check dot path
  QCString dotPath = Config_getString(DOT_PATH);
  if (!dotPath.isEmpty())
  {
    FileInfo fi(dotPath.str());
    if (!(fi.exists() && fi.isFile()) )// not an existing user specified path + exec
    {
      dotPath = dotPath+"/dot"+Portable::commandExtension();
      FileInfo dp(dotPath.str());
      if (!dp.exists() || !dp.isFile())
      {
        warn_uncond("the dot tool could not be found as '%s'\n",qPrint(dotPath));
        dotPath = "dot";
        dotPath += Portable::commandExtension();
      }
    }
#if defined(_WIN32) // convert slashes
    size_t l=dotPath.length();
    for (size_t i=0;i<l;i++) if (dotPath.at(i)=='/') dotPath.at(i)='\\';
#endif
  }
  else
  {
    dotPath = "dot";
    dotPath += Portable::commandExtension();
  }
  Doxygen::verifiedDotPath = dotPath;
  TRACE("{}",Doxygen::verifiedDotPath);
}

//----------------------------------------------------------------------------

/*! Generate a template version of the configuration file.
 *  If the \a shortList parameter is TRUE a configuration file without
 *  comments will be generated.
 */
static void generateConfigFile(const QCString &configFile,bool shortList,
                               bool updateOnly=FALSE)
{
  std::ofstream f;
  bool fileOpened=openOutputFile(configFile,f);
  bool writeToStdout=configFile=="-";
  if (fileOpened)
  {
    TextStream t(&f);
    Config::writeTemplate(t,shortList,updateOnly);
    if (!writeToStdout)
    {
      if (!updateOnly)
      {
        msg("\n\nConfiguration file '%s' created.\n\n",qPrint(configFile));
        msg("Now edit the configuration file and enter\n\n");
        if (configFile!="Doxyfile" && configFile!="doxyfile")
          msg("  doxygen %s\n\n",qPrint(configFile));
        else
          msg("  doxygen\n\n");
        msg("to generate the documentation for your project\n\n");
      }
      else
      {
        msg("\n\nConfiguration file '%s' updated.\n\n",qPrint(configFile));
      }
    }
  }
  else
  {
    term("Cannot open file %s for writing\n",qPrint(configFile));
  }
}

static void compareDoxyfile(Config::CompareMode diffList)
{
  std::ofstream f;
  bool fileOpened=openOutputFile("-",f);
  if (fileOpened)
  {
    TextStream t(&f);
    Config::compareDoxyfile(t,diffList);
  }
  else
  {
    term("Cannot open stdout for writing\n");
  }
}

//----------------------------------------------------------------------------
// read and parse a tag file

static void readTagFile(const std::shared_ptr<Entry> &root,const QCString &tagLine)
{
  QCString fileName;
  QCString destName;
  int eqPos = tagLine.find('=');
  if (eqPos!=-1) // tag command contains a destination
  {
    fileName = tagLine.left(eqPos).stripWhiteSpace();
    destName = tagLine.right(tagLine.length()-eqPos-1).stripWhiteSpace();
    if (fileName.isEmpty() || destName.isEmpty()) return;
    //printf("insert tagDestination %s->%s\n",qPrint(fi.fileName()),qPrint(destName));
  }
  else
  {
    fileName = tagLine;
  }

  FileInfo fi(fileName.str());
  if (!fi.exists() || !fi.isFile())
  {
    err("Tag file '%s' does not exist or is not a file. Skipping it...\n",
        qPrint(fileName));
    return;
  }

  if (Doxygen::tagFileSet.find(fi.absFilePath().c_str()) != Doxygen::tagFileSet.end()) return;

  Doxygen::tagFileSet.emplace(fi.absFilePath());

  if (!destName.isEmpty())
  {
    Doxygen::tagDestinationMap.emplace(fi.absFilePath(), destName.str());
    msg("Reading tag file '%s', location '%s'...\n",qPrint(fileName),qPrint(destName));
  }
  else
    msg("Reading tag file '%s'...\n",qPrint(fileName));

  parseTagFile(root,fi.absFilePath().c_str());
}

//----------------------------------------------------------------------------
static void copyLatexStyleSheet()
{
  const StringVector &latexExtraStyleSheet = Config_getList(LATEX_EXTRA_STYLESHEET);
  for (const auto &sheet : latexExtraStyleSheet)
  {
    std::string fileName = sheet;
    if (!fileName.empty())
    {
      FileInfo fi(fileName);
      if (!fi.exists())
      {
        err("Style sheet '%s' specified by LATEX_EXTRA_STYLESHEET does not exist!\n",qPrint(fileName));
      }
      else if (fi.isDir())
      {
        err("Style sheet '%s' specified by LATEX_EXTRA_STYLESHEET is a directory, it has to be a file!\n", qPrint(fileName));
      }
      else
      {
        QCString destFileName = Config_getString(LATEX_OUTPUT)+"/"+fi.fileName();
        if (!checkExtension(fi.fileName().c_str(), LATEX_STYLE_EXTENSION))
        {
          destFileName += LATEX_STYLE_EXTENSION;
        }
        copyFile(QCString(fileName), destFileName);
      }
    }
  }
}

//----------------------------------------------------------------------------
static void copyStyleSheet()
{
  QCString htmlStyleSheet = Config_getString(HTML_STYLESHEET);
  if (!htmlStyleSheet.isEmpty())
  {
    if (!htmlStyleSheet.startsWith("http:") && !htmlStyleSheet.startsWith("https:"))
    {
      FileInfo fi(htmlStyleSheet.str());
      if (!fi.exists())
      {
        err("Style sheet '%s' specified by HTML_STYLESHEET does not exist!\n",qPrint(htmlStyleSheet));
        htmlStyleSheet = Config_updateString(HTML_STYLESHEET,""); // revert to the default
      }
      else if (fi.isDir())
      {
        err("Style sheet '%s' specified by HTML_STYLESHEET is a directory, it has to be a file!\n",qPrint(htmlStyleSheet));
        htmlStyleSheet = Config_updateString(HTML_STYLESHEET,""); // revert to the default
      }
      else
      {
        QCString destFileName = Config_getString(HTML_OUTPUT)+"/"+fi.fileName();
        copyFile(htmlStyleSheet,destFileName);
      }
    }
  }
  const StringVector &htmlExtraStyleSheet = Config_getList(HTML_EXTRA_STYLESHEET);
  for (const auto &sheet : htmlExtraStyleSheet)
  {
    QCString fileName(sheet);
    if (!fileName.isEmpty() && !fileName.startsWith("http:") && !fileName.startsWith("https:"))
    {
      FileInfo fi(fileName.str());
      if (!fi.exists())
      {
        err("Style sheet '%s' specified by HTML_EXTRA_STYLESHEET does not exist!\n",qPrint(fileName));
      }
      else if (fi.fileName()=="doxygen.css" || fi.fileName()=="tabs.css" || fi.fileName()=="navtree.css")
      {
        err("Style sheet %s specified by HTML_EXTRA_STYLESHEET is already a built-in stylesheet. Please use a different name\n",qPrint(fi.fileName()));
      }
      else if (fi.isDir())
      {
        err("Style sheet '%s' specified by HTML_EXTRA_STYLESHEET is a directory, it has to be a file!\n",qPrint(fileName));
      }
      else
      {
        QCString destFileName = Config_getString(HTML_OUTPUT)+"/"+fi.fileName();
        copyFile(QCString(fileName), destFileName);
      }
    }
  }
}

static void copyLogo(const QCString &outputOption)
{
  QCString projectLogo = projectLogoFile();
  if (!projectLogo.isEmpty())
  {
    FileInfo fi(projectLogo.str());
    if (!fi.exists())
    {
      err("Project logo '%s' specified by PROJECT_LOGO does not exist!\n",qPrint(projectLogo));
      projectLogo = Config_updateString(PROJECT_LOGO,""); // revert to the default
    }
    else if (fi.isDir())
    {
      err("Project logo '%s' specified by PROJECT_LOGO is a directory, it has to be a file!\n",qPrint(projectLogo));
      projectLogo = Config_updateString(PROJECT_LOGO,""); // revert to the default
    }
    else
    {
      QCString destFileName = outputOption+"/"+fi.fileName();
      copyFile(projectLogo,destFileName);
      Doxygen::indexList->addImageFile(fi.fileName().c_str());
    }
  }
}

static void copyIcon(const QCString &outputOption)
{
  QCString projectIcon = Config_getString(PROJECT_ICON);
  if (!projectIcon.isEmpty())
  {
    FileInfo fi(projectIcon.str());
    if (!fi.exists())
    {
      err("Project icon '%s' specified by PROJECT_ICON does not exist!\n",qPrint(projectIcon));
      projectIcon = Config_updateString(PROJECT_ICON,""); // revert to the default
    }
    else if (fi.isDir())
    {
      err("Project icon '%s' specified by PROJECT_ICON is a directory, it has to be a file!\n",qPrint(projectIcon));
      projectIcon = Config_updateString(PROJECT_ICON,""); // revert to the default
    }
    else
    {
      QCString destFileName = outputOption+"/"+fi.fileName();
      copyFile(projectIcon,destFileName);
      Doxygen::indexList->addImageFile(fi.fileName().c_str());
    }
  }
}

static void copyExtraFiles(const StringVector &files,const QCString &filesOption,const QCString &outputOption)
{
  for (const auto &fileName : files)
  {
    if (!fileName.empty())
    {
      FileInfo fi(fileName);
      if (!fi.exists())
      {
        err("Extra file '%s' specified in %s does not exist!\n", fileName.c_str(),qPrint(filesOption));
      }
      else if (fi.isDir())
      {
        err("Extra file '%s' specified in %s is a directory, it has to be a file!\n", fileName.c_str(),qPrint(filesOption));
      }
      else
      {
        QCString destFileName = outputOption+"/"+fi.fileName();
        Doxygen::indexList->addImageFile(fi.fileName().c_str());
        copyFile(QCString(fileName), destFileName);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void generateDiskNames()
{
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    struct FileEntry
    {
      FileEntry(const QCString &p,FileDef *fd) : path(p), fileDef(fd) {}
      QCString path;
      FileDef *fileDef;
    };

    // collect the entry for which to compute the longest common prefix (LCP) of the path
    std::vector<FileEntry> fileEntries;
    for (const auto &fd : *fn)
    {
      if (!fd->isReference()) // skip external references
      {
        fileEntries.emplace_back(fd->getPath(),fd.get());
      }
    }

    size_t size = fileEntries.size();

    if (size==1) // name if unique, so diskname is simply the name
    {
      FileDef *fd = fileEntries[0].fileDef;
      fd->setDiskName(fn->fileName());
    }
    else if (size>1) // multiple occurrences of the same file name
    {
      // sort the array
      std::stable_sort(fileEntries.begin(),
                fileEntries.end(),
                [](const FileEntry &fe1,const FileEntry &fe2)
                { return qstricmp_sort(fe1.path,fe2.path)<0; }
               );

      // since the entries are sorted, the common prefix of the whole array is same
      // as the common prefix between the first and last entry
      const FileEntry &first = fileEntries[0];
      const FileEntry &last =  fileEntries[size-1];
      int first_path_size = static_cast<int>(first.path.size())-1; // -1 to skip trailing slash
      int last_path_size  = static_cast<int>(last.path.size())-1;  // -1 to skip trailing slash
      int j=0;
      int i=0;
      for (i=0;i<first_path_size && i<last_path_size;i++)
      {
        if (first.path[i]=='/') j=i;
        if (first.path[i]!=last.path[i]) break;
      }
      if (i==first_path_size && i<last_path_size && last.path[i]=='/')
      {
        // case first='some/path' and last='some/path/more' => match is 'some/path'
        j=first_path_size;
      }
      else if (i==last_path_size && i<first_path_size && first.path[i]=='/')
      {
        // case first='some/path/more' and last='some/path' => match is 'some/path'
        j=last_path_size;
      }

      // add non-common part of the path to the name
      for (auto &fileEntry : fileEntries)
      {
         QCString prefix = fileEntry.path.right(fileEntry.path.length()-j-1);
         fileEntry.fileDef->setName(prefix+fn->fileName());
         //printf("!!!!!!!! non unique disk name=%s:%s\n",qPrint(prefix),fn->fileName());
         fileEntry.fileDef->setDiskName(prefix+fn->fileName());
      }
    }
  }
}



//----------------------------------------------------------------------------

static std::unique_ptr<OutlineParserInterface> getParserForFile(const QCString &fn)
{
  QCString fileName=fn;
  QCString extension;
  int sep = fileName.findRev('/');
  int ei = fileName.findRev('.');
  if (ei!=-1 && (sep==-1 || ei>sep)) // matches dir/file.ext but not dir.1/file
  {
    extension=fileName.right(fileName.length()-ei);
  }
  else
  {
    extension = ".no_extension";
  }

  return Doxygen::parserManager->getOutlineParser(extension);
}

static std::shared_ptr<Entry> parseFile(OutlineParserInterface &parser,
                      FileDef *fd,const QCString &fn,
                      ClangTUParser *clangParser,bool newTU)
{
  QCString fileName=fn;
  AUTO_TRACE("fileName={}",fileName);
  QCString extension;
  int ei = fileName.findRev('.');
  if (ei!=-1)
  {
    extension=fileName.right(fileName.length()-ei);
  }
  else
  {
    extension = ".no_extension";
  }

  FileInfo fi(fileName.str());
  std::string preBuf;

  if (Config_getBool(ENABLE_PREPROCESSING) &&
      parser.needsPreprocessing(extension))
  {
    Preprocessor preprocessor;
    const StringVector &includePath = Config_getList(INCLUDE_PATH);
    for (const auto &s : includePath)
    {
      std::string absPath = FileInfo(s).absFilePath();
      preprocessor.addSearchDir(absPath.c_str());
    }
    std::string inBuf;
    msg("Preprocessing %s...\n",qPrint(fn));
    readInputFile(fileName,inBuf);
    addTerminalCharIfMissing(inBuf,'\n');
    preprocessor.processFile(fileName,inBuf,preBuf);
  }
  else // no preprocessing
  {
    msg("Reading %s...\n",qPrint(fn));
    readInputFile(fileName,preBuf);
    addTerminalCharIfMissing(preBuf,'\n');
  }

  std::string convBuf;
  convBuf.reserve(preBuf.size()+1024);

  // convert multi-line C++ comments to C style comments
  convertCppComments(preBuf,convBuf,fileName.str());

  std::shared_ptr<Entry> fileRoot = std::make_shared<Entry>();
  // use language parse to parse the file
  if (clangParser)
  {
    if (newTU) clangParser->parse();
    clangParser->switchToFile(fd);
  }
  parser.parseInput(fileName,convBuf.data(),fileRoot,clangParser);
  fileRoot->setFileDef(fd);
  return fileRoot;
}

//! parse the list of input files
static void parseFilesMultiThreading(const std::shared_ptr<Entry> &root)
{
  AUTO_TRACE();
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing)
  {
    StringUnorderedSet processedFiles;

    // create a dictionary with files to process
    StringUnorderedSet filesToProcess;
    for (const auto &s : g_inputFiles)
    {
      filesToProcess.insert(s);
    }

    std::mutex processedFilesLock;
    // process source files (and their include dependencies)
    std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
    msg("Processing input using %zu threads.\n",numThreads);
    ThreadPool threadPool(numThreads);
    using FutureType = std::vector< std::shared_ptr<Entry> >;
    std::vector< std::future< FutureType > > results;
    for (const auto &s : g_inputFiles)
    {
      bool ambig = false;
      FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
      ASSERT(fd!=nullptr);
      if (fd->isSource() && !fd->isReference() && fd->getLanguage()==SrcLangExt::Cpp) // this is a source file
      {
        // lambda representing the work to executed by a thread
        auto processFile = [s,&filesToProcess,&processedFilesLock,&processedFiles]() {
          bool ambig_l = false;
          std::vector< std::shared_ptr<Entry> > roots;
          FileDef *fd_l = findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig_l);
          auto clangParser = ClangParser::instance()->createTUParser(fd_l);
          auto parser = getParserForFile(s.c_str());
          auto fileRoot { parseFile(*parser.get(),fd_l,s.c_str(),clangParser.get(),true) };
          roots.push_back(fileRoot);

          // Now process any include files in the same translation unit
          // first. When libclang is used this is much more efficient.
          for (auto incFile : clangParser->filesInSameTU())
          {
            if (filesToProcess.find(incFile)!=filesToProcess.end())
            {
              bool needsToBeProcessed = false;
              {
                std::lock_guard<std::mutex> lock(processedFilesLock);
                needsToBeProcessed = processedFiles.find(incFile)==processedFiles.end();
                if (needsToBeProcessed) processedFiles.insert(incFile);
              }
              if (incFile!=s && needsToBeProcessed)
              {
                FileDef *ifd=findFileDef(Doxygen::inputNameLinkedMap,incFile.c_str(),ambig_l);
                if (ifd && !ifd->isReference())
                {
                  //printf("  Processing %s in same translation unit as %s\n",incFile,s->c_str());
                  fileRoot = parseFile(*parser.get(),ifd,incFile.c_str(),clangParser.get(),false);
                  roots.push_back(fileRoot);
                }
              }
            }
          }
          return roots;
        };
        // dispatch the work and collect the future results
        results.emplace_back(threadPool.queue(processFile));
      }
    }
    // synchronize with the Entry result lists produced and add them to the root
    for (auto &f : results)
    {
      auto l = f.get();
      for (auto &e : l)
      {
        root->moveToSubEntryAndKeep(e);
      }
    }
    // process remaining files
    results.clear();
    for (const auto &s : g_inputFiles)
    {
      if (processedFiles.find(s)==processedFiles.end()) // not yet processed
      {
        // lambda representing the work to executed by a thread
        auto processFile = [s]() {
          bool ambig = false;
          std::vector< std::shared_ptr<Entry> > roots;
          FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
          auto parser { getParserForFile(s.c_str()) };
          bool useClang = getLanguageFromFileName(s.c_str())==SrcLangExt::Cpp;
          if (useClang)
          {
            auto clangParser = ClangParser::instance()->createTUParser(fd);
            auto fileRoot = parseFile(*parser.get(),fd,s.c_str(),clangParser.get(),true);
            roots.push_back(fileRoot);
          }
          else
          {
            auto fileRoot = parseFile(*parser.get(),fd,s.c_str(),nullptr,true);
            roots.push_back(fileRoot);
          }
          return roots;
        };
        results.emplace_back(threadPool.queue(processFile));
      }
    }
    // synchronize with the Entry result lists produced and add them to the root
    for (auto &f : results)
    {
      auto l = f.get();
      for (auto &e : l)
      {
        root->moveToSubEntryAndKeep(e);
      }
    }
  }
  else // normal processing
#endif
  {
    std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
    msg("Processing input using %zu threads.\n",numThreads);
    ThreadPool threadPool(numThreads);
    using FutureType = std::shared_ptr<Entry>;
    std::vector< std::future< FutureType > > results;
    for (const auto &s : g_inputFiles)
    {
      // lambda representing the work to executed by a thread
      auto processFile = [s]() {
        bool ambig = false;
        FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
        auto parser = getParserForFile(s.c_str());
        auto fileRoot = parseFile(*parser.get(),fd,s.c_str(),nullptr,true);
        return fileRoot;
      };
      // dispatch the work and collect the future results
      results.emplace_back(threadPool.queue(processFile));
    }
    // synchronize with the Entry results produced and add them to the root
    for (auto &f : results)
    {
      root->moveToSubEntryAndKeep(f.get());
    }
  }
}

//! parse the list of input files
static void parseFilesSingleThreading(const std::shared_ptr<Entry> &root)
{
  AUTO_TRACE();
#if USE_LIBCLANG
  if (Doxygen::clangAssistedParsing)
  {
    StringUnorderedSet processedFiles;

    // create a dictionary with files to process
    StringUnorderedSet filesToProcess;
    for (const auto &s : g_inputFiles)
    {
      filesToProcess.insert(s);
    }

    // process source files (and their include dependencies)
    for (const auto &s : g_inputFiles)
    {
      bool ambig = false;
      FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
      ASSERT(fd!=nullptr);
      if (fd->isSource() && !fd->isReference() && getLanguageFromFileName(s.c_str())==SrcLangExt::Cpp) // this is a source file
      {
        auto clangParser = ClangParser::instance()->createTUParser(fd);
        auto parser { getParserForFile(s.c_str()) };
        auto fileRoot = parseFile(*parser.get(),fd,s.c_str(),clangParser.get(),true);
        root->moveToSubEntryAndKeep(fileRoot);
        processedFiles.insert(s);

        // Now process any include files in the same translation unit
        // first. When libclang is used this is much more efficient.
        for (auto incFile : clangParser->filesInSameTU())
        {
          //printf("    file %s\n",incFile.c_str());
          if (filesToProcess.find(incFile)!=filesToProcess.end() && // file need to be processed
              processedFiles.find(incFile)==processedFiles.end())   // and is not processed already
          {
            FileDef *ifd=findFileDef(Doxygen::inputNameLinkedMap,incFile.c_str(),ambig);
            if (ifd && !ifd->isReference())
            {
              //printf("  Processing %s in same translation unit as %s\n",incFile.c_str(),s.c_str());
              fileRoot = parseFile(*parser.get(),ifd,incFile.c_str(),clangParser.get(),false);
              root->moveToSubEntryAndKeep(fileRoot);
              processedFiles.insert(incFile);
            }
          }
        }
      }
    }
    // process remaining files
    for (const auto &s : g_inputFiles)
    {
      if (processedFiles.find(s)==processedFiles.end()) // not yet processed
      {
        bool ambig = false;
        FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
        if (getLanguageFromFileName(s.c_str())==SrcLangExt::Cpp) // not yet processed
        {
          auto clangParser = ClangParser::instance()->createTUParser(fd);
          auto parser { getParserForFile(s.c_str()) };
          auto fileRoot = parseFile(*parser.get(),fd,s.c_str(),clangParser.get(),true);
          root->moveToSubEntryAndKeep(fileRoot);
        }
        else
        {
          std::unique_ptr<OutlineParserInterface> parser { getParserForFile(s.c_str()) };
          std::shared_ptr<Entry> fileRoot = parseFile(*parser.get(),fd,s.c_str(),nullptr,true);
          root->moveToSubEntryAndKeep(fileRoot);
        }
        processedFiles.insert(s);
      }
    }
  }
  else // normal processing
#endif
  {
    for (const auto &s : g_inputFiles)
    {
      bool ambig = false;
      FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,s.c_str(),ambig);
      ASSERT(fd!=nullptr);
      std::unique_ptr<OutlineParserInterface> parser { getParserForFile(s.c_str()) };
      std::shared_ptr<Entry> fileRoot = parseFile(*parser.get(),fd,s.c_str(),nullptr,true);
      root->moveToSubEntryAndKeep(std::move(fileRoot));
    }
  }
}

// resolves a path that may include symlinks, if a recursive symlink is
// found an empty string is returned.
static std::string resolveSymlink(const std::string &path)
{
  int sepPos=0;
  int oldPos=0;
  StringUnorderedSet nonSymlinks;
  StringUnorderedSet known;
  QCString result(path);
  QCString oldPrefix = "/";
  do
  {
#if defined(_WIN32)
    // UNC path, skip server and share name
    if (sepPos==0 && (result.startsWith("//") || result.startsWith("\\\\")))
      sepPos = result.find('/',2);
    if (sepPos!=-1)
      sepPos = result.find('/',sepPos+1);
#else
    sepPos = result.find('/',sepPos+1);
#endif
    QCString prefix = sepPos==-1 ? result : result.left(sepPos);
    if (nonSymlinks.find(prefix.str())==nonSymlinks.end())
    {
      FileInfo fi(prefix.str());
      if (fi.isSymLink())
      {
        QCString target = fi.readLink();
        bool isRelative = FileInfo(target.str()).isRelative();
        if (isRelative)
        {
          target = Dir::cleanDirPath(oldPrefix.str()+"/"+target.str());
        }
        if (sepPos!=-1)
        {
          if (fi.isDir() && target.length()>0 && target.at(target.length()-1)!='/')
          {
            target+='/';
          }
          target+=result.mid(sepPos);
        }
        result = Dir::cleanDirPath(target.str());
        if (known.find(result.str())!=known.end()) return std::string(); // recursive symlink!
        known.insert(result.str());
        if (isRelative)
        {
          sepPos = oldPos;
        }
        else // link to absolute path
        {
          sepPos = 0;
          oldPrefix = "/";
        }
      }
      else
      {
        nonSymlinks.insert(prefix.str());
        oldPrefix = prefix;
      }
      oldPos = sepPos;
    }
  }
  while (sepPos!=-1);
  return Dir::cleanDirPath(result.str());
}

static StringUnorderedSet g_pathsVisited(1009);

//----------------------------------------------------------------------------
// Read all files matching at least one pattern in 'patList' in the
// directory represented by 'fi'.
// The directory is read iff the recursiveFlag is set.
// The contents of all files is append to the input string

static void readDir(FileInfo *fi,
            FileNameLinkedMap *fnMap,
            StringUnorderedSet *exclSet,
            const StringVector *patList,
            const StringVector *exclPatList,
            StringVector *resultList,
            StringUnorderedSet *resultSet,
            bool errorIfNotExist,
            bool recursive,
            StringUnorderedSet *killSet,
            StringUnorderedSet *paths
           )
{
  std::string dirName = fi->absFilePath();
  if (paths && !dirName.empty())
  {
    paths->insert(dirName);
  }
  //printf("%s isSymLink()=%d\n",qPrint(dirName),fi->isSymLink());
  if (fi->isSymLink())
  {
    dirName = resolveSymlink(dirName);
    if (dirName.empty())
    {
      //printf("RECURSIVE SYMLINK: %s\n",qPrint(dirName));
      return;  // recursive symlink
    }
  }

  if (g_pathsVisited.find(dirName)!=g_pathsVisited.end())
  {
    //printf("PATH ALREADY VISITED: %s\n",qPrint(dirName));
    return; // already visited path
  }
  g_pathsVisited.insert(dirName);

  Dir dir(dirName);
  msg("Searching for files in directory %s\n", qPrint(fi->absFilePath()));
  //printf("killSet=%p count=%d\n",killSet,killSet ? (int)killSet->count() : -1);

  StringVector dirResultList;

  for (const auto &dirEntry : dir.iterator())
  {
    FileInfo cfi(dirEntry.path());
    if (exclSet==nullptr || exclSet->find(cfi.absFilePath())==exclSet->end())
    { // file should not be excluded
      //printf("killSet->find(%s)\n",qPrint(cfi->absFilePath()));
      if (Config_getBool(EXCLUDE_SYMLINKS) && cfi.isSymLink())
      {
      }
      else if (!cfi.exists() || !cfi.isReadable())
      {
        if (errorIfNotExist)
        {
          warn_uncond("source '%s' is not a readable file or directory... skipping.\n",cfi.absFilePath().c_str());
        }
      }
      else if (cfi.isFile() &&
          (patList==nullptr || patternMatch(cfi,*patList)) &&
          (exclPatList==nullptr || !patternMatch(cfi,*exclPatList)) &&
          (killSet==nullptr || killSet->find(cfi.absFilePath())==killSet->end())
          )
      {
        std::string name=cfi.fileName();
        std::string path=cfi.dirPath()+"/";
        std::string fullName=path+name;
        if (fnMap)
        {
          auto fd = createFileDef(QCString(path),QCString(name));
          FileName *fn=nullptr;
          if (!name.empty())
          {
            fn = fnMap->add(QCString(name),QCString(fullName));
            fn->push_back(std::move(fd));
          }
        }
        dirResultList.push_back(fullName);
        if (resultSet) resultSet->insert(fullName);
        if (killSet) killSet->insert(fullName);
      }
      else if (recursive &&
          cfi.isDir() &&
          (exclPatList==nullptr || !patternMatch(cfi,*exclPatList)) &&
          cfi.fileName().at(0)!='.') // skip "." ".." and ".dir"
      {
        FileInfo acfi(cfi.absFilePath());
        readDir(&acfi,fnMap,exclSet,
            patList,exclPatList,&dirResultList,resultSet,errorIfNotExist,
            recursive,killSet,paths);
      }
    }
  }
  if (resultList && !dirResultList.empty())
  {
    // sort the resulting list to make the order platform independent.
    std::stable_sort(dirResultList.begin(),
              dirResultList.end(),
              [](const auto &f1,const auto &f2) { return qstricmp_sort(f1.c_str(),f2.c_str())<0; });

    // append the sorted results to resultList
    resultList->insert(resultList->end(), dirResultList.begin(), dirResultList.end());
  }
}


//----------------------------------------------------------------------------
// read a file or all files in a directory and append their contents to the
// input string. The names of the files are appended to the 'fiList' list.

void readFileOrDirectory(const QCString &s,
                        FileNameLinkedMap *fnMap,
                        StringUnorderedSet *exclSet,
                        const StringVector *patList,
                        const StringVector *exclPatList,
                        StringVector *resultList,
                        StringUnorderedSet *resultSet,
                        bool recursive,
                        bool errorIfNotExist,
                        StringUnorderedSet *killSet,
                        StringUnorderedSet *paths
                       )
{
  //printf("killSet count=%d\n",killSet ? (int)killSet->size() : -1);
  // strip trailing slashes
  if (s.isEmpty()) return;

  g_pathsVisited.clear();

  FileInfo fi(s.str());
  //printf("readFileOrDirectory(%s)\n",s);
  {
    if (exclSet==nullptr || exclSet->find(fi.absFilePath())==exclSet->end())
    {
      if (Config_getBool(EXCLUDE_SYMLINKS) && fi.isSymLink())
      {
      }
      else if (!fi.exists() || !fi.isReadable())
      {
        if (errorIfNotExist)
        {
          warn_uncond("source '%s' is not a readable file or directory... skipping.\n",qPrint(s));
        }
      }
      else if (fi.isFile())
      {
        std::string dirPath = fi.dirPath(true);
        std::string filePath = fi.absFilePath();
        if (paths && !dirPath.empty())
        {
          paths->insert(dirPath);
        }
        //printf("killSet.find(%s)=%d\n",qPrint(fi.absFilePath()),killSet.find(fi.absFilePath())!=killSet.end());
        if (killSet==nullptr || killSet->find(filePath)==killSet->end())
        {
          std::string name=fi.fileName();
          if (fnMap)
          {
            auto fd = createFileDef(QCString(dirPath+"/"),QCString(name));
            if (!name.empty())
            {
              FileName *fn = fnMap->add(QCString(name),QCString(filePath));
              fn->push_back(std::move(fd));
            }
          }
          if (resultList || resultSet)
          {
            if (resultList) resultList->push_back(filePath);
            if (resultSet) resultSet->insert(filePath);
          }

          if (killSet) killSet->insert(fi.absFilePath());
        }
      }
      else if (fi.isDir()) // readable dir
      {
        readDir(&fi,fnMap,exclSet,patList,
            exclPatList,resultList,resultSet,errorIfNotExist,
            recursive,killSet,paths);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void dumpSymbol(TextStream &t,Definition *d)
{
  QCString anchor;
  if (d->definitionType()==Definition::TypeMember)
  {
    MemberDef *md = toMemberDef(d);
    anchor=":"+md->anchor();
  }
  QCString scope;
  QCString fn = d->getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  if (d->getOuterScope() && d->getOuterScope()!=Doxygen::globalScope)
  {
    scope = fn;
  }
  t << "REPLACE INTO symbols (symbol_id,scope_id,name,file,line) VALUES('"
    << fn+anchor << "','"
    << scope << "','"
    << d->name() << "','"
    << d->getDefFileName() << "','"
    << d->getDefLine()
    << "');\n";
}

static void dumpSymbolMap()
{
  std::ofstream f = Portable::openOutputStream("symbols.sql");
  if (f.is_open())
  {
    TextStream t(&f);
    for (const auto &[name,symList] : *Doxygen::symbolMap)
    {
      for (const auto &def : symList)
      {
        dumpSymbol(t,def);
      }
    }
  }
}

// print developer options of Doxygen
static void devUsage()
{
  msg("Developer parameters:\n");
  msg("  -m          dump symbol map\n");
  msg("  -b          making messages output unbuffered\n");
#if ENABLE_TRACING
  msg("  -t        [<file|stdout|stderr>] trace debug info to file, stdout, or stderr (default file stdout)\n");
  msg("  -t_time   [<file|stdout|stderr>] trace debug info to file, stdout, or stderr (default file stdout),\n"
      "                                   and include time and thread information\n");
#endif
  msg("  -d <level>  enable a debug level, such as (multiple invocations of -d are possible):\n");
  Debug::printFlags();
}


//----------------------------------------------------------------------------
// print the version of Doxygen

static void version(const bool extended)
{
  QCString versionString = getFullVersion();
  msg("%s\n",qPrint(versionString));
  if (extended)
  {
    QCString extVers;
    if (!extVers.isEmpty()) extVers+= ", ";
    extVers += "sqlite3 ";
    extVers += sqlite3_libversion();
#if USE_LIBCLANG
    if (!extVers.isEmpty()) extVers+= ", ";
    extVers += "clang support ";
    extVers += CLANG_VERSION_STRING;
#endif
    if (!extVers.isEmpty())
    {
      int lastComma = extVers.findRev(',');
      if (lastComma != -1) extVers = extVers.replace(lastComma,1," and");
      msg("    with %s.\n",qPrint(extVers));
    }
  }
}

//----------------------------------------------------------------------------
// print the usage of Doxygen

static void usage(const QCString &name,const QCString &versionString)
{
  msg("Doxygen version %s\nCopyright Dimitri van Heesch 1997-2024\n\n",qPrint(versionString));
  msg("You can use Doxygen in a number of ways:\n\n");
  msg("1) Use Doxygen to generate a template configuration file*:\n");
  msg("    %s [-s] -g [configName]\n\n",qPrint(name));
  msg("2) Use Doxygen to update an old configuration file*:\n");
  msg("    %s [-s] -u [configName]\n\n",qPrint(name));
  msg("3) Use Doxygen to generate documentation using an existing ");
  msg("configuration file*:\n");
  msg("    %s [configName]\n\n",qPrint(name));
  msg("4) Use Doxygen to generate a template file controlling the layout of the\n");
  msg("   generated documentation:\n");
  msg("    %s -l [layoutFileName]\n\n",qPrint(name));
  msg("    In case layoutFileName is omitted DoxygenLayout.xml will be used as filename.\n");
  msg("    If - is used for layoutFileName Doxygen will write to standard output.\n\n");
  msg("5) Use Doxygen to generate a template style sheet file for RTF, HTML or Latex.\n");
  msg("    RTF:        %s -w rtf styleSheetFile\n",qPrint(name));
  msg("    HTML:       %s -w html headerFile footerFile styleSheetFile [configFile]\n",qPrint(name));
  msg("    LaTeX:      %s -w latex headerFile footerFile styleSheetFile [configFile]\n\n",qPrint(name));
  msg("6) Use Doxygen to generate a rtf extensions file\n");
  msg("    %s -e rtf extensionsFile\n\n",qPrint(name));
  msg("    If - is used for extensionsFile Doxygen will write to standard output.\n\n");
  msg("7) Use Doxygen to compare the used configuration file with the template configuration file\n");
  msg("    %s -x [configFile]\n\n",qPrint(name));
  msg("   Use Doxygen to compare the used configuration file with the template configuration file\n");
  msg("   without replacing the environment variables or CMake type replacement variables\n");
  msg("    %s -x_noenv [configFile]\n\n",qPrint(name));
  msg("8) Use Doxygen to show a list of built-in emojis.\n");
  msg("    %s -f emoji outputFileName\n\n",qPrint(name));
  msg("    If - is used for outputFileName Doxygen will write to standard output.\n\n");
  msg("*) If -s is specified the comments of the configuration items in the config file will be omitted.\n");
  msg("   If configName is omitted 'Doxyfile' will be used as a default.\n");
  msg("   If - is used for configFile Doxygen will write / read the configuration to /from standard output / input.\n\n");
  msg("If -q is used for a Doxygen documentation run, Doxygen will see this as if QUIET=YES has been set.\n\n");
  msg("-v print version string, -V print extended version information\n");
  msg("-h,-? prints usage help information\n");
  msg("%s -d prints additional usage flags for debugging purposes\n",qPrint(name));
}

//----------------------------------------------------------------------------
// read the argument of option 'c' from the comment argument list and
// update the option index 'optInd'.

static const char *getArg(int argc,char **argv,int &optInd)
{
  char *s=nullptr;
  if (qstrlen(&argv[optInd][2])>0)
    s=&argv[optInd][2];
  else if (optInd+1<argc && argv[optInd+1][0]!='-')
    s=argv[++optInd];
  return s;
}

//----------------------------------------------------------------------------

/** @brief /dev/null outline parser */
class NullOutlineParser : public OutlineParserInterface
{
  public:
    void parseInput(const QCString &/* file */, const char * /* buf */,const std::shared_ptr<Entry> &, ClangTUParser*) override {}
    bool needsPreprocessing(const QCString &) const override { return FALSE; }
    void parsePrototype(const QCString &) override {}
};


template<class T> std::function< std::unique_ptr<T>() > make_parser_factory()
{
  return []() { return std::make_unique<T>(); };
}

void initDoxygen()
{
  initResources();
  QCString lang = Portable::getenv("LC_ALL");
  if (!lang.isEmpty()) Portable::setenv("LANG",lang);
  std::setlocale(LC_ALL,"");
  std::setlocale(LC_CTYPE,"C"); // to get isspace(0xA0)==0, needed for UTF-8
  std::setlocale(LC_NUMERIC,"C");

  Doxygen::symbolMap = new SymbolMap<Definition>;

  Debug::startTimer();
  Doxygen::parserManager = new ParserManager(            make_parser_factory<NullOutlineParser>(),
                                                         make_parser_factory<FileCodeParser>());
  Doxygen::parserManager->registerParser("c",            make_parser_factory<COutlineParser>(),
                                                         make_parser_factory<CCodeParser>());
  Doxygen::parserManager->registerParser("python",       make_parser_factory<PythonOutlineParser>(),
                                                         make_parser_factory<PythonCodeParser>());
  Doxygen::parserManager->registerParser("fortran",      make_parser_factory<FortranOutlineParser>(),
                                                         make_parser_factory<FortranCodeParser>());
  Doxygen::parserManager->registerParser("fortranfree",  make_parser_factory<FortranOutlineParserFree>(),
                                                         make_parser_factory<FortranCodeParserFree>());
  Doxygen::parserManager->registerParser("fortranfixed", make_parser_factory<FortranOutlineParserFixed>(),
                                                         make_parser_factory<FortranCodeParserFixed>());
  Doxygen::parserManager->registerParser("vhdl",         make_parser_factory<VHDLOutlineParser>(),
                                                         make_parser_factory<VHDLCodeParser>());
  Doxygen::parserManager->registerParser("xml",          make_parser_factory<NullOutlineParser>(),
                                                         make_parser_factory<XMLCodeParser>());
  Doxygen::parserManager->registerParser("sql",          make_parser_factory<NullOutlineParser>(),
                                                         make_parser_factory<SQLCodeParser>());
  Doxygen::parserManager->registerParser("md",           make_parser_factory<MarkdownOutlineParser>(),
                                                         make_parser_factory<FileCodeParser>());
  Doxygen::parserManager->registerParser("lex",          make_parser_factory<LexOutlineParser>(),
                                                         make_parser_factory<LexCodeParser>());

  // register any additional parsers here...

  initDefaultExtensionMapping();

#if USE_LIBCLANG
  Doxygen::clangUsrMap   = new ClangUsrMap;
#endif
  Doxygen::memberNameLinkedMap = new MemberNameLinkedMap;
  Doxygen::functionNameLinkedMap = new MemberNameLinkedMap;
  Doxygen::groupLinkedMap = new GroupLinkedMap;
  Doxygen::namespaceLinkedMap = new NamespaceLinkedMap;
  Doxygen::classLinkedMap = new ClassLinkedMap;
  Doxygen::hiddenClassLinkedMap = new ClassLinkedMap;
  Doxygen::conceptLinkedMap = new ConceptLinkedMap;
  Doxygen::dirLinkedMap = new DirLinkedMap;
  Doxygen::pageLinkedMap = new PageLinkedMap;          // all doc pages
  Doxygen::exampleLinkedMap = new PageLinkedMap;       // all examples
  //Doxygen::tagDestinationDict.setAutoDelete(TRUE);
  Doxygen::indexList = new IndexList;

  // initialization of these globals depends on
  // configuration switches so we need to postpone these
  Doxygen::globalScope     = nullptr;
  Doxygen::inputNameLinkedMap   = nullptr;
  Doxygen::includeNameLinkedMap = nullptr;
  Doxygen::exampleNameLinkedMap = nullptr;
  Doxygen::imageNameLinkedMap   = nullptr;
  Doxygen::dotFileNameLinkedMap = nullptr;
  Doxygen::mscFileNameLinkedMap = nullptr;
  Doxygen::diaFileNameLinkedMap = nullptr;
  Doxygen::plantUmlFileNameLinkedMap = nullptr;

}

void cleanUpDoxygen()
{
  FormulaManager::instance().clear();
  SectionManager::instance().clear();
  ModuleManager::instance().clear();

  delete Doxygen::indexList;
  delete Doxygen::inputNameLinkedMap;
  delete Doxygen::includeNameLinkedMap;
  delete Doxygen::exampleNameLinkedMap;
  delete Doxygen::imageNameLinkedMap;
  delete Doxygen::dotFileNameLinkedMap;
  delete Doxygen::mscFileNameLinkedMap;
  delete Doxygen::diaFileNameLinkedMap;
  delete Doxygen::plantUmlFileNameLinkedMap;
  Doxygen::mainPage.reset();
  delete Doxygen::pageLinkedMap;
  delete Doxygen::exampleLinkedMap;
  Doxygen::globalNamespaceDef.reset();
  Doxygen::globalScope = nullptr;
  delete Doxygen::parserManager;
  delete theTranslator;
  delete g_outputList;

  delete Doxygen::memberNameLinkedMap;
  delete Doxygen::functionNameLinkedMap;
  delete Doxygen::groupLinkedMap;
  delete Doxygen::namespaceLinkedMap;
  delete Doxygen::dirLinkedMap;
  delete Doxygen::symbolMap;
}

static int computeIdealCacheParam(size_t v)
{
  //printf("computeIdealCacheParam(v=%u)\n",v);

  int r=0;
  while (v!=0) v>>=1,r++;
  // r = log2(v)

  // convert to a valid cache size value
  return std::max(0,std::min(r-16,9));
}

void readConfiguration(int argc, char **argv)
{
  QCString versionString = getFullVersion();

  // helper that calls \a func to write to file \a fileName via a TextStream
  auto writeFile = [](const char *fileName,std::function<void(TextStream&)> func) -> bool
  {
    std::ofstream f;
    if (openOutputFile(fileName,f))
    {
      TextStream t(&f);
      func(t);
      return true;
    }
    return false;
  };


  /**************************************************************************
   *             Handle arguments                                           *
   **************************************************************************/

  int optInd=1;
  QCString configName;
  QCString traceName;
  bool genConfig=false;
  bool shortList=false;
  bool traceTiming=false;
  Config::CompareMode diffList=Config::CompareMode::Full;
  bool updateConfig=false;
  bool quiet = false;
  while (optInd<argc && argv[optInd][0]=='-' &&
               (isalpha(argv[optInd][1]) || argv[optInd][1]=='?' ||
                argv[optInd][1]=='-')
        )
  {
    switch(argv[optInd][1])
    {
      case 'g':
        {
          genConfig=TRUE;
        }
        break;
      case 'l':
        {
          QCString layoutName;
          if (optInd+1>=argc)
          {
            layoutName="DoxygenLayout.xml";
          }
          else
          {
            layoutName=argv[optInd+1];
          }
          writeDefaultLayoutFile(layoutName);
          cleanUpDoxygen();
          exit(0);
        }
        break;
      case 'd':
        {
          QCString debugLabel=getArg(argc,argv,optInd);
          if (debugLabel.isEmpty())
          {
            devUsage();
            cleanUpDoxygen();
            exit(0);
          }
          int retVal = Debug::setFlagStr(debugLabel);
          if (!retVal)
          {
            err("option \"-d\" has unknown debug specifier: \"%s\".\n",qPrint(debugLabel));
            devUsage();
            cleanUpDoxygen();
            exit(1);
          }
        }
        break;
      case 't':
        {
#if ENABLE_TRACING
          if (!strcmp(argv[optInd]+1,"t_time"))
          {
            traceTiming = true;
          }
          else if (!strcmp(argv[optInd]+1,"t"))
          {
            traceTiming = false;
          }
          else
          {
            err("option should be \"-t\" or \"-t_time\", found: \"%s\".\n",argv[optInd]);
            cleanUpDoxygen();
            exit(1);
          }
          if (optInd+1>=argc || argv[optInd+1][0] == '-') // no file name given
          {
            traceName="stdout";
          }
          else
          {
            traceName=argv[optInd+1];
            optInd++;
          }
#else
          err("support for option \"-t\" has not been compiled in (use a debug build or a release build with tracing enabled).\n");
          cleanUpDoxygen();
          exit(1);
#endif
        }
        break;
      case 'x':
        if (!strcmp(argv[optInd]+1,"x_noenv")) diffList=Config::CompareMode::CompressedNoEnv;
        else if (!strcmp(argv[optInd]+1,"x")) diffList=Config::CompareMode::Compressed;
        else
        {
          err("option should be \"-x\" or \"-x_noenv\", found: \"%s\".\n",argv[optInd]);
          cleanUpDoxygen();
          exit(1);
        }
        break;
      case 's':
        shortList=TRUE;
        break;
      case 'u':
        updateConfig=TRUE;
        break;
      case 'e':
        {
          QCString formatName=getArg(argc,argv,optInd);
          if (formatName.isEmpty())
          {
            err("option \"-e\" is missing format specifier rtf.\n");
            cleanUpDoxygen();
            exit(1);
          }
          if (qstricmp(formatName.data(),"rtf")==0)
          {
            if (optInd+1>=argc)
            {
              err("option \"-e rtf\" is missing an extensions file name\n");
              cleanUpDoxygen();
              exit(1);
            }
            writeFile(argv[optInd+1],RTFGenerator::writeExtensionsFile);
            cleanUpDoxygen();
            exit(0);
          }
          err("option \"-e\" has invalid format specifier.\n");
          cleanUpDoxygen();
          exit(1);
        }
        break;
      case 'f':
        {
          QCString listName=getArg(argc,argv,optInd);
          if (listName.isEmpty())
          {
            err("option \"-f\" is missing list specifier.\n");
            cleanUpDoxygen();
            exit(1);
          }
          if (qstricmp(listName.data(),"emoji")==0)
          {
            if (optInd+1>=argc)
            {
              err("option \"-f emoji\" is missing an output file name\n");
              cleanUpDoxygen();
              exit(1);
            }
            writeFile(argv[optInd+1],[](TextStream &t) { EmojiEntityMapper::instance().writeEmojiFile(t); });
            cleanUpDoxygen();
            exit(0);
          }
          err("option \"-f\" has invalid list specifier.\n");
          cleanUpDoxygen();
          exit(1);
        }
        break;
      case 'w':
        {
          QCString formatName=getArg(argc,argv,optInd);
          if (formatName.isEmpty())
          {
            err("option \"-w\" is missing format specifier rtf, html or latex\n");
            cleanUpDoxygen();
            exit(1);
          }
          if (qstricmp(formatName.data(),"rtf")==0)
          {
            if (optInd+1>=argc)
            {
              err("option \"-w rtf\" is missing a style sheet file name\n");
              cleanUpDoxygen();
              exit(1);
            }
            if (!writeFile(argv[optInd+1],RTFGenerator::writeStyleSheetFile))
            {
              err("error opening RTF style sheet file %s!\n",argv[optInd+1]);
              cleanUpDoxygen();
              exit(1);
            }
            cleanUpDoxygen();
            exit(0);
          }
          else if (qstricmp(formatName.data(),"html")==0)
          {
            Config::init();
            if (optInd+4<argc || FileInfo("Doxyfile").exists() || FileInfo("doxyfile").exists())
              // explicit config file mentioned or default found on disk
            {
              QCString df = optInd+4<argc ? argv[optInd+4] : (FileInfo("Doxyfile").exists() ? QCString("Doxyfile") : QCString("doxyfile"));
              if (!Config::parse(df)) // parse the config file
              {
                err("error opening or reading configuration file %s!\n",argv[optInd+4]);
                cleanUpDoxygen();
                exit(1);
              }
            }
            if (optInd+3>=argc)
            {
              err("option \"-w html\" does not have enough arguments\n");
              cleanUpDoxygen();
              exit(1);
            }
            Config::postProcess(TRUE);
            Config::updateObsolete();
            Config::checkAndCorrect(Config_getBool(QUIET), false);
            setTranslator(Config_getEnum(OUTPUT_LANGUAGE));
            writeFile(argv[optInd+1],[&](TextStream &t) { HtmlGenerator::writeHeaderFile(t,argv[optInd+3]); });
            writeFile(argv[optInd+2],HtmlGenerator::writeFooterFile);
            writeFile(argv[optInd+3],HtmlGenerator::writeStyleSheetFile);
            cleanUpDoxygen();
            exit(0);
          }
          else if (qstricmp(formatName.data(),"latex")==0)
          {
            Config::init();
            if (optInd+4<argc || FileInfo("Doxyfile").exists() || FileInfo("doxyfile").exists())
            {
              QCString df = optInd+4<argc ? argv[optInd+4] : (FileInfo("Doxyfile").exists() ? QCString("Doxyfile") : QCString("doxyfile"));
              if (!Config::parse(df))
              {
                err("error opening or reading configuration file %s!\n",argv[optInd+4]);
                cleanUpDoxygen();
                exit(1);
              }
            }
            if (optInd+3>=argc)
            {
              err("option \"-w latex\" does not have enough arguments\n");
              cleanUpDoxygen();
              exit(1);
            }
            Config::postProcess(TRUE);
            Config::updateObsolete();
            Config::checkAndCorrect(Config_getBool(QUIET), false);
            setTranslator(Config_getEnum(OUTPUT_LANGUAGE));
            writeFile(argv[optInd+1],LatexGenerator::writeHeaderFile);
            writeFile(argv[optInd+2],LatexGenerator::writeFooterFile);
            writeFile(argv[optInd+3],LatexGenerator::writeStyleSheetFile);
            cleanUpDoxygen();
            exit(0);
          }
          else
          {
            err("Illegal format specifier \"%s\": should be one of rtf, html or latex\n",qPrint(formatName));
            cleanUpDoxygen();
            exit(1);
          }
        }
        break;
      case 'm':
        g_dumpSymbolMap = TRUE;
        break;
      case 'v':
        version(false);
        cleanUpDoxygen();
        exit(0);
        break;
      case 'V':
        version(true);
        cleanUpDoxygen();
        exit(0);
        break;
      case '-':
        if (qstrcmp(&argv[optInd][2],"help")==0)
        {
          usage(argv[0],versionString);
          exit(0);
        }
        else if (qstrcmp(&argv[optInd][2],"version")==0)
        {
          version(false);
          cleanUpDoxygen();
          exit(0);
        }
        else if ((qstrcmp(&argv[optInd][2],"Version")==0) ||
                 (qstrcmp(&argv[optInd][2],"VERSION")==0))
        {
          version(true);
          cleanUpDoxygen();
          exit(0);
        }
        else
        {
          err("Unknown option \"-%s\"\n",&argv[optInd][1]);
          usage(argv[0],versionString);
          exit(1);
        }
        break;
      case 'b':
        setvbuf(stdout,nullptr,_IONBF,0);
        break;
      case 'q':
        quiet = true;
        break;
      case 'h':
      case '?':
        usage(argv[0],versionString);
        exit(0);
        break;
      default:
        err("Unknown option \"-%c\"\n",argv[optInd][1]);
        usage(argv[0],versionString);
        exit(1);
    }
    optInd++;
  }

  /**************************************************************************
   *            Parse or generate the config file                           *
   **************************************************************************/

  initTracing(traceName.data(),traceTiming);
  TRACE("Doxygen version used: {}",getFullVersion());
  Config::init();

  FileInfo configFileInfo1("Doxyfile"),configFileInfo2("doxyfile");
  if (optInd>=argc)
  {
    if (configFileInfo1.exists())
    {
      configName="Doxyfile";
    }
    else if (configFileInfo2.exists())
    {
      configName="doxyfile";
    }
    else if (genConfig)
    {
      configName="Doxyfile";
    }
    else
    {
      err("Doxyfile not found and no input file specified!\n");
      usage(argv[0],versionString);
      exit(1);
    }
  }
  else
  {
    FileInfo fi(argv[optInd]);
    if (fi.exists() || qstrcmp(argv[optInd],"-")==0 || genConfig)
    {
      configName=argv[optInd];
    }
    else
    {
      err("configuration file %s not found!\n",argv[optInd]);
      usage(argv[0],versionString);
      exit(1);
    }
  }

  if (genConfig)
  {
    generateConfigFile(configName,shortList);
    cleanUpDoxygen();
    exit(0);
  }

  if (!Config::parse(configName,updateConfig,diffList))
  {
    err("could not open or read configuration file %s!\n",qPrint(configName));
    cleanUpDoxygen();
    exit(1);
  }

  if (diffList!=Config::CompareMode::Full)
  {
    Config::updateObsolete();
    compareDoxyfile(diffList);
    cleanUpDoxygen();
    exit(0);
  }

  if (updateConfig)
  {
    Config::updateObsolete();
    generateConfigFile(configName,shortList,TRUE);
    cleanUpDoxygen();
    exit(0);
  }

  /* Perlmod wants to know the path to the config file.*/
  FileInfo configFileInfo(configName.str());
  setPerlModDoxyfile(configFileInfo.absFilePath());

  /* handle -q option */
  if (quiet) Config_updateBool(QUIET,TRUE);
}

/** check and resolve config options */
void checkConfiguration()
{
  AUTO_TRACE();

  Config::postProcess(FALSE);
  Config::updateObsolete();
  Config::checkAndCorrect(Config_getBool(QUIET), true);
  initWarningFormat();
}

/** adjust globals that depend on configuration settings. */
void adjustConfiguration()
{
  AUTO_TRACE();
  Doxygen::globalNamespaceDef = createNamespaceDef("<globalScope>",1,1,"<globalScope>");
  Doxygen::globalScope = toNamespaceDefMutable(Doxygen::globalNamespaceDef.get());
  Doxygen::inputNameLinkedMap = new FileNameLinkedMap;
  Doxygen::includeNameLinkedMap = new FileNameLinkedMap;
  Doxygen::exampleNameLinkedMap = new FileNameLinkedMap;
  Doxygen::imageNameLinkedMap = new FileNameLinkedMap;
  Doxygen::dotFileNameLinkedMap = new FileNameLinkedMap;
  Doxygen::mscFileNameLinkedMap = new FileNameLinkedMap;
  Doxygen::diaFileNameLinkedMap = new FileNameLinkedMap;
  Doxygen::plantUmlFileNameLinkedMap = new FileNameLinkedMap;

  setTranslator(Config_getEnum(OUTPUT_LANGUAGE));

  /* Set the global html file extension. */
  Doxygen::htmlFileExtension = Config_getString(HTML_FILE_EXTENSION);


  Doxygen::parseSourcesNeeded = Config_getBool(CALL_GRAPH) ||
                                Config_getBool(CALLER_GRAPH) ||
                                Config_getBool(REFERENCES_RELATION) ||
                                Config_getBool(REFERENCED_BY_RELATION);

  /**************************************************************************
   *            Add custom extension mappings
   **************************************************************************/

  const StringVector &extMaps = Config_getList(EXTENSION_MAPPING);
  for (const auto &mapping : extMaps)
  {
    QCString mapStr = mapping.c_str();
    int i=mapStr.find('=');
    if (i==-1)
    {
      continue;
    }
    else
    {
      QCString ext = mapStr.left(i).stripWhiteSpace().lower();
      QCString language = mapStr.mid(i+1).stripWhiteSpace().lower();
      if (ext.isEmpty() || language.isEmpty())
      {
        continue;
      }

      if (!updateLanguageMapping(ext,language))
      {
        err("Failed to map file extension '%s' to unsupported language '%s'.\n"
            "Check the EXTENSION_MAPPING setting in the config file.\n",
            qPrint(ext),qPrint(language));
      }
      else
      {
        msg("Adding custom extension mapping: '%s' will be treated as language '%s'\n",
            qPrint(ext),qPrint(language));
      }
    }
  }
  // create input file exncodings

  // check INPUT_ENCODING
  void *cd = portable_iconv_open("UTF-8",Config_getString(INPUT_ENCODING).data());
  if (cd==reinterpret_cast<void *>(-1))
  {
    term("unsupported character conversion: '%s'->'%s': %s\n"
        "Check the 'INPUT_ENCODING' setting in the config file!\n",
        qPrint(Config_getString(INPUT_ENCODING)),qPrint("UTF-8"),strerror(errno));
  }
  else
  {
    portable_iconv_close(cd);
  }

  // check and split INPUT_FILE_ENCODING
  const StringVector &fileEncod = Config_getList(INPUT_FILE_ENCODING);
  for (const auto &mapping : fileEncod)
  {
    QCString mapStr = mapping.c_str();
    int i=mapStr.find('=');
    if (i==-1)
    {
      continue;
    }
    else
    {
      QCString pattern = mapStr.left(i).stripWhiteSpace().lower();
      QCString encoding = mapStr.mid(i+1).stripWhiteSpace().lower();
      if (pattern.isEmpty() || encoding.isEmpty())
      {
        continue;
      }
      cd = portable_iconv_open("UTF-8",encoding.data());
      if (cd==reinterpret_cast<void *>(-1))
      {
        term("unsupported character conversion: '%s'->'%s': %s\n"
            "Check the 'INPUT_FILE_ENCODING' setting in the config file!\n",
            qPrint(encoding),qPrint("UTF-8"),strerror(errno));
      }
      else
      {
        portable_iconv_close(cd);
      }

      Doxygen::inputFileEncodingList.emplace_back(pattern, encoding);
    }
  }

  // add predefined macro name to a dictionary
  const StringVector &expandAsDefinedList =Config_getList(EXPAND_AS_DEFINED);
  for (const auto &s : expandAsDefinedList)
  {
    Doxygen::expandAsDefinedSet.insert(s.c_str());
  }

  // read aliases and store them in a dictionary
  readAliases();

  // store number of spaces in a tab into Doxygen::spaces
  int tabSize = Config_getInt(TAB_SIZE);
  Doxygen::spaces.resize(tabSize);
  for (int sp=0; sp<tabSize; sp++) Doxygen::spaces.at(sp)=' ';
  Doxygen::spaces.at(tabSize)='\0';
}

#ifdef HAS_SIGNALS
static void stopDoxygen(int)
{
  signal(SIGINT,SIG_DFL);   // Re-register signal handler for default action
  Dir thisDir;
  msg("Cleaning up...\n");
  if (!Doxygen::filterDBFileName.isEmpty())
  {
    thisDir.remove(Doxygen::filterDBFileName.str());
  }
  killpg(0,SIGINT);
  cleanUpDoxygen();
  exitTracing();
  exit(1);
}
#endif

static void writeTagFile()
{
  QCString generateTagFile = Config_getString(GENERATE_TAGFILE);
  if (generateTagFile.isEmpty()) return;

  std::ofstream f = Portable::openOutputStream(generateTagFile);
  if (!f.is_open())
  {
    err("cannot open tag file %s for writing\n",
        qPrint(generateTagFile)
       );
    return;
  }
  TextStream tagFile(&f);
  tagFile << "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n";
  tagFile << "<tagfile doxygen_version=\"" << getDoxygenVersion() << "\"";
  std::string gitVersion = getGitVersion();
  if (!gitVersion.empty())
  {
    tagFile << " doxygen_gitid=\"" << gitVersion << "\"";
  }
  tagFile << ">\n";

  // for each file
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      if (fd->isLinkableInProject()) fd->writeTagFile(tagFile);
    }
  }
  // for each class
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    ClassDefMutable *cdm = toClassDefMutable(cd.get());
    if (cdm && cdm->isLinkableInProject())
    {
      cdm->writeTagFile(tagFile);
    }
  }
  // for each concept
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    ConceptDefMutable *cdm = toConceptDefMutable(cd.get());
    if (cdm && cdm->isLinkableInProject())
    {
      cdm->writeTagFile(tagFile);
    }
  }
  // for each namespace
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    NamespaceDefMutable *ndm = toNamespaceDefMutable(nd.get());
    if (ndm && nd->isLinkableInProject())
    {
      ndm->writeTagFile(tagFile);
    }
  }
  // for each group
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (gd->isLinkableInProject()) gd->writeTagFile(tagFile);
  }
  // for each module
  for (const auto &mod : ModuleManager::instance().modules())
  {
    if (mod->isLinkableInProject()) mod->writeTagFile(tagFile);
  }
  // for each page
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->isLinkableInProject()) pd->writeTagFile(tagFile);
  }
  if (Doxygen::mainPage) Doxygen::mainPage->writeTagFile(tagFile);

  tagFile << "</tagfile>\n";
}

static void exitDoxygen() noexcept
{
  if (!g_successfulRun)  // premature exit
  {
    Dir thisDir;
    msg("Exiting...\n");
    if (!Doxygen::filterDBFileName.isEmpty())
    {
      thisDir.remove(Doxygen::filterDBFileName.str());
    }
  }
}

static QCString createOutputDirectory(const QCString &baseDirName,
                                  const QCString &formatDirName,
                                  const char *defaultDirName)
{
  QCString result = formatDirName;
  if (result.isEmpty())
  {
    result = baseDirName + defaultDirName;
  }
  else if (formatDirName[0]!='/' && (formatDirName.length()==1 || formatDirName[1]!=':'))
  {
    result.prepend(baseDirName+"/");
  }
  Dir formatDir(result.str());
  if (!formatDir.exists() && !formatDir.mkdir(result.str()))
  {
    term("Could not create output directory %s\n", qPrint(result));
  }
  return result;
}

void searchInputFiles()
{
  StringUnorderedSet killSet;

  const StringVector &exclPatterns = Config_getList(EXCLUDE_PATTERNS);
  bool alwaysRecursive = Config_getBool(RECURSIVE);
  StringUnorderedSet excludeNameSet;

  // gather names of all files in the include path
  g_s.begin("Searching for include files...\n");
  killSet.clear();
  const StringVector &includePathList = Config_getList(INCLUDE_PATH);
  for (const auto &s : includePathList)
  {
    size_t plSize = Config_getList(INCLUDE_FILE_PATTERNS).size();
    const StringVector &pl = plSize==0 ? Config_getList(FILE_PATTERNS) :
                                         Config_getList(INCLUDE_FILE_PATTERNS);
    readFileOrDirectory(s.c_str(),                     // s
                        Doxygen::includeNameLinkedMap, // fnDict
                        nullptr,                       // exclSet
                        &pl,                           // patList
                        &exclPatterns,                 // exclPatList
                        nullptr,                       // resultList
                        nullptr,                       // resultSet
                        false,                         // INCLUDE_PATH isn't recursive
                        TRUE,                          // errorIfNotExist
                        &killSet);                     // killSet
  }
  g_s.end();

  g_s.begin("Searching for example files...\n");
  killSet.clear();
  const StringVector &examplePathList = Config_getList(EXAMPLE_PATH);
  for (const auto &s : examplePathList)
  {
    readFileOrDirectory(s.c_str(),                                              // s
                        Doxygen::exampleNameLinkedMap,                          // fnDict
                        nullptr,                                                // exclSet
                        &Config_getList(EXAMPLE_PATTERNS),                      // patList
                        nullptr,                                                // exclPatList
                        nullptr,                                                // resultList
                        nullptr,                                                // resultSet
                        (alwaysRecursive || Config_getBool(EXAMPLE_RECURSIVE)), // recursive
                        TRUE,                                                   // errorIfNotExist
                        &killSet);                                              // killSet
  }
  g_s.end();

  g_s.begin("Searching for images...\n");
  killSet.clear();
  const StringVector &imagePathList=Config_getList(IMAGE_PATH);
  for (const auto &s : imagePathList)
  {
    readFileOrDirectory(s.c_str(),                        // s
                        Doxygen::imageNameLinkedMap,      // fnDict
                        nullptr,                          // exclSet
                        nullptr,                          // patList
                        nullptr,                          // exclPatList
                        nullptr,                          // resultList
                        nullptr,                          // resultSet
                        alwaysRecursive,                  // recursive
                        TRUE,                             // errorIfNotExist
                        &killSet);                        // killSet
  }
  g_s.end();

  g_s.begin("Searching for dot files...\n");
  killSet.clear();
  const StringVector &dotFileList=Config_getList(DOTFILE_DIRS);
  for (const auto &s : dotFileList)
  {
    readFileOrDirectory(s.c_str(),                      // s
                        Doxygen::dotFileNameLinkedMap,  // fnDict
                        nullptr,                        // exclSet
                        nullptr,                        // patList
                        nullptr,                        // exclPatList
                        nullptr,                        // resultList
                        nullptr,                        // resultSet
                        alwaysRecursive,                // recursive
                        TRUE,                           // errorIfNotExist
                        &killSet);                      // killSet
  }
  g_s.end();

  g_s.begin("Searching for msc files...\n");
  killSet.clear();
  const StringVector &mscFileList=Config_getList(MSCFILE_DIRS);
  for (const auto &s : mscFileList)
  {
    readFileOrDirectory(s.c_str(),                       // s
                        Doxygen::mscFileNameLinkedMap,   // fnDict
                        nullptr,                         // exclSet
                        nullptr,                         // patList
                        nullptr,                         // exclPatList
                        nullptr,                         // resultList
                        nullptr,                         // resultSet
                        alwaysRecursive,                 // recursive
                        TRUE,                            // errorIfNotExist
                        &killSet);                       // killSet
  }
  g_s.end();

  g_s.begin("Searching for dia files...\n");
  killSet.clear();
  const StringVector &diaFileList=Config_getList(DIAFILE_DIRS);
  for (const auto &s : diaFileList)
  {
    readFileOrDirectory(s.c_str(),                         // s
                        Doxygen::diaFileNameLinkedMap,     // fnDict
                        nullptr,                           // exclSet
                        nullptr,                           // patList
                        nullptr,                           // exclPatList
                        nullptr,                           // resultList
                        nullptr,                           // resultSet
                        alwaysRecursive,                   // recursive
                        TRUE,                              // errorIfNotExist
                        &killSet);                         // killSet
  }
  g_s.end();

  g_s.begin("Searching for plantuml files...\n");
  killSet.clear();
  const StringVector &plantUmlFileList=Config_getList(PLANTUMLFILE_DIRS);
  for (const auto &s : plantUmlFileList)
  {
    readFileOrDirectory(s.c_str(),                         // s
                        Doxygen::plantUmlFileNameLinkedMap,// fnDict
                        nullptr,                           // exclSet
                        nullptr,                           // patList
                        nullptr,                           // exclPatList
                        nullptr,                           // resultList
                        nullptr,                           // resultSet
                        alwaysRecursive,                   // recursive
                        TRUE,                              // errorIfNotExist
                        &killSet);                         // killSet
  }
  g_s.end();

  g_s.begin("Searching for files to exclude\n");
  const StringVector &excludeList = Config_getList(EXCLUDE);
  for (const auto &s : excludeList)
  {
    readFileOrDirectory(s.c_str(),                          // s
                        nullptr,                            // fnDict
                        nullptr,                            // exclSet
                        &Config_getList(FILE_PATTERNS),     // patList
                        nullptr,                            // exclPatList
                        nullptr,                            // resultList
                        &excludeNameSet,                    // resultSet
                        alwaysRecursive,                    // recursive
                        FALSE);                             // errorIfNotExist
  }
  g_s.end();

  /**************************************************************************
   *             Determine Input Files                                      *
   **************************************************************************/

  g_s.begin("Searching INPUT for files to process...\n");
  killSet.clear();
  Doxygen::inputPaths.clear();
  const StringVector &inputList=Config_getList(INPUT);
  for (const auto &s : inputList)
  {
    QCString path=s.c_str();
    size_t l = path.length();
    if (l>0)
    {
      // strip trailing slashes
      if (path.at(l-1)=='\\' || path.at(l-1)=='/') path=path.left(l-1);

      readFileOrDirectory(
          path,                               // s
          Doxygen::inputNameLinkedMap,        // fnDict
          &excludeNameSet,                    // exclSet
          &Config_getList(FILE_PATTERNS),     // patList
          &exclPatterns,                      // exclPatList
          &g_inputFiles,                      // resultList
          nullptr,                            // resultSet
          alwaysRecursive,                    // recursive
          TRUE,                               // errorIfNotExist
          &killSet,                           // killSet
          &Doxygen::inputPaths);              // paths
    }
  }

  // Sort the FileDef objects by full path to get a predictable ordering over multiple runs
  std::stable_sort(Doxygen::inputNameLinkedMap->begin(),
            Doxygen::inputNameLinkedMap->end(),
            [](const auto &f1,const auto &f2)
            {
              return  qstricmp_sort(f1->fullName(),f2->fullName())<0;
            });
  for (auto &fileName : *Doxygen::inputNameLinkedMap)
  {
    if (fileName->size()>1)
    {
      std::stable_sort(fileName->begin(),fileName->end(),[](const auto &f1,const auto &f2)
        {
          return qstricmp_sort(f1->absFilePath(),f2->absFilePath())<0;
        });
    }
  }
  if (Doxygen::inputNameLinkedMap->empty())
  {
    warn_uncond("No files to be processed, please check your settings, in particular INPUT, FILE_PATTERNS, and RECURSIVE\n");
  }
  g_s.end();
}


static void checkMarkdownMainfile()
{
  if (Config_getBool(MARKDOWN_SUPPORT))
  {
    QCString mdfileAsMainPage = Config_getString(USE_MDFILE_AS_MAINPAGE);
    if (mdfileAsMainPage.isEmpty()) return;
    FileInfo fi(mdfileAsMainPage.data());
    if (!fi.exists())
    {
      warn_uncond("Specified markdown mainpage '%s' does not exist\n",qPrint(mdfileAsMainPage));
      return;
    }
    bool ambig = false;
    if (findFileDef(Doxygen::inputNameLinkedMap,fi.absFilePath(),ambig)==nullptr)
    {
      warn_uncond("Specified markdown mainpage '%s' has not been defined as input file\n",qPrint(mdfileAsMainPage));
      return;
    }
  }
}

void parseInput()
{
  AUTO_TRACE();
  std::atexit(exitDoxygen);

  Portable::correctPath(Config_getList(EXTERNAL_TOOL_PATH));

#if USE_LIBCLANG
  Doxygen::clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
#endif

  // we would like to show the versionString earlier, but we first have to handle the configuration file
  // to know the value of the QUIET setting.
  QCString versionString = getFullVersion();
  msg("Doxygen version used: %s\n",qPrint(versionString));

  computeVerifiedDotPath();

  /**************************************************************************
   *            Make sure the output directory exists
   **************************************************************************/
  QCString outputDirectory = Config_getString(OUTPUT_DIRECTORY);
  if (outputDirectory.isEmpty())
  {
    outputDirectory = Config_updateString(OUTPUT_DIRECTORY,Dir::currentDirPath().c_str());
  }
  else
  {
    Dir dir(outputDirectory.str());
    if (!dir.exists())
    {
      dir.setPath(Dir::currentDirPath());
      if (!dir.mkdir(outputDirectory.str()))
      {
        term("tag OUTPUT_DIRECTORY: Output directory '%s' does not "
             "exist and cannot be created\n",qPrint(outputDirectory));
      }
      else
      {
        msg("Notice: Output directory '%s' does not exist. "
            "I have created it for you.\n", qPrint(outputDirectory));
      }
      dir.setPath(outputDirectory.str());
    }
    outputDirectory = Config_updateString(OUTPUT_DIRECTORY,dir.absPath().c_str());
  }
  AUTO_TRACE_ADD("outputDirectory={}",outputDirectory);

  /**************************************************************************
   *            Initialize global lists and dictionaries
   **************************************************************************/

  // also scale lookup cache with SYMBOL_CACHE_SIZE
  int cacheSize = Config_getInt(LOOKUP_CACHE_SIZE);
  if (cacheSize<0) cacheSize=0;
  if (cacheSize>9) cacheSize=9;
  uint32_t lookupSize = 65536 << cacheSize;
  Doxygen::typeLookupCache = new Cache<std::string,LookupInfo>(lookupSize);
  Doxygen::symbolLookupCache = new Cache<std::string,LookupInfo>(lookupSize);

#ifdef HAS_SIGNALS
  signal(SIGINT, stopDoxygen);
#endif

  uint32_t pid = Portable::pid();
  Doxygen::filterDBFileName.sprintf("doxygen_filterdb_%d.tmp",pid);
  Doxygen::filterDBFileName.prepend(outputDirectory+"/");

  /**************************************************************************
   *            Check/create output directories                             *
   **************************************************************************/

  QCString htmlOutput;
  bool generateHtml = Config_getBool(GENERATE_HTML);
  if (generateHtml)
  {
    htmlOutput = createOutputDirectory(outputDirectory,Config_getString(HTML_OUTPUT),"/html");
    Config_updateString(HTML_OUTPUT,htmlOutput);

    QCString sitemapUrl = Config_getString(SITEMAP_URL);
    bool generateSitemap = !sitemapUrl.isEmpty();
    if (generateSitemap && !sitemapUrl.endsWith("/"))
    {
      Config_updateString(SITEMAP_URL,sitemapUrl+"/");
    }

    // add HTML indexers that are enabled
    bool generateHtmlHelp    = Config_getBool(GENERATE_HTMLHELP);
    bool generateEclipseHelp = Config_getBool(GENERATE_ECLIPSEHELP);
    bool generateQhp         = Config_getBool(GENERATE_QHP);
    bool generateTreeView    = Config_getBool(GENERATE_TREEVIEW);
    bool generateDocSet      = Config_getBool(GENERATE_DOCSET);
    if (generateEclipseHelp) Doxygen::indexList->addIndex<EclipseHelp>();
    if (generateHtmlHelp)    Doxygen::indexList->addIndex<HtmlHelp>();
    if (generateQhp)         Doxygen::indexList->addIndex<Qhp>();
    if (generateSitemap)     Doxygen::indexList->addIndex<Sitemap>();
    if (generateTreeView)    Doxygen::indexList->addIndex<FTVHelp>(TRUE);
    if (generateDocSet)      Doxygen::indexList->addIndex<DocSets>();
    Doxygen::indexList->addIndex<Crawlmap>();
    Doxygen::indexList->initialize();
  }

  QCString docbookOutput;
  bool generateDocbook = Config_getBool(GENERATE_DOCBOOK);
  if (generateDocbook)
  {
    docbookOutput = createOutputDirectory(outputDirectory,Config_getString(DOCBOOK_OUTPUT),"/docbook");
    Config_updateString(DOCBOOK_OUTPUT,docbookOutput);
  }

  QCString xmlOutput;
  bool generateXml = Config_getBool(GENERATE_XML);
  if (generateXml)
  {
    xmlOutput = createOutputDirectory(outputDirectory,Config_getString(XML_OUTPUT),"/xml");
    Config_updateString(XML_OUTPUT,xmlOutput);
  }

  QCString latexOutput;
  bool generateLatex = Config_getBool(GENERATE_LATEX);
  if (generateLatex)
  {
    latexOutput = createOutputDirectory(outputDirectory,Config_getString(LATEX_OUTPUT), "/latex");
    Config_updateString(LATEX_OUTPUT,latexOutput);
  }

  QCString rtfOutput;
  bool generateRtf = Config_getBool(GENERATE_RTF);
  if (generateRtf)
  {
    rtfOutput = createOutputDirectory(outputDirectory,Config_getString(RTF_OUTPUT),"/rtf");
    Config_updateString(RTF_OUTPUT,rtfOutput);
  }

  QCString manOutput;
  bool generateMan = Config_getBool(GENERATE_MAN);
  if (generateMan)
  {
    manOutput = createOutputDirectory(outputDirectory,Config_getString(MAN_OUTPUT),"/man");
    Config_updateString(MAN_OUTPUT,manOutput);
  }

  QCString sqlOutput;
  bool generateSql = Config_getBool(GENERATE_SQLITE3);
  if (generateSql)
  {
    sqlOutput = createOutputDirectory(outputDirectory,Config_getString(SQLITE3_OUTPUT),"/sqlite3");
    Config_updateString(SQLITE3_OUTPUT,sqlOutput);
  }

  if (Config_getBool(HAVE_DOT))
  {
    QCString curFontPath = Config_getString(DOT_FONTPATH);
    if (curFontPath.isEmpty())
    {
      Portable::getenv("DOTFONTPATH");
      QCString newFontPath = ".";
      if (!curFontPath.isEmpty())
      {
        newFontPath+=Portable::pathListSeparator();
        newFontPath+=curFontPath;
      }
      Portable::setenv("DOTFONTPATH",qPrint(newFontPath));
    }
    else
    {
      Portable::setenv("DOTFONTPATH",qPrint(curFontPath));
    }
  }



  /**************************************************************************
   *             Handle layout file                                         *
   **************************************************************************/

  LayoutDocManager::instance().init();
  QCString layoutFileName = Config_getString(LAYOUT_FILE);
  bool defaultLayoutUsed = FALSE;
  if (layoutFileName.isEmpty())
  {
    layoutFileName = Config_updateString(LAYOUT_FILE,"DoxygenLayout.xml");
    defaultLayoutUsed = TRUE;
  }
  AUTO_TRACE_ADD("defaultLayoutUsed={}, layoutFileName={}",defaultLayoutUsed,layoutFileName);

  FileInfo fi(layoutFileName.str());
  if (fi.exists())
  {
    msg("Parsing layout file %s...\n",qPrint(layoutFileName));
    LayoutDocManager::instance().parse(layoutFileName);
  }
  else if (!defaultLayoutUsed)
  {
      warn_uncond("failed to open layout file '%s' for reading! Using default settings.\n",qPrint(layoutFileName));
  }
  printLayout();

  /**************************************************************************
   *             Read and preprocess input                                  *
   **************************************************************************/

  // prevent search in the output directories
  StringVector exclPatterns = Config_getList(EXCLUDE_PATTERNS);
  if (generateHtml)    exclPatterns.push_back(htmlOutput.str());
  if (generateDocbook) exclPatterns.push_back(docbookOutput.str());
  if (generateXml)     exclPatterns.push_back(xmlOutput.str());
  if (generateLatex)   exclPatterns.push_back(latexOutput.str());
  if (generateRtf)     exclPatterns.push_back(rtfOutput.str());
  if (generateMan)     exclPatterns.push_back(manOutput.str());
  Config_updateList(EXCLUDE_PATTERNS,exclPatterns);

  searchInputFiles();

  checkMarkdownMainfile();

  // Notice: the order of the function calls below is very important!

  if (Config_getBool(GENERATE_HTML) && !Config_getBool(USE_MATHJAX))
  {
    FormulaManager::instance().initFromRepository(Config_getString(HTML_OUTPUT));
  }
  if (Config_getBool(GENERATE_RTF))
  {
    FormulaManager::instance().initFromRepository(Config_getString(RTF_OUTPUT));
  }
  if (Config_getBool(GENERATE_DOCBOOK))
  {
    FormulaManager::instance().initFromRepository(Config_getString(DOCBOOK_OUTPUT));
  }

  FormulaManager::instance().checkRepositories();

  /**************************************************************************
   *             Handle Tag Files                                           *
   **************************************************************************/

  std::shared_ptr<Entry> root = std::make_shared<Entry>();
  msg("Reading and parsing tag files\n");

  const StringVector &tagFileList = Config_getList(TAGFILES);
  for (const auto &s : tagFileList)
  {
    readTagFile(root,s.c_str());
  }

  /**************************************************************************
   *             Parse source files                                         *
   **************************************************************************/

  addSTLSupport(root);

  g_s.begin("Parsing files\n");
  if (Config_getInt(NUM_PROC_THREADS)==1)
  {
    parseFilesSingleThreading(root);
  }
  else
  {
    parseFilesMultiThreading(root);
  }
  g_s.end();

  /**************************************************************************
   *             Gather information                                         *
   **************************************************************************/

  g_s.begin("Building macro definition list...\n");
  buildDefineList();
  g_s.end();

  g_s.begin("Building group list...\n");
  buildGroupList(root.get());
  organizeSubGroups(root.get());
  g_s.end();

  g_s.begin("Building directory list...\n");
  buildDirectories();
  findDirDocumentation(root.get());
  g_s.end();

  g_s.begin("Building namespace list...\n");
  buildNamespaceList(root.get());
  findUsingDirectives(root.get());
  g_s.end();

  g_s.begin("Building file list...\n");
  buildFileList(root.get());
  g_s.end();

  g_s.begin("Building class list...\n");
  buildClassList(root.get());
  g_s.end();

  g_s.begin("Building concept list...\n");
  buildConceptList(root.get());
  g_s.end();

  // build list of using declarations here (global list)
  buildListOfUsingDecls(root.get());
  g_s.end();

  g_s.begin("Computing nesting relations for classes...\n");
  resolveClassNestingRelations();
  g_s.end();
  // 1.8.2-20121111: no longer add nested classes to the group as well
  //distributeClassGroupRelations();

  // calling buildClassList may result in cached relations that
  // become invalid after resolveClassNestingRelations(), that's why
  // we need to clear the cache here
  Doxygen::typeLookupCache->clear();
  // we don't need the list of using declaration anymore
  g_usingDeclarations.clear();

  g_s.begin("Associating documentation with classes...\n");
  buildClassDocList(root.get());
  g_s.end();

  g_s.begin("Associating documentation with concepts...\n");
  buildConceptDocList(root.get());
  distributeConceptGroups();
  g_s.end();

  g_s.begin("Associating documentation with modules...\n");
  findModuleDocumentation(root.get());
  g_s.end();

  g_s.begin("Building example list...\n");
  buildExampleList(root.get());
  g_s.end();

  g_s.begin("Searching for enumerations...\n");
  findEnums(root.get());
  g_s.end();

  // Since buildVarList calls isVarWithConstructor
  // and this calls getResolvedClass we need to process
  // typedefs first so the relations between classes via typedefs
  // are properly resolved. See bug 536385 for an example.
  g_s.begin("Searching for documented typedefs...\n");
  buildTypedefList(root.get());
  g_s.end();

  if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
  {
    g_s.begin("Searching for documented sequences...\n");
    buildSequenceList(root.get());
    g_s.end();

    g_s.begin("Searching for documented dictionaries...\n");
    buildDictionaryList(root.get());
    g_s.end();
  }

  g_s.begin("Searching for members imported via using declarations...\n");
  // this should be after buildTypedefList in order to properly import
  // used typedefs
  findUsingDeclarations(root.get(),TRUE);  // do for python packages first
  findUsingDeclarations(root.get(),FALSE); // then the rest
  g_s.end();

  g_s.begin("Searching for included using directives...\n");
  findIncludedUsingDirectives();
  g_s.end();

  g_s.begin("Searching for documented variables...\n");
  buildVarList(root.get());
  g_s.end();

  g_s.begin("Building interface member list...\n");
  buildInterfaceAndServiceList(root.get()); // UNO IDL

  g_s.begin("Building member list...\n"); // using class info only !
  buildFunctionList(root.get());
  g_s.end();

  g_s.begin("Searching for friends...\n");
  findFriends();
  g_s.end();

  g_s.begin("Searching for documented defines...\n");
  findDefineDocumentation(root.get());
  g_s.end();

  g_s.begin("Computing class inheritance relations...\n");
  findClassEntries(root.get());
  findInheritedTemplateInstances();
  g_s.end();

  g_s.begin("Computing class usage relations...\n");
  findUsedTemplateInstances();
  g_s.end();

  if (Config_getBool(INLINE_SIMPLE_STRUCTS))
  {
    g_s.begin("Searching for tag less structs...\n");
    findTagLessClasses();
    g_s.end();
  }

  g_s.begin("Flushing cached template relations that have become invalid...\n");
  flushCachedTemplateRelations();
  g_s.end();

  g_s.begin("Warn for undocumented namespaces...\n");
  warnUndocumentedNamespaces();
  g_s.end();

  g_s.begin("Computing class relations...\n");
  computeTemplateClassRelations();
  flushUnresolvedRelations();
  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
  {
    VhdlDocGen::computeVhdlComponentRelations();
  }
  computeClassRelations();
  g_classEntries.clear();
  g_s.end();

  g_s.begin("Add enum values to enums...\n");
  addEnumValuesToEnums(root.get());
  findEnumDocumentation(root.get());
  g_s.end();

  g_s.begin("Searching for member function documentation...\n");
  findObjCMethodDefinitions(root.get());
  findMemberDocumentation(root.get()); // may introduce new members !
  findUsingDeclImports(root.get()); // may introduce new members !
  transferRelatedFunctionDocumentation();
  transferFunctionDocumentation();
  transferStaticInstanceInitializers();
  g_s.end();

  // moved to after finding and copying documentation,
  // as this introduces new members see bug 722654
  g_s.begin("Creating members for template instances...\n");
  createTemplateInstanceMembers();
  g_s.end();

  g_s.begin("Building page list...\n");
  buildPageList(root.get());
  g_s.end();

  g_s.begin("Search for main page...\n");
  findMainPage(root.get());
  findMainPageTagFiles(root.get());
  g_s.end();

  g_s.begin("Computing page relations...\n");
  computePageRelations(root.get());
  checkPageRelations();
  g_s.end();

  g_s.begin("Determining the scope of groups...\n");
  findGroupScope(root.get());
  g_s.end();

  g_s.begin("Computing module relations...\n");
  auto &mm = ModuleManager::instance();
  mm.resolvePartitions();
  mm.resolveImports();
  mm.collectExportedSymbols();
  g_s.end();

  auto memberNameComp = [](const MemberNameLinkedMap::Ptr &n1,const MemberNameLinkedMap::Ptr &n2)
  {
    return qstricmp_sort(n1->memberName().data()+getPrefixIndex(n1->memberName()),
                         n2->memberName().data()+getPrefixIndex(n2->memberName())
                        )<0;
  };

  auto classComp = [](const ClassLinkedMap::Ptr &c1,const ClassLinkedMap::Ptr &c2)
  {
    if (Config_getBool(SORT_BY_SCOPE_NAME))
    {
      return qstricmp_sort(c1->name(), c2->name())<0;
    }
    else
    {
      int i = qstricmp_sort(c1->className(), c2->className());
      return i==0 ? qstricmp_sort(c1->name(), c2->name())<0 : i<0;
    }
  };

  auto namespaceComp = [](const NamespaceLinkedMap::Ptr &n1,const NamespaceLinkedMap::Ptr &n2)
  {
    return qstricmp_sort(n1->name(),n2->name())<0;
  };

  auto conceptComp = [](const ConceptLinkedMap::Ptr &c1,const ConceptLinkedMap::Ptr &c2)
  {
    return qstricmp_sort(c1->name(),c2->name())<0;
  };

  g_s.begin("Sorting lists...\n");
  std::stable_sort(Doxygen::memberNameLinkedMap->begin(),
            Doxygen::memberNameLinkedMap->end(),
            memberNameComp);
  std::stable_sort(Doxygen::functionNameLinkedMap->begin(),
            Doxygen::functionNameLinkedMap->end(),
            memberNameComp);
  std::stable_sort(Doxygen::hiddenClassLinkedMap->begin(),
            Doxygen::hiddenClassLinkedMap->end(),
            classComp);
  std::stable_sort(Doxygen::classLinkedMap->begin(),
            Doxygen::classLinkedMap->end(),
            classComp);
  std::stable_sort(Doxygen::conceptLinkedMap->begin(),
            Doxygen::conceptLinkedMap->end(),
            conceptComp);
  std::stable_sort(Doxygen::namespaceLinkedMap->begin(),
            Doxygen::namespaceLinkedMap->end(),
            namespaceComp);
  g_s.end();

  g_s.begin("Determining which enums are documented\n");
  findDocumentedEnumValues();
  g_s.end();

  g_s.begin("Computing member relations...\n");
  mergeCategories();
  computeMemberRelations();
  g_s.end();

  g_s.begin("Building full member lists recursively...\n");
  buildCompleteMemberLists();
  g_s.end();

  g_s.begin("Adding members to member groups.\n");
  addMembersToMemberGroup();
  g_s.end();

  if (Config_getBool(DISTRIBUTE_GROUP_DOC))
  {
    g_s.begin("Distributing member group documentation.\n");
    distributeMemberGroupDocumentation();
    g_s.end();
  }

  g_s.begin("Computing member references...\n");
  computeMemberReferences();
  g_s.end();

  if (Config_getBool(INHERIT_DOCS))
  {
    g_s.begin("Inheriting documentation...\n");
    inheritDocumentation();
    g_s.end();
  }


  // compute the shortest possible names of all files
  // without losing the uniqueness of the file names.
  g_s.begin("Generating disk names...\n");
  generateDiskNames();
  g_s.end();

  g_s.begin("Adding source references...\n");
  addSourceReferences();
  g_s.end();

  g_s.begin("Adding xrefitems...\n");
  addListReferences();
  generateXRefPages();
  g_s.end();

  g_s.begin("Sorting member lists...\n");
  sortMemberLists();
  g_s.end();

  g_s.begin("Setting anonymous enum type...\n");
  setAnonymousEnumType();
  g_s.end();

  g_s.begin("Computing dependencies between directories...\n");
  computeDirDependencies();
  g_s.end();

  g_s.begin("Generating citations page...\n");
  CitationManager::instance().generatePage();
  g_s.end();

  g_s.begin("Counting members...\n");
  countMembers();
  g_s.end();

  g_s.begin("Counting data structures...\n");
  Index::instance().countDataStructures();
  g_s.end();

  g_s.begin("Resolving user defined references...\n");
  resolveUserReferences();
  g_s.end();

  g_s.begin("Finding anchors and sections in the documentation...\n");
  findSectionsInDocumentation();
  g_s.end();

  g_s.begin("Transferring function references...\n");
  transferFunctionReferences();
  g_s.end();

  g_s.begin("Combining using relations...\n");
  combineUsingRelations();
  g_s.end();

  initSearchIndexer();
  g_s.begin("Adding members to index pages...\n");
  addMembersToIndex();
  addToIndices();
  g_s.end();

  g_s.begin("Correcting members for VHDL...\n");
  vhdlCorrectMemberProperties();
  g_s.end();

  g_s.begin("Computing tooltip texts...\n");
  computeTooltipTexts();
  g_s.end();

  if (Config_getBool(SORT_GROUP_NAMES))
  {
    std::stable_sort(Doxygen::groupLinkedMap->begin(),
              Doxygen::groupLinkedMap->end(),
              [](const auto &g1,const auto &g2)
              { return g1->groupTitle() < g2->groupTitle(); });

    for (const auto &gd : *Doxygen::groupLinkedMap)
    {
      gd->sortSubGroups();
    }
  }

  printNavTree(root.get(),0);
  printSectionsTree();
}

void generateOutput()
{
  AUTO_TRACE();
  /**************************************************************************
   *            Initialize output generators                                *
   **************************************************************************/

  /// add extra languages for which we can only produce syntax highlighted code
  addCodeOnlyMappings();

  //// dump all symbols
  if (g_dumpSymbolMap)
  {
    dumpSymbolMap();
    exit(0);
  }

  bool generateHtml  = Config_getBool(GENERATE_HTML);
  bool generateLatex = Config_getBool(GENERATE_LATEX);
  bool generateMan   = Config_getBool(GENERATE_MAN);
  bool generateRtf   = Config_getBool(GENERATE_RTF);
  bool generateDocbook = Config_getBool(GENERATE_DOCBOOK);


  g_outputList = new OutputList;
  if (generateHtml)
  {
    g_outputList->add<HtmlGenerator>();
    HtmlGenerator::init();
    HtmlGenerator::writeTabData();
  }
  if (generateLatex)
  {
    g_outputList->add<LatexGenerator>();
    LatexGenerator::init();
  }
  if (generateDocbook)
  {
    g_outputList->add<DocbookGenerator>();
    DocbookGenerator::init();
  }
  if (generateMan)
  {
    g_outputList->add<ManGenerator>();
    ManGenerator::init();
  }
  if (generateRtf)
  {
    g_outputList->add<RTFGenerator>();
    RTFGenerator::init();
  }
  if (Config_getBool(USE_HTAGS))
  {
    Htags::useHtags = TRUE;
    QCString htmldir = Config_getString(HTML_OUTPUT);
    if (!Htags::execute(htmldir))
       err("USE_HTAGS is YES but htags(1) failed. \n");
    else if (!Htags::loadFilemap(htmldir))
       err("htags(1) ended normally but failed to load the filemap. \n");
  }

  /**************************************************************************
   *                        Generate documentation                          *
   **************************************************************************/

  g_s.begin("Generating style sheet...\n");
  //printf("writing style info\n");
  g_outputList->writeStyleInfo(0); // write first part
  g_s.end();

  bool searchEngine      = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);

  g_s.begin("Generating search indices...\n");
  if (searchEngine && !serverBasedSearch && generateHtml)
  {
    createJavaScriptSearchIndex();
  }

  // generate search indices (need to do this before writing other HTML
  // pages as these contain a drop down menu with options depending on
  // what categories we find in this function.
  if (generateHtml && searchEngine)
  {
    QCString searchDirName = Config_getString(HTML_OUTPUT)+"/search";
    Dir searchDir(searchDirName.str());
    if (!searchDir.exists() && !searchDir.mkdir(searchDirName.str()))
    {
      term("Could not create search results directory '%s' $PWD='%s'\n",
          qPrint(searchDirName),Dir::currentDirPath().c_str());
    }
    HtmlGenerator::writeSearchData(searchDirName);
    if (!serverBasedSearch) // client side search index
    {
      writeJavaScriptSearchIndex();
    }
  }
  g_s.end();

  // copy static stuff
  if (generateHtml)
  {
    FTVHelp::generateTreeViewImages();
    copyStyleSheet();
    copyLogo(Config_getString(HTML_OUTPUT));
    copyIcon(Config_getString(HTML_OUTPUT));
    copyExtraFiles(Config_getList(HTML_EXTRA_FILES),"HTML_EXTRA_FILES",Config_getString(HTML_OUTPUT));
  }
  if (generateLatex)
  {
    copyLatexStyleSheet();
    copyLogo(Config_getString(LATEX_OUTPUT));
    copyIcon(Config_getString(LATEX_OUTPUT));
    copyExtraFiles(Config_getList(LATEX_EXTRA_FILES),"LATEX_EXTRA_FILES",Config_getString(LATEX_OUTPUT));
  }
  if (generateDocbook)
  {
    copyLogo(Config_getString(DOCBOOK_OUTPUT));
    copyIcon(Config_getString(DOCBOOK_OUTPUT));
  }
  if (generateRtf)
  {
    copyLogo(Config_getString(RTF_OUTPUT));
    copyIcon(Config_getString(RTF_OUTPUT));
    copyExtraFiles(Config_getList(RTF_EXTRA_FILES),"RTF_EXTRA_FILES",Config_getString(RTF_OUTPUT));
  }

  FormulaManager &fm = FormulaManager::instance();
  if (fm.hasFormulas() && generateHtml
      && !Config_getBool(USE_MATHJAX))
  {
    g_s.begin("Generating images for formulas in HTML...\n");
    fm.generateImages(Config_getString(HTML_OUTPUT), Config_getEnum(HTML_FORMULA_FORMAT)==HTML_FORMULA_FORMAT_t::svg ?
        FormulaManager::Format::Vector : FormulaManager::Format::Bitmap, FormulaManager::HighDPI::On);
    g_s.end();
  }
  if (fm.hasFormulas() && generateRtf)
  {
    g_s.begin("Generating images for formulas in RTF...\n");
    fm.generateImages(Config_getString(RTF_OUTPUT),FormulaManager::Format::Bitmap);
    g_s.end();
  }

  if (fm.hasFormulas() && generateDocbook)
  {
    g_s.begin("Generating images for formulas in Docbook...\n");
    fm.generateImages(Config_getString(DOCBOOK_OUTPUT),FormulaManager::Format::Bitmap);
    g_s.end();
  }

  g_s.begin("Generating example documentation...\n");
  generateExampleDocs();
  g_s.end();

  g_s.begin("Generating file sources...\n");
  generateFileSources();
  g_s.end();

  g_s.begin("Generating file documentation...\n");
  generateFileDocs();
  g_s.end();

  g_s.begin("Generating page documentation...\n");
  generatePageDocs();
  g_s.end();

  g_s.begin("Generating group documentation...\n");
  generateGroupDocs();
  g_s.end();

  g_s.begin("Generating class documentation...\n");
  generateClassDocs();
  g_s.end();

  g_s.begin("Generating concept documentation...\n");
  generateConceptDocs();
  g_s.end();

  g_s.begin("Generating module documentation...\n");
  ModuleManager::instance().writeDocumentation(*g_outputList);
  g_s.end();

  g_s.begin("Generating namespace documentation...\n");
  generateNamespaceDocs();
  g_s.end();

  if (Config_getBool(GENERATE_LEGEND))
  {
    g_s.begin("Generating graph info page...\n");
    writeGraphInfo(*g_outputList);
    g_s.end();
  }

  g_s.begin("Generating directory documentation...\n");
  generateDirDocs(*g_outputList);
  g_s.end();

  if (g_outputList->size()>0)
  {
    writeIndexHierarchy(*g_outputList);
  }

  g_s.begin("finalizing index lists...\n");
  Doxygen::indexList->finalize();
  g_s.end();

  g_s.begin("writing tag file...\n");
  writeTagFile();
  g_s.end();

  if (Config_getBool(GENERATE_XML))
  {
    g_s.begin("Generating XML output...\n");
    Doxygen::generatingXmlOutput=TRUE;
    generateXML();
    Doxygen::generatingXmlOutput=FALSE;
    g_s.end();
  }
  if (Config_getBool(GENERATE_SQLITE3))
  {
    g_s.begin("Generating SQLITE3 output...\n");
    generateSqlite3();
    g_s.end();
  }

  if (Config_getBool(GENERATE_AUTOGEN_DEF))
  {
    g_s.begin("Generating AutoGen DEF output...\n");
    generateDEF();
    g_s.end();
  }
  if (Config_getBool(GENERATE_PERLMOD))
  {
    g_s.begin("Generating Perl module output...\n");
    generatePerlMod();
    g_s.end();
  }
  if (generateHtml && searchEngine && serverBasedSearch)
  {
    g_s.begin("Generating search index\n");
    if (Doxygen::searchIndex.kind()==SearchIndexIntf::Internal) // write own search index
    {
      HtmlGenerator::writeSearchPage();
      Doxygen::searchIndex.write(Config_getString(HTML_OUTPUT)+"/search/search.idx");
    }
    else // write data for external search index
    {
      HtmlGenerator::writeExternalSearchPage();
      QCString searchDataFile = Config_getString(SEARCHDATA_FILE);
      if (searchDataFile.isEmpty())
      {
        searchDataFile="searchdata.xml";
      }
      if (!Portable::isAbsolutePath(searchDataFile.data()))
      {
        searchDataFile.prepend(Config_getString(OUTPUT_DIRECTORY)+"/");
      }
      Doxygen::searchIndex.write(searchDataFile);
    }
    g_s.end();
  }

  if (generateRtf)
  {
    g_s.begin("Combining RTF output...\n");
    if (!RTFGenerator::preProcessFileInplace(Config_getString(RTF_OUTPUT),"refman.rtf"))
    {
      err("An error occurred during post-processing the RTF files!\n");
    }
    g_s.end();
  }

  g_s.begin("Running plantuml with JAVA...\n");
  PlantumlManager::instance().run();
  g_s.end();

  if (Config_getBool(HAVE_DOT))
  {
    g_s.begin("Running dot...\n");
    DotManager::instance()->run();
    g_s.end();
  }

  if (generateHtml &&
      Config_getBool(GENERATE_HTMLHELP) &&
      !Config_getString(HHC_LOCATION).isEmpty())
  {
    g_s.begin("Running html help compiler...\n");
    runHtmlHelpCompiler();
    g_s.end();
  }

  if ( generateHtml &&
       Config_getBool(GENERATE_QHP) &&
      !Config_getString(QHG_LOCATION).isEmpty())
  {
    g_s.begin("Running qhelpgenerator...\n");
    runQHelpGenerator();
    g_s.end();
  }

  g_outputList->cleanup();

  msg("type lookup cache used %zu/%zu hits=%" PRIu64 " misses=%" PRIu64 "\n",
      Doxygen::typeLookupCache->size(),
      Doxygen::typeLookupCache->capacity(),
      Doxygen::typeLookupCache->hits(),
      Doxygen::typeLookupCache->misses());
  msg("symbol lookup cache used %zu/%zu hits=%" PRIu64 " misses=%" PRIu64 "\n",
      Doxygen::symbolLookupCache->size(),
      Doxygen::symbolLookupCache->capacity(),
      Doxygen::symbolLookupCache->hits(),
      Doxygen::symbolLookupCache->misses());
  int typeCacheParam   = computeIdealCacheParam(static_cast<size_t>(Doxygen::typeLookupCache->misses()*2/3)); // part of the cache is flushed, hence the 2/3 correction factor
  int symbolCacheParam = computeIdealCacheParam(static_cast<size_t>(Doxygen::symbolLookupCache->misses()));
  int cacheParam = std::max(typeCacheParam,symbolCacheParam);
  if (cacheParam>Config_getInt(LOOKUP_CACHE_SIZE))
  {
    msg("Note: based on cache misses the ideal setting for LOOKUP_CACHE_SIZE is %d at the cost of higher memory usage.\n",cacheParam);
  }

  if (Debug::isFlagSet(Debug::Time))
  {

    std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
    if (numThreads<1) numThreads=1;
    msg("Total elapsed time: %.6f seconds\n(of which an average of %.6f seconds per thread waiting for external tools to finish)\n",
         (static_cast<double>(Debug::elapsedTime())),
         Portable::getSysElapsedTime()/static_cast<double>(numThreads)
        );
    g_s.print();

    Debug::clearFlag(Debug::Time);
    msg("finished...\n");
    Debug::setFlag(Debug::Time);
  }
  else
  {
    msg("finished...\n");
  }


  /**************************************************************************
   *                        Start cleaning up                               *
   **************************************************************************/

  cleanUpDoxygen();

  finalizeSearchIndexer();
  Dir thisDir;
  thisDir.remove(Doxygen::filterDBFileName.str());
  finishWarnExit();
  exitTracing();
  Config::deinit();
  delete Doxygen::clangUsrMap;
  g_successfulRun=TRUE;

  //dumpDocNodeSizes();
}

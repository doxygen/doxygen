/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstrlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <qtextcodec.h>
#include <unistd.h>
#include <errno.h>
#include <qptrdict.h>

#include "version.h"
#include "doxygen.h"
#include "scanner.h"
#include "entry.h"
#include "index.h"
#include "logos.h"
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
#include "ftvhelp.h"
#include "defargs.h"
#include "rtfgen.h"
#include "xmlgen.h"
#include "defgen.h"
#include "perlmodgen.h"
#include "reflist.h"
#include "pagedef.h"
#include "bufstr.h"
#include "commentcnv.h"
#include "cmdmapper.h"
#include "searchindex.h"
#include "parserintf.h"
#include "htags.h"
#include "pyscanner.h"
#include "fortranscanner.h"
#include "dbusxmlscanner.h"
#include "tclscanner.h"
#include "code.h"
#include "objcache.h"
#include "store.h"
#include "marshal.h"
#include "portable.h"
#include "vhdlscanner.h"
#include "vhdldocgen.h"
#include "eclipsehelp.h"
#include "cite.h"
#include "filestorage.h"
#include "markdown.h"
#include "arguments.h"

#include "layout.h"

#define RECURSE_ENTRYTREE(func,var) \
  do { if (var->children()) { \
    EntryNavListIterator eli(*var->children()); \
    for (;eli.current();++eli) func(eli.current()); \
  } } while(0) 


#if !defined(_WIN32) || defined(__CYGWIN__)
#include <signal.h>
#define HAS_SIGNALS
#endif

// globally accessible variables
ClassSDict      *Doxygen::classSDict = 0;
ClassSDict      *Doxygen::hiddenClasses = 0;
NamespaceSDict  *Doxygen::namespaceSDict = 0;
MemberNameSDict *Doxygen::memberNameSDict = 0;
MemberNameSDict *Doxygen::functionNameSDict = 0;   
FileNameList    *Doxygen::inputNameList = 0;       // all input files
FileNameDict    *Doxygen::inputNameDict = 0;          
GroupSDict      *Doxygen::groupSDict = 0;
FormulaList      Doxygen::formulaList;             // all formulas
FormulaDict      Doxygen::formulaDict(1009);       // all formulas
FormulaDict      Doxygen::formulaNameDict(1009);   // the label name of all formulas
PageSDict       *Doxygen::pageSDict = 0;
PageSDict       *Doxygen::exampleSDict = 0;
SectionDict      Doxygen::sectionDict(257);        // all page sections
CiteDict        *Doxygen::citeDict=0;              // database of bibliographic references
StringDict       Doxygen::aliasDict(257);          // aliases
FileNameDict    *Doxygen::includeNameDict = 0;     // include names
FileNameDict    *Doxygen::exampleNameDict = 0;     // examples
FileNameDict    *Doxygen::imageNameDict = 0;       // images
FileNameDict    *Doxygen::dotFileNameDict = 0;     // dot files
FileNameDict    *Doxygen::mscFileNameDict = 0;     // dot files
StringDict       Doxygen::namespaceAliasDict(257); // all namespace aliases
StringDict       Doxygen::tagDestinationDict(257); // all tag locations
QDict<void>      Doxygen::expandAsDefinedDict(257); // all macros that should be expanded
QIntDict<MemberGroupInfo> Doxygen::memGrpInfoDict(1009); // dictionary of the member groups heading
PageDef         *Doxygen::mainPage = 0;
bool             Doxygen::insideMainPage = FALSE; // are we generating docs for the main page?
FTextStream      Doxygen::tagFile;
NamespaceDef    *Doxygen::globalScope = 0;
QDict<RefList>  *Doxygen::xrefLists = new QDict<RefList>; // dictionary of cross-referenced item lists
bool             Doxygen::parseSourcesNeeded = FALSE;
QTime            Doxygen::runningTime;
SearchIndexIntf *Doxygen::searchIndex=0;
QDict<DefinitionIntf> *Doxygen::symbolMap;
bool             Doxygen::outputToWizard=FALSE;
QDict<int> *     Doxygen::htmlDirMap = 0;
QCache<LookupInfo> *Doxygen::lookupCache;
DirSDict        *Doxygen::directories;
SDict<DirRelation> Doxygen::dirRelations(257);
ParserManager   *Doxygen::parserManager = 0;
QCString Doxygen::htmlFileExtension;
bool             Doxygen::suppressDocWarnings = FALSE;
ObjCache        *Doxygen::symbolCache = 0;
Store           *Doxygen::symbolStorage;
QCString         Doxygen::objDBFileName;
QCString         Doxygen::entryDBFileName;
bool             Doxygen::gatherDefines = TRUE;
IndexList        Doxygen::indexList;
int              Doxygen::subpageNestingLevel = 0;
bool             Doxygen::userComments = FALSE;
QCString         Doxygen::spaces;
bool             Doxygen::generatingXmlOutput = FALSE;
bool             Doxygen::markdownSupport = TRUE;
GenericsSDict    Doxygen::genericsDict;

// locally accessible globals
static QDict<EntryNav>  g_classEntries(1009);
static StringList       g_inputFiles;         
static QDict<void>      g_compoundKeywordDict(7);  // keywords recognised as compounds
static OutputList      *g_outputList = 0;          // list of output generating objects
static QDict<FileDef>   g_usingDeclarations(1009); // used classes
static FileStorage     *g_storage = 0;
static bool             g_successfulRun = FALSE;
static bool             g_dumpSymbolMap = FALSE;
static bool             g_dumpConfigAsXML = FALSE;

void clearAll()
{
  g_inputFiles.clear();      
  //g_excludeNameDict.clear();  
  //delete g_outputList; g_outputList=0;

  Doxygen::classSDict->clear();       
  Doxygen::namespaceSDict->clear();   
  Doxygen::pageSDict->clear();         
  Doxygen::exampleSDict->clear();      
  Doxygen::inputNameList->clear();   
  Doxygen::formulaList.clear();     
  Doxygen::sectionDict.clear();       
  Doxygen::inputNameDict->clear();    
  Doxygen::includeNameDict->clear();  
  Doxygen::exampleNameDict->clear();  
  Doxygen::imageNameDict->clear();     
  Doxygen::dotFileNameDict->clear();     
  Doxygen::mscFileNameDict->clear();     
  Doxygen::formulaDict.clear();      
  Doxygen::formulaNameDict.clear();  
  Doxygen::tagDestinationDict.clear();
  delete Doxygen::citeDict;
  delete Doxygen::mainPage; Doxygen::mainPage=0;
}

void statistics()
{
  fprintf(stderr,"--- inputNameDict stats ----\n");
  Doxygen::inputNameDict->statistics();
  fprintf(stderr,"--- includeNameDict stats ----\n");
  Doxygen::includeNameDict->statistics();
  fprintf(stderr,"--- exampleNameDict stats ----\n");
  Doxygen::exampleNameDict->statistics();
  fprintf(stderr,"--- imageNameDict stats ----\n");
  Doxygen::imageNameDict->statistics();
  fprintf(stderr,"--- dotFileNameDict stats ----\n");
  Doxygen::dotFileNameDict->statistics();
  fprintf(stderr,"--- mscFileNameDict stats ----\n");
  Doxygen::mscFileNameDict->statistics();
  //fprintf(stderr,"--- g_excludeNameDict stats ----\n");
  //g_excludeNameDict.statistics();
  fprintf(stderr,"--- aliasDict stats ----\n");
  Doxygen::aliasDict.statistics();
  fprintf(stderr,"--- typedefDict stats ----\n");
  fprintf(stderr,"--- namespaceAliasDict stats ----\n");
  Doxygen::namespaceAliasDict.statistics();
  fprintf(stderr,"--- formulaDict stats ----\n");
  Doxygen::formulaDict.statistics();
  fprintf(stderr,"--- formulaNameDict stats ----\n");
  Doxygen::formulaNameDict.statistics();
  fprintf(stderr,"--- tagDestinationDict stats ----\n");
  Doxygen::tagDestinationDict.statistics();
  fprintf(stderr,"--- g_compoundKeywordDict stats ----\n");
  g_compoundKeywordDict.statistics();
  fprintf(stderr,"--- expandAsDefinedDict stats ----\n");
  Doxygen::expandAsDefinedDict.statistics();
  fprintf(stderr,"--- memGrpInfoDict stats ----\n");
  Doxygen::memGrpInfoDict.statistics();
}



static void addMemberDocs(EntryNav *rootNav,MemberDef *md, const char *funcDecl,
                   ArgumentList *al,bool over_load,NamespaceSDict *nl=0);
static void findMember(EntryNav *rootNav,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      );

/** A struct contained the data for an STL class */
struct STLInfo
{
  const char *className;
  const char *baseClass1;
  const char *baseClass2;
  const char *templType1;
  const char *templName1;
  const char *templType2;
  const char *templName2;
  bool virtualInheritance;
  bool iterators;
};

static STLInfo g_stlinfo[] =
{
  // className              baseClass1                      baseClass2             templType1     templName1     templType2    templName2     virtInheritance  // iterators
  { "allocator",            0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "array",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE }, // C++11
  { "auto_ptr",             0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // deprecated
  { "smart_ptr",            0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "unique_ptr",           0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "weak_ptr",             0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "ios_base",             0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_code",           0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_category",       0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "system_error",         0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_condition",      0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "thread",               0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "basic_ios",            "ios_base",                     0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_istream",        "basic_ios<Char>",              0,                     "Char",        0,             0,            0,             TRUE,               FALSE },
  { "basic_ostream",        "basic_ios<Char>",              0,                     "Char",        0,             0,            0,             TRUE,               FALSE },
  { "basic_iostream",       "basic_istream<Char>",          "basic_ostream<Char>", "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ifstream",       "basic_istream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ofstream",       "basic_ostream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_fstream",        "basic_iostream<Char>",         0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_istringstream",  "basic_istream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ostringstream",  "basic_ostream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_stringstream",   "basic_iostream<Char>",         0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "ios",                  "basic_ios<char>",              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wios",                 "basic_ios<wchar_t>",           0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "istream",              "basic_istream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wistream",             "basic_istream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ostream",              "basic_ostream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wostream",             "basic_ostream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ifstream",             "basic_ifstream<char>",         0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wifstream",            "basic_ifstream<wchar_t>",      0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ofstream",             "basic_ofstream<char>",         0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wofstream",            "basic_ofstream<wchar_t>",      0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "fstream",              "basic_fstream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wfstream",             "basic_fstream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "istringstream",        "basic_istringstream<char>",    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wistringstream",       "basic_istringstream<wchar_t>", 0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ostringstream",        "basic_ostringstream<char>",    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wostringstream",       "basic_ostringstream<wchar_t>", 0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "stringstream",         "basic_stringstream<char>",     0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wstringstream",        "basic_stringstream<wchar_t>",  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "basic_string",         0,                              0,                     "Char",        0,             0,            0,             FALSE,              TRUE  },
  { "string",               "basic_string<char>",           0,                     0,             0,             0,            0,             FALSE,              TRUE  },
  { "wstring",              "basic_string<wchar_t>",        0,                     0,             0,             0,            0,             FALSE,              TRUE  },
  { "complex",              0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bitset",               0,                              0,                     "Bits",        0,             0,            0,             FALSE,              FALSE },
  { "deque",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "list",                 0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "forward_list",         0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  }, // C++11
  { "map",                  0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  },
  { "unordered_map",        0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  }, // C++11
  { "multimap",             0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  },
  { "unordered_multimap",   0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  }, // C++11
  { "set",                  0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  },
  { "unordered_set",        0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  }, // C++11
  { "multiset",             0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  },
  { "unordered_multiset",   0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  }, // C++11
  { "vector",               0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "queue",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "priority_queue",       0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "stack",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "valarray",             0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "exception",            0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_alloc",            "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_cast",             "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_typeid",           "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "logic_error",          "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ios_base::failure",    "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "runtime_error",        "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_exception",        "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "domain_error",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "invalid_argument",     "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "length_error",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "out_of_range",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "range_error",          "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "overflow_error",       "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "underflow_error",      "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { 0,                      0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }
};

static void addSTLMember(EntryNav *rootNav,const char *type,const char *name)
{
  Entry *memEntry = new Entry;
  memEntry->name       = name;
  memEntry->type       = type;
  memEntry->protection = Public;
  memEntry->section    = Entry::VARIABLE_SEC;
  memEntry->brief      = "STL member";
  memEntry->hidden     = FALSE; 
  memEntry->artificial = TRUE;
  //memEntry->parent     = root;
  //root->addSubEntry(memEntry);
  EntryNav *memEntryNav = new EntryNav(rootNav,memEntry);
  memEntryNav->setEntry(memEntry);
  rootNav->addChild(memEntryNav);
}

static void addSTLIterator(EntryNav *classEntryNav,const char *name)
{
  Entry *iteratorClassEntry = new Entry;
  iteratorClassEntry->fileName  = "[STL]";
  iteratorClassEntry->startLine = 1;
  iteratorClassEntry->name      = name;
  iteratorClassEntry->section   = Entry::CLASS_SEC;
  iteratorClassEntry->brief     = "STL iterator class";
  iteratorClassEntry->hidden    = FALSE;
  iteratorClassEntry->artificial= TRUE;
  EntryNav *iteratorClassEntryNav = new EntryNav(classEntryNav,iteratorClassEntry);
  iteratorClassEntryNav->setEntry(iteratorClassEntry);
  classEntryNav->addChild(iteratorClassEntryNav);
}


static void addSTLClasses(EntryNav *rootNav)
{
  Entry *namespaceEntry = new Entry;
  namespaceEntry->fileName  = "[STL]";
  namespaceEntry->startLine = 1;
  //namespaceEntry->parent    = rootNav->entry();
  namespaceEntry->name      = "std";
  namespaceEntry->section   = Entry::NAMESPACE_SEC;
  namespaceEntry->brief     = "STL namespace";
  namespaceEntry->hidden    = FALSE;
  namespaceEntry->artificial= TRUE;
  //root->addSubEntry(namespaceEntry);
  EntryNav *namespaceEntryNav = new EntryNav(rootNav,namespaceEntry);
  namespaceEntryNav->setEntry(namespaceEntry);
  rootNav->addChild(namespaceEntryNav);
  
  STLInfo *info = g_stlinfo;
  while (info->className)
  {
    //printf("Adding STL class %s\n",info->className);
    QCString fullName = info->className;
    fullName.prepend("std::");

    // add fake Entry for the class
    Entry *classEntry = new Entry;
    classEntry->fileName  = "[STL]";
    classEntry->startLine = 1;
    classEntry->name      = fullName;
    //classEntry->parent    = namespaceEntry;
    classEntry->section   = Entry::CLASS_SEC;
    classEntry->brief     = "STL class";
    classEntry->hidden    = FALSE;
    classEntry->artificial= TRUE;
    //namespaceEntry->addSubEntry(classEntry);
    EntryNav *classEntryNav = new EntryNav(namespaceEntryNav,classEntry);
    classEntryNav->setEntry(classEntry);
    namespaceEntryNav->addChild(classEntryNav);

    // add template arguments to class
    if (info->templType1)
    {
      ArgumentList *al = new ArgumentList;
      Argument *a=new Argument;
      a->type="typename";
      a->name=info->templType1;
      al->append(a);
      if (info->templType2) // another template argument
      {
        a=new Argument;
        a->type="typename";
        a->name=info->templType2;
        al->append(a);
      }
      classEntry->tArgLists = new QList<ArgumentList>;
      classEntry->tArgLists->setAutoDelete(TRUE);
      classEntry->tArgLists->append(al);
    }
    // add member variables
    if (info->templName1)
    {
      addSTLMember(classEntryNav,info->templType1,info->templName1);
    }
    if (info->templName2)
    {
      addSTLMember(classEntryNav,info->templType2,info->templName2);
    }
    if (fullName=="std::auto_ptr" || fullName=="std::smart_ptr" ||
        fullName=="std::unique_ptr" || fullName=="std::weak_ptr")
    {
      Entry *memEntry = new Entry;
      memEntry->name       = "operator->";
      memEntry->args       = "()";
      memEntry->type       = "T*";
      memEntry->protection = Public;
      memEntry->section    = Entry::FUNCTION_SEC;
      memEntry->brief      = "STL member";
      memEntry->hidden     = FALSE; 
      memEntry->artificial = FALSE;
      EntryNav *memEntryNav = new EntryNav(classEntryNav,memEntry);
      memEntryNav->setEntry(memEntry);
      classEntryNav->addChild(memEntryNav);
    }
    if (info->baseClass1)
    {
      classEntry->extends->append(new BaseInfo(info->baseClass1,Public,info->virtualInheritance?Virtual:Normal));
    }
    if (info->baseClass2)
    {
      classEntry->extends->append(new BaseInfo(info->baseClass2,Public,info->virtualInheritance?Virtual:Normal));
    }
    if (info->iterators)
    {
      // add iterator class
      addSTLIterator(classEntryNav,fullName+"::iterator");
      addSTLIterator(classEntryNav,fullName+"::const_iterator");
      addSTLIterator(classEntryNav,fullName+"::reverse_iterator");
      addSTLIterator(classEntryNav,fullName+"::const_reverse_iterator");
    }
    info++;
  }
}

//----------------------------------------------------------------------------

static Definition *findScopeFromQualifiedName(Definition *startScope,const QCString &n,
                                              FileDef *fileScope=0);

static void addPageToContext(PageDef *pd,EntryNav *rootNav)
{
  if (rootNav->parent()) // add the page to it's scope
  {
    QCString scope = rootNav->parent()->name();
    if (rootNav->parent()->section()==Entry::PACKAGEDOC_SEC)
    {
      scope=substitute(scope,".","::");
    }
    scope = stripAnonymousNamespaceScope(scope);
    scope+="::"+pd->name();
    Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,scope);
    if (d) 
    {
      pd->setPageScope(d);
    }
  }
}

static void addRelatedPage(EntryNav *rootNav)
{
  Entry *root = rootNav->entry();
  GroupDef *gd=0;
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname))) break;
  }
  //printf("---> addRelatedPage() %s gd=%p\n",root->name.data(),gd);
  QCString doc;
  if (root->brief.isEmpty())
  {
    doc=root->doc+root->inbodyDocs;
  }
  else
  {
    doc=root->brief+"\n\n"+root->doc+root->inbodyDocs;
  }
  PageDef *pd = addRelatedPage(root->name,root->args,doc,root->anchors,
      root->docFile,root->docLine,
      root->sli,
      gd,rootNav->tagInfo(),
      root->lang
     );
  if (pd)
  {
    pd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    pd->addSectionsToDefinition(root->anchors);
    pd->setShowToc(root->stat);
    addPageToContext(pd,rootNav);
  }
}

static void buildGroupListFiltered(EntryNav *rootNav,bool additional, bool includeExternal)
{
  if (rootNav->section()==Entry::GROUPDOC_SEC && !rootNav->name().isEmpty() && 
        ((!includeExternal && rootNav->tagInfo()==0) ||
         ( includeExternal && rootNav->tagInfo()!=0))
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL &&  additional))
    {
      GroupDef *gd = Doxygen::groupSDict->find(root->name);
      //printf("Processing group '%s':'%s' add=%d ext=%d gd=%p\n",
      //    root->type.data(),root->name.data(),additional,includeExternal,gd);

      if (gd)
      {
        if ( !gd->hasGroupTitle() )
        {
          gd->setGroupTitle( root->type );
        }
        else if ( root->type.length() > 0 && root->name != root->type && gd->groupTitle() != root->type )
        {
          warn( root->fileName,root->startLine,
              "group %s: ignoring title \"%s\" that does not match old title \"%s\"\n",
              qPrint(root->name), qPrint(root->type), qPrint(gd->groupTitle()) );
        }
        gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        gd->setDocumentation( root->doc, root->docFile, root->docLine );
        gd->setInbodyDocumentation( root->inbodyDocs, root->inbodyFile, root->inbodyLine );
        gd->addSectionsToDefinition(root->anchors);
        gd->setRefItems(root->sli);
        gd->setLanguage(root->lang);
      }
      else
      {
        if (rootNav->tagInfo())
        {
          gd = new GroupDef(root->fileName,root->startLine,root->name,root->type,rootNav->tagInfo()->fileName);
          gd->setReference(rootNav->tagInfo()->tagName);
        }
        else
        {
          gd = new GroupDef(root->fileName,root->startLine,root->name,root->type);
        }
        gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        // allow empty docs for group
        gd->setDocumentation(!root->doc.isEmpty() ? root->doc : QCString(" "),root->docFile,root->docLine,FALSE);
        gd->setInbodyDocumentation( root->inbodyDocs, root->inbodyFile, root->inbodyLine );
        gd->addSectionsToDefinition(root->anchors);
        Doxygen::groupSDict->append(root->name,gd);
        gd->setRefItems(root->sli);
        gd->setLanguage(root->lang);
      }
    }

    rootNav->releaseEntry();
  }
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *e;
    for (;(e=eli.current());++eli)
    {
      buildGroupListFiltered(e,additional,includeExternal);
    }
  }
}

static void buildGroupList(EntryNav *rootNav)
{
  // --- first process only local groups
  // first process the @defgroups blocks
  buildGroupListFiltered(rootNav,FALSE,FALSE);
  // then process the @addtogroup, @weakgroup blocks
  buildGroupListFiltered(rootNav,TRUE,FALSE);

  // --- then also process external groups
  // first process the @defgroups blocks
  buildGroupListFiltered(rootNav,FALSE,TRUE);
  // then process the @addtogroup, @weakgroup blocks
  buildGroupListFiltered(rootNav,TRUE,TRUE);
}

static void findGroupScope(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::GROUPDOC_SEC && !rootNav->name().isEmpty() && 
      rootNav->parent() && !rootNav->parent()->name().isEmpty())
  {
    GroupDef *gd;
    if ((gd=Doxygen::groupSDict->find(rootNav->name())))
    {
      QCString scope = rootNav->parent()->name();
      if (rootNav->parent()->section()==Entry::PACKAGEDOC_SEC)
      {
        scope=substitute(scope,".","::");
      }
      scope = stripAnonymousNamespaceScope(scope);
      scope+="::"+gd->name();
      Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,scope);
      if (d) 
      {
        gd->setGroupScope(d);
      }
    }
  }
  RECURSE_ENTRYTREE(findGroupScope,rootNav);
}

static void organizeSubGroupsFiltered(EntryNav *rootNav,bool additional)
{
  if (rootNav->section()==Entry::GROUPDOC_SEC && !rootNav->name().isEmpty())
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL && additional))
    {
      GroupDef *gd;
      if ((gd=Doxygen::groupSDict->find(root->name)))
      {
        //printf("adding %s to group %s\n",root->name.data(),gd->name().data());
        addGroupToGroups(root,gd);
      }
    }

    rootNav->releaseEntry();
  }
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *e;
    for (;(e=eli.current());++eli)
    {
      organizeSubGroupsFiltered(e,additional);
    }
  }
}

static void organizeSubGroups(EntryNav *rootNav)
{
  //printf("Defining groups\n");
  // first process the @defgroups blocks
  organizeSubGroupsFiltered(rootNav,FALSE);
  //printf("Additional groups\n");
  // then process the @addtogroup, @weakgroup blocks
  organizeSubGroupsFiltered(rootNav,TRUE);
}

//----------------------------------------------------------------------

static void buildFileList(EntryNav *rootNav)
{
  if (((rootNav->section()==Entry::FILEDOC_SEC) ||
        ((rootNav->section() & Entry::FILE_MASK) && Config_getBool("EXTRACT_ALL"))) &&
      !rootNav->name().isEmpty() && !rootNav->tagInfo() // skip any file coming from tag files
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    bool ambig;
    FileDef *fd=findFileDef(Doxygen::inputNameDict,root->name,ambig);
    //printf("**************** root->name=%s fd=%p\n",root->name.data(),fd);
    if (fd && !ambig)
    {
#if 0
      if ((!root->doc.isEmpty() && !fd->documentation().isEmpty()) ||
          (!root->brief.isEmpty() && !fd->briefDescription().isEmpty()))
      {
        warn(
            root->fileName,root->startLine,
            "warning: file %s already documented. "
            "Skipping documentation.",
            root->name.data()
            );
      }
      else
#endif
      {
        //printf("Adding documentation!\n");
        // using FALSE in setDocumentation is small hack to make sure a file 
        // is documented even if a \file command is used without further 
        // documentation
        fd->setDocumentation(root->doc,root->docFile,root->docLine,FALSE);
        fd->setBriefDescription(root->brief,root->briefFile,root->briefLine); 
        fd->addSectionsToDefinition(root->anchors);
        fd->setRefItems(root->sli);
        QListIterator<Grouping> gli(*root->groups);
        Grouping *g;
        for (;(g=gli.current());++gli)
        {
          GroupDef *gd=0;
          if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
          {
            gd->addFile(fd);
            fd->makePartOfGroup(gd);
            //printf("File %s: in group %s\n",fd->name().data(),s->data());
          }
        }
      }
    }
    else
    {
      const char *fn = root->fileName.data();
      QCString text(4096);
      text.sprintf("warning: the name `%s' supplied as "
          "the second argument in the \\file statement ",
          qPrint(root->name));
      if (ambig) // name is ambiguous
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameDict,root->name);
        text+="Please use a more specific name by "
          "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(fn,root->startLine,text);
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildFileList,rootNav);
}

static void addIncludeFile(ClassDef *cd,FileDef *ifd,Entry *root)
{
  if ( 
      (!root->doc.stripWhiteSpace().isEmpty() || 
       !root->brief.stripWhiteSpace().isEmpty() || 
       Config_getBool("EXTRACT_ALL")
      ) && root->protection!=Private
     )
  { 
    //printf(">>>>>> includeFile=%s\n",root->includeFile.data());

    bool local=Config_getBool("FORCE_LOCAL_INCLUDES");
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

    bool ambig;
    FileDef *fd=0;
    // see if we need to include a verbatim copy of the header file
    //printf("root->includeFile=%s\n",root->includeFile.data());
    if (!includeFile.isEmpty() && 
        (fd=findFileDef(Doxygen::inputNameDict,includeFile,ambig))==0
       )
    { // explicit request
      QCString text;
      text.sprintf("warning: the name `%s' supplied as "
                  "the argument of the \\class, \\struct, \\union, or \\include command ",
                  qPrint(includeFile)
                 );
      if (ambig) // name is ambiguous
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameDict,root->includeFile);
        text+="Please use a more specific name by "
            "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(root->fileName,root->startLine,text);
    }
    else if (includeFile.isEmpty() && ifd &&
        // see if the file extension makes sense
        guessSection(ifd->name())==Entry::HEADER_SEC)
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
      else if (!Config_getList("STRIP_FROM_INC_PATH").isEmpty()) 
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
        cd->setIncludeFile(0,iName,local,TRUE);
      }
    }
  }
}

#if 0
static bool addNamespace(Entry *root,ClassDef *cd)
{
  // see if this class is defined inside a namespace
  if (root->section & Entry::COMPOUND_MASK)
  {
    Entry *e = root->parent;
    while (e)
    {
      if (e->section==Entry::NAMESPACE_SEC)
      {
        NamespaceDef *nd=0;
        QCString nsName = stripAnonymousNamespaceScope(e->name);
        //printf("addNameSpace() trying: %s\n",nsName.data());
        if (!nsName.isEmpty() && nsName.at(0)!='@' &&
            (nd=getResolvedNamespace(nsName))
           )
        {
          cd->setNamespace(nd);
          cd->setOuterScope(nd);
          nd->insertClass(cd);
          return TRUE;
        }
      }
      e=e->parent;
    } 
  }
  return FALSE;
}
#endif

#if 0
static Definition *findScope(Entry *root,int level=0)
{
  if (root==0) return 0;
  //printf("start findScope name=%s\n",root->name.data());
  Definition *result=0;
  if (root->section&Entry::SCOPE_MASK)
  {
    result = findScope(root->parent,level+1); // traverse to the root of the tree
    if (result)
    {
      //printf("Found %s inside %s at level %d\n",root->name.data(),result->name().data(),level);
      // TODO: look at template arguments
      result = result->findInnerCompound(root->name);
    }
    else // reached the global scope
    {
      // TODO: look at template arguments
      result = Doxygen::globalScope->findInnerCompound(root->name);
      //printf("Found in globalScope %s at level %d\n",result->name().data(),level);
    }
  }
  //printf("end findScope(%s,%d)=%s\n",root->name.data(),
  //       level,result==0 ? "<none>" : result->name().data());
  return result;
}
#endif

/*! returns the Definition object belonging to the first \a level levels of 
 *  full qualified name \a name. Creates an artificial scope if the scope is
 *  not found and set the parent/child scope relation if the scope is found.
 */
static Definition *buildScopeFromQualifiedName(const QCString name,int level,SrcLangExt lang)
{
  int i=0;
  int p=0,l;
  Definition *prevScope=Doxygen::globalScope;
  QCString fullScope;
  while (i<level)
  {
    int idx=getScopeFragment(name,p,&l);
    QCString nsName = name.mid(idx,l);
    if (nsName.isEmpty()) return prevScope;
    if (!fullScope.isEmpty()) fullScope+="::";
    fullScope+=nsName;
    NamespaceDef *nd=Doxygen::namespaceSDict->find(fullScope);
    Definition *innerScope = nd;
    ClassDef *cd=0; 
    if (nd==0) cd = getClass(fullScope);
    if (nd==0 && cd) // scope is a class
    {
      innerScope = cd;
    }
    else if (nd==0 && cd==0) // scope is not known!
    {
      // introduce bogus namespace
      //printf("++ adding dummy namespace %s to %s\n",nsName.data(),prevScope->name().data());
      nd=new NamespaceDef(
        "[generated]",1,fullScope);
      nd->setLanguage(lang);

      // add namespace to the list
      Doxygen::namespaceSDict->inSort(fullScope,nd);
      innerScope = nd;
    }
    else // scope is a namespace
    {
    }
    // make the parent/child scope relation
    prevScope->addInnerCompound(innerScope);
    innerScope->setOuterScope(prevScope);
    // proceed to the next scope fragment
    p=idx+l+2;
    prevScope=innerScope;
    i++;
  }
  return prevScope;
}

static Definition *findScopeFromQualifiedName(Definition *startScope,const QCString &n,
                                              FileDef *fileScope)
{
  //printf("<findScopeFromQualifiedName(%s,%s)\n",startScope ? startScope->name().data() : 0, n.data());
  Definition *resultScope=startScope;
  if (resultScope==0) resultScope=Doxygen::globalScope;
  QCString scope=stripTemplateSpecifiersFromScope(n,FALSE);
  int l1=0,i1;
  i1=getScopeFragment(scope,0,&l1);
  if (i1==-1) 
  {
    //printf(">no fragments!\n");
    return resultScope;
  }
  int p=i1+l1,l2=0,i2;
  while ((i2=getScopeFragment(scope,p,&l2))!=-1)
  {
    QCString nestedNameSpecifier = scope.mid(i1,l1);
    Definition *orgScope = resultScope;
    //printf("  nestedNameSpecifier=%s\n",nestedNameSpecifier.data());
    resultScope = resultScope->findInnerCompound(nestedNameSpecifier);
    //printf("  resultScope=%p\n",resultScope);
    if (resultScope==0) 
    {
      NamespaceSDict *usedNamespaces;
      if (orgScope==Doxygen::globalScope && fileScope &&
          (usedNamespaces = fileScope->getUsedNamespaces())) 
        // also search for used namespaces 
      {
        NamespaceSDict::Iterator ni(*usedNamespaces);
        NamespaceDef *nd;
        for (ni.toFirst();((nd=ni.current()) && resultScope==0);++ni)
        {
          // restart search within the used namespace
          resultScope = findScopeFromQualifiedName(nd,n,fileScope);
        }
        if (resultScope) 
        {
          // for a nested class A::I in used namespace N, we get
          // N::A::I while looking for A, so we should compare
          // resultScope->name() against scope.left(i2+l2)
          //printf("  -> result=%s scope=%s\n",resultScope->name().data(),scope.data());
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
      QDictIterator<FileDef> ui(g_usingDeclarations);
      FileDef *usedFd;
      for (ui.toFirst();(usedFd=ui.current());++ui)
      {
        //printf("Checking using class %s\n",ui.currentKey());
        if (rightScopeMatch(ui.currentKey(),nestedNameSpecifier))
        {
          // ui.currentKey() is the fully qualified name of nestedNameSpecifier
          // so use this instead.
          QCString fqn = QCString(ui.currentKey())+
                         scope.right(scope.length()-p);
          resultScope = buildScopeFromQualifiedName(fqn,fqn.contains("::"),startScope->getLanguage());
          //printf("Creating scope from fqn=%s result %p\n",fqn.data(),resultScope);
          if (resultScope) 
          {
            //printf("> Match! resultScope=%s\n",resultScope->name().data());
            return resultScope;
          }
        }
      }
      
      //printf("> name %s not found in scope %s\n",nestedNameSpecifier.data(),orgScope->name().data());
      return 0;
    }
 nextFragment:
    i1=i2;
    l1=l2;
    p=i2+l2;
  }
  //printf(">findScopeFromQualifiedName scope %s\n",resultScope->name().data());
  return resultScope;
}

ArgumentList *getTemplateArgumentsFromName(
                  const QCString &name,
                  const QList<ArgumentList> *tArgLists)
{
  if (tArgLists==0) return 0;
  
  QListIterator<ArgumentList> ali(*tArgLists);
  // for each scope fragment, check if it is a template and advance through
  // the list if so.
  int i,p=0;
  while ((i=name.find("::",p))!=-1)
  {
    NamespaceDef *nd = Doxygen::namespaceSDict->find(name.left(i));
    if (nd==0)
    {
      ClassDef *cd = getClass(name.left(i));
      if (cd)
      {
        if (cd->templateArguments())
        {
          ++ali;
        }
      }
    }
    p=i+2;
  }
  return ali.current();
}

static ClassDef::CompoundType convertToCompoundType(int section,int specifier)
{
  ClassDef::CompoundType sec=ClassDef::Class; 
  if (specifier&Entry::Struct) 
    sec=ClassDef::Struct;
  else if (specifier&Entry::Union) 
    sec=ClassDef::Union;
  else if (specifier&Entry::Category) 
    sec=ClassDef::Category;
  else if (specifier&Entry::Interface) 
    sec=ClassDef::Interface;
  else if (specifier&Entry::Protocol) 
    sec=ClassDef::Protocol;
  else if (specifier&Entry::Exception) 
    sec=ClassDef::Exception;

  switch(section)
  {
    //case Entry::UNION_SEC: 
    case Entry::UNIONDOC_SEC: 
      sec=ClassDef::Union; 
      break;
      //case Entry::STRUCT_SEC:
    case Entry::STRUCTDOC_SEC: 
      sec=ClassDef::Struct; 
      break;
      //case Entry::INTERFACE_SEC:
    case Entry::INTERFACEDOC_SEC:
      sec=ClassDef::Interface; 
      break;
      //case Entry::PROTOCOL_SEC:
    case Entry::PROTOCOLDOC_SEC:
      sec=ClassDef::Protocol; 
      break;
      //case Entry::CATEGORY_SEC:
    case Entry::CATEGORYDOC_SEC:
      sec=ClassDef::Category; 
      break;
      //case Entry::EXCEPTION_SEC:
    case Entry::EXCEPTIONDOC_SEC:
      sec=ClassDef::Exception; 
      break;
  }
  return sec;
}


static void addClassToContext(EntryNav *rootNav)
{
  //printf("Loading entry for rootNav=%p name=%s\n",rootNav,rootNav->name().data());
  rootNav->loadEntry(g_storage);
  Entry *root = rootNav->entry();

  //NamespaceDef *nd = 0;
  FileDef *fd = rootNav->fileDef();

  QCString scName;
  if (rootNav->parent()->section()&Entry::SCOPE_MASK)
  {
     scName=rootNav->parent()->name();
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
  ClassDef *cd = getClass(qualifiedName);

  Debug::print(Debug::Classes,0, "  Found class with name %s (qualifiedName=%s -> cd=%p)\n",
      cd ? cd->name().data() : root->name.data(), qualifiedName.data(),cd);

  if (cd) 
  {
    fullName=cd->name();
    Debug::print(Debug::Classes,0,"  Existing class %s!\n",cd->name().data());
    //if (cd->templateArguments()==0)
    //{
    //  //printf("existing ClassDef tempArgList=%p specScope=%s\n",root->tArgList,root->scopeSpec.data());
    //  cd->setTemplateArguments(tArgList);
    //}

    cd->setDocumentation(root->doc,root->docFile,root->docLine);
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);

    if (root->bodyLine!=-1 && cd->getStartBodyLine()==-1)
    {
      cd->setBodySegment(root->bodyLine,root->endBodyLine);
      cd->setBodyDef(fd);
    }
    //cd->setName(fullName); // change name to match docs

    if (cd->templateArguments()==0) 
    {
      // this happens if a template class declared with @class is found
      // before the actual definition.
      ArgumentList *tArgList = 
        getTemplateArgumentsFromName(cd->name(),root->tArgLists);
      cd->setTemplateArguments(tArgList);
    }

    cd->setCompoundType(convertToCompoundType(root->section,root->spec));
  }
  else // new class
  {
    ClassDef::CompoundType sec = convertToCompoundType(root->section,root->spec);

    QCString className;
    QCString namespaceName;
    extractNamespaceName(fullName,className,namespaceName);

    //printf("New class: fullname %s namespace `%s' name=`%s' brief=`%s' docs=`%s'\n",
    //    fullName.data(),namespaceName.data(),className.data(),root->brief.data(),root->doc.data());

    QCString tagName;
    QCString refFileName;
    if (rootNav->tagInfo())
    {
      tagName     = rootNav->tagInfo()->tagName;
      refFileName = rootNav->tagInfo()->fileName;
    }
    cd=new ClassDef(root->fileName,root->startLine,fullName,sec,
        tagName,refFileName,TRUE,root->spec&Entry::Enum);
    Debug::print(Debug::Classes,0,"  New class `%s' (sec=0x%08x)! #tArgLists=%d\n",
        fullName.data(),sec,root->tArgLists ? (int)root->tArgLists->count() : -1);
    cd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    cd->setLanguage(root->lang);    
    cd->setHidden(root->hidden);        
    cd->setArtificial(root->artificial);        
    cd->setClassSpecifier(root->spec);
    cd->setTypeConstraints(root->typeConstr);   
    //printf("new ClassDef %s tempArgList=%p specScope=%s\n",fullName.data(),root->tArgList,root->scopeSpec.data());    

    ArgumentList *tArgList =    
      getTemplateArgumentsFromName(fullName,root->tArgLists);   
    //printf("class %s template args=%s\n",fullName.data(),     
    //    tArgList ? tempArgListToString(tArgList).data() : "<none>");          
    cd->setTemplateArguments(tArgList);         
    cd->setProtection(root->protection);        
    cd->setIsStatic(root->stat);        

    // file definition containing the class cd          
    cd->setBodySegment(root->bodyLine,root->endBodyLine);       
    cd->setBodyDef(fd);         

    // see if the class is found inside a namespace     
    //bool found=addNamespace(root,cd);         

    // the empty string test is needed for extract all case     
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);    
    cd->insertUsedFile(root->fileName);

    // add class to the list
    //printf("ClassDict.insert(%s)\n",resolveDefines(fullName).data());
    Doxygen::classSDict->append(fullName,cd);

    if (cd->isGeneric()) // generics are also stored in a separate dictionary for fast lookup of instantions
    {
      Doxygen::genericsDict.insert(fullName,cd);
    }
  }

  cd->addSectionsToDefinition(root->anchors);
  if (!root->subGrouping) cd->setSubGrouping(FALSE);
  if (cd->hasDocumentation())
  {
    addIncludeFile(cd,fd,root);
  }
  if (fd && (root->section & Entry::COMPOUND_MASK)) 
  {
    //printf(">> Inserting class `%s' in file `%s' (root->fileName=`%s')\n",
    //    cd->name().data(),
    //    fd->name().data(),
    //    root->fileName.data()
    //   );
    cd->setFileDef(fd);
    fd->insertClass(cd);
  }
  addClassToGroups(root,cd);
  cd->setRefItems(root->sli);

  rootNav->releaseEntry();
}
            
//----------------------------------------------------------------------
// build a list of all classes mentioned in the documentation
// and all classes that have a documentation block before their definition.
static void buildClassList(EntryNav *rootNav)
{
  if (
        ((rootNav->section() & Entry::COMPOUND_MASK) || 
         rootNav->section()==Entry::OBJCIMPL_SEC) && !rootNav->name().isEmpty()
     )
  {
    addClassToContext(rootNav);
  }
  RECURSE_ENTRYTREE(buildClassList,rootNav);
}

static void buildClassDocList(EntryNav *rootNav)
{
  if (
       (rootNav->section() & Entry::COMPOUNDDOC_MASK) && !rootNav->name().isEmpty()
     )
  {
    addClassToContext(rootNav);
  }
  RECURSE_ENTRYTREE(buildClassDocList,rootNav);
}

static void resolveClassNestingRelations()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;

  bool done=FALSE;
  int iteration=0;
  while (!done)
  {
    done=TRUE;
    ++iteration;
    ClassDef *cd=0;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      if (!cd->visited)
      {
        QCString name = stripAnonymousNamespaceScope(cd->name());
        //printf("processing=%s, iteration=%d\n",cd->name().data(),iteration);
        // also add class to the correct structural context 
        Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,
                                                 name,cd->getFileDef());
        if (d)
        {
          //printf("****** adding %s to scope %s in iteration %d\n",cd->name().data(),d->name().data(),iteration);
          d->addInnerCompound(cd);
          cd->setOuterScope(d);
          cd->visited=TRUE;
          done=FALSE;
        }
        //else
        //{
        //  printf("****** ignoring %s: scope not (yet) found in iteration %d\n",cd->name().data(),iteration);
        //}
      }
    }
  }

  //give warnings for unresolved compounds
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (!cd->visited)
    {
      QCString name = stripAnonymousNamespaceScope(cd->name());
      //printf("processing unresolved=%s, iteration=%d\n",cd->name().data(),iteration);
      /// create the scope artificially
      // anyway, so we can at least relate scopes properly.
      Definition *d = buildScopeFromQualifiedName(name,name.contains("::"),cd->getLanguage());
      if (d!=cd && !cd->getDefFileName().isEmpty()) 
                 // avoid recursion in case of redundant scopes, i.e: namespace N { class N::C {}; }
                 // for this case doxygen assumes the exitance of a namespace N::N in which C is to be found!
                 // also avoid warning for stuff imported via a tagfile.
      {
        d->addInnerCompound(cd);
        cd->setOuterScope(d);
        warn(cd->getDefFileName(),cd->getDefLine(),
            "warning: Internal inconsistency: scope for class %s not "
            "found!",name.data()
            );
      }
    }
  }
}

void distributeClassGroupRelations()
{
  //static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  //if (!inlineGroupedClasses) return;
  //printf("** distributeClassGroupRelations()\n");

  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;

  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    //printf("Checking %s\n",cd->name().data());
    // distribute the group to nested classes as well
    if (!cd->visited && cd->partOfGroups()!=0 && cd->getClassSDict())
    {
      //printf("  Candidate for merging\n");
      ClassSDict::Iterator ncli(*cd->getClassSDict());
      ClassDef *ncd;
      GroupDef *gd = cd->partOfGroups()->at(0);
      for (ncli.toFirst();(ncd=ncli.current());++ncli)
      {
        if (ncd->partOfGroups()==0)
        {
          //printf("  Adding %s to group '%s'\n",ncd->name().data(),
          //    gd->groupTitle());
          ncd->makePartOfGroup(gd);
          gd->addClass(ncd);
        }
      }
      cd->visited=TRUE; // only visit every class once
    }
  }
}

//----------------------------

static ClassDef *createTagLessInstance(ClassDef *rootCd,ClassDef *templ,const QCString &fieldName)
{
  QCString fullName = removeAnonymousScopes(templ->name());
  if (fullName.right(2)=="::") fullName=fullName.left(fullName.length()-2);
  fullName+="."+fieldName;
  ClassDef *cd = new ClassDef(templ->getDefFileName(),
                              templ->getDefLine(),
                              fullName,
                              templ->compoundType());
  cd->setDocumentation(templ->documentation(),templ->docFile(),templ->docLine()); // copy docs to definition
  cd->setBriefDescription(templ->briefDescription(),templ->briefFile(),templ->briefLine());
  cd->setLanguage(templ->getLanguage());
  cd->setBodySegment(templ->getStartBodyLine(),templ->getEndBodyLine());
  cd->setBodyDef(templ->getBodyDef());

  cd->setOuterScope(rootCd->getOuterScope());
  if (rootCd->getOuterScope()!=Doxygen::globalScope)
  {
    rootCd->getOuterScope()->addInnerCompound(cd);
  }

  FileDef *fd = templ->getFileDef();
  if (fd)
  {
    cd->setFileDef(fd);
    fd->insertClass(cd);
  }
  LockingPtr<GroupList> groups = rootCd->partOfGroups();
  if ( groups!=0 )
  {
    GroupListIterator gli(*groups);
    GroupDef *gd;
    for (gli.toFirst();(gd=gli.current());++gli)
    {
      cd->makePartOfGroup(gd);
      gd->addClass(cd);
    }
  }
  //cd->insertUsedFile(root->fileName);
  //printf("** adding class %s based on %s\n",fullName.data(),templ->name().data());
  Doxygen::classSDict->append(fullName,cd);

  MemberList *ml = templ->getMemberList(MemberList::pubAttribs);
  if (ml)
  {
    MemberListIterator li(*ml);
    MemberDef *md;
    for (li.toFirst();(md=li.current());++li)
    {
      //printf("    Member %s type=%s\n",md->name().data(),md->typeString());
      MemberDef *imd = new MemberDef(md->getDefFileName(),md->getDefLine(),
                                     md->typeString(),md->name(),md->argsString(),md->excpString(),
                                     md->protection(),md->virtualness(),md->isStatic(),Member,
                                     md->memberType(),
                                     0,0);
      imd->setMemberClass(cd);
      imd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
      imd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
      imd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      imd->setMemberSpecifiers(md->getMemberSpecifiers());
      imd->setMemberGroupId(md->getMemberGroupId());
      imd->setInitializer(md->initializer());
      imd->setMaxInitLines(md->initializerLines());
      imd->setBitfields(md->bitfieldString());
      imd->setLanguage(md->getLanguage());
      cd->insertMember(imd);
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
static void processTagLessClasses(ClassDef *rootCd,
                                  ClassDef *cd,
                                  ClassDef *tagParentCd,
                                  const QCString &prefix,int count)
{
  //printf("%d: processTagLessClasses %s\n",count,cd->name().data());
  //printf("checking members for %s\n",cd->name().data());
  if (cd->getClassSDict())
  {
    MemberList *ml = cd->getMemberList(MemberList::pubAttribs);
    if (ml)
    {
      MemberListIterator li(*ml);
      MemberDef *md;
      for (li.toFirst();(md=li.current());++li)
      {
        QCString type = md->typeString();
        if (type.find("::@")!=-1) // member of tag less struct/union
        {
          ClassSDict::Iterator it(*cd->getClassSDict());
          ClassDef *icd;
          for (it.toFirst();(icd=it.current());++it)
          {
            //printf("  member %s: type='%s'\n",md->name().data(),type.data());
            //printf("  comparing '%s'<->'%s'\n",type.data(),icd->name().data());
            if (type.find(icd->name())!=-1) // matching tag less struct/union
            {
              QCString name = md->name();
              if (name.at(0)=='@') name = "__unnamed__";
              if (!prefix.isEmpty()) name.prepend(prefix+".");
              //printf("    found %s for class %s\n",name.data(),cd->name().data());
              ClassDef *ncd = createTagLessInstance(rootCd,icd,name);
              processTagLessClasses(rootCd,icd,ncd,name,count+1);
              //printf("    addTagged %s to %s\n",ncd->name().data(),tagParentCd->name().data());
              tagParentCd->addTaggedInnerClass(ncd);
              ncd->setTagLessReference(icd);

              // replace tag-less type for generated/original member
              // by newly created class name.
              // note the difference between changing cd and tagParentCd.
              // for the initial call this is the same pointer, but for 
              // recursive calls cd is the original tag-less struct (of which
              // there is only one instance) and tagParentCd is the newly
              // generated tagged struct of which there can be multiple instances!
              MemberList *pml = tagParentCd->getMemberList(MemberList::pubAttribs);
              if (pml)
              {
                MemberListIterator pli(*pml);
                MemberDef *pmd;
                for (pli.toFirst();(pmd=pli.current());++pli)
                {
                  if (pmd->name()==md->name())
                  {
                    pmd->setAccessorType(ncd,substitute(pmd->typeString(),icd->name(),ncd->name()));
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

static void writeMainPageTagFileData()
{
  if (Doxygen::mainPage && !Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    Doxygen::tagFile << "  <compound kind=\"page\">" << endl
                     << "    <name>"
                     << convertToXML(Doxygen::mainPage->name())
                     << "</name>" << endl
                     << "    <title>"
                     << convertToXML(Doxygen::mainPage->title())
                     << "</title>" << endl
                     << "    <filename>"
                     << convertToXML(Doxygen::mainPage->getOutputFileBase())
                     << "</filename>" << endl;

    Doxygen::mainPage->writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }
}

static void findTagLessClasses(ClassDef *cd)
{
  if (cd->getClassSDict())
  {
    ClassSDict::Iterator it(*cd->getClassSDict());
    ClassDef *icd;
    for (it.toFirst();(icd=it.current());++it)
    {
      if (icd->name().find("@")==-1) // process all non-anonymous inner classes
      {
        findTagLessClasses(icd);
      }
    }
  }

  processTagLessClasses(cd,cd,cd,"",0); // process tag less inner struct/classes (if any)
}

static void findTagLessClasses()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli) // for each class
  {
    Definition *scope = cd->getOuterScope();
    if (scope && scope->definitionType()!=Definition::TypeClass) // that is not nested
    {
      findTagLessClasses(cd);
    }
  }
}


//----------------------------------------------------------------------
// build a list of all namespaces mentioned in the documentation
// and all namespaces that have a documentation block before their definition.
static void buildNamespaceList(EntryNav *rootNav)
{
  if (
       (rootNav->section()==Entry::NAMESPACE_SEC ||
        rootNav->section()==Entry::NAMESPACEDOC_SEC ||
        rootNav->section()==Entry::PACKAGEDOC_SEC
       ) && 
       !rootNav->name().isEmpty()
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    //printf("** buildNamespaceList(%s)\n",root->name.data());

    QCString fName = root->name;
    if (root->section==Entry::PACKAGEDOC_SEC)
    {
      fName=substitute(fName,".","::");
    }

    QCString fullName = stripAnonymousNamespaceScope(fName);
    if (!fullName.isEmpty())
    {
      //printf("Found namespace %s in %s at line %d\n",root->name.data(),
      //        root->fileName.data(), root->startLine);
      NamespaceDef *nd;
      if ((nd=Doxygen::namespaceSDict->find(fullName))) // existing namespace
      {
        nd->setDocumentation(root->doc,root->docFile,root->docLine);
        nd->setName(fullName); // change name to match docs
        nd->addSectionsToDefinition(root->anchors);
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        if (nd->getLanguage()==SrcLangExt_Unknown)
        {
          nd->setLanguage(root->lang);
        }

        // file definition containing the namespace nd
        FileDef *fd=rootNav->fileDef();
        // insert the namespace in the file definition
        if (fd) fd->insertNamespace(nd);
        addNamespaceToGroups(root,nd);
        nd->setRefItems(root->sli);
      }
      else // fresh namespace
      {
        QCString tagName;
        QCString tagFileName;
        if (rootNav->tagInfo())
        {
          tagName=rootNav->tagInfo()->tagName;
          tagFileName=rootNav->tagInfo()->fileName;
        }
        //printf("++ new namespace %s lang=%s\n",fullName.data(),langToString(root->lang).data());
        NamespaceDef *nd=new NamespaceDef(root->fileName,root->startLine,fullName,tagName,tagFileName);
        nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->addSectionsToDefinition(root->anchors);
        nd->setHidden(root->hidden);
        nd->setArtificial(root->artificial);
        nd->setLanguage(root->lang);

        //printf("Adding namespace to group\n");
        addNamespaceToGroups(root,nd);
        nd->setRefItems(root->sli);

        // file definition containing the namespace nd
        FileDef *fd=rootNav->fileDef();
        // insert the namespace in the file definition
        if (fd) fd->insertNamespace(nd);

        // the empty string test is needed for extract all case
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->insertUsedFile(root->fileName);
        nd->setBodySegment(root->bodyLine,root->endBodyLine);
        nd->setBodyDef(fd);
        // add class to the list
        Doxygen::namespaceSDict->inSort(fullName,nd);

        // also add namespace to the correct structural context 
        Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,fullName);
        //printf("adding namespace %s to context %s\n",nd->name().data(),d?d->name().data():"<none>");
        if (d==0) // we didn't find anything, create the scope artificially
                  // anyway, so we can at least relate scopes properly.
        {
          Definition *d = buildScopeFromQualifiedName(fullName,fullName.contains("::"),nd->getLanguage());
          d->addInnerCompound(nd);
          nd->setOuterScope(d);
          // TODO: Due to the order in which the tag file is written
          // a nested class can be found before its parent!
        }
        else
        {
          d->addInnerCompound(nd);
          nd->setOuterScope(d);
        }
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildNamespaceList,rootNav);
}

//----------------------------------------------------------------------

static NamespaceDef *findUsedNamespace(NamespaceSDict *unl,
                              const QCString &name)
{
  NamespaceDef *usingNd =0;
  if (unl)
  {
    //printf("Found namespace dict %d\n",unl->count());
    NamespaceSDict::Iterator unli(*unl);
    NamespaceDef *und;
    for (unli.toFirst();(und=unli.current());++unli)
    {
      QCString uScope=und->name()+"::";
      usingNd = getResolvedNamespace(uScope+name);
      //printf("Also trying with scope=`%s' usingNd=%p\n",(uScope+name).data(),usingNd);
    }
  }
  return usingNd;
}

static void findUsingDirectives(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::USINGDIR_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    //printf("Found using directive %s at line %d of %s\n",
    //    root->name.data(),root->startLine,root->fileName.data());
    QCString name=substitute(root->name,".","::");
    if (!name.isEmpty())
    {
      NamespaceDef *usingNd = 0;
      NamespaceDef *nd = 0;
      FileDef      *fd = rootNav->fileDef();
      QCString nsName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (rootNav->parent() && rootNav->parent()->section()==Entry::NAMESPACE_SEC &&
          (fd==0 || fd->getLanguage()!=SrcLangExt_Java) // not a .java file
         )
      {
        nsName=stripAnonymousNamespaceScope(rootNav->parent()->name());
        if (!nsName.isEmpty())
        {
          nd = getResolvedNamespace(nsName);
        }
      }

      // find the scope in which the `using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e. 
      // file scope). 
      int scopeOffset = nsName.length();
      do
      {
        QCString scope=scopeOffset>0 ? 
                      nsName.left(scopeOffset)+"::" : QCString();
        usingNd = getResolvedNamespace(scope+name);
        //printf("Trying with scope=`%s' usingNd=%p\n",(scope+name).data(),usingNd);
        if (scopeOffset==0)
        {
          scopeOffset=-1;
        }
        else if ((scopeOffset=nsName.findRev("::",scopeOffset-1))==-1)
        {
          scopeOffset=0;
        }
      } while (scopeOffset>=0 && usingNd==0);

      if (usingNd==0 && nd) // not found, try used namespaces in this scope
                            // or in one of the parent namespace scopes
      {
        NamespaceDef *pnd = nd;
        while (pnd && usingNd==0)
        {
          // also try with one of the used namespaces found earlier
          usingNd = findUsedNamespace(pnd->getUsedNamespaces(),name);

          // goto the parent
          Definition *s = pnd->getOuterScope();
          if (s && s->definitionType()==Definition::TypeNamespace)
          {
            pnd = (NamespaceDef*)s;
          }
          else
          {
            pnd = 0;
          }
        }
      }
      if (usingNd==0 && fd) // still nothing, also try used namespace in the
                            // global scope
      {
        usingNd = findUsedNamespace(fd->getUsedNamespaces(),name);
      }

      //printf("%s -> %s\n",name.data(),usingNd?usingNd->name().data():"<none>");

      // add the namespace the correct scope
      if (usingNd)
      {
        //printf("using fd=%p nd=%p\n",fd,nd);
        if (nd)
        {
          //printf("Inside namespace %s\n",nd->name().data());
          nd->addUsingDirective(usingNd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",fd->name().data());
          fd->addUsingDirective(usingNd);
        }
      }
      else // unknown namespace, but add it anyway.
      {
        //printf("++ new unknown namespace %s lang=%s\n",name.data(),langToString(root->lang).data());
        NamespaceDef *nd=new NamespaceDef(root->fileName,root->startLine,name);
        nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->addSectionsToDefinition(root->anchors);
        //printf("** Adding namespace %s hidden=%d\n",name.data(),root->hidden);
        nd->setHidden(root->hidden);
        nd->setArtificial(TRUE);
        nd->setLanguage(root->lang);

        QListIterator<Grouping> gli(*root->groups);
        Grouping *g;
        for (;(g=gli.current());++gli)
        {
          GroupDef *gd=0;
          if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict->find(g->groupname)))
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
        nd->insertUsedFile(root->fileName);
        // add class to the list
        Doxygen::namespaceSDict->inSort(name,nd);
        nd->setRefItems(root->sli);
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findUsingDirectives,rootNav);
}

//----------------------------------------------------------------------

static void buildListOfUsingDecls(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::USINGDECL_SEC &&
      !(rootNav->parent()->section()&Entry::COMPOUND_MASK) // not a class/struct member
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    QCString name = substitute(root->name,".","::");

    if (g_usingDeclarations.find(name)==0)
    {
      FileDef *fd = rootNav->fileDef();
      if (fd)
      {
        g_usingDeclarations.insert(name,fd);
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildListOfUsingDecls,rootNav);
}

  
static void findUsingDeclarations(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::USINGDECL_SEC &&
      !(rootNav->parent()->section()&Entry::COMPOUND_MASK) // not a class/struct member
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    //printf("Found using declaration %s at line %d of %s inside section %x\n",
    //   root->name.data(),root->startLine,root->fileName.data(),
    //   rootNav->parent()->section());
    if (!root->name.isEmpty())
    {
      ClassDef *usingCd = 0;
      NamespaceDef *nd = 0;
      FileDef      *fd = rootNav->fileDef();
      QCString scName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (rootNav->parent()->section() == Entry::NAMESPACE_SEC)
      {
        scName=rootNav->parent()->name();
        if (!scName.isEmpty())
        {
          nd = getResolvedNamespace(scName);
        }
      }

      // Assume the using statement was used to import a class.
      // Find the scope in which the `using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e. 
      // file scope).

      QCString name = substitute(root->name,".","::"); //Java/C# scope->internal
      usingCd = getClass(name);
      if (usingCd==0)
      {
        usingCd = Doxygen::hiddenClasses->find(name);
      }

      //printf("%s -> %p\n",root->name.data(),usingCd);
      if (usingCd==0) // definition not in the input => add an artificial class
      {
        Debug::print(Debug::Classes,0,"  New using class `%s' (sec=0x%08x)! #tArgLists=%d\n",
             name.data(),root->section,root->tArgLists ? (int)root->tArgLists->count() : -1);
        usingCd = new ClassDef(
                     "<using>",1,
                     name,ClassDef::Class);
        Doxygen::hiddenClasses->append(root->name,usingCd);
        usingCd->setArtificial(TRUE);
        usingCd->setLanguage(root->lang);
      }
      else
      {
        Debug::print(Debug::Classes,0,"  Found used class %s in scope=%s\n",
            usingCd->name().data(),nd?nd->name().data():fd->name().data());
      }

      if (usingCd) // add the class to the correct scope
      {
        if (nd)
        {
          //printf("Inside namespace %s\n",nd->name().data());
          nd->addUsingDeclaration(usingCd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",fd->name().data());
          fd->addUsingDeclaration(usingCd);
        }
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findUsingDeclarations,rootNav);
}

//----------------------------------------------------------------------

static void findUsingDeclImports(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::USINGDECL_SEC &&
      (rootNav->parent()->section()&Entry::COMPOUND_MASK) // in a class/struct member
     )
  {
    //printf("Found using declaration %s at line %d of %s inside section %x\n",
    //    root->name.data(),root->startLine,root->fileName.data(),
    //    root->parent->section);
    QCString fullName=removeRedundantWhiteSpace(rootNav->parent()->name());
    fullName=stripAnonymousNamespaceScope(fullName);
    fullName=stripTemplateSpecifiersFromScope(fullName);
    ClassDef *cd = getClass(fullName);
    if (cd)
    {
      //printf("found class %s\n",cd->name().data());
      int i=rootNav->name().find("::");
      if (i!=-1)
      {
        QCString scope=rootNav->name().left(i);
        QCString memName=rootNav->name().right(rootNav->name().length()-i-2);
        ClassDef *bcd = getResolvedClass(cd,0,scope); // todo: file in fileScope parameter
        if (bcd)
        {
          //printf("found class %s\n",bcd->name().data());
          MemberNameInfoSDict *mndict=bcd->memberNameInfoSDict();
          if (mndict)
          {
            MemberNameInfo *mni = mndict->find(memName);
            if (mni)
            {
              MemberNameInfoIterator mnii(*mni); 
              MemberInfo *mi;
              for ( ; (mi=mnii.current()) ; ++mnii )
              {
                MemberDef *md = mi->memberDef;
                if (md && md->protection()!=Private)
                {

                  rootNav->loadEntry(g_storage);
                  Entry *root = rootNav->entry();

                  //printf("found member %s\n",mni->memberName());
                  MemberDef *newMd = 0;
                  {
                    LockingPtr<ArgumentList> templAl = md->templateArguments();
                    LockingPtr<ArgumentList> al = md->templateArguments();
                    newMd = new MemberDef(
                      root->fileName,root->startLine,
                      md->typeString(),memName,md->argsString(),
                      md->excpString(),root->protection,root->virt,
                      md->isStatic(),Member,md->memberType(),
                      templAl.pointer(),al.pointer()
                      );
                  }
                  newMd->setMemberClass(cd);
                  cd->insertMember(newMd);
                  if (!root->doc.isEmpty() || !root->brief.isEmpty())
                  {
                    newMd->setDocumentation(root->doc,root->docFile,root->docLine);
                    newMd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
                    newMd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                  }
                  else
                  {
                    newMd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
                    newMd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
                    newMd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
                  }
                  newMd->setDefinition(md->definition());
                  newMd->enableCallGraph(root->callGraph);
                  newMd->enableCallerGraph(root->callerGraph);
                  newMd->setBitfields(md->bitfieldString());
                  newMd->addSectionsToDefinition(root->anchors);
                  newMd->setBodySegment(md->getStartBodyLine(),md->getEndBodyLine());
                  newMd->setBodyDef(md->getBodyDef());
                  newMd->setInitializer(md->initializer());
                  newMd->setMaxInitLines(md->initializerLines());
                  newMd->setMemberGroupId(root->mGrpId);
                  newMd->setMemberSpecifiers(md->getMemberSpecifiers());
                  newMd->setLanguage(root->lang);

                  rootNav->releaseEntry();
                }
              }
            }
          }
        }
      }
    }

  }
  RECURSE_ENTRYTREE(findUsingDeclImports,rootNav);
}

//----------------------------------------------------------------------

static void findIncludedUsingDirectives()
{
  // first mark all files as not visited
  FileNameListIterator fnli(*Doxygen::inputNameList); 
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      fd->visited=FALSE;
    }
  }
  // then recursively add using directives found in #include files
  // to files that have not been visited.
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      if (!fd->visited) 
      {
        //printf("----- adding using directives for file %s\n",fd->name().data());
        fd->addIncludedUsingDirectives();
      }
    }
  }
}

//----------------------------------------------------------------------

static MemberDef *addVariableToClass(
    EntryNav *rootNav,
    ClassDef *cd,
    MemberDef::MemberType mtype,
    const QCString &name,
    bool fromAnnScope,
    MemberDef *fromAnnMemb,
    Protection prot,
    Relationship related)
{
  Entry *root = rootNav->entry();

  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  QCString scopeSeparator="::";
  SrcLangExt lang = cd->getLanguage();
  if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
  {
    qualScope = substitute(qualScope,"::",".");
    scopeSeparator=".";
  }
  Debug::print(Debug::Variables,0,
      "  class variable:\n"
      "    `%s' `%s'::`%s' `%s' prot=`%d ann=%d init=`%s'\n",
      root->type.data(),
      qualScope.data(), 
      name.data(),
      root->args.data(),
      root->protection,
      fromAnnScope,
      root->initializer.data()
              );

  QCString def;
  if (!root->type.isEmpty())
  {
    if (related || mtype==MemberDef::Friend || Config_getBool("HIDE_SCOPE_NAMES"))
    {
      if (root->spec&Entry::Alias) // turn 'typedef B A' into 'using A = B'  
      {
        def="using "+name+" = "+root->type.mid(7);
      }
      else
      {
        def=root->type+" "+name+root->args;
      }
    }
    else
    {
      if (root->spec&Entry::Alias) // turn 'typedef B C::A' into 'using C::A = B'  
      {
        def="using "+qualScope+scopeSeparator+name+" = "+root->type.mid(7);
      }
      else
      {
        def=root->type+" "+qualScope+scopeSeparator+name+root->args;
      }
    }
  }
  else
  {
    if (Config_getBool("HIDE_SCOPE_NAMES"))
    {
      def=name+root->args;
    }
    else
    {
      def=qualScope+scopeSeparator+name+root->args;
    }
  }
  def.stripPrefix("static ");

  // see if the member is already found in the same scope
  // (this may be the case for a static member that is initialized
  //  outside the class)
  MemberName *mn=Doxygen::memberNameSDict->find(name);
  if (mn)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      //printf("md->getClassDef()=%p cd=%p type=[%s] md->typeString()=[%s]\n",
      //    md->getClassDef(),cd,root->type.data(),md->typeString());
      if (md->getClassDef()==cd && 
          removeRedundantWhiteSpace(root->type)==md->typeString()) 
        // member already in the scope
      {

        if (root->lang==SrcLangExt_ObjC && 
            root->mtype==Property && 
            md->memberType()==MemberDef::Variable)
        { // Objective-C 2.0 property
          // turn variable into a property
          md->setProtection(root->protection);
          cd->reclassifyMember(md,MemberDef::Property);
        }
        addMemberDocs(rootNav,md,def,0,FALSE);
        //printf("    Member already found!\n");
        return md;
      }
    } 
  }

  // new member variable, typedef or enum value
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,0,
      prot,Normal,root->stat,related,
      mtype,root->tArgLists ? root->tArgLists->last() : 0,0);
  md->setTagInfo(rootNav->tagInfo());
  md->setMemberClass(cd); // also sets outer scope (i.e. getOuterScope())
  //md->setDefFile(root->fileName);
  //md->setDefLine(root->startLine);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->setDefinition(def);
  md->setBitfields(root->bitfields);
  md->addSectionsToDefinition(root->anchors);
  md->setFromAnonymousScope(fromAnnScope);
  md->setFromAnonymousMember(fromAnnMemb);
  //md->setIndentDepth(indentDepth);
  md->setBodySegment(root->bodyLine,root->endBodyLine);
  md->setInitializer(root->initializer);
  md->setMaxInitLines(root->initLines);
  md->setMemberGroupId(root->mGrpId);
  md->setMemberSpecifiers(root->spec);
  md->setReadAccessor(root->read);
  md->setWriteAccessor(root->write);
  md->enableCallGraph(root->callGraph);
  md->enableCallerGraph(root->callerGraph);
  md->setHidden(root->hidden);
  md->setArtificial(root->artificial);
  md->setLanguage(root->lang);
  addMemberToGroups(root,md);
  //if (root->mGrpId!=-1) 
  //{
  //  printf("memberdef %s in memberGroup %d\n",name.data(),root->mGrpId);
  //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
  //
  md->setBodyDef(rootNav->fileDef());

  //printf("    Adding member=%s\n",md->name().data());
  // add the member to the global list
  if (mn)
  {
    mn->append(md);
  }
  else // new variable name
  {
    mn = new MemberName(name);
    mn->append(md);
    //printf("Adding memberName=%s\n",mn->memberName());
    //Doxygen::memberNameDict.insert(name,mn);
    //Doxygen::memberNameList.append(mn);
    Doxygen::memberNameSDict->append(name,mn);
    // add the member to the class
  }
  //printf("    New member adding to %s (%p)!\n",cd->name().data(),cd);
  cd->insertMember(md);
  md->setRefItems(root->sli);

  //TODO: insert FileDef instead of filename strings.
  cd->insertUsedFile(root->fileName);
  rootNav->changeSection(Entry::EMPTY_SEC);
  return md;
}

//----------------------------------------------------------------------

static MemberDef *addVariableToFile(
    EntryNav *rootNav,
    MemberDef::MemberType mtype,
    const QCString &scope,
    const QCString &name,
    bool fromAnnScope,
    /*int indentDepth,*/
    MemberDef *fromAnnMemb)
{
  Entry *root = rootNav->entry();
  Debug::print(Debug::Variables,0,
      "  global variable:\n"
      "    type=`%s' scope=`%s' name=`%s' args=`%s' prot=`%d mtype=%d lang=%d\n",
      root->type.data(),
      scope.data(), 
      name.data(),
      root->args.data(),
      root->protection,
      mtype,
      root->lang
              );

  FileDef *fd = rootNav->fileDef();

  // see if we have a typedef that should hide a struct or union
  if (mtype==MemberDef::Typedef && Config_getBool("TYPEDEF_HIDES_STRUCT"))
  {
    QCString type = root->type;
    type.stripPrefix("typedef ");
    if (type.left(7)=="struct " || type.left(6)=="union ")
    {
      type.stripPrefix("struct ");
      type.stripPrefix("union ");
      static QRegExp re("[a-z_A-Z][a-z_A-Z0-9]*");
      int l,s;
      s = re.match(type,0,&l);
      if (s>=0)
      {
        QCString typeValue = type.mid(s,l);
        ClassDef *cd = getClass(typeValue);
        if (cd)
        {
          // this typedef should hide compound name cd, so we
          // change the name that is displayed from cd.
          cd->setClassName(name);
          cd->setDocumentation(root->doc,root->docFile,root->docLine);
          cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          return 0;
        }
      }
    }
  }

  // see if the function is inside a namespace
  NamespaceDef *nd = 0;
  QCString nscope;
  if (!scope.isEmpty())
  {
    if (scope.find('@')!=-1) return 0; // anonymous scope!
    //nscope=removeAnonymousScopes(scope);
    //if (!nscope.isEmpty())
    //{
    nd = getResolvedNamespace(scope);
    //}
  }
  QCString def;

  // determine the definition of the global variable
  if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@' && 
      !Config_getBool("HIDE_SCOPE_NAMES")
     )
    // variable is inside a namespace, so put the scope before the name
  {
    SrcLangExt lang = nd->getLanguage();
    QCString sep=getLanguageSpecificSeparator(lang);

    if (!root->type.isEmpty())
    {
      if (root->spec&Entry::Alias) // turn 'typedef B NS::A' into 'using NS::A = B'  
      {
        def="using "+nd->name()+sep+name+" = "+root->type;
      }
      else // normal member
      {
        def=root->type+" "+nd->name()+sep+name+root->args;
      }
    }
    else
    {
      def=nd->name()+sep+name+root->args;
    }
  }
  else
  {
    if (!root->type.isEmpty() && !root->name.isEmpty())
    {
      if (name.at(0)=='@') // dummy variable representing anonymous union
      {
        def=root->type;
      }
      else
      {
        if (root->spec&Entry::Alias) // turn 'typedef B A' into 'using A = B'  
        {
          def="using "+root->name+" = "+root->type.mid(7);
        }
        else // normal member
        {
          def=root->type+" "+name+root->args;
        }
      }
    }
    else
    {
      def=name+root->args;
    }
  }
  def.stripPrefix("static ");

  MemberName *mn=Doxygen::functionNameSDict->find(name);
  if (mn)
  {
    //QCString nscope=removeAnonymousScopes(scope);
    //NamespaceDef *nd=0;
    //if (!nscope.isEmpty())
    if (!scope.isEmpty())
    {
      nd = getResolvedNamespace(scope);
    }
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (
          ((nd==0 && md->getNamespaceDef()==0 && md->getFileDef() && 
            root->fileName==md->getFileDef()->absFilePath()
           ) // both variable names in the same file
           || (nd!=0 && md->getNamespaceDef()==nd) // both in same namespace
          )
          && !md->isDefine() // function style #define's can be "overloaded" by typedefs or variables
          && !md->isEnumerate() // in C# an enum value and enum can have the same name
         )
        // variable already in the scope
      {
        bool isPHPArray = md->getLanguage()==SrcLangExt_PHP &&
                          md->argsString()!=root->args && 
                          root->args.find('[')!=-1;
        bool staticsInDifferentFiles = 
                          root->stat && md->isStatic() && 
                          root->fileName!=md->getDefFileName();

        if (md->getFileDef() &&
            !isPHPArray && // not a php array
            !staticsInDifferentFiles
           ) 
          // not a php array variable
        {

          Debug::print(Debug::Variables,0,
              "    variable already found: scope=%s\n",md->getOuterScope()->name().data());
          addMemberDocs(rootNav,md,def,0,FALSE);
          md->setRefItems(root->sli);
          return md;
        }
      }
    } 
  }
  Debug::print(Debug::Variables,0,
    "    new variable, nd=%s!\n",nd?nd->name().data():"<global>");
  // new global variable, enum value or typedef
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,0,
      Public, Normal,root->stat,Member,
      mtype,root->tArgLists ? root->tArgLists->last() : 0,0);
  md->setTagInfo(rootNav->tagInfo());
  md->setMemberSpecifiers(root->spec);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->addSectionsToDefinition(root->anchors);
  md->setFromAnonymousScope(fromAnnScope);
  md->setFromAnonymousMember(fromAnnMemb);
  md->setInitializer(root->initializer);
  md->setMaxInitLines(root->initLines);
  md->setMemberGroupId(root->mGrpId);
  md->setDefinition(def);
  md->setLanguage(root->lang);
  md->enableCallGraph(root->callGraph);
  md->enableCallerGraph(root->callerGraph);
  md->setExplicitExternal(root->explicitExternal);
  //md->setOuterScope(fd);
  if (!root->explicitExternal)
  {
    md->setBodySegment(root->bodyLine,root->endBodyLine);
    md->setBodyDef(fd);
  }
  addMemberToGroups(root,md);

  md->setRefItems(root->sli);
  if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
  {
    md->setNamespace(nd);
    nd->insertMember(md); 
  }

  // add member to the file (we do this even if we have already inserted
  // it into the namespace. 
  if (fd)
  {
    md->setFileDef(fd); 
    fd->insertMember(md);
  }

  // add member definition to the list of globals 
  if (mn)
  {
    mn->append(md);
  }
  else
  {
    mn = new MemberName(name);
    mn->append(md);
    Doxygen::functionNameSDict->append(name,mn);
  }
  rootNav->changeSection(Entry::EMPTY_SEC);
  return md;
}

/*! See if the return type string \a type is that of a function pointer 
 *  \returns -1 if this is not a function pointer variable or
 *           the index at which the closing brace of (...*name) was found.
 */
static int findFunctionPtr(const QCString &type,int lang, int *pLength=0)
{
  if (lang == SrcLangExt_Fortran) return -1; // Fortran does not have function pointers
  static const QRegExp re("([^)]*[\\*\\^][^)]*)");
  int i=-1,l;
  int bb=type.find('<');
  int be=type.findRev('>');
  if (!type.isEmpty() &&             // return type is non-empty
      (i=re.match(type,0,&l))!=-1 && // contains (...*...)
      type.find("operator")==-1 &&   // not an operator
      (type.find(")(")==-1 || type.find("typedef ")!=-1) &&
                                    // not a function pointer return type
      !(bb<i && i<be) // bug665855: avoid treating "typedef A<void (T*)> type" as a function pointer
     )
  {
    if (pLength) *pLength=l;
    //printf("findFunctionPtr=%d\n",i);
    return i;
  }
  else
  {
    //printf("findFunctionPtr=%d\n",-1);
    return -1;
  }
}


/*! Returns TRUE iff \a type is a class within scope \a context.
 *  Used to detect variable declarations that look like function prototypes.
 */
static bool isVarWithConstructor(EntryNav *rootNav)
{
  static QRegExp initChars("[0-9\"'&*!^]+");
  static QRegExp idChars("[a-z_A-Z][a-z_A-Z0-9]*");
  bool result=FALSE;
  bool typeIsClass;
  QCString type;
  Definition *ctx = 0;
  FileDef *fd = 0;
  int ti;

  //printf("isVarWithConstructor(%s)\n",rootNav->name().data());
  rootNav->loadEntry(g_storage);
  Entry *root = rootNav->entry();

  if (rootNav->parent()->section() & Entry::COMPOUND_MASK)
  { // inside a class
    result=FALSE;
    goto done;
  }
  else if ((fd = rootNav->fileDef()) &&
            (fd->name().right(2)==".c" || fd->name().right(2)==".h")
          )
  { // inside a .c file
    result=FALSE;
    goto done;
  }
  if (root->type.isEmpty()) 
  {
    result=FALSE;
    goto done;
  }
  if (!rootNav->parent()->name().isEmpty()) 
  {
    ctx=Doxygen::namespaceSDict->find(rootNav->parent()->name());
  }
  type = root->type;
  // remove qualifiers
  findAndRemoveWord(type,"const");
  findAndRemoveWord(type,"static");
  findAndRemoveWord(type,"volatile");
  //if (type.left(6)=="const ") type=type.right(type.length()-6);
  typeIsClass=getResolvedClass(ctx,fd,type)!=0;
  if (!typeIsClass && (ti=type.find('<'))!=-1)
  {
    typeIsClass=getResolvedClass(ctx,fd,type.left(ti))!=0;
  }
  if (typeIsClass) // now we still have to check if the arguments are 
                   // types or values. Since we do not have complete type info
                   // we need to rely on heuristics :-(
  {
    //printf("typeIsClass\n");
    ArgumentList *al = root->argList;
    if (al==0 || al->isEmpty()) 
    {
      result=FALSE; // empty arg list -> function prototype.
      goto done;
    }
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      if (!a->name.isEmpty() || !a->defval.isEmpty()) 
      {
        if (a->name.find(initChars)==0)
        {
          result=TRUE;
        }
        else
        {
          result=FALSE; // arg has (type,name) pair -> function prototype
        }
        goto done;
      }
      if (a->type.isEmpty() || getResolvedClass(ctx,fd,a->type)!=0) 
      {
        result=FALSE; // arg type is a known type
        goto done;
      }
      if (checkIfTypedef(ctx,fd,a->type))
      {
         //printf("%s:%d: false (arg is typedef)\n",__FILE__,__LINE__);
         result=FALSE; // argument is a typedef
         goto done;
      }
      if (a->type.at(a->type.length()-1)=='*' ||
          a->type.at(a->type.length()-1)=='&')  
                     // type ends with * or & => pointer or reference
      {
        result=FALSE;
        goto done;
      }
      if (a->type.find(initChars)==0) 
      {
        result=TRUE; // argument type starts with typical initializer char
        goto done;
      }
      QCString resType=resolveTypeDef(ctx,a->type);
      if (resType.isEmpty()) resType=a->type;
      int len;
      if (idChars.match(resType,0,&len)==0) // resType starts with identifier
      {
        resType=resType.left(len);
        //printf("resType=%s\n",resType.data());
        if (resType=="int"    || resType=="long" || resType=="float" || 
            resType=="double" || resType=="char" || resType=="signed" || 
            resType=="const"  || resType=="unsigned" || resType=="void") 
        {
          result=FALSE; // type keyword -> function prototype
          goto done;
        }
      }
    }
    result=TRUE;
  }

done:
  //printf("isVarWithConstructor(%s,%s)=%d\n",rootNav->parent()->name().data(),
  //                                          root->type.data(),result);
  rootNav->releaseEntry();
  return result;
}

static void addVariable(EntryNav *rootNav,int isFuncPtr=-1)
{
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    Debug::print(Debug::Variables,0,
                  "VARIABLE_SEC: \n"
                  "  type=`%s' name=`%s' args=`%s' bodyLine=`%d' mGrpId=%d relates=%s\n",
                   root->type.data(),
                   root->name.data(),
                   root->args.data(),
                   root->bodyLine,
                   root->mGrpId,
                   root->relates.data()
                );
    //printf("root->parent->name=%s\n",root->parent->name.data());

    if (root->type.isEmpty() && root->name.find("operator")==-1 &&
        (root->name.find('*')!=-1 || root->name.find('&')!=-1))
    {
      // recover from parse error caused by redundant braces 
      // like in "int *(var[10]);", which is parsed as
      // type="" name="int *" args="(var[10])"

      root->type=root->name;
      static const QRegExp reName("[a-z_A-Z][a-z_A-Z0-9]*");
      int l;
      int i=root->args.isEmpty() ? -1 : reName.match(root->args,0,&l);
      root->name=root->args.mid(i,l);
      root->args=root->args.mid(i+l,root->args.find(')',i+l)-i-l);
      //printf("new: type=`%s' name=`%s' args=`%s'\n",
      //    root->type.data(),root->name.data(),root->args.data());
    }
    else
    {
      int i=isFuncPtr;
      if (i==-1 && (root->spec&Entry::Alias)==0) i=findFunctionPtr(root->type,root->lang); // for typedefs isFuncPtr is not yet set
      Debug::print(Debug::Variables,0,"  functionPtr? %s\n",i!=-1?"yes":"no");
      if (i!=-1) // function pointer
      {
        int ai = root->type.find('[',i);
        if (ai>i) // function pointer array
        {
          root->args.prepend(root->type.right(root->type.length()-ai));
          root->type=root->type.left(ai);
        }
        else if (root->type.find(')',i)!=-1) // function ptr, not variable like "int (*bla)[10]"
        {
          root->type=root->type.left(root->type.length()-1);
          root->args.prepend(")");
          //printf("root->type=%s root->args=%s\n",root->type.data(),root->args.data());
        }
      }
      else if (root->type.find("typedef ")!=-1 && root->type.right(2)=="()") // typedef void (func)(int)
      {
        root->type=root->type.left(root->type.length()-1);
        root->args.prepend(")");
      }
    }
    
    QCString scope,name=removeRedundantWhiteSpace(root->name);

    // find the scope of this variable 
    EntryNav *p = rootNav->parent();
    while ((p->section() & Entry::SCOPE_MASK))
    {
      QCString scopeName = p->name();
      if (!scopeName.isEmpty())
      {
        scope.prepend(scopeName);
        break;
      }
      p=p->parent();
    }
    
    MemberDef::MemberType mtype;
    QCString type=root->type.stripWhiteSpace();
    ClassDef *cd=0;
    bool isRelated=FALSE;
    bool isMemberOf=FALSE;

    QCString classScope=stripAnonymousNamespaceScope(scope);
    classScope=stripTemplateSpecifiersFromScope(classScope,FALSE);
    QCString annScopePrefix=scope.left(scope.length()-classScope.length());

    if (root->name.findRev("::")!=-1) 
    {
      if (root->type=="friend class" || root->type=="friend struct" || 
          root->type=="friend union")
      {
         cd=getClass(scope);
         if (cd)
         {
           addVariableToClass(rootNav,  // entry
                              cd,    // class to add member to
                              MemberDef::Friend, // type of member
                              name, // name of the member
                              FALSE,  // from Anonymous scope
                              0,      // anonymous member
                              Public, // protection
                              Member  // related to a class
                             );
         }
      }
      goto nextMember;
               /* skip this member, because it is a 
                * static variable definition (always?), which will be
                * found in a class scope as well, but then we know the
                * correct protection level, so only then it will be
                * inserted in the correct list!
                */
    }

    if (type=="@") 
      mtype=MemberDef::EnumValue;
    else if (type.left(8)=="typedef ") 
      mtype=MemberDef::Typedef;
    else if (type.left(7)=="friend ")
      mtype=MemberDef::Friend;
    else if (root->mtype==Property)
      mtype=MemberDef::Property;
    else if (root->mtype==Event)
      mtype=MemberDef::Event;
    else
      mtype=MemberDef::Variable;

    if (!root->relates.isEmpty()) // related variable
    {
      isRelated=TRUE;
      isMemberOf=(root->relatesType == MemberOf);
      if (getClass(root->relates)==0 && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else 
        scope=root->relates;
    }
    
    cd=getClass(scope);
    if (cd==0 && classScope!=scope) cd=getClass(classScope);
    if (cd)
    {
      MemberDef *md=0;

      // if cd is an anonymous (=tag less) scope we insert the member 
      // into a non-anonymous parent scope as well. This is needed to
      // be able to refer to it using \var or \fn

      //int indentDepth=0;
      int si=scope.find('@');
      //int anonyScopes = 0;
      //bool added=FALSE;
      
      static bool inlineSimpleStructs = Config_getBool("INLINE_SIMPLE_STRUCTS");
      if (si!=-1 && !inlineSimpleStructs) // anonymous scope or type
      {
        QCString pScope;
        ClassDef *pcd=0;
        pScope = scope.left(QMAX(si-2,0)); // scope without tag less parts
        if (!pScope.isEmpty())
          pScope.prepend(annScopePrefix);
        else if (annScopePrefix.length()>2)
          pScope=annScopePrefix.left(annScopePrefix.length()-2);
        if (name.at(0)!='@')
        {
          if (!pScope.isEmpty() && (pcd=getClass(pScope)))
          {
            md=addVariableToClass(rootNav,  // entry
                                  pcd,   // class to add member to
                                  mtype, // member type
                                  name,  // member name
                                  TRUE,  // from anonymous scope
                                  0,     // from anonymous member
                                  root->protection,
                                  isMemberOf ? Foreign : isRelated ? Related : Member
                                 );
            //added=TRUE;
          }
          else // anonymous scope inside namespace or file => put variable in the global scope
          {
            if (mtype==MemberDef::Variable)
            {
              md=addVariableToFile(rootNav,mtype,pScope,name,TRUE,0); 
            }
            //added=TRUE;
          }
        }
      }

      //printf("name=`%s' scope=%s scope.right=%s\n",
      //                   name.data(),scope.data(),
      //                   scope.right(scope.length()-si).data());
      addVariableToClass(rootNav,   // entry
                         cd,     // class to add member to
                         mtype,  // member type
                         name,   // name of the member
                         FALSE,  // from anonymous scope
                         md,     // from anonymous member
                         root->protection, 
                         isMemberOf ? Foreign : isRelated ? Related : Member);
    }
    else if (!name.isEmpty()) // global variable
    {
      //printf("Inserting member in global scope %s!\n",scope.data());
      addVariableToFile(rootNav,mtype,scope,name,FALSE,/*0,*/0);
    }

nextMember:
    rootNav->releaseEntry();
}

//----------------------------------------------------------------------
// Searches the Entry tree for typedef documentation sections.
// If found they are stored in their class or in the global list.
static void buildTypedefList(EntryNav *rootNav)
{
  //printf("buildVarList(%s)\n",rootNav->name().data());
  if (!rootNav->name().isEmpty() &&
      rootNav->section()==Entry::VARIABLE_SEC &&
      rootNav->type().find("typedef ")!=-1 // its a typedef
     ) 
  {
    addVariable(rootNav);
  }
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *e;
    for (;(e=eli.current());++eli)
    {
      if (e->section()!=Entry::ENUM_SEC) 
      {
        buildTypedefList(e);
      }
    }
  }
}

//----------------------------------------------------------------------
// Searches the Entry tree for Variable documentation sections.
// If found they are stored in their class or in the global list.

static void buildVarList(EntryNav *rootNav)
{
  //printf("buildVarList(%s) section=%08x\n",rootNav->name().data(),rootNav->section());
  int isFuncPtr=-1;
  if (!rootNav->name().isEmpty() &&
      (rootNav->type().isEmpty() || g_compoundKeywordDict.find(rootNav->type())==0) &&
      (
       (rootNav->section()==Entry::VARIABLE_SEC    // it's a variable
       ) ||
       (rootNav->section()==Entry::FUNCTION_SEC && // or maybe a function pointer variable 
        (isFuncPtr=findFunctionPtr(rootNav->type(),rootNav->lang()))!=-1
       ) ||
       (rootNav->section()==Entry::FUNCTION_SEC && // class variable initialized by constructor
        isVarWithConstructor(rootNav)
       )
      ) 
     ) // documented variable
  {
    addVariable(rootNav,isFuncPtr);
  }
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *e;
    for (;(e=eli.current());++eli)
    {
      if (e->section()!=Entry::ENUM_SEC) 
      {
        buildVarList(e);
      }
    }
  }
}

//----------------------------------------------------------------------
// Searches the Entry tree for Function sections.
// If found they are stored in their class or in the global list.

static void addMethodToClass(EntryNav *rootNav,ClassDef *cd,
                  const QCString &rname,bool isFriend)
{
  Entry *root = rootNav->entry();
  FileDef *fd=rootNav->fileDef();

  int l;
  static QRegExp re("([a-z_A-Z0-9: ]*[ &*]+[ ]*");
  int ts=root->type.find('<');
  int te=root->type.findRev('>');
  int i=re.match(root->type,0,&l);
  if (i!=-1 && ts!=-1 && ts<te && ts<i && i<te) // avoid changing A<int(int*)>, see bug 677315
  {
    i=-1;
  }

  if (cd->getLanguage()==SrcLangExt_Cpp && // only C has pointers
      !root->type.isEmpty() && (root->spec&Entry::Alias)==0 && i!=-1) // function variable
  {
    root->args+=root->type.right(root->type.length()-i-l);
    root->type=root->type.left(i+l);
  }

  QCString name=removeRedundantWhiteSpace(rname);
  if (name.left(2)=="::") name=name.right(name.length()-2);

  MemberDef::MemberType mtype;
  if (isFriend)                 mtype=MemberDef::Friend;
  else if (root->mtype==Signal) mtype=MemberDef::Signal;
  else if (root->mtype==Slot)   mtype=MemberDef::Slot;
  else if (root->mtype==DCOP)   mtype=MemberDef::DCOP;
  else                          mtype=MemberDef::Function;

  // strip redundant template specifier for constructors
  if ((fd==0 || fd->getLanguage()==SrcLangExt_Cpp) &&
     name.left(9)!="operator " && (i=name.find('<'))!=-1 && name.find('>')!=-1)
  {
    name=name.left(i); 
  }

  //printf("root->name=`%s; root->args=`%s' root->argList=`%s'\n", 
  //    root->name.data(),root->args.data(),argListToString(root->argList).data()
  //   );

  // adding class member
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,root->exception,
      root->protection,root->virt,
      root->stat && root->relatesType != MemberOf,
      root->relates.isEmpty() ? Member :
          root->relatesType == MemberOf ? Foreign : Related,
      mtype,root->tArgLists ? root->tArgLists->last() : 0,root->argList);
  md->setTagInfo(rootNav->tagInfo());
  md->setMemberClass(cd);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setDocsForDefinition(!root->proto);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->setBodySegment(root->bodyLine,root->endBodyLine);
  md->setMemberSpecifiers(root->spec);
  md->setMemberGroupId(root->mGrpId);
  md->setTypeConstraints(root->typeConstr);
  md->setLanguage(root->lang);
  md->setBodyDef(fd);
  md->setFileDef(fd);
  //md->setScopeTemplateArguments(root->tArgList);
  md->addSectionsToDefinition(root->anchors);
  QCString def;
  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  SrcLangExt lang = cd->getLanguage();
  QCString scopeSeparator=getLanguageSpecificSeparator(lang);
  if (scopeSeparator!="::")
  {
    qualScope = substitute(qualScope,"::",scopeSeparator);
  }
  if (lang==SrcLangExt_PHP)
  {
    // for PHP we use Class::method and Namespace\method
    scopeSeparator="::";
  }
  if (!root->relates.isEmpty() || isFriend || Config_getBool("HIDE_SCOPE_NAMES"))
  {
    if (!root->type.isEmpty())
    {
      if (root->argList)
      {
        def=root->type+" "+name;
      }
      else
      {
        def=root->type+" "+name+root->args;
      }
    }
    else
    {
      if (root->argList)
      {
        def=name;
      }
      else
      {
        def=name+root->args;
      }
    }
  }
  else
  {
    if (!root->type.isEmpty())
    {
      if (root->argList)
      {
        def=root->type+" "+qualScope+scopeSeparator+name;
      }
      else
      {
        def=root->type+" "+qualScope+scopeSeparator+name+root->args;
      }
    }
    else
    {
      if (root->argList)
      {
        def=qualScope+scopeSeparator+name;
      }
      else
      {
        def=qualScope+scopeSeparator+name+root->args;
      }
    }
  }
  if (def.left(7)=="friend ") def=def.right(def.length()-7);
  md->setDefinition(def);
  md->enableCallGraph(root->callGraph);
  md->enableCallerGraph(root->callerGraph);

  Debug::print(Debug::Functions,0,
      "  Func Member:\n"
      "    `%s' `%s'::`%s' `%s' proto=%d\n"
      "    def=`%s'\n",
      root->type.data(),
      qualScope.data(),
      rname.data(),
      root->args.data(),
      root->proto,
      def.data()
              );

  // add member to the global list of all members
  //printf("Adding member=%s class=%s\n",md->name().data(),cd->name().data());
  MemberName *mn;
  if ((mn=Doxygen::memberNameSDict->find(name)))
  {
    mn->append(md);
  }
  else
  {
    mn = new MemberName(name);
    mn->append(md);
    Doxygen::memberNameSDict->append(name,mn);
  }

  // add member to the class cd
  cd->insertMember(md);
  // add file to list of used files
  cd->insertUsedFile(root->fileName);

  addMemberToGroups(root,md);
  rootNav->changeSection(Entry::EMPTY_SEC);
  md->setRefItems(root->sli);
}


static void buildFunctionList(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::FUNCTION_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    Debug::print(Debug::Functions,0,
                 "FUNCTION_SEC:\n"
                 "  `%s' `%s'::`%s' `%s' relates=`%s' relatesType=`%d' file=`%s' line=`%d' bodyLine=`%d' #tArgLists=%d mGrpId=%d spec=%d proto=%d docFile=%s\n",
                 root->type.data(),
                 rootNav->parent()->name().data(),
                 root->name.data(),
                 root->args.data(),
                 root->relates.data(),
                 root->relatesType,
                 root->fileName.data(),
                 root->startLine,
                 root->bodyLine,
                 root->tArgLists ? (int)root->tArgLists->count() : -1,
                 root->mGrpId,
                 root->spec,
                 root->proto,
                 root->docFile.data()
                );

    bool isFriend=root->type.find("friend ")!=-1;
    QCString rname = removeRedundantWhiteSpace(root->name);
    //printf("rname=%s\n",rname.data());

    QCString scope=rootNav->parent()->name(); //stripAnonymousNamespaceScope(root->parent->name);
    if (!rname.isEmpty() && scope.find('@')==-1)
    {
      ClassDef *cd=0;
      // check if this function's parent is a class
      scope=stripTemplateSpecifiersFromScope(scope,FALSE);

      FileDef *rfd=rootNav->fileDef();

      int memIndex=rname.findRev("::");

      cd=getClass(scope);
      if (cd && scope+"::"==rname.left(scope.length()+2)) // found A::f inside A
      {
        // strip scope from name
        rname=rname.right(rname.length()-rootNav->parent()->name().length()-2); 
      }

      NamespaceDef *nd = 0;
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
          //nd = Doxygen::namespaceSDict->find(rname.left(memIndex));
          //isMember = nd==0;
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

      static QRegExp re("([a-z_A-Z0-9: ]*[ &*]+[ ]*");
      int ts=root->type.find('<');
      int te=root->type.findRev('>');
      int ti;
      if (!rootNav->parent()->name().isEmpty() &&
          (rootNav->parent()->section() & Entry::COMPOUND_MASK) && 
          cd &&
          // do some fuzzy things to exclude function pointers 
          (root->type.isEmpty() || 
           ((ti=root->type.find(re,0))==-1 ||      // type does not contain ..(..* 
            (ts!=-1 && ts<te && ts<ti && ti<te) || // outside of < ... >
           root->args.find(")[")!=-1) ||           // and args not )[.. -> function pointer
           root->type.find(")(")!=-1 || root->type.find("operator")!=-1 || // type contains ..)(.. and not "operator"
           cd->getLanguage()!=SrcLangExt_Cpp                               // language other than C
          )
         )
      {
        Debug::print(Debug::Functions,0,"  --> member %s of class %s!\n",
            rname.data(),cd->name().data());
        addMethodToClass(rootNav,cd,rname,isFriend);
      }
      else if (!((rootNav->parent()->section() & Entry::COMPOUND_MASK) 
                 || rootNav->parent()->section()==Entry::OBJCIMPL_SEC
                ) &&
               !isMember &&
               (root->relates.isEmpty() || root->relatesType == Duplicate) &&
               root->type.left(7)!="extern " && root->type.left(8)!="typedef " 
              )
      // no member => unrelated function 
      {
        /* check the uniqueness of the function name in the file.
         * A file could contain a function prototype and a function definition
         * or even multiple function prototypes.
         */
        bool found=FALSE;
        MemberName *mn;
        MemberDef *md=0;
        if ((mn=Doxygen::functionNameSDict->find(rname)))
        {
          Debug::print(Debug::Functions,0,"  --> function %s already found!\n",rname.data());
          MemberNameIterator mni(*mn);
          for (mni.toFirst();(!found && (md=mni.current()));++mni)
          {
            NamespaceDef *mnd = md->getNamespaceDef();
            NamespaceDef *rnd = 0;
            //printf("root namespace=%s\n",rootNav->parent()->name().data());
            QCString fullScope = scope;
            QCString parentScope = rootNav->parent()->name();
            if (!parentScope.isEmpty() && !leftScopeMatch(parentScope,scope))
            {
              if (!scope.isEmpty()) fullScope.prepend("::");
              fullScope.prepend(parentScope);
            }
            //printf("fullScope=%s\n",fullScope.data());
            rnd = getResolvedNamespace(fullScope);
            FileDef *mfd = md->getFileDef();
            QCString nsName,rnsName;
            if (mnd)  nsName = mnd->name().copy();
            if (rnd) rnsName = rnd->name().copy();
            //printf("matching arguments for %s%s %s%s\n",
            //    md->name().data(),md->argsString(),rname.data(),argListToString(root->argList).data());
            LockingPtr<ArgumentList> mdAl = md->argumentList();
            LockingPtr<ArgumentList> mdTempl = md->templateArguments();

            // in case of template functions, we need to check if the
            // functions have the same number of template parameters
            bool sameNumTemplateArgs = TRUE;
            if (mdTempl!=0 && root->tArgLists)
            {
              if (mdTempl->count()!=root->tArgLists->getLast()->count())
              {
                sameNumTemplateArgs = FALSE;
              }
            }

            bool staticsInDifferentFiles = 
                    root->stat && md->isStatic() && root->fileName!=md->getDefFileName();

            if (
                matchArguments2(md->getOuterScope(),mfd,mdAl.pointer(),
                                rnd ? rnd : Doxygen::globalScope,rfd,root->argList,
                                FALSE) &&
                sameNumTemplateArgs && 
                !staticsInDifferentFiles
               )
            {
              GroupDef *gd=0;
              if (root->groups->first()!=0)
              {
                gd = Doxygen::groupSDict->find(root->groups->first()->groupname.data());
              }
              //printf("match!\n");
              //printf("mnd=%p rnd=%p nsName=%s rnsName=%s\n",mnd,rnd,nsName.data(),rnsName.data());
              // see if we need to create a new member
              found=(mnd && rnd && nsName==rnsName) ||   // members are in the same namespace
                    ((mnd==0 && rnd==0 && mfd!=0 &&       // no external reference and
                      mfd->absFilePath()==root->fileName // prototype in the same file
                     )
                    );
              // otherwise, allow a duplicate global member with the same argument list
              if (!found && gd && gd==md->getGroupDef() && nsName==rnsName)
              {
                // member is already in the group, so we don't want to add it again.
                found=TRUE;
              }

              //printf("combining function with prototype found=%d in namespace %s\n",
              //    found,nsName.data());

              if (found)
              {
                // merge argument lists
                mergeArguments(mdAl.pointer(),root->argList,!root->doc.isEmpty());
                // merge documentation
                if (md->documentation().isEmpty() && !root->doc.isEmpty())
                {
                  ArgumentList *argList = new ArgumentList;
                  stringToArgumentList(root->args,argList);
                  if (root->proto)
                  {
                    //printf("setDeclArgumentList to %p\n",argList);
                    md->setDeclArgumentList(argList);
                  }
                  else
                  {
                    md->setArgumentList(argList);
                  }
                }

                md->setDocumentation(root->doc,root->docFile,root->docLine);
                md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                md->setDocsForDefinition(!root->proto);
                if (md->getStartBodyLine()!=-1 && md->getStartBodyLine()==-1)
                {
                  md->setBodySegment(root->bodyLine,root->endBodyLine);
                  md->setBodyDef(rfd);
                }

                if (md->briefDescription().isEmpty() && !root->brief.isEmpty())
                {
                  md->setArgsString(root->args);
                }
                md->setBriefDescription(root->brief,root->briefFile,root->briefLine);

                md->addSectionsToDefinition(root->anchors);

                md->enableCallGraph(md->hasCallGraph() || root->callGraph);
                md->enableCallerGraph(md->hasCallerGraph() || root->callerGraph);

                // merge ingroup specifiers
                if (md->getGroupDef()==0 && root->groups->first()!=0)
                {
                  addMemberToGroups(root,md);
                }
                else if (md->getGroupDef()!=0 && root->groups->count()==0)
                {
                  //printf("existing member is grouped, new member not\n");
                  root->groups->append(new Grouping(md->getGroupDef()->name(), md->getGroupPri()));
                }
                else if (md->getGroupDef()!=0 && root->groups->first()!=0)
                {
                  //printf("both members are grouped\n");
                }

                // if md is a declaration and root is the corresponding
                // definition, then turn md into a definition.
                if (md->isPrototype() && !root->proto)
                {
                  md->setPrototype(FALSE);
                }
              }
            }
          }
        }
        if (!found) /* global function is unique with respect to the file */
        {
          Debug::print(Debug::Functions,0,"  --> new function %s found!\n",rname.data());
          //printf("New function type=`%s' name=`%s' args=`%s' bodyLine=%d\n",
          //       root->type.data(),rname.data(),root->args.data(),root->bodyLine);

          // new global function
          ArgumentList *tArgList = root->tArgLists ? root->tArgLists->last() : 0;
          QCString name=removeRedundantWhiteSpace(rname);
          md=new MemberDef(
              root->fileName,root->startLine,
              root->type,name,root->args,root->exception,
              root->protection,root->virt,root->stat,Member,
              MemberDef::Function,tArgList,root->argList);

          md->setTagInfo(rootNav->tagInfo());
          md->setLanguage(root->lang);
          //md->setDefFile(root->fileName);
          //md->setDefLine(root->startLine);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setPrototype(root->proto);
          md->setDocsForDefinition(!root->proto);
          md->setTypeConstraints(root->typeConstr);
          //md->setBody(root->body);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          FileDef *fd=rootNav->fileDef();
          md->setBodyDef(fd);
          md->addSectionsToDefinition(root->anchors);
          md->setMemberSpecifiers(root->spec);
          md->setMemberGroupId(root->mGrpId);

          // see if the function is inside a namespace that was not part of
          // the name already (in that case nd should be non-zero already)
          if (nd==0 && rootNav->parent()->section() == Entry::NAMESPACE_SEC )
          {
            //QCString nscope=removeAnonymousScopes(rootNav->parent()->name());
            QCString nscope=rootNav->parent()->name();
            if (!nscope.isEmpty())
            {
              nd = getResolvedNamespace(nscope);
            }
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

          QCString def;
          if (!root->type.isEmpty())
          {
            if (root->argList)
            {
              def=root->type+" "+scope+name;
            }
            else
            {
              def=root->type+" "+scope+name+root->args;
            }
          }
          else
          {
            if (root->argList)
            {
              def=scope+name.copy();
            }
            else
            {
              def=scope+name+root->args;
            }
          }
          Debug::print(Debug::Functions,0,
                     "  Global Function:\n"
                     "    `%s' `%s'::`%s' `%s' proto=%d\n"
                     "    def=`%s'\n",
                     root->type.data(),
                     rootNav->parent()->name().data(),
                     rname.data(),
                     root->args.data(),
                     root->proto,
                     def.data()
                    );
          md->setDefinition(def);
          md->enableCallGraph(root->callGraph);
          md->enableCallerGraph(root->callerGraph);
          //if (root->mGrpId!=-1) 
          //{
          //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
          //}

          md->setRefItems(root->sli);
          if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
          {
            // add member to namespace
            md->setNamespace(nd);
            nd->insertMember(md); 
          }
          if (fd)
          {
            // add member to the file (we do this even if we have already
            // inserted it into the namespace)
            md->setFileDef(fd); 
            fd->insertMember(md);
          }

          // add member to the list of file members
          //printf("Adding member=%s\n",md->name().data());
          MemberName *mn;
          if ((mn=Doxygen::functionNameSDict->find(name)))
          {
            mn->append(md);
          }
          else 
          {
            mn = new MemberName(name);
            mn->append(md);
            Doxygen::functionNameSDict->append(name,mn);
          }
          addMemberToGroups(root,md);
          if (root->relatesType == Simple) // if this is a relatesalso command,
                                           // allow find Member to pick it up
          {
            rootNav->changeSection(Entry::EMPTY_SEC); // Otherwise we have finished 
                                                      // with this entry.

          }
        }
        else
        {
          FileDef *fd=rootNav->fileDef();
          if (fd)
          {
            // add member to the file (we do this even if we have already
            // inserted it into the namespace)
            fd->insertMember(md);
          }
        }

        //printf("unrelated function %d `%s' `%s' `%s'\n",
        //    root->parent->section,root->type.data(),rname.data(),root->args.data());
      }
      else
      {
          Debug::print(Debug::Functions,0,"  --> %s not processed!\n",rname.data());
      }
    }
    else if (rname.isEmpty())
    {
        warn(root->fileName,root->startLine,
             "warning: Illegal member name found."
            );
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildFunctionList,rootNav);
}

//----------------------------------------------------------------------

static void findFriends()
{
  //printf("findFriends()\n");
  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    //printf("Function name=`%s'\n",fn->memberName());
    MemberName *mn;
    if ((mn=Doxygen::memberNameSDict->find(fn->memberName())))
    { // there are members with the same name
      //printf("Function name is also a member name\n");
      MemberNameIterator fni(*fn);
      MemberDef *fmd;
      for (;(fmd=fni.current());++fni) // for each function with that name
      {
        MemberNameIterator mni(*mn);
        MemberDef *mmd;
        for (;(mmd=mni.current());++mni) // for each member with that name
        {
          //printf("Checking for matching arguments 
          //        mmd->isRelated()=%d mmd->isFriend()=%d mmd->isFunction()=%d\n",
          //    mmd->isRelated(),mmd->isFriend(),mmd->isFunction());
          LockingPtr<ArgumentList> mmdAl = mmd->argumentList();
          LockingPtr<ArgumentList> fmdAl = fmd->argumentList();
          if ((mmd->isFriend() || (mmd->isRelated() && mmd->isFunction())) &&
              matchArguments2(mmd->getOuterScope(), mmd->getFileDef(), mmdAl.pointer(),
                              fmd->getOuterScope(), fmd->getFileDef(), fmdAl.pointer(),
                              TRUE
                             )
                             
             ) // if the member is related and the arguments match then the 
               // function is actually a friend.
          {
            mergeArguments(mmdAl.pointer(),fmdAl.pointer());
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
              mmd->setBodySegment(fmd->getStartBodyLine(),fmd->getEndBodyLine());
              mmd->setBodyDef(fmd->getBodyDef());
              //mmd->setBodyMember(fmd);
            }
            else if (mmd->getStartBodyLine()!=-1 && fmd->getStartBodyLine()==-1)
            {
              fmd->setBodySegment(mmd->getStartBodyLine(),mmd->getEndBodyLine());
              fmd->setBodyDef(mmd->getBodyDef());
              //fmd->setBodyMember(mmd);
            }
            mmd->setDocsForDefinition(fmd->isDocsForDefinition());

            mmd->enableCallGraph(mmd->hasCallGraph() || fmd->hasCallGraph());
            mmd->enableCallerGraph(mmd->hasCallerGraph() || fmd->hasCallerGraph());
            fmd->enableCallGraph(mmd->hasCallGraph() || fmd->hasCallGraph());
            fmd->enableCallerGraph(mmd->hasCallerGraph() || fmd->hasCallerGraph());
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferFunctionDocumentation()
{
  //printf("---- transferFunctionDocumentation()\n");

  // find matching function declaration and definitions.
  MemberNameSDict::Iterator mnli(*Doxygen::functionNameSDict);
  MemberName *mn;
  for (;(mn=mnli.current());++mnli)
  {
    //printf("memberName=%s count=%d\n",mn->memberName(),mn->count());
    MemberDef *mdef=0,*mdec=0;
    MemberNameIterator mni1(*mn);
    /* find a matching function declaration and definition for this function */
    for (;(mdec=mni1.current());++mni1)
    {
      if (mdec->isPrototype() ||
          (mdec->isVariable() && mdec->isExternal()) 
         )
      {
        MemberNameIterator mni2(*mn);
        for (;(mdef=mni2.current());++mni2)
        {
          combineDeclarationAndDefinition(mdec,mdef);
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferFunctionReferences()
{
  MemberNameSDict::Iterator mnli(*Doxygen::functionNameSDict);
  MemberName *mn;
  for (;(mn=mnli.current());++mnli)
  {
    MemberDef *md,*mdef=0,*mdec=0;
    MemberNameIterator mni(*mn);
    /* find a matching function declaration and definition for this function */
    for (;(md=mni.current());++mni)
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
    if (mdef && mdec)
    {
      LockingPtr<ArgumentList> mdefAl = mdef->argumentList();
      LockingPtr<ArgumentList> mdecAl = mdec->argumentList();
      if (
          matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdefAl.pointer(),
                          mdec->getOuterScope(),mdec->getFileDef(),mdecAl.pointer(),
                          TRUE
            )
         ) /* match found */
      {
        LockingPtr<MemberSDict> defDict = mdef->getReferencesMembers();
        LockingPtr<MemberSDict> decDict = mdec->getReferencesMembers();
        if (defDict!=0)
        {
          MemberSDict::Iterator msdi(*defDict);
          MemberDef *rmd;
          for (msdi.toFirst();(rmd=msdi.current());++msdi)
          {
            if (decDict==0 || decDict->find(rmd->name())==0)
            {
              mdec->addSourceReferences(rmd);
            }
          }
        }
        if (decDict!=0)
        {
          MemberSDict::Iterator msdi(*decDict);
          MemberDef *rmd;
          for (msdi.toFirst();(rmd=msdi.current());++msdi)
          {
            if (defDict==0 || defDict->find(rmd->name())==0)
            {
              mdef->addSourceReferences(rmd);
            }
          }
        }

        defDict = mdef->getReferencedByMembers();
        decDict = mdec->getReferencedByMembers();
        if (defDict!=0)
        {
          MemberSDict::Iterator msdi(*defDict);
          MemberDef *rmd;
          for (msdi.toFirst();(rmd=msdi.current());++msdi)
          {
            if (decDict==0 || decDict->find(rmd->name())==0)
            {
              mdec->addSourceReferencedBy(rmd);
            }
          }
        }
        if (decDict!=0)
        {
          MemberSDict::Iterator msdi(*decDict);
          MemberDef *rmd;
          for (msdi.toFirst();(rmd=msdi.current());++msdi)
          {
            if (defDict==0 || defDict->find(rmd->name())==0)
            {
              mdef->addSourceReferencedBy(rmd);
            }
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferRelatedFunctionDocumentation()
{
  // find match between function declaration and definition for 
  // related functions
  MemberNameSDict::Iterator mnli(*Doxygen::functionNameSDict);
  MemberName *mn;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    /* find a matching function declaration and definition for this function */
    for (mni.toFirst();(md=mni.current());++mni) // for each global function
    {
      //printf("  Function `%s'\n",md->name().data());
      MemberName *rmn;
      if ((rmn=Doxygen::memberNameSDict->find(md->name()))) // check if there is a member with the same name
      {
        //printf("  Member name found\n");
        MemberDef *rmd;
        MemberNameIterator rmni(*rmn);
        for (rmni.toFirst();(rmd=rmni.current());++rmni) // for each member with the same name
        {
          LockingPtr<ArgumentList>  mdAl = md->argumentList();
          LockingPtr<ArgumentList> rmdAl = rmd->argumentList();
          //printf("  Member found: related=`%d'\n",rmd->isRelated());
          if ((rmd->isRelated() || rmd->isForeign()) && // related function
              matchArguments2( md->getOuterScope(), md->getFileDef(), mdAl.pointer(),
                              rmd->getOuterScope(),rmd->getFileDef(),rmdAl.pointer(),
                              TRUE
                             )
             )
          {
            //printf("  Found related member `%s'\n",md->name().data());
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

//----------------------------------------------------------------------

/*! make a dictionary of all template arguments of class cd
 * that are part of the base class name. 
 * Example: A template class A with template arguments <R,S,T> 
 * that inherits from B<T,T,S> will have T and S in the dictionary.
 */
static QDict<int> *getTemplateArgumentsInName(ArgumentList *templateArguments,const QCString &name)
{
  QDict<int> *templateNames = new QDict<int>(17);
  templateNames->setAutoDelete(TRUE);
  static QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
  if (templateArguments)
  {
    ArgumentListIterator ali(*templateArguments);
    Argument *arg;
    int count=0;
    for (ali.toFirst();(arg=ali.current());++ali,count++)
    {
      int i,p=0,l;
      while ((i=re.match(name,p,&l))!=-1)
      {
        QCString n = name.mid(i,l);
        if (n==arg->name)
        {
          if (templateNames->find(n)==0)
          {
            templateNames->insert(n,new int(count));
          }
        }
        p=i+l;
      }
    }
  }
  return templateNames;
}

/*! Searches a class from within \a context and \a cd and returns its
 *  definition if found (otherwise 0 is returned).
 */
static ClassDef *findClassWithinClassContext(Definition *context,ClassDef *cd,const QCString &name)
{
  FileDef *fd=cd->getFileDef();
  ClassDef *result=0;
  if (context && cd!=context)
  {
    result = getResolvedClass(context,0,name,0,0,TRUE,TRUE);
  }
  if (result==0)
  {
    result = getResolvedClass(cd,fd,name,0,0,TRUE,TRUE);
  }
  if (result==0) // try direct class, needed for namespaced classes imported via tag files (see bug624095)
  {
    result = getClass(name);
  }
  if (result==0 && cd && cd->getLanguage()==SrcLangExt_CSharp && name.find('<')!=-1)
  {
    result = Doxygen::genericsDict.find(name);
  }
  //printf("** Trying to find %s within context %s class %s result=%s lookup=%p\n",
  //       name.data(),
  //       context ? context->name().data() : "<none>",
  //       cd      ? cd->name().data()      : "<none>",
  //       result  ? result->name().data()  : "<none>",
  //       Doxygen::classSDict->find(name)
  //      );
  return result;
}

enum FindBaseClassRelation_Mode 
{ 
  TemplateInstances, 
  DocumentedOnly, 
  Undocumented 
};

static bool findClassRelation(
                           EntryNav *rootNav,
                           Definition *context,
                           ClassDef *cd,
                           BaseInfo *bi,
                           QDict<int> *templateNames,
                           /*bool insertUndocumented*/
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          );


static void findUsedClassesForClass(EntryNav *rootNav,
                           Definition *context,
                           ClassDef *masterCd,
                           ClassDef *instanceCd,
                           bool isArtificial,
                           ArgumentList *actualArgs=0,
                           QDict<int> *templateNames=0
                           )
{
  masterCd->visited=TRUE;
  ArgumentList *formalArgs = masterCd->templateArguments();
  if (masterCd->memberNameInfoSDict())
  {
    MemberNameInfoSDict::Iterator mnili(*masterCd->memberNameInfoSDict());
    MemberNameInfo *mni;
    for (;(mni=mnili.current());++mnili)
    {
      MemberNameInfoIterator mnii(*mni);
      MemberInfo *mi;
      for (mnii.toFirst();(mi=mnii.current());++mnii)
      {
        MemberDef *md=mi->memberDef;
        if (md->isVariable() || md->isObjCProperty()) // for each member variable in this class
        {
          //printf("    Found variable %s in class %s\n",md->name().data(),masterCd->name().data());
          QCString type=removeRedundantWhiteSpace(md->typeString());
          QCString typedefValue = resolveTypeDef(masterCd,type);
          if (!typedefValue.isEmpty())
          {
            type = typedefValue;
          }
          int pos=0;
          QCString usedClassName;
          QCString templSpec;
          bool found=FALSE;
          // the type can contain template variables, replace them if present
          if (actualArgs)
          {
            type = substituteTemplateArgumentsInString(type,formalArgs,actualArgs);
          }

          //printf("      template substitution gives=%s\n",type.data());
          while (!found && extractClassNameFromType(type,pos,usedClassName,templSpec,rootNav->lang())!=-1)
          {
            // find the type (if any) that matches usedClassName
            ClassDef *typeCd = getResolvedClass(masterCd,
                masterCd->getFileDef(),
                usedClassName,
                0,0,
                FALSE,TRUE
                );
            //printf("====>  usedClassName=%s -> typeCd=%s\n",
            //     usedClassName.data(),typeCd?typeCd->name().data():"<none>");
            if (typeCd)
            {
              usedClassName = typeCd->name();
            }

            int sp=usedClassName.find('<');
            if (sp==-1) sp=0;
            int si=usedClassName.findRev("::",sp);
            if (si!=-1)
            {
              // replace any namespace aliases
              replaceNamespaceAliases(usedClassName,si);
            }
            // add any template arguments to the class
            QCString usedName = removeRedundantWhiteSpace(usedClassName+templSpec);
            //printf("    usedName=%s\n",usedName.data());

            bool delTempNames=FALSE;
            if (templateNames==0)
            {
              templateNames = getTemplateArgumentsInName(formalArgs,usedName);
              delTempNames=TRUE;
            }
            BaseInfo bi(usedName,Public,Normal);
            findClassRelation(rootNav,context,instanceCd,&bi,templateNames,TemplateInstances,isArtificial);

            if (masterCd->templateArguments())
            {
              ArgumentListIterator ali(*masterCd->templateArguments());
              Argument *arg;
              int count=0;
              for (ali.toFirst();(arg=ali.current());++ali,++count)
              {
                if (arg->name==usedName) // type is a template argument
                {
                  found=TRUE;
                  Debug::print(Debug::Classes,0,"    New used class `%s'\n", usedName.data());

                  ClassDef *usedCd = Doxygen::hiddenClasses->find(usedName);
                  if (usedCd==0)
                  {
                    usedCd = new ClassDef(
                        masterCd->getDefFileName(),masterCd->getDefLine(),
                        usedName,ClassDef::Class);
                    //printf("making %s a template argument!!!\n",usedCd->name().data());
                    usedCd->makeTemplateArgument();
                    usedCd->setUsedOnly(TRUE);
                    usedCd->setLanguage(masterCd->getLanguage());
                    Doxygen::hiddenClasses->append(usedName,usedCd);
                  }
                  if (usedCd)
                  {
                    if (isArtificial) usedCd->setArtificial(TRUE);
                    Debug::print(Debug::Classes,0,"      Adding used class `%s' (1)\n", usedCd->name().data());
                    instanceCd->addUsedClass(usedCd,md->name(),md->protection());
                    usedCd->addUsedByClass(instanceCd,md->name(),md->protection());
                  }
                }
              }
            }

            if (!found)
            {
              ClassDef *usedCd=findClassWithinClassContext(context,masterCd,usedName);
              //printf("Looking for used class %s: result=%s master=%s\n",
              //    usedName.data(),usedCd?usedCd->name().data():"<none>",masterCd?masterCd->name().data():"<none>");

              if (usedCd) 
              {
                found=TRUE;
                Debug::print(Debug::Classes,0,"    Adding used class `%s' (2)\n", usedCd->name().data());
                instanceCd->addUsedClass(usedCd,md->name(),md->protection()); // class exists 
                usedCd->addUsedByClass(instanceCd,md->name(),md->protection());
              }
            }
            if (delTempNames)
            {
              delete templateNames;
              templateNames=0;
            }
          }
          if (!found && !type.isEmpty()) // used class is not documented in any scope
          {
            ClassDef *usedCd = Doxygen::hiddenClasses->find(type);
            if (usedCd==0 && !Config_getBool("HIDE_UNDOC_RELATIONS"))
            {
              if (type.right(2)=="(*" || type.right(2)=="(^") // type is a function pointer
              {
                type+=md->argsString();
              }
              Debug::print(Debug::Classes,0,"  New undocumented used class `%s'\n", type.data());
              usedCd = new ClassDef(
                  masterCd->getDefFileName(),masterCd->getDefLine(),
                  type,ClassDef::Class);
              usedCd->setUsedOnly(TRUE);
              usedCd->setLanguage(masterCd->getLanguage());
              Doxygen::hiddenClasses->append(type,usedCd);
            }
            if (usedCd)
            {
              if (isArtificial) usedCd->setArtificial(TRUE);
              Debug::print(Debug::Classes,0,"    Adding used class `%s' (3)\n", usedCd->name().data());
              instanceCd->addUsedClass(usedCd,md->name(),md->protection()); 
              usedCd->addUsedByClass(instanceCd,md->name(),md->protection());
            }
          }
        }
      }
    }
  }
  else
  {
    //printf("no members for class %s (%p)\n",masterCd->name().data(),masterCd);
  }
}

static void findBaseClassesForClass(
      EntryNav *rootNav,
      Definition *context,
      ClassDef *masterCd,
      ClassDef *instanceCd,
      FindBaseClassRelation_Mode mode,
      bool isArtificial,
      ArgumentList *actualArgs=0,
      QDict<int> *templateNames=0
    )
{
  Entry *root = rootNav->entry();
  //if (masterCd->visited) return;
  masterCd->visited=TRUE;
  // The base class could ofcouse also be a non-nested class
  ArgumentList *formalArgs = masterCd->templateArguments();
  QListIterator<BaseInfo> bii(*root->extends);
  BaseInfo *bi=0;
  for (bii.toFirst();(bi=bii.current());++bii)
  {
    //printf("masterCd=%s bi->name='%s' #actualArgs=%d\n",
    //    masterCd->localName().data(),bi->name.data(),actualArgs?(int)actualArgs->count():-1);
    bool delTempNames=FALSE;
    if (templateNames==0)
    {
      templateNames = getTemplateArgumentsInName(formalArgs,bi->name);
      delTempNames=TRUE;
    }
    BaseInfo tbi(bi->name,bi->prot,bi->virt);
    if (actualArgs) // substitute the formal template arguments of the base class
    {
      tbi.name = substituteTemplateArgumentsInString(bi->name,formalArgs,actualArgs);
    }
    //printf("bi->name=%s tbi.name=%s\n",bi->name.data(),tbi.name.data());

    if (mode==DocumentedOnly)
    {
      // find a documented base class in the correct scope
      if (!findClassRelation(rootNav,context,instanceCd,&tbi,templateNames,DocumentedOnly,isArtificial))
      {
        // 1.8.2: decided to show inheritance relations even if not documented,
        //        we do make them artificial, so they do not appear in the index
        //if (!Config_getBool("HIDE_UNDOC_RELATIONS")) 
        bool b = Config_getBool("HIDE_UNDOC_RELATIONS") ? TRUE : isArtificial;
        //{
          // no documented base class -> try to find an undocumented one
          findClassRelation(rootNav,context,instanceCd,&tbi,templateNames,Undocumented,b);
        //}
      }
    }
    else if (mode==TemplateInstances)
    {
      findClassRelation(rootNav,context,instanceCd,&tbi,templateNames,TemplateInstances,isArtificial);
    }
    if (delTempNames)
    {
      delete templateNames;
      templateNames=0;
    }  
  }
}

//----------------------------------------------------------------------

static bool findTemplateInstanceRelation(Entry *root,
            Definition *context,
            ClassDef *templateClass,const QCString &templSpec,
            QDict<int> *templateNames,
            bool isArtificial)
{
  Debug::print(Debug::Classes,0,"    derived from template %s with parameters %s\n",
         templateClass->name().data(),templSpec.data());
  //printf("findTemplateInstanceRelation(base=%s templSpec=%s templateNames=",
  //    templateClass->name().data(),templSpec.data());
  //if (templateNames)
  //{
  //  QDictIterator<int> qdi(*templateNames);
  //  int *tempArgIndex;
  //  for (;(tempArgIndex=qdi.current());++qdi)
  //  {
  //    printf("(%s->%d) ",qdi.currentKey(),*tempArgIndex);
  //  }
  //}
  //printf("\n");
  
  bool existingClass = (templSpec ==
                        tempArgListToString(templateClass->templateArguments())
                       );
  if (existingClass) return TRUE;

  bool freshInstance=FALSE;
  ClassDef *instanceClass = templateClass->insertTemplateInstance(
                     root->fileName,root->startLine,templSpec,freshInstance);
  if (isArtificial) instanceClass->setArtificial(TRUE);
  instanceClass->setLanguage(root->lang);

  if (freshInstance)
  {
    Debug::print(Debug::Classes,0,"      found fresh instance '%s'!\n",instanceClass->name().data());
    Doxygen::classSDict->append(instanceClass->name(),instanceClass);
    instanceClass->setTemplateBaseClassNames(templateNames);

    // search for new template instances caused by base classes of 
    // instanceClass 
    EntryNav *templateRootNav = g_classEntries.find(templateClass->name());
    if (templateRootNav)
    {
      bool unloadNeeded=FALSE;
      Entry *templateRoot = templateRootNav->entry();
      if (templateRoot==0) // not yet loaded
      {
        templateRootNav->loadEntry(g_storage);
        templateRoot = templateRootNav->entry();
        ASSERT(templateRoot!=0); // now it should really be loaded
        unloadNeeded=TRUE;
      }

      Debug::print(Debug::Classes,0,"        template root found %s templSpec=%s!\n",
          templateRoot->name.data(),templSpec.data());
      ArgumentList *templArgs = new ArgumentList;
      stringToArgumentList(templSpec,templArgs);
      findBaseClassesForClass(templateRootNav,context,templateClass,instanceClass,
          TemplateInstances,isArtificial,templArgs,templateNames);

      findUsedClassesForClass(templateRootNav,context,templateClass,instanceClass,
          isArtificial,templArgs,templateNames);
      delete templArgs;

      if (unloadNeeded) // still cleanup to do
      {
        templateRootNav->releaseEntry();
      }
    }
    else
    {
      Debug::print(Debug::Classes,0,"        no template root entry found!\n");
      // TODO: what happened if we get here?
    }

    //Debug::print(Debug::Classes,0,"    Template instance %s : \n",instanceClass->name().data());
    //ArgumentList *tl = templateClass->templateArguments();
  }
  else
  {
    Debug::print(Debug::Classes,0,"      instance already exists!\n");
  }
  return TRUE;
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
static int findEndOfTemplate(const QCString &s,int startPos)
{
  // locate end of template
  int e=startPos;
  int brCount=1;
  int roundCount=0;
  int len = s.length();
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
  return brCount==0 ? e : -1;
}

static bool findClassRelation(
                           EntryNav *rootNav,
                           Definition *context,
                           ClassDef *cd,
                           BaseInfo *bi,
                           QDict<int> *templateNames,
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          )
{
  //printf("findClassRelation(class=%s base=%s templateNames=",
  //    cd->name().data(),bi->name.data());
  //if (templateNames)
  //{
  //  QDictIterator<int> qdi(*templateNames);
  //  int *tempArgIndex;
  //  for (;(tempArgIndex=qdi.current());++qdi)
  //  {
  //    printf("(%s->%d) ",qdi.currentKey(),*tempArgIndex);
  //  }
  //}
  //printf("\n");

  Entry *root = rootNav->entry();

  QCString biName=bi->name;
  bool explicitGlobalScope=FALSE;
  //printf("findClassRelation: biName=`%s'\n",biName.data());
  if (biName.left(2)=="::") // explicit global scope
  {
     biName=biName.right(biName.length()-2);
     explicitGlobalScope=TRUE;
  }

  EntryNav *parentNode=rootNav->parent();
  bool lastParent=FALSE;
  do // for each parent scope, starting with the largest scope 
     // (in case of nested classes)
  {
    QCString scopeName= parentNode ? parentNode->name().data() : "";
    int scopeOffset=explicitGlobalScope ? 0 : scopeName.length();
    do // try all parent scope prefixes, starting with the largest scope
    {
      //printf("scopePrefix=`%s' biName=`%s'\n",
      //    scopeName.left(scopeOffset).data(),biName.data());

      QCString baseClassName=biName;
      if (scopeOffset>0)
      {
        baseClassName.prepend(scopeName.left(scopeOffset)+"::");
      }
      //QCString stripped;
      //baseClassName=stripTemplateSpecifiersFromScope
      //                    (removeRedundantWhiteSpace(baseClassName),TRUE,
      //                    &stripped);
      MemberDef *baseClassTypeDef=0;
      QCString templSpec;
      ClassDef *baseClass=getResolvedClass(explicitGlobalScope ? Doxygen::globalScope : context,
                                           cd->getFileDef(), 
                                           baseClassName,
                                           &baseClassTypeDef,
                                           &templSpec,
                                           mode==Undocumented,
                                           TRUE
                                          );
      //printf("baseClassName=%s baseClass=%p cd=%p explicitGlobalScope=%d\n",
      //    baseClassName.data(),baseClass,cd,explicitGlobalScope);
      //printf("    scope=`%s' baseClassName=`%s' baseClass=%s templSpec=%s\n",
      //                    cd ? cd->name().data():"<none>",
      //                    baseClassName.data(),
      //                    baseClass?baseClass->name().data():"<none>",
      //                    templSpec.data()
      //      );
      //if (baseClassName.left(root->name.length())!=root->name ||
      //    baseClassName.at(root->name.length())!='<'
      //   ) // Check for base class with the same name.
      //     // If found then look in the outer scope for a match
      //     // and prevent recursion.
      if (!isRecursiveBaseClass(rootNav->name(),baseClassName) || explicitGlobalScope)
      {
        Debug::print(
            Debug::Classes,0,"    class relation %s inherited/used by %s found (%s and %s) templSpec='%s'\n",
            baseClassName.data(),
            rootNav->name().data(),
            (bi->prot==Private)?"private":((bi->prot==Protected)?"protected":"public"),
            (bi->virt==Normal)?"normal":"virtual",
            templSpec.data()
           );

        int i=baseClassName.find('<');
        int si=baseClassName.findRev("::",i==-1 ? baseClassName.length() : i);
        if (si==-1) si=0;
        if (baseClass==0 && i!=-1) 
          // base class has template specifiers
        {
          if (root->lang == SrcLangExt_CSharp)
          {
            baseClass = Doxygen::genericsDict.find(baseClassName);
          }
          else
          {
            // TODO: here we should try to find the correct template specialization
            // but for now, we only look for the unspecializated base class.
            int e=findEndOfTemplate(baseClassName,i+1);
            //printf("baseClass==0 i=%d e=%d\n",i,e);
            if (e!=-1) // end of template was found at e
            {
              templSpec=removeRedundantWhiteSpace(baseClassName.mid(i,e-i));
              baseClassName=baseClassName.left(i)+baseClassName.right(baseClassName.length()-e);
              baseClass=getResolvedClass(explicitGlobalScope ? Doxygen::globalScope : context,
                  cd->getFileDef(),
                  baseClassName,
                  &baseClassTypeDef,
                  0, //&templSpec,
                  mode==Undocumented,
                  TRUE
                  );
              //printf("baseClass=%p -> baseClass=%s templSpec=%s\n",
              //      baseClass,baseClassName.data(),templSpec.data());
            }
          }
        }
        else if (baseClass && !templSpec.isEmpty()) // we have a known class, but also
                                                    // know it is a template, so see if
                                                    // we can also link to the explicit
                                                    // instance (for instance if a class
                                                    // derived from a template argument)
        {
          //printf("baseClass=%p templSpec=%s\n",baseClass,templSpec.data());
          ClassDef *templClass=getClass(baseClass->name()+templSpec);
          if (templClass)
          {
            // use the template instance instead of the template base.
            baseClass = templClass;
            templSpec.resize(0);
          }
        }

        //printf("cd=%p baseClass=%p\n",cd,baseClass);
        bool found=baseClass!=0 && (baseClass!=cd || mode==TemplateInstances);
        //printf("1. found=%d\n",found);
        if (!found && si!=-1)
        {
          QCString tmpTemplSpec;
          // replace any namespace aliases
          replaceNamespaceAliases(baseClassName,si);
          baseClass=getResolvedClass(explicitGlobalScope ? Doxygen::globalScope : context,
                                     cd->getFileDef(),
                                     baseClassName,
                                     &baseClassTypeDef,
                                     &tmpTemplSpec,
                                     mode==Undocumented,
                                     TRUE
                                    );
          found=baseClass!=0 && baseClass!=cd;
          if (found) templSpec = tmpTemplSpec;
        }
        //printf("2. found=%d\n",found);
        
        //printf("root->name=%s biName=%s baseClassName=%s\n",
        //        root->name.data(),biName.data(),baseClassName.data());
        //if (cd->isCSharp() && i!=-1) // C# generic -> add internal -g postfix
        //{
        //  baseClassName+="-g";
        //}

        if (!found)
        {
          baseClass=findClassWithinClassContext(context,cd,baseClassName);
          //printf("findClassWithinClassContext(%s,%s)=%p\n",
          //    cd->name().data(),baseClassName.data(),baseClass);
          found = baseClass!=0 && baseClass!=cd;

        }
        if (!found)
        {
          // for PHP the "use A\B as C" construct map class C to A::B, so we lookup
          // the class name also in the alias mapping.
          QCString *aliasName = Doxygen::namespaceAliasDict[baseClassName];
          if (aliasName) // see if it is indeed a class.
          {
            baseClass=getClass(*aliasName);
            found = baseClass!=0 && baseClass!=cd;
          }
        }
        bool isATemplateArgument = templateNames!=0 && templateNames->find(biName)!=0;
        // make templSpec canonical
        // warning: the following line doesn't work for Mixin classes (see bug 560623)
        // templSpec = getCanonicalTemplateSpec(cd, cd->getFileDef(), templSpec);

        //printf("3. found=%d\n",found);
        if (found)
        {
          Debug::print(Debug::Classes,0,"    Documented base class `%s' templSpec=%s\n",biName.data(),templSpec.isEmpty()?"":templSpec.data());
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
            // doxygen
            if (baseClassTypeDef==0)
            {
              //printf("       => findTemplateInstanceRelation: %p\n",baseClassTypeDef);
              findTemplateInstanceRelation(root,context,baseClass,templSpec,templateNames,isArtificial);
            }
          }
          else if (mode==DocumentedOnly || mode==Undocumented)
          {
            //printf("       => insert base class\n");
            QCString usedName;
            if (baseClassTypeDef || cd->isCSharp()) 
            {
              usedName=biName;
              //printf("***** usedName=%s templSpec=%s\n",usedName.data(),templSpec.data());
            }
            if (Config_getBool("SIP_SUPPORT")) bi->prot=Public;
            cd->insertBaseClass(baseClass,usedName,bi->prot,bi->virt,templSpec);
            // add this class as super class to the base class
            baseClass->insertSubClass(cd,bi->prot,bi->virt,templSpec);
          }
          return TRUE;
        }
        else if (mode==Undocumented && (scopeOffset==0 || isATemplateArgument))
        {
          Debug::print(Debug::Classes,0,
                       "    New undocumented base class `%s' baseClassName=%s templSpec=%s isArtificial=%d\n",
                       biName.data(),baseClassName.data(),templSpec.data(),isArtificial
                      );
          baseClass=0;
          if (isATemplateArgument)
          {
            baseClass=Doxygen::hiddenClasses->find(baseClassName);
            if (baseClass==0)
            {
              baseClass=new ClassDef(root->fileName,root->startLine,
                                 baseClassName,ClassDef::Class);
              Doxygen::hiddenClasses->append(baseClassName,baseClass);
              if (isArtificial) baseClass->setArtificial(TRUE);
              baseClass->setLanguage(root->lang);
            }
          }
          else
          {
            baseClass=Doxygen::classSDict->find(baseClassName);
            //printf("*** classDDict->find(%s)=%p biName=%s templSpec=%s\n",
            //    baseClassName.data(),baseClass,biName.data(),templSpec.data());
            if (baseClass==0)
            {
              baseClass=new ClassDef(root->fileName,root->startLine,
                  baseClassName,ClassDef::Class);
              Doxygen::classSDict->append(baseClassName,baseClass);
              if (isArtificial) baseClass->setArtificial(TRUE);
              baseClass->setLanguage(root->lang);
            }
          }
          if (biName.right(2)=="-p")
          {
            biName="<"+biName.left(biName.length()-2)+">";
          }
          // add base class to this class
          cd->insertBaseClass(baseClass,biName,bi->prot,bi->virt,templSpec);
          // add this class as super class to the base class
          baseClass->insertSubClass(cd,bi->prot,bi->virt,templSpec);
          // the undocumented base was found in this file
          baseClass->insertUsedFile(root->fileName);
          baseClass->setOuterScope(Doxygen::globalScope);
          if (baseClassName.right(2)=="-p")
          {
            baseClass->setCompoundType(ClassDef::Protocol);
          }
          return TRUE;
        }
        else
        {
          Debug::print(Debug::Classes,0,"    Base class `%s' not found\n",biName.data());
        }
      }
      else
      {
        if (mode!=TemplateInstances)
        {
          warn(root->fileName,root->startLine,
              "Detected potential recursive class relation "
              "between class %s and base class %s!\n",
              root->name.data(),baseClassName.data()
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
      //printf("new scopeOffset=`%d'",scopeOffset);
    } while (scopeOffset>=0);

    if (parentNode==0)
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

static bool isClassSection(EntryNav *rootNav)
{
  if ( !rootNav->name().isEmpty() )
  {
    if (rootNav->section() & Entry::COMPOUND_MASK)
         // is it a compound (class, struct, union, interface ...)
    {
      return TRUE;
    }
    else if (rootNav->section() & Entry::COMPOUNDDOC_MASK) 
         // is it a documentation block with inheritance info.
    {
      rootNav->loadEntry(g_storage);
      Entry *root = rootNav->entry();
      bool extends = root->extends->count()>0;
      rootNav->releaseEntry(); 
      if (extends) return TRUE;
    }
  }
  return FALSE;
}


/*! Builds a dictionary of all entry nodes in the tree starting with \a root
 */
static void findClassEntries(EntryNav *rootNav)
{
  if (isClassSection(rootNav))
  {
    g_classEntries.insert(rootNav->name(),rootNav);
  }
  RECURSE_ENTRYTREE(findClassEntries,rootNav);
}

/*! Using the dictionary build by findClassEntries(), this 
 *  function will look for additional template specialization that
 *  exists as inheritance relations only. These instances will be
 *  added to the template they are derived from.
 */
static void findInheritedTemplateInstances()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<EntryNav> edi(g_classEntries);
  EntryNav *rootNav;
  for (;(rootNav=edi.current());++edi)
  {
    ClassDef *cd;
    // strip any anonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(rootNav->name());
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Inheritance: Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      rootNav->loadEntry(g_storage);
      //printf("Class %s %d\n",cd->name().data(),root->extends->count());
      findBaseClassesForClass(rootNav,cd,cd,cd,TemplateInstances,FALSE);
      rootNav->releaseEntry();
    }
  }
}

static void findUsedTemplateInstances()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<EntryNav> edi(g_classEntries);
  EntryNav *rootNav;
  for (;(rootNav=edi.current());++edi)
  {
    ClassDef *cd;
    // strip any anonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(rootNav->name());
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Usage: Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      rootNav->loadEntry(g_storage);
      findUsedClassesForClass(rootNav,cd,cd,cd,TRUE);
      rootNav->releaseEntry();
    }
  }
}

static void computeClassRelations()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<EntryNav> edi(g_classEntries);
  EntryNav *rootNav;
  for (;(rootNav=edi.current());++edi)
  {
    ClassDef *cd;

    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    // strip any anonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(rootNav->name());
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Relations: Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      findBaseClassesForClass(rootNav,cd,cd,cd,DocumentedOnly,FALSE);
    }
    int numMembers = cd && cd->memberNameInfoSDict() ? cd->memberNameInfoSDict()->count() : 0;
    if ((cd==0 || (!cd->hasDocumentation() && !cd->isReference())) && numMembers>0 &&
        bName.right(2)!="::")
    {
      if (!root->name.isEmpty() && root->name.find('@')==-1 && // normal name
          (guessSection(root->fileName)==Entry::HEADER_SEC || 
           Config_getBool("EXTRACT_LOCAL_CLASSES")) && // not defined in source file
           protectionLevelVisible(root->protection) && // hidden by protection
           !Config_getBool("HIDE_UNDOC_CLASSES") // undocumented class are visible
         )
        warn_undoc(
                   root->fileName,root->startLine,
                   "warning: Compound %s is not documented.",
                   root->name.data()
             );
    }

    rootNav->releaseEntry();
  }
}

static void computeTemplateClassRelations()
{
  QDictIterator<EntryNav> edi(g_classEntries);
  EntryNav *rootNav;
  for (;(rootNav=edi.current());++edi)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    ClassDef *cd=getClass(bName);
    // strip any anonymous scopes first 
    QDict<ClassDef> *templInstances = 0;
    if (cd && (templInstances=cd->getTemplateInstances()))
    {
      Debug::print(Debug::Classes,0,"  Template class %s : \n",cd->name().data());
      QDictIterator<ClassDef> tdi(*templInstances);
      ClassDef *tcd;
      for (tdi.toFirst();(tcd=tdi.current());++tdi) // for each template instance
      {
        Debug::print(Debug::Classes,0,"    Template instance %s : \n",tcd->name().data());
        QCString templSpec = tdi.currentKey();
        ArgumentList *templArgs = new ArgumentList;
        stringToArgumentList(templSpec,templArgs);
        QList<BaseInfo> *baseList=root->extends;
        BaseInfo *bi=baseList->first();
        while (bi) // for each base class of the template
        {
          // check if the base class is a template argument
          BaseInfo tbi(bi->name,bi->prot,bi->virt);
          ArgumentList *tl = cd->templateArguments();
          if (tl)
          {
            QDict<int> *baseClassNames = tcd->getTemplateBaseClassNames();
            QDict<int> *templateNames = getTemplateArgumentsInName(tl,bi->name);
            // for each template name that we inherit from we need to
            // substitute the formal with the actual arguments
            QDict<int> *actualTemplateNames = new QDict<int>(17);
            actualTemplateNames->setAutoDelete(TRUE);
            QDictIterator<int> qdi(*templateNames);
            for (qdi.toFirst();qdi.current();++qdi)
            {
              int templIndex = *qdi.current();
              Argument *actArg = 0;
              if (templIndex<(int)templArgs->count()) 
              {
                actArg=templArgs->at(templIndex);
              }
              if (actArg!=0 &&
                  baseClassNames!=0 &&
                  baseClassNames->find(actArg->type)!=0 &&
                  actualTemplateNames->find(actArg->type)==0
                 )
              {
                actualTemplateNames->insert(actArg->type,new int(templIndex));
              }
            }
            delete templateNames;
            
            tbi.name = substituteTemplateArgumentsInString(bi->name,tl,templArgs);
            // find a documented base class in the correct scope
            if (!findClassRelation(rootNav,cd,tcd,&tbi,actualTemplateNames,DocumentedOnly,FALSE))
            {
              // no documented base class -> try to find an undocumented one
              findClassRelation(rootNav,cd,tcd,&tbi,actualTemplateNames,Undocumented,FALSE);
            }
            delete actualTemplateNames;
          }
          bi=baseList->next();
        }
        delete templArgs;
      } // class has no base classes
    }

    rootNav->releaseEntry();
  }
}

//-----------------------------------------------------------------------
// compute the references (anchors in HTML) for each function in the file

static void computeMemberReferences()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->computeAnchors();
  } 
  FileName *fn=Doxygen::inputNameList->first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->computeAnchors();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList->next();
  }
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    nd->computeAnchors();
  }
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->computeAnchors();
  }
}

//----------------------------------------------------------------------

static void addListReferences()
{
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      md->visited=FALSE;
    }
  }
  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  for (fnli.toFirst();(mn=fnli.current());++fnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      md->visited=FALSE;
    }
  }

  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->addListReferences();
  } 
  FileName *fn=Doxygen::inputNameList->first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->addListReferences();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList->next();
  }
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    nd->addListReferences();
  }
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->addListReferences();
  }
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    QCString name = pd->getOutputFileBase();
    if (pd->getGroupDef())
    {
      name = pd->getGroupDef()->getOutputFileBase();
    }
    {
      LockingPtr< QList<ListItemInfo> > xrefItems = pd->xrefListItems();
      addRefItem(xrefItems.pointer(),
          name,
          theTranslator->trPage(TRUE,TRUE),
          name,pd->title(),0);
    }
  }
  DirSDict::Iterator ddi(*Doxygen::directories);
  DirDef *dd = 0;
  for (ddi.toFirst();(dd=ddi.current());++ddi)
  {
    QCString name = dd->getOutputFileBase();
    //if (dd->getGroupDef())
    //{
    //  name = dd->getGroupDef()->getOutputFileBase();
    //}
    LockingPtr< QList<ListItemInfo> > xrefItems = dd->xrefListItems();
    addRefItem(xrefItems.pointer(),
        name,
        theTranslator->trDir(TRUE,TRUE),
        name,dd->displayName(),0);
  }
}

//----------------------------------------------------------------------

static void generateXRefPages()
{
  QDictIterator<RefList> di(*Doxygen::xrefLists);
  RefList *rl;
  for (di.toFirst();(rl=di.current());++di)
  {
    rl->generatePage();
  }
}

//----------------------------------------------------------------------
// Copy the documentation in entry `root' to member definition `md' and
// set the function declaration of the member to `funcDecl'. If the boolean 
// over_load is set the standard overload text is added. 

static void addMemberDocs(EntryNav *rootNav,
                   MemberDef *md, const char *funcDecl,
                   ArgumentList *al,
                   bool over_load,
                   NamespaceSDict *
                  )
{
  Entry *root = rootNav->entry();
  //printf("addMemberDocs: `%s'::`%s' `%s' funcDecl=`%s' mSpec=%d\n",
  //     root->parent->name.data(),md->name().data(),md->argsString(),funcDecl,root->spec);
  QCString fDecl=funcDecl;
  // strip extern specifier
  fDecl.stripPrefix("extern ");
  md->setDefinition(fDecl);
  md->enableCallGraph(root->callGraph);
  md->enableCallerGraph(root->callerGraph);
  ClassDef     *cd=md->getClassDef();
  NamespaceDef *nd=md->getNamespaceDef();
  QCString fullName;
  if (cd) 
    fullName = cd->name();
  else if (nd) 
    fullName = nd->name();

  if (!fullName.isEmpty()) fullName+="::";
  fullName+=md->name();
  FileDef *rfd=rootNav->fileDef();

  // TODO determine scope based on root not md
  Definition *rscope = md->getOuterScope();

  LockingPtr<ArgumentList> mdAl = md->argumentList();
  if (al)
  {
    //printf("merging arguments (1) docs=%d\n",root->doc.isEmpty());
    mergeArguments(mdAl.pointer(),al,!root->doc.isEmpty());
  }
  else
  {
    if ( 
          matchArguments2( md->getOuterScope(), md->getFileDef(), mdAl.pointer(),
                           rscope,rfd,root->argList,
                           TRUE
                         )
       ) 
    {
      //printf("merging arguments (2)\n");
      mergeArguments(mdAl.pointer(),root->argList,!root->doc.isEmpty());
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
         !rootNav->parent()->name().isEmpty()
        ) && !root->inbodyDocs.isEmpty()
       )
    {
      md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    }
  }

  //printf("initializer: '%s'(isEmpty=%d) '%s'(isEmpty=%d)\n",
  //    md->initializer().data(),md->initializer().isEmpty(),
  //    root->initializer.data(),root->initializer.isEmpty()
  //   );
  if (md->initializer().isEmpty() && !root->initializer.isEmpty())
  {
    //printf("setInitializer\n");
    md->setInitializer(root->initializer);
  }

  md->setMaxInitLines(root->initLines);

  if (rfd)
  {
    if ((md->getStartBodyLine()==-1 && root->bodyLine!=-1) 
       )
    {
      //printf("Setting new body segment [%d,%d]\n",root->bodyLine,root->endBodyLine);
      md->setBodySegment(root->bodyLine,root->endBodyLine);
      md->setBodyDef(rfd);
    }

    md->setRefItems(root->sli);
  }

  md->enableCallGraph(md->hasCallGraph() || root->callGraph);
  md->enableCallerGraph(md->hasCallerGraph() || root->callerGraph);

  md->mergeMemberSpecifiers(root->spec);
  md->addSectionsToDefinition(root->anchors);
  addMemberToGroups(root,md);
  if (cd) cd->insertUsedFile(root->fileName);
  //printf("root->mGrpId=%d\n",root->mGrpId);
  if (root->mGrpId!=-1)
  {
    if (md->getMemberGroupId()!=-1)
    {
      if (md->getMemberGroupId()!=root->mGrpId)
      {
        warn(
             root->fileName,root->startLine,
             "warning: member %s belongs to two different groups. The second "
             "one found here will be ignored.",
             md->name().data()
            );
      }
    }
    else // set group id
    {
      //printf("setMemberGroupId=%d md=%s\n",root->mGrpId,md->name().data());
      md->setMemberGroupId(root->mGrpId);
    }
  }
}

//----------------------------------------------------------------------
// find a class definition given the scope name and (optionally) a 
// template list specifier

static ClassDef *findClassDefinition(FileDef *fd,NamespaceDef *nd,
                         const char *scopeName)
{
  ClassDef *tcd = getResolvedClass(nd,fd,scopeName,0,0,TRUE,TRUE);
  return tcd;
}


//----------------------------------------------------------------------
// Adds the documentation contained in `root' to a global function
// with name `name' and argument list `args' (for overloading) and
// function declaration `decl' to the corresponding member definition.

static bool findGlobalMember(EntryNav *rootNav, 
                           const QCString &namespaceName,
                           const char *name, 
                           const char *tempArg,
                           const char *, 
                           const char *decl)
{
  Entry *root = rootNav->entry();
  Debug::print(Debug::FindMembers,0,
       "2. findGlobalMember(namespace=%s,name=%s,tempArg=%s,decl=%s)\n",
          namespaceName.data(),name,tempArg,decl);
  QCString n=name;
  if (n.isEmpty()) return FALSE;
  if (n.find("::")!=-1) return FALSE; // skip undefined class members
  MemberName *mn=Doxygen::functionNameSDict->find(n+tempArg); // look in function dictionary
  if (mn==0)
  {
    mn=Doxygen::functionNameSDict->find(n); // try without template arguments
  }
  if (mn) // function name defined
  {
    Debug::print(Debug::FindMembers,0,"3. Found function scope\n");
    //int count=0;
    MemberNameIterator mni(*mn);
    MemberDef *md;
    bool found=FALSE;
    for (mni.toFirst();(md=mni.current()) && !found;++mni)
    {
      NamespaceDef *nd=md->getNamespaceDef();

      //printf("Namespace namespaceName=%s nd=%s\n",
      //    namespaceName.data(),nd ? nd->name().data() : "<none>");

      FileDef *fd=rootNav->fileDef();
      //printf("File %s\n",fd ? fd->name().data() : "<none>");
      NamespaceSDict *nl = fd ? fd->getUsedNamespaces() : 0;
      //SDict<Definition> *cl = fd ? fd->getUsedClasses()    : 0;
      //printf("NamespaceList %p\n",nl);

      // search in the list of namespaces that are imported via a 
      // using declaration
      bool viaUsingDirective = nl && nd && nl->find(nd->qualifiedName())!=0;

      if ((namespaceName.isEmpty() && nd==0) ||  // not in a namespace
          (nd && nd->name()==namespaceName) ||   // or in the same namespace 
          viaUsingDirective                      // member in `using' namespace
         )     
      {
        Debug::print(Debug::FindMembers,0,"4. Try to add member `%s' to scope `%s'\n",
            md->name().data(),namespaceName.data());
        QCString nsName = nd ? nd->name().data() : "";

        NamespaceDef *rnd = 0;
        if (!namespaceName.isEmpty()) rnd = Doxygen::namespaceSDict->find(namespaceName);

        LockingPtr<ArgumentList> mdAl = md->argumentList();
        bool matching=
          (mdAl==0 && root->argList->count()==0) ||
          md->isVariable() || md->isTypedef() || /* in case of function pointers */
          matchArguments2(md->getOuterScope(),md->getFileDef(),mdAl.pointer(),
                          rnd ? rnd : Doxygen::globalScope,fd,root->argList,
                          FALSE);

        // for template members we need to check if the number of
        // template arguments is the same, otherwise we are dealing with
        // different functions.
        if (matching && root->tArgLists)
        {
          LockingPtr<ArgumentList> mdTempl = md->templateArguments();
          if (mdTempl!=0)
          {
            if (root->tArgLists->getLast()->count()!=mdTempl->count())
            {
              matching=FALSE;
            }
          }
        }

        //printf("%s<->%s\n",
        //    argListToString(md->argumentList()).data(),
        //    argListToString(root->argList).data());

        // for static members we also check if the comment block was found in 
        // the same file. This is needed because static members with the same
        // name can be in different files. Thus it would be wrong to just
        // put the comment block at the first syntactically matching member.
        if (matching && md->isStatic() && 
            md->getDefFileName()!=root->fileName && 
            mn->count()>1)
        {
          matching = FALSE;
        }

        if (matching) // add docs to the member
        {
          Debug::print(Debug::FindMembers,0,"5. Match found\n");
          addMemberDocs(rootNav,md,decl,root->argList,FALSE);
          found=TRUE;
        }
      }
    } 
    if (!found && root->relatesType != Duplicate && root->section==Entry::FUNCTION_SEC) // no match
    {
      QCString fullFuncDecl=decl;
      if (root->argList) fullFuncDecl+=argListToString(root->argList,TRUE);
      QCString warnMsg =
         QCString("warning: no matching file member found for \n")+substitute(fullFuncDecl,"%","%%");
      if (mn->count()>0)
      {
        warnMsg+="\nPossible candidates:\n";
        for (mni.toFirst();(md=mni.current());++mni)
        {
          warnMsg+=" '";
          warnMsg+=substitute(md->declaration(),"%","%%");
          warnMsg+="' at line "+QCString().setNum(md->getDefLine())+
                   " of file"+md->getDefFileName()+"\n";
        }
      }
      warn(root->fileName,root->startLine,warnMsg);
    }
  }
  else // got docs for an undefined member!
  {
    if (root->type!="friend class" && 
        root->type!="friend struct" &&
        root->type!="friend union" &&
        (!Config_getBool("TYPEDEF_HIDES_STRUCT") || 
         root->type.find("typedef ")==-1)
       )
    {
      warn(root->fileName,root->startLine,
           "warning: documented function `%s' was not declared or defined.",decl
          );
    }
  }
  return TRUE;
}

static bool isSpecialization(
                  const QList<ArgumentList> &srcTempArgLists,
                  const QList<ArgumentList> &dstTempArgLists
    )
{
    QListIterator<ArgumentList> srclali(srcTempArgLists);
    QListIterator<ArgumentList> dstlali(dstTempArgLists);
    for (;srclali.current();++srclali,++dstlali)
    {
      ArgumentList *sal = srclali.current();
      ArgumentList *dal = dstlali.current();
      if (!(sal && dal && sal->count()==dal->count())) return TRUE;
    }
    return FALSE;
}


static QCString substituteTemplatesInString(
    const QList<ArgumentList> &srcTempArgLists,
    const QList<ArgumentList> &dstTempArgLists,
    ArgumentList *funcTempArgList, // can be used to match template specializations
    const QCString &src
    )
{
  QCString dst;
  QRegExp re( "[A-Za-z_][A-Za-z_0-9]*");
  //printf("type=%s\n",sa->type.data());
  int i,p=0,l; 
  while ((i=re.match(src,p,&l))!=-1) // for each word in srcType
  {
    bool found=FALSE;
    dst+=src.mid(p,i-p);
    QCString name=src.mid(i,l);

    QListIterator<ArgumentList> srclali(srcTempArgLists);
    QListIterator<ArgumentList> dstlali(dstTempArgLists);
    for (;srclali.current() && !found;++srclali,++dstlali)
    {
      ArgumentListIterator tsali(*srclali.current());
      ArgumentListIterator tdali(*dstlali.current());
      Argument *tsa =0,*tda=0, *fa=0;
      if (funcTempArgList)
      {
        fa=funcTempArgList->first();
      }

      for (tsali.toFirst();(tsa=tsali.current()) && !found;++tsali)
      {
        tda = tdali.current();
        //if (tda) printf("tsa=%s|%s tda=%s|%s\n",
        //    tsa->type.data(),tsa->name.data(),
        //    tda->type.data(),tda->name.data());
        if (name==tsa->name)
        {
          if (tda && tda->name.isEmpty())
          {
            int vc=0;
            if (tda->type.left(6)=="class ") vc=6;
            else if (tda->type.left(9)=="typename ") vc=9;
            if (vc>0) // convert type=="class T" to type=="class" name=="T"
            {
              tda->name = tda->type.mid(vc);
              tda->type = tda->type.left(vc-1);
            }
          }
          if (tda && !tda->name.isEmpty())
          {
            name=tda->name; // substitute
            found=TRUE;
          }
          else if (fa)
          {
            name=fa->type;
            found=TRUE;
          }
        }
        if (tda) 
          ++tdali; 
        else if (fa) 
          fa=funcTempArgList->next();
      }
      //printf("   srcList='%s' dstList='%s faList='%s'\n",
      //  argListToString(srclali.current()).data(),
      //  argListToString(dstlali.current()).data(),
      //  funcTempArgList ? argListToString(funcTempArgList).data() : "<none>");
    }
    dst+=name; 
    p=i+l;
  }
  dst+=src.right(src.length()-p);
  //printf("  substituteTemplatesInString(%s)=%s\n",
  //    src.data(),dst.data());
  return dst;
}

static void substituteTemplatesInArgList(
                  const QList<ArgumentList> &srcTempArgLists,
                  const QList<ArgumentList> &dstTempArgLists,
                  ArgumentList *src,
                  ArgumentList *dst,
                  ArgumentList *funcTempArgs = 0
                 )
{
  ArgumentListIterator sali(*src);
  Argument *sa=0;
  Argument *da=dst->first();

  for (sali.toFirst();(sa=sali.current());++sali) // for each member argument
  {
    QCString dstType = substituteTemplatesInString(
                                  srcTempArgLists,dstTempArgLists,funcTempArgs,
                                  sa->type);
    QCString dstArray = substituteTemplatesInString(
                                  srcTempArgLists,dstTempArgLists,funcTempArgs,
                                  sa->array);
    if (da==0)
    {
      da=new Argument(*sa);
      dst->append(da);
      da->type=dstType;
      da->array=dstArray;
      da=0;
    }
    else
    {
      da->type=dstType;
      da->type=dstArray;
      da=dst->next();
    }
  }
  dst->constSpecifier     = src->constSpecifier;
  dst->volatileSpecifier  = src->volatileSpecifier;
  dst->pureSpecifier      = src->pureSpecifier;
  dst->trailingReturnType = substituteTemplatesInString(
                             srcTempArgLists,dstTempArgLists,
                             funcTempArgs,src->trailingReturnType);
  //printf("substituteTemplatesInArgList: replacing %s with %s\n",
  //    argListToString(src).data(),argListToString(dst).data()
  //    );
}



/*! This function tries to find a member (in a documented class/file/namespace) 
 * that corresponds to the function/variable declaration given in \a funcDecl.
 *
 * The boolean \a overloaded is used to specify whether or not a standard
 * overload documentation line should be generated.
 *
 * The boolean \a isFunc is a hint that indicates that this is a function
 * instead of a variable or typedef.
 */
static void findMember(EntryNav *rootNav,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      )
{
  Entry *root = rootNav->entry();

  Debug::print(Debug::FindMembers,0,
               "findMember(root=%p,funcDecl=`%s',related=`%s',overload=%d,"
               "isFunc=%d mGrpId=%d tArgList=%p (#=%d) "
               "spec=%d lang=%x\n",
               root,funcDecl.data(),root->relates.data(),overloaded,isFunc,root->mGrpId,
               root->tArgLists,root->tArgLists ? root->tArgLists->count() : 0,
               root->spec,root->lang
              );

  QCString scopeName;
  QCString className;
  QCString namespaceName;
  QCString funcType;
  QCString funcName;
  QCString funcArgs;
  QCString funcTempList;
  QCString exceptions;
  QCString funcSpec;
  bool isRelated=FALSE;
  bool isMemberOf=FALSE;
  bool isFriend=FALSE;
  bool done;
  do
  {
    done=TRUE;
    if (funcDecl.stripPrefix("friend ")) // treat friends as related members
    {
      isFriend=TRUE;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("inline "))
    {
      root->spec|=Entry::Inline;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("explicit "))
    {
      root->spec|=Entry::Explicit;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("mutable "))
    {
      root->spec|=Entry::Mutable;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("virtual "))
    {
      done=FALSE;
    }
  } while (!done);

  // delete any ; from the function declaration
  int sep;
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
  
  //printf("funcDecl=`%s'\n",funcDecl.data());
  if (isFriend && funcDecl.left(6)=="class ")
  {
    //printf("friend class\n");
    funcDecl=funcDecl.right(funcDecl.length()-6);
    funcName = funcDecl.copy();
  }
  else if (isFriend && funcDecl.left(7)=="struct ")
  {
    funcDecl=funcDecl.right(funcDecl.length()-7);
    funcName = funcDecl.copy();
  }
  else
  {
    // extract information from the declarations
    parseFuncDecl(funcDecl,root->lang==SrcLangExt_ObjC,scopeName,funcType,funcName,
                funcArgs,funcTempList,exceptions
               );
  }
  //printf("scopeName=`%s' funcType=`%s' funcName=`%s' funcArgs=`%s'\n",
  //    scopeName.data(),funcType.data(),funcName.data(),funcArgs.data());

  // the class name can also be a namespace name, we decide this later.
  // if a related class name is specified and the class name could
  // not be derived from the function declaration, then use the
  // related field.
  //printf("scopeName=`%s' className=`%s' namespaceName=`%s'\n",
  //    scopeName.data(),className.data(),namespaceName.data());
  if (!root->relates.isEmpty()) 
  {                             // related member, prefix user specified scope
    isRelated=TRUE;
    isMemberOf=(root->relatesType == MemberOf);
    if (getClass(root->relates)==0 && !scopeName.isEmpty())
    {
      scopeName= mergeScopes(scopeName,root->relates);
    }
    else 
    {
      scopeName = root->relates;
    }
  }

  if (root->relates.isEmpty() && rootNav->parent() && 
      ((rootNav->parent()->section()&Entry::SCOPE_MASK) ||
       (rootNav->parent()->section()==Entry::OBJCIMPL_SEC)
      ) &&
      !rootNav->parent()->name().isEmpty()) // see if we can combine scopeName 
                                     // with the scope in which it was found
  {
    QCString joinedName = rootNav->parent()->name()+"::"+scopeName;
    if (!scopeName.isEmpty() && 
        (getClass(joinedName) || Doxygen::namespaceSDict->find(joinedName)))
    {
      scopeName = joinedName;
    }
    else
    {
      scopeName = mergeScopes(rootNav->parent()->name(),scopeName);
    }
  }
  else // see if we can prefix a namespace or class that is used from the file
  {
     FileDef *fd=rootNav->fileDef();
     if (fd)
     {
       NamespaceSDict *fnl = fd->getUsedNamespaces();
       if (fnl)
       {
         QCString joinedName;
         NamespaceDef *fnd;
         NamespaceSDict::Iterator nsdi(*fnl);
         for (nsdi.toFirst();(fnd=nsdi.current());++nsdi)
         {
           joinedName = fnd->name()+"::"+scopeName;
           if (Doxygen::namespaceSDict->find(joinedName))
           {
             scopeName=joinedName;
             break;
           }
         }
       }
     }
  }
  scopeName=stripTemplateSpecifiersFromScope(
      removeRedundantWhiteSpace(scopeName),FALSE,&funcSpec); 

  // funcSpec contains the last template specifiers of the given scope.
  // If this method does not have any template arguments or they are 
  // empty while funcSpec is not empty we assume this is a 
  // specialization of a method. If not, we clear the funcSpec and treat
  // this as a normal method of a template class.
  if (!(root->tArgLists && 
        root->tArgLists->count()>0 &&
        root->tArgLists->first()->count()==0
       )
     ) 
  {
    funcSpec.resize(0);
  }
  
  // split scope into a namespace and a class part
  extractNamespaceName(scopeName,className,namespaceName,TRUE);
  //printf("scopeName=`%s' className=`%s' namespaceName=`%s'\n",
  //       scopeName.data(),className.data(),namespaceName.data());
  
  //namespaceName=removeAnonymousScopes(namespaceName);
  if (namespaceName.find('@')!=-1) return; // skip stuff in anonymous namespace...

  //printf("namespaceName=`%s' className=`%s'\n",namespaceName.data(),className.data());
  // merge class and namespace scopes again
  scopeName.resize(0);
  if (!namespaceName.isEmpty())
  {
    if (className.isEmpty())
    {
      scopeName=namespaceName;
    }
    else if (!root->relates.isEmpty() || // relates command with explicit scope
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
  //printf("new scope=`%s'\n",scopeName.data());

  QCString tempScopeName=scopeName;
  ClassDef *cd=getClass(scopeName);
  if (cd)
  {
    if (root->tArgLists) root->tArgLists->first();
    if (funcSpec.isEmpty())
    {
      tempScopeName=cd->qualifiedNameWithTemplateParameters(root->tArgLists);
    }
    else
    {
      tempScopeName=scopeName+funcSpec;
    }
  }
  //printf("scopeName=%s cd=%p root->tArgLists=%p result=%s\n",
  //    scopeName.data(),cd,root->tArgLists,tempScopeName.data());
  
  //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
  // rebuild the function declaration (needed to get the scope right).
  if (!scopeName.isEmpty() && !isRelated && !isFriend && !Config_getBool("HIDE_SCOPE_NAMES"))
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
      else // variable => add `argument' list
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
      else // variable => add `argument' list
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
  
  Debug::print(Debug::FindMembers,0,
           "findMember() Parse results:\n"
           "  namespaceName=`%s'\n"
           "  className=`%s`\n"
           "  funcType=`%s'\n"
           "  funcSpec=`%s'\n"
           "  funcName=`%s'\n"
           "  funcArgs=`%s'\n"
           "  funcTempList=`%s'\n"
           "  funcDecl=`%s'\n"
           "  related=`%s'\n" 
           "  exceptions=`%s'\n"
           "  isRelated=%d\n"
           "  isMemberOf=%d\n"
           "  isFriend=%d\n"
           "  isFunc=%d\n\n",
           namespaceName.data(),className.data(),
           funcType.data(),funcSpec.data(),funcName.data(),funcArgs.data(),funcTempList.data(),
           funcDecl.data(),root->relates.data(),exceptions.data(),isRelated,isMemberOf,isFriend,
           isFunc
          );

  MemberName *mn=0;
  if (!funcName.isEmpty()) // function name is valid
  { 
    Debug::print(Debug::FindMembers,0,
                 "1. funcName=`%s'\n",funcName.data());
    if (funcName.left(9)=="operator ") // strip class scope from cast operator
    {
      funcName = substitute(funcName,className+"::","");
    }
    if (!funcTempList.isEmpty()) // try with member specialization
    {
      mn=Doxygen::memberNameSDict->find(funcName+funcTempList);
    }
    if (mn==0) // try without specialization
    {
      mn=Doxygen::memberNameSDict->find(funcName);
    }
    if (!isRelated && mn) // function name already found
    {
      Debug::print(Debug::FindMembers,0,
                   "2. member name exists (%d members with this name)\n",mn->count());
      if (!className.isEmpty()) // class name is valid
      {
        if (funcSpec.isEmpty()) // not a member specialization
        {
          int count=0;
          int noMatchCount=0;
          MemberNameIterator mni(*mn);
          MemberDef *md;
          bool memFound=FALSE;
          for (mni.toFirst();!memFound && (md=mni.current());++mni)
          {
            ClassDef *cd=md->getClassDef();
            Debug::print(Debug::FindMembers,0,
                "3. member definition found, "
                "scope needed=`%s' scope=`%s' args=`%s' fileName=%s\n",
                scopeName.data(),cd ? cd->name().data() : "<none>",
                md->argsString(),
                root->fileName.data());
            //printf("Member %s (member scopeName=%s) (this scopeName=%s) classTempList=%s\n",md->name().data(),cd->name().data(),scopeName.data(),classTempList.data());
            FileDef *fd=rootNav->fileDef();
            NamespaceDef *nd=0;
            if (!namespaceName.isEmpty()) nd=getResolvedNamespace(namespaceName);

            //printf("scopeName %s->%s\n",scopeName.data(),
            //       stripTemplateSpecifiersFromScope(scopeName,FALSE).data());

            ClassDef *tcd=findClassDefinition(fd,nd,scopeName);
            if (tcd==0 && stripAnonymousNamespaceScope(cd->name())==scopeName)
            {
              // don't be fooled by anonymous scopes
              tcd=cd;
            }
            //printf("Looking for %s inside nd=%s result=%p (%s) cd=%p\n",
            //    scopeName.data(),nd?nd->name().data():"<none>",tcd,tcd?tcd->name().data():"",cd);

            if (cd && tcd==cd) // member's classes match
            {
              Debug::print(Debug::FindMembers,0,
                  "4. class definition %s found\n",cd->name().data());

              // get the template parameter lists found at the member declaration
              QList<ArgumentList> declTemplArgs;
              cd->getTemplateParameterLists(declTemplArgs);
              LockingPtr<ArgumentList> templAl = md->templateArguments();
              if (templAl!=0)
              {
                declTemplArgs.append(templAl.pointer());
              }

              // get the template parameter lists found at the member definition
              QList<ArgumentList> *defTemplArgs = root->tArgLists;
              //printf("defTemplArgs=%p\n",defTemplArgs);

              // do we replace the decl argument lists with the def argument lists?
              bool substDone=FALSE;
              ArgumentList *argList=0;

              /* substitute the occurrences of class template names in the 
               * argument list before matching 
               */
              LockingPtr<ArgumentList> mdAl = md->argumentList();
              if (declTemplArgs.count()>0 && defTemplArgs &&
                  declTemplArgs.count()==defTemplArgs->count() &&
                  mdAl.pointer()
                 )
              {
                /* the function definition has template arguments
                 * and the class definition also has template arguments, so
                 * we must substitute the template names of the class by that
                 * of the function definition before matching.
                 */
                argList = new ArgumentList;
                substituteTemplatesInArgList(declTemplArgs,*defTemplArgs,
                    mdAl.pointer(),argList);

                substDone=TRUE;
              }
              else /* no template arguments, compare argument lists directly */
              {
                argList = mdAl.pointer();
              }

              Debug::print(Debug::FindMembers,0,
                  "5. matching `%s'<=>`%s' className=%s namespaceName=%s\n",
                  argListToString(argList,TRUE).data(),argListToString(root->argList,TRUE).data(),
                  className.data(),namespaceName.data()
                  );

              bool matching=
                md->isVariable() || md->isTypedef() || // needed for function pointers
                (mdAl.pointer()==0 && root->argList->count()==0) || 
                matchArguments2(
                    md->getClassDef(),md->getFileDef(),argList, 
                    cd,fd,root->argList,
                    TRUE);

              if (md->getLanguage()==SrcLangExt_ObjC && md->isVariable() && (root->section&Entry::FUNCTION_SEC))
              {
                matching = FALSE; // don't match methods and attributes with the same name
              }

              Debug::print(Debug::FindMembers,0,
                  "6. match results of matchArguments2 = %d\n",matching);

              if (substDone) // found a new argument list
              {
                if (matching) // replace member's argument list
                {
                  md->setDefinitionTemplateParameterLists(root->tArgLists);
                  md->setArgumentList(argList); // new owner of the list => no delete
                }
                else // no match 
                {
                  if (!funcTempList.isEmpty() && 
                      isSpecialization(declTemplArgs,*defTemplArgs))
                  {
                    // check if we are dealing with a partial template
                    // specialization. In this case we add it to the class
                    // even though the member arguments do not match.
                    
                    // TODO: copy other aspects?
                    root->protection=md->protection(); // copy protection level
                    addMethodToClass(rootNav,cd,md->name(),isFriend);
                    return;
                  }
                  delete argList;
                }
              }
              if (matching) 
              {
                addMemberDocs(rootNav,md,funcDecl,0,overloaded,0/* TODO */);
                count++;
                memFound=TRUE;
              }
            } 
            else if (cd && cd!=tcd) // we did find a class with the same name as cd
                                    // but in a different namespace
            {
              noMatchCount++;
            }
          } 
          if (count==0 && rootNav->parent() && 
              rootNav->parent()->section()==Entry::OBJCIMPL_SEC)
          {
            goto localObjCMethod;
          }
          if (count==0 && !(isFriend && funcType=="class"))
          {
            int candidates=0;
            ClassDef *ecd = 0, *ucd = 0;
            MemberDef *emd = 0, *umd = 0;
            if (mn->count()>0)
            {
              //printf("Assume template class\n");
              for (mni.toFirst();(md=mni.current());++mni)
              {
                ClassDef *ccd=md->getClassDef();
                MemberDef *cmd=md;
                //printf("ccd->name()==%s className=%s\n",ccd->name().data(),className.data());
                if (ccd!=0 && rightScopeMatch(ccd->name(),className)) 
                {
                  LockingPtr<ArgumentList> templAl = md->templateArguments();
                  if (root->tArgLists && templAl!=0 &&
                      root->tArgLists->getLast()->count()<=templAl->count())
                  { 
                    addMethodToClass(rootNav,ccd,md->name(),isFriend);
                    return;
                  }
                  if (md->argsString()==argListToString(root->argList,TRUE,FALSE))
                  { // exact argument list match -> remember
                    ucd = ecd = ccd;
                    umd = emd = cmd;
                    Debug::print(Debug::FindMembers,0,
                     "7. new candidate className=%s scope=%s args=%s exact match\n",
                         className.data(),ccd->name().data(),md->argsString());
                  }
                  else // arguments do not match, but member name and scope do -> remember
                  {
                    ucd = ccd;
                    umd = cmd;
                    Debug::print(Debug::FindMembers,0,
                     "7. new candidate className=%s scope=%s args=%s no match\n",
                         className.data(),ccd->name().data(),md->argsString());
                  }
                  candidates++;
                }
              }
            }
            static bool strictProtoMatching = Config_getBool("STRICT_PROTO_MATCHING");
            if (!strictProtoMatching)
            {
              if (candidates==1 && ucd && umd)
              {
                // we didn't find an actual match on argument lists, but there is only 1 member with this
                // name in the same scope, so that has to be the one. 
                addMemberDocs(rootNav,umd,funcDecl,0,overloaded,0);
                return;
              }
              else if (candidates>1 && ecd && emd)
              {
                // we didn't find a unique match using type resolution, 
                // but one of the matches has the exact same signature so
                // we take that one.
                addMemberDocs(rootNav,emd,funcDecl,0,overloaded,0);
                return;
              }
            }

            QCString warnMsg = "warning: no ";
            if (noMatchCount>1) warnMsg+="uniquely ";
            warnMsg+="matching class member found for \n";

            if (root->tArgLists)
            {
              QListIterator<ArgumentList> alli(*root->tArgLists);
              ArgumentList *al;
              for (;(al=alli.current());++alli)
              {
                warnMsg+="  template ";
                warnMsg+=tempArgListToString(al);
                warnMsg+='\n';
              }
            }
            QCString fullFuncDecl=funcDecl.copy();
            if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);

            warnMsg+="  ";
            warnMsg+=fullFuncDecl;
            warnMsg+='\n';

            if (candidates>0)
            {
              warnMsg+="Possible candidates:\n";
              for (mni.toFirst();(md=mni.current());++mni)
              {
                ClassDef *cd=md->getClassDef();
                if (cd!=0 && rightScopeMatch(cd->name(),className))
                {
                  LockingPtr<ArgumentList> templAl = md->templateArguments();
                  if (templAl!=0)
                  {
                    warnMsg+="  'template ";
                    warnMsg+=tempArgListToString(templAl.pointer());
                    warnMsg+='\n';
                  }
                  warnMsg+="  ";
                  if (md->typeString()) 
                  {
                    warnMsg+=md->typeString();
                    warnMsg+=' ';
                  }
                  QCString qScope = cd->qualifiedNameWithTemplateParameters();
                  if (!qScope.isEmpty()) 
                    warnMsg+=qScope+"::"+md->name();
                  if (md->argsString()) 
                    warnMsg+=md->argsString();
                  if (noMatchCount>1) 
                  {
                    warnMsg+="' at line "+QCString().setNum(md->getDefLine()) +
                             " of file "+md->getDefFileName();
                  }

                  warnMsg+='\n';
                }
              }
            }
            warn_simple(root->fileName,root->startLine,warnMsg);
          }
        }
        else if (cd) // member specialization
        {
          MemberNameIterator mni(*mn);
          MemberDef *declMd=0;
          MemberDef *md=0;
          for (mni.toFirst();(md=mni.current());++mni)
          {
            if (md->getClassDef()==cd) 
            {
              // TODO: we should probably also check for matching arguments
              declMd = md;
              break;
            }
          }
          MemberDef::MemberType mtype=MemberDef::Function;
          ArgumentList *tArgList = new ArgumentList;
          //  getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
          md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              declMd ? declMd->protection() : root->protection,
              root->virt,root->stat,Member,
              mtype,tArgList,root->argList);
          //printf("new specialized member %s args=`%s'\n",md->name().data(),funcArgs.data());
          md->setTagInfo(rootNav->tagInfo());
          md->setLanguage(root->lang);
          md->setMemberClass(cd);
          md->setTemplateSpecialization(TRUE);
          md->setTypeConstraints(root->typeConstr);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          md->enableCallerGraph(root->callerGraph);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->addSectionsToDefinition(root->anchors);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          FileDef *fd=rootNav->fileDef();
          md->setBodyDef(fd);
          md->setMemberSpecifiers(root->spec);
          md->setMemberGroupId(root->mGrpId);
          mn->append(md);
          cd->insertMember(md);
          md->setRefItems(root->sli);
          delete tArgList;
        }
        else
        {
          //printf("*** Specialized member %s of unknown scope %s%s found!\n",
          //        scopeName.data(),funcName.data(),funcArgs.data());
        }
      }
      else if (overloaded) // check if the function belongs to only one class 
      {
        // for unique overloaded member we allow the class to be
        // omitted, this is to be Qt compatible. Using this should 
        // however be avoided, because it is error prone
        MemberNameIterator mni(*mn);
        MemberDef *md=mni.toFirst();
        ASSERT(md);
        ClassDef *cd=md->getClassDef();
        ASSERT(cd);
        QCString className=cd->name().copy();
        ++mni;
        bool unique=TRUE;
        for (;(md=mni.current());++mni)
        {
          ClassDef *cd=md->getClassDef();
          if (className!=cd->name()) unique=FALSE; 
        } 
        if (unique)
        {
          MemberDef::MemberType mtype;
          if      (root->mtype==Signal)  mtype=MemberDef::Signal;
          else if (root->mtype==Slot)    mtype=MemberDef::Slot;
          else if (root->mtype==DCOP)    mtype=MemberDef::DCOP;
          else                 mtype=MemberDef::Function;
          
          // new overloaded member function
          ArgumentList *tArgList = 
            getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
          //printf("new related member %s args=`%s'\n",md->name().data(),funcArgs.data());
          MemberDef *md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,root->stat,Related,
              mtype,tArgList,root->argList);
          md->setTagInfo(rootNav->tagInfo());
          md->setLanguage(root->lang);
          md->setTypeConstraints(root->typeConstr);
          md->setMemberClass(cd);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          md->enableCallerGraph(root->callerGraph);
          QCString doc=getOverloadDocs();
          doc+="<p>";
          doc+=root->doc;
          md->setDocumentation(doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->addSectionsToDefinition(root->anchors);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          FileDef *fd=rootNav->fileDef();
          md->setBodyDef(fd);
          md->setMemberSpecifiers(root->spec);
          md->setMemberGroupId(root->mGrpId);
          mn->append(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
          md->setRefItems(root->sli);
        }
      }
      else // unrelated function with the same name as a member
      {
        if (!findGlobalMember(rootNav,namespaceName,funcName,funcTempList,funcArgs,funcDecl))
        {
          QCString fullFuncDecl=funcDecl.copy();
          if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
          warn(root->fileName,root->startLine,
               "warning: Cannot determine class for function\n%s",
               fullFuncDecl.data()
              );   
        }
      }
    }
    else if (isRelated && !root->relates.isEmpty())
    {
      Debug::print(Debug::FindMembers,0,"2. related function\n"
              "  scopeName=%s className=%s\n",scopeName.data(),className.data());
      if (className.isEmpty()) className=root->relates;
      ClassDef *cd;
      //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
      if ((cd=getClass(scopeName)))
      {
        bool newMember=TRUE; // assume we have a new member
        bool newMemberName=FALSE; 
        MemberDef *mdDefine=0;
        bool isDefine=FALSE;
        {
          MemberName *mn = Doxygen::functionNameSDict->find(funcName);
          if (mn)
          {
            mdDefine = mn->first();
            while (mdDefine && !isDefine)
            {
              isDefine = isDefine || mdDefine->isDefine();
              if (!isDefine) mdDefine = mn->next();
            }
          }
        }

        FileDef *fd=rootNav->fileDef();

        if ((mn=Doxygen::memberNameSDict->find(funcName))==0)
        {
          mn=new MemberName(funcName);
          newMemberName=TRUE; // we create a new member name
        }
        else
        {
          MemberDef *rmd=mn->first();
          while (rmd && newMember) // see if we got another member with matching arguments
          {
            LockingPtr<ArgumentList> rmdAl = rmd->argumentList();

            newMember=
              className!=rmd->getOuterScope()->name() ||
              !matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),rmdAl.pointer(),
                               cd,fd,root->argList,
                               TRUE);
            if (newMember) rmd=mn->next();
          }
          if (!newMember && rmd) // member already exists as rmd -> add docs
          {
            //printf("addMemberDocs for related member %s\n",root->name.data());
            //rmd->setMemberDefTemplateArguments(root->mtArgList);
            addMemberDocs(rootNav,rmd,funcDecl,0,overloaded);
          }
        }

        if (newMember) // need to create a new member
        {
          MemberDef::MemberType mtype;
          if (isDefine)
            mtype=MemberDef::Define;
          else if (root->mtype==Signal)  
            mtype=MemberDef::Signal;
          else if (root->mtype==Slot) 
            mtype=MemberDef::Slot;
          else if (root->mtype==DCOP)
            mtype=MemberDef::DCOP;
          else
            mtype=MemberDef::Function;

          if (isDefine && mdDefine)
          {
            mdDefine->setHidden(TRUE);
            funcType="#define";
            funcArgs=mdDefine->argsString();
            funcDecl=funcType + " " + funcName;
          } 

          //printf("New related name `%s' `%d'\n",funcName.data(),
          //    root->argList ? (int)root->argList->count() : -1);

          // first note that we pass:
          //   (root->tArgLists ? root->tArgLists->last() : 0)
          // for the template arguments fo the new "member."
          // this accurately reflects the template arguments of
          // the related function, which don't have to do with
          // those of the related class.
          MemberDef *md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,
              root->stat && !isMemberOf,
              isMemberOf ? Foreign : isRelated ? Related : Member,
              mtype,
              (root->tArgLists ? root->tArgLists->last() : 0),
              funcArgs.isEmpty() ? 0 : root->argList);

          if (isDefine && mdDefine)
          {
            md->setInitializer(mdDefine->initializer());
          }

          // 
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
          md->setDefinitionTemplateParameterLists(root->tArgLists);

          md->setTagInfo(rootNav->tagInfo());



          //printf("Related member name=`%s' decl=`%s' bodyLine=`%d'\n",
          //       funcName.data(),funcDecl.data(),root->bodyLine);

          // try to find the matching line number of the body from the
          // global function list 
          bool found=FALSE;
          if (root->bodyLine==-1)
          {
            MemberName *rmn=Doxygen::functionNameSDict->find(funcName);
            if (rmn)
            {
              MemberDef *rmd=rmn->first();
              while (rmd && !found) // see if we got another member with matching arguments
              {
                LockingPtr<ArgumentList> rmdAl = rmd->argumentList();
                // check for matching argument lists
                if (
                    matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),rmdAl.pointer(),
                                    cd,fd,root->argList,
                                    TRUE)
                   )
                {
                  found=TRUE;
                }
                if (!found) rmd=rmn->next();
              }
              if (rmd) // member found -> copy line number info
              {
                md->setBodySegment(rmd->getStartBodyLine(),rmd->getEndBodyLine());
                md->setBodyDef(rmd->getBodyDef());
                //md->setBodyMember(rmd);
              }
            }
          }
          if (!found) // line number could not be found or is available in this
                      // entry
          {
            md->setBodySegment(root->bodyLine,root->endBodyLine);
            md->setBodyDef(fd);
          }

          //if (root->mGrpId!=-1) 
          //{
          //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
          //}
          md->setMemberClass(cd);
          md->setMemberSpecifiers(root->spec);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          md->enableCallerGraph(root->callerGraph);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          md->addSectionsToDefinition(root->anchors);
          md->setMemberGroupId(root->mGrpId);
          md->setLanguage(root->lang);
          //md->setMemberDefTemplateArguments(root->mtArgList);
          mn->append(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
          md->setRefItems(root->sli);
          if (root->relatesType == Duplicate) md->setRelatedAlso(cd);
          if (!isDefine)
          {
            addMemberToGroups(root,md);
          }
          //printf("Adding member=%s\n",md->name().data());
          if (newMemberName)
          {
            //Doxygen::memberNameList.append(mn);
            //Doxygen::memberNameDict.insert(funcName,mn);
            Doxygen::memberNameSDict->append(funcName,mn);
          }
        }
        if (root->relatesType == Duplicate)
        {
          if (!findGlobalMember(rootNav,namespaceName,funcName,funcTempList,funcArgs,funcDecl))
          {
            QCString fullFuncDecl=funcDecl.copy();
            if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
            warn(root->fileName,root->startLine,
               "warning: Cannot determine file/namespace for relatedalso function\n%s",
               fullFuncDecl.data()
              );   
          }
        }
      }
      else
      {
        warn_undoc(root->fileName,root->startLine,
                   "warning: class `%s' for related function `%s' is not "
                   "documented.", 
                   className.data(),funcName.data()
                  );
      }
    }
    else if (rootNav->parent() && rootNav->parent()->section()==Entry::OBJCIMPL_SEC)
    {
localObjCMethod:
      ClassDef *cd;
      //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
      if (Config_getBool("EXTRACT_LOCAL_METHODS") && (cd=getClass(scopeName)))
      {
        Debug::print(Debug::FindMembers,0,"4. Local objective C method %s\n"
              "  scopeName=%s className=%s\n",root->name.data(),scopeName.data(),className.data());
        //printf("Local objective C method `%s' of class `%s' found\n",root->name.data(),cd->name().data());
        MemberDef *md=new MemberDef(
            root->fileName,root->startLine,
            funcType,funcName,funcArgs,exceptions,
            root->protection,root->virt,root->stat,Member,
            MemberDef::Function,0,root->argList);
        md->setTagInfo(rootNav->tagInfo());
        md->setLanguage(root->lang);
        md->makeImplementationDetail();
        md->setMemberClass(cd);
        md->setDefinition(funcDecl);
        md->enableCallGraph(root->callGraph);
        md->enableCallerGraph(root->callerGraph);
        md->setDocumentation(root->doc,root->docFile,root->docLine);
        md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
        md->setDocsForDefinition(!root->proto);
        md->setPrototype(root->proto);
        md->addSectionsToDefinition(root->anchors);
        md->setBodySegment(root->bodyLine,root->endBodyLine);
        FileDef *fd=rootNav->fileDef();
        md->setBodyDef(fd);
        md->setMemberSpecifiers(root->spec);
        md->setMemberGroupId(root->mGrpId);
        cd->insertMember(md);
        cd->insertUsedFile(root->fileName);
        md->setRefItems(root->sli);
        if ((mn=Doxygen::memberNameSDict->find(root->name)))
        {
          mn->append(md);
        }
        else 
        {
          mn = new MemberName(root->name);
          mn->append(md);
          Doxygen::memberNameSDict->append(root->name,mn);
        }
      }
      else
      {
        // local objective C method found for class without interface
      }
    }
    else // unrelated not overloaded member found
    {
      bool globMem = findGlobalMember(rootNav,namespaceName,funcName,funcTempList,funcArgs,funcDecl);
      if (className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "warning: class for member `%s' cannot "
             "be found.", funcName.data()
            ); 
      }
      else if (!className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "warning: member `%s' of class `%s' cannot be found",
             funcName.data(),className.data());
      }
    }
  }
  else
  {
    // this should not be called
    warn(root->fileName,root->startLine,
         "warning: member with no name found.");
  }
  return;
} 

//----------------------------------------------------------------------
// find the members corresponding to the different documentation blocks
// that are extracted from the sources.

static void filterMemberDocumentation(EntryNav *rootNav)
{
  Entry *root = rootNav->entry();
  int i=-1,l;
  Debug::print(Debug::FindMembers,0,
      "findMemberDocumentation(): root->type=`%s' root->inside=`%s' root->name=`%s' root->args=`%s' section=%x root->spec=%d root->mGrpId=%d\n",
      root->type.data(),root->inside.data(),root->name.data(),root->args.data(),root->section,root->spec,root->mGrpId
      );
  //printf("rootNav->parent()->name()=%s\n",rootNav->parent()->name().data());
  bool isFunc=TRUE;

  if (root->relatesType == Duplicate && !root->relates.isEmpty())
  {
    QCString tmp = root->relates;
    root->relates.resize(0);
    filterMemberDocumentation(rootNav);
    root->relates = tmp;
  }

  if ( // detect func variable/typedef to func ptr
      (i=findFunctionPtr(root->type,root->lang,&l))!=-1 
     )
  {
    //printf("Fixing function pointer!\n");
    // fix type and argument
    root->args.prepend(root->type.right(root->type.length()-i-l));
    root->type=root->type.left(i+l);
    //printf("Results type=%s,name=%s,args=%s\n",root->type.data(),root->name.data(),root->args.data());
    isFunc=FALSE;
  }
  else if ((root->type.left(8)=="typedef " && root->args.find('(')!=-1)) 
    // detect function types marked as functions
  {
    isFunc=FALSE;
  }

  //printf("Member %s isFunc=%d\n",root->name.data(),isFunc);
  if (root->section==Entry::MEMBERDOC_SEC)
  {
    //printf("Documentation for inline member `%s' found args=`%s'\n",
    //    root->name.data(),root->args.data());
    //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
    if (root->type.isEmpty())
    {
      findMember(rootNav,root->name+root->args+root->exception,FALSE,isFunc);
    }
    else
    {
      findMember(rootNav,root->type+" "+root->name+root->args+root->exception,FALSE,isFunc);
    }
  }
  else if (root->section==Entry::OVERLOADDOC_SEC) 
  {
    //printf("Overloaded member %s found\n",root->name.data());
    findMember(rootNav,root->name,TRUE,isFunc);
  }
  else if 
    ((root->section==Entry::FUNCTION_SEC      // function
      ||   
      (root->section==Entry::VARIABLE_SEC &&  // variable
       !root->type.isEmpty() &&                // with a type
       g_compoundKeywordDict.find(root->type)==0 // that is not a keyword 
       // (to skip forward declaration of class etc.)
      )
     ) 
    )
    {
      //printf("Documentation for member `%s' found args=`%s' excp=`%s'\n",
      //    root->name.data(),root->args.data(),root->exception.data());
      //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
      //printf("Inside=%s\n Relates=%s\n",root->inside.data(),root->relates.data());
      if (root->type=="friend class" || root->type=="friend struct" || 
          root->type=="friend union")
      {
        findMember(rootNav,
            root->type+" "+
            root->name,
            FALSE,FALSE);

      }
      else if (!root->type.isEmpty())
      {
        findMember(rootNav,
            root->type+" "+
            root->inside+
            root->name+
            root->args+
            root->exception,
            FALSE,isFunc);
      }
      else
      {
        findMember(rootNav,
            root->inside+
            root->name+
            root->args+
            root->exception,
            FALSE,isFunc);
      }
    }
  else if (root->section==Entry::DEFINE_SEC && !root->relates.isEmpty())
  {
    findMember(rootNav,root->name+root->args,FALSE,!root->args.isEmpty());
  }
  else if (root->section==Entry::VARIABLEDOC_SEC)
  {
    //printf("Documentation for variable %s found\n",root->name.data());
    //if (!root->relates.isEmpty()) printf("  Relates %s\n",root->relates.data());
    findMember(rootNav,root->name,FALSE,FALSE);
  }
  else
  {
    // skip section 
    //printf("skip section\n");
  }
}

static void findMemberDocumentation(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::MEMBERDOC_SEC ||
      rootNav->section()==Entry::OVERLOADDOC_SEC ||
      rootNav->section()==Entry::FUNCTION_SEC ||
      rootNav->section()==Entry::VARIABLE_SEC ||
      rootNav->section()==Entry::VARIABLEDOC_SEC ||
      rootNav->section()==Entry::DEFINE_SEC
     )
  {
    rootNav->loadEntry(g_storage);

    filterMemberDocumentation(rootNav);

    rootNav->releaseEntry();
  }
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *e;
    for (;(e=eli.current());++eli)
    {
      if (e->section()!=Entry::ENUM_SEC) findMemberDocumentation(e);
    }
  }
}

//----------------------------------------------------------------------

static void findObjCMethodDefinitions(EntryNav *rootNav)
{
  if (rootNav->children())
  {
    EntryNavListIterator eli(*rootNav->children());
    EntryNav *objCImplNav;
    for (;(objCImplNav=eli.current());++eli)
    {
      if (objCImplNav->section()==Entry::OBJCIMPL_SEC && objCImplNav->children())
      {
        EntryNavListIterator seli(*objCImplNav->children());
        EntryNav *objCMethodNav;
        for (;(objCMethodNav=seli.current());++seli)
        {
          if (objCMethodNav->section()==Entry::FUNCTION_SEC)
          {
            objCMethodNav->loadEntry(g_storage);
            Entry *objCMethod = objCMethodNav->entry();

            //Printf("  Found ObjC method definition %s\n",objCMethod->name.data());
            findMember(objCMethodNav, objCMethod->type+" "+objCImplNav->name()+"::"+
                       objCMethod->name+" "+objCMethod->args, FALSE,TRUE);
            objCMethod->section=Entry::EMPTY_SEC;

            objCMethodNav->releaseEntry();
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------
// find and add the enumeration to their classes, namespaces or files

static void findEnums(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::ENUM_SEC)
    // non anonymous enumeration
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    MemberDef      *md=0;
    ClassDef       *cd=0;
    FileDef        *fd=0;
    NamespaceDef   *nd=0;
    MemberNameSDict *mnsd=0;
    bool isGlobal;
    bool isRelated=FALSE;
    bool isMemberOf=FALSE;
    //printf("Found enum with name `%s' relates=%s\n",root->name.data(),root->relates.data());
    int i;

    QCString name;
    QCString scope;

    if ((i=root->name.findRev("::"))!=-1) // scope is specified
    {
      scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }
    else // no scope, check the scope in which the docs where found
    {
      if (( rootNav->parent()->section() & Entry::SCOPE_MASK )
          && !rootNav->parent()->name().isEmpty()
         ) // found enum docs inside a compound
      {
        scope=rootNav->parent()->name();
        if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
      }
      name=root->name;
    }

    if (!root->relates.isEmpty()) 
    {   // related member, prefix user specified scope
      isRelated=TRUE;
      isMemberOf=(root->relatesType == MemberOf);
      if (getClass(root->relates)==0 && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else 
        scope=root->relates.copy();
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }

    if (cd && !name.isEmpty()) // found a enum inside a compound
    {
      //printf("Enum `%s'::`%s'\n",cd->name(),name.data());
      fd=0;
      mnsd=Doxygen::memberNameSDict;
      isGlobal=FALSE;
    }
    else if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@') // found enum inside namespace
    {
      mnsd=Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }
    else // found a global enum
    {
      fd=rootNav->fileDef();
      mnsd=Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }

    if (!name.isEmpty())
    {
      // new enum type
      md = new MemberDef(
          root->fileName,root->startLine,
          0,name,0,0,
          root->protection,Normal,FALSE,
          isMemberOf ? Foreign : isRelated ? Related : Member,
          MemberDef::Enumeration,
          0,0);
      md->setTagInfo(rootNav->tagInfo());
      md->setLanguage(root->lang);
      if (!isGlobal) md->setMemberClass(cd); else md->setFileDef(fd);
      md->setBodySegment(root->bodyLine,root->endBodyLine);
      md->setBodyDef(rootNav->fileDef());
      md->setEnumBaseType(root->args);
      //printf("Enum %s definition at line %d of %s: protection=%d\n",
      //    root->name.data(),root->bodyLine,root->fileName.data(),root->protection);
      md->addSectionsToDefinition(root->anchors);
      md->setMemberGroupId(root->mGrpId);
      md->enableCallGraph(root->callGraph);
      md->enableCallerGraph(root->callerGraph);
      //printf("%s::setRefItems(%d)\n",md->name().data(),root->sli?root->sli->count():-1);
      md->setRefItems(root->sli);
      //printf("found enum %s nd=%p\n",name.data(),nd);
      bool defSet=FALSE;

      QCString baseType = root->args;
      if (!baseType.isEmpty())
      {
        baseType.prepend(" : ");
      }

      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
      {
        if (isRelated || Config_getBool("HIDE_SCOPE_NAMES"))
        {
          md->setDefinition(name+baseType);  
        }
        else
        {
          md->setDefinition(nd->name()+"::"+name+baseType);  
        }
        //printf("definition=%s\n",md->definition());
        defSet=TRUE;
        md->setNamespace(nd);
        nd->insertMember(md);
      }

      // even if we have already added the enum to a namespace, we still
      // also want to add it to other appropriate places such as file
      // or class.
      if (isGlobal)
      {
        if (!defSet) md->setDefinition(name+baseType);
        if (fd==0 && rootNav->parent())
        {
          fd=rootNav->parent()->fileDef();
        }
        if (fd) 
        {
          md->setFileDef(fd);
          fd->insertMember(md);
        }
      }
      else if (cd)
      {
        if (isRelated || Config_getBool("HIDE_SCOPE_NAMES"))
        {
          md->setDefinition(name+baseType);  
        }
        else
        {
          md->setDefinition(cd->name()+"::"+name+baseType);  
        }
        cd->insertMember(md);
        cd->insertUsedFile(root->fileName);
      }
      md->setDocumentation(root->doc,root->docFile,root->docLine);
      md->setDocsForDefinition(!root->proto);
      md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);

      //printf("Adding member=%s\n",md->name().data());
      MemberName *mn;
      if ((mn=(*mnsd)[name]))
      {
        // this is used if the same enum is in multiple namespaces/classes
        mn->append(md);
      }
      else // new enum name
      {
        mn = new MemberName(name);
        mn->append(md);
        mnsd->append(name,mn);
        //printf("add %s to new memberName. Now %d members\n",
        //       name.data(),mn->count());
      }
      addMemberToGroups(root,md);

#if 0
      if (rootNav->children())
      {
        EntryNavListIterator eli(*rootNav->children());
        EntryNav *e;
        for (;(e=eli.current());++eli)
        {
          //printf("e->name=%s isRelated=%d\n",e->name.data(),isRelated);
          MemberName *fmn=0;
          MemberNameSDict *emnsd = isRelated ? Doxygen::functionNameSDict : mnsd;
          if (!e->name().isEmpty() && (fmn=(*emnsd)[e->name()])) 
            // get list of members with the same name as the field
          {
            MemberNameIterator fmni(*fmn);
            MemberDef *fmd;
            for (fmni.toFirst(); (fmd=fmni.current()) ; ++fmni) 
            {
              if (fmd->isEnumValue())
              {
                //printf("found enum value with same name\n");
                if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
                {
                  NamespaceDef *fnd=fmd->getNamespaceDef();
                  if (fnd==nd) // enum value is inside a namespace
                  {
                    md->insertEnumField(fmd);
                    fmd->setEnumScope(md);
                  }
                }
                else if (isGlobal)
                {
                  FileDef *ffd=fmd->getFileDef();
                  if (ffd==fd) // enum value has file scope
                  {
                    md->insertEnumField(fmd);
                    fmd->setEnumScope(md);
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
                  ClassDef *fcd=fmd->getClassDef();
                  if (fcd==cd) // enum value is inside a class
                  {
                    //printf("Inserting enum field %s in enum scope %s\n",
                    //    fmd->name().data(),md->name().data());
                    md->insertEnumField(fmd); // add field def to list
                    fmd->setEnumScope(md);    // cross ref with enum name
                  }
                }
              } 
            }
          }
        }
      }
#endif
    }

    rootNav->releaseEntry();
  }
  else
  {
    RECURSE_ENTRYTREE(findEnums,rootNav);
  }
}

//----------------------------------------------------------------------

static void addEnumValuesToEnums(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::ENUM_SEC)
    // non anonymous enumeration
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    ClassDef       *cd=0;
    FileDef        *fd=0;
    NamespaceDef   *nd=0;
    MemberNameSDict *mnsd=0;
    bool isGlobal;
    bool isRelated=FALSE;
    //printf("Found enum with name `%s' relates=%s\n",root->name.data(),root->relates.data());
    int i;

    QCString name;
    QCString scope;

    if ((i=root->name.findRev("::"))!=-1) // scope is specified
    {
      scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }
    else // no scope, check the scope in which the docs where found
    {
      if (( rootNav->parent()->section() & Entry::SCOPE_MASK )
          && !rootNav->parent()->name().isEmpty()
         ) // found enum docs inside a compound
      {
        scope=rootNav->parent()->name();
        if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
      }
      name=root->name;
    }

    if (!root->relates.isEmpty()) 
    {   // related member, prefix user specified scope
      isRelated=TRUE;
      if (getClass(root->relates)==0 && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else 
        scope=root->relates.copy();
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }

    if (cd && !name.isEmpty()) // found a enum inside a compound
    {
      //printf("Enum in class `%s'::`%s'\n",cd->name().data(),name.data());
      fd=0;
      mnsd=Doxygen::memberNameSDict;
      isGlobal=FALSE;
    }
    else if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@') // found enum inside namespace
    {
      //printf("Enum in namespace `%s'::`%s'\n",nd->name().data(),name.data());
      mnsd=Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }
    else // found a global enum
    {
      fd=rootNav->fileDef();
      //printf("Enum in file `%s': `%s'\n",fd->name().data(),name.data());
      mnsd=Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }

    if (!name.isEmpty())
    {
      //printf("** name=%s\n",name.data());
      MemberName *mn = mnsd->find(name); // for all members with this name
      if (mn)
      {
        MemberNameIterator mni(*mn);
        MemberDef *md;
        for (mni.toFirst(); (md=mni.current()) ; ++mni)  // for each enum in this list
        {
          if (md->isEnumerate() && rootNav->children())
          {
            //printf("   enum with %d children\n",rootNav->children()->count());
            EntryNavListIterator eli(*rootNav->children()); // for each enum value
            EntryNav *e;
            for (;(e=eli.current());++eli)
            {
              SrcLangExt sle;
              if (
                   (sle=rootNav->lang())==SrcLangExt_CSharp || 
                   sle==SrcLangExt_Java || 
                   sle==SrcLangExt_XML ||
                   (root->spec&Entry::Strong)
                 )
              {
                // Unlike classic C/C++ enums, for C++11, C# & Java enum 
                // values are only inside the enum scope, so we must create 
                // them here and only add them to the enum
                e->loadEntry(g_storage);
                Entry *root = e->entry();
                //printf("md->qualifiedName()=%s rootNav->name()=%s\n",
                //    md->qualifiedName().data(),rootNav->name().data());
                if (md->qualifiedName()==substitute(rootNav->name(),"::",".")) // enum value scope matches that of the enum
                {
                  MemberDef *fmd=new MemberDef(
                      root->fileName,root->startLine,
                      root->type,root->name,root->args,0,
                      Public, Normal,root->stat,Member,
                      MemberDef::EnumValue,0,0);
                  if (md->getClassDef()) fmd->setMemberClass(md->getClassDef());
                  else if (md->getNamespaceDef()) fmd->setNamespace(md->getNamespaceDef());
                  else if (md->getFileDef()) fmd->setFileDef(md->getFileDef());
                  fmd->setOuterScope(md->getOuterScope());
                  fmd->setTagInfo(e->tagInfo());
                  fmd->setLanguage(root->lang);
                  fmd->setDocumentation(root->doc,root->docFile,root->docLine);
                  fmd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
                  fmd->addSectionsToDefinition(root->anchors);
                  fmd->setInitializer(root->initializer);
                  fmd->setMaxInitLines(root->initLines);
                  fmd->setMemberGroupId(root->mGrpId);
                  fmd->setExplicitExternal(root->explicitExternal);
                  fmd->setRefItems(root->sli);
                  if (fmd)
                  {
                    md->insertEnumField(fmd);
                    fmd->setEnumScope(md);
                  }
                }
                e->releaseEntry();
              }
              else
              {
                //printf("e->name=%s isRelated=%d\n",e->name().data(),isRelated);
                MemberName *fmn=0;
                MemberNameSDict *emnsd = isRelated ? Doxygen::functionNameSDict : mnsd;
                if (!e->name().isEmpty() && (fmn=(*emnsd)[e->name()])) 
                  // get list of members with the same name as the field
                {
                  MemberNameIterator fmni(*fmn);
                  MemberDef *fmd;
                  for (fmni.toFirst(); (fmd=fmni.current()) ; ++fmni) 
                  {
                    if (fmd->isEnumValue() && fmd->getOuterScope()==md->getOuterScope()) // in same scope
                    {
                      //printf("found enum value with same name %s in scope %s\n",
                      //    fmd->name().data(),fmd->getOuterScope()->name().data());
                      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
                      {
                        NamespaceDef *fnd=fmd->getNamespaceDef();
                        if (fnd==nd) // enum value is inside a namespace
                        {
                          md->insertEnumField(fmd);
                          fmd->setEnumScope(md);
                        }
                      }
                      else if (isGlobal)
                      {
                        FileDef *ffd=fmd->getFileDef();
                        if (ffd==fd) // enum value has file scope
                        {
                          md->insertEnumField(fmd);
                          fmd->setEnumScope(md);
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
                        ClassDef *fcd=fmd->getClassDef();
                        if (fcd==cd) // enum value is inside a class
                        {
                          //printf("Inserting enum field %s in enum scope %s\n",
                          //    fmd->name().data(),md->name().data());
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
    }

    rootNav->releaseEntry();
  }
  else
  {
    RECURSE_ENTRYTREE(addEnumValuesToEnums,rootNav);
  }
}


//----------------------------------------------------------------------
// find the documentation blocks for the enumerations

static void findEnumDocumentation(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::ENUMDOC_SEC
      && !rootNav->name().isEmpty()
      && rootNav->name().at(0)!='@'        // skip anonymous enums
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    //printf("Found docs for enum with name `%s' in context %s\n",
    //    root->name.data(),root->parent->name.data());
    int i;
    QCString name;
    QCString scope;
    if ((i=root->name.findRev("::"))!=-1) // scope is specified as part of the name
    {
      name=root->name.right(root->name.length()-i-2); // extract name
      scope=root->name.left(i); // extract scope
      //printf("Scope=`%s' Name=`%s'\n",scope.data(),name.data());
    }
    else // just the name
    {
      name=root->name;
    }
    if (( rootNav->parent()->section() & Entry::SCOPE_MASK )
        && !rootNav->parent()->name().isEmpty()
       ) // found enum docs inside a compound
    {
      if (!scope.isEmpty()) scope.prepend("::");
      scope.prepend(rootNav->parent()->name());
    }
    ClassDef *cd=getClass(scope);

    if (!name.isEmpty())
    {
      bool found=FALSE;
      if (cd)
      {
        //printf("Enum: scope=`%s' name=`%s'\n",cd->name(),name.data());
        QCString className=cd->name().copy();
        MemberName *mn=Doxygen::memberNameSDict->find(name);
        if (mn)
        {
          MemberNameIterator mni(*mn);
          MemberDef *md;
          for (mni.toFirst();(md=mni.current()) && !found;++mni)
          {
            ClassDef *cd=md->getClassDef();
            if (cd && cd->name()==className && md->isEnumerate())
            {
              // documentation outside a compound overrides the documentation inside it
#if 0
              if (!md->documentation() || rootNav->parent()->name().isEmpty()) 
#endif
              {
                md->setDocumentation(root->doc,root->docFile,root->docLine);
                md->setDocsForDefinition(!root->proto);
              }

              // brief descriptions inside a compound override the documentation 
              // outside it
#if 0
              if (!md->briefDescription() || !rootNav->parent()->name().isEmpty())
#endif
              {
                md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
              }

              if (!md->inbodyDocumentation() || !rootNav->parent()->name().isEmpty())
              {
                md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
              }

              if (root->mGrpId!=-1 && md->getMemberGroupId()==-1)
              {
                md->setMemberGroupId(root->mGrpId);
              }

              md->addSectionsToDefinition(root->anchors);
              md->setRefItems(root->sli);

              GroupDef *gd=md->getGroupDef();
              if (gd==0 &&root->groups->first()!=0) // member not grouped but out-of-line documentation is
              {
                addMemberToGroups(root,md);
              }

              found=TRUE;
            }
          }
        }
        else
        {
          //printf("MemberName %s not found!\n",name.data());
        }
      }
      else // enum outside class 
      {
        //printf("Enum outside class: %s grpId=%d\n",name.data(),root->mGrpId);
        MemberName *mn=Doxygen::functionNameSDict->find(name);
        if (mn)
        {
          MemberNameIterator mni(*mn);
          MemberDef *md;
          for (mni.toFirst();(md=mni.current()) && !found;++mni)
          {
            if (md->isEnumerate())
            {
              md->setDocumentation(root->doc,root->docFile,root->docLine);
              md->setDocsForDefinition(!root->proto);
              md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
              md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
              md->addSectionsToDefinition(root->anchors);
              md->setMemberGroupId(root->mGrpId);

              GroupDef *gd=md->getGroupDef();
              if (gd==0 && root->groups->first()!=0) // member not grouped but out-of-line documentation is
              {
                addMemberToGroups(root,md);
              }

              found=TRUE;
            }
          }
        }
      } 
      if (!found)
      {
        warn(root->fileName,root->startLine,
             "warning: Documentation for undefined enum `%s' found.",
             name.data()
            );
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findEnumDocumentation,rootNav);
}

// search for each enum (member or function) in mnl if it has documented 
// enum values.
static void findDEV(const MemberNameSDict &mnsd)
{
  MemberName *mn;
  MemberNameSDict::Iterator mnli(mnsd);
  // for each member name
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    // for each member definition
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (md->isEnumerate()) // member is an enum
      {
        LockingPtr<MemberList> fmdl = md->enumFieldList();
        int documentedEnumValues=0;
        if (fmdl!=0) // enum has values
        {
          MemberListIterator fmni(*fmdl);
          MemberDef *fmd;
          // for each enum value
          for (fmni.toFirst();(fmd=fmni.current());++fmni)
          {
            if (fmd->isLinkableInProject()) documentedEnumValues++;
          }
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
  findDEV(*Doxygen::memberNameSDict);
  findDEV(*Doxygen::functionNameSDict); 
}

//----------------------------------------------------------------------

static void addMembersToIndex()
{
  MemberName *mn;
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  // for each member name
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    // for each member definition
    for (mni.toFirst();(md=mni.current());++mni)
    {
      addClassMemberNameToIndex(md);
    }
  }
  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  // for each member name
  for (fnli.toFirst();(mn=fnli.current());++fnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    // for each member definition
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (md->getNamespaceDef())
      {
        addNamespaceMemberNameToIndex(md);
      }
      else
      {
        addFileMemberNameToIndex(md);
      }
    }
  }
}

//----------------------------------------------------------------------
// computes the relation between all members. For each member `m'
// the members that override the implementation of `m' are searched and
// the member that `m' overrides is searched.

static void computeMemberRelations()
{
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  MemberName *mn;
  for ( ; (mn=mnli.current()) ; ++mnli ) // for each member name
  {
    MemberNameIterator mdi(*mn);
    MemberDef *md;
    for ( ; (md=mdi.current()) ; ++mdi ) // for each member with a specific name
    {
      MemberDef *bmd = mn->first(); // for each other member with the same name
      while (bmd)
      {
        ClassDef *mcd  = md->getClassDef();
        if (mcd && mcd->baseClasses())
        {
          ClassDef *bmcd = bmd->getClassDef();
          //printf("Check relation between `%s'::`%s' (%p) and `%s'::`%s' (%p)\n",
          //      mcd->name().data(),md->name().data(),md,
          //       bmcd->name().data(),bmd->name().data(),bmd
          //      );
          if (md!=bmd && bmd->virtualness()!=Normal && md->isFunction() && 
              bmcd && mcd && mcd->isLinkable() && bmcd->isLinkable() &&
              bmcd!=mcd && mcd->isBaseClass(bmcd,TRUE))
          {
            //printf("  derived scope\n");
            LockingPtr<ArgumentList> bmdAl = bmd->argumentList();
            LockingPtr<ArgumentList>  mdAl =  md->argumentList();
            //printf(" Base argList=`%s'\n Super argList=`%s'\n",
            //        argListToString(bmdAl.pointer()).data(),
            //        argListToString(mdAl.pointer()).data()
            //      );
            if ( 
                matchArguments2(bmd->getOuterScope(),bmd->getFileDef(),bmdAl.pointer(),
                  md->getOuterScope(), md->getFileDef(), mdAl.pointer(),
                  TRUE
                  ) 
               )
            {
              MemberDef *rmd;
              if ((rmd=md->reimplements())==0 ||
                  minClassDistance(mcd,bmcd)<minClassDistance(mcd,rmd->getClassDef())
                 )
              {
                //printf("setting (new) reimplements member\n");
                md->setReimplements(bmd);
              }
              //printf("%s: add reimplementedBy member %s\n",bmcd->name().data(),mcd->name().data());
              bmd->insertReimplementedBy(md);
            }
          }
        }
        bmd = mn->next();
      }
    }
  }  
}


//----------------------------------------------------------------------------
//static void computeClassImplUsageRelations()
//{
//  ClassDef *cd;
//  ClassSDict::Iterator cli(*Doxygen::classSDict);
//  for (;(cd=cli.current());++cli)
//  {
//    cd->determineImplUsageRelation();
//  }
//}

//----------------------------------------------------------------------------

static void createTemplateInstanceMembers()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  // for each class
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    // that is a template
    QDict<ClassDef> *templInstances = cd->getTemplateInstances();
    if (templInstances)
    {
      QDictIterator<ClassDef> qdi(*templInstances);
      ClassDef *tcd=0;
      // for each instance of the template
      for (qdi.toFirst();(tcd=qdi.current());++qdi)
      {
        tcd->addMembersToTemplateInstance(cd,qdi.currentKey());
      }
    }
  }
}

//----------------------------------------------------------------------------

static void mergeCategories()
{
  ClassDef *cd;
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  // merge members of categories into the class they extend
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    int i=cd->name().find('(');
    if (i!=-1) // it is an Objective-C category
    {
      QCString baseName=cd->name().left(i);
      ClassDef *baseClass=Doxygen::classSDict->find(baseName);
      if (baseClass)
      {
        //printf("*** merging members of category %s into %s\n",
        //    cd->name().data(),baseClass->name().data());
        baseClass->mergeCategory(cd);
      }
    }
  }
}

// builds the list of all members for each class

static void buildCompleteMemberLists()
{
  ClassDef *cd;
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  // merge the member list of base classes into the inherited classes.
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (// !cd->isReference() && // not an external class
         cd->subClasses()==0 && // is a root of the hierarchy
         cd->baseClasses()) // and has at least one base class
    {
      //printf("*** merging members for %s\n",cd->name().data());
      cd->mergeMembers();
    }
  }
  // now sort the member list of all classes.
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (cd->memberNameInfoSDict()) cd->memberNameInfoSDict()->sort();
  }
}

//----------------------------------------------------------------------------

static void generateFileSources()
{
  if (documentedHtmlFiles==0) return;
  if (Doxygen::inputNameList->count()>0)
  {
    FileNameListIterator fnli(*Doxygen::inputNameList); 
    FileName *fn;
    for (;(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (;(fd=fni.current());++fni)
      {
        if (fd->generateSourceFile()) // sources need to be shown in the output
        {
          msg("Generating code for file %s...\n",fd->docName().data());
          fd->writeSource(*g_outputList);
        }
        else if (!fd->isReference() && Doxygen::parseSourcesNeeded)
          // we needed to parse the sources even if we do not show them
        {
          msg("Parsing code for file %s...\n",fd->docName().data());
          fd->parseSource();
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void generateFileDocs()
{
  if (documentedHtmlFiles==0) return;
  
  if (Doxygen::inputNameList->count()>0)
  {
    FileNameListIterator fnli(*Doxygen::inputNameList);
    FileName *fn;
    for (fnli.toFirst();(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (fni.toFirst();(fd=fni.current());++fni)
      {
        bool doc = fd->isLinkableInProject();
        if (doc)
        {
          msg("Generating docs for file %s...\n",fd->docName().data());
          fd->writeDocumentation(*g_outputList);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void addSourceReferences()
{
  // add source references for class definitions
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    FileDef *fd=cd->getBodyDef();
    if (fd && cd->isLinkableInProject() && cd->getStartBodyLine()!=-1)
    {
      fd->addSourceRef(cd->getStartBodyLine(),cd,0);
    }
  }
  // add source references for namespace definitions
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    FileDef *fd=nd->getBodyDef();
    if (fd && nd->isLinkableInProject() && nd->getStartBodyLine()!=-1)
    {
      fd->addSourceRef(nd->getStartBodyLine(),nd,0);
    }
  }
  
  // add source references for member names
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      //printf("class member %s: def=%s body=%d link?=%d\n",
      //    md->name().data(),
      //    md->getBodyDef()?md->getBodyDef()->name().data():"<none>",
      //    md->getStartBodyLine(),md->isLinkableInProject());
      FileDef *fd=md->getBodyDef();
      if (fd && 
          md->getStartBodyLine()!=-1 &&
          md->isLinkableInProject() &&
          (fd->generateSourceFile() || Doxygen::parseSourcesNeeded)
         )
      {
        //printf("Found member `%s' in file `%s' at line `%d' def=%s\n",
        //    md->name().data(),fd->name().data(),md->getStartBodyLine(),md->getOuterScope()->name().data()); 
        fd->addSourceRef(md->getStartBodyLine(),md->getOuterScope(),md);
      }
    }
  }
  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  for (fnli.toFirst();(mn=fnli.current());++fnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      FileDef *fd=md->getBodyDef();
      //printf("member %s body=[%d,%d] fd=%p link=%d parseSources=%d\n",
      //    md->name().data(),
      //    md->getStartBodyLine(),md->getEndBodyLine(),fd,
      //    md->isLinkableInProject(),
      //    Doxygen::parseSourcesNeeded);
      if (fd && 
          md->getStartBodyLine()!=-1 && 
          md->isLinkableInProject() && 
          (fd->generateSourceFile() || Doxygen::parseSourcesNeeded)
         )
      {
        //printf("Found member `%s' in file `%s' at line `%d' def=%s\n",
        //    md->name().data(),fd->name().data(),md->getStartBodyLine(),md->getOuterScope()->name().data()); 
        fd->addSourceRef(md->getStartBodyLine(),md->getOuterScope(),md);
      }  
    }
  }
}

//----------------------------------------------------------------------------

static void sortMemberLists()
{
  // sort class member lists
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->sortMemberLists();
  }

  // sort namespace member lists
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    nd->sortMemberLists();
  }

  // sort file member lists
  FileNameListIterator fnli(*Doxygen::inputNameList); 
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      fd->sortMemberLists();
    }
  }

  // sort group member lists
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->sortMemberLists();
  }
}

//----------------------------------------------------------------------------
// generate the documentation of all classes
  
static void generateClassList(ClassSDict &classSDict)
{
  ClassSDict::Iterator cli(classSDict);
  for ( ; cli.current() ; ++cli )
  {
    ClassDef *cd=cli.current();
   
    //printf("cd=%s getOuterScope=%p global=%p\n",cd->name().data(),cd->getOuterScope(),Doxygen::globalScope);
    if ((cd->getOuterScope()==0 || // <-- should not happen, but can if we read an old tag file
         cd->getOuterScope()==Doxygen::globalScope // only look at global classes
        ) && !cd->isHidden() && !cd->isEmbeddedInOuterScope()
       ) 
    {
      // skip external references, anonymous compounds and 
      // template instances 
      if ( cd->isLinkableInProject() && cd->templateMaster()==0)
      {
        msg("Generating docs for compound %s...\n",cd->name().data());

        cd->writeDocumentation(*g_outputList);
        cd->writeMemberList(*g_outputList);
      }
      // even for undocumented classes, the inner classes can be documented.
      cd->writeDocumentationForInnerClasses(*g_outputList);
    }
  }
}

static void generateClassDocs()
{
  generateClassList(*Doxygen::classSDict);
  generateClassList(*Doxygen::hiddenClasses);
}

//----------------------------------------------------------------------------

static void inheritDocumentation()
{
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  MemberName *mn;
  //int count=0;
  for (;(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (;(md=mni.current());++mni)
    {
      //printf("%04d Member `%s'\n",count++,md->name().data());
      if (md->documentation().isEmpty() && md->briefDescription().isEmpty())
      { // no documentation yet
        MemberDef *bmd = md->reimplements();
        while (bmd && bmd->documentation().isEmpty() && 
                      bmd->briefDescription().isEmpty()
              )
        { // search up the inheritance tree for a documentation member
          //printf("bmd=%s class=%s\n",bmd->name().data(),bmd->getClassDef()->name().data());
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
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      fd->visited=FALSE;
    }
  }
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      fd->combineUsingRelations();
    }
  }

  // for each namespace
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst() ; (nd=nli.current()) ; ++nli )
  {
    nd->visited=FALSE;
  }
  for (nli.toFirst() ; (nd=nli.current()) ; ++nli )
  {
    nd->combineUsingRelations();
  }
}

//----------------------------------------------------------------------------
  
static void addMembersToMemberGroup()
{
  // for each class
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->addMembersToMemberGroup();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList->first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->addMembersToMemberGroup();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList->next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->addMembersToMemberGroup();
  }
  // for each group
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->addMembersToMemberGroup();
  }
}

//----------------------------------------------------------------------------

static void distributeMemberGroupDocumentation()
{
  // for each class
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->distributeMemberGroupDocumentation();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList->first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->distributeMemberGroupDocumentation();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList->next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->distributeMemberGroupDocumentation();
  }
  // for each group
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->distributeMemberGroupDocumentation();
  }
}

//----------------------------------------------------------------------------

static void findSectionsInDocumentation()
{
  // for each class
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->findSectionsInDocumentation();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList->first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->findSectionsInDocumentation();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList->next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->findSectionsInDocumentation();
  }
  // for each group
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->findSectionsInDocumentation();
  }
  // for each page
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    pd->findSectionsInDocumentation();
  }
  if (Doxygen::mainPage) Doxygen::mainPage->findSectionsInDocumentation();
}

static void flushCachedTemplateRelations()
{
  // remove all references to classes from the cache
  // as there can be new template instances in the inheritance path
  // to this class. Optimization: only remove those classes that
  // have inheritance instances as direct or indirect sub classes.
  QCacheIterator<LookupInfo> ci(*Doxygen::lookupCache);
  LookupInfo *li=0;
  for (ci.toFirst();(li=ci.current());++ci)
  {
    if (li->classDef)
    {
      Doxygen::lookupCache->remove(ci.currentKey());
    }
  }
  // remove all cached typedef resolutions whose target is a
  // template class as this may now be a template instance
  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    MemberNameIterator fni(*fn);
    MemberDef *fmd;
    for (;(fmd=fni.current());++fni) // for each function with that name
    {
      if (fmd->isTypedefValCached())
      {
        ClassDef *cd = fmd->getCachedTypedefVal();
        if (cd->isTemplate()) fmd->invalidateTypedefValCache();
      }
    }
  }
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  for (;(fn=mnli.current());++mnli) // for each class method name
  {
    MemberNameIterator mni(*fn);
    MemberDef *fmd;
    for (;(fmd=mni.current());++mni) // for each function with that name
    {
      if (fmd->isTypedefValCached())
      {
        ClassDef *cd = fmd->getCachedTypedefVal();
        if (cd->isTemplate()) fmd->invalidateTypedefValCache();
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
  // (which is correct if you igore the inheritance relation between A and B).
  // 
  // class A { class I {} };
  // class B : public A {};
  // class C : public B::I {};
  //
  QCacheIterator<LookupInfo> ci(*Doxygen::lookupCache);
  LookupInfo *li=0;
  for (ci.toFirst();(li=ci.current());++ci)
  {
    if (li->classDef==0 && li->typeDef==0)
    {
      Doxygen::lookupCache->remove(ci.currentKey());
    }
  }

  MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    MemberNameIterator fni(*fn);
    MemberDef *fmd;
    for (;(fmd=fni.current());++fni) // for each function with that name
    {
      fmd->invalidateCachedArgumentTypes();
    }
  }
  MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
  for (;(fn=mnli.current());++mnli) // for each class method name
  {
    MemberNameIterator mni(*fn);
    MemberDef *fmd;
    for (;(fmd=mni.current());++mni) // for each function with that name
    {
      fmd->invalidateCachedArgumentTypes();
    }
  }

}

//----------------------------------------------------------------------------

static void findDefineDocumentation(EntryNav *rootNav)
{
  if ((rootNav->section()==Entry::DEFINEDOC_SEC ||
       rootNav->section()==Entry::DEFINE_SEC) && !rootNav->name().isEmpty()
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();
    
    //printf("found define `%s' `%s' brief=`%s' doc=`%s'\n",
    //       root->name.data(),root->args.data(),root->brief.data(),root->doc.data());

    if (rootNav->tagInfo() && !root->name.isEmpty()) // define read from a tag file
    {
      MemberDef *md=new MemberDef("<tagfile>",1,
                    "#define",root->name,root->args,0,
                    Public,Normal,FALSE,Member,MemberDef::Define,0,0);
      md->setTagInfo(rootNav->tagInfo());
      md->setLanguage(root->lang);
      //printf("Searching for `%s' fd=%p\n",filePathName.data(),fd);
      md->setFileDef(rootNav->parent()->fileDef());
      //printf("Adding member=%s\n",md->name().data());
      MemberName *mn;
      if ((mn=Doxygen::functionNameSDict->find(root->name)))
      {
        mn->append(md);
      }
      else 
      {
        mn = new MemberName(root->name);
        mn->append(md);
        Doxygen::functionNameSDict->append(root->name,mn);
      }
    }
    MemberName *mn=Doxygen::functionNameSDict->find(root->name);
    if (mn)
    {
      int count=0;
      MemberDef *md=mn->first();
      while (md)
      {
        if (md->memberType()==MemberDef::Define) count++;
        md=mn->next();
      }
      if (count==1)
      {
        md=mn->first();
        while (md)
        {
          if (md->memberType()==MemberDef::Define)
          {
            md->setDocumentation(root->doc,root->docFile,root->docLine);
            md->setDocsForDefinition(!root->proto);
            md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            if (md->inbodyDocumentation().isEmpty())
            {
              md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
            }
            md->setBodySegment(root->bodyLine,root->endBodyLine);
            md->setBodyDef(rootNav->fileDef());
            md->addSectionsToDefinition(root->anchors);
            md->setMaxInitLines(root->initLines);
            md->setRefItems(root->sli);
            if (root->mGrpId!=-1) md->setMemberGroupId(root->mGrpId);
            addMemberToGroups(root,md);
          }
          md=mn->next();
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
        md=mn->first();
        while (md)
        {
          if (md->memberType()==MemberDef::Define)
          {
            FileDef *fd=md->getFileDef();
            if (fd && fd->absFilePath()==root->fileName) 
              // doc and define in the same file assume they belong together.
            {
#if 0
              if (md->documentation().isEmpty())
#endif
              {
                md->setDocumentation(root->doc,root->docFile,root->docLine);
                md->setDocsForDefinition(!root->proto);
              }
#if 0
              if (md->briefDescription().isEmpty())
#endif
              {
                md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
              }
              if (md->inbodyDocumentation().isEmpty())
              {
                md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
              }
              md->setBodySegment(root->bodyLine,root->endBodyLine);
              md->setBodyDef(rootNav->fileDef());
              md->addSectionsToDefinition(root->anchors);
              md->setRefItems(root->sli);
              md->setLanguage(root->lang);
              if (root->mGrpId!=-1) md->setMemberGroupId(root->mGrpId);
              addMemberToGroups(root,md);
            }
          }
          md=mn->next();
        }
        //warn("warning: define %s found in the following files:\n",root->name.data());
        //warn("Cannot determine where to add the documentation found "
        //     "at line %d of file %s. \n",
        //     root->startLine,root->fileName.data());
      }
    }
    else if (!root->doc.isEmpty() || !root->brief.isEmpty()) // define not found
    {
      static bool preEnabled = Config_getBool("ENABLE_PREPROCESSING");
      if (preEnabled)
      {
        warn(root->fileName,root->startLine,
             "warning: documentation for unknown define %s found.\n",
             root->name.data()
            );
      }
      else
      {
        warn(root->fileName,root->startLine,
             "warning: found documented #define but ignoring it because "
             "ENABLE_PREPROCESSING is NO.\n",
             root->name.data()
            );
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findDefineDocumentation,rootNav);
}

//----------------------------------------------------------------------------

static void findDirDocumentation(EntryNav *rootNav)
{
  if (rootNav->section() == Entry::DIRDOC_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    QCString normalizedName = root->name;
    normalizedName = substitute(normalizedName,"\\","/");
    //printf("root->docFile=%s normalizedName=%s\n",
    //    root->docFile.data(),normalizedName.data());
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
    DirDef *dir,*matchingDir=0;
    SDict<DirDef>::Iterator sdi(*Doxygen::directories);
    for (sdi.toFirst();(dir=sdi.current());++sdi)
    {
      //printf("Dir: %s<->%s\n",dir->name().data(),normalizedName.data());
      if (dir->name().right(normalizedName.length())==normalizedName)
      {
        if (matchingDir)
        {
           warn(root->fileName,root->startLine,
             "warning: \\dir command matches multiple directories.\n"
             "  Applying the command for directory %s\n"
             "  Ignoring the command for directory %s\n",
             matchingDir->name().data(),dir->name().data()
           );
        }
        else
        {
          matchingDir=dir;
        }
      }
    }
    if (matchingDir)
    {
      //printf("Match for with dir %s\n",matchingDir->name().data());
      matchingDir->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      matchingDir->setDocumentation(root->doc,root->docFile,root->docLine);
      matchingDir->setRefItems(root->sli);
      addDirToGroups(root,matchingDir);
    }
    else
    {
      warn(root->fileName,root->startLine,"warning: No matching "
          "directory found for command \\dir %s\n",normalizedName.data());
    }
    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findDirDocumentation,rootNav);
}


//----------------------------------------------------------------------------
// create a (sorted) list of separate documentation pages

static void buildPageList(EntryNav *rootNav)
{
  if (rootNav->section() == Entry::PAGEDOC_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    if (!root->name.isEmpty())
    {
      addRelatedPage(rootNav);
    }

    rootNav->releaseEntry();
  }
  else if (rootNav->section() == Entry::MAINPAGEDOC_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    QCString title=root->args.stripWhiteSpace();
    if (title.isEmpty()) title=theTranslator->trMainPage();
    //QCString name = Config_getBool("GENERATE_TREEVIEW")?"main":"index";
    QCString name = "index";
    addRefItem(root->sli,
               name,
               "page",
               name,
               title,
               0
               );

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildPageList,rootNav);
}

static void findMainPage(EntryNav *rootNav)
{
  if (rootNav->section() == Entry::MAINPAGEDOC_SEC)
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    if (Doxygen::mainPage==0)
    {
      //printf("Found main page! \n======\n%s\n=======\n",root->doc.data());
      QCString title=root->args.stripWhiteSpace();
      //QCString indexName=Config_getBool("GENERATE_TREEVIEW")?"main":"index";
      QCString indexName="index";
      Doxygen::mainPage = new PageDef(root->docFile,root->docLine,
                              indexName, root->brief+root->doc+root->inbodyDocs,title);
      //setFileNameForSections(root->anchors,"index",Doxygen::mainPage);
      Doxygen::mainPage->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      Doxygen::mainPage->setFileName(indexName);
      Doxygen::mainPage->setShowToc(root->stat);
      addPageToContext(Doxygen::mainPage,rootNav);
          
      // a page name is a label as well!
      SectionInfo *si=new SectionInfo(
          indexName,
          Doxygen::mainPage->name(),
          Doxygen::mainPage->title(),
          SectionInfo::Page,
          0); // level 0
      Doxygen::sectionDict.append(indexName,si);
      Doxygen::mainPage->addSectionsToDefinition(root->anchors);
    }
    else
    {
      warn(root->fileName,root->startLine,
           "warning: found more than one \\mainpage comment block! Skipping this "
           "block."
          );
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(findMainPage,rootNav);
}

static void computePageRelations(EntryNav *rootNav)
{
  if ((rootNav->section()==Entry::PAGEDOC_SEC || 
       rootNav->section()==Entry::MAINPAGEDOC_SEC
      )
      && !rootNav->name().isEmpty()
     )
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    PageDef *pd = root->section==Entry::PAGEDOC_SEC ?
                    Doxygen::pageSDict->find(root->name) : 
                    Doxygen::mainPage; 
    if (pd)
    {
      QListIterator<BaseInfo> bii(*root->extends);
      BaseInfo *bi;
      for (bii.toFirst();(bi=bii.current());++bii)
      {
        PageDef *subPd = Doxygen::pageSDict->find(bi->name);
        if (subPd)
        {
          pd->addInnerCompound(subPd);
          //printf("*** Added subpage relation: %s->%s\n",
          //    pd->name().data(),subPd->name().data());
        }
      }
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(computePageRelations,rootNav);
}

static void checkPageRelations()
{
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    Definition *ppd = pd->getOuterScope();
    while (ppd)
    {
      if (ppd==pd)
      {
        err("warning: page defined at line %d of file %s with label %s is a subpage "
            "of itself! Please remove this cyclic dependency.\n",
            pd->docLine(),pd->docFile().data(),pd->name().data());
        exit(1);
      }
      ppd=ppd->getOuterScope();
    }
  }
}

//----------------------------------------------------------------------------

static void resolveUserReferences()
{
  SDict<SectionInfo>::Iterator sdi(Doxygen::sectionDict);
  SectionInfo *si;
  for (;(si=sdi.current());++sdi)
  {
    //printf("si->label=`%s' si->definition=%s si->fileName=`%s'\n",
    //        si->label.data(),si->definition?si->definition->name().data():"<none>",
    //        si->fileName.data());
    PageDef *pd=0;

    // hack: the items of a todo/test/bug/deprecated list are all fragments from 
    // different files, so the resulting section's all have the wrong file 
    // name (not from the todo/test/bug/deprecated list, but from the file in 
    // which they are defined). We correct this here by looking at the 
    // generated section labels!
    QDictIterator<RefList> rli(*Doxygen::xrefLists);
    RefList *rl;
    for (rli.toFirst();(rl=rli.current());++rli)
    {
      QCString label="_"+rl->listName(); // "_todo", "_test", ...
      if (si->label.left(label.length())==label)
      {
        si->fileName=rl->listName();
        si->generated=TRUE;
        break;
      }
    }

    //printf("start: si->label=%s si->fileName=%s\n",si->label.data(),si->fileName.data());
    if (!si->generated)
    {
      // if this section is in a page and the page is in a group, then we
      // have to adjust the link file name to point to the group.
      if (!si->fileName.isEmpty() && 
          (pd=Doxygen::pageSDict->find(si->fileName)) &&
          pd->getGroupDef())
      {
        si->fileName=pd->getGroupDef()->getOutputFileBase().copy();
      }

      if (si->definition)
      {
        // TODO: there should be one function in Definition that returns
        // the file to link to, so we can avoid the following tests.
        GroupDef *gd=0;
        if (si->definition->definitionType()==Definition::TypeMember)
        {
          gd = ((MemberDef *)si->definition)->getGroupDef();
        }

        if (gd)
        {
          si->fileName=gd->getOutputFileBase().copy();
        }
        else
        {
          //si->fileName=si->definition->getOutputFileBase().copy();
          //printf("Setting si->fileName to %s\n",si->fileName.data());
        }
      }
    }
    //printf("end: si->label=%s si->fileName=%s\n",si->label.data(),si->fileName.data());
  }
}



//----------------------------------------------------------------------------
// generate all separate documentation pages


static void generatePageDocs()
{
  //printf("documentedPages=%d real=%d\n",documentedPages,Doxygen::pageSDict->count());
  if (documentedPages==0) return;
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->getGroupDef() && !pd->isReference())
    {
      msg("Generating docs for page %s...\n",pd->name().data());
      Doxygen::insideMainPage=TRUE;
      pd->writeDocumentation(*g_outputList);
      Doxygen::insideMainPage=FALSE;
    }
  }
}

//----------------------------------------------------------------------------
// create a (sorted) list & dictionary of example pages

static void buildExampleList(EntryNav *rootNav)
{
  if (rootNav->section()==Entry::EXAMPLE_SEC && !rootNav->name().isEmpty()) 
  {
    rootNav->loadEntry(g_storage);
    Entry *root = rootNav->entry();

    if (Doxygen::exampleSDict->find(root->name))
    {
      warn(root->fileName,root->startLine,
          "warning: Example %s was already documented. Ignoring "
          "documentation found here.",
          root->name.data()
          );
    }
    else
    {
      PageDef *pd=new PageDef(root->fileName,root->startLine,
          root->name,root->brief+root->doc+root->inbodyDocs,root->args);
      pd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      pd->setFileName(convertNameToFile(pd->name()+"-example",FALSE,TRUE));
      pd->addSectionsToDefinition(root->anchors);
      pd->setLanguage(root->lang);
      //pi->addSections(root->anchors);

      Doxygen::exampleSDict->inSort(root->name,pd);
      //we don't add example to groups 
      //addExampleToGroups(root,pd);
    }

    rootNav->releaseEntry();
  }
  RECURSE_ENTRYTREE(buildExampleList,rootNav);
}

//----------------------------------------------------------------------------
// prints the Entry tree (for debugging)

void printNavTree(EntryNav *rootNav,int indent)
{
  QCString indentStr;
  indentStr.fill(' ',indent);
  msg("%s%s (sec=0x%x)\n",
      indentStr.isEmpty()?"":indentStr.data(),
      rootNav->name().isEmpty()?"<empty>":rootNav->name().data(),
      rootNav->section());
  if (rootNav->children()) 
  {
    EntryNavListIterator eli(*rootNav->children());
    for (;eli.current();++eli) printNavTree(eli.current(),indent+2);
  }
}


//----------------------------------------------------------------------------
// generate the example documentation 

static void generateExampleDocs()
{
  g_outputList->disable(OutputGenerator::Man);
  PageSDict::Iterator pdi(*Doxygen::exampleSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    msg("Generating docs for example %s...\n",pd->name().data());
    resetCCodeParserState();
    QCString n=pd->getOutputFileBase();
    startFile(*g_outputList,n,n,pd->name());
    startTitle(*g_outputList,n);
    g_outputList->docify(pd->name());
    endTitle(*g_outputList,n,0);
    g_outputList->startContents();
    g_outputList->parseDoc(pd->docFile(),                            // file
                         pd->docLine(),                            // startLine
                         pd,                                       // context
                         0,                                        // memberDef
                         pd->documentation()+"\n\n\\include "+pd->name(),          // docs
                         TRUE,                                     // index words
                         TRUE,                                     // is example
                         pd->name()
                        );
    endFile(*g_outputList); // contains g_outputList->endContents()
  }
  g_outputList->enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------
// generate module pages

static void generateGroupDocs()
{
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    if (!gd->isReference())
    {
      gd->writeDocumentation(*g_outputList);
    }
  }
}

//----------------------------------------------------------------------------

//static void generatePackageDocs()
//{
//  writePackageIndex(*g_outputList);
//  
//  if (Doxygen::packageDict.count()>0)
//  {
//    PackageSDict::Iterator pdi(Doxygen::packageDict);
//    PackageDef *pd;
//    for (pdi.toFirst();(pd=pdi.current());++pdi)
//    {
//      pd->writeDocumentation(*g_outputList);
//    }
//  }
//}

//----------------------------------------------------------------------------
// generate module pages

static void generateNamespaceDocs()
{
  //writeNamespaceIndex(*g_outputList);
  
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  // for each namespace...
  for (;(nd=nli.current());++nli)
  {

    if (nd->isLinkableInProject())
    {
      msg("Generating docs for namespace %s\n",nd->name().data());
      nd->writeDocumentation(*g_outputList);
    }

    // for each class in the namespace...
    ClassSDict::Iterator cli(*nd->getClassSDict());
    for ( ; cli.current() ; ++cli )
    {
      ClassDef *cd=cli.current();
      if ( ( cd->isLinkableInProject() && 
             cd->templateMaster()==0
           ) // skip external references, anonymous compounds and 
             // template instances and nested classes
           && !cd->isHidden() && !cd->isEmbeddedInOuterScope()
         )
      {
        msg("Generating docs for compound %s...\n",cd->name().data());

        cd->writeDocumentation(*g_outputList);
        cd->writeMemberList(*g_outputList);
      }
      cd->writeDocumentationForInnerClasses(*g_outputList);
    }
  }
}

#if defined(_WIN32)
static QCString fixSlashes(QCString &s)
{
  QCString result;
  uint i;
  for (i=0;i<s.length();i++)
  {
    switch(s.at(i))
    {
      case '/': 
      case '\\': 
        result+="\\\\"; 
        break;
      default:
        result+=s.at(i);
    }
  }
  return result;
}
#endif


//----------------------------------------------------------------------------

static bool openOutputFile(const char *outFile,QFile &f)
{
  bool fileOpened=FALSE;
  bool writeToStdout=(outFile[0]=='-' && outFile[1]=='\0');
  if (writeToStdout) // write to stdout
  {
    fileOpened = f.open(IO_WriteOnly,stdout);
  }
  else // write to file
  {
    QFileInfo fi(outFile);
    if (fi.exists()) // create a backup
    {
      QDir dir=fi.dir();
      QFileInfo backup(fi.fileName()+".bak");
      if (backup.exists()) // remove existing backup
        dir.remove(backup.fileName());
      dir.rename(fi.fileName(),fi.fileName()+".bak");
    } 
    f.setName(outFile);
    fileOpened = f.open(IO_WriteOnly|IO_Translate);
  }
  return fileOpened;
}

/*! Generate a template version of the configuration file.
 *  If the \a shortList parameter is TRUE a configuration file without
 *  comments will be generated.
 */
static void generateConfigFile(const char *configFile,bool shortList,
                               bool updateOnly=FALSE)
{
  QFile f;
  bool fileOpened=openOutputFile(configFile,f);
  bool writeToStdout=(configFile[0]=='-' && configFile[1]=='\0');
  if (fileOpened)
  {
    FTextStream t(&f);
    Config::instance()->writeTemplate(t,shortList,updateOnly);
    if (!writeToStdout)
    {
      if (!updateOnly)
      {
        msg("\n\nConfiguration file `%s' created.\n\n",configFile);
        msg("Now edit the configuration file and enter\n\n");
        if (strcmp(configFile,"Doxyfile") || strcmp(configFile,"doxyfile"))
          msg("  doxygen %s\n\n",configFile);
        else
          msg("  doxygen\n\n");
        msg("to generate the documentation for your project\n\n");
      }
      else
      {
        msg("\n\nConfiguration file `%s' updated.\n\n",configFile);
      }
    }
  }
  else
  {
    err("error: Cannot open file %s for writing\n",configFile);
    exit(1);
  }
}

//----------------------------------------------------------------------------
// read and parse a tag file

//static bool readLineFromFile(QFile &f,QCString &s)
//{
//  char c=0;
//  s.resize(0);
//  while (!f.atEnd() && (c=f.getch())!='\n') s+=c;
//  return f.atEnd();
//}

//----------------------------------------------------------------------------

static void readTagFile(Entry *root,const char *tl)
{
  QCString tagLine = tl;
  QCString fileName;
  QCString destName;
  int eqPos = tagLine.find('=');
  if (eqPos!=-1) // tag command contains a destination
  {
    fileName = tagLine.left(eqPos).stripWhiteSpace();
    destName = tagLine.right(tagLine.length()-eqPos-1).stripWhiteSpace();
    QFileInfo fi(fileName);
    Doxygen::tagDestinationDict.insert(fi.fileName().utf8(),new QCString(destName));
    //printf("insert tagDestination %s->%s\n",fi.fileName().data(),destName.data());
  }
  else
  {
    fileName = tagLine;
  }
    
  QFileInfo fi(fileName);
  if (!fi.exists() || !fi.isFile())
  {
    err("error: Tag file `%s' does not exist or is not a file. Skipping it...\n",
        fileName.data());
    return;
  }

  if (!destName.isEmpty())
    msg("Reading tag file `%s', location `%s'...\n",fileName.data(),destName.data());
  else
    msg("Reading tag file `%s'...\n",fileName.data());

  parseTagFile(root,fi.absFilePath().utf8(),fi.fileName().utf8());
}

//----------------------------------------------------------------------------
static void copyStyleSheet()
{
  QCString &htmlStyleSheet = Config_getString("HTML_STYLESHEET");
  if (!htmlStyleSheet.isEmpty())
  {
    QFileInfo fi(htmlStyleSheet);
    if (!fi.exists())
    {
      err("Style sheet '%s' specified by HTML_STYLESHEET does not exist!\n",htmlStyleSheet.data());
      htmlStyleSheet.resize(0); // revert to the default
    }
    else
    {
      QCString destFileName = Config_getString("HTML_OUTPUT")+"/"+fi.fileName().data();
      copyFile(htmlStyleSheet,destFileName);
    }
  }
  QCString &htmlExtraStyleSheet = Config_getString("HTML_EXTRA_STYLESHEET");
  if (!htmlExtraStyleSheet.isEmpty())
  {
    QFileInfo fi(htmlExtraStyleSheet);
    if (!fi.exists())
    {
      err("Style sheet '%s' specified by HTML_EXTRA_STYLESHEET does not exist!\n",htmlExtraStyleSheet.data());
      htmlExtraStyleSheet.resize(0); // revert to the default
    }
    else
    {
      QCString destFileName = Config_getString("HTML_OUTPUT")+"/"+fi.fileName().data();
      copyFile(htmlExtraStyleSheet,destFileName);
    }
  }
  
}

static void copyLogo()
{
  QCString &projectLogo = Config_getString("PROJECT_LOGO");
  if (!projectLogo.isEmpty())
  {
    QFileInfo fi(projectLogo);
    if (!fi.exists())
    {
      err("Project logo '%s' specified by PROJECT_LOGO does not exist!\n",projectLogo.data());
      projectLogo.resize(0); // revert to the default
    }
    else
    {
      QCString destFileName = Config_getString("HTML_OUTPUT")+"/"+fi.fileName().data();
      copyFile(projectLogo,destFileName);
      Doxygen::indexList.addImageFile(fi.fileName().data());
    }
  }
}

static void copyExtraFiles()
{
  QStrList files = Config_getList("HTML_EXTRA_FILES");
  uint i;
  for (i=0; i<files.count(); ++i)
  {
    QCString fileName(files.at(i));
    
    if (!fileName.isEmpty())
    {
      QFileInfo fi(fileName);
      if (!fi.exists()) 
      {
        err("Extra HTML file '%s' specified in HTML_EXTRA_FILES does not exist!\n", fileName.data());
      }
      else
      {
        QCString destFileName = Config_getString("HTML_OUTPUT")+"/"+fi.fileName().data();
        Doxygen::indexList.addImageFile(fi.fileName().data());
        copyFile(fileName, destFileName);
      }
    }
  }
}

//! parse the list of input files
static void parseFiles(Entry *root,EntryNav *rootNav)
{
  QCString *s=g_inputFiles.first();
  while (s)
  {
    QCString fileName=*s;
    QCString extension;
    int ei = fileName.findRev('.');
    if (ei!=-1) extension=fileName.right(fileName.length()-ei);
    ParserInterface *parser = Doxygen::parserManager->getParser(extension);

    QFileInfo fi(fileName);
    BufStr preBuf(fi.size()+4096);

    if (Config_getBool("ENABLE_PREPROCESSING") && 
        parser->needsPreprocessing(extension))
    {
      BufStr inBuf(fi.size()+4096);
      msg("Preprocessing %s...\n",s->data());
      readInputFile(fileName,inBuf);
      preprocessFile(fileName,inBuf,preBuf);
    }
    else // no preprocessing
    {
      msg("Reading %s...\n",s->data());
      readInputFile(fileName,preBuf);
    }

    BufStr convBuf(preBuf.curPos()+1024);

    // convert multi-line C++ comments to C style comments
    convertCppComments(&preBuf,&convBuf,fileName);

    convBuf.addChar('\0');

    // use language parse to parse the file
    parser->parseInput(fileName,convBuf.data(),root);

    // store the Entry tree in a file and create an index to
    // navigate/load entries
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::inputNameDict,fileName,ambig);
    ASSERT(fd!=0);
    //printf("root->createNavigationIndex for %s\n",fd->name().data());
    root->createNavigationIndex(rootNav,g_storage,fd);

    s=g_inputFiles.next();
  }
}

// resolves a path that may include symlinks, if a recursive symlink is
// found an empty string is returned.
static QCString resolveSymlink(QCString path)
{
  int sepPos=0;
  int oldPos=0;
  QFileInfo fi;
  QDict<void> nonSymlinks;
  QDict<void> known;
  QCString result = path;
  QCString oldPrefix = "/";
  do
  {
#ifdef WIN32
    // UNC path, skip server and share name
    if (sepPos==0 && (result.left(2)=="//" || result.left(2)=="\\\\")) 
      sepPos = result.find('/',2);
    if (sepPos!=-1) 
      sepPos = result.find('/',sepPos+1);
#else
    sepPos = result.find('/',sepPos+1);
#endif
    QCString prefix = sepPos==-1 ? result : result.left(sepPos);
    if (nonSymlinks.find(prefix)==0)
    {
      fi.setFile(prefix);
      if (fi.isSymLink())
      {
        QString target = fi.readLink();
        bool isRelative = QFileInfo(target).isRelative();
        if (isRelative)
        {
          target = QDir::cleanDirPath(oldPrefix+"/"+target.data());
        }
        if (sepPos!=-1)
        {
          if (fi.isDir() && target.length()>0 && target.at(target.length()-1)!='/')
          {
            target+='/';
          }
          target+=result.mid(sepPos);
        }
        result = QDir::cleanDirPath(target).data();
        sepPos = 0;
        if (known.find(result)) return QCString(); // recursive symlink!
        known.insert(result,(void*)0x8);
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
        nonSymlinks.insert(prefix,(void*)0x8);
        oldPrefix = prefix;
      }
      oldPos = sepPos;
    }
  }
  while (sepPos!=-1);
  return QDir::cleanDirPath(result).data();
}

static QDict<void> g_pathsVisited(1009);

//----------------------------------------------------------------------------
// Read all files matching at least one pattern in `patList' in the 
// directory represented by `fi'.
// The directory is read iff the recusiveFlag is set.
// The contents of all files is append to the input string

int readDir(QFileInfo *fi,
            FileNameList *fnList,
            FileNameDict *fnDict,
            StringDict  *exclDict,
            QStrList *patList,
            QStrList *exclPatList,
            StringList *resultList,
            StringDict *resultDict,
            bool errorIfNotExist,
            bool recursive,
            QDict<void> *killDict
           )
{
  QCString dirName = fi->absFilePath().utf8();
  if (fi->isSymLink())
  {
    dirName = resolveSymlink(dirName.data());
    if (dirName.isEmpty()) return 0;            // recusive symlink
    if (g_pathsVisited.find(dirName)) return 0; // already visited path
    g_pathsVisited.insert(dirName,(void*)0x8);
  }
  QDir dir(dirName);
  dir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );
  int totalSize=0;
  msg("Searching for files in directory %s\n", fi->absFilePath().data());
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  
  const QFileInfoList *list = dir.entryInfoList();
  if (list)
  {
    QFileInfoListIterator it( *list );
    QFileInfo *cfi;

    while ((cfi=it.current()))
    {
      if (exclDict==0 || exclDict->find(cfi->absFilePath().utf8())==0) 
      { // file should not be excluded
        //printf("killDict->find(%s)\n",cfi->absFilePath().data());
        if (!cfi->exists() || !cfi->isReadable())
        {
          if (errorIfNotExist)
          {
            err("warning: source %s is not a readable file or directory... skipping.\n",cfi->absFilePath().data());
          }
        }
        else if (cfi->isFile() && 
            (!Config_getBool("EXCLUDE_SYMLINKS") || !cfi->isSymLink()) &&
            (patList==0 || patternMatch(*cfi,patList)) && 
            !patternMatch(*cfi,exclPatList) &&
            (killDict==0 || killDict->find(cfi->absFilePath().utf8())==0)
            )
        {
          totalSize+=cfi->size()+cfi->absFilePath().length()+4;
          QCString name=convertToQCString(cfi->fileName());
          //printf("New file %s\n",name.data());
          if (fnDict)
          {
            FileDef  *fd=new FileDef(cfi->dirPath().utf8()+"/",name);
            FileName *fn=0;
            if (!name.isEmpty() && (fn=(*fnDict)[name]))
            {
              fn->append(fd);
            }
            else
            {
              fn = new FileName(cfi->absFilePath().utf8(),name);
              fn->append(fd);
              if (fnList) fnList->inSort(fn);
              fnDict->insert(name,fn);
            }
          }
          QCString *rs=0;
          if (resultList || resultDict)
          {
            rs=new QCString(cfi->absFilePath().utf8());
          }
          if (resultList) resultList->append(rs);
          if (resultDict) resultDict->insert(cfi->absFilePath().utf8(),rs);
          if (killDict) killDict->insert(cfi->absFilePath().utf8(),(void *)0x8);
        }
        else if (recursive &&
            (!Config_getBool("EXCLUDE_SYMLINKS") || !cfi->isSymLink()) &&
            cfi->isDir() && 
            !patternMatch(*cfi,exclPatList) &&
            cfi->fileName().at(0)!='.') // skip "." ".." and ".dir"
        {
          cfi->setFile(cfi->absFilePath());
          totalSize+=readDir(cfi,fnList,fnDict,exclDict,
              patList,exclPatList,resultList,resultDict,errorIfNotExist,
              recursive,killDict);
        }
      }
      ++it;
    }
  }
  return totalSize;
}


//----------------------------------------------------------------------------
// read a file or all files in a directory and append their contents to the
// input string. The names of the files are appended to the `fiList' list.

int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict,
                        bool recursive,
                        bool errorIfNotExist,
                        QDict<void> *killDict
                       )
{
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  // strip trailing slashes
  if (s==0) return 0;
  QCString fs = s;
  char lc = fs.at(fs.length()-1);
  if (lc=='/' || lc=='\\') fs = fs.left(fs.length()-1);

  QFileInfo fi(fs);
  //printf("readFileOrDirectory(%s)\n",s);
  int totalSize=0;
  {
    if (exclDict==0 || exclDict->find(fi.absFilePath().utf8())==0)
    {
      if (!fi.exists() || !fi.isReadable())
      {
        if (errorIfNotExist)
        {
          err("warning: source %s is not a readable file or directory... skipping.\n",s);
        }
      }
      else if (!Config_getBool("EXCLUDE_SYMLINKS") || !fi.isSymLink())
      {
        if (fi.isFile())
        {
          //printf("killDict->find(%s)\n",fi.absFilePath().data());
          if (killDict==0 || killDict->find(fi.absFilePath().utf8())==0)
          {
            totalSize+=fi.size()+fi.absFilePath().length()+4; //readFile(&fi,fiList,input); 
            //fiList->inSort(new FileInfo(fi));
            QCString name=convertToQCString(fi.fileName());
            //printf("New file %s\n",name.data());
            if (fnDict)
            {
              FileDef  *fd=new FileDef(fi.dirPath(TRUE).utf8()+"/",name);
              FileName *fn=0;
              if (!name.isEmpty() && (fn=(*fnDict)[name]))
              {
                fn->append(fd);
              }
              else
              {
                fn = new FileName(fi.absFilePath().utf8(),name);
                fn->append(fd);
                if (fnList) fnList->inSort(fn);
                fnDict->insert(name,fn);
              }
            }
            QCString *rs=0;
            if (resultList || resultDict)
            {
              rs=new QCString(fi.absFilePath().utf8());
              if (resultList) resultList->append(rs);
              if (resultDict) resultDict->insert(fi.absFilePath().utf8(),rs);
            }

            if (killDict) killDict->insert(fi.absFilePath().utf8(),(void *)0x8);
          }
        }
        else if (fi.isDir()) // readable dir
        {
          totalSize+=readDir(&fi,fnList,fnDict,exclDict,patList,
              exclPatList,resultList,resultDict,errorIfNotExist,
              recursive,killDict);
        }
      }
    }
  }
  return totalSize;
}

//----------------------------------------------------------------------------

void readFormulaRepository()
{
  QFile f(Config_getString("HTML_OUTPUT")+"/formula.repository");
  if (f.open(IO_ReadOnly)) // open repository
  {
    msg("Reading formula repository...\n");
    QTextStream t(&f);
    QCString line;
    while (!t.eof())
    {
      line=t.readLine().utf8();
      int se=line.find(':'); // find name and text separator.
      if (se==-1)
      {
        err("warning: formula.repository is corrupted!\n");
        break;
      }
      else
      {
        QCString formName = line.left(se);
        QCString formText = line.right(line.length()-se-1); 
        Formula *f=new Formula(formText);
        Doxygen::formulaList.append(f);
        Doxygen::formulaDict.insert(formText,f);
        Doxygen::formulaNameDict.insert(formName,f);
      }
    }
  }
}

//----------------------------------------------------------------------------

static void expandAliases()
{
  QDictIterator<QCString> adi(Doxygen::aliasDict);
  QCString *s;
  for (adi.toFirst();(s=adi.current());++adi)
  {
    *s = expandAlias(adi.currentKey(),*s);
  }
}

//----------------------------------------------------------------------------

static void escapeAliases()
{
  QDictIterator<QCString> adi(Doxygen::aliasDict);
  QCString *s;
  for (adi.toFirst();(s=adi.current());++adi)
  {
    QCString value=*s,newValue;
    int in,p=0;
    // for each \n in the alias command value
    while ((in=value.find("\\n",p))!=-1)
    {
      newValue+=value.mid(p,in-p);
      // expand \n's except if \n is part of a built-in command.
      if (value.mid(in,5)!="\\note" && 
          value.mid(in,5)!="\\name" && 
          value.mid(in,10)!="\\namespace" && 
          value.mid(in,14)!="\\nosubgrouping"
         ) 
      {
        newValue+="\\_linebr ";
      }
      else 
      {
        newValue+="\\n";
      }
      p=in+2;
    }
    newValue+=value.mid(p,value.length()-p);
    *s=newValue;
    //printf("Alias %s has value %s\n",adi.currentKey().data(),s->data());
  }
}

//----------------------------------------------------------------------------

void readAliases()
{ 
  // add aliases to a dictionary
  Doxygen::aliasDict.setAutoDelete(TRUE);
  QStrList &aliasList = Config_getList("ALIASES");
  const char *s=aliasList.first();
  while (s)
  {
    if (Doxygen::aliasDict[s]==0)
    {
      QCString alias=s;
      int i=alias.find('=');
      if (i>0)
      {
        QCString name=alias.left(i).stripWhiteSpace();
        QCString value=alias.right(alias.length()-i-1);
        //printf("Alias: found name=`%s' value=`%s'\n",name.data(),value.data()); 
        if (!name.isEmpty())
        {
          QCString *dn=Doxygen::aliasDict[name];
          if (dn==0) // insert new alias
          {
            Doxygen::aliasDict.insert(name,new QCString(value));
          }
          else // overwrite previous alias
          {
            *dn=value;
          }
        }
      }
    }
    s=aliasList.next();
  }
  expandAliases();
  escapeAliases();
}

//----------------------------------------------------------------------------

static void dumpSymbol(FTextStream &t,Definition *d)
{
  QCString anchor;
  if (d->definitionType()==Definition::TypeMember)
  {
    MemberDef *md = (MemberDef *)d;
    anchor=":"+md->anchor();
  }
  QCString scope;
  if (d->getOuterScope() && d->getOuterScope()!=Doxygen::globalScope) 
  {
    scope = d->getOuterScope()->getOutputFileBase()+Doxygen::htmlFileExtension;
  }
  t << "REPLACE INTO symbols (symbol_id,scope_id,name,file,line) VALUES('"
    << d->getOutputFileBase()+Doxygen::htmlFileExtension+anchor << "','"
    << scope << "','"
    << d->name() << "','"
    << d->getDefFileName() << "','"
    << d->getDefLine()
    << "');" << endl;
}

static void dumpSymbolMap()
{ 
  QFile f("symbols.sql");
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    QDictIterator<DefinitionIntf> di(*Doxygen::symbolMap);
    DefinitionIntf *intf;
    for (;(intf=di.current());++di)
    {
      if (intf->definitionType()==DefinitionIntf::TypeSymbolList) // list of symbols
      {
        DefinitionListIterator dli(*(DefinitionList*)intf);
        Definition *d;
        // for each symbol
        for (dli.toFirst();(d=dli.current());++dli)
        {
          dumpSymbol(t,d);
        }
      }
      else // single symbol
      {
        Definition *d = (Definition *)intf;
        if (d!=Doxygen::globalScope) dumpSymbol(t,d);
      }
    }
  }
}

//----------------------------------------------------------------------------

void dumpConfigAsXML()
{
  QFile f("config.xml");
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    Config::instance()->writeXML(t);
  }
}

//----------------------------------------------------------------------------
// print the usage of doxygen

static void usage(const char *name)
{
  msg("Doxygen version %s\nCopyright Dimitri van Heesch 1997-2012\n\n",versionString);
  msg("You can use doxygen in a number of ways:\n\n");
  msg("1) Use doxygen to generate a template configuration file:\n");
  msg("    %s [-s] -g [configName]\n\n",name);
  msg("    If - is used for configName doxygen will write to standard output.\n\n");
  msg("2) Use doxygen to update an old configuration file:\n");
  msg("    %s [-s] -u [configName]\n\n",name);
  msg("3) Use doxygen to generate documentation using an existing ");
  msg("configuration file:\n");
  msg("    %s [configName]\n\n",name);
  msg("    If - is used for configName doxygen will read from standard input.\n\n");
  msg("4) Use doxygen to generate a template file controlling the layout of the\n");
  msg("   generated documentation:\n");
  msg("    %s -l layoutFileName.xml\n\n",name);
  msg("5) Use doxygen to generate a template style sheet file for RTF, HTML or Latex.\n");
  msg("    RTF:        %s -w rtf styleSheetFile\n",name);
  msg("    HTML:       %s -w html headerFile footerFile styleSheetFile [configFile]\n",name);
  msg("    LaTeX:      %s -w latex headerFile footerFile styleSheetFile [configFile]\n\n",name);
  msg("6) Use doxygen to generate an rtf extensions file\n");
  msg("    RTF:   %s -e rtf extensionsFile\n\n",name);
  msg("If -s is specified the comments in the config file will be omitted.\n");
  msg("If configName is omitted `Doxyfile' will be used as a default.\n\n");
  exit(1);
}

//----------------------------------------------------------------------------
// read the argument of option `c' from the comment argument list and
// update the option index `optind'.

static const char *getArg(int argc,char **argv,int &optind)
{
  char *s=0;
  if (strlen(&argv[optind][2])>0)
    s=&argv[optind][2];
  else if (optind+1<argc && argv[optind+1][0]!='-')
    s=argv[++optind];
  return s;
}

//----------------------------------------------------------------------------

void initDoxygen()
{
  const char *lang = portable_getenv("LC_ALL");
  if (lang) portable_setenv("LANG",lang);
  setlocale(LC_ALL,"");
  setlocale(LC_CTYPE,"C"); // to get isspace(0xA0)==0, needed for UTF-8
  setlocale(LC_NUMERIC,"C");

  //Doxygen::symbolMap->setAutoDelete(TRUE);

  Doxygen::runningTime.start();
  initPreprocessor();

  Doxygen::parserManager = new ParserManager;
  Doxygen::parserManager->registerParser("c",       new CLanguageScanner, TRUE);
  Doxygen::parserManager->registerParser("python",  new PythonLanguageScanner);
  Doxygen::parserManager->registerParser("fortran", new FortranLanguageScanner);
  Doxygen::parserManager->registerParser("vhdl",    new VHDLLanguageScanner);
  Doxygen::parserManager->registerParser("dbusxml", new DBusXMLScanner);
  Doxygen::parserManager->registerParser("tcl",     new TclLanguageScanner);
  Doxygen::parserManager->registerParser("md",      new MarkdownFileParser);

  // register any additional parsers here...

  initDefaultExtensionMapping();
  initClassMemberIndices();
  initNamespaceMemberIndices();
  initFileMemberIndices();

  Doxygen::symbolMap     = new QDict<DefinitionIntf>(1000);
  Doxygen::inputNameList = new FileNameList;
  Doxygen::inputNameList->setAutoDelete(TRUE);
  Doxygen::memberNameSDict = new MemberNameSDict(10000);   
  Doxygen::memberNameSDict->setAutoDelete(TRUE);
  Doxygen::functionNameSDict = new MemberNameSDict(10000);   
  Doxygen::functionNameSDict->setAutoDelete(TRUE);
  Doxygen::groupSDict = new GroupSDict(17);          
  Doxygen::groupSDict->setAutoDelete(TRUE);
  Doxygen::globalScope = new NamespaceDef("<globalScope>",1,"<globalScope>");
  Doxygen::namespaceSDict = new NamespaceSDict(20);      
  Doxygen::namespaceSDict->setAutoDelete(TRUE);
  Doxygen::classSDict = new ClassSDict(1009);         
  Doxygen::classSDict->setAutoDelete(TRUE);
  Doxygen::hiddenClasses = new ClassSDict(257);
  Doxygen::hiddenClasses->setAutoDelete(TRUE);
  Doxygen::directories = new DirSDict(17);
  Doxygen::directories->setAutoDelete(TRUE);
  Doxygen::pageSDict = new PageSDict(1009);          // all doc pages
  Doxygen::pageSDict->setAutoDelete(TRUE);
  Doxygen::exampleSDict = new PageSDict(1009);       // all examples
  Doxygen::exampleSDict->setAutoDelete(TRUE);
  Doxygen::inputNameDict = new FileNameDict(10007);
  Doxygen::includeNameDict = new FileNameDict(10007);
  Doxygen::exampleNameDict = new FileNameDict(1009);
  Doxygen::exampleNameDict->setAutoDelete(TRUE);
  Doxygen::imageNameDict = new FileNameDict(257);
  Doxygen::dotFileNameDict = new FileNameDict(257);
  Doxygen::mscFileNameDict = new FileNameDict(257);
  Doxygen::sectionDict.setAutoDelete(TRUE);
  Doxygen::memGrpInfoDict.setAutoDelete(TRUE);
  Doxygen::tagDestinationDict.setAutoDelete(TRUE);
  Doxygen::dirRelations.setAutoDelete(TRUE);
  Doxygen::citeDict = new CiteDict(257);
}

void cleanUpDoxygen()
{
  delete Doxygen::inputNameDict;
  delete Doxygen::includeNameDict;
  delete Doxygen::exampleNameDict;
  delete Doxygen::imageNameDict;
  delete Doxygen::dotFileNameDict;
  delete Doxygen::mscFileNameDict;
  delete Doxygen::mainPage;
  delete Doxygen::pageSDict;  
  delete Doxygen::exampleSDict;
  delete Doxygen::globalScope;
  delete Doxygen::xrefLists;
  delete Doxygen::parserManager;
  cleanUpPreprocessor();
  delete theTranslator;
  delete g_outputList;
  Mappers::freeMappers();
  codeFreeScanner();

  if (Doxygen::symbolMap)
  {
    // iterate through Doxygen::symbolMap and delete all
    // DefinitionList objects, since they have no owner
    QDictIterator<DefinitionIntf> dli(*Doxygen::symbolMap);
    DefinitionIntf *di;
    for (dli.toFirst();(di=dli.current());)
    {
      if (di->definitionType()==DefinitionIntf::TypeSymbolList)
      {
        DefinitionIntf *tmp = Doxygen::symbolMap->take(dli.currentKey());
        delete (DefinitionList *)tmp;
      }
      else
      {
        ++dli;
      }
    } 
  }

  delete Doxygen::inputNameList;
  delete Doxygen::memberNameSDict;
  delete Doxygen::functionNameSDict;
  delete Doxygen::groupSDict;
  delete Doxygen::classSDict;
  delete Doxygen::hiddenClasses;
  delete Doxygen::namespaceSDict;
  delete Doxygen::directories;

  //delete Doxygen::symbolMap; <- we cannot do this unless all static lists 
  //                              (such as Doxygen::namespaceSDict)
  //                              with objects based on Definition are made
  //                              dynamic first
}

static int computeIdealCacheParam(uint v)
{
  //printf("computeIdealCacheParam(v=%u)\n",v);

  int r=0;
  while (v!=0) v>>=1,r++; 
  // r = log2(v)

  // convert to a valid cache size value
  return QMAX(0,QMIN(r-16,9));
}

void readConfiguration(int argc, char **argv)
{
  /**************************************************************************
   *             Handle arguments                                           *
   **************************************************************************/

  int optind=1;
  const char *configName=0;
  const char *layoutName=0;
  const char *debugLabel;
  const char *formatName;
  bool genConfig=FALSE;
  bool shortList=FALSE;
  bool updateConfig=FALSE;
  bool genLayout=FALSE;
  while (optind<argc && argv[optind][0]=='-' && 
               (isalpha(argv[optind][1]) || argv[optind][1]=='?' || 
                argv[optind][1]=='-')
        )
  {
    switch(argv[optind][1])
    {
      case 'g':
        genConfig=TRUE;
        configName=getArg(argc,argv,optind);
        if (strcmp(argv[optind+1],"-")==0)
        { configName="-"; optind++; }
        if (!configName) 
        { configName="Doxyfile"; }
        break;
      case 'l':
        genLayout=TRUE;
        layoutName=getArg(argc,argv,optind);
        if (!layoutName)
        { layoutName="DoxygenLayout.xml"; }
        break;
      case 'd':
        debugLabel=getArg(argc,argv,optind);
        Debug::setFlag(debugLabel);
        break;
      case 's':
        shortList=TRUE;
        break;
      case 'u':
        updateConfig=TRUE;
        break;
      case 'e':
        formatName=getArg(argc,argv,optind);
        if (!formatName)
        {
          err("error: option -e is missing format specifier rtf.\n");
          cleanUpDoxygen();
          exit(1);
        }
        if (stricmp(formatName,"rtf")==0)
        {
          if (optind+1>=argc)
          {
            err("error: option \"-e rtf\" is missing an extensions file name\n");
            cleanUpDoxygen();
            exit(1);
          }
          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            RTFGenerator::writeExtensionsFile(f);
          }
          cleanUpDoxygen();
          exit(1);
        }
        err("error: option \"-e\" has invalid format specifier.\n");
        cleanUpDoxygen();
        exit(1);
        break; 
      case 'w':
        formatName=getArg(argc,argv,optind);
        if (!formatName)
        {
          err("error: option -w is missing format specifier rtf, html or latex\n");
          cleanUpDoxygen();
          exit(1);
        } 
        if (stricmp(formatName,"rtf")==0)
        {
          if (optind+1>=argc)
          {
            err("error: option \"-w rtf\" is missing a style sheet file name\n");
            cleanUpDoxygen();
            exit(1);
          }
          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            RTFGenerator::writeStyleSheetFile(f);
          }
          cleanUpDoxygen();
          exit(1);
        }
        else if (stricmp(formatName,"html")==0)
        {
          if (optind+4<argc || QFileInfo("Doxyfile").exists())
          {
            QCString df = optind+4<argc ? argv[optind+4] : QCString("Doxyfile");
            if (!Config::instance()->parse(df))
            {
              err("error opening or reading configuration file %s!\n",argv[optind+4]);
              cleanUpDoxygen();
              exit(1);
            }
            Config::instance()->substituteEnvironmentVars();
            Config::instance()->convertStrToVal();
            // avoid bootstrapping issues when the config file already
            // refers to the files that we are supposed to parse.
            Config_getString("HTML_HEADER")="";
            Config_getString("HTML_FOOTER")="";
            Config::instance()->check();
          }
          else
          {
            Config::instance()->init();
          }
          if (optind+3>=argc)
          {
            err("error: option \"-w html\" does not have enough arguments\n");
            cleanUpDoxygen();
            exit(1);
          }

          QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
          if (!setTranslator(outputLanguage))
          {
            err("warning: Output language %s not supported! Using English instead.\n", outputLanguage.data());
          }

          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            HtmlGenerator::writeHeaderFile(f, argv[optind+3]);
          }
          f.close();
          if (openOutputFile(argv[optind+2],f))
          {
            HtmlGenerator::writeFooterFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+3],f))
          {
            HtmlGenerator::writeStyleSheetFile(f);
          } 
          cleanUpDoxygen();
          exit(0);
        }
        else if (stricmp(formatName,"latex")==0)
        {
          if (optind+4<argc) // use config file to get settings
          {
            if (!Config::instance()->parse(argv[optind+4]))
            {
              err("error opening or reading configuration file %s!\n",argv[optind+4]);
              exit(1);
            }
            Config::instance()->substituteEnvironmentVars();
            Config::instance()->convertStrToVal();
            Config_getString("LATEX_HEADER")="";
            Config::instance()->check();
          }
          else // use default config
          {
            Config::instance()->init();
          }
          if (optind+3>=argc)
          {
            err("error: option \"-w latex\" does not have enough arguments\n");
            cleanUpDoxygen();
            exit(1);
          }

          QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
          if (!setTranslator(outputLanguage))
          {
            err("warning: Output language %s not supported! Using English instead.\n", outputLanguage.data());
          }

          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            LatexGenerator::writeHeaderFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+2],f))
          {
            LatexGenerator::writeFooterFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+3],f))
          {
            LatexGenerator::writeStyleSheetFile(f);
          }
          cleanUpDoxygen();
          exit(0);
        }
        else
        {
          err("error: Illegal format specifier %s: should be one of rtf, html, latex, or bst\n",formatName);
          cleanUpDoxygen();
          exit(1);
        }
        break;
      case 'm':
        g_dumpSymbolMap = TRUE;
        break;
      case 'x':
        g_dumpConfigAsXML = TRUE;
        break;
      case '-':
        if (strcmp(&argv[optind][2],"help")==0)
        {
          usage(argv[0]);
        }
        else if (strcmp(&argv[optind][2],"version")==0)
        {
          msg("%s\n",versionString); 
          cleanUpDoxygen();
          exit(0);
        }
        break;
      case 'b':
        setvbuf(stdout,NULL,_IONBF,0);
        Doxygen::outputToWizard=TRUE;
        break;
      case 'h':
      case '?':
        usage(argv[0]);
        break;
      default:
        err("Unknown option -%c\n",argv[optind][1]);
        usage(argv[0]);
    }
    optind++;
  }
  
  /**************************************************************************
   *            Parse or generate the config file                           *
   **************************************************************************/

  Config::instance()->init();

  if (genConfig)
  {
    if (g_dumpConfigAsXML)
    {
      checkConfiguration();
      generateConfigFile(configName,shortList);
      dumpConfigAsXML();
      exit(0); 
    }
    else
    {
      generateConfigFile(configName,shortList);
    }
    cleanUpDoxygen();
    exit(0);
  }
  if (genLayout)
  {
    writeDefaultLayoutFile(layoutName);
    cleanUpDoxygen();
    exit(0);
  }

  QFileInfo configFileInfo1("Doxyfile"),configFileInfo2("doxyfile");
  if (optind>=argc)
  { 
    if (configFileInfo1.exists()) 
    {
      configName="Doxyfile";
    }
    else if (configFileInfo2.exists())
    {
      configName="doxyfile";
    }
    else
    {
      err("Doxyfile not found and no input file specified!\n");
      usage(argv[0]);
    }
  }
  else
  {
    QFileInfo fi(argv[optind]);
    if (fi.exists() || strcmp(argv[optind],"-")==0)
    {
      configName=argv[optind];
    }
    else
    {
      err("error: configuration file %s not found!\n",argv[optind]);
      usage(argv[0]);
    }
  }


  if (!Config::instance()->parse(configName))
  {
    err("error: could not open or read configuration file %s!\n",configName);
    cleanUpDoxygen();
    exit(1);
  }

  if (updateConfig)
  {
    generateConfigFile(configName,shortList,TRUE);
    cleanUpDoxygen();
    exit(0);
  }

  /* Perlmod wants to know the path to the config file.*/
  QFileInfo configFileInfo(configName);
  setPerlModDoxyfile(configFileInfo.absFilePath().data());

}

/** check and resolve config options */
void checkConfiguration()
{
  
  Config::instance()->substituteEnvironmentVars();
  Config::instance()->convertStrToVal();
  Config::instance()->check();
  
  initWarningFormat();
}

/** adjust globals that depend on configuration settings. */
void adjustConfiguration()
{
  QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
  if (!setTranslator(outputLanguage))
  {
    err("warning: Output language %s not supported! Using English instead.\n",
       outputLanguage.data());
  }
  QStrList &includePath = Config_getList("INCLUDE_PATH");
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    addSearchDir(fi.absFilePath().utf8());
    s=includePath.next();
  }

  /* Set the global html file extension. */ 
  Doxygen::htmlFileExtension = Config_getString("HTML_FILE_EXTENSION");


  Doxygen::xrefLists->setAutoDelete(TRUE);

  Doxygen::parseSourcesNeeded = Config_getBool("CALL_GRAPH") || 
                                Config_getBool("CALLER_GRAPH") ||
                                Config_getBool("REFERENCES_RELATION") ||
                                Config_getBool("REFERENCED_BY_RELATION");

  Doxygen::markdownSupport = Config_getBool("MARKDOWN_SUPPORT");
  
  /**************************************************************************
   *            Add custom extension mappings
   **************************************************************************/

  QStrList &extMaps = Config_getList("EXTENSION_MAPPING");
  char *mapping = extMaps.first();
  while (mapping)
  {
    QCString mapStr = mapping;
    int i;
    if ((i=mapStr.find('='))!=-1)
    {
      QCString ext=mapStr.left(i).stripWhiteSpace().lower();
      QCString language=mapStr.mid(i+1).stripWhiteSpace().lower();
      if (!updateLanguageMapping(ext,language))
      {
        err("Failed to map file extension '%s' to unsupported language '%s'.\n"
            "Check the EXTENSION_MAPPING setting in the config file.\n", 
            ext.data(),language.data());
      }
      else
      {
        msg("Adding custom extension mapping: .%s will be treated as language %s\n",
            ext.data(),language.data());
      }
    }
    mapping = extMaps.next();
  }


  // add predefined macro name to a dictionary
  QStrList &expandAsDefinedList =Config_getList("EXPAND_AS_DEFINED");
  s=expandAsDefinedList.first();
  while (s)
  {
    if (Doxygen::expandAsDefinedDict[s]==0)
    {
      Doxygen::expandAsDefinedDict.insert(s,(void *)666);
    }
    s=expandAsDefinedList.next();
  }

  // read aliases and store them in a dictionary
  readAliases();

  // store number of spaces in a tab into Doxygen::spaces
  int &tabSize = Config_getInt("TAB_SIZE");
  Doxygen::spaces.resize(tabSize+1);
  int sp;for (sp=0;sp<tabSize;sp++) Doxygen::spaces.at(sp)=' ';
  Doxygen::spaces.at(tabSize)='\0';
}

#ifdef HAS_SIGNALS
static void stopDoxygen(int)
{
  QDir thisDir;
  msg("Cleaning up...\n");
  if (!Doxygen::entryDBFileName.isEmpty())
  {
    thisDir.remove(Doxygen::entryDBFileName);
  }
  if (!Doxygen::objDBFileName.isEmpty())
  {
    thisDir.remove(Doxygen::objDBFileName);
  }
  exit(1);
}
#endif

static void exitDoxygen()
{
  if (!g_successfulRun)  // premature exit
  {
    QDir thisDir;
    msg("Exiting...\n");
    if (!Doxygen::entryDBFileName.isEmpty())
    {
      thisDir.remove(Doxygen::entryDBFileName);
    }
    if (!Doxygen::objDBFileName.isEmpty())
    {
      thisDir.remove(Doxygen::objDBFileName);
    }
  }
}

static QCString createOutputDirectory(const QCString &baseDirName,
                                  const char *formatDirOption,
                                  const char *defaultDirName)
{
  // Note the & on the next line, we modify the formatDirOption!
  QCString &formatDirName = Config_getString(formatDirOption);
  if (formatDirName.isEmpty())
  {
    formatDirName = baseDirName + defaultDirName;
  }
  else if (formatDirName[0]!='/' && (formatDirName.length()==1 || formatDirName[1]!=':'))
  {
    formatDirName.prepend(baseDirName+'/');
  }
  QDir formatDir(formatDirName);
  if (!formatDir.exists() && !formatDir.mkdir(formatDirName))
  {
    err("Could not create output directory %s\n", formatDirName.data());
    cleanUpDoxygen();
    exit(1);
  }
  return formatDirName;
}

static QCString getQchFileName()
{
  QCString const & qchFile = Config_getString("QCH_FILE");
  if (!qchFile.isEmpty())
  {
    return qchFile;
  }

  QCString const & projectName = Config_getString("PROJECT_NAME");
  QCString const & versionText = Config_getString("PROJECT_NUMBER");

  return QCString("../qch/")
      + (projectName.isEmpty() ? QCString("index") : projectName)
      + (versionText.isEmpty() ? QCString("") : QCString("-") + versionText)
      + QCString(".qch");
}

void searchInputFiles(StringList &inputFiles)
{
  QStrList &exclPatterns = Config_getList("EXCLUDE_PATTERNS");
  bool alwaysRecursive = Config_getBool("RECURSIVE");
  StringDict excludeNameDict(1009);
  excludeNameDict.setAutoDelete(TRUE);

  // gather names of all files in the include path
  msg("Searching for include files...\n");
  QStrList &includePathList = Config_getList("INCLUDE_PATH");
  char *s=includePathList.first();
  while (s)
  {
    QStrList &pl = Config_getList("INCLUDE_FILE_PATTERNS");
    if (pl.count()==0) 
    {
      pl = Config_getList("FILE_PATTERNS");
    }
    readFileOrDirectory(s,0,Doxygen::includeNameDict,0,&pl,
                        &exclPatterns,0,0,
                        alwaysRecursive);
    s=includePathList.next(); 
  }
  
  msg("Searching for example files...\n");
  QStrList &examplePathList = Config_getList("EXAMPLE_PATH");
  s=examplePathList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::exampleNameDict,0,
                        &Config_getList("EXAMPLE_PATTERNS"),
                        0,0,0,
                        (alwaysRecursive || Config_getBool("EXAMPLE_RECURSIVE")));
    s=examplePathList.next(); 
  }

  msg("Searching for images...\n");
  QStrList &imagePathList=Config_getList("IMAGE_PATH");
  s=imagePathList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::imageNameDict,0,0,
                        0,0,0,
                        alwaysRecursive);
    s=imagePathList.next(); 
  }

  msg("Searching for dot files...\n");
  QStrList &dotFileList=Config_getList("DOTFILE_DIRS");
  s=dotFileList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::dotFileNameDict,0,0,
                        0,0,0,
                        alwaysRecursive);
    s=dotFileList.next(); 
  }

  msg("Searching for msc files...\n");
  QStrList &mscFileList=Config_getList("MSCFILE_DIRS");
  s=mscFileList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::mscFileNameDict,0,0,
                        0,0,0,
                        alwaysRecursive);
    s=mscFileList.next(); 
  }


  msg("Searching for files to exclude\n");
  QStrList &excludeList = Config_getList("EXCLUDE");
  s=excludeList.first();
  while (s)
  {
    readFileOrDirectory(s,0,0,0,&Config_getList("FILE_PATTERNS"),
                        0,0,&excludeNameDict,
                        alwaysRecursive,
                        FALSE);
    s=excludeList.next();
  }

  /**************************************************************************
   *             Determine Input Files                                      *
   **************************************************************************/

  msg("Searching for files to process...\n");
  QDict<void> *killDict = new QDict<void>(10007);
  int inputSize=0;
  QStrList &inputList=Config_getList("INPUT");
  inputFiles.setAutoDelete(TRUE);
  s=inputList.first();
  while (s)
  {
    QCString path=s;
    uint l = path.length();
    // strip trailing slashes
    if (path.at(l-1)=='\\' || path.at(l-1)=='/') path=path.left(l-1);

    inputSize+=readFileOrDirectory(
        path,
        Doxygen::inputNameList,
        Doxygen::inputNameDict,
        &excludeNameDict,
        &Config_getList("FILE_PATTERNS"),
        &exclPatterns,
        &inputFiles,0,
        alwaysRecursive,
        TRUE,
        killDict);
    s=inputList.next();
  }
  delete killDict;
}

  
void parseInput()
{
  atexit(exitDoxygen);


  /**************************************************************************
   *            Make sure the output directory exists
   **************************************************************************/
  QCString &outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty()) 
  {
    outputDirectory=QDir::currentDirPath().utf8();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
        err("error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",outputDirectory.data());
        cleanUpDoxygen();
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
	err("Notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath().utf8();
  }

  /**************************************************************************
   *            Initialize global lists and dictionaries
   **************************************************************************/

  int cacheSize = Config_getInt("SYMBOL_CACHE_SIZE");
  if (cacheSize<0) cacheSize=0;
  if (cacheSize>9) cacheSize=9;
  Doxygen::symbolCache   = new ObjCache(16+cacheSize); // 16 -> room for 65536 elements, 
                                                       //       ~2.0 MByte "overhead"
  //Doxygen::symbolCache   = new ObjCache(1);  // only to stress test cache behaviour
  Doxygen::symbolStorage = new Store;

  // also scale lookup cache with SYMBOL_CACHE_SIZE
  cacheSize = Config_getInt("LOOKUP_CACHE_SIZE");
  if (cacheSize<0) cacheSize=0;
  if (cacheSize>9) cacheSize=9;
  uint lookupSize = 65536 << cacheSize;
  Doxygen::lookupCache = new QCache<LookupInfo>(lookupSize,lookupSize);
  Doxygen::lookupCache->setAutoDelete(TRUE);

#ifdef HAS_SIGNALS
  signal(SIGINT, stopDoxygen);
#endif

  uint pid = portable_pid();
  Doxygen::objDBFileName.sprintf("doxygen_objdb_%d.tmp",pid);
  Doxygen::objDBFileName.prepend(outputDirectory+"/");
  Doxygen::entryDBFileName.sprintf("doxygen_entrydb_%d.tmp",pid);
  Doxygen::entryDBFileName.prepend(outputDirectory+"/");
  
  if (Doxygen::symbolStorage->open(Doxygen::objDBFileName)==-1)
  {
    err("Failed to open temporary file %s\n",Doxygen::objDBFileName.data());
    exit(1);
  }


  /**************************************************************************
   *            Initialize some global constants
   **************************************************************************/
  
  g_compoundKeywordDict.insert("template class",(void *)8);
  g_compoundKeywordDict.insert("template struct",(void *)8);
  g_compoundKeywordDict.insert("class",(void *)8);
  g_compoundKeywordDict.insert("struct",(void *)8);
  g_compoundKeywordDict.insert("union",(void *)8);
  g_compoundKeywordDict.insert("interface",(void *)8);
  g_compoundKeywordDict.insert("exception",(void *)8);


  /**************************************************************************
   *            Check/create output directorties                            *
   **************************************************************************/

  QCString htmlOutput;
  bool &generateHtml = Config_getBool("GENERATE_HTML");
  if (generateHtml)
    htmlOutput = createOutputDirectory(outputDirectory,"HTML_OUTPUT","/html");

  QCString xmlOutput;
  bool &generateXml = Config_getBool("GENERATE_XML");
  if (generateXml)
    xmlOutput = createOutputDirectory(outputDirectory,"XML_OUTPUT","/xml");
    
  QCString latexOutput;
  bool &generateLatex = Config_getBool("GENERATE_LATEX");
  if (generateLatex)
    latexOutput = createOutputDirectory(outputDirectory,"LATEX_OUTPUT","/latex");

  QCString rtfOutput;
  bool &generateRtf = Config_getBool("GENERATE_RTF");
  if (generateRtf)
    rtfOutput = createOutputDirectory(outputDirectory,"RTF_OUTPUT","/rtf");

  QCString manOutput;
  bool &generateMan = Config_getBool("GENERATE_MAN");
  if (generateMan)
    manOutput = createOutputDirectory(outputDirectory,"MAN_OUTPUT","/man");


  if (Config_getBool("HAVE_DOT"))
  {
    QCString curFontPath = Config_getString("DOT_FONTPATH");
    if (curFontPath.isEmpty())
    {
      portable_getenv("DOTFONTPATH");
      QCString newFontPath = ".";
      if (!curFontPath.isEmpty())
      {
        newFontPath+=portable_pathListSeparator();
        newFontPath+=curFontPath;
      }
      portable_setenv("DOTFONTPATH",newFontPath);
    }
    else
    {
      portable_setenv("DOTFONTPATH",curFontPath);
    }
  }



  /**************************************************************************
   *             Handle layout file                                         *
   **************************************************************************/

  LayoutDocManager::instance().init();
  QCString &layoutFileName = Config_getString("LAYOUT_FILE");
  bool defaultLayoutUsed = FALSE;
  if (layoutFileName.isEmpty())
  {
    layoutFileName = "DoxygenLayout.xml";
    defaultLayoutUsed = TRUE;
  }

  QFile layoutFile(layoutFileName);
  if (layoutFile.open(IO_ReadOnly))
  {
    msg("Parsing layout file %s...\n",layoutFileName.data());
    QTextStream t(&layoutFile); 
    t.setEncoding(QTextStream::Latin1);
    LayoutDocManager::instance().parse(t,layoutFileName);
  }
  else if (!defaultLayoutUsed)
  {
    err("warning: failed to open layout file '%s' for reading!\n",layoutFileName.data());
  }

  /**************************************************************************
   *             Read and preprocess input                                  *
   **************************************************************************/
 
  // prevent search in the output directories
  QStrList &exclPatterns = Config_getList("EXCLUDE_PATTERNS");
  if (generateHtml)  exclPatterns.append(htmlOutput);
  if (generateXml)   exclPatterns.append(xmlOutput);
  if (generateLatex) exclPatterns.append(latexOutput);
  if (generateRtf)   exclPatterns.append(rtfOutput);
  if (generateMan)   exclPatterns.append(manOutput);


  searchInputFiles(g_inputFiles);

  // Notice: the order of the function calls below is very important!
  
  if (Config_getBool("GENERATE_HTML"))
  {
    readFormulaRepository();
  }
  
  /**************************************************************************
   *             Handle Tag Files                                           *
   **************************************************************************/

  g_storage = new FileStorage;
  g_storage->setName(Doxygen::entryDBFileName);
  if (!g_storage->open(IO_WriteOnly))
  {
    err("Failed to create temporary storage file %s\n",
        Doxygen::entryDBFileName.data());
    exit(1);
  }
  Entry *root=new Entry;
  EntryNav *rootNav = new EntryNav(0,root);
  rootNav->setEntry(root);
  msg("Reading and parsing tag files\n");
  
  QStrList &tagFileList = Config_getList("TAGFILES");
  char *s=tagFileList.first();
  while (s)
  {
    readTagFile(root,s);
    root->createNavigationIndex(rootNav,g_storage,0);
    s=tagFileList.next();
  }
  
  /**************************************************************************
   *             Parse source files                                         * 
   **************************************************************************/

  if (Config_getBool("BUILTIN_STL_SUPPORT"))
  {
    addSTLClasses(rootNav);
  }

  parseFiles(root,rootNav);
  g_storage->close();

  // we are done with input scanning now, so free up the buffers used by flex
  // (can be around 4MB)
  preFreeScanner();
  scanFreeScanner();
  pyscanFreeScanner();

  if (!g_storage->open(IO_ReadOnly))
  {
    err("Failed to open temporary storage file %s for reading",
        Doxygen::entryDBFileName.data());
    exit(1);
  }

  /**************************************************************************
   *             Gather information                                         * 
   **************************************************************************/
  
  msg("Building group list...\n");
  buildGroupList(rootNav);
  organizeSubGroups(rootNav);

  msg("Building directory list...\n");
  buildDirectories();
  findDirDocumentation(rootNav);

  msg("Building namespace list...\n");
  buildNamespaceList(rootNav);
  findUsingDirectives(rootNav);

  msg("Building file list...\n");
  buildFileList(rootNav);
  //generateFileTree();
  
  msg("Building class list...\n");
  buildClassList(rootNav);

  msg("Associating documentation with classes...\n");
  buildClassDocList(rootNav);

  // build list of using declarations here (global list)
  buildListOfUsingDecls(rootNav);
  
  msg("Computing nesting relations for classes...\n");
  resolveClassNestingRelations();
  distributeClassGroupRelations();

  // calling buildClassList may result in cached relations that
  // become invalid after resolveClassNestingRelations(), that's why
  // we need to clear the cache here
  Doxygen::lookupCache->clear();
  // we don't need the list of using declaration anymore
  g_usingDeclarations.clear();

  msg("Building example list...\n");
  buildExampleList(rootNav);
  
  msg("Searching for enumerations...\n");
  findEnums(rootNav);
  
  // Since buildVarList calls isVarWithConstructor
  // and this calls getResolvedClass we need to process
  // typedefs first so the relations between classes via typedefs
  // are properly resolved. See bug 536385 for an example.
  msg("Searching for documented typedefs...\n");
  buildTypedefList(rootNav);

  msg("Searching for members imported via using declarations...\n");
  findUsingDeclImports(rootNav);
  // this should be after buildTypedefList in order to properly import
  // used typedefs
  findUsingDeclarations(rootNav);

  msg("Searching for included using directives...\n");
  findIncludedUsingDirectives();

  msg("Searching for documented variables...\n");
  buildVarList(rootNav);

  msg("Building member list...\n"); // using class info only !
  buildFunctionList(rootNav);

  msg("Searching for friends...\n");
  findFriends();
  
  msg("Searching for documented defines...\n");
  findDefineDocumentation(rootNav); 
  
  findClassEntries(rootNav);         
  msg("Computing class inheritance relations...\n");
  findInheritedTemplateInstances();       
  msg("Computing class usage relations...\n");
  findUsedTemplateInstances();       
  if (Config_getBool("INLINE_SIMPLE_STRUCTS"))
  {
    msg("Searching for tag less structs...\n");
    findTagLessClasses();
  }

  msg("Flushing cached template relations that have become invalid...\n");
  flushCachedTemplateRelations();
  
  msg("Creating members for template instances...\n");
  createTemplateInstanceMembers();

  msg("Computing class relations...\n");
  computeTemplateClassRelations(); 
  flushUnresolvedRelations();
  if (Config_getBool("OPTIMIZE_OUTPUT_VHDL"))
  {
    VhdlDocGen::computeVhdlComponentRelations(); 
  }
  computeClassRelations();        
  g_classEntries.clear();          

  msg("Add enum values to enums...\n");
  addEnumValuesToEnums(rootNav);
  findEnumDocumentation(rootNav);

  msg("Searching for member function documentation...\n");
  findObjCMethodDefinitions(rootNav);
  findMemberDocumentation(rootNav); // may introduce new members !

  transferRelatedFunctionDocumentation();
  transferFunctionDocumentation();
  
  msg("Building page list...\n");
  buildPageList(rootNav);

  msg("Search for main page...\n");
  findMainPage(rootNav);

  msg("Computing page relations...\n");
  computePageRelations(rootNav);
  checkPageRelations();

  msg("Determining the scope of groups...\n");
  findGroupScope(rootNav);

  msg("Sorting lists...\n");
  Doxygen::memberNameSDict->sort();
  Doxygen::functionNameSDict->sort();
  Doxygen::hiddenClasses->sort();
  Doxygen::classSDict->sort();
  
  msg("Freeing entry tree\n");
  delete rootNav;
  g_storage->close();
  delete g_storage;
  g_storage=0;

  QDir thisDir;
  thisDir.remove(Doxygen::entryDBFileName);
  
  msg("Determining which enums are documented\n");
  findDocumentedEnumValues();

  msg("Computing member relations...\n");
  mergeCategories();
  computeMemberRelations();

  msg("Building full member lists recursively...\n");
  buildCompleteMemberLists();
  
  msg("Adding members to member groups.\n");
  addMembersToMemberGroup();

  if (Config_getBool("DISTRIBUTE_GROUP_DOC"))
  {
    msg("Distributing member group documentation.\n");
    distributeMemberGroupDocumentation();
  }
  
  msg("Computing member references...\n");
  computeMemberReferences(); 

  if (Config_getBool("INHERIT_DOCS"))
  {
    msg("Inheriting documentation...\n");
    inheritDocumentation();
  }

  // compute the shortest possible names of all files
  // without losing the uniqueness of the file names.
  msg("Generating disk names...\n");
  Doxygen::inputNameList->generateDiskNames();
  
  msg("Adding source references...\n");
  addSourceReferences();

  msg("Adding xrefitems...\n");
  addListReferences();
  generateXRefPages();

  msg("Sorting member lists...\n");
  sortMemberLists();

  if (Config_getBool("DIRECTORY_GRAPH"))
  {
    msg("Computing dependencies between directories...\n");
    computeDirDependencies();
  }

  //msg("Resolving citations...\n");
  //Doxygen::citeDict->resolve();

  msg("Generating citations page...\n");
  Doxygen::citeDict->generatePage();

  msg("Counting data structures...\n");
  countDataStructures();
 
  msg("Resolving user defined references...\n");
  resolveUserReferences();

  msg("Finding anchors and sections in the documentation...\n");
  findSectionsInDocumentation();

  transferFunctionReferences();

  msg("Combining using relations...\n");
  combineUsingRelations();

  msg("Adding members to index pages...\n");
  addMembersToIndex();

  if (Config_getBool("OPTIMIZE_OUTPUT_VHDL") && 
      Config_getBool("HAVE_DOT") &&
      Config_getEnum("DOT_IMAGE_FORMAT")=="svg")
  {
    VhdlDocGen::writeOverview();
  }
}

void generateOutput()
{
  /**************************************************************************
   *            Initialize output generators                                *
   **************************************************************************/

  //// dump all symbols
  if (g_dumpSymbolMap)
  {
    dumpSymbolMap();
    exit(0);
  }

  initSearchIndexer();

  bool generateHtml  = Config_getBool("GENERATE_HTML");
  bool generateLatex = Config_getBool("GENERATE_LATEX");
  bool generateMan   = Config_getBool("GENERATE_MAN");
  bool generateRtf   = Config_getBool("GENERATE_RTF");


  g_outputList = new OutputList(TRUE);
  if (generateHtml)  
  {
    g_outputList->add(new HtmlGenerator);
    HtmlGenerator::init();

    // add HTML indexers that are enabled
    bool generateHtmlHelp    = Config_getBool("GENERATE_HTMLHELP");
    bool generateEclipseHelp = Config_getBool("GENERATE_ECLIPSEHELP");
    bool generateQhp         = Config_getBool("GENERATE_QHP");
    bool generateTreeView    = Config_getBool("GENERATE_TREEVIEW");
    bool generateDocSet      = Config_getBool("GENERATE_DOCSET");
    if (generateEclipseHelp) Doxygen::indexList.addIndex(new EclipseHelp);
    if (generateHtmlHelp)    Doxygen::indexList.addIndex(new HtmlHelp);
    if (generateQhp)         Doxygen::indexList.addIndex(new Qhp);
    if (generateTreeView)    Doxygen::indexList.addIndex(new FTVHelp(TRUE));
    if (generateDocSet)      Doxygen::indexList.addIndex(new DocSets);
    Doxygen::indexList.initialize();
    HtmlGenerator::writeTabData();

    // copy static stuff
    copyStyleSheet();
    copyLogo();
    copyExtraFiles();
    FTVHelp::generateTreeViewImages();
  }
  if (generateLatex) 
  {
    g_outputList->add(new LatexGenerator);
    LatexGenerator::init();
  }
  if (generateMan)
  {
    g_outputList->add(new ManGenerator);
    ManGenerator::init();
  }
  if (generateRtf)
  {
    g_outputList->add(new RTFGenerator);
    RTFGenerator::init();
  }

  if (Config_getBool("USE_HTAGS"))
  {
    Htags::useHtags = TRUE;
    QCString htmldir = Config_getString("HTML_OUTPUT");
    if (!Htags::execute(htmldir))
       err("error: USE_HTAGS is YES but htags(1) failed. \n");
    if (!Htags::loadFilemap(htmldir))
       err("error: htags(1) ended normally but failed to load the filemap. \n");
  }
  
  /**************************************************************************
   *                        Generate documentation                          *
   **************************************************************************/

  QFile *tag=0;
  QCString &generateTagFile = Config_getString("GENERATE_TAGFILE");
  if (!generateTagFile.isEmpty())
  {
    tag=new QFile(generateTagFile);
    if (!tag->open(IO_WriteOnly))
    {
      err("error: cannot open tag file %s for writing\n",
          generateTagFile.data()
         );
      cleanUpDoxygen();
      exit(1);
    }
    Doxygen::tagFile.setDevice(tag);
    Doxygen::tagFile << "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>" << endl;
    Doxygen::tagFile << "<tagfile>" << endl;
  }

  if (generateHtml)  writeDoxFont(Config_getString("HTML_OUTPUT"));
  if (generateLatex) writeDoxFont(Config_getString("LATEX_OUTPUT"));
  if (generateRtf)   writeDoxFont(Config_getString("RTF_OUTPUT"));

  msg("Generating style sheet...\n");
  //printf("writing style info\n");
  QCString genString = 
        theTranslator->trGeneratedAt(dateToString(TRUE),Config_getString("PROJECT_NAME"));
  g_outputList->writeStyleInfo(0); // write first part
  g_outputList->disableAllBut(OutputGenerator::Latex);
  g_outputList->parseText(genString);
  g_outputList->writeStyleInfo(1); // write second part
  //parseText(*g_outputList,theTranslator->trWrittenBy());
  g_outputList->writeStyleInfo(2); // write third part
  g_outputList->parseText(genString);
  g_outputList->writeStyleInfo(3); // write fourth part
  //parseText(*g_outputList,theTranslator->trWrittenBy());
  g_outputList->writeStyleInfo(4); // write last part
  g_outputList->enableAll();
  
  static bool searchEngine      = Config_getBool("SEARCHENGINE");
  static bool serverBasedSearch = Config_getBool("SERVER_BASED_SEARCH");

  // generate search indices (need to do this before writing other HTML
  // pages as these contain a drop down menu with options depending on
  // what categories we find in this function.
  if (generateHtml && searchEngine)
  {
    msg("Generating search indices...\n");
    QCString searchDirName = Config_getString("HTML_OUTPUT")+"/search";
    QDir searchDir(searchDirName);
    if (!searchDir.exists() && !searchDir.mkdir(searchDirName))
    {
      err("error: Could not create search results directory '%s' $PWD='%s'\n",
          searchDirName.data(),QDir::currentDirPath().data());
      exit(1);
    }
    HtmlGenerator::writeSearchData(searchDirName);
    if (!serverBasedSearch) // client side search index
    {
      writeJavascriptSearchIndex();
    }
  }

  msg("Generating example documentation...\n");
  generateExampleDocs();

  msg("Generating file sources...\n");
  if (!Htags::useHtags)
  {
    generateFileSources();
  }

  msg("Generating file documentation...\n");
  generateFileDocs();
  
  msg("Generating page documentation...\n");
  generatePageDocs();
  
  msg("Generating group documentation...\n");
  generateGroupDocs();

  msg("Generating class documentation...\n");
  generateClassDocs();
  
  msg("Generating namespace index...\n");
  generateNamespaceDocs();
  
  if (Config_getBool("GENERATE_LEGEND"))
  {
    msg("Generating graph info page...\n");
    writeGraphInfo(*g_outputList);
  }

  msg("Generating directory documentation...\n");
  generateDirDocs(*g_outputList);

  if (Doxygen::formulaList.count()>0 && generateHtml
      && !Config_getBool("USE_MATHJAX"))
  {
    msg("Generating bitmaps for formulas in HTML...\n");
    Doxygen::formulaList.generateBitmaps(Config_getString("HTML_OUTPUT"));
  }
  
  writeMainPageTagFileData();

  if (g_outputList->count()>0)
  {
    writeIndexHierarchy(*g_outputList);
  }

  msg("finalizing index lists...\n");
  Doxygen::indexList.finalize();

  if (!generateTagFile.isEmpty())
  {
    Doxygen::tagFile << "</tagfile>" << endl;
    delete tag;
  }

  if (Config_getBool("DOT_CLEANUP"))
  {
    if (generateHtml)
      removeDoxFont(Config_getString("HTML_OUTPUT"));
    if (generateRtf)  
      removeDoxFont(Config_getString("RTF_OUTPUT"));
    if (generateLatex)  
      removeDoxFont(Config_getString("LATEX_OUTPUT"));
  }

  if (Config_getBool("GENERATE_XML"))
  {
    msg("Generating XML output...\n");
    Doxygen::generatingXmlOutput=TRUE;
    generateXML();
    Doxygen::generatingXmlOutput=FALSE;
  }
  if (Config_getBool("GENERATE_AUTOGEN_DEF"))
  {
    msg("Generating AutoGen DEF output...\n");
    generateDEF();
  }
  if (Config_getBool("GENERATE_PERLMOD"))
  {
    msg("Generating Perl module output...\n");
    generatePerlMod();
  }
  if (generateHtml && searchEngine && serverBasedSearch)
  {
    msg("Generating search index\n");
    if (Doxygen::searchIndex->kind()==SearchIndexIntf::Internal) // write own search index
    {
      HtmlGenerator::writeSearchPage();
      Doxygen::searchIndex->write(Config_getString("HTML_OUTPUT")+"/search/search.idx");
    }
    else // write data for external search index
    {
      Doxygen::searchIndex->write(Config_getString("OUTPUT_DIRECTORY")+"/searchdata.xml");
    }
  }

  if (generateRtf)
  {
    msg("Combining RTF output...\n");
    if (!RTFGenerator::preProcessFileInplace(Config_getString("RTF_OUTPUT"),"refman.rtf"))
    {
      err("An error occurred during post-processing the RTF files!\n");
    }
  }

  if (Config_getBool("HAVE_DOT"))
  {
    DotManager::instance()->run();
  }

  if (generateHtml &&
      Config_getBool("GENERATE_HTMLHELP") && 
      !Config_getString("HHC_LOCATION").isEmpty())
  {
    msg("Running html help compiler...\n");
    QString oldDir = QDir::currentDirPath();
    QDir::setCurrent(Config_getString("HTML_OUTPUT"));
    portable_sysTimerStart();
    if (portable_system(Config_getString("HHC_LOCATION"), "index.hhp", FALSE))
    {
      err("error: failed to run html help compiler on index.hhp\n");
    }
    portable_sysTimerStop();
    QDir::setCurrent(oldDir);
  }
  if ( generateHtml &&
       Config_getBool("GENERATE_QHP") && 
      !Config_getString("QHG_LOCATION").isEmpty())
  {
    msg("Running qhelpgenerator...\n");
    QCString const qhpFileName = Qhp::getQhpFileName();
    QCString const qchFileName = getQchFileName();

    QCString const args = QCString().sprintf("%s -o \"%s\"", qhpFileName.data(), qchFileName.data());
    QString const oldDir = QDir::currentDirPath();
    QDir::setCurrent(Config_getString("HTML_OUTPUT"));
    portable_sysTimerStart();
    if (portable_system(Config_getString("QHG_LOCATION"), args.data(), FALSE))
    {
      err("error: failed to run qhelpgenerator on index.qhp\n");
    }
    portable_sysTimerStop();
    QDir::setCurrent(oldDir);
  }

  int cacheParam;
  msg("symbol cache used %d/%d hits=%d misses=%d\n",
      Doxygen::symbolCache->count(),
      Doxygen::symbolCache->size(),
      Doxygen::symbolCache->hits(),
      Doxygen::symbolCache->misses());
  cacheParam = computeIdealCacheParam(Doxygen::symbolCache->misses());
  if (cacheParam>Config_getInt("SYMBOL_CACHE_SIZE"))
  {
    msg("Note: based on cache misses the ideal setting for SYMBOL_CACHE_SIZE is %d at the cost of higher memory usage.\n",cacheParam);
  }
  msg("lookup cache used %d/%d hits=%d misses=%d\n",
      Doxygen::lookupCache->count(),
      Doxygen::lookupCache->size(),
      Doxygen::lookupCache->hits(),
      Doxygen::lookupCache->misses());
  cacheParam = computeIdealCacheParam(Doxygen::lookupCache->misses()*2/3); // part of the cache is flushed, hence the 2/3 correction factor
  if (cacheParam>Config_getInt("LOOKUP_CACHE_SIZE"))
  {
    msg("Note: based on cache misses the ideal setting for LOOKUP_CACHE_SIZE is %d at the cost of higher memory usage.\n",cacheParam);
  }

  if (Debug::isFlagSet(Debug::Time))
  {
    msg("Total elapsed time: %.3f seconds\n(of which %.3f seconds waiting for external tools to finish)\n",
         ((double)Doxygen::runningTime.elapsed())/1000.0,
         portable_getSysElapsedTime()
        );
  }
  else
  {
    msg("finished...\n");
  }

  /**************************************************************************
   *                        Start cleaning up                               *
   **************************************************************************/

  cleanUpDoxygen();

  finializeSearchIndexer();
  Doxygen::symbolStorage->close();
  QDir thisDir;
  thisDir.remove(Doxygen::objDBFileName);
  Config::deleteInstance();
  QTextCodec::deleteAllCodecs();
  delete Doxygen::symbolCache;
  delete Doxygen::symbolMap;
  delete Doxygen::symbolStorage;
  g_successfulRun=TRUE;
}


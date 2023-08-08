/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include <utility>
#include <algorithm>

#include "searchindex_js.h"
#include "doxygen.h"
#include "groupdef.h"
#include "pagedef.h"
#include "namespacedef.h"
#include "classdef.h"
#include "classlist.h"
#include "membername.h"
#include "filename.h"
#include "language.h"
#include "textstream.h"
#include "util.h"
#include "version.h"
#include "message.h"
#include "resourcemgr.h"
#include "indexlist.h"
#include "portable.h"
#include "threadpool.h"

QCString searchName(const Definition *d)
{
  return d->definitionType()==Definition::TypeGroup ?  QCString(toGroupDef(d)->groupTitle()) :
         d->definitionType()==Definition::TypePage  ?  toPageDef(d)->title() :
                                                       d->localName();
}

static QCString searchId(const std::string& sname)
{
  TextStream t;
  for (size_t i=0;i<sname.length();i++)
  {
    if (isIdJS(sname[i]))
    {
      t << sname[i];
    }
    else // escape non-identifier characters
    {
      static const char *hex = "0123456789ABCDEF";
      unsigned char uc = static_cast<unsigned char>(sname[i]);
      t << '_';
      t << hex[uc>>4];
      t << hex[uc&0xF];
    }
  }

  return convertUTF8ToLower(t.str());
}

QCString searchId(const Definition *d)
{
  return searchId(searchName(d).str());
}

static void sortSearchIndexList(SearchIndexList& symList)
{
  std::sort(symList.begin(),
            symList.end(),
            [](const Definition *d1,const Definition *d2)
            {
              return qstricmp(d1->name(),d2->name())<0;
            });

  // removing same Definition's
  symList.erase(std::unique(symList.begin(),symList.end()),symList.end());
}

static std::map<std::string, SearchIndexMap> getSearchIndexMapByLetter(const SearchIndexMap &map)
{

  std::map<std::string, SearchIndexMap> result;
  for (const auto &value : map)
  {
    const auto &[searchName, symList] = value;
    std::string letter = convertUTF8ToLower(getUTF8CharAt(searchName,0));

    auto &resultMap = result[letter];
    for (const Definition* d: symList)
    {
      std::string id = searchId(searchName).str();
      resultMap[id].push_back(d);
    }
  }

  for (auto& [letter, searchMap]: result)
  {
    for (auto& [id, list]: searchMap)
    {
      sortSearchIndexList(list);
    }
  }

  return result;
}

static StringVector splitSearchTokens(std::string str)
{
  for (auto& c : str)
  {
    if (c == '(' || c == ')' || c == ',' || c == '!' || c == '/' || c == '.')
    {
      c = ' ';
    }
  }

  StringVector result = split(str, " ");
  const auto remover = [](const auto& part)
  {
    return part.empty();
  };
  result.erase(std::remove_if(result.begin(),result.end(),remover),result.end());

  if (result.size() > 1)
  {
    // Concatenate the words starting from the end to allow multiword search
    auto rit = result.end();
    const auto rend = result.begin();
    auto prev = --rit;

    do {
        --rit;
        rit->append(" ");
        rit->append(*prev);
        prev = rit;
    } while (rit != rend);
  }

  return result;
}

#define SEARCH_INDEX_ALL           0
#define SEARCH_INDEX_CLASSES       1
#define SEARCH_INDEX_INTERFACES    2
#define SEARCH_INDEX_STRUCTS       3
#define SEARCH_INDEX_EXCEPTIONS    4
#define SEARCH_INDEX_NAMESPACES    5
#define SEARCH_INDEX_FILES         6
#define SEARCH_INDEX_FUNCTIONS     7
#define SEARCH_INDEX_VARIABLES     8
#define SEARCH_INDEX_TYPEDEFS      9
#define SEARCH_INDEX_SEQUENCES    10
#define SEARCH_INDEX_DICTIONARIES 11
#define SEARCH_INDEX_ENUMS        12
#define SEARCH_INDEX_ENUMVALUES   13
#define SEARCH_INDEX_PROPERTIES   14
#define SEARCH_INDEX_EVENTS       15
#define SEARCH_INDEX_RELATED      16
#define SEARCH_INDEX_DEFINES      17
#define SEARCH_INDEX_GROUPS       18
#define SEARCH_INDEX_PAGES        19
#define SEARCH_INDEX_CONCEPTS     20

static std::array<SearchIndexInfo,NUM_SEARCH_INDICES> g_searchIndexInfo =
{ {
  //  index                         name            getText                                                  symbolList
  { /* SEARCH_INDEX_ALL */          "all"         , []() { return theTranslator->trAll();                 }, {} },
  { /* SEARCH_INDEX_CLASSES */      "classes"     , []() { return theTranslator->trClasses();             }, {} },
  { /* SEARCH_INDEX_INTERFACES */   "interfaces"  , []() { return theTranslator->trSliceInterfaces();     }, {} },
  { /* SEARCH_INDEX_STRUCTS */      "structs"     , []() { return theTranslator->trStructs();             }, {} },
  { /* SEARCH_INDEX_EXCEPTIONS */   "exceptions"  , []() { return theTranslator->trExceptions();          }, {} },
  { /* SEARCH_INDEX_NAMESPACES */   "namespaces"  , []() { return Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                  theTranslator->trModules() :
                                                                  theTranslator->trNamespace(TRUE,FALSE); }, {} },
  { /* SEARCH_INDEX_FILES */        "files"       , []() { return theTranslator->trFile(TRUE,FALSE);      }, {} },
  { /* SEARCH_INDEX_FUNCTIONS */    "functions"   , []() { return Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                  theTranslator->trOperations() :
                                                                  theTranslator->trFunctions();           }, {} },
  { /* SEARCH_INDEX_VARIABLES */    "variables"   , []() { return Config_getBool(OPTIMIZE_OUTPUT_SLICE) ?
                                                                  theTranslator->trConstants() :
                                                                  theTranslator->trVariables();           }, {} },
  { /* SEARCH_INDEX_TYPEDEFS */     "typedefs"    , []() { return theTranslator->trTypedefs();            }, {} },
  { /* SEARCH_INDEX_SEQUENCES */    "sequences"   , []() { return theTranslator->trSequences();           }, {} },
  { /* SEARCH_INDEX_DICTIONARIES */ "dictionaries", []() { return theTranslator->trDictionaries();        }, {} },
  { /* SEARCH_INDEX_ENUMS */        "enums"       , []() { return theTranslator->trEnumerations();        }, {} },
  { /* SEARCH_INDEX_ENUMVALUES */   "enumvalues"  , []() { return theTranslator->trEnumerationValues();   }, {} },
  { /* SEARCH_INDEX_PROPERTIES */   "properties"  , []() { return theTranslator->trProperties();          }, {} },
  { /* SEARCH_INDEX_EVENTS */       "events"      , []() { return theTranslator->trEvents();              }, {} },
  { /* SEARCH_INDEX_RELATED */      "related"     , []() { return theTranslator->trFriends();             }, {} },
  { /* SEARCH_INDEX_DEFINES */      "defines"     , []() { return theTranslator->trDefines();             }, {} },
  { /* SEARCH_INDEX_GROUPS */       "groups"      , []() { return theTranslator->trGroup(TRUE,FALSE);     }, {} },
  { /* SEARCH_INDEX_PAGES */        "pages"       , []() { return theTranslator->trPage(TRUE,FALSE);      }, {} },
  { /* SEARCH_INDEX_CONCEPTS */     "concepts"    , []() { return theTranslator->trConcept(true,false);   }, {} }
} };

static void addMemberToSearchIndex(const MemberDef *md)
{
  bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isLinkable = md->isLinkable();
  const ClassDef *cd=0;
  const NamespaceDef *nd=0;
  const FileDef *fd=0;
  const GroupDef *gd=0;
  if (isLinkable &&
      (
       ((cd=md->getClassDef()) && cd->isLinkable() && cd->templateMaster()==0) ||
       ((gd=md->getGroupDef()) && gd->isLinkable())
      )
     )
  {
    std::string n = md->name().str();
    if (!n.empty())
    {
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,md);
      }
      if (md->isFunction() || md->isSlot() || md->isSignal())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(n,md);
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(n,md);
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(n,md);
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(n,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(n,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(n,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(n,md);
      }
      else if (md->isProperty())
      {
        g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].add(n,md);
      }
      else if (md->isEvent())
      {
        g_searchIndexInfo[SEARCH_INDEX_EVENTS].add(n,md);
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_RELATED].add(n,md);
      }
    }
  }
  else if (isLinkable &&
      (((nd=md->getNamespaceDef()) && nd->isLinkable()) ||
       ((fd=md->getFileDef())      && fd->isLinkable())
      )
     )
  {
    std::string n = md->name().str();
    if (!n.empty())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,md);

      if (md->isFunction())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(n,md);
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(n,md);
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(n,md);
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(n,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(n,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(n,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(n,md);
      }
      else if (md->isDefine())
      {
        g_searchIndexInfo[SEARCH_INDEX_DEFINES].add(n,md);
      }
    }
  }
}

//---------------------------------------------------------------------------------------------

void createJavaScriptSearchIndex()
{
  // index classes
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    std::string n = cd->localName().str();
    if (cd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,cd.get());
      if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        if (cd->compoundType()==ClassDef::Interface)
        {
          g_searchIndexInfo[SEARCH_INDEX_INTERFACES].add(n,cd.get());
        }
        else if (cd->compoundType()==ClassDef::Struct)
        {
          g_searchIndexInfo[SEARCH_INDEX_STRUCTS].add(n,cd.get());
        }
        else if (cd->compoundType()==ClassDef::Exception)
        {
          g_searchIndexInfo[SEARCH_INDEX_EXCEPTIONS].add(n,cd.get());
        }
        else // cd->compoundType()==ClassDef::Class
        {
          g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(n,cd.get());
        }
      }
      else // non slice optimisation: group all types under classes
      {
        g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(n,cd.get());
      }
    }
  }

  // index namespaces
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    std::string n = nd->name().str();
    if (nd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,nd.get());
      g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].add(n,nd.get());
    }
  }

  // index concepts
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    std::string n = cd->name().str();
    if (cd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,cd.get());
      g_searchIndexInfo[SEARCH_INDEX_CONCEPTS].add(n,cd.get());
    }
  }

  // index files
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      std::string n = fd->name().str();
      if (fd->isLinkable())
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(n,fd.get());
        g_searchIndexInfo[SEARCH_INDEX_FILES].add(n,fd.get());
      }
    }
  }

  // index class members
  {
    // for each member name
    for (const auto &mn : *Doxygen::memberNameLinkedMap)
    {
      // for each member definition
      for (const auto &md : *mn)
      {
        addMemberToSearchIndex(md.get());
      }
    }
  }

  // index file/namespace members
  {
    // for each member name
    for (const auto &mn : *Doxygen::functionNameLinkedMap)
    {
      // for each member definition
      for (const auto &md : *mn)
      {
        addMemberToSearchIndex(md.get());
      }
    }
  }

  // index groups
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    if (gd->isLinkable())
    {
      std::string title = gd->groupTitle().str();
      for(const auto& word: splitSearchTokens(title))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(word,gd.get());
        g_searchIndexInfo[SEARCH_INDEX_GROUPS].add(word,gd.get());
      }
    }
  }

  // index pages
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->isLinkable())
    {
      std::string title = pd->title().str();
      for(const auto& word: splitSearchTokens(title))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(word,pd.get());
        g_searchIndexInfo[SEARCH_INDEX_PAGES].add(word,pd.get());
      }
    }
  }
  if (Doxygen::mainPage)
  {
    std::string title = Doxygen::mainPage->title().str();
    for(const auto& word: splitSearchTokens(title))
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(word,Doxygen::mainPage.get());
      g_searchIndexInfo[SEARCH_INDEX_PAGES].add(word,Doxygen::mainPage.get());
    }
  }
}

static void writeJavascriptSearchData(const QCString &searchDirName)
{
  std::ofstream t = Portable::openOutputStream(searchDirName+"/searchdata.js");
  if (t.is_open())
  {
    t << "var indexSectionsWithContent =\n";
    t << "{\n";
    int j=0;
    for (const auto &sii : g_searchIndexInfo)
    {
      if (!sii.symbolMap.empty())
      {
        if (j>0) t << ",\n";
        t << "  " << j << ": \"";

        std::string previous_letter;  // Does not exist in search names
        for (const auto &[searchName,list] : sii.symbolMap)
        {
          std::string letter = convertUTF8ToLower(getUTF8CharAt(searchName,0));
          if (letter != previous_letter) {
            if ( letter == "\"" ) t << "\\";
            t << letter;
            previous_letter = letter;
          }
        }
        t << "\"";
        j++;
      }
    }
    if (j>0) t << "\n";
    t << "};\n\n";
    t << "var indexSectionNames =\n";
    t << "{\n";
    j=0;
    for (const auto &sii : g_searchIndexInfo)
    {
      if (!sii.symbolMap.empty())
      {
        if (j>0) t << ",\n";
        t << "  " << j << ": \"" << sii.name << "\"";
        j++;
      }
    }
    if (j>0) t << "\n";
    t << "};\n\n";
    t << "var indexSectionLabels =\n";
    t << "{\n";
    j=0;
    for (const auto &sii : g_searchIndexInfo)
    {
      if (!sii.symbolMap.empty())
      {
        if (j>0) t << ",\n";
        t << "  " << j << ": \"" << convertToXML(sii.getText()) << "\"";
        j++;
      }
    }
    if (j>0) t << "\n";
    t << "};\n\n";
  }
}

static void writeJavasScriptSearchDataPage(const QCString &baseName,const QCString &dataFileName,const SearchIndexMap &map)
{
  int cnt = 0;
  std::ofstream ti = Portable::openOutputStream(dataFileName);
  if (!ti.is_open())
  {
    err("Failed to open file '%s' for writing...\n",qPrint(dataFileName));
    return;
  }

  ti << "var searchData=\n";
  // format
  // searchData[] = array of items
  // searchData[x][0] = id
  // searchData[x][1] = [ name + child1 + child2 + .. ]
  // searchData[x][1][0] = name as shown
  // searchData[x][1][y+1] = info for child y
  // searchData[x][1][y+1][0] = url
  // searchData[x][1][y+1][1] = 1 => target="_parent"
  // searchData[x][1][y+1][1] = 0 => target="_blank"
  // searchData[x][1][y+1][2] = scope

  ti << "[\n";
  bool firstEntry=TRUE;

  int childCount=0;
  bool newId=TRUE;
  QCString lastName;
  const Definition *prevScope = 0;
  for (const auto &[id, list]: map)
  {
    newId=TRUE;
    for (auto it=list.begin(); it!=list.end();)
    {
      const Definition *d = *it;
      QCString sname = searchName(d);

      if (sname!=lastName || newId) // this item has a different search word
      {
        if (!firstEntry)
        {
          ti << "]]]";
          ti << ",\n";
        }
        firstEntry=FALSE;
        ti << "  ['" << id << "_" << cnt++ << "',['" << convertToXML(sname) << "',[";
        childCount=0;
        prevScope=0;
        newId=FALSE;
      }

      ++it;

      const Definition *scope     = d->getOuterScope();
      const Definition *next      = it!=list.end() ? *it : 0;
      const Definition *nextScope = 0;
      const MemberDef  *md        = toMemberDef(d);
      if (next) nextScope = next->getOuterScope();
      QCString anchor = d->anchor();

      if (childCount>0)
      {
        ti << "],[";
      }
      QCString fn = d->getOutputFileBase();
      addHtmlExtensionIfMissing(fn);
      ti << "'" << externalRef("../",d->getReference(),TRUE) << fn;
      if (!anchor.isEmpty())
      {
        ti << "#" << anchor;
      }
      ti << "',";

      bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
      if (!extLinksInWindow || d->getReference().isEmpty())
      {
        ti << "1,";
      }
      else
      {
        ti << "0,";
      }

      if (lastName!=sname && (next==0 || searchName(next)!=sname)) // unique name
      {
        if (d->getOuterScope()!=Doxygen::globalScope)
        {
          ti << "'" << convertToXML(d->getOuterScope()->name()) << "'";
        }
        else if (md)
        {
          const FileDef *fd = md->getBodyDef();
          if (fd==0) fd = md->getFileDef();
          if (fd)
          {
            ti << "'" << convertToXML(fd->localName()) << "'";
          }
        }
        else
        {
          ti << "''";
        }
      }
      else // multiple entries with the same name
      {
        bool found=FALSE;
        bool overloadedFunction = ((prevScope!=0 && scope==prevScope) ||
            (scope && scope==nextScope)) && md && (md->isFunction() || md->isSlot());
        QCString prefix;
        if (md) prefix=convertToXML(md->localName());
        if (overloadedFunction) // overloaded member function
        {
          prefix+=convertToXML(md->argsString());
          // show argument list to disambiguate overloaded functions
        }
        else if (md && md->isCallable()) // unique member function
        {
          prefix+="()"; // only to show it is a function
        }
        QCString name;
        if (d->definitionType()==Definition::TypeClass)
        {
          name = convertToXML((toClassDef(d))->displayName());
          found = TRUE;
        }
        else if (d->definitionType()==Definition::TypeNamespace)
        {
          name = convertToXML((toNamespaceDef(d))->displayName());
          found = TRUE;
        }
        else if (scope==0 || scope==Doxygen::globalScope) // in global scope
        {
          if (md)
          {
            const FileDef *fd = md->getBodyDef();
            if (fd==0) fd = md->resolveAlias()->getFileDef();
            if (fd)
            {
              if (!prefix.isEmpty()) prefix+=":&#160;";
              name = prefix + convertToXML(fd->localName());
              found = TRUE;
            }
          }
        }
        else if (md && (md->resolveAlias()->getClassDef() || md->resolveAlias()->getNamespaceDef()))
          // member in class or namespace scope
        {
          SrcLangExt lang = md->getLanguage();
          name = convertToXML(d->getOuterScope()->qualifiedName())
            + getLanguageSpecificSeparator(lang) + prefix;
          found = TRUE;
        }
        else if (scope) // some thing else? -> show scope
        {
          name = prefix + convertToXML(scope->name());
          found = TRUE;
        }
        if (!found) // fallback
        {
          name = prefix + "("+theTranslator->trGlobalNamespace()+")";
        }

        ti << "'" << name << "'";

        prevScope = scope;
        childCount++;
      }
      lastName = sname;
    }
  }
  if (!firstEntry)
  {
    ti << "]]]\n";
  }
  ti << "];\n";
  Doxygen::indexList->addStyleSheetFile(("search/"+baseName+".js").data());
}


void writeJavaScriptSearchIndex()
{
  // write index files
  QCString searchDirName = Config_getString(HTML_OUTPUT)+"/search";

  std::size_t numThreads = static_cast<std::size_t>(Config_getInt(NUM_PROC_THREADS));
  if (numThreads>1) // multi threaded version
  {
    ThreadPool threadPool(numThreads);
    std::vector< std::future<int> > results;
    for (auto &sii : g_searchIndexInfo)
    {
      int p=0;
      for (auto [searchName,symList] : getSearchIndexMapByLetter(sii.symbolMap))
      {
        QCString baseName;
        baseName.sprintf("%s_%x",sii.name.data(),p);
        QCString dataFileName = searchDirName + "/"+baseName+".js";
        auto processFile = [p,baseName,dataFileName,list=std::move(symList)]()
        {
          writeJavasScriptSearchDataPage(baseName,dataFileName,list);
          return p;
        };
        results.emplace_back(threadPool.queue(processFile));
        p++;
      }
    }
    // wait for the results
    for (auto &f : results) f.get();
  }
  else // single threaded version
  {
    for (auto &sii : g_searchIndexInfo)
    {
      int p=0;
      for (const auto &[searchName,symList] : getSearchIndexMapByLetter(sii.symbolMap))
      {
        QCString baseName;
        baseName.sprintf("%s_%x",sii.name.data(),p);
        QCString dataFileName = searchDirName + "/"+baseName+".js";
        writeJavasScriptSearchDataPage(baseName,dataFileName,symList);
        p++;
      }
    }
  }

  writeJavascriptSearchData(searchDirName);
  ResourceMgr::instance().copyResource("search.js",searchDirName);

  Doxygen::indexList->addStyleSheetFile("search/searchdata.js");
  Doxygen::indexList->addStyleSheetFile("search/search.js");
}

//--------------------------------------------------------------------------------------

void SearchIndexInfo::add(const std::string &searchName,const Definition *def)
{
  std::string lowercaseSearchName = convertUTF8ToLower(searchName);
  auto it = symbolMap.find(lowercaseSearchName);
  if (it!=symbolMap.end())
  {
    it->second.push_back(def);
  }
  else
  {
    symbolMap.insert(std::make_pair(lowercaseSearchName,std::vector<const Definition*>({def})));
  }
}

const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices()
{
  return g_searchIndexInfo;
}


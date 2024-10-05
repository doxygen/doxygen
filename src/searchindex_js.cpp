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
#include <cassert>

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
#include "moduledef.h"
#include "section.h"

//-------------------------------------------------------------------------------------------

void SearchTerm::makeTitle()
{
  if (std::holds_alternative<const Definition *>(info))
  {
    const Definition *def = std::get<const Definition *>(info);
    Definition::DefType type = def->definitionType();
    title = type==Definition::TypeGroup ?  filterTitle(toGroupDef(def)->groupTitle()) :
            type==Definition::TypePage  ?  filterTitle(toPageDef(def)->title()) :
                                           def->localName();
  }
  else if (std::holds_alternative<const SectionInfo *>(info))
  {
    title = std::get<const SectionInfo *>(info)->title();
  }
  else
  {
    assert(false);
  }
}

QCString SearchTerm::termEncoded() const
{
  TextStream t;
  for (size_t i=0;i<word.length();i++)
  {
    if (isIdJS(word.at(i)))
    {
      t << word.at(i);
    }
    else // escape non-identifier characters
    {
      static const char *hex = "0123456789ABCDEF";
      unsigned char uc = static_cast<unsigned char>(word.at(i));
      t << '_';
      t << hex[uc>>4];
      t << hex[uc&0xF];
    }
  }

  return convertUTF8ToLower(t.str());
}

//-------------------------------------------------------------------------------------------

//! helper function to simplify the given title string, and fill a list of start positions
//! for the start of each word in the simplified title string.
static void splitSearchTokens(QCString &title,IntVector &indices)
{
  if (title.isEmpty()) return;

  // simplify title to contain only words with single space as separator
  size_t di=0;
  bool lastIsSpace=true;
  for (size_t si=0; si<title.length(); si++)
  {
    char c = title.at(si);
    if (isId(c) || c==':') // add "word" character
    {
      title.at(di)=c;
      di++;
      lastIsSpace=false;
    }
    else if (!lastIsSpace) // add one separator as space
    {
      title.at(di)=' ';
      di++;
      lastIsSpace=true;
    }
  }
  if (di>0 && title.at(di-1)==' ') di--; // strip trailing whitespace
  title.resize(di);

  // create a list of start positions within title for
  // each unique word in order of appearance
  int p=0,i=0;
  while ((i=title.find(' ',p))!=-1)
  {
    std::string word = title.mid(p,i-p).str();
    indices.push_back(p);
    p = i+1;
  }
  if (p<static_cast<int>(title.length()))
  {
    std::string word = title.mid(p).str();
    indices.push_back(p);
  }
}

//-------------------------------------------------------------------------------------------

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
#define SEARCH_INDEX_MODULES      21

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
  { /* SEARCH_INDEX_CONCEPTS */     "concepts"    , []() { return theTranslator->trConcept(true,false);   }, {} },
  { /* SEARCH_INDEX_MODULES */      "modules"     , []() { return theTranslator->trModule(true,false);    }, {} }
} };

static void addMemberToSearchIndex(const MemberDef *md)
{
  bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isLinkable = md->isLinkable();
  const ClassDef *cd=nullptr;
  const NamespaceDef *nd=nullptr;
  const FileDef *fd=nullptr;
  const GroupDef *gd=nullptr;
  if (isLinkable &&
      (
       ((cd=md->getClassDef()) && cd->isLinkable() && cd->templateMaster()==nullptr) ||
       ((gd=md->getGroupDef()) && gd->isLinkable())
      )
     )
  {
    QCString n = md->name();
    if (!n.isEmpty())
    {
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,md));
      }
      if (md->isFunction() || md->isSlot() || md->isSignal())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(SearchTerm(n,md));
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(SearchTerm(n,md));
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(SearchTerm(n,md));
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(SearchTerm(n,md));
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(SearchTerm(n,md));
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(SearchTerm(n,md));
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(SearchTerm(n,md));
      }
      else if (md->isProperty())
      {
        g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].add(SearchTerm(n,md));
      }
      else if (md->isEvent())
      {
        g_searchIndexInfo[SEARCH_INDEX_EVENTS].add(SearchTerm(n,md));
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_RELATED].add(SearchTerm(n,md));
      }
    }
  }
  else if (isLinkable &&
      (((nd=md->getNamespaceDef()) && nd->isLinkable()) ||
       ((fd=md->getFileDef())      && fd->isLinkable())
      )
     )
  {
    QCString n = md->name();
    if (!n.isEmpty())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,md));

      if (md->isFunction())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(SearchTerm(n,md));
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(SearchTerm(n,md));
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(SearchTerm(n,md));
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(SearchTerm(n,md));
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(SearchTerm(n,md));
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(SearchTerm(n,md));
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(SearchTerm(n,md));
      }
      else if (md->isDefine())
      {
        g_searchIndexInfo[SEARCH_INDEX_DEFINES].add(SearchTerm(n,md));
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
    if (cd->isLinkable())
    {
      QCString n = cd->localName();
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,cd.get()));
      if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        if (cd->compoundType()==ClassDef::Interface)
        {
          g_searchIndexInfo[SEARCH_INDEX_INTERFACES].add(SearchTerm(n,cd.get()));
        }
        else if (cd->compoundType()==ClassDef::Struct)
        {
          g_searchIndexInfo[SEARCH_INDEX_STRUCTS].add(SearchTerm(n,cd.get()));
        }
        else if (cd->compoundType()==ClassDef::Exception)
        {
          g_searchIndexInfo[SEARCH_INDEX_EXCEPTIONS].add(SearchTerm(n,cd.get()));
        }
        else // cd->compoundType()==ClassDef::Class
        {
          g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(SearchTerm(n,cd.get()));
        }
      }
      else // non slice optimization: group all types under classes
      {
        g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(SearchTerm(n,cd.get()));
      }
    }
  }

  // index namespaces
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    if (nd->isLinkable())
    {
      QCString n = nd->name();
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,nd.get()));
      g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].add(SearchTerm(n,nd.get()));
    }
  }

  // index concepts
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    if (cd->isLinkable())
    {
      QCString n = cd->name();
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,cd.get()));
      g_searchIndexInfo[SEARCH_INDEX_CONCEPTS].add(SearchTerm(n,cd.get()));
    }
  }

  // index modules
  for (const auto &mod : ModuleManager::instance().modules())
  {
    if (mod->isLinkable() && mod->isPrimaryInterface())
    {
      QCString n = mod->name();
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,mod.get()));
      g_searchIndexInfo[SEARCH_INDEX_MODULES].add(SearchTerm(n,mod.get()));
    }
  }

  // index files
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      QCString n = fd->name();
      if (fd->isLinkable())
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(n,fd.get()));
        g_searchIndexInfo[SEARCH_INDEX_FILES].add(SearchTerm(n,fd.get()));
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
      QCString title(filterTitle(gd->groupTitle()).str());
      IntVector tokenIndices;
      splitSearchTokens(title,tokenIndices);
      for (int index : tokenIndices)
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(title.mid(index),gd.get()));
        g_searchIndexInfo[SEARCH_INDEX_GROUPS].add(SearchTerm(title.mid(index),gd.get()));
      }
    }
  }

  // index pages
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->isLinkable())
    {
      QCString title(filterTitle(pd->title()).str());
      IntVector tokenIndices;
      splitSearchTokens(title,tokenIndices);
      for (int index : tokenIndices)
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(title.mid(index),pd.get()));
        g_searchIndexInfo[SEARCH_INDEX_PAGES].add(SearchTerm(title.mid(index),pd.get()));
      }
    }
  }

  // main page
  if (Doxygen::mainPage)
  {
    QCString title(filterTitle(Doxygen::mainPage->title()).str());
    IntVector tokenIndices;
    splitSearchTokens(title,tokenIndices);
    for (int index : tokenIndices)
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(title.mid(index),Doxygen::mainPage.get()));
      g_searchIndexInfo[SEARCH_INDEX_PAGES].add(SearchTerm(title.mid(index),Doxygen::mainPage.get()));
    }
  }

  // sections
  const auto &sm = SectionManager::instance();
  for (const auto &sectionInfo : sm)
  {
    if (sectionInfo->level()>0) // level 0 is for page titles
    {
      QCString title = filterTitle(sectionInfo->title());
      IntVector tokenIndices;
      splitSearchTokens(title,tokenIndices);
      for (int index : tokenIndices)
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(SearchTerm(title.mid(index),sectionInfo.get()));
      }
    }
  }

  // sort all lists
  for (auto &sii : g_searchIndexInfo) // for each index
  {
    for (auto &[name,symList] : sii.symbolMap) // for each symbol in the index
    {
      // sort the symbols (first on search term, and then on full name)
      //
      // `std::stable_sort` is used here due to reproducibility issues
      // on key collisions
      // https://github.com/doxygen/doxygen/issues/10445
      std::stable_sort(symList.begin(),
                symList.end(),
                [](const auto &t1,const auto &t2)
                {
                  int    eq =    qstricmp_sort(t1.word,t2.word);             // search term first
                  return eq==0 ? qstricmp_sort(t1.title,t2.title)<0 : eq<0;  // then full title
                });
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

        std::string previous_letter;  // start with value that does not exist in the map
        for (const auto &[letter,list] : sii.symbolMap)
        {
          if (letter != previous_letter)
          {
            if ( letter == "\"" ) t << "\\"; // add escape for backslash
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

static void writeJavasScriptSearchDataPage(const QCString &baseName,const QCString &dataFileName,const SearchIndexList &list)
{
  auto isDef = [](const SearchTerm::LinkInfo &info)
  {
    return std::holds_alternative<const Definition *>(info);
  };
  auto getDef = [&isDef](const SearchTerm::LinkInfo &info)
  {
    return isDef(info) ? std::get<const Definition *>(info) : nullptr;
  };
  auto isSection = [](const SearchTerm::LinkInfo &info)
  {
    return std::holds_alternative<const SectionInfo *>(info);
  };
  auto getSection = [&isSection](const SearchTerm::LinkInfo &info)
  {
    return isSection(info) ? std::get<const SectionInfo *>(info) : nullptr;
  };

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
  QCString lastWord;
  const Definition *prevScope = nullptr;
  for (auto it = list.begin(); it!=list.end();)
  {
    const SearchTerm &term = *it;
    const SearchTerm::LinkInfo info = term.info;
    const Definition *d             = getDef(info);
    const SectionInfo *si           = getSection(info);
    assert(d || si); // either d or si should be valid
    QCString word                   = term.word;
    QCString id                     = term.termEncoded();
    ++it;
    const Definition *scope         = d ? d->getOuterScope() : nullptr;
    const SearchTerm::LinkInfo next = it!=list.end() ? it->info : SearchTerm::LinkInfo();
    const Definition *nextScope     = isDef(next) ? getDef(next)->getOuterScope() : nullptr;
    const MemberDef  *md            = toMemberDef(d);
    QCString         anchor         = d ? d->anchor() : si ? si->label() : QCString();

    if (word!=lastWord) // this item has a different search word
    {
      if (!firstEntry)
      {
        ti << "]]]";
        ti << ",\n";
      }
      firstEntry=FALSE;
      ti << "  ['" << id << "_" << cnt++ << "',['";
      if (next==SearchTerm::LinkInfo() || it->word!=word) // unique result, show title
      {
        ti << convertToXML(term.title);
      }
      else // multiple results, show matching word only, expanded list will show title
      {
        ti << convertToXML(term.word);
      }
      ti << "',[";
      childCount=0;
      prevScope=nullptr;
    }

    if (childCount>0)
    {
      ti << "],[";
    }
    QCString fn  = d ? d->getOutputFileBase() : si ? si->fileName() : QCString();
    QCString ref = d ? d->getReference()      : si ? si->ref()      : QCString();
    addHtmlExtensionIfMissing(fn);
    ti << "'" << externalRef("../",ref,TRUE) << fn;
    if (!anchor.isEmpty())
    {
      ti << "#" << anchor;
    }
    ti << "',";

    bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
    if (!extLinksInWindow || ref.isEmpty())
    {
      ti << "1,";
    }
    else
    {
      ti << "0,";
    }

    if (lastWord!=word && (next==SearchTerm::LinkInfo() || it->word!=word)) // unique search result
    {
      if (d && d->getOuterScope()!=Doxygen::globalScope)
      {
        ti << "'" << convertToXML(d->getOuterScope()->name()) << "'";
      }
      else if (md)
      {
        const FileDef *fd = md->getBodyDef();
        if (fd==nullptr) fd = md->getFileDef();
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
      bool overloadedFunction = ((prevScope!=nullptr && scope==prevScope) || (scope && scope==nextScope)) &&
                                 md && md->isCallable();
      QCString prefix;
      if (md) prefix=convertToXML(md->localName());
      if (overloadedFunction) // overloaded member function
      {
        prefix+=convertToXML(md->argsString());
        // show argument list to disambiguate overloaded functions
      }
      else if (md && md->isCallable()) // unique member function
      {
        prefix+="()"; // only to show it is a callable symbol
      }
      QCString name;
      if (d)
      {
        switch (d->definitionType())
        {
          case Definition::TypeClass:     name = convertToXML((toClassDef(d))->displayName());                    found=true; break;
          case Definition::TypeNamespace: name = convertToXML((toNamespaceDef(d))->displayName());                found=true; break;
          case Definition::TypeModule:    name = convertToXML(d->name()+" "+theTranslator->trModule(false,true)); found=true; break;
          case Definition::TypePage:      name = convertToXML(filterTitle(toPageDef(d)->title()));                found=true; break;
          case Definition::TypeGroup:     name = convertToXML(filterTitle(toGroupDef(d)->groupTitle()));          found=true; break;
          default:
            if (scope==nullptr || scope==Doxygen::globalScope) // in global scope
            {
              if (md)
              {
                const FileDef *fd = md->getBodyDef();
                if (fd==nullptr) fd = md->resolveAlias()->getFileDef();
                if (fd)
                {
                  if (!prefix.isEmpty()) prefix+=":&#160;";
                  name = prefix + convertToXML(fd->localName());
                  found = true;
                }
              }
            }
            else if (md && (md->resolveAlias()->getClassDef() || md->resolveAlias()->getNamespaceDef()))
              // member in class or namespace scope
            {
              SrcLangExt lang = md->getLanguage();
              name = convertToXML(d->getOuterScope()->qualifiedName()) + getLanguageSpecificSeparator(lang) + prefix;
              found = true;
            }
            else if (scope) // some thing else? -> show scope
            {
              name = prefix + convertToXML(scope->name());
              found = true;
            }
            break;
        }
      }
      else if (si)
      {
        name = convertToXML(filterTitle(si->title()));
        found = true;
      }
      if (!found) // fallback
      {
        name = prefix + "("+theTranslator->trGlobalNamespace()+")";
      }

      ti << "'" << name << "'";

      prevScope = scope;
      childCount++;
    }
    lastWord = word;
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
      for (const auto &[letter,symList] : sii.symbolMap)
      {
        QCString baseName;
        baseName.sprintf("%s_%x",sii.name.data(),p);
        QCString dataFileName = searchDirName + "/"+baseName+".js";
        auto &list = symList;
        auto processFile = [p,baseName,dataFileName,&list]()
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
      for (const auto &[letter,symList] : sii.symbolMap)
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
  auto &mgr = ResourceMgr::instance();
  {
    std::ofstream fn = Portable::openOutputStream(searchDirName+"/search.js");
    if (fn.is_open())
    {
      TextStream t(&fn);
      t << substitute(mgr.getAsString("search.js"),"$PROJECTID",getProjectId());
    }
  }

  Doxygen::indexList->addStyleSheetFile("search/searchdata.js");
  Doxygen::indexList->addStyleSheetFile("search/search.js");
}

//--------------------------------------------------------------------------------------

void SearchIndexInfo::add(const SearchTerm &term)
{
  std::string letter = convertUTF8ToLower(getUTF8CharAt(term.word.str(),0));
  auto &list = symbolMap[letter]; // creates a new entry if not found
  list.push_back(term);
}

const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices()
{
  return g_searchIndexInfo;
}


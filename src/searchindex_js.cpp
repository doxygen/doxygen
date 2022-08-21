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
#include <fstream>

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

QCString searchName(const Definition *d)
{
  return d->definitionType()==Definition::TypeGroup ?  QCString(toGroupDef(d)->groupTitle()) :
         d->definitionType()==Definition::TypePage  ?  toPageDef(d)->title() :
                                                       d->localName();
}

QCString searchId(const Definition *d)
{
  std::string s = searchName(d).str();
  TextStream t;
  for (size_t i=0;i<s.length();i++)
  {
    if (isIdJS(s[i]))
    {
      t << s[i];
    }
    else // escape non-identifier characters
    {
      static const char *hex = "0123456789ABCDEF";
      unsigned char uc = static_cast<unsigned char>(s[i]);
      t << '_';
      t << hex[uc>>4];
      t << hex[uc&0xF];
    }
  }

  return convertUTF8ToLower(t.str());
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
      std::string letter = convertUTF8ToLower(getUTF8CharAt(n,0));
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,md);
      }
      if (md->isFunction() || md->isSlot() || md->isSignal())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(letter,md);
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(letter,md);
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(letter,md);
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(letter,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(letter,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(letter,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(letter,md);
      }
      else if (md->isProperty())
      {
        g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].add(letter,md);
      }
      else if (md->isEvent())
      {
        g_searchIndexInfo[SEARCH_INDEX_EVENTS].add(letter,md);
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_RELATED].add(letter,md);
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
      std::string letter = convertUTF8ToLower(getUTF8CharAt(n,0));
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,md);

      if (md->isFunction())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].add(letter,md);
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].add(letter,md);
      }
      else if (md->isSequence())
      {
        g_searchIndexInfo[SEARCH_INDEX_SEQUENCES].add(letter,md);
      }
      else if (md->isDictionary())
      {
        g_searchIndexInfo[SEARCH_INDEX_DICTIONARIES].add(letter,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].add(letter,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].add(letter,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].add(letter,md);
      }
      else if (md->isDefine())
      {
        g_searchIndexInfo[SEARCH_INDEX_DEFINES].add(letter,md);
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
    std::string letter = convertUTF8ToLower(getUTF8CharAt(cd->localName().str(),0));
    if (cd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,cd.get());
      if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        if (cd->compoundType()==ClassDef::Interface)
        {
          g_searchIndexInfo[SEARCH_INDEX_INTERFACES].add(letter,cd.get());
        }
        else if (cd->compoundType()==ClassDef::Struct)
        {
          g_searchIndexInfo[SEARCH_INDEX_STRUCTS].add(letter,cd.get());
        }
        else if (cd->compoundType()==ClassDef::Exception)
        {
          g_searchIndexInfo[SEARCH_INDEX_EXCEPTIONS].add(letter,cd.get());
        }
        else // cd->compoundType()==ClassDef::Class
        {
          g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(letter,cd.get());
        }
      }
      else // non slice optimisation: group all types under classes
      {
        g_searchIndexInfo[SEARCH_INDEX_CLASSES].add(letter,cd.get());
      }
    }
  }

  // index namespaces
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    std::string letter = convertUTF8ToLower(getUTF8CharAt(nd->name().str(),0));
    if (nd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,nd.get());
      g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].add(letter,nd.get());
    }
  }

  // index concepts
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    std::string letter = convertUTF8ToLower(getUTF8CharAt(cd->name().str(),0));
    if (cd->isLinkable())
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,cd.get());
      g_searchIndexInfo[SEARCH_INDEX_CONCEPTS].add(letter,cd.get());
    }
  }

  // index files
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      std::string letter = convertUTF8ToLower(getUTF8CharAt(fd->name().str(),0));
      if (fd->isLinkable())
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,fd.get());
        g_searchIndexInfo[SEARCH_INDEX_FILES].add(letter,fd.get());
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
      if (!title.empty()) // TODO: able searching for all word in the title
      {
        std::string letter = convertUTF8ToLower(getUTF8CharAt(title,0));
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,gd.get());
        g_searchIndexInfo[SEARCH_INDEX_GROUPS].add(letter,gd.get());
      }
    }
  }

  // index pages
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (pd->isLinkable())
    {
      std::string title = pd->title().str();
      if (!title.empty())
      {
        std::string letter = convertUTF8ToLower(getUTF8CharAt(title,0));
        g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,pd.get());
        g_searchIndexInfo[SEARCH_INDEX_PAGES].add(letter,pd.get());
      }
    }
  }
  if (Doxygen::mainPage)
  {
    std::string title = Doxygen::mainPage->title().str();
    if (!title.empty())
    {
      std::string letter = convertUTF8ToLower(getUTF8CharAt(title,0));
      g_searchIndexInfo[SEARCH_INDEX_ALL].add(letter,Doxygen::mainPage.get());
      g_searchIndexInfo[SEARCH_INDEX_PAGES].add(letter,Doxygen::mainPage.get());
    }
  }

  // sort all lists
  for (auto &sii : g_searchIndexInfo) // for each index
  {
    for (auto &kv : sii.symbolMap) // for each symbol in the index
    {
      // sort the symbols (first on "search" name, and then on full name)
      std::sort(kv.second.begin(),
                kv.second.end(),
                [](const Definition *d1,const Definition *d2)
                {
                  int eq = qstricmp(searchName(d1),searchName(d2));         // search name first
                  return eq==0 ? qstricmp(d1->name(),d2->name())<0 : eq<0;  // then full name
                });
    }
  }
}

void writeJavaScriptSearchIndex()
{
  // write index files
  QCString searchDirName = Config_getString(HTML_OUTPUT)+"/search";

  for (auto &sii : g_searchIndexInfo)
  {
    int p=0;
    for (const auto &kv : sii.symbolMap)
    {
      int cnt = 0;
      QCString baseName;
      baseName.sprintf("%s_%x",sii.name.data(),p);

      QCString dataFileName = searchDirName + "/"+baseName+".js";

      std::ofstream ti(dataFileName.str(), std::ofstream::out | std::ofstream::binary);
      if (ti.is_open())
      {

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
        QCString lastName;
        const Definition *prevScope = 0;
        for (auto it = kv.second.begin(); it!=kv.second.end();)
        {
          const Definition *d = *it;
          QCString sname = searchName(d);
          QCString id    = searchId(d);

          if (sname!=lastName) // this item has a different search word
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
          }

          ++it;
          const Definition *scope     = d->getOuterScope();
          const Definition *next      = it!=kv.second.end() ? *it : 0;
          const Definition *nextScope = 0;
          const MemberDef  *md        = toMemberDef(d);
          if (next) nextScope = next->getOuterScope();
          QCString anchor = d->anchor();

          if (childCount>0)
          {
            ti << "],[";
          }
          ti << "'" << externalRef("../",d->getReference(),TRUE)
            << addHtmlExtensionIfMissing(d->getOutputFileBase());
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
            else if (md) // unique member function
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
        if (!firstEntry)
        {
          ti << "]]]\n";
        }
        ti << "];\n";
      }
      else
      {
        err("Failed to open file '%s' for writing...\n",qPrint(dataFileName));
      }
      p++;
    }
  }

  {
    std::ofstream t(searchDirName.str()+"/searchdata.js",
                    std::ofstream::out | std::ofstream::binary);
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

          for (const auto &kv : sii.symbolMap)
          {
            if ( kv.first == "\"" ) t << "\\";
            t << kv.first;
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
    ResourceMgr::instance().copyResource("search.js",searchDirName);
  }

  Doxygen::indexList->addStyleSheetFile("search/search.js");
}

//--------------------------------------------------------------------------------------

void SearchIndexInfo::add(const std::string &letter,const Definition *def)
{
  //printf("%p: %s->%s (full=%s)\n",this,qPrint(letter),qPrint(searchName(def)),qPrint(def->name()));
  auto it = symbolMap.find(letter);
  if (it!=symbolMap.end())
  {
    it->second.push_back(def);
  }
  else
  {
    symbolMap.insert(std::make_pair(letter,std::vector<const Definition*>({def})));
  }
}

const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices()
{
  return g_searchIndexInfo;
}


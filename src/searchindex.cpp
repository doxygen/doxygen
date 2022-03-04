/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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


#include <ctype.h>
#include <assert.h>
#include <sstream>

#include "searchindex.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "language.h"
#include "pagedef.h"
#include "growbuf.h"
#include "message.h"
#include "version.h"
#include "groupdef.h"
#include "filedef.h"
#include "memberdef.h"
#include "filename.h"
#include "membername.h"
#include "resourcemgr.h"
#include "namespacedef.h"
#include "classdef.h"
#include "utf8.h"
#include "classlist.h"

//---------------------------------------------------------------------------------------------
// the following part is for the server based search engine
//---------------------------------------------------------------------------------------------

// file format: (all multi-byte values are stored in big endian format)
//   4 byte header
//   256*256*4 byte index (4 bytes)
//   for each index entry: a zero terminated list of words
//   for each word: a \0 terminated string + 4 byte offset to the stats info
//   padding bytes to align at 4 byte boundary
//   for each word: the number of urls (4 bytes)
//               + for each url containing the word 8 bytes statistics
//                 (4 bytes index to url string + 4 bytes frequency counter)
//   for each url: a \0 terminated string

const size_t numIndexEntries = 256*256;

//--------------------------------------------------------------------

IndexWord::IndexWord(QCString word) : m_word(word)
{
  //printf("IndexWord::IndexWord(%s)\n",word);
}

void IndexWord::addUrlIndex(int idx,bool hiPriority)
{
  //printf("IndexWord::addUrlIndex(%d,%d)\n",idx,hiPriority);
  auto it = m_urls.find(idx);
  if (it==m_urls.end())
  {
    //printf("URLInfo::URLInfo(%d)\n",idx);
    it = m_urls.insert(std::make_pair(idx,URLInfo(idx,0))).first;
  }
  it->second.freq+=2;
  if (hiPriority) it->second.freq|=1; // mark as high priority document
}

//--------------------------------------------------------------------

SearchIndex::SearchIndex() : SearchIndexIntf(Internal)
{
  m_index.resize(numIndexEntries);
}

void SearchIndex::setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
{
  if (ctx==0) return;
  assert(!isSourceFile || ctx->definitionType()==Definition::TypeFile);
  //printf("SearchIndex::setCurrentDoc(%s,%s,%s)\n",name,baseName,anchor);
  QCString url=isSourceFile ? (toFileDef(ctx))->getSourceFileBase() : ctx->getOutputFileBase();
  url+=Config_getString(HTML_FILE_EXTENSION);
  QCString baseUrl = url;
  if (!anchor.isEmpty()) url+=QCString("#")+anchor;
  if (!isSourceFile) baseUrl=url;
  QCString name=ctx->qualifiedName();
  if (ctx->definitionType()==Definition::TypeMember)
  {
    const MemberDef *md = toMemberDef(ctx);
    name.prepend((md->getLanguage()==SrcLangExt_Fortran  ?
                 theTranslator->trSubprogram(TRUE,TRUE) :
                 theTranslator->trMember(TRUE,TRUE))+" ");
  }
  else // compound type
  {
    SrcLangExt lang = ctx->getLanguage();
    QCString sep = getLanguageSpecificSeparator(lang);
    if (sep!="::")
    {
      name = substitute(name,"::",sep);
    }
    switch (ctx->definitionType())
    {
      case Definition::TypePage:
        {
          const PageDef *pd = toPageDef(ctx);
          if (pd->hasTitle())
          {
            name = theTranslator->trPage(TRUE,TRUE)+" "+pd->title();
          }
          else
          {
            name = theTranslator->trPage(TRUE,TRUE)+" "+pd->name();
          }
        }
        break;
      case Definition::TypeClass:
        {
          const ClassDef *cd = toClassDef(ctx);
          name.prepend(cd->compoundTypeString()+" ");
        }
        break;
      case Definition::TypeNamespace:
        {
          if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
          {
            name = theTranslator->trPackage(name);
          }
          else if (lang==SrcLangExt_Fortran)
          {
            name.prepend(theTranslator->trModule(TRUE,TRUE)+" ");
          }
          else
          {
            name.prepend(theTranslator->trNamespace(TRUE,TRUE)+" ");
          }
        }
        break;
      case Definition::TypeGroup:
        {
          const GroupDef *gd = toGroupDef(ctx);
          if (!gd->groupTitle().isEmpty())
          {
            name = theTranslator->trGroup(TRUE,TRUE)+" "+gd->groupTitle();
          }
          else
          {
            name.prepend(theTranslator->trGroup(TRUE,TRUE)+" ");
          }
        }
        break;
      default:
        break;
    }
  }

  auto it = m_url2IdMap.find(baseUrl.str());
  if (it == m_url2IdMap.end())
  {
    ++m_urlIndex;
    m_url2IdMap.insert(std::make_pair(baseUrl.str(),m_urlIndex));
    m_urls.insert(std::make_pair(m_urlIndex,URL(name,url)));
  }
  else
  {
    m_urls.insert(std::make_pair(it->second,URL(name,url)));
  }
}

static int charsToIndex(const QCString &word)
{
  if (word.length()<2) return -1;

  // Fast string hashing algorithm
  //register ushort h=0;
  //const char *k = word;
  //ushort mask=0xfc00;
  //while ( *k )
  //{
  //  h = (h&mask)^(h<<6)^(*k++);
  //}
  //return h;

  // Simple hashing that allows for substring searching
  uint c1=static_cast<uchar>(word[0]);
  uint c2=static_cast<uchar>(word[1]);
  return c1*256+c2;
}

void SearchIndex::addWord(const QCString &word,bool hiPriority,bool recurse)
{
  if (word.isEmpty()) return;
  QCString wStr = QCString(word).lower();
  //printf("SearchIndex::addWord(%s,%d) wStr=%s\n",word,hiPriority,qPrint(wStr));
  int idx=charsToIndex(wStr);
  if (idx<0 || idx>=static_cast<int>(m_index.size())) return;
  auto it = m_words.find(wStr.str());
  if (it==m_words.end())
  {
    //fprintf(stderr,"addWord(%s) at index %d\n",word,idx);
    m_index[idx].push_back(IndexWord(wStr));
    it = m_words.insert({ wStr.str(), static_cast<int>(m_index[idx].size())-1 }).first;
  }
  m_index[idx][it->second].addUrlIndex(m_urlIndex,hiPriority);
  int i;
  bool found=FALSE;
  if (!recurse) // the first time we check if we can strip the prefix
  {
    i=getPrefixIndex(word);
    if (i>0)
    {
      addWord(word.data()+i,hiPriority,TRUE);
      found=TRUE;
    }
  }
  if (!found) // no prefix stripped
  {
    i=0;
    while (word[i]!=0 &&
           !((word[i]=='_' || word[i]==':' || (word[i]>='a' && word[i]<='z')) &&  // [_a-z:]
             (word[i+1]>='A' && word[i+1]<='Z')))                                 // [A-Z]
    {
      i++;
    }
    if (word[i]!=0 && i>=1)
    {
      addWord(word.data()+i+1,hiPriority,TRUE);
    }
  }
}

void SearchIndex::addWord(const QCString &word,bool hiPriority)
{
  addWord(word,hiPriority,FALSE);
}

static void writeInt(std::ostream &f,size_t index)
{
  f.put(static_cast<int>(index>>24));
  f.put(static_cast<int>((index>>16)&0xff));
  f.put(static_cast<int>((index>>8)&0xff));
  f.put(static_cast<int>(index&0xff));
}

static void writeString(std::ostream &f,const QCString &s)
{
  uint l = s.length();
  for (uint i=0;i<l;i++) f.put(s[i]);
  f.put(0);
}

void SearchIndex::write(const QCString &fileName)
{
  size_t i;
  size_t size=4; // for the header
  size+=4*numIndexEntries; // for the index
  size_t wordsOffset = size;
  // first pass: compute the size of the wordlist
  for (i=0;i<numIndexEntries;i++)
  {
    const auto &wlist = m_index[i];
    if (!wlist.empty())
    {
      for (const auto &iw : wlist)
      {
        int ws = iw.word().length()+1;
        size+=ws+4; // word + url info list offset
      }
      size+=1; // zero list terminator
    }
  }

  // second pass: compute the offsets in the index
  size_t indexOffsets[numIndexEntries];
  size_t offset=wordsOffset;
  for (i=0;i<numIndexEntries;i++)
  {
    const auto &wlist = m_index[i];
    if (!wlist.empty())
    {
      indexOffsets[i]=offset;
      for (const auto &iw : wlist)
      {
        offset+= iw.word().length()+1;
        offset+=4; // word + offset to url info array
      }
      offset+=1; // zero list terminator
    }
    else
    {
      indexOffsets[i]=0;
    }
  }
  size_t padding = size;
  size = (size+3)&~3; // round up to 4 byte boundary
  padding = size - padding;

  std::vector<size_t> wordStatOffsets(m_words.size());

  int count=0;

  // third pass: compute offset to stats info for each word
  for (i=0;i<numIndexEntries;i++)
  {
    const auto &wlist = m_index[i];
    if (!wlist.empty())
    {
      for (const auto &iw : wlist)
      {
        //printf("wordStatOffsets[%d]=%d\n",count,size);
        wordStatOffsets[count++] = size;
        size+=4 + iw.urls().size() * 8; // count + (url_index,freq) per url
      }
    }
  }
  std::vector<size_t> urlOffsets(m_urls.size());
  for (const auto &udi : m_urls)
  {
    urlOffsets[udi.first]=size;
    size+=udi.second.name.length()+1+
          udi.second.url.length()+1;
  }

  //printf("Total size %x bytes (word=%x stats=%x urls=%x)\n",size,wordsOffset,statsOffset,urlsOffset);
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (f.is_open())
  {
    // write header
    f.put('D'); f.put('O'); f.put('X'); f.put('S');
    // write index
    for (i=0;i<numIndexEntries;i++)
    {
      writeInt(f,indexOffsets[i]);
    }
    // write word lists
    count=0;
    for (i=0;i<numIndexEntries;i++)
    {
      const auto &wlist = m_index[i];
      if (!wlist.empty())
      {
        for (const auto &iw : wlist)
        {
          writeString(f,iw.word());
          writeInt(f,wordStatOffsets[count++]);
        }
        f.put(0);
      }
    }
    // write extra padding bytes
    for (i=0;i<padding;i++) f.put(0);
    // write word statistics
    for (i=0;i<numIndexEntries;i++)
    {
      const auto &wlist = m_index[i];
      if (!wlist.empty())
      {
        for (const auto &iw : wlist)
        {
          size_t numUrls = iw.urls().size();
          writeInt(f,numUrls);
          for (const auto &ui : iw.urls())
          {
            writeInt(f,urlOffsets[ui.second.urlIdx]);
            writeInt(f,ui.second.freq);
          }
        }
      }
    }
    // write urls
    for (const auto &udi : m_urls)
    {
      writeString(f,udi.second.name);
      writeString(f,udi.second.url);
    }
  }

}


//---------------------------------------------------------------------------
// the following part is for writing an external search index

struct SearchDocEntry
{
  QCString type;
  QCString name;
  QCString args;
  QCString extId;
  QCString url;
  GrowBuf  importantText;
  GrowBuf  normalText;
};

struct SearchIndexExternal::Private
{
  std::map<std::string,SearchDocEntry> docEntries;
  SearchDocEntry *current = 0;
};

SearchIndexExternal::SearchIndexExternal() : SearchIndexIntf(External), p(std::make_unique<Private>())
{
}

static QCString definitionToName(const Definition *ctx)
{
  if (ctx && ctx->definitionType()==Definition::TypeMember)
  {
    const MemberDef *md = toMemberDef(ctx);
    if (md->isFunction())
      return "function";
    else if (md->isSlot())
      return "slot";
    else if (md->isSignal())
      return "signal";
    else if (md->isVariable())
      return "variable";
    else if (md->isTypedef())
      return "typedef";
    else if (md->isEnumerate())
      return "enum";
    else if (md->isEnumValue())
      return "enumvalue";
    else if (md->isProperty())
      return "property";
    else if (md->isEvent())
      return "event";
    else if (md->isRelated() || md->isForeign())
      return "related";
    else if (md->isFriend())
      return "friend";
    else if (md->isDefine())
      return "define";
  }
  else if (ctx)
  {
    switch(ctx->definitionType())
    {
      case Definition::TypeClass:
        return (toClassDef(ctx))->compoundTypeString();
      case Definition::TypeFile:
        return "file";
      case Definition::TypeNamespace:
        return "namespace";
      case Definition::TypeConcept:
        return "concept";
      case Definition::TypeGroup:
        return "group";
      case Definition::TypePackage:
        return "package";
      case Definition::TypePage:
        return "page";
      case Definition::TypeDir:
        return "dir";
      default:
        break;
    }
  }
  return "unknown";
}

void SearchIndexExternal::setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
{
  QCString extId = stripPath(Config_getString(EXTERNAL_SEARCH_ID));
  QCString baseName = isSourceFile ? (toFileDef(ctx))->getSourceFileBase() : ctx->getOutputFileBase();
  QCString url = addHtmlExtensionIfMissing(baseName);
  if (!anchor.isEmpty()) url+=QCString("#")+anchor;
  QCString key = extId+";"+url;

  auto it = p->docEntries.find(key.str());
  if (it == p->docEntries.end())
  {
    SearchDocEntry e;
    e.type = isSourceFile ? QCString("source") : definitionToName(ctx);
    e.name = ctx->qualifiedName();
    if (ctx->definitionType()==Definition::TypeMember)
    {
      e.args = (toMemberDef(ctx))->argsString();
    }
    e.extId = extId;
    e.url  = url;
    it = p->docEntries.insert({key.str(),e}).first;
    //printf("searchIndexExt %s : %s\n",qPrint(e->name),qPrint(e->url));
  }
  p->current = &it->second;
}

void SearchIndexExternal::addWord(const QCString &word,bool hiPriority)
{
  if (word.isEmpty() || !isId(word[0]) || p->current==0) return;
  GrowBuf *pText = hiPriority ? &p->current->importantText : &p->current->normalText;
  if (pText->getPos()>0) pText->addChar(' ');
  pText->addStr(word);
  //printf("addWord %s\n",word);
}

void SearchIndexExternal::write(const QCString &fileName)
{
  std::ofstream t(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (t.is_open())
  {
    t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    t << "<add>\n";
    for (auto &kv : p->docEntries)
    {
      SearchDocEntry &doc = kv.second;
      doc.normalText.addChar(0);    // make sure buffer ends with a 0 terminator
      doc.importantText.addChar(0); // make sure buffer ends with a 0 terminator
      t << "  <doc>\n";
      t << "    <field name=\"type\">"     << doc.type << "</field>\n";
      t << "    <field name=\"name\">"     << convertToXML(doc.name) << "</field>\n";
      if (!doc.args.isEmpty())
      {
        t << "    <field name=\"args\">"     << convertToXML(doc.args) << "</field>\n";
      }
      if (!doc.extId.isEmpty())
      {
        t << "    <field name=\"tag\">"      << convertToXML(doc.extId)  << "</field>\n";
      }
      t << "    <field name=\"url\">"      << convertToXML(doc.url)  << "</field>\n";
      t << "    <field name=\"keywords\">" << convertToXML(doc.importantText.get())  << "</field>\n";
      t << "    <field name=\"text\">"     << convertToXML(doc.normalText.get())     << "</field>\n";
      t << "  </doc>\n";
    }
    t << "</add>\n";
  }
  else
  {
    err("Failed to open file %s for writing!\n",qPrint(fileName));
  }
}

//---------------------------------------------------------------------------------------------
// the following part is for the javascript based search engine
//---------------------------------------------------------------------------------------------

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

      QCString fileName = searchDirName + "/"+baseName+Doxygen::htmlFileExtension;
      QCString dataFileName = searchDirName + "/"+baseName+".js";

      std::ofstream t(fileName.str(), std::ofstream::out | std::ofstream::binary);
      std::ofstream ti(dataFileName.str(), std::ofstream::out | std::ofstream::binary);
      if (t.is_open() && ti.is_open())
      {
        {
          t << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
            " \"https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
          t << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
          t << "<head><title></title>\n";
          t << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>\n";
          t << "<meta name=\"generator\" content=\"Doxygen " << getDoxygenVersion() << "\"/>\n";
          t << "<link rel=\"stylesheet\" type=\"text/css\" href=\"search.css\"/>\n";
          t << "<script type=\"text/javascript\" src=\"" << baseName << ".js\"></script>\n";
          t << "<script type=\"text/javascript\" src=\"search.js\"></script>\n";
          t << "</head>\n";
          t << "<body class=\"SRPage\">\n";
          t << "<div id=\"SRIndex\">\n";
          t << "<div class=\"SRStatus\" id=\"Loading\">" << theTranslator->trLoading() << "</div>\n";
          t << "<div id=\"SRResults\"></div>\n"; // here the results will be inserted
          t << "<script type=\"text/javascript\">\n";
          t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
          t << "createResults();\n"; // this function will insert the results
          t << "/* @license-end */\n";
          t << "</script>\n";
          t << "<div class=\"SRStatus\" id=\"Searching\">"
            << theTranslator->trSearching() << "</div>\n";
          t << "<div class=\"SRStatus\" id=\"NoMatches\">"
            << theTranslator->trNoMatches() << "</div>\n";

          t << "<script type=\"text/javascript\">\n";
          t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
          t << "document.getElementById(\"Loading\").style.display=\"none\";\n";
          t << "document.getElementById(\"NoMatches\").style.display=\"none\";\n";
          t << "var searchResults = new SearchResults(\"searchResults\");\n";
          t << "searchResults.Search();\n";
          t << "window.addEventListener(\"message\", function(event) {\n";
          t << "  if (event.data == \"take_focus\") {\n";
          t << "    var elem = searchResults.NavNext(0);\n";
          t << "    if (elem) elem.focus();\n";
          t << "  }\n";
          t << "});\n";
          t << "/* @license-end */\n";
          t << "</script>\n";
          t << "</div>\n"; // SRIndex
          t << "</body>\n";
          t << "</html>\n";
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
        err("Failed to open file '%s' for writing...\n",qPrint(fileName));
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

  {
    QCString noMatchesFileName =searchDirName+"/nomatches"+Doxygen::htmlFileExtension;
    std::ofstream t(noMatchesFileName.str(), std::ofstream::out | std::ofstream::binary);
    if (t.is_open())
    {
      t << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
           "\"https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
      t << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
      t << "<head><title></title>\n";
      t << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>\n";
      t << "<link rel=\"stylesheet\" type=\"text/css\" href=\"search.css\"/>\n";
      t << "<script type=\"text/javascript\" src=\"search.js\"></script>\n";
      t << "</head>\n";
      t << "<body class=\"SRPage\">\n";
      t << "<div id=\"SRIndex\">\n";
      t << "<div class=\"SRStatus\" id=\"NoMatches\">"
        << theTranslator->trNoMatches() << "</div>\n";
      t << "</div>\n";
      t << "</body>\n";
      t << "</html>\n";
    }
  }

  Doxygen::indexList->addStyleSheetFile("search/search.js");
}

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

//---------------------------------------------------------------------------------------------

void initSearchIndexer()
{
  bool searchEngine      = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  bool externalSearch    = Config_getBool(EXTERNAL_SEARCH);
  if (searchEngine && serverBasedSearch)
  {
    if (externalSearch) // external tools produce search index and engine
    {
      Doxygen::searchIndex = new SearchIndexExternal;
    }
    else // doxygen produces search index and engine
    {
      Doxygen::searchIndex = new SearchIndex;
    }
  }
  else // no search engine or pure javascript based search function
  {
    Doxygen::searchIndex = 0;
  }
}

void finalizeSearchIndexer()
{
  delete Doxygen::searchIndex;
}

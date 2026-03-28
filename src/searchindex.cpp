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
#include <mutex>
#include <map>
#include <unordered_map>

#include "searchindex.h"

#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "language.h"
#include "pagedef.h"
#include "message.h"
#include "groupdef.h"
#include "filedef.h"
#include "portable.h"


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

static std::mutex g_searchIndexMutex;

//--------------------------------------------------------------------

void SearchIndex::IndexWord::addUrlIndex(int idx,bool hiPriority)
{
  //printf("IndexWord::addUrlIndex(%d,%d)\n",idx,hiPriority);
  auto it = m_urls.find(idx);
  if (it==m_urls.end())
  {
    //printf("URLInfo::URLInfo(%d)\n",idx);
    it = m_urls.emplace(idx,URLInfo(idx,0)).first;
  }
  it->second.freq+=2;
  if (hiPriority) it->second.freq|=1; // mark as high priority document
}

//--------------------------------------------------------------------

SearchIndex::SearchIndex()
{
  m_index.resize(numIndexEntries);
}

void SearchIndex::setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
{
  if (ctx==nullptr) return;
  std::lock_guard<std::mutex> lock(g_searchIndexMutex);
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
    name.prepend((md->getLanguage()==SrcLangExt::Fortran  ?
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
          if (lang==SrcLangExt::Java || lang==SrcLangExt::CSharp)
          {
            name = theTranslator->trPackage(name);
          }
          else if (lang==SrcLangExt::Fortran)
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
      case Definition::TypeModule:
        {
          name.prepend(theTranslator->trModule(TRUE,TRUE)+" ");
        }
        break;
      default:
        break;
    }
  }

  auto it = m_url2IdMap.find(baseUrl.str());
  if (it == m_url2IdMap.end()) // new entry
  {
    m_urlIndex = m_urlMaxIndex++;
    m_url2IdMap.emplace(baseUrl.str(),m_urlIndex);
    m_urls.emplace(m_urlIndex,URL(name,url));
  }
  else // existing entry
  {
    m_urlIndex=it->second;
    m_urls.emplace(it->second,URL(name,url));
  }
}

static int charsToIndex(const QCString &word)
{
  if (word.length()<2) return -1;

  // Fast string hashing algorithm
  //register uint16_t h=0;
  //const char *k = word;
  //uint16_t mask=0xfc00;
  //while ( *k )
  //{
  //  h = (h&mask)^(h<<6)^(*k++);
  //}
  //return h;

  // Simple hashing that allows for substring searching
  uint32_t c1=static_cast<uint8_t>(word[0]);
  uint32_t c2=static_cast<uint8_t>(word[1]);
  return c1*256+c2;
}

void SearchIndex::addWordRec(const QCString &word,bool hiPriority,bool recurse)
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
    m_index[idx].emplace_back(wStr);
    it = m_words.emplace( wStr.str(), static_cast<int>(m_index[idx].size())-1 ).first;
  }
  m_index[idx][it->second].addUrlIndex(m_urlIndex,hiPriority);
  bool found=FALSE;
  if (!recurse) // the first time we check if we can strip the prefix
  {
    int i=getPrefixIndex(word);
    if (i>0)
    {
      addWordRec(word.data()+i,hiPriority,TRUE);
      found=TRUE;
    }
  }
  if (!found) // no prefix stripped
  {
    int i=0;
    while (word[i]!=0 &&
           !((word[i]=='_' || word[i]==':' || (word[i]>='a' && word[i]<='z')) &&  // [_a-z:]
             (word[i+1]>='A' && word[i+1]<='Z')))                                 // [A-Z]
    {
      i++;
    }
    if (word[i]!=0 && i>=1)
    {
      addWordRec(word.data()+i+1,hiPriority,TRUE);
    }
  }
}

void SearchIndex::addWord(const QCString &word,bool hiPriority)
{
  std::lock_guard<std::mutex> lock(g_searchIndexMutex);
  addWordRec(word,hiPriority,FALSE);
}

static void writeInt(std::ostream &f,size_t index)
{
  f.put(static_cast<int>((index>>24)&0xff));
  f.put(static_cast<int>((index>>16)&0xff));
  f.put(static_cast<int>((index>>8)&0xff));
  f.put(static_cast<int>(index&0xff));
}

static void writeString(std::ostream &f,const QCString &s)
{
  size_t l = s.length();
  for (size_t i=0;i<l;i++) f.put(s[i]);
  f.put(0);
}

void SearchIndex::write(const QCString &fileName)
{
  size_t size=4; // for the header
  size+=4*numIndexEntries; // for the index
  size_t wordsOffset = size;
  // first pass: compute the size of the wordlist
  for (size_t i=0;i<numIndexEntries;i++)
  {
    const auto &wlist = m_index[i];
    if (!wlist.empty())
    {
      for (const auto &iw : wlist)
      {
        size_t ws = iw.word().length()+1;
        size+=ws+4; // word + url info list offset
      }
      size+=1; // zero list terminator
    }
  }

  // second pass: compute the offsets in the index
  size_t indexOffsets[numIndexEntries];
  size_t offset=wordsOffset;
  for (size_t i=0;i<numIndexEntries;i++)
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
  for (size_t i=0;i<numIndexEntries;i++)
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
  std::ofstream f = Portable::openOutputStream(fileName);
  if (f.is_open())
  {
    // write header
    f.put('D'); f.put('O'); f.put('X'); f.put('S');
    // write index
    for (size_t i=0;i<numIndexEntries;i++)
    {
      writeInt(f,indexOffsets[i]);
    }
    // write word lists
    count=0;
    for (size_t i=0;i<numIndexEntries;i++)
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
    for (size_t i=0;i<padding;i++) f.put(0);
    // write word statistics
    for (size_t i=0;i<numIndexEntries;i++)
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

SearchIndexExternal::SearchIndexExternal()
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
      case Definition::TypeModule:
        return "module";
      default:
        break;
    }
  }
  return "unknown";
}

void SearchIndexExternal::setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
{
  std::lock_guard<std::mutex> lock(g_searchIndexMutex);
  QCString extId = stripPath(Config_getString(EXTERNAL_SEARCH_ID));
  QCString url = isSourceFile ? (toFileDef(ctx))->getSourceFileBase() : ctx->getOutputFileBase();
  addHtmlExtensionIfMissing(url);
  if (!anchor.isEmpty()) url+=QCString("#")+anchor;
  QCString key = extId+";"+url;

  auto it = m_docEntries.find(key.str());
  if (it == m_docEntries.end())
  {
    SearchDocEntry e;
    e.type = isSourceFile ? QCString("source") : definitionToName(ctx);
    e.name = ctx->qualifiedName();
    if (ctx->definitionType()==Definition::TypeMember)
    {
      e.args = (toMemberDef(ctx))->argsString();
    }
    else if (ctx->definitionType()==Definition::TypeGroup)
    {
      const GroupDef *gd = toGroupDef(ctx);
      if (!gd->groupTitle().isEmpty())
      {
        e.name = filterTitle(gd->groupTitle());
      }
    }
    else if (ctx->definitionType()==Definition::TypePage)
    {
      const PageDef *pd = toPageDef(ctx);
      if (pd->hasTitle())
      {
        e.name = filterTitle(pd->title());
      }
    }
    e.extId = extId;
    e.url  = url;
    it = m_docEntries.emplace(key.str(),e).first;
    //printf("searchIndexExt %s : %s\n",qPrint(e->name),qPrint(e->url));
  }
  m_current = &it->second;
}

void SearchIndexExternal::addWord(const QCString &word,bool hiPriority)
{
  std::lock_guard<std::mutex> lock(g_searchIndexMutex);
  if (word.isEmpty() || !isId(word[0]) || m_current==nullptr) return;
  QCString &text = hiPriority ? m_current->importantText : m_current->normalText;
  if (!text.isEmpty()) text+=' ';
  text+=word;
  //printf("addWord %s\n",word);
}

void SearchIndexExternal::write(const QCString &fileName)
{
  std::ofstream t = Portable::openOutputStream(fileName);
  if (t.is_open())
  {
    t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    t << "<add>\n";
    for (auto &[name,doc] : m_docEntries)
    {
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
      t << "    <field name=\"keywords\">" << convertToXML(doc.importantText)  << "</field>\n";
      t << "    <field name=\"text\">"     << convertToXML(doc.normalText)     << "</field>\n";
      t << "  </doc>\n";
    }
    t << "</add>\n";
  }
  else
  {
    err("Failed to open file {} for writing!\n",fileName);
  }
}

//---------------------------------------------------------------------------------------------

void initSearchIndexer()
{
  bool searchEngine      = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  bool externalSearch    = Config_getBool(EXTERNAL_SEARCH);
  if (searchEngine && serverBasedSearch)
  {
    Doxygen::searchIndex.setKind(externalSearch ? SearchIndexIntf::External : SearchIndexIntf::Internal);
  }
}

void finalizeSearchIndexer()
{
  Doxygen::searchIndex.setKind(SearchIndexIntf::Disabled);
}



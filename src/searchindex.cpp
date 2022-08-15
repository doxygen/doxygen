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
#include <mutex>
#include <map>
#include <unordered_map>

#include "searchindex.h"

#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "language.h"
#include "pagedef.h"
#include "growbuf.h"
#include "message.h"
#include "groupdef.h"
#include "filedef.h"


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

void SearchIndex::IndexWord::addUrlIndex(int idx,bool hiPriority)
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

SearchIndex::SearchIndex()
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
      addWordRec(word.data()+i,hiPriority,TRUE);
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
      addWordRec(word.data()+i+1,hiPriority,TRUE);
    }
  }
}

void SearchIndex::addWord(const QCString &word,bool hiPriority)
{
  addWordRec(word,hiPriority,FALSE);
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

static std::mutex g_transferSearchIndexMutex;

void SIDataCollection::transfer()
{
  if (Doxygen::searchIndex)
  {
    std::lock_guard<std::mutex> lock(g_transferSearchIndexMutex);
    for (const auto &v : m_data)
    {
      if (std::holds_alternative<SIData_Word>(v))
      {
        const auto &d = std::get<SIData_Word>(v);
        Doxygen::searchIndex->addWord(d.word,d.hiPrio);
      }
      else if (std::holds_alternative<SIData_CurrentDoc>(v))
      {
        const auto &d = std::get<SIData_CurrentDoc>(v);
        Doxygen::searchIndex->setCurrentDoc(d.ctx,d.anchor,d.isSourceFile);
      }
    }
  }
  m_data.clear();
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
    e.extId = extId;
    e.url  = url;
    it = m_docEntries.insert({key.str(),e}).first;
    //printf("searchIndexExt %s : %s\n",qPrint(e->name),qPrint(e->url));
  }
  m_current = &it->second;
}

void SearchIndexExternal::addWord(const QCString &word,bool hiPriority)
{
  if (word.isEmpty() || !isId(word[0]) || m_current==0) return;
  GrowBuf *pText = hiPriority ? &m_current->importantText : &m_current->normalText;
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
    for (auto &kv : m_docEntries)
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

void initSearchIndexer()
{
  bool searchEngine      = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  bool externalSearch    = Config_getBool(EXTERNAL_SEARCH);
  if (searchEngine && serverBasedSearch)
  {
    Doxygen::searchIndex = new SearchIndexIntf(externalSearch ? SearchIndexIntf::External : SearchIndexIntf::Internal);
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

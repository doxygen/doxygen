/******************************************************************************
 *
 * 
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

#include <ctype.h>
#include <assert.h>

#include <qfile.h>
#include <qregexp.h>

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
#include "classlist.h"
#include "filedef.h"
#include "memberdef.h"
#include "filename.h"
#include "membername.h"
#include "resourcemgr.h"

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

const int numIndexEntries = 256*256;

//--------------------------------------------------------------------

IndexWord::IndexWord(const char *word) : m_word(word), m_urls(17)
{
  m_urls.setAutoDelete(TRUE);
  //printf("IndexWord::IndexWord(%s)\n",word);
}

void IndexWord::addUrlIndex(int idx,bool hiPriority)
{
  //printf("IndexWord::addUrlIndex(%d,%d)\n",idx,hiPriority);
  URLInfo *ui = m_urls.find(idx);
  if (ui==0)
  {
    //printf("URLInfo::URLInfo(%d)\n",idx);
    ui=new URLInfo(idx,0);
    m_urls.insert(idx,ui);
  }
  ui->freq+=2;
  if (hiPriority) ui->freq|=1; // mark as high priority document
}

//--------------------------------------------------------------------

SearchIndex::SearchIndex() : SearchIndexIntf(Internal), 
      m_words(328829), m_index(numIndexEntries), m_url2IdMap(10007), m_urls(10007), m_urlIndex(-1)
{
  int i;
  m_words.setAutoDelete(TRUE);
  m_url2IdMap.setAutoDelete(TRUE);
  m_urls.setAutoDelete(TRUE);
  m_index.setAutoDelete(TRUE);
  for (i=0;i<numIndexEntries;i++) m_index.insert(i,new QList<IndexWord>);
}

void SearchIndex::setCurrentDoc(Definition *ctx,const char *anchor,bool isSourceFile)
{
  if (ctx==0) return;
  assert(!isSourceFile || ctx->definitionType()==Definition::TypeFile);
  //printf("SearchIndex::setCurrentDoc(%s,%s,%s)\n",name,baseName,anchor);
  QCString url=isSourceFile ? ((FileDef*)ctx)->getSourceFileBase() : ctx->getOutputFileBase();
  url+=Config_getString(HTML_FILE_EXTENSION);
  QCString baseUrl = url;
  if (anchor) url+=QCString("#")+anchor;  
  if (!isSourceFile) baseUrl=url;
  QCString name=ctx->qualifiedName();
  if (ctx->definitionType()==Definition::TypeMember)
  {
    MemberDef *md = (MemberDef *)ctx;
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
          PageDef *pd = (PageDef *)ctx;
          if (!pd->title().isEmpty())
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
          ClassDef *cd = (ClassDef *)ctx;
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
          GroupDef *gd = (GroupDef *)ctx;
          if (gd->groupTitle())
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

  int *pIndex = m_url2IdMap.find(baseUrl);
  if (pIndex==0)
  {
    ++m_urlIndex;
    m_url2IdMap.insert(baseUrl,new int(m_urlIndex));
    m_urls.insert(m_urlIndex,new URL(name,url));
  }
  else
  {
    m_urls.insert(*pIndex,new URL(name,url));
  }
}

static int charsToIndex(const char *word)
{
  if (word==0) return -1;

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
  uint c1=((uchar *)word)[0];
  if (c1==0) return -1;
  uint c2=((uchar *)word)[1];
  if (c2==0) return -1;
  return c1*256+c2;
}

void SearchIndex::addWord(const char *word,bool hiPriority,bool recurse)
{
  static QRegExp nextPart("[_a-z:][A-Z]");
  if (word==0 || word[0]=='\0') return;
  QCString wStr = QCString(word).lower();
  //printf("SearchIndex::addWord(%s,%d) wStr=%s\n",word,hiPriority,wStr.data());
  IndexWord *w = m_words[wStr];
  if (w==0)
  {
    int idx=charsToIndex(wStr);
    //fprintf(stderr,"addWord(%s) at index %d\n",word,idx);
    if (idx<0) return;
    w = new IndexWord(wStr);
    m_index[idx]->append(w);
    m_words.insert(wStr,w);
  }
  w->addUrlIndex(m_urlIndex,hiPriority);
  int i;
  bool found=FALSE;
  if (!recurse) // the first time we check if we can strip the prefix
  {
    i=getPrefixIndex(word);
    if (i>0)
    {
      addWord(word+i,hiPriority,TRUE);
      found=TRUE;
    }
  }
  if (!found) // no prefix stripped
  {
    if ((i=nextPart.match(word))>=1)
    {
      addWord(word+i+1,hiPriority,TRUE);
    }
  }
}

void SearchIndex::addWord(const char *word,bool hiPriority)
{
  addWord(word,hiPriority,FALSE);
}

static void writeInt(QFile &f,int index)
{
  f.putch(((uint)index)>>24);
  f.putch((((uint)index)>>16)&0xff);
  f.putch((((uint)index)>>8)&0xff);
  f.putch(((uint)index)&0xff);
}

static void writeString(QFile &f,const char *s)
{
  const char *p = s;
  while (*p) f.putch(*p++);
  f.putch(0);
}

void SearchIndex::write(const char *fileName)
{
  int i;
  int size=4; // for the header
  size+=4*numIndexEntries; // for the index
  int wordsOffset = size;
  // first pass: compute the size of the wordlist
  for (i=0;i<numIndexEntries;i++)
  {
    QList<IndexWord> *wlist = m_index[i];
    if (!wlist->isEmpty())
    {
      QListIterator<IndexWord> iwi(*wlist);
      IndexWord *iw;
      for (iwi.toFirst();(iw=iwi.current());++iwi)
      {
        int ws = iw->word().length()+1; 
        size+=ws+4; // word + url info list offset
      }
      size+=1; // zero list terminator
    }
  }

  // second pass: compute the offsets in the index
  int indexOffsets[numIndexEntries];
  int offset=wordsOffset;
  for (i=0;i<numIndexEntries;i++)
  {
    QList<IndexWord> *wlist = m_index[i];
    if (!wlist->isEmpty())
    {
      indexOffsets[i]=offset;
      QListIterator<IndexWord> iwi(*wlist);
      IndexWord *iw;
      for (iwi.toFirst();(iw=iwi.current());++iwi)
      {
        offset+= iw->word().length()+1; 
        offset+=4; // word + offset to url info array 
      }
      offset+=1; // zero list terminator
    }
    else
    {
      indexOffsets[i]=0;
    }
  }
  int padding = size;
  size = (size+3)&~3; // round up to 4 byte boundary
  padding = size - padding;

  //int statsOffset = size;
  //IndexWord *iw;
  int *wordStatOffsets = new int[m_words.count()];
  
  int count=0;

  // third pass: compute offset to stats info for each word
  for (i=0;i<numIndexEntries;i++)
  {
    QList<IndexWord> *wlist = m_index[i];
    if (!wlist->isEmpty())
    {
      QListIterator<IndexWord> iwi(*wlist);
      IndexWord *iw;
      for (iwi.toFirst();(iw=iwi.current());++iwi)
      {
        //printf("wordStatOffsets[%d]=%d\n",count,size);
        wordStatOffsets[count++] = size;
        size+=4+iw->urls().count()*8; // count + (url_index,freq) per url
      }
    }
  }
  int *urlOffsets = new int[m_urls.count()];
  //int urlsOffset = size;
  QIntDictIterator<URL> udi(m_urls);
  URL *url;
  for (udi.toFirst();(url=udi.current());++udi)
  {
    urlOffsets[udi.currentKey()]=size;
    size+=url->name.length()+1+
          url->url.length()+1;
  }
  //printf("Total size %x bytes (word=%x stats=%x urls=%x)\n",size,wordsOffset,statsOffset,urlsOffset);
  QFile f(fileName);
  if (f.open(IO_WriteOnly))
  {
    // write header
    f.putch('D'); f.putch('O'); f.putch('X'); f.putch('S');
    // write index
    for (i=0;i<numIndexEntries;i++)
    {
      writeInt(f,indexOffsets[i]);
    }
    // write word lists
    count=0;
    for (i=0;i<numIndexEntries;i++)
    {
      QList<IndexWord> *wlist = m_index[i];
      if (!wlist->isEmpty())
      {
        QListIterator<IndexWord> iwi(*wlist);
        IndexWord *iw;
        for (iwi.toFirst();(iw=iwi.current());++iwi)
        {
          writeString(f,iw->word());
          writeInt(f,wordStatOffsets[count++]);
        }
        f.putch(0);
      }
    }
    // write extra padding bytes
    for (i=0;i<padding;i++) f.putch(0);
    // write word statistics
    for (i=0;i<numIndexEntries;i++)
    {
      QList<IndexWord> *wlist = m_index[i];
      if (!wlist->isEmpty())
      {
        QListIterator<IndexWord> iwi(*wlist);
        IndexWord *iw;
        for (iwi.toFirst();(iw=iwi.current());++iwi)
        {
          int numUrls = iw->urls().count();
          writeInt(f,numUrls);
          QIntDictIterator<URLInfo> uli(iw->urls());
          URLInfo *ui;
          for (uli.toFirst();(ui=uli.current());++uli)
          {
            writeInt(f,urlOffsets[ui->urlIdx]);
            writeInt(f,ui->freq);
          }
        }
      }
    }
    // write urls
    QIntDictIterator<URL> udi(m_urls);
    URL *url;
    for (udi.toFirst();(url=udi.current());++udi)
    {
      writeString(f,url->name);
      writeString(f,url->url);
    }
  }

  delete[] urlOffsets;
  delete[] wordStatOffsets;
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
  Private() : docEntries(12251) {}
  SDict<SearchDocEntry> docEntries;
  SearchDocEntry *current;
};

SearchIndexExternal::SearchIndexExternal() : SearchIndexIntf(External)
{
  p = new SearchIndexExternal::Private;
  p->docEntries.setAutoDelete(TRUE);
  p->current=0;
}

SearchIndexExternal::~SearchIndexExternal()
{
  //printf("p->docEntries.count()=%d\n",p->docEntries.count());
  delete p;
}

static QCString definitionToName(Definition *ctx)
{
  if (ctx && ctx->definitionType()==Definition::TypeMember)
  {
    MemberDef *md = (MemberDef*)ctx;
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
        return ((ClassDef*)ctx)->compoundTypeString();
      case Definition::TypeFile:
        return "file";
      case Definition::TypeNamespace:
        return "namespace";
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

void SearchIndexExternal::setCurrentDoc(Definition *ctx,const char *anchor,bool isSourceFile)
{
  QCString extId = stripPath(Config_getString(EXTERNAL_SEARCH_ID));
  QCString baseName = isSourceFile ? ((FileDef*)ctx)->getSourceFileBase() : ctx->getOutputFileBase();
  QCString url = baseName + Doxygen::htmlFileExtension;
  if (anchor) url+=QCString("#")+anchor;
  QCString key = extId+";"+url;

  p->current = p->docEntries.find(key);
  //printf("setCurrentDoc(url=%s,isSourceFile=%d) current=%p\n",url.data(),isSourceFile,p->current);
  if (!p->current)
  {
    SearchDocEntry *e = new SearchDocEntry;
    e->type = isSourceFile ? QCString("source") : definitionToName(ctx);
    e->name = ctx->qualifiedName();
    if (ctx->definitionType()==Definition::TypeMember)
    {
      e->args = ((MemberDef*)ctx)->argsString();
    }
    e->extId = extId;
    e->url  = url;
    p->current = e;
    p->docEntries.append(key,e);
    //printf("searchIndexExt %s : %s\n",e->name.data(),e->url.data());
  }
}

void SearchIndexExternal::addWord(const char *word,bool hiPriority)
{
  if (word==0 || !isId(*word) || p->current==0) return;
  GrowBuf *pText = hiPriority ? &p->current->importantText : &p->current->normalText;
  if (pText->getPos()>0) pText->addChar(' ');
  pText->addStr(word);
  //printf("addWord %s\n",word);
}

void SearchIndexExternal::write(const char *fileName)
{
  QFile f(fileName);
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    t << "<add>" << endl;
    SDict<SearchDocEntry>::Iterator it(p->docEntries);
    SearchDocEntry *doc;
    for (it.toFirst();(doc=it.current());++it)
    {
      doc->normalText.addChar(0);    // make sure buffer ends with a 0 terminator
      doc->importantText.addChar(0); // make sure buffer ends with a 0 terminator
      t << "  <doc>" << endl;
      t << "    <field name=\"type\">"     << doc->type << "</field>" << endl;
      t << "    <field name=\"name\">"     << convertToXML(doc->name) << "</field>" << endl;
      if (!doc->args.isEmpty())
      {
        t << "    <field name=\"args\">"     << convertToXML(doc->args) << "</field>" << endl;
      }
      if (!doc->extId.isEmpty())
      {
        t << "    <field name=\"tag\">"      << convertToXML(doc->extId)  << "</field>" << endl;
      }
      t << "    <field name=\"url\">"      << convertToXML(doc->url)  << "</field>" << endl;
      t << "    <field name=\"keywords\">" << convertToXML(doc->importantText.get())  << "</field>" << endl;
      t << "    <field name=\"text\">"     << convertToXML(doc->normalText.get())     << "</field>" << endl;
      t << "  </doc>" << endl;
    }
    t << "</add>" << endl;
  }
  else
  {
    err("Failed to open file %s for writing!\n",fileName);
  }
}

//---------------------------------------------------------------------------
// the following part is for the javascript based search engine

#include "memberdef.h"
#include "namespacedef.h"
#include "pagedef.h"
#include "classdef.h"
#include "filedef.h"
#include "language.h"
#include "doxygen.h"
#include "message.h"

static SearchIndexInfo g_searchIndexInfo[NUM_SEARCH_INDICES];

static void addMemberToSearchIndex(MemberDef *md)
{
  static bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isLinkable = md->isLinkable();
  ClassDef *cd=0;
  NamespaceDef *nd=0;
  FileDef *fd=0;
  GroupDef *gd=0;
  if (isLinkable &&
      (
       ((cd=md->getClassDef()) && cd->isLinkable() && cd->templateMaster()==0) ||
       ((gd=md->getGroupDef()) && gd->isLinkable())
      )
     )
  {
    QCString n = md->name();
    if (!n.isEmpty())
    {
      uint letter = getUtf8CodeToLower(n,0);
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" ||
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,md);
      }
      if (md->isFunction() || md->isSlot() || md->isSignal())
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].symbolList.append(letter,md);
      }
      else if (md->isVariable())
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].symbolList.append(letter,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].symbolList.append(letter,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].symbolList.append(letter,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].symbolList.append(letter,md);
      }
      else if (md->isProperty())
      {
        g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].symbolList.append(letter,md);
      }
      else if (md->isEvent())
      {
        g_searchIndexInfo[SEARCH_INDEX_EVENTS].symbolList.append(letter,md);
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        g_searchIndexInfo[SEARCH_INDEX_RELATED].symbolList.append(letter,md);
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
      uint letter = getUtf8CodeToLower(n,0);
      g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,md);

      if (md->isFunction()) 
      {
        g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].symbolList.append(letter,md);
      }
      else if (md->isVariable()) 
      {
        g_searchIndexInfo[SEARCH_INDEX_VARIABLES].symbolList.append(letter,md);
      }
      else if (md->isTypedef())
      {
        g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].symbolList.append(letter,md);
      }
      else if (md->isEnumerate())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMS].symbolList.append(letter,md);
      }
      else if (md->isEnumValue())
      {
        g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].symbolList.append(letter,md);
      }
      else if (md->isDefine())
      {
        g_searchIndexInfo[SEARCH_INDEX_DEFINES].symbolList.append(letter,md);
      }
    }
  }
}

// see also function convertToId() in search.js, which should match in
// behaviour
static QCString searchId(const QCString &s)
{
  int c;
  uint i;
  QCString result;
  for (i=0;i<s.length();i++)
  {
    c=s.at(i);
    if (c>0x7f || c<0) // part of multibyte character
    {
      result+=(char)c;
    }
    else if (isalnum(c)) // simply alpha numerical character
    {
      result+=(char)tolower(c);
    }
    else // other 'unprintable' characters
    {
      char val[4];
      sprintf(val,"_%02x",(uchar)c);
      result+=val;
    }
  }
  return result;
}

void createJavascriptSearchIndex()
{
  // set index names
  g_searchIndexInfo[SEARCH_INDEX_ALL].name        = "all";
  g_searchIndexInfo[SEARCH_INDEX_CLASSES].name    = "classes";
  g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].name = "namespaces";
  g_searchIndexInfo[SEARCH_INDEX_FILES].name      = "files";
  g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].name  = "functions";
  g_searchIndexInfo[SEARCH_INDEX_VARIABLES].name  = "variables";
  g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].name   = "typedefs";
  g_searchIndexInfo[SEARCH_INDEX_ENUMS].name      = "enums";
  g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].name = "enumvalues";
  g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].name = "properties";
  g_searchIndexInfo[SEARCH_INDEX_EVENTS].name     = "events";
  g_searchIndexInfo[SEARCH_INDEX_RELATED].name    = "related";
  g_searchIndexInfo[SEARCH_INDEX_DEFINES].name    = "defines";
  g_searchIndexInfo[SEARCH_INDEX_GROUPS].name     = "groups";
  g_searchIndexInfo[SEARCH_INDEX_PAGES].name      = "pages";

  // set index texts
  g_searchIndexInfo[SEARCH_INDEX_ALL].text        = theTranslator->trAll();
  g_searchIndexInfo[SEARCH_INDEX_CLASSES].text    = theTranslator->trClasses();
  g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].text = theTranslator->trNamespace(TRUE,FALSE);
  g_searchIndexInfo[SEARCH_INDEX_FILES].text      = theTranslator->trFile(TRUE,FALSE);
  g_searchIndexInfo[SEARCH_INDEX_FUNCTIONS].text  = theTranslator->trFunctions();
  g_searchIndexInfo[SEARCH_INDEX_VARIABLES].text  = theTranslator->trVariables();
  g_searchIndexInfo[SEARCH_INDEX_TYPEDEFS].text   = theTranslator->trTypedefs();
  g_searchIndexInfo[SEARCH_INDEX_ENUMS].text      = theTranslator->trEnumerations();
  g_searchIndexInfo[SEARCH_INDEX_ENUMVALUES].text = theTranslator->trEnumerationValues();
  g_searchIndexInfo[SEARCH_INDEX_PROPERTIES].text = theTranslator->trProperties();
  g_searchIndexInfo[SEARCH_INDEX_EVENTS].text     = theTranslator->trEvents();
  g_searchIndexInfo[SEARCH_INDEX_RELATED].text    = theTranslator->trFriends();
  g_searchIndexInfo[SEARCH_INDEX_DEFINES].text    = theTranslator->trDefines();
  g_searchIndexInfo[SEARCH_INDEX_GROUPS].text     = theTranslator->trGroup(TRUE,FALSE);
  g_searchIndexInfo[SEARCH_INDEX_PAGES].text      = theTranslator->trPage(TRUE,FALSE);

  // add symbols to letter -> symbol list map

  // index classes
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    uint letter = getUtf8CodeToLower(cd->localName(),0);
    if (cd->isLinkable() && isId(letter))
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,cd);
      g_searchIndexInfo[SEARCH_INDEX_CLASSES].symbolList.append(letter,cd);
    }
  }

  // index namespaces
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    uint letter = getUtf8CodeToLower(nd->name(),0);
    if (nd->isLinkable() && isId(letter))
    {
      g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,nd);
      g_searchIndexInfo[SEARCH_INDEX_NAMESPACES].symbolList.append(letter,nd);
    }
  }

  // index files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      uint letter = getUtf8CodeToLower(fd->name(),0);
      if (fd->isLinkable() && isId(letter))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,fd);
        g_searchIndexInfo[SEARCH_INDEX_FILES].symbolList.append(letter,fd);
      }
    }
  }

  // index class members
  {
    MemberNameSDict::Iterator mnli(*Doxygen::memberNameSDict);
    MemberName *mn;
    // for each member name
    for (mnli.toFirst();(mn=mnli.current());++mnli)
    {
      MemberDef *md;
      MemberNameIterator mni(*mn);
      // for each member definition
      for (mni.toFirst();(md=mni.current());++mni)
      {
        addMemberToSearchIndex(md);
      }
    }
  }

  // index file/namespace members
  {
    MemberNameSDict::Iterator fnli(*Doxygen::functionNameSDict);
    MemberName *mn;
    // for each member name
    for (fnli.toFirst();(mn=fnli.current());++fnli)
    {
      MemberDef *md;
      MemberNameIterator mni(*mn);
      // for each member definition
      for (mni.toFirst();(md=mni.current());++mni)
      {
        addMemberToSearchIndex(md);
      }
    }
  }

  // index groups
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    if (gd->isLinkable())
    {
      QCString title = gd->groupTitle();
      if (!title.isEmpty()) // TODO: able searching for all word in the title
      {
        uchar charCode = title.at(0);
        uint letter = charCode<128 ? tolower(charCode) : charCode;
        if (isId(letter))
        {
          g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,gd);
          g_searchIndexInfo[SEARCH_INDEX_GROUPS].symbolList.append(letter,gd);
        }
      }
    }
  }

  // index pages
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (pd->isLinkable())
    {
      QCString title = pd->title();
      if (!title.isEmpty())
      {
        uchar charCode = title.at(0);
        uint letter = charCode<128 ? tolower(charCode) : charCode;
        if (isId(letter))
        {
          g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,pd);
          g_searchIndexInfo[SEARCH_INDEX_PAGES].symbolList.append(letter,pd);
        }
      }
    }
  }
  if (Doxygen::mainPage)
  {
    QCString title = Doxygen::mainPage->title();
    if (!title.isEmpty())
    {
      uchar charCode = title.at(0);
      uint letter = charCode<128 ? tolower(charCode) : charCode;
      if (isId(letter))
      {
        g_searchIndexInfo[SEARCH_INDEX_ALL].symbolList.append(letter,Doxygen::mainPage);
        g_searchIndexInfo[SEARCH_INDEX_PAGES].symbolList.append(letter,Doxygen::mainPage);
      }
    }
  }

  // sort all lists
  int i;
  for (i=0;i<NUM_SEARCH_INDICES;i++)
  {
    SIntDict<SearchIndexList>::Iterator it(g_searchIndexInfo[i].symbolList);
    SearchIndexList *sl;
    for (it.toFirst();(sl=it.current());++it)
    {
      sl->sort();
    }
  }
}

void writeJavascriptSearchIndex()
{
  int i;
  // write index files
  QCString searchDirName = Config_getString(HTML_OUTPUT)+"/search";

  for (i=0;i<NUM_SEARCH_INDICES;i++) // for each index
  {
    SIntDict<SearchIndexList>::Iterator it(g_searchIndexInfo[i].symbolList);
    SearchIndexList *sl;
    int p=0;
    for (it.toFirst();(sl=it.current());++it,++p) // for each letter
    {
      QCString baseName;
      baseName.sprintf("%s_%x",g_searchIndexInfo[i].name.data(),p);

      QCString fileName = searchDirName + "/"+baseName+".html";
      QCString dataFileName = searchDirName + "/"+baseName+".js";

      QFile outFile(fileName);
      QFile dataOutFile(dataFileName);
      if (outFile.open(IO_WriteOnly) && dataOutFile.open(IO_WriteOnly))
      {
        {
          FTextStream t(&outFile);

          t << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
          t << "<html><head><title></title>" << endl;
          t << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>" << endl;
          t << "<meta name=\"generator\" content=\"Doxygen " << versionString << "\"/>" << endl;
          t << "<link rel=\"stylesheet\" type=\"text/css\" href=\"search.css\"/>" << endl;
          t << "<script type=\"text/javascript\" src=\"" << baseName << ".js\"></script>" << endl;
          t << "<script type=\"text/javascript\" src=\"search.js\"></script>" << endl;
          t << "</head>" << endl;
          t << "<body class=\"SRPage\">" << endl;
          t << "<div id=\"SRIndex\">" << endl;
          t << "<div class=\"SRStatus\" id=\"Loading\">" << theTranslator->trLoading() << "</div>" << endl;
          t << "<div id=\"SRResults\"></div>" << endl; // here the results will be inserted
          t << "<script type=\"text/javascript\"><!--" << endl;
          t << "createResults();" << endl; // this function will insert the results
          t << "--></script>" << endl;
          t << "<div class=\"SRStatus\" id=\"Searching\">" 
            << theTranslator->trSearching() << "</div>" << endl;
          t << "<div class=\"SRStatus\" id=\"NoMatches\">"
            << theTranslator->trNoMatches() << "</div>" << endl;

          t << "<script type=\"text/javascript\"><!--" << endl;
          t << "document.getElementById(\"Loading\").style.display=\"none\";" << endl;
          t << "document.getElementById(\"NoMatches\").style.display=\"none\";" << endl;
          t << "var searchResults = new SearchResults(\"searchResults\");" << endl;
          t << "searchResults.Search();" << endl;
          t << "--></script>" << endl;
          t << "</div>" << endl; // SRIndex
          t << "</body>" << endl;
          t << "</html>" << endl;
        }
        FTextStream ti(&dataOutFile);

        ti << "var searchData=" << endl;
        // format
        // searchData[] = array of items
        // searchData[x][0] = id
        // searchData[x][1] = [ name + child1 + child2 + .. ]
        // searchData[x][1][0] = name as shown
        // searchData[x][1][y+1] = info for child y
        // searchData[x][1][y+1][0] = url
        // searchData[x][1][y+1][1] = 1 => target="_parent"
        // searchData[x][1][y+1][2] = scope

        ti << "[" << endl;
        bool firstEntry=TRUE;

        SDict<SearchDefinitionList>::Iterator li(*sl);
        SearchDefinitionList *dl;
        int itemCount=0;
        for (li.toFirst();(dl=li.current());++li)
        {
          Definition *d = dl->getFirst();

          if (!firstEntry)
          {
            ti << "," << endl;
          }
          firstEntry=FALSE;

          ti << "  ['" << dl->id() << "',['" << convertToXML(dl->name()) << "',[";

          if (dl->count()==1) // item with a unique name
          {
            MemberDef  *md   = 0;
            bool isMemberDef = d->definitionType()==Definition::TypeMember;
            if (isMemberDef) md = (MemberDef*)d;
            QCString anchor = d->anchor();

            ti << "'" << externalRef("../",d->getReference(),TRUE)
              << d->getOutputFileBase() << Doxygen::htmlFileExtension;
            if (!anchor.isEmpty())
            {
              ti << "#" << anchor;
            }
            ti << "',";

            static bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
            if (!extLinksInWindow || d->getReference().isEmpty())
            {
              ti << "1,";
            }
            else
            {
              ti << "0,";
            }

            if (d->getOuterScope()!=Doxygen::globalScope)
            {
              ti << "'" << convertToXML(d->getOuterScope()->name()) << "'";
            }
            else if (md)
            {
              FileDef *fd = md->getBodyDef();
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
            ti << "]]";
          }
          else // multiple items with the same name
          {
            QListIterator<Definition> di(*dl);
            bool overloadedFunction = FALSE;
            Definition *prevScope = 0;
            int childCount=0;
            for (di.toFirst();(d=di.current());)
            {
              ++di;
              Definition *scope     = d->getOuterScope();
              Definition *next      = di.current();
              Definition *nextScope = 0;
              MemberDef  *md        = 0;
              bool isMemberDef = d->definitionType()==Definition::TypeMember;
              if (isMemberDef) md = (MemberDef*)d;
              if (next) nextScope = next->getOuterScope();
              QCString anchor = d->anchor();

              if (childCount>0)
              {
                ti << "],[";
              }
              ti << "'" << externalRef("../",d->getReference(),TRUE)
                << d->getOutputFileBase() << Doxygen::htmlFileExtension;
              if (!anchor.isEmpty())
              {
                ti << "#" << anchor;
              }
              ti << "',";

              static bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
              if (!extLinksInWindow || d->getReference().isEmpty())
              {
                ti << "1,";
              }
              else
              {
                ti << "0,";
              }
              bool found=FALSE;
              overloadedFunction = ((prevScope!=0 && scope==prevScope) ||
                  (scope && scope==nextScope)
                  ) && md && 
                (md->isFunction() || md->isSlot());
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
                name = convertToXML(((ClassDef*)d)->displayName());
                found = TRUE;
              }
              else if (d->definitionType()==Definition::TypeNamespace)
              {
                name = convertToXML(((NamespaceDef*)d)->displayName());
                found = TRUE;
              }
              else if (scope==0 || scope==Doxygen::globalScope) // in global scope
              {
                if (md)
                {
                  FileDef *fd = md->getBodyDef();
                  if (fd==0) fd = md->getFileDef();
                  if (fd)
                  {
                    if (!prefix.isEmpty()) prefix+=":&#160;";
                    name = prefix + convertToXML(fd->localName());
                    found = TRUE;
                  }
                }
              }
              else if (md && (md->getClassDef() || md->getNamespaceDef())) 
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

            ti << "]]";
          }
          ti << "]";
          itemCount++;
        }
        if (!firstEntry)
        {
          ti << endl;
        }

        ti << "];" << endl;

      }
      else
      {
        err("Failed to open file '%s' for writing...\n",fileName.data());
      }
    }
  }

  {
    QFile f(searchDirName+"/searchdata.js");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << "var indexSectionsWithContent =" << endl;
      t << "{" << endl;
      bool first=TRUE;
      int j=0;
      for (i=0;i<NUM_SEARCH_INDICES;i++)
      {
        if (g_searchIndexInfo[i].symbolList.count()>0)
        {
          if (!first) t << "," << endl;
          t << "  " << j << ": \"";

          SIntDict<SearchIndexList>::Iterator it(g_searchIndexInfo[i].symbolList);
          SearchIndexList *sl;
          for (it.toFirst();(sl=it.current());++it) // for each letter
          {
            t << QString( QChar( sl->letter() ) ).utf8();
          }
          t << "\"";
          first=FALSE;
          j++;
        }
      }
      if (!first) t << "\n";
      t << "};" << endl << endl;
      t << "var indexSectionNames =" << endl;
      t << "{" << endl;
      first=TRUE;
      j=0;
      for (i=0;i<NUM_SEARCH_INDICES;i++)
      {
        if (g_searchIndexInfo[i].symbolList.count()>0)
        {
          if (!first) t << "," << endl;
          t << "  " << j << ": \"" << g_searchIndexInfo[i].name << "\"";
          first=FALSE;
          j++;
        }
      }
      if (!first) t << "\n";
      t << "};" << endl << endl;
      t << "var indexSectionLabels =" << endl;
      t << "{" << endl;
      first=TRUE;
      j=0;
      for (i=0;i<NUM_SEARCH_INDICES;i++)
      {
        if (g_searchIndexInfo[i].symbolList.count()>0)
        {
          if (!first) t << "," << endl;
          t << "  " << j << ": \"" << convertToXML(g_searchIndexInfo[i].text) << "\"";
          first=FALSE;
          j++;
        }
      }
      if (!first) t << "\n";
      t << "};" << endl << endl;
    }
    ResourceMgr::instance().copyResource("search.js",searchDirName);
  }
  {
    QFile f(searchDirName+"/nomatches.html");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
           "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
      t << "<html><head><title></title>" << endl;
      t << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>" << endl;
      t << "<link rel=\"stylesheet\" type=\"text/css\" href=\"search.css\"/>" << endl;
      t << "<script type=\"text/javascript\" src=\"search.js\"></script>" << endl;
      t << "</head>" << endl;
      t << "<body class=\"SRPage\">" << endl;
      t << "<div id=\"SRIndex\">" << endl;
      t << "<div class=\"SRStatus\" id=\"NoMatches\">"
        << theTranslator->trNoMatches() << "</div>" << endl;
      t << "</div>" << endl;
      t << "</body>" << endl;
      t << "</html>" << endl;
    }
  }
  Doxygen::indexList->addStyleSheetFile("search/search.js");
}

const SearchIndexInfo *getSearchIndices()
{
  return g_searchIndexInfo;
}

//---------------------------------------------------------------------------------------------

SearchIndexList::SearchIndexList(uint letter)
  : SDict< SearchDefinitionList >(17,FALSE), m_letter(letter)
{
  setAutoDelete(TRUE);
}

SearchIndexList::~SearchIndexList()
{
}

void SearchIndexList::append(Definition *d)
{
  SearchDefinitionList *l = find(d->name());
  if (l==0)
  {
    QCString dispName = d->localName();
    if (d->definitionType()==Definition::TypeGroup)
    {
      dispName = ((GroupDef*)d)->groupTitle();
    }
    else if (d->definitionType()==Definition::TypePage)
    {
      dispName = ((PageDef*)d)->title();
    }
    l=new SearchDefinitionList(searchId(dispName),dispName);
    SDict< SearchDefinitionList >::append(d->name(),l);
  }
  l->append(d);
}

uint SearchIndexList::letter() const
{
  return m_letter;
}

int SearchIndexList::compareValues(const SearchDefinitionList *md1, const SearchDefinitionList *md2) const
{
  QCString n1 = md1->getFirst()->localName();
  QCString n2 = md2->getFirst()->localName();
  return qstricmp(n1.data(),n2.data());
}

//---------------------------------------------------------------------------------------------

void initSearchIndexer()
{
  static bool searchEngine      = Config_getBool(SEARCHENGINE);
  static bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  static bool externalSearch    = Config_getBool(EXTERNAL_SEARCH);
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

void finializeSearchIndexer()
{
  delete Doxygen::searchIndex;
}



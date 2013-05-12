/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
  url+=Config_getString("HTML_FILE_EXTENSION");
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
  QDictIterator<IndexWord> wdi(m_words);
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
  if (ctx->definitionType()==Definition::TypeMember)
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
  QCString extId = stripPath(Config_getString("EXTERNAL_SEARCH_ID"));
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

static const char search_script[]=
#include "search_js.h"
;

#define MEMBER_INDEX_ENTRIES   256

#define SEARCH_INDEX_ALL         0
#define SEARCH_INDEX_CLASSES     1
#define SEARCH_INDEX_NAMESPACES  2
#define SEARCH_INDEX_FILES       3
#define SEARCH_INDEX_FUNCTIONS   4
#define SEARCH_INDEX_VARIABLES   5
#define SEARCH_INDEX_TYPEDEFS    6
#define SEARCH_INDEX_ENUMS       7
#define SEARCH_INDEX_ENUMVALUES  8
#define SEARCH_INDEX_PROPERTIES  9
#define SEARCH_INDEX_EVENTS     10
#define SEARCH_INDEX_RELATED    11
#define SEARCH_INDEX_DEFINES    12
#define SEARCH_INDEX_GROUPS     13
#define SEARCH_INDEX_PAGES      14
#define NUM_SEARCH_INDICES      15

class SearchIndexList : public SDict< QList<Definition> >
{
  public:
    SearchIndexList(int size=17) : SDict< QList<Definition> >(size,FALSE) 
    {
      setAutoDelete(TRUE);
    }
   ~SearchIndexList() {}
    void append(Definition *d)
    {
      QList<Definition> *l = find(d->name());
      if (l==0)
      {
        l=new QList<Definition>;
        SDict< QList<Definition> >::append(d->name(),l);
      }
      l->append(d);
    }
    int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      QList<Definition> *md1=(QList<Definition> *)item1;
      QList<Definition> *md2=(QList<Definition> *)item2;
      QCString n1 = md1->first()->localName();
      QCString n2 = md2->first()->localName();
      return qstricmp(n1.data(),n2.data());
    }
};

static void addMemberToSearchIndex(
         SearchIndexList symbols[NUM_SEARCH_INDICES][MEMBER_INDEX_ENTRIES],
         int symbolCount[NUM_SEARCH_INDICES],
         MemberDef *md)
{
  static bool hideFriendCompounds = Config_getBool("HIDE_FRIEND_COMPOUNDS");
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
    uchar charCode = (uchar)n.at(0);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (!n.isEmpty()) 
    {
      bool isFriendToHide = hideFriendCompounds &&
        (QCString(md->typeString())=="friend class" || 
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union");
      if (!(md->isFriend() && isFriendToHide))
      {
        symbols[SEARCH_INDEX_ALL][letter].append(md);
        symbolCount[SEARCH_INDEX_ALL]++;
      }
      if (md->isFunction() || md->isSlot() || md->isSignal())
      {
        symbols[SEARCH_INDEX_FUNCTIONS][letter].append(md);
        symbolCount[SEARCH_INDEX_FUNCTIONS]++;
      } 
      else if (md->isVariable())
      {
        symbols[SEARCH_INDEX_VARIABLES][letter].append(md);
        symbolCount[SEARCH_INDEX_VARIABLES]++;
      }
      else if (md->isTypedef())
      {
        symbols[SEARCH_INDEX_TYPEDEFS][letter].append(md);
        symbolCount[SEARCH_INDEX_TYPEDEFS]++;
      }
      else if (md->isEnumerate())
      {
        symbols[SEARCH_INDEX_ENUMS][letter].append(md);
        symbolCount[SEARCH_INDEX_ENUMS]++;
      }
      else if (md->isEnumValue())
      {
        symbols[SEARCH_INDEX_ENUMVALUES][letter].append(md);
        symbolCount[SEARCH_INDEX_ENUMVALUES]++;
      }
      else if (md->isProperty())
      {
        symbols[SEARCH_INDEX_PROPERTIES][letter].append(md);
        symbolCount[SEARCH_INDEX_PROPERTIES]++;
      }
      else if (md->isEvent())
      {
        symbols[SEARCH_INDEX_EVENTS][letter].append(md);
        symbolCount[SEARCH_INDEX_EVENTS]++;
      }
      else if (md->isRelated() || md->isForeign() ||
               (md->isFriend() && !isFriendToHide))
      {
        symbols[SEARCH_INDEX_RELATED][letter].append(md);
        symbolCount[SEARCH_INDEX_RELATED]++;
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
    uchar charCode = (uchar)n.at(0);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (!n.isEmpty()) 
    {
      symbols[SEARCH_INDEX_ALL][letter].append(md);
      symbolCount[SEARCH_INDEX_ALL]++;

      if (md->isFunction()) 
      {
        symbols[SEARCH_INDEX_FUNCTIONS][letter].append(md);
        symbolCount[SEARCH_INDEX_FUNCTIONS]++;
      }
      else if (md->isVariable()) 
      {
        symbols[SEARCH_INDEX_VARIABLES][letter].append(md);
        symbolCount[SEARCH_INDEX_VARIABLES]++;
      }
      else if (md->isTypedef())
      {
        symbols[SEARCH_INDEX_TYPEDEFS][letter].append(md);
        symbolCount[SEARCH_INDEX_TYPEDEFS]++;
      }
      else if (md->isEnumerate())
      {
        symbols[SEARCH_INDEX_ENUMS][letter].append(md);
        symbolCount[SEARCH_INDEX_ENUMS]++;
      }
      else if (md->isEnumValue())
      {
        symbols[SEARCH_INDEX_ENUMVALUES][letter].append(md);
        symbolCount[SEARCH_INDEX_ENUMVALUES]++;
      }
      else if (md->isDefine())
      {
        symbols[SEARCH_INDEX_DEFINES][letter].append(md);
        symbolCount[SEARCH_INDEX_DEFINES]++;
      }
    }
  }
}

static QCString searchId(const QCString &s)
{
  int c;
  uint i;
  QCString result;
  for (i=0;i<s.length();i++)
  {
    c=s.at(i);
    if ((c>='0' && c<='9') || (c>='A' && c<='Z') || (c>='a' && c<='z'))
    {
      result+=(char)tolower(c);
    }
    else
    {
      char val[4];
      sprintf(val,"_%02x",(uchar)c);
      result+=val;
    }
  }
  return result;
}

static int g_searchIndexCount[NUM_SEARCH_INDICES];
static SearchIndexList g_searchIndexSymbols[NUM_SEARCH_INDICES][MEMBER_INDEX_ENTRIES];
static const char *g_searchIndexName[NUM_SEARCH_INDICES] = 
{ 
    "all",
    "classes",
    "namespaces",
    "files",
    "functions",
    "variables",
    "typedefs", 
    "enums", 
    "enumvalues",
    "properties", 
    "events", 
    "related",
    "defines",
    "groups",
    "pages"
};


class SearchIndexCategoryMapping
{
  public:
    SearchIndexCategoryMapping()
    {
      categoryLabel[SEARCH_INDEX_ALL]        = theTranslator->trAll();
      categoryLabel[SEARCH_INDEX_CLASSES]    = theTranslator->trClasses();
      categoryLabel[SEARCH_INDEX_NAMESPACES] = theTranslator->trNamespace(TRUE,FALSE);
      categoryLabel[SEARCH_INDEX_FILES]      = theTranslator->trFile(TRUE,FALSE);
      categoryLabel[SEARCH_INDEX_FUNCTIONS]  = theTranslator->trFunctions();
      categoryLabel[SEARCH_INDEX_VARIABLES]  = theTranslator->trVariables();
      categoryLabel[SEARCH_INDEX_TYPEDEFS]   = theTranslator->trTypedefs();
      categoryLabel[SEARCH_INDEX_ENUMS]      = theTranslator->trEnumerations();
      categoryLabel[SEARCH_INDEX_ENUMVALUES] = theTranslator->trEnumerationValues();
      categoryLabel[SEARCH_INDEX_PROPERTIES] = theTranslator->trProperties();
      categoryLabel[SEARCH_INDEX_EVENTS]     = theTranslator->trEvents();
      categoryLabel[SEARCH_INDEX_RELATED]    = theTranslator->trFriends();
      categoryLabel[SEARCH_INDEX_DEFINES]    = theTranslator->trDefines();
      categoryLabel[SEARCH_INDEX_GROUPS]     = theTranslator->trGroup(TRUE,FALSE);
      categoryLabel[SEARCH_INDEX_PAGES]      = theTranslator->trPage(TRUE,FALSE);
    }
    QCString categoryLabel[NUM_SEARCH_INDICES];
};

void writeJavascriptSearchIndex()
{
  if (!Config_getBool("GENERATE_HTML")) return;

  // index classes
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    uchar charCode = (uchar)cd->localName().at(0);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (cd->isLinkable() && isId(letter))
    {
      g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(cd);
      g_searchIndexSymbols[SEARCH_INDEX_CLASSES][letter].append(cd);
      g_searchIndexCount[SEARCH_INDEX_ALL]++;
      g_searchIndexCount[SEARCH_INDEX_CLASSES]++;
    }
  }

  // index namespaces
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    uchar charCode = (uchar)nd->name().at(0);
    uint letter = charCode<128 ? tolower(charCode) : charCode;
    if (nd->isLinkable() && isId(letter))
    {
      g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(nd);
      g_searchIndexSymbols[SEARCH_INDEX_NAMESPACES][letter].append(nd);
      g_searchIndexCount[SEARCH_INDEX_ALL]++;
      g_searchIndexCount[SEARCH_INDEX_NAMESPACES]++;
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
      uchar charCode = (uchar)fd->name().at(0);
      uint letter = charCode<128 ? tolower(charCode) : charCode;
      if (fd->isLinkable() && isId(letter))
      {
        g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(fd);
        g_searchIndexSymbols[SEARCH_INDEX_FILES][letter].append(fd);
        g_searchIndexCount[SEARCH_INDEX_ALL]++;
        g_searchIndexCount[SEARCH_INDEX_FILES]++;
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
        addMemberToSearchIndex(g_searchIndexSymbols,g_searchIndexCount,md);
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
        addMemberToSearchIndex(g_searchIndexSymbols,g_searchIndexCount,md);
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
          g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(gd);
          g_searchIndexSymbols[SEARCH_INDEX_GROUPS][letter].append(gd);
          g_searchIndexCount[SEARCH_INDEX_ALL]++;
          g_searchIndexCount[SEARCH_INDEX_GROUPS]++;
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
          g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(pd);
          g_searchIndexSymbols[SEARCH_INDEX_PAGES][letter].append(pd);
          g_searchIndexCount[SEARCH_INDEX_ALL]++;
          g_searchIndexCount[SEARCH_INDEX_PAGES]++;
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
        g_searchIndexSymbols[SEARCH_INDEX_ALL][letter].append(Doxygen::mainPage);
        g_searchIndexSymbols[SEARCH_INDEX_PAGES][letter].append(Doxygen::mainPage);
        g_searchIndexCount[SEARCH_INDEX_ALL]++;
        g_searchIndexCount[SEARCH_INDEX_PAGES]++;
      }
    }
  }
  
  // sort all lists
  int i,p;
  for (i=0;i<NUM_SEARCH_INDICES;i++)
  {
    for (p=0;p<MEMBER_INDEX_ENTRIES;p++)
    {
      if (g_searchIndexSymbols[i][p].count()>0)
      {
        g_searchIndexSymbols[i][p].sort();
      }
    }
  }

  // write index files
  QCString searchDirName = Config_getString("HTML_OUTPUT")+"/search";

  for (i=0;i<NUM_SEARCH_INDICES;i++)
  {
    for (p=0;p<MEMBER_INDEX_ENTRIES;p++)
    {
      if (g_searchIndexSymbols[i][p].count()>0)
      {
        QCString baseName;
        baseName.sprintf("%s_%02x",g_searchIndexName[i],p);

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
            t << "<meta name=\"generator\" content=\"Doxygen " << versionString << "\">" << endl;
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

          SDict<QList<Definition> >::Iterator li(g_searchIndexSymbols[i][p]);
          QList<Definition> *dl;
          int itemCount=0;
          for (li.toFirst();(dl=li.current());++li)
          {
            Definition *d = dl->first();
            QCString id = d->localName();

            if (!firstEntry)
            {
              ti << "," << endl;
            }
            firstEntry=FALSE;

            QCString dispName = d->localName();
            if (d->definitionType()==Definition::TypeGroup)
            {
              dispName = ((GroupDef*)d)->groupTitle();
            }
            else if (d->definitionType()==Definition::TypePage)
            {
              dispName = ((PageDef*)d)->title();
            }
            ti << "  ['" << searchId(dispName) << "',['" 
               << convertToXML(dispName) << "',[";

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

              static bool extLinksInWindow = Config_getBool("EXT_LINKS_IN_WINDOW");
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

                static bool extLinksInWindow = Config_getBool("EXT_LINKS_IN_WINDOW");
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
  }

  {
    QFile f(searchDirName+"/search.js");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << "// Search script generated by doxygen" << endl;
      t << "// Copyright (C) 2009 by Dimitri van Heesch." << endl << endl;
      t << "// The code in this file is loosly based on main.js, part of Natural Docs," << endl;
      t << "// which is Copyright (C) 2003-2008 Greg Valure" << endl;
      t << "// Natural Docs is licensed under the GPL." << endl << endl;
      t << "var indexSectionsWithContent =" << endl;
      t << "{" << endl;
      bool first=TRUE;
      int j=0;
      for (i=0;i<NUM_SEARCH_INDICES;i++)
      {
        if (g_searchIndexCount[i]>0)
        {
          if (!first) t << "," << endl;
          t << "  " << j << ": \"";
          for (p=0;p<MEMBER_INDEX_ENTRIES;p++)
          {
            t << (g_searchIndexSymbols[i][p].count()>0 ? "1" : "0");
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
        if (g_searchIndexCount[i]>0)
        {
          if (!first) t << "," << endl;
          t << "  " << j << ": \"" << g_searchIndexName[i] << "\"";
          first=FALSE;
          j++;
        }
      }
      if (!first) t << "\n";
      t << "};" << endl << endl;
      t << search_script;
    }
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

void writeSearchCategories(FTextStream &t)
{
  static SearchIndexCategoryMapping map;
  int i,j=0;
  for (i=0;i<NUM_SEARCH_INDICES;i++)
  {
    if (g_searchIndexCount[i]>0)
    {
      t << "<a class=\"SelectItem\" href=\"javascript:void(0)\" "
        << "onclick=\"searchBox.OnSelectItem(" << j << ")\">"
        << "<span class=\"SelectionMark\">&#160;</span>"
        << convertToXML(map.categoryLabel[i])
        << "</a>";
      j++;
    }
  }
}

//---------------------------------------------------------------------------------------------

void initSearchIndexer()
{
  static bool searchEngine      = Config_getBool("SEARCHENGINE");
  static bool serverBasedSearch = Config_getBool("SERVER_BASED_SEARCH");
  static bool externalSearch    = Config_getBool("EXTERNAL_SEARCH");
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



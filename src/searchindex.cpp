/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#include "searchindex.h"
#include <qfile.h>

#if 0
#include "suffixtree.h"

SearchIndex::SearchIndex() : refDict(10007), nameIndex(10007)
{
  indexCount=1;
  //indexTree = new IndexTree;
  suffixTree = new SuffixTree;
}

SearchIndex::~SearchIndex()
{
  //delete indexTree;
  delete suffixTree;
}

void SearchIndex::addReference(const char *key,const char *ref)
{
  DocRef *dr=new DocRef(indexCount,key,ref);
  nameIndex.insert(indexCount,dr);
  refList.append(dr);
  refDict.insert(key,dr);
  indexCount++;
}

bool SearchIndex::addWord(const char *key,const char *word,bool special)
{
  DocRef *dr=0;
  if (word && key && strlen(key)>0 && (dr=refDict[key]))
  {
    suffixTree->insertWord(((QCString)word).lower(),dr->index(),special); 
    return TRUE; 
  }
  else if (word)
  {
    printf("SearchIndex::addWord() key `%s' not found!\n",key);
    return FALSE;
  }
  else
  {
    printf("SearchIndex::addWord() trying to insert word with length 0\n");
    return FALSE;
  }
}

bool SearchIndex::saveIndex(const char *fileName)
{
  QFile f(fileName);
  if (!f.open(IO_WriteOnly)) return FALSE;
  
  // write header
  if (f.writeBlock("DOXI",4)!=4) return FALSE; // write header
  
  // compute forward offsets for all children of each node.
  suffixTree->resolveForwardReferences();
  
  // compute offset to the reference index table
  int offset=suffixTree->size()+9;
  
  // write the offset
  if (writeNumber(f,offset)) return FALSE;
  
  // write the suffix tree
  if (!suffixTree->write(f)) return FALSE;
  
  f.putch(0);
  
  // write the index reference table
  DocRef *dr=refList.first();
  offset+=refList.count()*4;
  while (dr)
  {
    writeNumber(f,offset);
    offset+=strlen(dr->name())+strlen(dr->url())+2;
    dr=refList.next(); 
  }
  
  // write the references
  dr=refList.first();
  while (dr)
  {
    writeString(f,dr->name());
    writeString(f,dr->url());
    dr=refList.next();
  }

  //printf("Building index tree\n");
  printf("Size of the suffix tree is %d bytes\n",suffixTree->size());
  printf("Number of tree nodes is %d\n",suffixTree->numberOfNodes());
  printf("Number of links %d\n",indexCount);
  //suffixTree->buildIndex();
  //printf("Computing reference offsets\n");
  //int offset=suffixTree->size()+8;
  //indexTree->setOffsets(&nameIndex);
  //printf("Saving suffix tree\n");
  //printf("Saving index tree\n");
  //result&=indexTree->write(f);
  //printf("Saving reference list\n");
  //dr=refList.first();
  //while (dr)
  //{
  //  result&=!writeString(f,dr->name());
  //  result&=!writeString(f,dr->url());
  //  dr=refList.next();
  //}
  //suffixTree->dump();
  return TRUE;
}
#endif


// file format:
//   4 byte header
//   256*256*4 byte index
//   for each index entry: a zero terminated list of words 
//   for each word: a 0 terminated string + 4 bytes stats index
//   padding bytes to align at 4 byte boundary
//   for each word: a counter + for each url containing the word 8 bytes statistics
//   for each url: a 0 terminated string

const int numIndexEntries = 256*256;

//--------------------------------------------------------------------

IndexWord::IndexWord(const char *word) : m_word(word), m_urls(17)
{
  m_urls.setAutoDelete(TRUE);
}

void IndexWord::addUrlIndex(int idx)
{
  URLInfo *ui = m_urls.find(idx);
  if (ui==0)
  {
    ui=new URLInfo(idx,0);
    m_urls.insert(idx,ui);
  }
  ui->freq++;
}

//--------------------------------------------------------------------

SearchIndex::SearchIndex() : m_words(328829), m_index(numIndexEntries), m_urlIndex(-1)
{
  int i;
  m_words.setAutoDelete(TRUE);
  m_urls.setAutoDelete(TRUE);
  for (i=0;i<numIndexEntries;i++) m_index.insert(i,new QList<IndexWord>);
}

void SearchIndex::setCurrentDoc(const char *name,const char *url)
{
  m_urlIndex++;
  m_urls.insert(m_urlIndex,new URL(name,url));
}


static int charsToIndex(const char *word)
{
  if (word==0) return -1;
  int c1=word[0];
  if (c1==0) return -1;
  int c2=word[1];
  if (c2==0) return -1;
  return c1*256+c2;
}

void SearchIndex::addWord(const char *word)
{
  IndexWord *w = m_words[word];
  if (w==0)
  {
    int idx=charsToIndex(word);
    if (idx==-1) return;
    w = new IndexWord(word);
    //printf("addWord(%s) at index %d\n",word,idx);
    m_index[idx]->append(w);
    m_words.insert(word,w);
  }
  w->addUrlIndex(m_urlIndex);
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

  //for (wdi.toFirst();(iw=wdi.current());++wdi)
  //{
  //  printf("Word %s:\n",wdi.currentKey().data());
  //  QIntDictIterator<URLInfo> udi(iw->urls());
  //  URLInfo *ui;
  //  for (udi.toFirst();(ui=udi.current());++udi)
  //  {
  //    printf("  url[%d]=(name=%s,url=%s),freq=%d\n",
  //        ui->urlIdx,
  //        m_urls[ui->urlIdx]->name.data(),
  //        m_urls[ui->urlIdx]->url.data(),
  //        ui->freq);
  //  }
  //}

  delete urlOffsets;
  delete wordStatOffsets;
}


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

#ifndef _SEARCHINDEX_H
#define _SEARCHINDEX_H

#include "qtbc.h"
#include <qintdict.h>
#include <qlist.h>
#include <qdict.h>
#include <qintdict.h>
#include <qvector.h>


#if 0 // old version
#include "suffixtree.h"
//class IndexTree;
class SuffixTree;

class DocRef
{
  public:
    DocRef(int index,const char *name,const char *url) 
          { i=index; n=name; u=url; }
   ~DocRef() {}
    void setOffset(int offset) { o=offset; }
    const char *name() const { return n; }
    const char *url() const { return u; }
    int index() const { return i; }
    int offset() const { return o; }
   
  private:
    QCString n;
    QCString u;
    int i;
    int o;
};

typedef QList<DocRef> DocRefList;
typedef QDict<DocRef> DocRefDict;

class SearchIndex
{
  public:
    SearchIndex();
   ~SearchIndex();
    void addReference(const char *key,const char *ref);
    bool addWord(const char *key,const char *word,bool special);
    bool saveIndex(const char *fileName);
    void dump() { suffixTree->dump(); }
   
  private:
    //IndexTree  *indexTree;
    SuffixTree *suffixTree;
    DocRefList refList;
    DocRefDict refDict;
    QIntDict<DocRef> nameIndex;
    int indexCount;
};

#endif

struct URL
{
  URL(const char *n,const char *u) : name(n), url(u) {} 
  QCString name;
  QCString url;
};


struct URLInfo
{
  URLInfo(int idx,int f) : urlIdx(idx), freq(f) {}
  int urlIdx;
  int freq;
};

class IndexWord
{
  public:
    IndexWord(const char *word);
    void addUrlIndex(int);
    const QIntDict<URLInfo> &urls() const { return m_urls; }
    QCString word() const { return m_word; }

  private:
    QCString    m_word;
    QIntDict<URLInfo> m_urls;
};

class SearchIndex
{
  public:
    SearchIndex();
    void setCurrentDoc(const char *name,const char *url);
    void addWord(const char *word);
    void write(const char *file);
  private:
    QDict<IndexWord> m_words;
    QVector< QList<IndexWord> > m_index;
    QIntDict<URL>  m_urls;
    int m_urlIndex;
};


#endif

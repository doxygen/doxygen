/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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
    void addUrlIndex(int,bool);
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
    void setCurrentDoc(const char *name,const char *baseName,const char *anchor=0);
    void addWord(const char *word,bool hiPriority);
    void write(const char *file);
  private:
    QDict<IndexWord> m_words;
    QVector< QList<IndexWord> > m_index;
    QIntDict<URL>  m_urls;
    int m_urlIndex;
};


#endif

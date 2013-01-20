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

#ifndef _SEARCHINDEX_H
#define _SEARCHINDEX_H

#include <qintdict.h>
#include <qlist.h>
#include <qdict.h>
#include <qintdict.h>
#include <qvector.h>

class FTextStream;
class Definition;
class MemberDef;

/*! Initialize the search indexer */
void initSearchIndexer();
/*! Cleanup the search indexer */
void finializeSearchIndexer();

//------- server side search index ----------------------

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

class SearchIndexIntf
{
  public:
    enum Kind { Internal, External };
    SearchIndexIntf(Kind k) : m_kind(k) {}
    virtual ~SearchIndexIntf() {}
    virtual void setCurrentDoc(Definition *ctx,const char *anchor,bool isSourceFile) = 0;
    virtual void addWord(const char *word,bool hiPriority) = 0;
    virtual void write(const char *file) = 0;
    Kind kind() const { return m_kind; }
  private:
    Kind m_kind;
};

class SearchIndex : public SearchIndexIntf
{
  public:
    SearchIndex();
    void setCurrentDoc(Definition *ctx,const char *anchor,bool isSourceFile);
    void addWord(const char *word,bool hiPriority);
    void write(const char *file);
  private:
    void addWord(const char *word,bool hiPrio,bool recurse);
    QDict<IndexWord> m_words;
    QVector< QList<IndexWord> > m_index;
    QDict<int> m_url2IdMap;
    QIntDict<URL> m_urls;
    int m_urlIndex;
};


class SearchIndexExternal : public SearchIndexIntf
{
    struct Private;
  public:
    SearchIndexExternal();
   ~SearchIndexExternal();
    void setCurrentDoc(Definition *ctx,const char *anchor,bool isSourceFile);
    void addWord(const char *word,bool hiPriority);
    void write(const char *file);
  private:
    Private *p;
};

//------- client side search index ----------------------

void writeJavascriptSearchIndex();
void writeSearchCategories(FTextStream &t);

#endif

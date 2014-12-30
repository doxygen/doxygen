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

#ifndef _SEARCHINDEX_H
#define _SEARCHINDEX_H

#include <qintdict.h>
#include <qlist.h>
#include <qdict.h>
#include <qintdict.h>
#include <qvector.h>
#include "sortdict.h"
#include "definition.h"
#include "util.h"

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

class SearchDefinitionList : public QList<Definition>
{
  public:
    SearchDefinitionList(const QCString &id,const QCString &name) : m_id(id), m_name(name) {}
    QCString id() const   { return m_id;   }
    QCString name() const { return m_name; }
  private:
    QCString m_id;
    QCString m_name;
};

class SearchIndexList : public SDict< SearchDefinitionList >
{
  public:
    typedef Definition ElementType;
    SearchIndexList(uint letter);
   ~SearchIndexList();
    void append(Definition *d);
    uint letter() const;
  private:
    int compareValues(const SearchDefinitionList *md1, const SearchDefinitionList *md2) const;
    uint m_letter;
};

struct SearchIndexInfo
{
  LetterToIndexMap<SearchIndexList> symbolList;
  QCString name;
  QCString text;
};

void createJavascriptSearchIndex();
void writeJavascriptSearchIndex();
const SearchIndexInfo *getSearchIndices();

#endif

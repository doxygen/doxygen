/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

/** @file
 *  @brief Web server based search engine.
 *
 *  Comes in two flavors: internal (via generated index) or external (via doxyindexer + doxysearch)
 */

#ifndef SEARCHINDEX_H
#define SEARCHINDEX_H

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <array>
#include <variant>

#include "qcstring.h"
#include "growbuf.h"

class Definition;
class SearchIndexIntf;

/*! Initialize the search indexer */
void initSearchIndexer();
/*! Cleanup the search indexer */
void finalizeSearchIndexer();

// --- intermediate data collected by one thread ------

struct SIData_CurrentDoc
{
  SIData_CurrentDoc(const Definition *d,const QCString &a,bool b)
    : ctx(d), anchor(a), isSourceFile(b) {}
  const Definition *ctx = 0;
  QCString anchor;
  bool isSourceFile;
};

struct SIData_Word
{
  SIData_Word(const QCString &w,bool b)
    : word(w), hiPrio(b) {}
  QCString word;
  bool hiPrio;
};

// class to aggregate the search data collected on a worker thread
// and later transfer it to the search index on the main thread.
class SIDataCollection
{
  public:
    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
    {
      m_data.emplace_back(SIData_CurrentDoc(ctx,anchor,isSourceFile));
    }
    void addWord(const QCString &word,bool hiPriority)
    {
      m_data.emplace_back(SIData_Word(word,hiPriority));
    }

    // transfer the collected data into the given search index
    void transfer();

  private:
    using SIData = std::variant<SIData_CurrentDoc,SIData_Word>;
    std::vector<SIData> m_data;
};

//-----------------------------

/** Writes search index for doxygen provided server based search engine that uses PHP. */
class SearchIndex
{
    struct URL
    {
      URL(QCString n,QCString u) : name(n), url(u) {}
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
        using URLInfoMap = std::unordered_map<int,URLInfo>;
        IndexWord(QCString word) : m_word(word) {}
        void addUrlIndex(int,bool);
        URLInfoMap urls() const { return m_urls; }
        QCString word() const { return m_word; }

      private:
        QCString    m_word;
        URLInfoMap  m_urls;
    };

  public:
    SearchIndex();
    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile);
    void addWord(const QCString &word,bool hiPriority);
    void write(const QCString &file);
  private:
    void addWord(const QCString &word,bool hiPrio,bool recurse);
    std::unordered_map<std::string,int> m_words;
    std::vector< std::vector< IndexWord> > m_index;
    std::unordered_map<std::string,int> m_url2IdMap;
    std::map<int,URL> m_urls;
    int m_urlIndex = -1;
};

/** Writes search index that should be used with an externally provided search engine,
 *  e.g. doxyindexer and doxysearch.cgi.
 */
class SearchIndexExternal
{
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

  public:
    SearchIndexExternal();
    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile);
    void addWord(const QCString &word,bool hiPriority);
    void write(const QCString &file);
  private:
    //std::unique_ptr<Private> p;
    std::map<std::string,SearchDocEntry> m_docEntries;
    SearchDocEntry *m_current = 0;
};


/** Abstract proxy interface for non-javascript based search indices.
 *  It forwards calls to either SearchIndex or SearchIndexExternal depending
 *  on the Kind passed during construction.
 */
class SearchIndexIntf
{
  public:
    using SearchIndexVariant = std::variant<SearchIndex,SearchIndexExternal>;
    enum Kind { Internal, External };
    SearchIndexIntf(Kind k) : m_kind(k),
        m_variant(k==Internal ? SearchIndexVariant(SearchIndex()) :
                                SearchIndexVariant(SearchIndexExternal())) { }
    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
    {
      switch (m_kind)
      {
        case Internal: std::get<SearchIndex        >(m_variant).setCurrentDoc(ctx,anchor,isSourceFile); break;
        case External: std::get<SearchIndexExternal>(m_variant).setCurrentDoc(ctx,anchor,isSourceFile); break;
      }
    }
    void addWord(const QCString &word,bool hiPriority)
    {
      switch (m_kind)
      {
        case Internal: std::get<SearchIndex        >(m_variant).addWord(word,hiPriority); break;
        case External: std::get<SearchIndexExternal>(m_variant).addWord(word,hiPriority); break;
      }
    }
    void write(const QCString &file)
    {
      switch (m_kind)
      {
        case Internal: std::get<SearchIndex        >(m_variant).write(file); break;
        case External: std::get<SearchIndexExternal>(m_variant).write(file); break;
      }
    }
    Kind kind() const { return m_kind; }
  private:
    Kind m_kind;
    SearchIndexVariant m_variant;
};


#endif

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

class Definition;

/*! Initialize the search indexer */
void initSearchIndexer();
/*! Cleanup the search indexer */
void finalizeSearchIndexer();

// --- intermediate data collected by one thread ------

struct SIData_CurrentDoc
{
  SIData_CurrentDoc(const Definition *d,const QCString &a,bool b)
    : ctx(d), anchor(a), isSourceFile(b) {}
  const Definition *ctx = nullptr;
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

//-----------------------------

/** Writes search index for doxygen provided server based search engine that uses PHP. */
class SearchIndex
{
    struct URL
    {
      URL(const QCString &n,const QCString &u) : name(n), url(u) {}
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
        IndexWord(const QCString &word) : m_word(word) {}
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
    void addWordRec(const QCString &word,bool hiPrio,bool recurse);
    std::unordered_map<std::string,int> m_words;
    std::vector< std::vector< IndexWord> > m_index;
    std::unordered_map<std::string,int> m_url2IdMap;
    std::map<int,URL> m_urls;
    int m_urlIndex = -1;
    int m_urlMaxIndex = 0;
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
      QCString importantText;
      QCString normalText;
    };

  public:
    SearchIndexExternal();
    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile);
    void addWord(const QCString &word,bool hiPriority);
    void write(const QCString &file);
  private:
    std::map<std::string,SearchDocEntry> m_docEntries;
    SearchDocEntry *m_current = nullptr;
};

/** Abstract proxy interface for non-javascript based search indices.
 *  It forwards calls to either SearchIndex or SearchIndexExternal depending
 *  on the Kind passed during construction.
 */
class SearchIndexIntf
{
  public:
    using SearchIndexVariant = std::variant<std::monostate,SearchIndex,SearchIndexExternal>;
    enum Kind { Disabled, Internal, External };
    SearchIndexIntf() : m_kind(Disabled) {}
    bool enabled() const { return m_kind!=Disabled; }

    void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile)
    {
      if (std::holds_alternative<SearchIndex>(m_variant))
      {
        std::get<SearchIndex>(m_variant).setCurrentDoc(ctx,anchor,isSourceFile);
      }
      else if (std::holds_alternative<SearchIndexExternal>(m_variant))
      {
        std::get<SearchIndexExternal>(m_variant).setCurrentDoc(ctx,anchor,isSourceFile);
      }
    }
    void addWord(const QCString &word,bool hiPriority)
    {
      if (std::holds_alternative<SearchIndex>(m_variant))
      {
        std::get<SearchIndex>(m_variant).addWord(word,hiPriority);
      }
      else if (std::holds_alternative<SearchIndexExternal>(m_variant))
      {
        std::get<SearchIndexExternal>(m_variant).addWord(word,hiPriority);
      }
    }
    void write(const QCString &file)
    {
      if (std::holds_alternative<SearchIndex>(m_variant))
      {
        std::get<SearchIndex>(m_variant).write(file);
      }
      else if (std::holds_alternative<SearchIndexExternal>(m_variant))
      {
        std::get<SearchIndexExternal>(m_variant).write(file);
      }
    }
    void setKind(Kind k) {
      m_kind=k;
      switch (m_kind)
      {
        case Disabled: m_variant = std::monostate();      break;
        case Internal: m_variant = SearchIndex();         break;
        case External: m_variant = SearchIndexExternal(); break;
      }
    }
    Kind kind() const { return m_kind; }
  private:
    Kind m_kind;
    SearchIndexVariant m_variant;
};


#endif

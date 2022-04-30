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
#include <string>
#include <array>
#include <variant>

#include "qcstring.h"

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

class SearchIndexIntf
{
  public:
    enum Kind { Internal, External };
    SearchIndexIntf(Kind k) : m_kind(k) {}
    virtual ~SearchIndexIntf() = default;
    virtual void setCurrentDoc(const Definition *ctx,const QCString &anchor,bool isSourceFile) = 0;
    virtual void addWord(const QCString &word,bool hiPriority) = 0;
    virtual void write(const QCString &file) = 0;
    Kind kind() const { return m_kind; }
  private:
    Kind m_kind;
};


#endif

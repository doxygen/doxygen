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
 *  @brief Javascript based search engine.
 */

#ifndef SEARCHINDEX_JS_H
#define SEARCHINDEX_JS_H

#include <array>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <variant>

#include "qcstring.h"
#include "utf8.h"

#define NUM_SEARCH_INDICES 22

class SectionInfo;
class Definition;

//QCString searchId(const Definition *d);
QCString searchName(const Definition *d);

//! Searchable term
struct SearchTerm
{
  using LinkInfo = std::variant<std::monostate,const Definition *,const SectionInfo *>;
  SearchTerm(const QCString &w,const Definition *d)  : word(w.str()), info(d)  { makeTitle(); }
  SearchTerm(const QCString &w,const SectionInfo *s) : word(w.str()), info(s)  { makeTitle(); }
  QCString word;                 //!< lower case word that is indexed (e.g. name of a symbol, or word from a title)
  QCString title;                //!< title to show in the output for this search result
  LinkInfo info;                 //!< definition to link to
  QCString termEncoded() const;  //!< encoded version of the search term
private:
  void makeTitle();
};

//! List of search terms
using SearchIndexList = std::vector<SearchTerm>;

//! Map of search terms for a given starting letter
using SearchIndexMap  = std::map<std::string,SearchIndexList>; // key is starting letter of a term (UTF-8).

//! Table entry to allow filtering the search results per category
struct SearchIndexInfo
{
  void add(const SearchTerm &term);
  QCString name;
  std::function<QCString()> getText;
  SearchIndexMap symbolMap;
};

void createJavaScriptSearchIndex();
void writeJavaScriptSearchIndex();
const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices();

#endif

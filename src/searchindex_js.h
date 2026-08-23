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
#include <functional>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "dstring.h"

#define NUM_SEARCH_INDICES 22

class SectionInfo;
class Definition;

//DString searchId(const Definition *d);
DString searchName(const Definition *d);

//! Searchable term
struct SearchTerm
{
  using LinkInfo = std::variant<std::monostate,const Definition *,const SectionInfo *>;
  SearchTerm(const DString &w,const Definition *d)  : word(w.str()), info(d)  { makeTitle(); }
  SearchTerm(const DString &w,const SectionInfo *s) : word(w.str()), info(s)  { makeTitle(); }
  DString word;                 //!< lower case word that is indexed (e.g. name of a symbol, or word from a title)
  DString title;                //!< title to show in the output for this search result
  LinkInfo info;                 //!< definition to link to
  DString termEncoded() const;  //!< encoded version of the search term
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
  DString name;
  std::function<DString()> getText;
  SearchIndexMap symbolMap;
};

void createJavaScriptSearchIndex();
void writeJavaScriptSearchIndex();
const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices();

#endif

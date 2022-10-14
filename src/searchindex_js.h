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

#include "qcstring.h"

#define NUM_SEARCH_INDICES 21

class Definition;

QCString searchId(const Definition *d);
QCString searchName(const Definition *d);

using SearchIndexList = std::vector<const Definition *>;
using SearchIndexMap  = std::map<std::string,SearchIndexList>;

struct SearchIndexInfo
{
  void add(const std::string &letter,const Definition *def);
  QCString name;
  std::function<QCString()> getText;
  SearchIndexMap symbolMap;
};

void createJavaScriptSearchIndex();
void writeJavaScriptSearchIndex();
const std::array<SearchIndexInfo,NUM_SEARCH_INDICES> &getSearchIndices();

#endif

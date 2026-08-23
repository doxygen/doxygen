/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <map>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using StringUnorderedMap = std::unordered_map<std::string,std::string>;
using StringUnorderedSet = std::unordered_set<std::string>;
using StringMap          = std::map<std::string,std::string>;
using StringSet          = std::set<std::string>;
using StringMultiSet     = std::multiset<std::string>;
using StringVector       = std::vector<std::string>;
using StringViewVector   = std::vector<std::string_view>;
using BoolStack          = std::stack<bool>;
using BoolVector         = std::vector<bool>;
using IntMap             = std::map<std::string,int>;
using IntVector          = std::vector<int>;
using SizeVector         = std::vector<size_t>;

#endif

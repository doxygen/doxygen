#ifndef AR_HPP
#define AR_HPP

#include "dijkstra.hpp"
#include <algorithm>
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "fuzzy.h"

namespace ar {

using Dijkstra::Edge;
using Dijkstra::Graph;
using Dijkstra::Node;
using std::cout;
using std::endl;
using std::function;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;

using Dictionary = vector<pair<string, int>>;
using LongForm = vector<string>;
using Lmatch = vector<string>;
using nonDictWords = vector<string>;
using matches = vector<pair<string, string>>;
using Phi = function<int(string &)>;
using Cfunc = function<int(string &)>;

LongForm expand_known_abbr(nonDictWords nonDictWords, Dictionary D);

void check_vowels_consonants(string word, int &vowels, int &consonants);

Lmatch split_matching(string ident, vector<Dictionary> D);
vector<string> string_matching(std::string token, Dictionary D, Phi phi,
                               Cfunc cost);

LongForm expansion_matching(nonDictWords nonDictWords, Dictionary D);

Lmatch string_matching(std::string token, Dictionary D, Phi phi, Cfunc cost);

Lmatch split_matching(string ident, vector<Dictionary> D);

} // namespace ar


#endif


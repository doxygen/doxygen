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
#include <algorithm>

#include "fuzzy.h"

namespace ar {

using Dictionary = std::map<std::string, int>;
using Dictionaries = std::vector<ar::Dictionary>;
using LongForm = std::vector<std::string>;
using Lmatch = std::vector<std::string>;
using nonDictWords = std::vector<std::string>;
using matches = std::vector<std::pair<std::string, std::string>>;
using Phi = std::function<int(std::string &)>;
using Cfunc = std::function<int(std::string &)>;

LongForm expand_known_abbr(nonDictWords nonDictWords, ar::Dictionary D);

void check_vowels_consonants(std::string word, int &vowels, int &consonants);

LongForm expansion_matching(ar::nonDictWords nonDictWords, ar::Dictionary D);

Lmatch string_matching(std::string token, ar::Dictionary D, ar::Phi phi,
                           ar::Cfunc cost);

Lmatch split_matching(std::string ident, std::vector<Dictionary> D);
}

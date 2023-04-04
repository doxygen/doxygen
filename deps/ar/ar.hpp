#include "dijkstra.hpp"
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "fuzzy.h"
#include "opts.hpp"

namespace ar {

using Dictionary = std::unordered_map<std::string, int>;

struct Dictionaries {
  std::vector<Dictionary> dicts;

  Dictionaries(std::vector<Dictionary> dict): dicts(dict){}
  Dictionaries(): dicts(){}

  int find(std::string word);

  bool check(std::string word);

  void add(Dictionary d);

  std::string compare(std::string token);
};

LongForm expand_known_abbr(nonDictWords nonDictWords, ar::Dictionary D);

void check_vowels_consonants(std::string word, int &vowels, int &consonants);

LongForm expansion_matching(nonDictWords nonDictWords, ar::Dictionary D);

Lmatch string_matching(std::string token, ar::Dictionary D, Phi phi,
                       Cfunc cost);

Lmatch split_matching(std::string ident, Dictionaries D);
} // namespace ar

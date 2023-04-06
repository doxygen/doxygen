#include "dijkstra.hpp"
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <iterator>
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
  std::unordered_map<std::string, std::string> known_abbr;
  using Iterator = decltype(dicts.begin());

  Dictionaries(std::vector<Dictionary> dict) : dicts(dict) {}
  Dictionaries(std::vector<Dictionary> dict, std::unordered_map<std::string, std::string> known_abbr)
    : dicts(dict), known_abbr(known_abbr){}
  Dictionaries(): dicts(){}

  int find(std::string word);

  bool check(std::string word);

  void add(Dictionary d);

  std::string compare(std::string token);
  int operator[](std::string &word);

  // Iterator definition here ...

  Iterator begin() {
    return dicts.begin(); }
  Iterator end()   {
    return dicts.end(); } // 200 is out of bounds
};

LongForm expansion_matching(NonDictWords nonDictWords, ar::Dictionaries D);

Lmatch string_matching(std::string token, ar::Dictionary D, Phi phi,
                       Cfunc cost);

Lmatch split_matching(std::string ident, ar::Dictionaries D, Lmatch *matches);
} // namespace ar

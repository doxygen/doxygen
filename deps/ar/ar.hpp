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

using Dictionary = std::unordered_map<std::string, int>;

struct Dictionaries {
  std::vector<Dictionary> dicts;

  Dictionaries(std::vector<Dictionary> dict): dicts(dict){}
  Dictionaries(): dicts(){}

  int find(std::string word) {
    for(auto dict: this->dicts) {
      try {
        return dict[word];
      } catch (std::out_of_range e) {
      }
    }
    return -1;
  }

  bool check(std::string word) {
    for(auto dict: this->dicts) {
      try {
        auto res = dict[word];
        return true;
      } catch (std::out_of_range e) {
      }
    }
    return false;
  }

  void add(Dictionary d) {
    this->dicts.push_back(d);
  }
};

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

Lmatch split_matching(std::string ident, Dictionaries D);
} // namespace ar

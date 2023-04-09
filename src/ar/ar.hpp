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
  Dictionaries(std::vector<Dictionary> dict, std::unordered_map<std::string, std::string> known_abbr_c)
    : dicts(dict), known_abbr(known_abbr_c){}
  Dictionaries(): dicts(){}

  int find(const std::string &word) const;

  bool check(const std::string &word) const;

  void add(Dictionary d);

  const std::string compare(std::string &token) const;
  int operator[](const std::string &word) const;

  const Iterator begin() {
    return dicts.begin(); }

  const Iterator end() {
    return dicts.end(); }

  const auto begin() const {
    return dicts.begin(); }

  const auto end() const {
    return dicts.end(); }
};

const std::string do_ar(std::string word);

const LongForm expansion_matching(NonDictWords &nonDictWords, const ar::Dictionaries &D);

const Lmatch string_matching(std::string token, const ar::Dictionary &D, const Phi &phi,
                       const Cfunc &cost);

const Lmatch split_matching(std::string ident, const ar::Dictionaries &D, Lmatch *matches);
} // namespace ar

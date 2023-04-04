#ifndef FUZZY_HPP
#define FUZZY_HPP

#include "opts.hpp"
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace fuzzy {

using matches = std::vector<std::pair<std::string, std::string>>;

std::string bitap_fuzzy_bitwise_search(std::string token, std::string pattern,
                                       int k);

matches BYP(std::string token, std::pair<std::string, int> word, Phi p);
} // namespace fuzzy

#endif

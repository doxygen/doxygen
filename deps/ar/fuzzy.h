#ifndef FUZZY_HPP
#define FUZZY_HPP

#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <vector>
#include "opts.hpp"

namespace fuzzy {

std::string bitap_fuzzy_bitwise_search(std::string token, std::string pattern,
                                       int k);

std::vector<std::pair<std::string, std::string>>
BYP(std::string token, std::pair<std::string, int> word,
    Phi p);
} // namespace fuzzy

#endif

#ifndef FUZZY_HPP
#define FUZZY_HPP

#include "opts.hpp"
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace fuzzy {

Matches BYP(std::string token, std::pair<std::string, int> word, const Phi &p);
} // namespace fuzzy

#endif

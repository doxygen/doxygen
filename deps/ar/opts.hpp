#ifndef OPTS_HPP
#define OPTS_HPP

#include <vector>
#include <string>
#include <functional>

using LongForm = std::vector<std::string>;
using Lmatch = std::vector<std::string>;
using nonDictWords = std::vector<std::string>;
using matches = std::vector<std::pair<std::string, std::string>>;
using Phi = std::function<int(std::string &, std::string &)>;
using Cfunc = std::function<int(std::string &)>;

#endif

#ifndef OPTS_HPP
#define OPTS_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

using LongForm = std::unordered_map<std::string, std::string>;
using Lmatch = std::vector<std::string>;
using NonDictWords = std::vector<std::string>;
using matches = std::vector<std::pair<std::string, std::string>>;
using Phi = std::function<int(std::string &, std::string &)>;
using Cfunc = std::function<int(std::string &)>;

#define REDDD_COLOR "\033[31m"
#define YELLO_COLOR "\033[33m"
#define RESET_COLOR "\033[30m"

#define INFO(message)                                                          \
  std::cout << YELLO_COLOR << std::setw(10) << "[INFO] " << message << RESET_COLOR << std::endl;

#define WARNING(message)                                                          \
  std::cout << REDDD_COLOR << std::setw(10) << "[WARNING] " << message << RESET_COLOR << std::endl;

#endif

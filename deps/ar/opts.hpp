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
#define GREEN_COLOR "\033[32m"
#define YELLO_COLOR "\033[33m"
#define RESET_COLOR "\033[0m"

#define INFO(message)                                                          \
  std::cout << YELLO_COLOR << std::setw(9) << std::left << "[INFO] " << message << RESET_COLOR << std::endl;

#define WARNING(message)                                                          \
  std::cout << REDDD_COLOR << std::setw(9) << std::left << "[WARNING] " << message << RESET_COLOR << std::endl;

#define RESULT(message)                                                          \
  std::cout << GREEN_COLOR << std::setw(9) << std::left << "[OUTPUT] " << message << RESET_COLOR << std::endl;

#endif

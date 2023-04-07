#ifndef OPTS_HPP
#define OPTS_HPP

#define INFO_LEVEL 0
#define RESULT_LEVEL 1
#define WARNING_LEVEL 2
#define DANGER_LEVEL 3

#define LOG_LEVEL INFO_LEVEL

#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using LongForm = std::unordered_map<std::string, std::string>;
using Lmatch = std::vector<std::string>;
using NonDictWords = std::vector<std::string>;
using Matches = std::vector<std::pair<std::string, std::string>>;
using Phi = std::function<int(std::string &, std::string &)>;
using Cfunc = std::function<int(std::string &)>;

#define REDDD_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLO_COLOR "\033[33m"
#define BLUER_COLOR "\033[34m"
#define RESET_COLOR "\033[0m"

enum log_levels { INFO, RESULT, WARNING, DANGER };

void LOG(std::string message, log_levels loglevel);

#define PRINT(color, type, message)                                            \
  std::cout << color << std::setw(9) << std::left << type << message           \
            << RESET_COLOR << std::endl;

#if LOG_LEVEL == INFO_LEVEL
#define INFO(message) PRINT(BLUER_COLOR, "[INFO] ", message)
#else
#define INFO(message)
#endif

#if LOG_LEVEL <= RESULT_LEVEL
#define RESULT(message) PRINT(GREEN_COLOR, "[OUTPUT] ", message)
#else
#define RESULT(message)
#endif

#if LOG_LEVEL <= WARNING_LEVEL
#define WARNING(message) PRINT(YELLO_COLOR, "[WARNING] ", message)
#else
#define WARNING(message)
#endif

#if LOG_LEVEL <= DANGER_LEVEL
#define DANGER(message) PRINT(REDDD_COLOR, "[DANGER] ", message)
#else
#define RESULT(message)
#endif

#endif

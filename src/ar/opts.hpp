#ifndef OPTS_HPP
#define OPTS_HPP

#include <set>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define INFO_LEVEL 0
#define RESULT_LEVEL 1
#define WARNING_LEVEL 2
#define DANGER_LEVEL 3

#define LOG_LEVEL WARNING_LEVEL

struct MNode {
  std::string word;
  int start;
  int end;


  bool operator==(const MNode &other) const {
    // return this->i == other.i && this->j == other.j &&
    //        this->word == other.word && this->cost == other.cost;
    return this->word == other.word &&
           this->start == other.start && this->end == other.end;
  }

  bool operator!=(const MNode &other) const {
    // return this->i == other.i && this->j == other.j &&
    //        this->word == other.word && this->cost == other.cost;
    return !(*this == other);
  }

  bool operator<(const MNode &other) const {
    // return this->i == other.i && this->j == other.j &&
    //        this->word == other.word && this->cost == other.cost;
    return this->start < other.start;
  }

  bool operator>(const MNode &other) const {
    // return this->i == other.i && this->j == other.j &&
    //        this->word == other.word && this->cost == other.cost;
    return this->start > other.start;
  }
};

struct mnode_hasher {
  std::hash<std::string> hasher;
  size_t operator()(const MNode &e) const {
    size_t hash_result = 0;
    hash_result += hasher(e.word) * 31;
    hash_result += e.start * 37;
    hash_result += e.end * 41;
    // calculate the hash result
    return hash_result;
  }
};

using LongForm = std::unordered_map<std::string, std::string>;
using Lres = std::unordered_map<int, std::string>;
using AllMatches = std::set<MNode, std::less<MNode>>;
using LmatchB = std::vector<std::pair<bool, std::string>>;
using Lmatch = std::vector<std::string>;
using NonDictWords = std::vector<std::string>;
using Matches = std::vector<std::pair<std::string, std::string>>;
using Phi = std::function<int(std::string &, std::string &)>;
using Cfunc = std::function<int(std::string &)>;

constexpr const char* EOS = "\n"; // END OF STRING
constexpr const char* NULLS = "\0"; // NULL-STRING

#define REDDD_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLO_COLOR "\033[33m"
#define BLUER_COLOR "\033[34m"
#define RESET_COLOR "\033[0m"

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

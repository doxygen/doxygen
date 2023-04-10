#include "ar.hpp"
#include "opts.hpp"
#include <stdexcept>

using Dijkstra::Graph;
using Dijkstra::Node;
using std::pair;
using std::string;

int ar::Dictionaries::operator[](const std::string &word) const {
  for (auto dict : this->dicts) {
    try {
      return dict[word];
    } catch (const std::out_of_range &e) {
    }
  }
  return -1;
}

bool ar::Dictionaries::check(const std::string &word) const {
  for (auto dict : this->dicts) {
    try {
      dict[word];
      return true;
    } catch (const std::out_of_range &e) {
    }
  }
  return false;
}

void ar::Dictionaries::add(Dictionary d) { this->dicts.push_back(d); }

const LongForm expand_known_abbr(NonDictWords &nonDictWords,
                                 const ar::Dictionaries &D,
                                 NonDictWords *toExpand) {
  LongForm retvec;

  for (size_t it = 0; it < nonDictWords.size(); it++) {
    const auto &each = nonDictWords[it];
    try {
      auto word = D.known_abbr.at(each);
      if (word.size() == 0) {
        toExpand->push_back(each);
        continue;
      }
      retvec.insert({each, word});
    } catch (const std::out_of_range &e) {
      toExpand->push_back(each);
    }
  }

  return retvec;
}

bool is_vowels(char character) {
  if (character > 'z' || character < 'a') {
    return false;
  }
  switch (character) {
  case 'a':
  case 'e':
  case 'i':
  case 'o':
  case 'u':
    return true;
  default:
    return false;
  }
}

bool is_consonant(char character) {
  if (character > 'z' || character < 'a') {
    return false;
  }
  return !is_vowels(character);
}

void check_vowels_consonants(const string &word, int *vowels, int *consonants) {
  for (auto each : word) {
    if (is_vowels(each)) {
      *vowels += 1;
    }
    if (is_consonant(each)) {
      *consonants += 1;
    }
  }
}

const LongForm ar::expansion_matching(NonDictWords &nonDictWords,
                                      const Dictionaries &D) {
  INFO("EXPANSION_MATCHING STARTED")
  NonDictWords toExpand;
  LongForm retvec = expand_known_abbr(nonDictWords, D, &toExpand);

  if (toExpand.size() == 0) {
    INFO("Nothing to expand")
    return retvec;
  }

  for (auto word : toExpand) {
    INFO("HAVE TO EXPAND:" << word)
  }

  Phi phi = nullptr;
  Cfunc cost = [&D](std::string &word) { return D[word]; };
  for (auto token : toExpand) {
    for (const auto &dictionary : D) {
      // INFO(token);
      int vowels = 0;
      int consonants = 0;
      check_vowels_consonants(token, &vowels, &consonants);
      if (vowels >= consonants) {
        // if the token is a prefix
        phi = [&token](string &tok, string &word) {
          if (tok.length() != token.length()) {
            return -1;
          }
          for (size_t i = 0; i < tok.length(); i++) {
            if (word[i] != tok[i]) {
              return -1;
            }
          }
          return 1;
        };
      } else {
        phi = [&token](string &tok, string &word) {
          int pos = 0;
          if (token[0] != word[0]) {
            return -1;
          }
          if (tok[pos] != word[pos]) {
            auto n_pos = token.find(tok);
            if (n_pos != string::npos) {
              pos = n_pos;
            } else {
              return -1;
            }
          }
          size_t top_counter = 0;
          size_t counter = 0;
          // Ensure that the word contains all the needed letters
          for (counter = 0; counter < token.length(); counter++) {
            for (size_t j = pos; j < word.length(); j++) {
              if (token[counter] == word[j]) {
                pos = j;
                top_counter++;
                break;
              }
            }
          }
          if (top_counter != counter) {
            return -1;
          }
          return 1;
        };
      }
      Lmatch matches = string_matching(token, dictionary, phi, cost);
      if (matches.size() > 0) {
        for (auto &each : matches) {
          retvec.insert({token, each});
        }
        break;
      }
    }
  }
  INFO("EXPANSION_MATCHING ENDED")
  return retvec;
}

const Lmatch ar::string_matching(std::string token, const Dictionary &D,
                                 const Phi &phi, const Cfunc &cost) {
  INFO("STRING_MATCHING STARTED")
  // char, vec<{Node, Node}>
  // G = (V, E) <- initializeMathingGraph(token)
  //
  Graph G = Dijkstra::initializeMatchingGraph(token);
  // h -> e    INT32_MIN
  // e -> l    INT32_MIN
  // l -> l    INT32_MIN
  // l -> '\0' INT32_MIN

  // for each: word E Dict do
  for (pair<string, int> word : D) {
    // matching_seq <- BYP(token, word, phi(word))
    Matches matching_seq = fuzzy::BYP(token, word, phi);
    // for each: (<ch_i..ch_j>, word) E matching_seq do
    for (auto &[first, last] : matching_seq) {
      int i = token.find(first);
      int j = i + first.length() - 1;
      auto any = Node{
          i,
          j + 1,
          last,
          cost(last),
      };
      // G(E) <- G(E) U {<i, j, word, cost(word)>}
      G[any.i].push_back({any});
    }
  }

  auto best_path = Dijkstra::dijkstra(G, token);
  RESULT("DIJKSTRA DONE")

  //    end for
  // end for
  // best_path <- Dijkstra(G)
  // return getEdgeLabels(best_path)
  INFO("STRING_MATCHING ENDED")
  return getEdgeLabels(best_path);
}

const Lmatch ar::split_matching(string ident, const Dictionaries &D,
                                Lmatch *matches) {

  INFO("SPLIT_MATCHING STARTED")
  Phi phi = [&D, &ident](std::string &token, std::string &word) {
    if (token == word) {
      return 1;
    }
    return -1;
  };

  Cfunc cost = [&D](std::string &word) { return D[word]; };

  for (auto dict : D.dicts) {
    for (std::string each : string_matching(ident, dict, phi, cost)) {
      matches->push_back(each);
    };

    if (matches->size() != 0) {
      INFO("SPLIT_MATCHING DONE")
      break;
    }
  }
  Lmatch retvec;
  for (auto each : *matches) {
    auto find = ident.find(each);
    auto split = ident.substr(0, find);
    ident.erase(0, find + each.size());
    retvec.push_back(split);
  }

  if (ident.size() > 0 && ident != "" && ident != "\0" && ident != "\n") {
    retvec.push_back(ident);
  }

  retvec.erase(
      std::remove_if(retvec.begin(), retvec.end(),
                     [](const std::string &str) { return str.size() == 0; }),
      retvec.end());
  INFO("SPLIT_MATCHING ENDED")
  return retvec;
}

#include <filesystem>
#include <fstream>
#include <math.h>
#include <ostream>

struct File {
  std::fstream ifile;
  File(std::string filename) {
    ifile.open(filename);

    if (!ifile.is_open()) {
      DANGER("Could not open file: " << filename)
      ifile.close();
      exit(1);
    }
  }

  ~File() { ifile.close(); }
};

/**
 * Gets the abbreviation-list from memory
 */
std::unordered_map<std::string, std::string> get_abbr(string filename) {
  std::unordered_map<std::string, std::string> retvec;
  File file(filename);

  string line;
  string delim = ";";
  while (std::getline(file.ifile, line)) {
    int first = line.find(delim);
    string word = line.substr(0, first);
    line.erase(0, first + delim.length());
    string other = line;
    retvec.insert({word, other});
  }

  return retvec;
}

/**
 * Reads the given file from memory
 */
ar::Dictionary get_dict(string filename) {
  ar::Dictionary retvec;

  File file(filename);

  file.ifile.clear();
  file.ifile.seekg(0);

  string line;
  string delim = ";";
  while (std::getline(file.ifile, line)) {
    int first = line.find(delim);
    string word = line.substr(0, first);
    line.erase(0, first + delim.length());
    int other = -1;
    try {
      other = std::stoi(line.substr(0, first));
    } catch (const std::invalid_argument &e) {
      other = (word.length() / std::log(word.length()));
    }
    retvec.insert({word, other});
  }

  return retvec;
}

/**
 * Gets all the necessary files from memory
 */
ar::Dictionaries get_true() {
  auto eng_dict = get_dict("eng_dict.txt");
  auto it_dict = get_dict("it_dict.txt");
  auto known_abbr = get_abbr("known_abbr.txt");
  ar::Dictionaries D({eng_dict, it_dict}, known_abbr);
  return D;
}

/**
 * Replaces a give string rep with what in the word word
 */
void replace(std::string &word, std::string rep, std::string what) {
  for (size_t index = 0;;) {
    /* Locate the substring to replace. */
    index = word.find(rep, index);
    if (index == std::string::npos)
      break;

    /* Make the replacement. */
    word.replace(index, rep.size(), what);

    /*
     * Advance index forward so the next iteration doesn't pick it up as
     * well.
     */
    index += what.size();
  }
}

// Global Dictionary
const auto D = get_true();

const std::string internal_do_ar(std::string token) {
  RESULT("STARTED: " << token)

  Lmatch matches;
  auto labels = ar::split_matching(token, D, &matches);
  LongForm thing;
  if (labels.size() != 0) {
    if (labels[0].length() != 1) {
      thing = ar::expansion_matching(labels, D);
    }
  }

  // Replace the words in the identifier
  INFO("REPLACING MATCHES")
  for (auto each : matches) {
    WARNING("\tno_match " << each)
    replace(token, each, each + "_");
  };
  INFO("REPLACING EXPANDED WORDS")
  for (auto &[each, word] : thing) {
    WARNING("\tmatch each " << each << " word " << word)
    if (each.length() <= 1) {
      continue;
    }
    replace(token, each, word + "_");
  }
  INFO("EVERYTHING DONE")

  RESULT("RESULT: " << token)

  return token;
}

const std::string ar::do_ar(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  auto vec = {"_", "-"};
  for (auto each : vec) {
    replace(token, each, "");
  }
  return internal_do_ar(token);
}

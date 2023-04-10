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

const Lmatch string_matching(std::string token, const ar::Dictionary &D,
                             const Phi &phi, const Cfunc &cost,
                             AllMatches *matches = nullptr) {
  INFO("STRING_MATCHING STARTED")
  // char, vec<{Node, Node}>
  // G = (V, E) <- initializeMathingGraph(token)
  //
  Graph G = Dijkstra::initializeMatchingGraph(token);

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
  if (matches != nullptr) {
    std::string saved = "";
    int start = 0;
    int end = 0;
    for (auto each : best_path) {
      INFO("parsing: " << each.word)
      if (each.word == EOS) {
        if (saved != "") {
          matches->insert({saved, start, end});
        }
        break;
      }
      if (each.cost == INT32_MIN) {
        saved += each.word;
        end += 1;
      } else {
        if (saved != "") {
          matches->insert({saved, start, end});
          start = end + 1;
          end = start + each.word.length();
          saved.clear();
        }
        matches->insert({each.word, each.i, each.j});
        start = each.j + 1;
        end = start;
      }
    }
  }
  RESULT("DIJKSTRA DONE")

  //    end for
  // end for
  // best_path <- Dijkstra(G)
  // return getEdgeLabels(best_path)
  INFO("STRING_MATCHING ENDED")
  return Dijkstra::getEdgeLabels(best_path);
}

const LongForm expansion_matching(NonDictWords &nonDictWords,
                                  const ar::Dictionaries &D) {
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
      auto matches = string_matching(token, dictionary, phi, cost);
      if (matches.size() > 0) {
        for (auto each : matches) {
          retvec.insert({token, each});
        }
        break;
      }
    }
  }
  INFO("EXPANSION_MATCHING ENDED")
  return retvec;
}

const Lmatch split_matching(string ident, const ar::Dictionaries &D,
                            AllMatches *matches) {

  INFO("SPLIT_MATCHING STARTED")
  Phi phi = [&D, &ident](std::string &token, std::string &word) {
    if (token == word) {
      return 1;
    }
    return -1;
  };

  Cfunc cost = [&D](std::string &word) { return D[word]; };

  Lmatch fullvec;
  for (auto dict : D.dicts) {
    for (auto each : string_matching(ident, dict, phi, cost, matches)) {
      fullvec.push_back(each);
    };

    if (fullvec.size() != 0) {
      INFO("SPLIT_MATCHING DONE")
      break;
    }
  }
  Lmatch retvec;
  for (auto each : *matches) {
    retvec.push_back(each.word);
  }
  for (auto each : *matches) {
    INFO("\t" << each.word)
  }

  retvec.erase(std::remove_if(retvec.begin(), retvec.end(),
                              [&fullvec](const std::string &str) {
                                return std::find(fullvec.begin(), fullvec.end(),
                                                 str) != fullvec.end() ||
                                       str.size() == 0;
                              }),
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

#include <numeric>

const std::string internal_do_ar(const std::string &token) {
  RESULT("STARTED: " << token)

  AllMatches matches;
  auto labels = split_matching(token, D, &matches);
  LongForm thing;
  if (labels.size() != 0) {
    if (labels[0].length() != 1) {
      thing = expansion_matching(labels, D);
    }
  }

  INFO("REPLACING EXPANDED WORDS")
  string retToken = "";
  size_t count = 0;
  for (auto &[each, start, end] : matches) {
    if (each == EOS || each == NULLS) {
      continue;
    }
    try {
      auto word = thing.at(each);
      INFO("\tword: " << word);
      retToken += word;
    } catch (const std::exception &e) {
      INFO("\teach: " << each);
      retToken += each;
    }

    count += 1;
    if (count != matches.size())
      retToken += "_";
  }
  INFO("EVERYTHING DONE")

  RESULT("RESULT: " << retToken)
  return retToken;
}

const std::string ar::do_ar(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  token.erase(std::remove_if(token.begin(), token.end(),
                             [](const char &s) {
                               if (s == '_')
                                 return false;
                               return !std::isalnum(s);
                             }),
              token.end());
  auto vec = {"_", "-"};
  for (auto each : vec) {
    replace(token, each, "");
  }
  return internal_do_ar(token);
}

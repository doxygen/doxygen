#include "ar.hpp"
#include "opts.hpp"
#include <exception>
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
          cost(last) * (int)first.size(),
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
        MNode any = {each.word, each.i, each.j};
        auto res = matches->insert(any);
        if (res.second == false) {
          WARNING("Could not be inserted\t" << any.word << " " << any.start
                                            << " " << any.end)
          WARNING("Due to\t" << res.first->word << " " << res.first->start
                             << " " << res.first->end)
        }
        start = each.j;
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
        phi = [&D, &token](string &tok, string &word) {
          int pos = 0;
          if (tok[0] != word[0]) {
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
          return 0;
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

#include <regex>
const std::regex r("([a-z](?=[A-Z]))|_");

const void split_regex(std::string ident, AllMatches *matches) {
  Lmatch retvec;
  std::smatch m;
  std::regex_search(ident, m, r);
  if (m.size() == 0) {
    return;
  }
  auto words_begin = std::sregex_iterator(ident.begin(), ident.end(), r);
  auto words_end = std::sregex_iterator();
  int prev = 0;
  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    int match_pos = match.position();
    if (match_str != "_") {
      match_pos += 1;
    }

    auto substr = ident.substr(prev, match_pos - prev);
    std::transform(substr.begin(), substr.end(), substr.begin(),
                   [](const unsigned char &c) { return std::tolower(c); });
    substr.erase(
        std::remove_if(substr.begin(), substr.end(),
                       [](const unsigned char &s) { return !std::isalnum(s); }),
        substr.end());
    matches->insert({substr, prev, match_pos});

    if (match_str == "_") {
      match_pos += 1;
    }
    prev = match_pos;
  }
  auto str = ident.substr(prev, ident.length());

  std::transform(str.begin(), str.end(), str.begin(),
                 [](const unsigned char &c) { return std::tolower(c); });
  str.erase(
      std::remove_if(str.begin(), str.end(),
                     [](const unsigned char &s) { return !std::isalnum(s); }),
      str.end());
  matches->insert({str, prev, (int)ident.length()});
}

const Lmatch split(std::string ident, const ar::Dictionary &D, const Phi &phi,
                   const Cfunc &cost, AllMatches *matches, bool *other) {
  Lmatch retvec;
  for (auto each : *matches) {
    try {
      auto res = D.at(each.word);
    } catch (const std::out_of_range &e) {
      retvec.push_back(each.word);
    }
  }
  return retvec;
}

const Lmatch split_matching(string ident, const ar::Dictionaries &D,
                            AllMatches *matches) {

  INFO("SPLIT_MATCHING STARTED")
  Phi phi = [&D, &ident](std::string &token, std::string &word) {
    if (token == word) {
      return 0;
    }
    return -1;
  };

  Cfunc cost = [&D](std::string &word) { return D[word]; };

  Lmatch fullvec;
  bool other = false;
  split_regex(ident, matches);
  if (matches->size() == 0) {
    other = true;
    for (auto dict : D.dicts) {
      for (auto each : string_matching(ident, dict, phi, cost, matches)) {
        fullvec.push_back(each);
      }

      if (fullvec.size() != 0) {
        INFO("SPLIT_MATCHING DONE")
        break;
      }
    }
  } else {
    Lmatch vec;
    for (auto each : *matches) {
      vec.push_back(each.word);
    }
    for (int i = 0; i < vec.size(); i++) {
      for (auto dict : D.dicts) {
        try {
          auto _ = dict.at(vec[i]);
          fullvec.push_back(vec[i]);
          break;
        } catch (const std::out_of_range &e) {
        }
      }
    }
  }
  Lmatch retvec{};
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
    int other = (word.length() / std::log(word.length()));
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
  size_t count = 0;
  std::stringstream retToken;
  for (auto &[each, _start, _end] : matches) {
    if (each == EOS || each == NULLS) {
      continue;
    }
    try {
      auto word = thing.at(each);
      INFO("\tword: " << word);
      retToken << word;
    } catch (const std::exception &e) {
      INFO("\teach: " << each);
      retToken << each;
    }

    count += 1;
    if (count != matches.size())
      retToken << "_";
  }
  INFO("EVERYTHING DONE")

  RESULT("RESULT: " << retToken.str())
  return retToken.str();
}

const std::string ar::do_ar(std::string token) {
  // token.erase(
  //     std::remove_if(token.begin(), token.end(),
  //                    [](const unsigned char &s) { return !std::isalnum(s);
  //                    }),
  //     token.end());
  // std::transform(token.begin(), token.end(), token.begin(),
  //                [](const unsigned char &c) { return std::tolower(c); });
  return internal_do_ar(token);
}

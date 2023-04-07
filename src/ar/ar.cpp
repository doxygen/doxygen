#include "ar.hpp"
#include "opts.hpp"
#include <stdexcept>

using Dijkstra::Graph;
using Dijkstra::Node;
using std::string;
using std::pair;

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

const LongForm expand_known_abbr(NonDictWords &nonDictWords, ar::Dictionaries &D,
                           NonDictWords *toExpand) {
  LongForm retvec;

  for (size_t it = 0; it < nonDictWords.size(); it++) {
    const auto &each = nonDictWords[it];
    try {
      auto word = D.known_abbr[each];
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

const LongForm ar::expansion_matching(NonDictWords &nonDictWords, Dictionaries &D) {
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
          if(token[0] != word[0]) {
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
          INFO(tok << " " << word)
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

  return retvec;
}

const Lmatch ar::string_matching(std::string token, const Dictionary &D, const Phi &phi,
                           const Cfunc &cost) {
  // Graph G = initializeMatchingGraph(token);
  //
  // int index = 0;
  // for (auto word : D) {
  //
  //   vector<pair<string, string>> matching_seq = BYP(token, word, phi);
  //
  //   cout << " first " << " second " << endl;
  //
  //   // mathing_seq <- BYP(token, word, phi(word))
  //   // for (<ch_i..ch_j>, word) e matching_seq do
  //   for (pair<string, string> match : matching_seq) {
  //     cout << "| " << std::setw(8) << match.first.c_str()
  //          << " | " << std::setw(8) << match.second.c_str() << " |" << endl;
  //     // hello
  //     // hel
  //     int i = token.find(match.first);    // 0
  //     int j = i + match.first.size() - 1; // 0 + 3
  //               // 0, 3, hel,         cost
  //     Node from = {i, j, match.first, token.substr(i, j), cost(match.first)};
  //
  //             // 3, 4, hello,        cost
  //     Node to = {j + 1, j, match.second, token.substr(j + 1, j),
  //     cost(match.second)};
  //
  //     // G(E) <- G(E) U {(i, j), word, c(word)};
  //     G[from.slice].push_back({.from = from, .to = to});
  //   }
  // }
  //
  // Node start = {0, 0, "h", "h", INT32_MIN};
  // Node end = {0, 3, "hell", "hell", (int)token.size()};
  // vector<Node> path = dijkstra(G, token, start, end);
  // for (Node label : path) {
  //   cout << "labels: " << label.word << " ";
  // }
  // return getEdgeLabels(path, G);

  // char, vec<{Node, Node}>
  // G = (V, E) <- initializeMathingGraph(token)
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
      // INFO(first << " " << last)
      int i = token.find(first);
      int j = i + first.length() - 1;
      auto any = Node{
          i,
          j + 1,
          last,
          cost(last),
      };
      // std::cout << "HELLO " << s_first << " " << s_second << std::endl;
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
  return getEdgeLabels(best_path);
}

const Lmatch ar::split_matching(string ident, const Dictionaries &D, Lmatch *matches) {

  Phi phi = [](std::string &token, std::string &word) {
    if (token == word) {
      return 0;
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

  retvec.push_back(ident);

  retvec.erase(
      std::remove_if(retvec.begin(), retvec.end(),
                     [](const std::string &str) { return str.size() == 0; }),
      retvec.end());
  return retvec;
}

#include <filesystem>
#include <fstream>
#include <math.h>
#include <ostream>

struct File {
  std::fstream ifile;
  std::string base = "./dicts/";
  File(std::string filename) {
    ifile.open(base + filename);

    if (!ifile.is_open()) {
      DANGER("Could not open file: " << filename)
      ifile.close();
      exit(1);
    }
  }

  ~File() { ifile.close(); }
};

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

ar::Dictionaries get_true() {
  ar::Dictionaries D;
  auto eng = get_dict("test.txt");
  auto it = get_dict("it_abbr.txt");
  D.add(eng);
  D.add(it);
  D.known_abbr = get_abbr("known_abbr.txt");
  return D;
}

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
    index += rep.size();
  }
}

auto D = get_true();
const std::string ar::do_ar(std::string token) {

  auto vec = {"_", "-"};
  for (auto each : vec) {
    replace(token, each, "");
  }
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  RESULT("STARTED: " << token)

  Lmatch matches;
  auto labels = ar::split_matching(token, D, &matches);

  auto thing = ar::expansion_matching(labels, D);

  // Replace the words in the identifier
  for (auto each : matches) {
    replace(token, each, each + "_");
  };
  for (auto &[each, word] : thing) {
    replace(token, each, word + "_");
  }

  RESULT(token)

  return token;
}

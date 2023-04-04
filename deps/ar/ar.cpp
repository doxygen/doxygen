#include "ar.hpp"

using ar::Cfunc;
using ar::Dictionary;
using ar::Lmatch;
using ar::LongForm;
using ar::matches;
using ar::nonDictWords;
using ar::Phi;

using Dijkstra::Edge;
using Dijkstra::Graph;
using Dijkstra::Node;
using std::cout;
using std::endl;
using std::function;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;

LongForm ar::expand_known_abbr(nonDictWords nonDictWords, Dictionary D) {
  LongForm retvec;

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

void ar::check_vowels_consonants(string word, int &vowels, int &consonants) {
  for (auto each : word) {
    if (is_vowels(each)) {
      vowels += 1;
    }
    if (is_consonant(each)) {
      consonants += 1;
    }
  }
}

LongForm ar::expansion_matching(nonDictWords nonDictWords, Dictionary D) {
  LongForm retvec = ar::expand_known_abbr(nonDictWords, D);

  LongForm toExpand;
  std::copy_if(nonDictWords.begin(), nonDictWords.end(), toExpand.begin(),
               [&retvec](string const value) {
                 return std::find(retvec.begin(), retvec.end(), value) ==
                        retvec.end();
               });

  Phi phi = nullptr;
  Cfunc c = nullptr;
  for (auto token : toExpand) {
    for (auto dictionary : D) {
      int vowels;
      int consonants;
      check_vowels_consonants(token, vowels, consonants);
      if (vowels > consonants) {
        phi = [&D](string token) { return 1; };
      } else {
        phi = [&D](string token) { return 0; };
      }
      Lmatch matches = string_matching(token, D, phi, c);
      if (matches.size() > 0) {
        std::copy_if(matches.begin(), matches.end(), retvec.begin(),
                     [&retvec](string const value) {
                       return std::find(retvec.begin(), retvec.end(), value) ==
                              retvec.end();
                     });
        break;
      }
    }
  }

  return retvec;
}

Lmatch ar::string_matching(std::string token, Dictionary D, Phi phi,
                           Cfunc cost) {
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
  for (auto word : D) {
    // matching_seq <- BYP(token, word, phi(word))
    matches matching_seq = fuzzy::BYP(token, word, phi);
    // for each: (<ch_i..ch_j>, word) E matching_seq do
    for (auto &[first, last] : matching_seq) {
      int i = token.find(first);
      int j = i + first.length() - 1;
      auto any = Node{
          .i = i,
          .j = j,
          .word = last,
          .cost = cost(last),
      };
      // std::cout << "HELLO " << s_first << " " << s_second << std::endl;
      // G(E) <- G(E) U {<i, j, word, cost(word)>}
      G[any.i].push_back({any});
    }
  }

  auto start = Node{.i = 0, .j = 0, .word = token, .cost = 0};
  auto end = Node{(int)token.length() - 1, (int)token.length(),
                  token.substr(token.length() - 1, 1), INT32_MIN};
  auto best_path = Dijkstra::dijkstra(G, token, start, end);

  cout << "RESULT" << endl;
  for (auto each : best_path) {
    cout << each.word << " -> ";
  }
  cout << endl;

  //    end for
  // end for
  // best_path <- Dijkstra(G)
  // return getEdgeLabels(best_path)
  vector<Node> path;
  return getEdgeLabels(path, G);
}

Lmatch ar::split_matching(string ident, Dictionaries D) {
  Lmatch retvec;

  ar::Phi phi = [&ident, &D](std::string &word) {
    for (auto each : D) {
      try {
        auto res = each[word];
        return 0;
      } catch (std::out_of_range e) {
      }
    }
    return -1;
  };
  ar::Cfunc cost = [&D](std::string &word) {
    for (auto dict : D) {
      try {
        return dict.at(word);
      } catch (std::out_of_range e) {
      }
    }
    return -1;
  };

  for (auto dict : D) {
    retvec = string_matching(ident, dict, phi, cost);
    if (retvec.size() != 0) {
      return retvec;
    }
  }
  return retvec;
}

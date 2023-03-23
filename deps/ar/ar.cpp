#include <functional>
#include <string>
#include <vector>
#include <algorithm>

namespace ar {

using std::function;
using std::string;
using std::vector;

using Dictionary = vector<string>;
using LongForm = vector<string>;
using Lmatch = vector<string>;
using nonDictWords = vector<string>;
using Phi = function<int(string)>;
using C = function<void(void)>;

LongForm expand_known_abbr(nonDictWords nonDictWords, Dictionary D) {
  LongForm retvec;

  return retvec;
}

int vowels(string word) {
  return 0;
}

int consonants(string word) {
  return 0;
}


Lmatch split_matching(string ident, vector<Dictionary> D);
vector<string> string_matching(std::string token, Dictionary D,
                               function<int(string)> phi,
                               function<void(void)> cost);

LongForm expansion_matching(nonDictWords nonDictWords, Dictionary D) {
  LongForm retvec = expand_known_abbr(nonDictWords, D);

  LongForm toExpand;
  std::copy_if(nonDictWords.begin(), nonDictWords.end(), toExpand, [&retvec](string const value){
      return std::find(retvec.begin(), retvec.end(), value) == retvec.end();
  });

  Phi phi = nullptr;
  C c = nullptr;
  for(auto token: toExpand) {
    for(auto dictionary: D) {
      if(vowels(token) > consonants(token)) {
        phi = [&D]( string token ){ return 0; };
      } else {
        phi = [&D]( string token ){ return 0; };
      }
      Lmatch matches = string_matching(token, D, phi, c);
      if(matches.size() > 0) {
        std::copy_if(matches.begin(), matches.end(), retvec, [&retvec](string const value){
            return std::find(retvec.begin(), retvec.end(), value) == retvec.end();
        });
        break;
      }
    }
  }

  return retvec;
}

vector<string> string_matching(std::string token, Dictionary D,
                               function<int(string)> phi,
                               function<void(void)> cost) {
  vector<string> retvec;

  return retvec;
}

Lmatch split_matching(string ident, vector<Dictionary> D) {
  Lmatch retvec;

  Phi phi = [&D](string word) {
    bool found = true;
    for(auto dict : D) {
      if(std::find(dict.begin(), dict.end(), word) == dict.end()) {
        found &= false;
      }
    }
    return found;
  };
  C cost = []() {};

  for (auto dict : D) {
    retvec = string_matching(ident, dict, phi, cost);
    if(retvec.size() != 0) {
      return retvec;
    }
  }
  return retvec;
}

} // namespace ar

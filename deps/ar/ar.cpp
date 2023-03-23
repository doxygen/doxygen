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

bool is_vowels(char word) {
  switch ( word ) {
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

int is_consonant(char word) {
  return !is_vowels(word);
}

void check_vowels_consonants(string word, int &vowels, int &consonants) {
  for(auto each: word) {
    if(is_vowels(each)) {
      vowels+=1;
    }
    if(is_consonant(each)) {
      consonants+=1;
    }
  }
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
      int vowels;
      int consonants;
      check_vowels_consonants(token, vowels, consonants);
      if(vowels > consonants) {
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

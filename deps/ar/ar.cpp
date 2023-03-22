#include <functional>
#include <string>
#include <vector>
#include <algorithm>

namespace ar {

using std::function;
using std::string;
using std::vector;

using Dictionary = vector<string>;

vector<string> string_matching(std::string token, Dictionary D,
                               function<int(string)> phi,
                               function<void(void)> cost) {
  vector<string> retvec;
  return retvec;
}
void expansion_matching();

vector<string> split_matching(string ident, vector<Dictionary> D) {
  vector<string> retvec;

  function<int(string)> phi = [&D](string word) {
    bool found = true;
    for(auto dict : D) {
      if(std::find(dict.begin(), dict.end(), word) == dict.end()) {
        found &= false;
      }
    }
    return found;
  };
  function<void(void)> cost = []() {};

  for (auto dict : D) {
    retvec = string_matching(ident, dict, phi, cost);
    if(retvec.size() != 0) {
      return retvec;
    }
  }
  return retvec;
}

void expansion_matching() {}

} // namespace ar

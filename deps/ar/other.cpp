#include <math.h>
#include <string>
#include <vector>

#include "ar.hpp"

int main(void) {

  // std::string token = "hel";
  // ar::vector<ar::Dictionary> D = {{
  //     {"hello", 3},
  //     {"help", 1},
  //     {"hollow", 2},
  //     {"hallo", 2},
  //     {"heal", 1},
  //     {"heel", 1},
  //     {"basketball", 4},
  //     {"bicycle", 3},
  // }};

  std::string token = "getpnt";
  std::vector<ar::Dictionary> D = {{
      {"get", 3},
      {"getter", 1},
      {"great", 2},
      {"hallo", 2},
      {"pointer", 3},
      {"point", 4},
      {"past", 2},
      {"bicycle", 3},
  }};

  // std::string token = "avg";
  // ar::vector<ar::Dictionary> D = {{
  //   {"average", 3},
  // }};

  ar::Phi phi = [&token](std::string &word) {
    int max = std::min(token.size(), word.size());
    int counter = 0;
    for (auto index = 0; index < max; index++) {
      if (token[index] == word[index]) {
        counter++;
      }
    }
    if (counter == max) {
      return 1;
    }

    return 0;

    // return (int)floor(word.length() / log(word.length()));
  }; // tolerance function always returns 1

  ar::Cfunc cost = [&D](ar::string &word) {
    for (auto dict : D) {
      auto res = std::find_if(dict.begin(), dict.end(),
                              [&word](ar::pair<ar::string, int> &match) {
                                return match.first == word;
                              });
      if (res != dict.end()) {
        return res->second;
      }
    }
    return -1;
  }; // cost function returns length of word
  std::vector<std::string> labels = ar::string_matching(token, D[0], phi, cost);
  printf("DONE\n");
  for (std::string label : labels) {
    std::cout << "found: " << label << " ";
  }
  std::cout << std::endl;
  return 0;
}

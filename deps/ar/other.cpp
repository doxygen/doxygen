#include <cmath>
#include <math.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <ostream>
#include <fstream>

#include "ar.hpp"

using std::vector;
using std::string;

ar::Dictionary get_dict(string file) {
  ar::Dictionary retvec;

  std::fstream ifile;
  ifile.open(file);

  if(!ifile.is_open()) {
    std::cout << "Could not open file " << file << std::endl;
    exit(1);
  }

  string line;
  string delim = ";";
  while(std::getline(ifile, line)) {
    int first = line.find(delim);
    string word = line.substr(0, first);
    line.erase(0, first + delim.length());
    int other = -1;
    try {
      other = std::stoi(line.substr(0, first));
    } catch(std::invalid_argument e) {
      other = word.length() / std::log(word.length());
    }
    retvec.insert({word, other});
  }

  return retvec;
}

int main(void) {
  ar::Dictionaries D;
  auto eng = get_dict("wordlist.10000");
  auto it = get_dict("List_of_computing_and_IT_abbreviations");
  D.push_back(it);
  D.push_back(eng);

  std::string token = "getpnt";
  std::cout << "STARTED:" << std::endl;

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

  ar::Cfunc cost = [&D](std::string &word) {
    for (auto dict : D) {
      try {
        return dict.at(word);
      } catch(std::out_of_range e) {
        
      }
    }
    return -1;
  }; // cost function returns length of word
  std::vector<std::string> labels = ar::string_matching(token, eng, phi, cost);
  printf("DONE\n");
  for (std::string label : labels) {
    std::cout << "found: " << label << " ";
  }
  std::cout << std::endl;
  return 0;
}

int main2(void) {

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

  ar::Cfunc cost = [&D](std::string &word) {
    for (auto dict : D) {
      try {
        return dict.at(word);
      } catch(std::out_of_range e) {
        
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

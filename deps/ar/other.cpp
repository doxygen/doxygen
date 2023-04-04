#include <cmath>
#include <fstream>
#include <math.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ar.hpp"

using std::string;
using std::vector;

ar::Dictionary get_dict(string file) {
  ar::Dictionary retvec;

  std::fstream ifile;
  ifile.open(file);

  if (!ifile.is_open()) {
    std::cout << "Could not open file " << file << std::endl;
    exit(1);
  }

  string line;
  string delim = ";";
  while (std::getline(ifile, line)) {
    int first = line.find(delim);
    string word = line.substr(0, first);
    line.erase(0, first + delim.length());
    int other = -1;
    try {
      other = std::stoi(line.substr(0, first));
    } catch (std::invalid_argument e) {
      other = word.length() / std::log(word.length());
    }
    retvec.insert({word, other});
  }

  return retvec;
}

ar::Dictionaries get_true() {
  ar::Dictionaries D;
  auto eng = get_dict("wordlist.10000");
  auto it = get_dict("List_of_computing_and_IT_abbreviations");
  D.add(eng);
  D.add(it);
  return D;
}

ar::Dictionaries get_false() {
  ar::Dictionaries D({{
                          {"get", 3},
                          {"getter", 1},
                          {"great", 2},
                          {"hallo", 2},
                          {"pointer", 3},
                          {"point", 4},
                          {"past", 2},
                          {"bicycle", 3},
                      },
                      {
                          {"hello", 3},
                          {"help", 1},
                          {"hollow", 2},
                          {"hallo", 2},
                          {"heal", 1},
                          {"heel", 1},
                          {"basketball", 4},
                          {"bicycle", 3},
                      }});
  return D;
}

int main(void) {
  auto D = get_false();
  std::string token = "getpnt";
  std::cout << "STARTED" << std::endl;

  auto labels = ar::split_matching(token, D);

  std::cout << "GOT BACK" << std::endl;

  for (std::string label : labels) {
    std::cout << "found: " << label << " ";
  }

  std::cout << "FINISHED" << std::endl;

  std::cout << std::endl;
  return 0;
}

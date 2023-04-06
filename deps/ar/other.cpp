#include <cmath>
#include <filesystem>
#include <fstream>
#include <math.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ar.hpp"
#include "opts.hpp"

using std::string;

std::unordered_map<std::string, std::string> get_abbr(string file) {
  std::unordered_map<std::string, std::string> retvec;

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
    string other = line;
    retvec.insert({word, other});
  }

  return retvec;
}

ar::Dictionary get_dict(string file) {
  ar::Dictionary retvec;

  std::fstream ifile;
  ifile.open(file);

  if (!ifile.is_open()) {
    std::cout << "Could not open file " << file << std::endl;
    exit(1);
  }
  int file_size = std::count(std::istreambuf_iterator<char>(ifile),
                             std::istreambuf_iterator<char>(), '\n');

  ifile.clear();
  ifile.seekg(0);

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
      other = (word.length() / std::log(word.length()))/*  / file_size */;
    }
    retvec.insert({word, other});
  }

  return retvec;
}

ar::Dictionaries get_true() {
  ar::Dictionaries D;
  auto eng = get_dict("test.txt");
  auto it = get_dict("List_of_computing_and_IT_abbreviations");
  D.add(eng);
  D.add(it);
  D.known_abbr = get_abbr("known_abbr.txt");
  return D;
}

ar::Dictionaries get_false() {
  ar::Dictionaries D(
      {{
          {"get", 3},     {"getter", 1}, {"great", 2},      {"hallo", 2},
          {"pointer", 3}, {"point", 4},  {"past", 2},       {"bicycle", 3},
          {"hello", 3},   {"help", 1},   {"hollow", 2},     {"hallo", 2},
          {"heal", 1},    {"heel", 1},   {"basketball", 4}, {"bicycle", 3},
          {"average", 2}, {"sum", 3},    {"number", 2},     {"array", 2},
      }},
      get_abbr("known_abbr.txt"));
  return D;
}

int main(void) {
  auto D = get_true();
  std::string token = "arrayavgsumnumheelobvs";
  RESULT("STARTED: " << token)

  Lmatch matches;
  auto labels = ar::split_matching(token, D, &matches);

  auto thing = ar::expansion_matching(labels, D);

  // Replace the words in the identifier
  for (auto each : matches) {
    size_t index = 0;
    while (true) {
      /* Locate the substring to replace. */
      index = token.find(each, index);
      if (index == std::string::npos)
        break;

      /* Make the replacement. */
      token.replace(index, each.size(), each + "_");

      /* Advance index forward so the next iteration doesn't pick it up as
       * well.
       */
      index += each.size();
    }
  }

  for (auto &[each, word] : thing) {
    size_t index = 0;
    while (true) {
      /* Locate the substring to replace. */
      index = token.find(each, index);
      if (index == std::string::npos)
        break;

      /* Make the replacement. */
      token.replace(index, each.size(), word + "_");

      /* Advance index forward so the next iteration doesn't pick it up as
       * well.
       */
      index += word.size();
    }
  }

  RESULT(token)

  return 0;
}

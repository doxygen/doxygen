#include "fuzzy.h"

#include <iostream>

Matches fuzzy::BYP(std::string token, std::pair<std::string, int> word,
                   const Phi &p) {
  Matches matching_seq;

  size_t n = token.length();

  for (size_t i = 0; i < n; i++) {
    for (size_t j = i; j < n; j++) {
      std::string subtoken = token.substr(i, j - i + 1);
      if (subtoken.length() > word.first.length()) {
        continue;
      }
      double tolerance = p(subtoken, word.first);
      if(tolerance == -1) {
        continue;
      }

      if (subtoken.length() <= (word.first.length() + tolerance)) {
        double distance = 0;
        for (size_t k = 0; k < subtoken.length(); k++) {
          if (subtoken[k] != word.first[k]) {
            distance++;
          }
        }
        if (distance <= tolerance) {
          matching_seq.push_back({subtoken, word.first});
        }
      }
    }
  }
  return matching_seq;
}

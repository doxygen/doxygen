#include "fuzzy.h"

#include <iostream>

#define CHAR_MAX 256

std::string fuzzy::bitap_fuzzy_bitwise_search(std::string token,
                                              std::string pattern, int k) {
  std::string result;
  const char *text = token.c_str();
  int m = pattern.size();
  unsigned long pattern_mask[CHAR_MAX + 1];
  int i, d;

  if (pattern[0] == '\0')
    return text;
  if (m > 31)
    return "The pattern is too long!";

  /* Initialize the bit array R */
  unsigned long R[(k + 1)];
  for (i = 0; i <= k; ++i)
    R[i] = ~1;

  /* Initialize the pattern bitmasks */
  for (i = 0; i <= CHAR_MAX; ++i)
    pattern_mask[i] = ~0;
  for (i = 0; i < m; ++i)
    pattern_mask[pattern[i]] &= ~(1UL << i);

  for (i = 0; text[i] != '\0'; ++i) {
    /* Update the bit arrays */
    unsigned long old_Rd1 = R[0];

    R[0] |= pattern_mask[text[i]];
    R[0] <<= 1;

    for (d = 1; d <= k; ++d) {
      unsigned long tmp = R[d];
      /* Substitution is all we care about */
      R[d] = (old_Rd1 & (R[d] | pattern_mask[text[i]])) << 1;
      old_Rd1 = tmp;
    }

    if (0 == (R[k] & (1UL << m))) {
      result = (text + i - m) + 1;
      break;
    }
  }

  return result;
}

matches
fuzzy::BYP(std::string token, std::pair<std::string, int> word,
           Phi p) {
  matches matching_seq;

  int n = token.length();
  int m = word.first.length();

  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      std::string subtoken = token.substr(i, j - i + 1);
      if(subtoken.length() > word.first.length()) {
        continue;
      }
      if(subtoken[0] != word.first[0]) {
        continue;
      }
      double tolerance = p(subtoken, word.first);
      // if(tolerance == 0 && subtoken == word.first) {
      //     matching_seq.push_back({subtoken, word.first});
      //     continue;
      // }

      // auto res = bitap_fuzzy_bitwise_search(word.first, subtoken, tolerance);
      // if (res == "\0") {
      //   continue;
      // }

      if (subtoken.length() <= (word.first.length() + tolerance)) {
        double distance = 0;
        for (int k = 0; k < subtoken.length(); k++) {
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

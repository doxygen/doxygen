#include "fuzzy.h"

#include <iostream>

#define CHAR_MAX 256

using std::function;

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

using matches = std::vector<std::pair<std::string, std::string>>;

std::vector<std::pair<std::string, std::string>>
fuzzy::BYP(std::string token, std::pair<std::string, int> word,
           Phi p) {
  matches matching_seq;

  int n = token.length();
  int m = word.first.length();

  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      std::string subtoken = token.substr(i, j - i + 1);
      double tolerance = p(subtoken, word.first);

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

#define SIZE 256    /* size of alpha index and count array */
#define MOD256 0xFF /* for the mod operation */
struct anode {      /* structure for index of alphabet */
  int offset;       /* distance of char from start of pattern */
  anode *next;      /* pointer to next idxnode if it exists */
};

anode alpha[SIZE]; /* offset for each alphabetic character */
int count[SIZE];   /* count of the characters that don't match */
void search(char *t, int n, int m, int k, anode alpha[],
            int count[]) /* string searching with mismatches */
{
  int i, off1;
  anode *aptr;
  for (i = 0; i < n; i++) {
    if ((off1 = (aptr = &alpha[*t++])->offset) >= 0) {
      count[(i + off1) & MOD256]--;
      for (aptr = aptr->next; aptr != NULL; aptr = aptr->next) {
        count[(i + aptr->offset) & MOD256]--;
      }
    }
    if (count[i & MOD256] <= k) {
      printf("Match in position %d with %d mismatches\n", i - m + 1,
             count[i & MOD256]);
      count[i & MOD256] = m;
    }
  }
}

void preprocess(char *p, int m, anode alpha[],
                int count[]) /* preprocessing routine */
{
  int i, j;
  anode *aptr;
  for (i = 0; i < SIZE; i++) {
    alpha[i].offset = -1;
    alpha[i].next = NULL;
    count[i] = m;
  }
  for (i = 0, j = 128; i < m; i++, p++) {
    count[i] = SIZE;
    if (alpha[*p].offset == -1)
      alpha[*p].offset = m - i - i;
    else {
      aptr = alpha[*p].next;
      alpha[*p].next = &alpha[j++];
      alpha[*p].next->offset = m - i - 1;
      alpha[*p].next->next = aptr;
      count[m - 1] = m;
    }
  }
}

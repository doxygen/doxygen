/******************************************************************************
 *
 * Copyright (C) 1997-2025 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef FREGEX_H
#define FREGEX_H

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <iterator>

#include "construct.h"

/** Namespace for the regular expression functions */
namespace reg
{

class Match;

/** Class representing a regular expression.
 *
 *  It has a similar API as `std::regex`,
 *  but is much faster (and also somewhat more limited).
 */
class Ex
{
  public:
    /** Matching algorithm */
    enum class Mode
    {
      RegEx,    /**< full regular expression. */
      Wildcard  /**< simple globbing pattern. */
    };
    /** Creates a regular expression object given the pattern as a string.
     *  Two modes of matching are supported: RegEx and Wildcard
     *
     *  The following special characters are supported in Mode::RegEx mode.
     *  - `c` matches character `c`
     *  - `.` matches any character
     *  - `^` matches the start of the input
     *  - `$` matches the end of the input
     *  - `\<` matches the start of a word
     *  - `\>` matches the end of a word
     *  - `[]` matches a set of characters
     *  - `x*` matches a sequence of zero or more `x`'s
     *  - `x+` matches a sequence of one or more `x`'s
     *  - `x?` matches an optional `x`
     *  - `(`  matches the start of a capture range
     *  - `)`  matches the ends a capture range
     *  - `\c` to escape a special character, such as `+`, `[`, `*`, `(`, etc.
     *  - `\t` matches a tab character
     *  - `\n` matches a newline character
     *  - `\r` matches a return character
     *  - `\s` matches any whitespace as defined by `std::isspace()`
     *  - `\d` matches any digit as defined by `std::digit()`
     *  - `\a` matches any alphabetical characters, same as `[a-z_A-Z\x80-\xFF]`
     *  - `\w` matches any alpha numerical character, same as `[a-z_A-Z0-9\x80-\xFF]`
     *  - `\xHH` matches a hexadecimal character, e.g. `\xA0` matches character code 160.
     *
     *  A character range can be used to match a character that falls inside a range
     *  (or set of ranges).
     *  Within the opening `[` and closing `]` brackets of a character ranges the following
     *  is supported:
     *  - `^` if at the start of the range, a character matches if it is \e not in the range,
     *        e.g. `[^\d]` matches any character not a digit
     *  - `-` when placed between 2 characters it defines a range from the first character to the second.
     *        any character that falls in the range will match, e.g. [0-9] matches the digit from 0 to 9.
     *  - `\s`, `\d`, `\a`, and `\w` as explained above.
     *
     *  @note that special characters `.`, `*`, `?`, `$`, `+`, `[` do not have a special
     *  meaning in a character range. `^` only has a special meaning as the first character.
     *
     *  @note capture ranges can be nested. Optional capture ranges `(...)?` are supported but
     *  repeated ranges `(...)*` or `(...)+` are not.
     *
     *  In Wildcard mode `*` is used to match any sequence of zero or more characters.
     *  The character `?` can be used to match an optional character. Character ranges are
     *  also supported, but other characters like `$` and `+` are just treated as
     *  literal characters.
     *
     */
    Ex(std::string_view pattern, Mode mode=Mode::RegEx);

    /** Destroys the regular expression object. Frees resources. */
   ~Ex();

    /** Check if a given string matches this regular expression.
     *  @param str   The input string to match against.
     *  @param match The match object to hold the matching results.
     *  @param pos   The position in the string at which to start the match.
     *  @returns true iff a match is found. Details are stored in the match object.
     */
    bool match(std::string_view str,Match &match,size_t pos=0) const;
    bool isValid() const;
  private:
    NON_COPYABLE(Ex)

    class Private;
    std::unique_ptr<Private> p;
};

/** Object representing the match results of a capture range. */
class SubMatch
{
  public:
    /** Creates a match for a single capture range given a non-owning pointer to the string. */
    SubMatch(std::string_view str) : m_str(str) {}

    /** Returns the position in the string at which the match starts. */
    size_t position()    const { return m_pos; }

    /** Returns the length of the matching part. */
    size_t length()      const { return m_len; }

    /** Returns the matching part as a string */
    std::string str()    const { return std::string{m_str.substr(m_pos,m_len)}; }

  private:
    friend class Match;
    void setStart(size_t pos) { m_pos=pos; }
    void setEnd(size_t pos)   { m_len=pos-m_pos; }
    void setMatch(size_t pos,size_t len) { m_pos=pos; m_len=len; }
    size_t m_pos = std::string::npos;
    size_t m_len = std::string::npos;
    std::string_view m_str;
};

/** Object representing the matching results. It consists of an array of
 *  SubMatch objects. The first entry of the array represents the whole match, any
 *  next elements represent each of the capture ranges.
 *
 *  For example string `@42` and expression `@(\\d+)` will have two
 *  Submatches, match[0] will point to the input string as a whole, and
 *  match[1] will point to the number 42 only.
 *
 */
class Match
{
  public:
    /** Creates an empty match object */
    Match() {}

    /** Returns the position of the match or std::string::npos if no position is set. */
    size_t position() const { return m_subMatches[0].position(); }

    /** Returns the position of the match or std::string::npos if no length is set. */
    size_t length()   const { return m_subMatches[0].length();   }

    /** Return a string representing the matching part. */
    std::string str() const { return std::string{m_subMatches[0].str()}; }

    /** Return the part of the string before the match */
    SubMatch prefix() const { SubMatch m(m_str); m.setMatch(0,position()); return m; }

    /** Return the part of the string after the match */
    SubMatch suffix() const
    {
      SubMatch m(m_str);
      if (!m_str.empty())
      {
        size_t e = position()+length();
        m.setMatch(e,m_str.length()-e);
      }
      return m;
    }

    /** Returns the number of sub matches available in this match. */
    size_t size()     const { return m_subMatches.size(); }

    /** Returns the n-th SubMatch object. Note that there is always 1 SubMatch object
     *  representing the whole match.
     */
    const SubMatch &operator[](size_t index) const { return m_subMatches[index]; }

  private:
    friend class Ex;
    void init(std::string_view str,size_t captureCount)
    {
      m_subMatches.clear();
      m_subMatches.reserve(captureCount+1);
      for (size_t i=0;i<captureCount+1;i++)
      {
        m_subMatches.emplace_back(str);
      }
      m_str = str;
    }
    void startCapture(size_t groupId,size_t index)
    {
      if (groupId < m_subMatches.size())
      {
        m_subMatches[groupId].setStart(index);
      }
    }
    void endCapture(size_t groupId,size_t index)
    {
      if (groupId < m_subMatches.size())
      {
        if (index>=m_subMatches[groupId].position())
        {
          m_subMatches[groupId].setEnd(index);
        }
      }
    }
    void setMatch(size_t pos,size_t len)
    {
      // Always set the whole match
      m_subMatches[0].setMatch(pos,len);
    }

    std::vector<SubMatch> m_subMatches;
    std::string_view m_str;
};

/** Class to iterate through matches.
 */
class Iterator
{
  public:
    using value_type = Match;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::forward_iterator_tag;

    /** Creates an end-of-sequence iterator */
    Iterator() {}

    /** Creates an iterator for input string \a str, using regular expression \a re to search.
     *  @note the string and regular expression objects should remain valid while iterating.
     */
    Iterator(std::string_view str, const Ex &re, size_t pos=0)
      : m_str(str), m_re(&re), m_pos(pos) { findNext(); }

    // Iterator holds pointers, so prevent temporaries to be passed as string or
    // regular expression
    Iterator(std::string &&str, const Ex &re) = delete;
    Iterator(const std::string &str, Ex &&re) = delete;
    Iterator(std::string &&str, Ex &&re) = delete;

    /** Returns true if the iterators point to the same match (or both are end-of-sequence iterators) */
    bool operator==(const Iterator &rhs) const { return rhs.m_pos==m_pos; }

    /** Returns true if the iterators are not pointing to the same match */
    bool operator!=(const Iterator &rhs) const { return rhs.m_pos!=m_pos; }

    /** Returns a reference to the current match */
    const value_type &operator*()        const { return m_match; }

    /** Returns a pointer to the current match */
    const value_type *operator->()       const { return &m_match;  }

    /** Advances the iterator to the next match. */
    Iterator &operator++() { findNext(); return *this; }

  private:
    void findNext()
    {
      if (!m_re || m_str.empty()) { m_pos=std::string::npos; return; } // end marker
      if (m_re->match(m_str,m_match,m_pos))
      {
        m_pos=m_match.position()+m_match.length(); // update m_pos to point beyond last match
      }
      else // no more matches, make the iterator point to the 'end-of-sequence'
      {
        m_pos=std::string::npos;
      }
    }
    std::string_view m_str;
    const Ex *m_re = nullptr;
    size_t m_pos = std::string::npos;
    Match m_match;
};

/** Search in a given string \a str starting at position \a pos for a match against regular expression \a re.
 *  Returns true iff a match was found.
 *  Details of what part of the string has matched is returned via the \a match object.
 *
 *  An example to show how to match all identifiers in a string.
 *  @code
 *  static reg::Ex re(R"(\a\w*)");
 *  std::string = u8"void(Func是<B_C::Códe42>(42));";
 *  while (reg::search(str,match,re,pos))
 *  {
 *    std::cout << match.str() << std::endl;
 *    pos=match.position()+match.length();
 *  }
 *  @endcode
 *  produces:
 *  @code
 *  void
 *  Func是
 *  B_C
 *  Códe42
 *  @endcode
 *
 *  @see Ex::Ex() for details on the regular expression patterns.
 */
bool search(std::string_view str,Match &match,const Ex &re,size_t pos=0);

/** Search in a given string \a str starting at position \a pos for a match against regular expression \a re.
 *  Returns true iff a match was found.
 */
bool search(std::string_view str,const Ex &re,size_t pos=0);

/** Matches a given string \a str for a match against regular expression \a re.
 *  Returns true iff a match was found for the whole string.
 *  Any capture groups are returned via the \a match object.
 */
bool match(std::string_view str,Match &match,const Ex &re);

/** Matches a given string \a str for a match against regular expression \a re.
 *  Returns true iff a match was found for the whole string.
 */
bool match(std::string_view str,const Ex &re);

/** Searching in a given input string \a for parts that match regular expression \a re and
 *  replaces those parts by string \a replacement.
 */
std::string replace(std::string_view str,const Ex &re,std::string_view replacement);

} // namespace

#endif

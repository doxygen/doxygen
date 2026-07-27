/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <string_view>

/** @file
 *  @brief Some helper functions for std::string
 */

/** Replaces occurrences of substring \a toReplace in string \a s with string \a replaceWith.
 *  Modifies \a s in place.
 */
inline void substituteInplace(std::string &s,
                              std::string_view toReplace,std::string_view replaceWith)
{
  std::string buf;
  size_t pos = 0;
  size_t prevPos = 0;
  buf.reserve(s.length());

  while ((pos=s.find(toReplace, prevPos))!=std::string::npos)
  {
    buf.append(s, prevPos, pos - prevPos);
    buf += replaceWith;
    prevPos = pos + toReplace.length();
  }
  buf.append(s, prevPos, s.size() - prevPos);
  s.swap(buf);
}

/** Returns a new string where occurrences of substring \a toReplace in string \a s are replaced by
 *  string \a replaceWith.
 */
inline std::string substituteStringView(std::string_view s,
                              std::string_view toReplace,std::string_view replaceWith)
{
  std::string buf;
  size_t pos = 0;
  size_t prevPos = 0;
  buf.reserve(s.length());

  while ((pos=s.find(toReplace, prevPos))!=std::string::npos)
  {
    buf.append(s, prevPos, pos - prevPos);
    buf += replaceWith;
    prevPos = pos + toReplace.length();
  }
  buf.append(s, prevPos, s.size() - prevPos);
  return buf;
}


/** Given a string view \a s, returns a new, narrower view on that string, skipping over any
 *  leading or trailing whitespace characters.
 */
inline std::string_view stripWhiteSpace(std::string_view s)
{
  static auto isspace = [](char c){ return c==' ' || c=='\t' || c=='\n' || c=='\r'; };
  size_t sl = s.length();
  if (sl==0 || (!isspace(s[0]) && !isspace(s[sl-1]))) return s;
  size_t start=0, end=sl-1;
  while (start<sl && isspace(s[start])) start++;
  if (start==sl) return s.substr(0,0); // only whitespace
  while (end>start && isspace(s[end])) end--;
  return s.substr(start,end+1-start);
}

inline void addTerminalCharIfMissing(std::string &s,char c)
{
  if (s.empty())
  {
    s+=c;
  }
  else
  {
    if (s[s.length()-1]!=c) s+=c;
  }
}

/// returns TRUE iff \a data points to a substring that matches string literal \a str
template <size_t N>
bool literal_at(const char *data,const char (&str)[N])
{
  size_t len = N-1; // exclude 0 terminator
  return data!=nullptr && data[0]==str[0] && qstrncmp(data+1,str+1,len-1)==0;
}

/// returns TRUE iff \a data points to a substring that matches string literal \a str
template <size_t N>
bool literal_at(std::string_view data,const char (&str)[N])
{
  size_t len = N-1; // exclude 0 terminator
  return len<=data.size() && data[0]==str[0] && qstrncmp(data.data()+1,str+1,len-1)==0;
}

#endif // STRINGUTIL_H

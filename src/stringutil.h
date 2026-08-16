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

#include "containers.h"
#include "regex.h"

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

/// returns true iff \a data points to a substring that matches string literal \a str
template <size_t N>
bool literal_at(const char *data,const char (&str)[N])
{
  size_t len = N-1; // exclude 0 terminator
  return data!=nullptr && data[0]==str[0] && dstrncmp(data+1,str+1,len-1)==0;
}

/// returns true iff \a data points to a substring that matches string literal \a str
template <size_t N>
bool literal_at(std::string_view data,const char (&str)[N])
{
  size_t len = N-1; // exclude 0 terminator
  return len<=data.size() && data[0]==str[0] && dstrncmp(data.data()+1,str+1,len-1)==0;
}

/// split input string \a s by string delimiter \a delimiter.
/// returns a vector of non-empty strings that are between the delimiters
inline StringVector split(const std::string &s,const std::string &delimiter)
{
  StringVector result;
  size_t prev = 0, pos = 0, len = s.length();
  do
  {
    pos = s.find(delimiter, prev);
    if (pos == std::string::npos) pos = len;
    if (pos>prev) result.push_back(s.substr(prev,pos-prev));
    prev = pos + delimiter.length();
  }
  while (pos<len && prev<len);
  return result;
}

/// split input string \a s by regular expression delimiter \a delimiter.
/// returns a vector of non-empty strings that are between the delimiters
inline StringVector split(const std::string &s,const reg::Ex &delimiter)
{
  StringVector result;
  reg::Iterator iter(s, delimiter);
  reg::Iterator end;
  size_t p=0;
  for ( ; iter != end; ++iter)
  {
    const auto &match = *iter;
    size_t i=match.position();
    size_t l=match.length();
    if (i>p) result.push_back(s.substr(p,i-p));
    p=i+l;
  }
  if (p<s.length()) result.push_back(s.substr(p));
  return result;
}

/// create a string where the string in the vector are joined by the given delimiter
inline std::string join(const StringVector &sv,const std::string &delimiter)
{
  std::string result;
  bool first=true;
  for (const auto &s : sv)
  {
    if (!first) result+=delimiter;
    first=false;
    result+=s;
  }
  return result;
}

/// find the index of a string in a vector of strings, returns std::string::npos if the string could not be found
inline size_t findIndex(const StringVector &sv,const std::string &s)
{
  auto it = std::find(sv.begin(),sv.end(),s);
  return it!=sv.end() ? static_cast<size_t>(it-sv.begin()) : std::string::npos;
}

/// find the index of the first occurrence of pattern \a re in a string \a s
/// returns std::string::npos if the pattern could not be found
inline size_t findIndex(const std::string &s,const reg::Ex &re)
{
  reg::Match match;
  return reg::search(s,match,re) ? static_cast<size_t>(match.position()) : std::string::npos;
}

inline std::string removeEmptyLines(const std::string &s)
{
  std::string out;
  out.reserve(s.length());
  const char *p=s.data();
  if (p)
  {
    char c = 0;
    while ((c=*p++))
    {
      if (c=='\n')
      {
        const char *e = p;
        while (*e==' ' || *e=='\t') e++;
        if (*e=='\n')
        {
          p=e;
        }
        else out+=c;
      }
      else
      {
        out+=c;
      }
    }
  }
  return out;
}


#endif // STRINGUTIL_H

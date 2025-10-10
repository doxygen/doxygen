/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

// STL includes
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

// Xapian includes
#include <xapian.h>

#include "version.h"


constexpr Xapian::valueno FIELD_TYPE{ 1 };
constexpr Xapian::valueno FIELD_NAME{ 2 };
constexpr Xapian::valueno FIELD_ARGS{ 3 };
constexpr Xapian::valueno FIELD_TAG{ 4 };
constexpr Xapian::valueno FIELD_URL{ 5 };
constexpr Xapian::valueno FIELD_KEYW{ 6 };
constexpr Xapian::valueno FIELD_DOC{ 7 };

constexpr int             hex2dec(unsigned char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}


bool dirExists(const std::string &dirname)
{
  std::error_code ec;
  return std::filesystem::is_directory(dirname, ec);
}


/** decodes a URI encoded string into a normal string. */
static std::string uriDecode(const std::string & sSrc)
{
  // Note from RFC1630: "Sequences which start with a percent
  // sign but are not followed by two hexadecimal characters
  // (0-9, A-F) are reserved for future extension"

  const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
  const size_t SRC_LEN = sSrc.length();
  const unsigned char * const SRC_END = pSrc + SRC_LEN;
  // last decodable '%'
  const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

  char * const pStart = new char[SRC_LEN];
  char * pEnd = pStart;

  while (pSrc < SRC_LAST_DEC)
  {
    if (*pSrc == '%') // replace %2A with corresponding ASCII character
    {
      int           dec1, dec2;
      unsigned char c1=*(pSrc+1);
      unsigned char c2=*(pSrc+2);
      if (-1 != (dec1 = hex2dec(c1)) && -1 != (dec2 = hex2dec(c2)))
      {
        *pEnd++ = (dec1 << 4) + dec2;
        pSrc += 3;
        continue;
      }
    }
    else if (*pSrc == '+') // replace '+' with space
    {
      *pEnd++ = ' '; pSrc++;
      continue;
    }
    *pEnd++ = *pSrc++;
  }

  // the last 2- chars
  while (pSrc < SRC_END) *pEnd++ = *pSrc++;

  std::string sResult(pStart, pEnd);
  delete [] pStart;
  return sResult;
}

/** return list of strings that result when splitting \a s using
 *  delimiter \a delim
 */
static std::vector<std::string> split(const std::string &s, char delim)
{
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim)) elems.push_back(item);
  return elems;
}

/** Read type T from string \a s */
template<class T>
T fromString(const std::string& s)
{
  std::istringstream stream (s);
  T t;
  stream >> t;
  return t;
}

/** Class that holds the starting position of a word */
struct WordPosition
{
  WordPosition(size_t s,size_t i) : start(s), index(i) {}
  size_t start;
  size_t index;
};

/** Class representing the '<' operator for WordPosition objects based on position. */
struct WordPosition_less
{
  bool operator()(const WordPosition &p1,const WordPosition &p2) const
  {
    return p1.start<p2.start;
  }
};

/** Class that holds a text fragment */
struct Fragment
{
  Fragment(const std::string &t,int occ) : text(t), occurrences(occ) {}
  std::string text;
  int occurrences;
};

/** Class representing the '>' operator for Fragment objects based on occurrence. */
struct Fragment_greater
{
  bool operator()(const Fragment &p1,const Fragment &p2) const
  {
    return p1.occurrences>p2.occurrences;
  }
};

/** Class representing a range within a string */
struct Range
{
  Range(size_t s,size_t e) : start(s), end(e) {}
  size_t start;
  size_t end;
};

/** Returns true if [start..start+len] is inside one of the \a ranges. */
static bool insideRange(const std::vector<Range> &ranges,size_t start,size_t len)
{
  for (std::vector<Range>::const_iterator it = ranges.begin();
       it!=ranges.end(); ++it
      )
  {
    Range r = *it;
    if (start>=r.start && start+len<r.end)
    {
      return true;
    }
  }
  return false;
}

/** Returns a list of text \a fragments from \a s containing one or
 *  more \a words. The list is sorted according to the
 *  number of occurrences of words within the fragment.
 */
static void highlighter(const std::string &s,
                 const std::vector<std::string> &words,
                 std::vector<Fragment> &fragments)
{
  const std::string spanStart="<span class=\"hl\">";
  const std::string spanEnd="</span>";
  const std::string dots="...";
  const size_t fragLen = 60;
  size_t sl=s.length();

  // find positions of words in s
  int j=0;
  std::vector<WordPosition> positions;
  for (std::vector<std::string>::const_iterator it=words.begin();
       it!=words.end();
       ++it,++j
      )
  {
    size_t pos=0;
    size_t i;
    std::string word = *it;
    while ((i=s.find(word,pos))!=std::string::npos)
    {
      positions.push_back(WordPosition(i,j));
      pos=i+word.length();
    }
  }
  // sort on position
  std::sort(positions.begin(),positions.end(),WordPosition_less());
  // get fragments around words
  std::vector<Range> ranges;
  for (std::vector<WordPosition>::const_iterator it=positions.begin();
       it!=positions.end();
       ++it)
  {
    WordPosition wp = *it;
    std::string w = words[wp.index];
    size_t i=wp.start;
    size_t wl=w.length();
    if (!insideRange(ranges,i,wl))
    {
      if (wl>fragLen)
      {
        fragments.push_back(Fragment(spanStart+w+spanEnd,1));
        ranges.push_back(Range(i,i+wl));
      }
      else
      {
        std::string startFragment,endFragment;
        int bi=static_cast<int>(i)-static_cast<int>((fragLen-wl)/2);
        size_t ei=i+wl+(fragLen-wl)/2;
        int occ=0;
        if (bi<0)  { ei-=bi; bi=0; } else startFragment=dots;
        if (ei>sl) { ei=sl;        } else endFragment=dots;
        while (bi>0  && !isspace(s[bi])) bi--; // round to start of the word
        while (ei<sl && !isspace(s[ei])) ei++; // round to end of the word
        // highlight any word in s between indexes bi and ei
        std::string fragment=startFragment;
        size_t pos=bi;
        for (std::vector<WordPosition>::const_iterator it2=positions.begin();
            it2!=positions.end();
            ++it2)
        {
          WordPosition wp2 = *it2;
          std::string w2 = words[wp2.index];
          size_t wl2 = w2.length();
          if (wp2.start>=bi && wp2.start+wl2<=ei) // word is inside the range!
          {
            fragment+=s.substr(pos,wp2.start-pos)+
              spanStart+
              s.substr(wp2.start,wl2)+
              spanEnd;
            pos=wp2.start+wl2;
            occ++;
          }
        }
        fragment+=s.substr(pos,ei-pos)+endFragment;
        fragments.push_back(Fragment(fragment,occ));
        ranges.push_back(Range(bi,ei));
      }
    }
  }
  std::sort(fragments.begin(),fragments.end(),Fragment_greater());
}

/** Escapes a string such that is can be included in a JSON structure */
static std::string escapeString(const std::string &s)
{
  std::stringstream dst;
  for (size_t i=0;i<s.length();i++)
  {
    char ch = s[i];
    switch (ch)
    {
      case '\"': dst << "\\\""; break;
      default: dst << ch; break;
    }
  }
  return dst.str();
}

static void showError(const std::string &callback,const std::string &error)
{
  std::cout << callback << "({\"error\":\"" << error << "\"})";
  exit(0);
}

static void usage(const char *name, int exitVal = 1)
{
  std::cerr << "Usage: " << name << "[query_string]" << std::endl;
  std::cerr << "       " << "alternatively the query string can be given by the environment variable QUERY_STRING" << std::endl;
  exit(exitVal);
}

/** Main routine */
int main(int argc,char **argv)
{
  // process inputs that were passed to us via QUERY_STRING
  std::string callback;
  try
  {
    std::string queryString;
    if (argc == 1)
    {
      const char *queryEnv = getenv("QUERY_STRING");
      if (queryEnv)
      {
        queryString = queryEnv;
      }
      else
      {
        usage(argv[0]);
      }
    }
    else if (argc == 2)
    {
      if (std::string(argv[1])=="-h" || std::string(argv[1])=="--help")
      {
        usage(argv[0],0);
      }
      else if (std::string(argv[1])=="-v" || std::string(argv[1])=="--version")
      {
        std::cerr << argv[0] << " version: " << getFullVersion() << std::endl;
        exit(0);
      }
      else
      {
        queryString = argv[1];
      }
    }
    else
    {
      usage(argv[0]);
    }

    std::cout << "Content-Type:application/javascript;charset=utf-8\r\n\n";
    // parse query string
    std::vector<std::string> parts = split(queryString,'&');
    std::string              searchFor;
    int num=1,page=0;
    for (std::vector<std::string>::const_iterator it=parts.begin();it!=parts.end();++it)
    {
      std::vector<std::string> kv = split(*it,'=');
      if (kv.size()==2)
      {
        std::string val = uriDecode(kv[1]);
        if      (kv[0]=="q")  searchFor = val;
        else if (kv[0]=="n")  num       = fromString<int>(val);
        else if (kv[0]=="p")  page      = fromString<int>(val);
        else if (kv[0]=="cb") callback  = val;
      }
    }

    std::string indexDir = "doxysearch.db";

    if (queryString=="test") // user test
    {
      bool dbOk = dirExists(indexDir);
      if (dbOk)
      {
        std::cout << "Test successful.";
      }
      else
      {
        std::cout << "Test failed: cannot find search index " << indexDir;
      }
      exit(0);
    }

    // create query
    Xapian::Database db(indexDir);
    Xapian::Enquire enquire(db);

    std::vector<std::string> words = split(searchFor,' ');
    Xapian::QueryParser parser;
    parser.set_database(db);
    parser.set_default_op(Xapian::Query::OP_AND);
    parser.set_stemming_strategy(Xapian::QueryParser::STEM_ALL);
    Xapian::termcount max_expansion=100;
#if (XAPIAN_MAJOR_VERSION==1) && (XAPIAN_MINOR_VERSION==2)
    parser.set_max_wildcard_expansion(max_expansion);
#else
    parser.set_max_expansion(max_expansion,Xapian::Query::WILDCARD_LIMIT_MOST_FREQUENT);
#endif
    Xapian::Query query=parser.parse_query(searchFor,
                                           Xapian::QueryParser::FLAG_DEFAULT  |
                                           Xapian::QueryParser::FLAG_WILDCARD |
                                           Xapian::QueryParser::FLAG_PHRASE   |
                                           Xapian::QueryParser::FLAG_PARTIAL
                                          );
    enquire.set_query(query);

    // get results
    Xapian::MSet matches = enquire.get_mset(page*num,num);
    unsigned int hits    = matches.get_matches_estimated();
    unsigned int offset  = page*num;
    unsigned int pages   = num>0 ? (hits+num-1)/num : 0;
    if (offset>hits)     offset=hits;
    if (offset+num>hits) num=hits-offset;

    // write results as JSONP
    std::cout << callback.c_str() << "(";
    std::cout << "{" << std::endl
              << "  \"hits\":"   << hits   << "," << std::endl
              << "  \"first\":"  << offset << "," << std::endl
              << "  \"count\":"  << num    << "," << std::endl
              << "  \"page\":"   << page   << "," << std::endl
              << "  \"pages\":"  << pages  << "," << std::endl
              << "  \"query\": \""  << escapeString(searchFor)  << "\"," << std::endl
              << "  \"items\":[" << std::endl;
    // foreach search result
    unsigned int o = offset;
    for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i,++o)
    {
      std::vector<Fragment> hl;
      Xapian::Document doc = i.get_document();
      highlighter(doc.get_value(FIELD_DOC),words,hl);
      std::cout << "  {\"type\": \"" << doc.get_value(FIELD_TYPE) << "\"," << std::endl
                << "   \"name\": \"" << doc.get_value(FIELD_NAME) << escapeString(doc.get_value(FIELD_ARGS)) << "\"," << std::endl
                << "   \"tag\": \""  << doc.get_value(FIELD_TAG) << "\"," << std::endl
                << "   \"url\": \""  << doc.get_value(FIELD_URL) << "\"," << std::endl;
      std::cout << "   \"fragments\":[" << std::endl;
      int c=0;
      bool first=true;
      for (std::vector<Fragment>::const_iterator it = hl.begin();it!=hl.end() && c<3;++it,++c)
      {
        if (!first) std::cout << "," << std::endl;
        std::cout << "     \"" << escapeString((*it).text) << "\"";
        first=false;
      }
      if (!first) std::cout << std::endl;
      std::cout << "   ]" << std::endl;
      std::cout << "  }";
      if (o<offset+num-1) std::cout << ",";
      std::cout << std::endl;
    }
    std::cout << " ]" << std::endl << "})" << std::endl;
  }
  catch (const Xapian::Error &e) // Xapian exception
  {
    showError(callback,e.get_description());
  }
  catch (...) // Any other exception
  {
    showError(callback,"Unknown Exception!");
    exit(1);
  }
  return 0;
}

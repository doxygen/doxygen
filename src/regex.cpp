/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include "regex.h"
#include <cstdint>
#include <vector>
#include <cctype>
#include <cassert>
#include <algorithm>

#define ENABLE_DEBUG 0
#if ENABLE_DEBUG
#define DBG(fmt,...) do { fprintf(stderr,fmt,__VA_ARGS__); } while(0)
#else
#define DBG(fmt,...) do {} while(0)
#endif

namespace reg
{

/** Class representing a token in the compiled regular expression token stream.
 *  A token has a kind and an optional value whose meaning depends on the kind.
 *  It is also possible to store a (from,to) character range in a token.
 */
class PToken
{
  public:
    /** The kind of token.
     *
     *  Ranges per bit mask:
     *  - `0x00FF` from part of a range, except for `0x0000` which is the End marker
     *  - `0x1FFF` built-in ranges
     *  - `0x2FFF` user defined ranges
     *  - `0x4FFF` special operations
     *  - `0x8000` literal character
     */
    enum class Kind : uint16_t
    {
      End            = 0x0000,
      WhiteSpace     = 0x1001,    // \s    range [ \t\r\n]
      Digit          = 0x1002,    // \d    range [0-9]
      Alpha          = 0x1003,    // \a    range [a-z_A-Z\x80-\xFF]
      AlphaNum       = 0x1004,    // \w    range [a-Z_A-Z0-9\x80-\xFF]
      CharClass      = 0x2001,    // []
      NegCharClass   = 0x2002,    // [^]
      BeginOfLine    = 0x4001,    // ^
      EndOfLine      = 0x4002,    // $
      BeginOfWord    = 0x4003,    // \<
      EndOfWord      = 0x4004,    // \>
      BeginCapture   = 0x4005,    // (
      EndCapture     = 0x4006,    // )
      Any            = 0x4007,    // .
      Star           = 0x4008,    // *
      Optional       = 0x4009,    // ?
      Character      = 0x8000     // c
    };

    /** returns a string representation of the tokens kind (useful for debugging). */
    const char *kindStr() const
    {
      if ((m_rep>>16)>=0x1000 || m_rep==0)
      {
        switch(static_cast<Kind>((m_rep>>16)))
        {
          case Kind::End:           return "End";
          case Kind::Alpha:         return "Alpha";
          case Kind::AlphaNum:      return "AlphaNum";
          case Kind::WhiteSpace:    return "WhiteSpace";
          case Kind::Digit:         return "Digit";
          case Kind::CharClass:     return "CharClass";
          case Kind::NegCharClass:  return "NegCharClass";
          case Kind::Character:     return "Character";
          case Kind::BeginOfLine:   return "BeginOfLine";
          case Kind::EndOfLine:     return "EndOfLine";
          case Kind::BeginOfWord:   return "BeginOfWord";
          case Kind::EndOfWord:     return "EndOfWord";
          case Kind::BeginCapture:  return "BeginCapture";
          case Kind::EndCapture:    return "EndCapture";
          case Kind::Any:           return "Any";
          case Kind::Star:          return "Star";
          case Kind::Optional:      return "Optional";
        }
      }
      else
      {
        return "Range";
      }
    }

    /** Creates a token of kind 'End' */
    PToken() : m_rep(0) {}

    /** Creates a token of the given kind \a k */
    explicit PToken(Kind k)    : m_rep(static_cast<uint32_t>(k)<<16) {}

    /** Create a token for an ASCII character */
    PToken(char c)             : m_rep((static_cast<uint32_t>(Kind::Character)<<16) |
                                        static_cast<uint32_t>(c)) {}

    /** Create a token for a byte of an UTF-8 character */
    PToken(uint16_t v)         : m_rep((static_cast<uint32_t>(Kind::Character)<<16) |
                                        static_cast<uint32_t>(v)) {}

    /** Create a token representing a range from one character \a from to another character \a to */
    PToken(uint16_t from,uint16_t to) : m_rep(static_cast<uint32_t>(from)<<16 | to) {}

    /** Sets the value for a token */
    void setValue(uint16_t value) { m_rep = (m_rep & 0xFFFF0000) | value; }

    /** Returns the kind of the token */
    Kind kind() const             { return static_cast<Kind>(m_rep>>16); }

    /** Returns the 'from' part of the character range. Only valid if this token represents a range */
    uint16_t from() const         { return m_rep>>16; }

    /** Returns the 'to' part of the character range. Only valid if this token represents a range */
    uint16_t to() const           { return m_rep & 0xFFFF; }

    /** Returns the value for this token */
    uint16_t value() const        { return m_rep & 0xFFFF; }

    /** Returns the value for this token as a ASCII character */
    char asciiValue() const       { return static_cast<char>(m_rep); }

    /** Returns true iff this token represents a range of characters */
    bool isRange() const          { return m_rep!=0 && from()<=to(); }

    /** Returns true iff this token is a positive or negative character class */
    bool isCharClass() const      { return kind()==Kind::CharClass || kind()==Kind::NegCharClass; }

  private:
    uint32_t m_rep;
};

/** Private members of a regular expression */
class Ex::Private
{
  public:
    /** Creates the private part */
    Private(const std::string &pat) : pattern(pat)
    {
      data.reserve(100);
    }
    void compile();
#if ENABLE_DEBUG
    void dump();
#endif
    bool matchAt(size_t tokenPos,const std::string &str,Match &match,size_t pos,int level) const;

    /** Flag indicating the expression was succesfully compiled */
    bool error = false;

    /** The token stream representing the compiled regular expression. */
    std::vector<PToken> data; // compiled pattern

    /** The pattern string as passed by the user */
    std::string pattern;
};

/** Compiles a regular expression passed as a string into a stream of tokens that can be used for
 *  efficient searching.
 */
void Ex::Private::compile()
{
  error = false;
  data.clear();
  if (pattern.empty()) return;
  const char *start = pattern.c_str();
  const char *ps = start;
  char c;

  int prevTokenPos=-1;
  int tokenPos=0;

  auto addToken = [&](PToken tok)
  {
    tokenPos++;
    data.emplace_back(tok);
  };

  auto getNextCharacter = [&]() -> PToken
  {
    char cs=*ps;
    PToken result = PToken(cs);
    if (cs=='\\') // escaped character
    {
      ps++;
      cs=*ps;
      switch (cs)
      {
        case 'n': result = PToken('\n');                      break;
        case 'r': result = PToken('\r');                      break;
        case 't': result = PToken('\t');                      break;
        case 's': result = PToken(PToken::Kind::WhiteSpace);  break;
        case 'a': result = PToken(PToken::Kind::Alpha);       break;
        case 'w': result = PToken(PToken::Kind::AlphaNum);    break;
        case 'd': result = PToken(PToken::Kind::Digit);       break;
        case '<': result = PToken(PToken::Kind::BeginOfWord); break;
        case '>': result = PToken(PToken::Kind::EndOfWord);   break;
        case 'x':
        case 'X':
          {
            uint16_t v=0;
            for (int i=0;i<2 && (cs=(*(ps+1)));i++) // 2 hex digits
            {
              int d = (cs>='a' && cs<='f') ? cs-'a'+10 :
                      (cs>='A' && cs<='F') ? cs-'A'+10 :
                      (cs>='0' && cs<='9') ? cs-'0'    :
                      -1;
              if (d>=0) { v<<=4; v|=d; ps++; } else break;
            }
            result = PToken(v);
          }
          break;
        case '\0': ps--; break; // backslash at the end of the pattern
        default:
          result = PToken(cs);
          break;
      }
    }
    return result;
  };

  while ((c=*ps))
  {
    switch (c)
    {
      case '^': // beginning of line (if first character of the pattern)
        prevTokenPos = tokenPos;
        addToken(ps==start ? PToken(PToken::Kind::BeginOfLine) :
                            PToken(c));
        break;
      case '$': // end of the line (if last character of the pattern)
        prevTokenPos = tokenPos;
        addToken(*(ps+1)=='\0' ? PToken(PToken::Kind::EndOfLine) :
                                PToken(c));
        break;
      case '.': // any character
        prevTokenPos = tokenPos;
        addToken(PToken(PToken::Kind::Any));
        break;
      case '(': // begin of capture group
        prevTokenPos = tokenPos;
        addToken(PToken(PToken::Kind::BeginCapture));
        break;
      case ')': // end of capture group
        prevTokenPos = tokenPos;
        addToken(PToken(PToken::Kind::EndCapture));
        break;
      case '[': // character class
        {
          prevTokenPos = tokenPos;
          ps++;
          if (*ps==0) { error=true; return; }
          bool esc = *ps=='\\';
          PToken tok = getNextCharacter();
          ps++;
          if (!esc && tok.kind()==PToken::Kind::Character &&
                      tok.asciiValue()=='^') // negated character class
          {
            addToken(PToken(PToken::Kind::NegCharClass));
            if (*ps==0) { error=true; return; }
            tok = getNextCharacter();
            ps++;
          }
          else
          {
            addToken(PToken(PToken::Kind::CharClass));
          }
          uint16_t numTokens=0;
          while ((c=*ps))
          {
            if (c=='-' && *(ps+1)!=']' && *(ps+1)!=0) // range
            {
              getNextCharacter();
              ps++;
              PToken endTok = getNextCharacter();
              ps++;
              if (tok.value()>endTok.value())
              {
                addToken(PToken(endTok.value(),tok.value())); // swap start and end
              }
              else
              {
                addToken(PToken(tok.value(),endTok.value()));
              }
              numTokens++;
            }
            else // single char, from==to
            {
              if (tok.kind()==PToken::Kind::Character)
              {
                addToken(PToken(tok.value(),tok.value()));
              }
              else // special token, add as-is since from>to
              {
                addToken(tok);
              }
              numTokens++;
            }
            if (*ps==0) { error=true; return; } // expected at least a ]
            esc = *ps=='\\';
            tok = getNextCharacter();
            if (!esc && tok.kind()==PToken::Kind::Character &&
                        tok.value()==static_cast<uint16_t>(']'))
            {
              break; // end of character class
            }
            if (*ps==0) { error=true; return; } // no ] found
            ps++;
          }
          // set the value of either NegCharClass or CharClass
          data[prevTokenPos].setValue(numTokens);
        }
        break;
      case '*': // 0 or more
      case '+': // 1 or more
      case '?': // optional: 0 or 1
        {
          if (prevTokenPos==-1)
          {
            error=true;
            return;
          }
          switch (data[prevTokenPos].kind())
          {
            case PToken::Kind::BeginOfLine:  // $*  or  $+ or  $?
            case PToken::Kind::BeginOfWord:  // \<* or \<+ or \<?
            case PToken::Kind::EndOfWord:    // \>* or \>+ or \>?
            case PToken::Kind::Star:         // **  or  *+ or  *?
            case PToken::Kind::Optional:     // ?*  or  ?+ or  ??
              error=true;
              return;
            default: // ok
              break;
          }
          int ddiff = static_cast<int>(tokenPos-prevTokenPos);
          if (*ps=='+') // convert <pat>+ -> <pat><pat>*
          {
            // turn a sequence of token [T1...Tn] followed by '+' into [T1..Tn T1..Tn T*]
            //                          ddiff=n                                ^prevTokenPos
            data.resize(data.size()+ddiff);
            std::copy_n(data.begin()+prevTokenPos,ddiff,data.begin()+tokenPos);
            prevTokenPos+=ddiff;
            tokenPos+=ddiff;
          }
          data.insert(data.begin()+prevTokenPos,
                      c=='?' ? PToken(PToken::Kind::Optional) : PToken(PToken::Kind::Star));
          tokenPos++;
          addToken(PToken(PToken::Kind::End));
          // turn a sequence of tokens [T1 T2 T3] followed by 'T*' or into [T* T1 T2 T3 TEND]
          //                            ^prevTokenPos
          // same for 'T?'.
        }
        break;
      default:
        prevTokenPos = tokenPos;
        addToken(getNextCharacter());
        break;
    }
    ps++;
  }
  //addToken(PToken(PToken::Kind::End));
}

#if ENABLE_DEBUG
/** Dump the compiled token stream for this regular expression. For debugging purposes. */
void Ex::Private::dump()
{
  size_t l = data.size();
  size_t i =0;
  DBG("==== compiled token stream for pattern '%s' ===\n",pattern.c_str());
  while (i<l)
  {
    DBG("[%s:%04x]\n",data[i].kindStr(),data[i].value());
    if (data[i].kind()==PToken::Kind::CharClass || data[i].kind()==PToken::Kind::NegCharClass)
    {
      uint16_t num = data[i].value();
      while (num>0 && i<l)
      {
        i++;
        if (data[i].isRange()) // from-to range
        {
          DBG("[%04x(%c)-%04x(%c)]\n",data[i].from(),data[i].from(),data[i].to(),data[i].to());
        }
        else // special character like \n or \s
        {
          DBG("[%s:%04x]\n",data[i].kindStr(),data[i].value());
        }
        num--;
      }
    }
    i++;
  }
}
#endif

/** Internal matching routine.
 *  @param tokenPos Offset into the token stream.
 *  @param str      The input string to match against.
 *  @param match    The object used to store the matching results.
 *  @param pos      The position in the input string to start with matching
 *  @param level    Recursion level (used for debugging)
 */
bool Ex::Private::matchAt(size_t tokenPos,const std::string &str,Match &match,const size_t pos,int level) const
{
  DBG("%d:matchAt(tokenPos=%zu, str='%s', pos=%zu)\n",level,tokenPos,str.c_str(),pos);
  auto isStartIdChar = [](char c) { return std::isalpha(c) || c=='_' || c<0; };
  auto isIdChar      = [](char c) { return std::isalnum(c) || c=='_' || c<0; };
  auto matchCharClass = [this,isStartIdChar,isIdChar](size_t tp,char c) -> bool
  {
    PToken tok = data[tp];
    bool negate = tok.kind()==PToken::Kind::NegCharClass;
    uint16_t numFields = tok.value();
    bool found = false;
    for (uint16_t i=0;i<numFields;i++)
    {
      tok = data[++tp];
      // first check for built-in ranges
      if ((tok.kind()==PToken::Kind::Alpha      && isStartIdChar(c)) ||
          (tok.kind()==PToken::Kind::AlphaNum   && isIdChar(c))      ||
          (tok.kind()==PToken::Kind::WhiteSpace && std::isspace(c))  ||
          (tok.kind()==PToken::Kind::Digit      && std::isdigit(c))
         )
      {
        found=true;
        break;
      }
      else // user specified range
      {
        uint16_t v = static_cast<uint16_t>(c);
        if (tok.from()<=v && v<=tok.to())
        {
          found=true;
          break;
        }
      }
    }
    DBG("matchCharClass(tp=%zu,c=%c (x%02x))=%d\n",tp,c,c,negate?!found:found);
    return negate ? !found : found;
  };
  size_t index = pos;
  enum SequenceType { Star, Optional };
  auto processSequence = [this,&tokenPos,&index,&str,&matchCharClass,
                          &isStartIdChar,&isIdChar,&match,&level,&pos](SequenceType type) -> bool
  {
    size_t startIndex = index;
    PToken tok = data[++tokenPos];
    if (tok.kind()==PToken::Kind::Character) // 'x*' -> eat x's
    {
      char c_tok = tok.asciiValue();
      while (index<=str.length() && str[index]==c_tok) { index++; if (type==Optional) break; }
      tokenPos++;
    }
    else if (tok.isCharClass()) // '[a-f0-4]* -> eat matching characters
    {
      while (index<=str.length() && matchCharClass(tokenPos,str[index])) { index++; if (type==Optional) break; }
      tokenPos+=tok.value()+1; // skip over character ranges + end token
    }
    else if (tok.kind()==PToken::Kind::Alpha) // '\a*' -> eat start id characters
    {
      while (index<=str.length() && isStartIdChar(str[index])) { index++; if (type==Optional) break; }
      tokenPos++;
    }
    else if (tok.kind()==PToken::Kind::AlphaNum) // '\w*' -> eat id characters
    {
      while (index<=str.length() && isIdChar(str[index])) { index++; if (type==Optional) break; }
      tokenPos++;
    }
    else if (tok.kind()==PToken::Kind::WhiteSpace) // '\s*' -> eat spaces
    {
      while (index<=str.length() && std::isspace(str[index])) { index++; if (type==Optional) break; }
      tokenPos++;
    }
    else if (tok.kind()==PToken::Kind::Digit) // '\d*' -> eat digits
    {
      while (index<=str.length() && std::isdigit(str[index])) { index++; if (type==Optional) break; }
      tokenPos++;
    }
    else if (tok.kind()==PToken::Kind::Any) // '.*' -> eat all
    {
      if (type==Optional) index++; else index = str.length();
      tokenPos++;
    }
    tokenPos++; // skip over end marker
    while ((int)index>=(int)startIndex)
    {
      // pattern 'x*xy' should match 'xy' and 'xxxxy'
      bool found = matchAt(tokenPos,str,match,index,level+1);
      if (found)
      {
        match.setMatch(pos,index-pos+match.length());
        return true;
      }
      index--;
    }
    return false;
  };

  while (tokenPos<data.size())
  {
    PToken tok = data[tokenPos];
    //DBG("loop tokenPos=%zu token=%s\n",tokenPos,tok.kindStr());
    if (tok.kind()==PToken::Kind::Character) // match literal character
    {
      char c_tok = tok.asciiValue();
      if (index>=str.length() || str[index]!=c_tok) return false; // end of string, or non matching char
      index++,tokenPos++;
    }
    else if (tok.isCharClass())
    {
      if (index>=str.length() || !matchCharClass(tokenPos,str[index])) return false;
      index++,tokenPos+=tok.value()+1; // skip over character ranges + end token
    }
    else
    {
      switch (tok.kind())
      {
        case PToken::Kind::Alpha:
          if (index>=str.length() || !isStartIdChar(str[index])) return false;
          index++;
          break;
        case PToken::Kind::AlphaNum:
          if (index>=str.length() || !isIdChar(str[index])) return false;
          index++;
          break;
        case PToken::Kind::WhiteSpace:
          if (index>=str.length() || !std::isspace(str[index])) return false;
          index++;
          break;
        case PToken::Kind::Digit:
          if (index>=str.length() || !std::isdigit(str[index])) return false;
          index++;
          break;
        case PToken::Kind::BeginOfLine:
          if (index!=pos) return false;
          break;
        case PToken::Kind::EndOfLine:
          if (index<str.length()) return false;
          break;
        case PToken::Kind::BeginOfWord:
          DBG("BeginOfWord: index=%zu isIdChar(%c)=%d prev.isIdChar(%c)=%d\n",
              index,str[index],isIdChar(str[index]),
              index>0?str[index]-1:0,
              index>0?isIdChar(str[index-1]):-1);
          if (index>=str.length() ||
              !isIdChar(str[index]) ||
              (index>0 && isIdChar(str[index-1]))) return false;
          break;
        case PToken::Kind::EndOfWord:
          DBG("EndOfWord: index=%zu pos=%zu idIdChar(%c)=%d  prev.isIsChar(%c)=%d\n",
              index,pos,str[index],isIdChar(str[index]),
              index==0 ? 0 : str[index-1],
              index==0 ? -1 : isIdChar(str[index-1]));
          if (index<str.length() &&
              (isIdChar(str[index]) || index==0 || !isIdChar(str[index-1]))) return false;
          break;
        case PToken::Kind::BeginCapture:
          DBG("BeginCapture(%zu)\n",index);
          match.startCapture(index);
          break;
        case PToken::Kind::EndCapture:
          DBG("EndCapture(%zu)\n",index);
          match.endCapture(index);
          break;
        case PToken::Kind::Any:
          if (index>=str.length()) return false;
          index++;
          break;
        case PToken::Kind::Star:
          return processSequence(Star);
        case PToken::Kind::Optional:
          return processSequence(Optional);
        default:
          return false;
      }
      tokenPos++;
    }
  }
  match.setMatch(pos,index-pos);
  return true;
}

static std::string wildcard2regex(const std::string &pattern)
{
  std::string result="^"; // match start of input
  char c;
  const char *p = pattern.c_str();
  while ((c=*p++))
  {
    switch(c)
    {
      case '*':
        result+=".*";
        break; // '*' => '.*'
      case '?':
        result+='.';
        break; // '?' => '.'
      case '.':
      case '+':
      case '\\':
      case '$':
      case '^':
      case '(':
      case ')':
         result+='\\'; result+=c;  // escape
         break;
      case '[':
         if (*p=='^') // don't escape ^ after [
         {
           result+="[^";
           p++;
         }
         else
         {
           result+=c;
         }
        break;
      default:  // just copy
        result+=c;
        break;
    }
  }
  result+='$'; // match end of input
  return result;
}


Ex::Ex(const std::string &pattern, Mode mode)
  : p(std::make_unique<Private>(mode==Mode::RegEx ? pattern : wildcard2regex(pattern)))
{
  p->compile();
#if ENABLE_DEBUG
  p->dump();
  assert(!p->error);
#endif
}

Ex::~Ex()
{
}

bool Ex::match(const std::string &str,Match &match,size_t pos) const
{
  bool found=false;
  if (p->data.size()==0 || p->error) return found;
  match.init(&str);

  PToken tok = p->data[0];
  if (tok.kind()==PToken::Kind::BeginOfLine) // only test match at the given position
  {
    found = p->matchAt(0,str,match,pos,0);
  }
  else
  {
    if (tok.kind()==PToken::Kind::Character) // search for the start character
    {
      size_t index = str.find(tok.asciiValue(),pos);
      if (index==std::string::npos)
      {
        DBG("Ex::match(str='%s',pos=%zu)=false (no start char '%c')\n",str.c_str(),pos,tok.asciiValue());
        return false;
      }
      DBG("pos=%zu str='%s' char='%c' index=%zu\n",index,str.c_str(),tok.asciiValue(),index);
      pos=index;
    }
    while (pos<str.length()) // search for a match starting at pos
    {
      found = p->matchAt(0,str,match,pos,0);
      if (found) break;
      pos++;
    }
  }
  DBG("Ex::match(str='%s',pos=%zu)=%d\n",str.c_str(),pos,found);
  return found;
}

bool Ex::isValid() const
{
  return !p->pattern.empty() && !p->error;
}

//----------------------------------------------------------------------------------------

bool search(const std::string &str,Match &match,const Ex &re,size_t pos)
{
  return re.match(str,match,pos);
}

bool search(const std::string &str,const Ex &re,size_t pos)
{
  Match match;
  return re.match(str,match,pos);
}

bool match(const std::string &str,Match &match,const Ex &re)
{
  return re.match(str,match,0) && match.position()==0 && match.length()==str.length();
}

bool match(const std::string &str,const Ex &re)
{
  Match match;
  return re.match(str,match,0) && match.position()==0 && match.length()==str.length();
}

std::string replace(const std::string &str,const Ex &re,const std::string &replacement)
{
  std::string result;
  Match match;
  size_t p=0;
  while (re.match(str,match,p))
  {
    size_t i=match.position();
    size_t l=match.length();
    if (i>p) result+=str.substr(p,i-p);
    result+=replacement;
    p=i+l;
  }
  if (p<str.length()) result+=str.substr(p);
  return result;
}

}

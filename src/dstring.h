/****************************************************************************
**
** Copyright (C) 1997-2015 by Dimitri van Heesch.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation under the terms of the GNU General Public License is hereby
** granted. No representations are made about the suitability of this software
** for any purpose. It is provided "as is" without express or implied warranty.
** See the GNU General Public License for more details.
**
**********************************************************************/

#ifndef DSTRING_H
#define DSTRING_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>
#include <string_view>

#include "utf8.h"

#define ASSERT(x)  if ( !(x) )\
        fprintf(stderr,"ASSERT: \"%s\" in %s (%d)\n",#x,__FILE__,__LINE__)


/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

//! Returns a copy of a string \a s.
//! Note that memory is passed to the caller, use dstrfree() to release.
char *dstrdup( const char *s );
//! Frees the memory allocated using dstrdup().
void dstrfree( const char *s );

//! Returns the length of string \a str, or 0 if a null pointer is passed.
inline uint32_t dstrlen( const char *str )
{ return str ? static_cast<uint32_t>(strlen(str)) : 0; }

inline char *dstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : nullptr; }

char *dstrncpy(char *dst,const char *src, size_t len);

inline bool disempty( const char *s)
{ return s==nullptr || *s=='\0'; }

inline int dstrcmp( const char *str1, const char *str2 )
{ return (str1 && str2) ? strcmp(str1,str2) :     // both non-empty
         (disempty(str1) && disempty(str2)) ? 0 : // both empty
         disempty(str1) ? -1 : 1;                 // one empty, other non-empty
}

inline int dstrncmp( const char *str1, const char *str2, size_t len )
{ return (str1 && str2) ? strncmp(str1,str2,len) :  // both non-empty
         (disempty(str1) && disempty(str2)) ? 0 :   // both empty
         disempty(str1) ? -1 : 1;                   // one empty other non-empty
}

inline bool disspace(char c)
{ return c==' ' || c=='\t' || c=='\n' || c=='\r'; }

int dstricmp( const char *str1, const char *str2 );

inline int dstricmp_sort( const char *str1, const char *str2 )
{
  int result = dstricmp(str1,str2);
  return result==0 ? dstrcmp(str1,str2) : result;
}

int dstrnicmp( const char *str1, const char *str2, size_t len );

#ifndef DISABLE_JAVACC
using JavaCCString = std::basic_string<JAVACC_CHAR_TYPE>;
#endif

/** A String class for use with Doxygen wrapping std::string and adding some additional
 *  functionality offered by QCString.
 *  QCString is part of qtools which was used as a portability layer in the past.
 */
class DString
{
  public:
    DString() = default;
    DString(const DString &) = default;
    DString &operator=(const DString &) = default;
    DString(DString &&) = default;
    DString &operator=(DString &&) = default;
   ~DString() = default;

    DString( const std::string &s ) : m_rep(s) {}

    DString( std::string &&s) : m_rep(std::move(s)) {}

    DString &operator=( std::string &&s)
    {
      m_rep=std::move(s);
      return *this;
    }

    DString( std::string_view sv) : m_rep(sv) {}

    DString &operator=(std::string_view sv)
    {
      m_rep=sv;
      return *this;
    }

    DString( int ) = delete;

    /** For converting a JavaCC string */
#ifndef DISABLE_JAVACC
    DString( const JavaCCString &s)
    {
      m_rep.resize(s.size());
      std::memcpy(m_rep.data(),s.data(),s.size());
    }
    DString &operator=( const JavaCCString &s)
    {
      m_rep.resize(s.size());
      std::memcpy(m_rep.data(),s.data(),s.size());
      return *this;
    }
#endif

    /** creates a string with room for size characters
     *  @param[in] size the number of character to allocate (also counting the 0-terminator!)
     */
    enum SizeTag { ExplicitSize };
    explicit DString( size_t size, SizeTag t) { m_rep.resize(size); }

    /** creates a string from a plain C string.
     *  @param[in] str A zero terminated C string. When 0 an empty string is created.
     */
    DString( const char *str ) : m_rep(str?str:"") {}

    /** creates a string from \a str and copies over the first \a maxlen characters. */
    DString( const char *str, size_t maxlen ) : m_rep(str?str:"") { m_rep.resize(maxlen); }

    /** replaces the contents by that of C string \a str. */
    DString &operator=( const char *str) { m_rep = str?str:""; return *this; }

    DString &operator=( const std::string &s) { m_rep = s; return *this; }

    /** Returns true iff the string is empty (std::string compatible alias for isEmpty()) */
    bool empty() const { return m_rep.empty(); }

    /** Returns the length of the string, not counting the 0-terminator. Equivalent to size(). */
    size_t length() const { return m_rep.size(); }

    /** Returns the length of the string, not counting the 0-terminator. */
    size_t size() const { return m_rep.size(); }

    /** Returns a pointer to the contents of the string in the form of a 0-terminated C string */
    const char *data() const { return m_rep.c_str(); }

    /** Returns a pointer to the contents of the string in the form of a 0-terminated C string */
    const char *c_str() const { return m_rep.c_str(); }

    std::string_view view() const { return m_rep; }

    /** Returns a writable pointer to the data.
     */
    char *rawData() { return &m_rep[0]; }

    using value_type             = std::string::value_type;
    using size_type               = std::string::size_type;
    using reference               = std::string::reference;
    using const_reference         = std::string::const_reference;
    using iterator                = std::string::iterator;
    using const_iterator          = std::string::const_iterator;
    using reverse_iterator        = std::string::reverse_iterator;
    using const_reverse_iterator  = std::string::const_reverse_iterator;

    /** value used to indicate 'not found' or 'to the end of the string', matching std::string::npos */
    static constexpr size_t npos = std::string::npos;

    iterator begin() { return m_rep.begin(); }
    const_iterator begin() const { return m_rep.begin(); }
    const_iterator cbegin() const { return m_rep.cbegin(); }
    iterator end() { return m_rep.end(); }
    const_iterator end() const { return m_rep.end(); }
    const_iterator cend() const { return m_rep.cend(); }

    reverse_iterator rbegin() { return m_rep.rbegin(); }
    const_reverse_iterator rbegin() const { return m_rep.rbegin(); }
    const_reverse_iterator crbegin() const { return m_rep.crbegin(); }
    reverse_iterator rend() { return m_rep.rend(); }
    const_reverse_iterator rend() const { return m_rep.rend(); }
    const_reverse_iterator crend() const { return m_rep.crend(); }

    /** Returns a reference to the first character. Undefined behavior if the string is empty. */
    char &front() { return m_rep.front(); }
    const char &front() const { return m_rep.front(); }

    /** Returns a reference to the last character. Undefined behavior if the string is empty. */
    char &back() { return m_rep.back(); }
    const char &back() const { return m_rep.back(); }

    void push_back( char c ) { m_rep.push_back(c); }
    void pop_back() { m_rep.pop_back(); }

    size_t capacity() const { return m_rep.capacity(); }
    size_t max_size() const { return m_rep.max_size(); }
    void shrink_to_fit() { m_rep.shrink_to_fit(); }

    void resize( size_t newlen) { m_rep.resize(newlen); }

    /** Resizes the string to \a newlen characters, filling any new characters with \a c */
    void resize( size_t newlen, char c ) { m_rep.resize(newlen,c); }

    void clear() { m_rep.clear(); }

    /** Reserve space for \a size bytes without changing the string contents */
    void reserve( size_t size ) { m_rep.reserve(size); }

    /** Swaps the contents of this string with \a other */
    void swap( DString &other ) { m_rep.swap(other.m_rep); }

    /** Returns a substring of length \a count starting at \a pos */
    DString substr( size_t pos=0, size_t count=npos ) const { return DString(m_rep.substr(pos,count)); }

    /** Copies (up to) \a count characters into \a dest, starting at \a pos. Returns the number of characters copied. */
    size_t copy( char *dest, size_t count, size_t pos=0 ) const { return m_rep.copy(dest,count,pos); }

    int compare( const DString &s ) const { return m_rep.compare(s.str()); }
    int compare( const char *s ) const { return m_rep.compare(s?s:""); }
    int compare( const std::string &s ) const { return m_rep.compare(s); }

    DString &erase( size_t index=0, size_t count=npos ) { m_rep.erase(index,count); return *this; }

    DString &assign( const char *s ) { return operator=(s); }
    DString &assign( const DString &s ) { m_rep = s.str(); return *this; }
    DString &assign( const std::string &s ) { return operator=(s); }
    DString &assign( std::string_view s ) { return operator=(s); }

    size_t find( char c, size_t pos=0 ) const { return m_rep.find(c,pos); }
    size_t find( const char *s, size_t pos=0 ) const { return s?m_rep.find(s,pos):npos; }
    size_t find( const DString &s, size_t pos=0 ) const { return m_rep.find(s.str(),pos); }
    size_t find( const std::string &s, size_t pos=0 ) const { return m_rep.find(s,pos); }

    size_t rfind( char c, size_t pos=npos ) const { return m_rep.rfind(c,pos); }
    size_t rfind( const char *s, size_t pos=npos ) const { return s?m_rep.rfind(s,pos):npos; }
    size_t rfind( const DString &s, size_t pos=npos ) const { return m_rep.rfind(s.str(),pos); }
    size_t rfind( const std::string &s, size_t pos=npos ) const { return m_rep.rfind(s,pos); }

    size_t rfind_insensitive( char c, size_t pos=npos) const;
    size_t rfind_insensitive( const char *str, size_t pos=npos) const;

    size_t find_first_of( char c, size_t pos=0 ) const { return m_rep.find_first_of(c,pos); }
    size_t find_first_of( const char *s, size_t pos=0 ) const { return s?m_rep.find_first_of(s,pos):npos; }
    size_t find_first_of( const DString &s, size_t pos=0 ) const { return m_rep.find_first_of(s.str(),pos); }
    size_t find_first_of( const std::string &s, size_t pos=0 ) const { return m_rep.find_first_of(s,pos); }

    size_t find_last_of( char c, size_t pos=npos ) const { return m_rep.find_last_of(c,pos); }
    size_t find_last_of( const char *s, size_t pos=npos ) const { return s?m_rep.find_last_of(s,pos):npos; }
    size_t find_last_of( const DString &s, size_t pos=npos ) const { return m_rep.find_last_of(s.str(),pos); }
    size_t find_last_of( const std::string &s, size_t pos=npos ) const { return m_rep.find_last_of(s,pos); }

    size_t find_first_not_of( char c, size_t pos=0 ) const { return m_rep.find_first_not_of(c,pos); }
    size_t find_first_not_of( const char *s, size_t pos=0 ) const { return s?m_rep.find_first_not_of(s,pos):npos; }
    size_t find_first_not_of( const DString &s, size_t pos=0 ) const { return m_rep.find_first_not_of(s.str(),pos); }
    size_t find_first_not_of( const std::string &s, size_t pos=0 ) const { return m_rep.find_first_not_of(s,pos); }

    size_t find_last_not_of( char c, size_t pos=npos ) const { return m_rep.find_last_not_of(c,pos); }
    size_t find_last_not_of( const char *s, size_t pos=npos ) const { return s?m_rep.find_last_not_of(s,pos):npos; }
    size_t find_last_not_of( const DString &s, size_t pos=npos ) const { return m_rep.find_last_not_of(s.str(),pos); }
    size_t find_last_not_of( const std::string &s, size_t pos=npos ) const { return m_rep.find_last_not_of(s,pos); }

    /** Fills a string with a predefined character
     *  @param[in] c the character used to fill the string with.
     *  @param[in] len the number of character to fill. Use -1 to fill the whole string.
     *  @note the string will be resized to contain \a len characters. The contents of the
     *  string will be lost.
     */
    DString fill( char c, size_t len)
    {
      //int l = len==-1 ? static_cast<int>(m_rep.size()) : len;
      m_rep = std::string(len,c);
      return *this;
    }

    DString &sprintf( const char *format, ... );

    int contains( char c, bool cs=true ) const;
    int contains( const char *str, bool cs=true ) const;

    bool stripPrefix(const DString &prefix)
    {
      if (prefix.empty() || m_rep.empty()) return false;
      if (m_rep.rfind(prefix.data(),0)==0) // string starts with prefix
      {
        m_rep.erase(0,prefix.length());
        return true;
      }
      return false;
    }

    bool stripPrefix(const char *prefix)
    {
      return stripPrefix(DString(prefix));
    }

    DString left( size_t len ) const
    {
      return m_rep.empty() ? DString() : DString(m_rep.substr(0,len));
    }

    DString right( size_t len ) const
    {
      return m_rep.empty()    ? DString() :
             len<m_rep.size() ? DString(m_rep.substr(m_rep.size()-len,len)) :
             *this;
    }

    DString mid( size_t index, size_t len=npos ) const
    {
      size_t slen = m_rep.size();
      if (len==npos) len = slen-index;
      return m_rep.empty() || index>slen || len==0 ? DString() :
             DString(m_rep.substr(index,len));
    }

    DString lower() const
    {
      return DString(convertUTF8ToLower(m_rep));
    }

    DString upper() const
    {
      return DString(convertUTF8ToUpper(m_rep));
    }

    /// returns a copy of this string with leading and trailing whitespace removed
    DString stripWhiteSpace() const
    {
      size_t sl = m_rep.size();
      if (sl==0 || (!disspace(m_rep[0]) && !disspace(m_rep[sl-1]))) return *this;
      size_t start=0,end=sl-1;
      while (start<sl && disspace(m_rep[start])) start++;
      if (start==sl) return DString(); // only whitespace
      while (end>start && disspace(m_rep[end])) end--;
      return DString(m_rep.substr(start,1+end-start));
    }

    DString stripLeadingAndTrailingEmptyLines() const;

    // Returns a quoted copy of this string, unless it is already quoted.
    // Note that trailing and leading whitespace is removed.
    DString quoted() const
    {
      size_t start=0, sl=m_rep.size(), end=sl-1;
      while (start<sl  && disspace(m_rep[start])) start++; // skip over leading whitespace
      if (start==sl) return DString(); // only whitespace
      while (end>start && disspace(m_rep[end]))   end--;   // skip over trailing whitespace
      bool needsQuotes=false;
      size_t i=start;
      if (i<end && m_rep[i]!='"') // stripped string has at least non-whitespace unquoted character
      {
        while (i<end && !needsQuotes) // check if the to be quoted part has at least one whitespace character
        {
          needsQuotes = m_rep[i] =='-';
          needsQuotes |= disspace(m_rep[i++]);
        }
      }
      DString result(m_rep.substr(start,1+end-start));
      if (needsQuotes)
      {
        result.prepend("\"");
        result.append("\"");
      }
      return result;
    }

    /// returns a copy of this string with all whitespace removed
    DString removeWhiteSpace() const
    {
      size_t sl = m_rep.size();
      if (sl==0) return *this;
      std::string result = m_rep;
      size_t src=0,dst=0;
      while (src<sl)
      {
        if (!disspace(m_rep[src])) result[dst++]=m_rep[src];
        src++;
      }
      if (dst<m_rep.size()) result.resize(dst);
      return DString(result);
    }

    /** return a copy of this string with leading and trailing whitespace removed and multiple internal
     *  whitespace characters replaced by a single space
     */
    DString simplifyWhiteSpace() const;

    /** removes occurrences of whole \a word from this string,
     *  while keeping internal spaces and reducing multiple sequences of spaces.
     *  Example: ` cat+ catfish cat cat concat cat`, word=`cat` returns: `+ catfish concat`
     *
     *  @param word The word to search for and remove from this string.
     *  @returns true if at least one occurrence of \a word was found and removed, false otherwise
     */
    bool findAndRemoveWord(const char *word);

    /** returns true if this string contains \a word as a whole word, false otherwise.
     *  A whole word is defined as a sequence of characters that is not preceded or followed by
     *  an identifier character (see isId()).
     */
    bool containsWord(const char *word) const;

    // Returns a copy of this string repeated n times
    DString repeat(unsigned int n) const
    {
      DString result(n * size(), ExplicitSize);
      size_t offset = 0;
      for (offset = 0; offset < n * size(); offset += size())
      {
        memcpy(result.rawData() + offset, data(), size());
      }
      return result;
    }

    DString &insert( size_t index, const DString &s )
    {
      if (s.length()>0)
      {
        size_t ol = m_rep.size();
        if (index>ol) // insert beyond end of string and fill gap with spaces
        {
          m_rep.resize(index+s.length());
          std::memset(&m_rep[ol],' ',index-ol);
          std::memcpy(&m_rep[index],s.data(),s.length()+1);
        }
        else // insert inside the string
        {
          m_rep.insert(index,s.str());
        }
      }
      return *this;
    }

    DString &insert( size_t index, std::string_view s)
    {
      if (s.length()>0)
      {
        size_t ol = m_rep.size();
        if (index>ol) // insert beyond end of string and fill gap with spaces
        {
          m_rep.resize(index+s.length());
          std::memset(&m_rep[ol],' ',index-ol);
          std::memcpy(&m_rep[index],s.data(),s.length()+1);
        }
        else // insert inside the string
        {
          m_rep.insert(index,s);
        }
      }
      return *this;
    }

    DString &insert( size_t index, const char *s )
    {
      size_t len = s ? dstrlen(s) : 0;
      if (len>0)
      {
        size_t ol = m_rep.size();
        if (index>ol) // insert beyond end of string and fill gap with spaces
        {
          m_rep.resize(index+len);
          std::memset(&m_rep[ol],' ',index-ol);
          std::memcpy(&m_rep[index],s,len+1);
        }
        else // insert inside the string
        {
          m_rep.insert(index,s);
        }
      }
      return *this;
    }

    DString &insert( size_t index, char c)
    {
      char s[2] = { c, '\0' };
      return insert(index,s);
    }

    DString &append( char c)
    {
      m_rep+=c;
      return *this;
    }

    DString &append( const char *s )
    {
      return operator+=(s);
    }

    DString &append( const DString &s )
    {
      return operator+=(s);
    }

    DString &append( const std::string &s )
    {
      return operator+=(s);
    }

    DString &append( std::string_view s)
    {
      return operator+=(s);
    }

    DString &prepend( const char *s )
    {
      return insert(0,s);
    }

    DString &prepend( const DString &s )
    {
      return insert(0,s.data());
    }

    DString &prepend( const std::string &s )
    {
      return insert(0,s.c_str());
    }

    DString &prepend( std::string_view s)
    {
      return insert(0,s);
    }

    DString &remove( size_t index, size_t len )
    {
      size_t ol = m_rep.size();
      if (index<ol && len>0) m_rep.erase(index,index+len>=ol ? std::string::npos : len);
      return *this;
    }

    DString &replace( size_t index, size_t len, const char *s);

    short         toShort(  bool *ok=nullptr, int base=10 ) const;
    uint16_t      toUShort( bool *ok=nullptr, int base=10 ) const;
    int           toInt(    bool *ok=nullptr, int base=10 ) const;
    uint32_t      toUInt(   bool *ok=nullptr, int base=10 ) const;
    long          toLong(   bool *ok=nullptr, int base=10 ) const;
    unsigned long toULong(  bool *ok=nullptr, int base=10 ) const;
    uint64_t      toUInt64( bool *ok=nullptr, int base=10 ) const;

    DString &setNum(short n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(uint16_t n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(int n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(uint32_t n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(long long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(unsigned long long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    DString &setNum(unsigned long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    bool startsWith( const char *s ) const
    {
      if (m_rep.empty() || s==nullptr) return s==nullptr;
      return m_rep.rfind(s,0)==0; // looking "backward" starting and ending at index 0
    }

    bool startsWith( const std::string &s) const
    {
      return m_rep.rfind(s,0)==0; // looking "backward" starting and ending at index 0
    }

    bool startsWith( const DString &s ) const
    {
      if (m_rep.empty() || s.empty()) return s.empty();
      return m_rep.rfind(s.str(),0)==0; // looking "backward" starting and ending at index 0
    }

    bool endsWith(const char *s) const
    {
      if (m_rep.empty() || s==nullptr) return s==nullptr;
      size_t l = strlen(s);
      return m_rep.length()>=l && m_rep.compare(m_rep.length()-l, l, s, l)==0;
    }

    bool endsWith(const std::string &s) const
    {
      size_t l = s.length();
      return m_rep.length()>=l && m_rep.compare(m_rep.length()-l, l, s)==0;
    }

    bool endsWith(const DString &s) const
    {
      size_t l = s.length();
      return m_rep.length()>=l && m_rep.compare(m_rep.length()-l, l, s.str())==0;
    }

#define HAS_IMPLICIT_CAST_TO_PLAIN_C_STRING 0
#if HAS_IMPLICIT_CAST_TO_PLAIN_C_STRING
    /** Converts the string to a plain C string */
    operator const char *() const
    {
      return data();
    }
#endif

    const std::string &str() const
    {
      return m_rep;
    }

    DString &operator+=( const DString &s)
    {
      m_rep+=s.str();
      return *this;
    }

    DString &operator+=( const std::string &s)
    {
      m_rep+=s;
      return *this;
    }

    DString &operator+=(std::string_view s)
    {
      m_rep+=s;
      return *this;
    }

    /** Appends string \a str to this string and returns a reference to the result. */
    DString &operator+=( const char *s )
    {
      if (s) m_rep+=s;
      return *this;
    }

#define HAS_CHARACTER_APPEND_OPERATOR 1
#if HAS_CHARACTER_APPEND_OPERATOR
    /** Appends character \a c to this string and returns a reference to the result. */
    DString &operator+=( char c )
    {
      m_rep+=c;
      return *this;
    }
#endif

    /** Returns a reference to the character at index \a i. */
    char &at( size_t i)
    {
      return m_rep[i];
    }

    const char &at( size_t i) const
    {
      return m_rep[i];
    }

    /** Indexing operator. Equivalent to at(). */
    char &operator[]( size_t i )
    {
      return m_rep[i];
    }

    const char &operator[]( size_t i ) const
    {
      return m_rep[i];
    }

    static DString integerToAlpha(int n, bool upper=true);
    static DString integerToRoman(int n, bool upper=true);

  private:
    std::string m_rep;
};

/*****************************************************************************
  DString non-member operators
 *****************************************************************************/

inline bool operator==( const DString &s1, const DString &s2 )
{ return s1.str() == s2.str(); }

inline bool operator==( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) == 0; }

inline bool operator==( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) == 0; }

inline bool operator!=( const DString &s1, const DString &s2 )
{ return s1.str() != s2.str(); }

inline bool operator!=( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) != 0; }

inline bool operator!=( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) != 0; }

inline bool operator<( const DString &s1, const DString& s2 )
{ return dstrcmp(s1.data(),s2.data()) < 0; }

inline bool operator<( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) < 0; }

inline bool operator<( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) < 0; }

inline bool operator<=( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) <= 0; }

inline bool operator<=( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) <= 0; }

inline bool operator>( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) > 0; }

inline bool operator>( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) > 0; }

inline bool operator>=( const DString &s1, const char *s2 )
{ return dstrcmp(s1.data(),s2) >= 0; }

inline bool operator>=( const char *s1, const DString &s2 )
{ return dstrcmp(s1,s2.data()) >= 0; }

inline DString operator+( const DString &s1, const DString &s2 )
{
  return DString(s1.str()+s2.str());
}


inline DString operator+( const DString &s1, const char *s2 )
{
    DString tmp(s1);
    tmp.append(s2);
    return tmp;
}

inline DString operator+( const char *s1, const DString &s2 )
{
    DString tmp(s1);
    tmp.append(s2);
    return tmp;
}

inline const char *qPrint(const char *s)
{
  if (s) return s; else return "";
}

inline const char *qPrint(const DString &s)
{
  if (!s.empty()) return s.data(); else return "";
}

inline const char *qPrint(const std::string &s)
{
  return s.c_str();
}

inline std::string toStdString(const DString &s)
{
  return s.str();
}

//---- overloads

inline int dstrcmp( const DString &str1, const char *str2 )
{
  return dstrcmp(str1.data(),str2);
}

inline int dstrcmp( const char *str1, const DString &str2 )
{
  return dstrcmp(str1,str2.data());
}

inline int dstrcmp( const DString &str1, const DString &str2 )
{
  return dstrcmp(str1.data(),str2.data());
}

inline int dstricmp( const DString &str1, const char *str2 )
{
  return dstricmp(str1.data(),str2);
}

inline int dstricmp( const char *str1, const DString &str2 )
{
  return dstricmp(str1,str2.data());
}

inline int dstricmp( const DString &str1, const DString &str2 )
{
  return dstricmp(str1.data(),str2.data());
}

inline int dstricmp_sort( const DString &str1, const char *str2 )
{
  return dstricmp_sort(str1.data(),str2);
}

inline int dstricmp_sort( const char *str1, const DString &str2 )
{
  return dstricmp_sort(str1,str2.data());
}

inline int dstricmp_sort( const DString &str1, const DString &str2 )
{
  return dstricmp_sort(str1.data(),str2.data());
}


inline int dstrnicmp( const DString &str1, const char *str2, size_t len )
{
  return dstrnicmp(str1.data(),str2,len);
}

inline int dstrnicmp( const char *str1, const DString &str2, size_t len )
{
  return dstrnicmp(str1,str2.data(),len);
}

inline int dstrnicmp( const DString &str1, const DString &str2, size_t len )
{
  return dstrnicmp(str1.data(),str2.data(),len);
}

// helper functions
DString substitute(const DString &str,const DString &find,const DString &replace);
inline DString substitute(const DString &str,const char *find,const char *replace)
{
  return substitute(str,DString(find),DString(replace));
}
DString substitute(const DString &s,const DString &src,const DString &dst,int skip_seq);

inline DString substitute(const DString &s,char srcChar,char dstChar)
{
  std::string ss = s.str();
  std::replace(ss.begin(),ss.end(),srcChar,dstChar);
  return DString(ss);
}

inline std::ostream& operator<<(std::ostream& os, const DString& s)
{
    os << s.str();
    return os;
}

inline void swap(DString &s1, DString &s2)
{
  s1.swap(s2);
}

/** Returns true if \a c is a valid character for an identifier.
 *  Valid characters are: letters, digits, underscore, dollar sign and any character with a value >= 128.
 */
inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c) || c=='$';
}

/*! Returns a place holder for a position in a list. Used for
 *  translators to be able to specify different elements orders
 *  depending on whether text flows from left to right or visa versa.
 */
inline DString generateMarker(int id)
{
  const int maxMarkerStrLen = 20;
  char result[maxMarkerStrLen];
  snprintf(result,maxMarkerStrLen,"@%d",id);
  return result;
}

namespace std
{
  template<> struct hash<DString>
  {
    size_t operator()(const DString &s) const
    {
      return hash<std::string>{}(s.str());
    }
  };
}

#endif // DSTRING_H

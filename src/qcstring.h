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
** Note: this is a reimplementation of the qcstring.h that came with
** an Qt version 2.2.3. For short strings it stores the string data inside
** the object. For long strings it uses a separate array with reference counting.
**
**********************************************************************/

#ifndef QCSTRING_H
#define QCSTRING_H

#include <string>
#include <string_view>
#include <algorithm>

#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ostream>

#include "utf8.h"

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif
#define ASSERT(x)  if ( !(x) )\
	fprintf(stderr,"ASSERT: \"%s\" in %s (%d)\n",#x,__FILE__,__LINE__)


/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

void *qmemmove( void *dst, const void *src, size_t len );

#define qsnprintf snprintf

//! Returns a copy of a string \a s.
//! Note that memory is passed to the caller, use qstrfree() to release.
char *qstrdup( const char *s );
//! Frees the memory allocated using qstrdup().
void qstrfree( const char *s );

//! Returns the length of string \a str, or 0 if a null pointer is passed.
inline uint32_t qstrlen( const char *str )
{ return str ? static_cast<uint32_t>(strlen(str)) : 0; }

inline char *qstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : nullptr; }

char *qstrncpy(char *dst,const char *src, size_t len);

inline bool qisempty( const char *s)
{ return s==nullptr || *s=='\0'; }

inline int qstrcmp( const char *str1, const char *str2 )
{ return (str1 && str2) ? strcmp(str1,str2) :     // both non-empty
         (qisempty(str1) && qisempty(str2)) ? 0 : // both empty
         qisempty(str1) ? -1 : 1;                 // one empty, other non-empty
}

inline int qstrncmp( const char *str1, const char *str2, size_t len )
{ return (str1 && str2) ? strncmp(str1,str2,len) :  // both non-empty
         (qisempty(str1) && qisempty(str2)) ? 0 :   // both empty
         qisempty(str1) ? -1 : 1;                   // one empty other non-empty
}

inline bool qisspace(char c)
{ return c==' ' || c=='\t' || c=='\n' || c=='\r'; }

int qstricmp( const char *str1, const char *str2 );

inline int qstricmp_sort( const char *str1, const char *str2 )
{
  int result = qstricmp(str1,str2);
  return result==0 ? qstrcmp(str1,str2) : result;
}


int qstrnicmp( const char *str1, const char *str2, size_t len );

using JavaCCString = std::basic_string<JAVACC_CHAR_TYPE>;

/** This is an alternative implementation of QCString. It provides basically
 *  the same functions but uses std::string as the underlying string type
 */
class QCString
{
  public:
    QCString() = default;
    explicit QCString( const std::string &s ) : m_rep(s) {}

    QCString( std::string &&s) : m_rep(std::move(s)) {}

    QCString( std::string_view sv) : m_rep(sv) {}

    QCString &operator=(std::string_view sv) {
      m_rep=sv;
      return *this;
    }

    QCString( int ) = delete;

    /** For converting a JavaCC string */
    QCString( const JavaCCString &s)
    {
      m_rep.resize(s.size());
      memcpy(m_rep.data(),s.data(),s.size());
    }
    QCString &operator=( const JavaCCString &s)
    {
      m_rep.resize(s.size());
      memcpy(m_rep.data(),s.data(),s.size());
      return *this;
    }

    /** creates a string with room for size characters
     *  @param[in] size the number of character to allocate (also counting the 0-terminator!)
     */
    enum SizeTag { ExplicitSize };
    explicit QCString( size_t size, SizeTag t) { m_rep.resize(size); }

    /** creates a string from a plain C string.
     *  @param[in] str A zero terminated C string. When 0 an empty string is created.
     */
    QCString( const char *str ) : m_rep(str?str:"") {}

    /** creates a string from \a str and copies over the first \a maxlen characters. */
    QCString( const char *str, size_t maxlen ) : m_rep(str?str:"") { m_rep.resize(maxlen); }

    /** replaces the contents by that of C string \a str. */
    QCString &operator=( const char *str) { m_rep = str?str:""; return *this; }

    QCString &operator=( const std::string &s) { m_rep = s; return *this; }

    /** Returns TRUE iff the string is empty */
    bool isEmpty() const { return m_rep.empty(); }

    /** Returns the length of the string, not counting the 0-terminator. Equivalent to size(). */
    size_t length() const { return m_rep.size(); }

    /** Returns the length of the string, not counting the 0-terminator. */
    size_t size() const { return m_rep.size(); }

    /** Returns a pointer to the contents of the string in the form of a 0-terminated C string */
    const char *data() const { return m_rep.c_str(); }

    std::string_view view() const { return m_rep; }

    /** Returns a writable pointer to the data.
     */
    char *rawData() { return &m_rep[0]; }

    void resize( size_t newlen) { m_rep.resize(newlen); }

    void clear() { m_rep.clear(); }

    /** Reserve space for \a size bytes without changing the string contents */
    void reserve( size_t size ) { m_rep.reserve(size); }

    /** Fills a string with a predefined character
     *  @param[in] c the character used to fill the string with.
     *  @param[in] len the number of character to fill. Use -1 to fill the whole string.
     *  @note the string will be resized to contain \a len characters. The contents of the
     *  string will be lost.
     */
    void fill( char c, int len = -1 )
    {
      int l = len==-1 ? static_cast<int>(m_rep.size()) : len;
      m_rep = std::string(l,c);
    }

    QCString &sprintf( const char *format, ... );

    int	find( char c, int index=0, bool cs=TRUE ) const;
    int	find( const char *str, int index=0, bool cs=TRUE ) const;
    int find( const QCString &str, int index=0, bool cs=TRUE ) const;

    int	findRev( char c, int index=-1, bool cs=TRUE) const;
    int	findRev( const char *str, int index=-1, bool cs=TRUE) const;

    int	contains( char c, bool cs=TRUE ) const;
    int	contains( const char *str, bool cs=TRUE ) const;

    bool stripPrefix(const QCString &prefix)
    {
      if (prefix.isEmpty() || m_rep.empty()) return FALSE;
      if (m_rep.rfind(prefix.data(),0)==0) // string starts with prefix
      {
        m_rep.erase(0,prefix.length());
        return TRUE;
      }
      return FALSE;
    }

    bool stripPrefix(const char *prefix)
    {
      return stripPrefix(QCString(prefix));
    }

    QCString left( size_t len ) const
    {
      return m_rep.empty() ? QCString() : QCString(m_rep.substr(0,len));
    }

    QCString right( size_t len ) const
    {
      return m_rep.empty()    ? QCString() :
             len<m_rep.size() ? QCString(m_rep.substr(m_rep.size()-len,len)) :
             *this;
    }

    QCString mid( size_t index, size_t len=static_cast<size_t>(-1) ) const
    {
      size_t slen = m_rep.size();
      if (len==static_cast<uint32_t>(-1)) len = slen-index;
      return m_rep.empty() || index>slen || len==0 ? QCString() :
             QCString(m_rep.substr(index,len));
    }

    QCString lower() const
    {
      return QCString(convertUTF8ToLower(m_rep));
    }

    QCString upper() const
    {
      return QCString(convertUTF8ToUpper(m_rep));
    }

    /// returns a copy of this string with leading and trailing whitespace removed
    QCString stripWhiteSpace() const
    {
      size_t sl = m_rep.size();
      if (sl==0 || (!qisspace(m_rep[0]) && !qisspace(m_rep[sl-1]))) return *this;
      size_t start=0,end=sl-1;
      while (start<sl && qisspace(m_rep[start])) start++;
      if (start==sl) return QCString(); // only whitespace
      while (end>start && qisspace(m_rep[end])) end--;
      return QCString(m_rep.substr(start,1+end-start));
    }

    QCString stripLeadingAndTrailingEmptyLines() const;

    // Returns a quoted copy of this string, unless it is already quoted.
    // Note that trailing and leading whitespace is removed.
    QCString quoted() const
    {
      size_t start=0, sl=m_rep.size(), end=sl-1;
      while (start<sl  && qisspace(m_rep[start])) start++; // skip over leading whitespace
      if (start==sl) return QCString(); // only whitespace
      while (end>start && qisspace(m_rep[end]))   end--;   // skip over trailing whitespace
      bool needsQuotes=false;
      size_t i=start;
      if (i<end && m_rep[i]!='"') // stripped string has at least non-whitespace unquoted character
      {
        while (i<end && !needsQuotes) // check if the to be quoted part has at least one whitespace character
        {
          needsQuotes = m_rep[i] =='-';
          needsQuotes |= qisspace(m_rep[i++]);
        }
      }
      QCString result(m_rep.substr(start,1+end-start));
      if (needsQuotes)
      {
        result.prepend("\"");
        result.append("\"");
      }
      return result;
    }

    /// returns a copy of this string with all whitespace removed
    QCString removeWhiteSpace() const
    {
      size_t sl = m_rep.size();
      if (sl==0) return *this;
      std::string result = m_rep;
      size_t src=0,dst=0;
      while (src<sl)
      {
        if (!qisspace(m_rep[src])) result[dst++]=m_rep[src];
        src++;
      }
      if (dst<m_rep.size()) result.resize(dst);
      return QCString(result);
    }

    /// return a copy of this string with leading and trailing whitespace removed and multiple
    /// whitespace characters replaced by a single space
    QCString simplifyWhiteSpace() const;

    // Returns a copy of this string repeated n times
    QCString repeat(unsigned int n) const
    {
      QCString result(n * size(), ExplicitSize);
      size_t offset = 0;
      for (offset = 0; offset < n * size(); offset += size())
      {
        memcpy(result.rawData() + offset, data(), size());
      }
      return result;
    }

    QCString &insert( size_t index, const QCString &s )
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

    QCString &insert( size_t index, std::string_view s)
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

    QCString &insert( size_t index, const char *s )
    {
      size_t len = s ? qstrlen(s) : 0;
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

    QCString &insert( size_t index, char c)
    {
      char s[2] = { c, '\0' };
      return insert(index,s);
    }

    QCString &append( char c)
    {
      m_rep+=c;
      return *this;
    }

    QCString &append( const char *s )
    {
      return operator+=(s);
    }

    QCString &append( const QCString &s )
    {
      return operator+=(s);
    }

    QCString &append( const std::string &s )
    {
      return operator+=(s);
    }

    QCString &append( std::string_view s)
    {
      return operator+=(s);
    }

    QCString &prepend( const char *s )
    {
      return insert(0,s);
    }

    QCString &prepend( const QCString &s )
    {
      return insert(0,s.data());
    }

    QCString &prepend( const std::string &s )
    {
      return insert(0,s.c_str());
    }

    QCString &prepend( std::string_view s)
    {
      return insert(0,s);
    }

    QCString &remove( size_t index, size_t len )
    {
      size_t ol = m_rep.size();
      if (index<ol && len>0) m_rep.erase(index,index+len>=ol ? std::string::npos : len);
      return *this;
    }

    QCString &replace( size_t index, size_t len, const char *s);

    short         toShort(  bool *ok=nullptr, int base=10 ) const;
    uint16_t      toUShort( bool *ok=nullptr, int base=10 ) const;
    int	          toInt(    bool *ok=nullptr, int base=10 ) const;
    uint32_t      toUInt(   bool *ok=nullptr, int base=10 ) const;
    long          toLong(   bool *ok=nullptr, int base=10 ) const;
    unsigned long toULong(  bool *ok=nullptr, int base=10 ) const;
    uint64_t      toUInt64( bool *ok=nullptr, int base=10 ) const;

    QCString &setNum(short n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(uint16_t n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(int n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(uint32_t n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(long long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(unsigned long long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(unsigned long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    bool startsWith( const char *s ) const
    {
      if (m_rep.empty() || s==nullptr) return s==nullptr;
      return m_rep.rfind(s,0)==0; // looking "backward" starting and ending at index 0
    }

    bool startsWith( const QCString &s ) const
    {
      if (m_rep.empty() || s.isEmpty()) return s.isEmpty();
      return m_rep.rfind(s.str(),0)==0; // looking "backward" starting and ending at index 0
    }

    bool endsWith(const char *s) const
    {
      if (m_rep.empty() || s==nullptr) return s==nullptr;
      size_t l = strlen(s);
      return m_rep.length()>=l && m_rep.compare(m_rep.length()-l, l, s, l)==0;
    }

    bool endsWith(const QCString &s) const
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

    QCString &operator+=( const QCString &s)
    {
      m_rep+=s.str();
      return *this;
    }

    QCString &operator+=( const std::string &s)
    {
      m_rep+=s;
      return *this;
    }

    QCString &operator+=(std::string_view s)
    {
      m_rep+=s;
      return *this;
    }

    /** Appends string \a str to this string and returns a reference to the result. */
    QCString &operator+=( const char *s )
    {
      if (s) m_rep+=s;
      return *this;
    }

#define HAS_CHARACTER_APPEND_OPERATOR 1
#if HAS_CHARACTER_APPEND_OPERATOR
    /** Appends character \a c to this string and returns a reference to the result. */
    QCString &operator+=( char c )
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

  private:
    std::string m_rep;
};

/*****************************************************************************
  QCString non-member operators
 *****************************************************************************/

inline bool operator==( const QCString &s1, const QCString &s2 )
{ return s1.str() == s2.str(); }

inline bool operator==( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) == 0; }

inline bool operator==( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) == 0; }

inline bool operator!=( const QCString &s1, const QCString &s2 )
{ return s1.str() != s2.str(); }

inline bool operator!=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) != 0; }

inline bool operator!=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) != 0; }

inline bool operator<( const QCString &s1, const QCString& s2 )
{ return qstrcmp(s1.data(),s2.data()) < 0; }

inline bool operator<( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) < 0; }

inline bool operator<( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) < 0; }

inline bool operator<=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) <= 0; }

inline bool operator<=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) <= 0; }

inline bool operator>( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) > 0; }

inline bool operator>( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) > 0; }

inline bool operator>=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) >= 0; }

inline bool operator>=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) >= 0; }

inline QCString operator+( const QCString &s1, const QCString &s2 )
{
  return QCString(s1.str()+s2.str());
}


inline QCString operator+( const QCString &s1, const char *s2 )
{
    QCString tmp(s1);
    tmp.append(s2);
    return tmp;
}

inline QCString operator+( const char *s1, const QCString &s2 )
{
    QCString tmp(s1);
    tmp.append(s2);
    return tmp;
}

inline const char *qPrint(const char *s)
{
  if (s) return s; else return "";
}

inline const char *qPrint(const QCString &s)
{
  if (!s.isEmpty()) return s.data(); else return "";
}

inline const char *qPrint(const std::string &s)
{
  return s.c_str();
}

inline std::string toStdString(const QCString &s)
{
  return s.str();
}

//---- overloads

inline int qstrcmp( const QCString &str1, const char *str2 )
{
  return qstrcmp(str1.data(),str2);
}

inline int qstrcmp( const char *str1, const QCString &str2 )
{
  return qstrcmp(str1,str2.data());
}

inline int qstrcmp( const QCString &str1, const QCString &str2 )
{
  return qstrcmp(str1.data(),str2.data());
}

inline int qstricmp( const QCString &str1, const char *str2 )
{
  return qstricmp(str1.data(),str2);
}

inline int qstricmp( const char *str1, const QCString &str2 )
{
  return qstricmp(str1,str2.data());
}

inline int qstricmp( const QCString &str1, const QCString &str2 )
{
  return qstricmp(str1.data(),str2.data());
}

inline int qstricmp_sort( const QCString &str1, const char *str2 )
{
  return qstricmp_sort(str1.data(),str2);
}

inline int qstricmp_sort( const char *str1, const QCString &str2 )
{
  return qstricmp_sort(str1,str2.data());
}

inline int qstricmp_sort( const QCString &str1, const QCString &str2 )
{
  return qstricmp_sort(str1.data(),str2.data());
}


inline int qstrnicmp( const QCString &str1, const char *str2, size_t len )
{
  return qstrnicmp(str1.data(),str2,len);
}

inline int qstrnicmp( const char *str1, const QCString &str2, size_t len )
{
  return qstrnicmp(str1,str2.data(),len);
}

inline int qstrnicmp( const QCString &str1, const QCString &str2, size_t len )
{
  return qstrnicmp(str1.data(),str2.data(),len);
}

// helper functions
QCString substitute(const QCString &str,const QCString &find,const QCString &replace);
inline QCString substitute(const QCString &str,const char *find,const char *replace)
{
  return substitute(str,QCString(find),QCString(replace));
}
QCString substitute(const QCString &s,const QCString &src,const QCString &dst,int skip_seq);

inline QCString substitute(const QCString &s,char srcChar,char dstChar)
{
  std::string ss = s.str();
  std::replace(ss.begin(),ss.end(),srcChar,dstChar);
  return QCString(ss);
}

inline std::ostream& operator<<(std::ostream& os, const QCString& s)
{
    os << s.str();
    return os;
}

#endif // QCSTRING_H

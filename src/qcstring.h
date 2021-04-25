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
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned	uint;
typedef unsigned long	ulong;
typedef int64_t         int64;
typedef uint64_t        uint64;
#define ASSERT(x)  if ( !(x) )\
	fprintf(stderr,"ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__)


/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

void *qmemmove( void *dst, const void *src, uint len );

#if defined(_OS_WIN32_)
#define qsnprintf _snprintf
#else
#define qsnprintf snprintf
#endif

char *qstrdup( const char * );

inline uint cstrlen( const char *str )
{ return (uint)strlen(str); }

inline uint qstrlen( const char *str )
{ return str ? (uint)strlen(str) : 0; }

inline char *cstrcpy( char *dst, const char *src )
{ return strcpy(dst,src); }

inline char *qstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : 0; }

char * qstrncpy(char *dst,const char *src, uint len);

inline int cstrcmp( const char *str1, const char *str2 )
{ return strcmp(str1,str2); }

inline bool qisempty( const char *s)
{ return s==0 || *s==0; }

inline int qstrcmp( const char *str1, const char *str2 )
{ return (str1 && str2) ? strcmp(str1,str2) :     // both non-empty
         (qisempty(str1) && qisempty(str2)) ? 0 : // both empty
         qisempty(str1) ? -1 : 1;                 // one empty, other non-empty
}

inline int cstrncmp( const char *str1, const char *str2, uint len )
{ return strncmp(str1,str2,len); }

inline int qstrncmp( const char *str1, const char *str2, uint len )
{ return (str1 && str2) ? strncmp(str1,str2,len) :  // both non-empty
         (qisempty(str1) && qisempty(str2)) ? 0 :   // both empty
         qisempty(str1) ? -1 : 1;                   // one empty other non-empty
}

inline bool qisspace(char c)
{ return c==' ' || c=='\t' || c=='\n' || c=='\r'; }

int qstricmp( const char *str1, const char *str2 );

int qstrnicmp( const char *str1, const char *str2, uint len );


/** This is an alternative implementation of QCString. It provides basically
 *  the same functions but uses std::string as the underlying string type
 */
class QCString
{
  public:
    QCString() = default;
   ~QCString() = default;
    QCString( const QCString &s ) = default;
    QCString &operator=( const QCString &s ) = default;
    QCString( QCString &&s ) = default;
    QCString &operator=( QCString &&s ) = default;

    explicit QCString( const std::string &s ) : m_rep(s) {}

    QCString( std::string &&s) { m_rep = std::move(s); }

    /** creates a string with room for size characters
     *  @param[in] size the number of character to allocate (also counting the 0-terminator!)
     */
    explicit QCString( uint size ) { m_rep.resize(size>0 ? size-1 : 0); }

    /** creates a string from a plain C string.
     *  @param[in] str A zero terminated C string. When 0 an empty string is created.
     */
    QCString( const char *str ) : m_rep(str?str:"") {}

    /** creates a string from \a str and copies over the first \a maxlen characters. */
    QCString( const char *str, uint maxlen ) : m_rep(str?str:"") { m_rep.resize(maxlen); }

    /** replaces the contents by that of C string \a str. */
    QCString &operator=( const char *str) { m_rep = str?str:""; return *this; }

    QCString &operator=( const std::string &s) { m_rep = s; return *this; }

    /** Returns TRUE iff the string is empty. Equivalent to isEmpty(). */
    bool isNull() const { return m_rep.empty(); }

    /** Returns TRUE iff the string is empty */
    bool isEmpty() const { return m_rep.empty(); }

    /** Returns the length of the string, not counting the 0-terminator. Equivalent to size(). */
    uint length() const { return (uint)m_rep.size(); }

    /** Returns the length of the string, not counting the 0-terminator. */
    uint size() const { return (uint)m_rep.size(); }

    /** Returns a pointer to the contents of the string in the form of a 0-terminated C string */
    const char *data() const { return m_rep.c_str(); }

    /** Returns a writable pointer to the data.
     */
    char *rawData() const { return m_rep.empty() ? 0 : const_cast<char*>(&m_rep[0]); }

    /** Resizes the string to hold \a newlen characters
     *  (this value should also count the 0-terminator).
     *  If the string is enlarged the contents will
     *  be left unmodified.
     */
    bool resize( uint newlen ) { m_rep.resize( newlen>0 ? newlen-1 : 0 ); return TRUE; }

    /** Truncates the string at position \a pos. */
    bool truncate( uint pos ) { return resize( pos + 1 ); }

    /** Fills a string with a predefined character
     *  @param[in] c the character used to fill the string with.
     *  @param[in] len the number of character to fill. Use -1 to fill the whole string.
     *  @note the string will be resized to contain \a len characters. The contents of the
     *  string will be lost.
     */
    bool fill( char c, int len = -1 )
    {
      int l = len==-1 ? (int)m_rep.size() : len;
      m_rep = std::string(l,c);
      return TRUE;
    }

    QCString &sprintf( const char *format, ... );

    int	find( char c, int index=0, bool cs=TRUE ) const;
    int	find( const char *str, int index=0, bool cs=TRUE ) const;
    int find( const QCString &str, int index=0, bool cs=TRUE ) const;
    //int	find( const QRegExp &rx, int index=0 ) const;

    int	findRev( char c, int index=-1, bool cs=TRUE) const;
    int	findRev( const char *str, int index=-1, bool cs=TRUE) const;
    //int	findRev( const QRegExp &rx, int index=-1 ) const;

    int	contains( char c, bool cs=TRUE ) const;
    int	contains( const char *str, bool cs=TRUE ) const;
    //int contains( const QRegExp &rx ) const;

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

    QCString left( uint len ) const
    {
      return m_rep.empty() ? QCString() : QCString(m_rep.substr(0,len));
    }

    QCString right( uint len ) const
    {
      return m_rep.empty()    ? QCString() :
             len<m_rep.size() ? QCString(m_rep.substr(m_rep.size()-len,len)) :
             *this;
    }

    QCString mid( uint index, uint len=(uint)-1) const
    {
      uint slen = (uint)m_rep.size();
      if (len==(uint)-1) len = slen-index;
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

    QCString stripWhiteSpace() const
    {
      int sl = (uint)m_rep.size();
      if (sl==0 || (!qisspace(m_rep[0]) && !qisspace(m_rep[sl-1]))) return *this;
      int start=0,end=sl-1;
      while (start<sl && qisspace(m_rep[start])) start++;
      if (start==sl) return QCString(); // only whitespace
      while (end>start && qisspace(m_rep[end])) end--;
      return QCString(m_rep.substr(start,1+end-start));
    }

    QCString simplifyWhiteSpace() const;

    QCString &insert( uint index, const QCString &s )
    {
      if (s.length()>0)
      {
        uint ol = (uint)m_rep.size();
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
    QCString &insert( uint index, const char *s )
    {
      uint len = s ? qstrlen(s) : 0;
      if (len>0)
      {
        uint ol = (uint)m_rep.size();
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

    QCString &insert( uint index, char c)
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

    QCString &remove( uint index, uint len )
    {
      uint ol = (uint)m_rep.size();
      if (index<ol && len>0) m_rep.erase(index,index+len>=ol ? std::string::npos : len);
      return *this;
    }

    QCString &replace( uint index, uint len, const char *s);
    //QCString &replace( const QRegExp &rx, const char *str );

    short  toShort(  bool *ok=0, int base=10 ) const;
    ushort toUShort( bool *ok=0, int base=10 ) const;
    int	   toInt(    bool *ok=0, int base=10 ) const;
    uint   toUInt(   bool *ok=0, int base=10 ) const;
    long   toLong(   bool *ok=0, int base=10 ) const;
    ulong  toULong(  bool *ok=0, int base=10 ) const;
    uint64 toUInt64( bool *ok=0, int base=10 ) const;

    QCString &setNum(short n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(ushort n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(int n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(uint n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(long n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    QCString &setNum(ulong n)
    {
      m_rep = std::to_string(n);
      return *this;
    }

    bool startsWith( const char *s ) const
    {
      if (m_rep.empty() || s==0) return s==0;
      return m_rep.rfind(s,0)==0; // looking "backward" starting and ending at index 0
    }

#define HAS_IMPLICIT_CAST_TO_PLAIN_C_STRING 0
#if HAS_IMPLICIT_CAST_TO_PLAIN_C_STRING
    /** Converts the string to a plain C string */
    operator const char *() const
    {
      return data();
    }
#endif

    std::string str() const
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
    char &at( uint i) const
    {
      return const_cast<char&>(m_rep[i]);
    }

    /** Indexing operator. Equivalent to at(). */
    char &operator[]( int i ) const
    {
      return const_cast<char&>(m_rep[i]);
    }

  private:
    std::string m_rep;
};

/*****************************************************************************
  QCString non-member operators
 *****************************************************************************/

inline bool operator==( const QCString &s1, const QCString &s2 )
{ return qstrcmp(s1.data(),s2.data()) == 0; }

inline bool operator==( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) == 0; }

inline bool operator==( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) == 0; }

inline bool operator!=( const QCString &s1, const QCString &s2 )
{ return qstrcmp(s1.data(),s2.data()) != 0; }

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

#define HAD_PLUS_OPERATOR_FOR_CHAR 0
#if HAS_PLUS_OPERATOR_FOR_CHAR
inline QCString operator+( const QCString &s1, char c2 )
{
    QCString tmp( s1.data() );
    tmp.append(c2);
    return tmp;
}

inline QCString operator+( char c1, const QCString &s2 )
{
    QCString tmp;
    tmp.append(c1);
    tmp.append(s2);
    return tmp;
}
#endif

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

inline int qstrnicmp( const QCString &str1, const char *str2, uint len )
{
  return qstrnicmp(str1.data(),str2,len);
}

inline int qstrnicmp( const char *str1, const QCString &str2, uint len )
{
  return qstrnicmp(str1,str2.data(),len);
}

inline int qstrnicmp( const QCString &str1, const QCString &str2, uint len )
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

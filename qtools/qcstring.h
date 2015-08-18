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

#ifndef QT_H
#include "qarray.h"
#endif // QT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_OS_WIN32_) || defined(__MINGW32__)
#include <stdint.h>
#endif

#if defined(_OS_SUN_) && defined(_CC_GNU_)
#include <strings.h>
#endif

#include <assert.h>

class QGString;

/*****************************************************************************
  Fixes and workarounds for some platforms
 *****************************************************************************/

#if defined(_OS_HPUX_)
// HP-UX has badly defined strstr() etc.
// ### fix in 3.0: change hack_* to qt_hack_*
// by the way HP-UX is probably right, the standard has evolved and
// we'll have to adapt to it
inline char *hack_strstr( const char *s1, const char *s2 )
{ return (char *)strstr(s1, s2); }
inline char *hack_strchr( const char *s, int c )
{ return (char *)strchr(s, c); }
inline char *hack_strrchr( const char *s, int c )
{ return (char *)strrchr(s, c); }
#define strstr(s1,s2)	hack_strstr((s1),(s2))
#define strchr(s,c)	hack_strchr((s),(c))
#define strrchr(s,c)	hack_strrchr((s),(c))
#endif

/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

Q_EXPORT void *qmemmove( void *dst, const void *src, uint len );

#if defined(_OS_SUN_) || defined(_CC_OC_)
#define memmove(s1,s2,n) qmemmove((s1),(s2),(n))
#endif

#if defined(_OS_WIN32_)
#define qsnprintf _snprintf
#else
#define qsnprintf snprintf
#endif

Q_EXPORT char *qstrdup( const char * );

Q_EXPORT inline uint cstrlen( const char *str )
{ return (uint)strlen(str); }

Q_EXPORT inline uint qstrlen( const char *str )
{ return str ? (uint)strlen(str) : 0; }

Q_EXPORT inline char *cstrcpy( char *dst, const char *src )
{ return strcpy(dst,src); }

Q_EXPORT inline char *qstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : 0; }

Q_EXPORT char * qstrncpy(char *src,const char *dst, uint len);

Q_EXPORT inline int cstrcmp( const char *str1, const char *str2 )
{ return strcmp(str1,str2); }

Q_EXPORT inline int qstrcmp( const char *str1, const char *str2 )
{ return (str1 && str2) ? strcmp(str1,str2) : (int)((intptr_t)str2 - (intptr_t)str1); }

Q_EXPORT inline int cstrncmp( const char *str1, const char *str2, uint len )
{ return strncmp(str1,str2,len); }

Q_EXPORT inline int qstrncmp( const char *str1, const char *str2, uint len )
{ return (str1 && str2) ? strncmp(str1,str2,len) :
			  (int)((intptr_t)str2 - (intptr_t)str1); }

Q_EXPORT int qstricmp( const char *str1, const char *str2 );

Q_EXPORT int qstrnicmp( const char *str1, const char *str2, uint len );

/*****************************************************************************
  QByteArray class
 *****************************************************************************/

#if defined(Q_TEMPLATEDLL)
template class Q_EXPORT QArray<char>;
#endif
typedef QArray<char> QByteArray;

/*****************************************************************************
  QByteArray stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const QByteArray & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QByteArray & );
#endif

class QRegExp;

/** This is an alternative implementation of QCString. It provides basically
 *  the same functions but uses reference counting and copy on write.
 */
class QCString
{
public:
    /** creates an empty string */
    QCString() : m_rep()
    {
    }

    /** destroys the string */
   ~QCString()
    {
    }

    /** makes a copy of a string. */
    QCString( const QCString &s ) : m_rep(s.m_rep)
    {
    }

    /** creates a string with room for size characters
     *  @param[in] size the number of character to allocate (including the 0-terminator)
     */
    explicit QCString( int size ) : m_rep(size)
    {
    }

    /** creates a string from a plain C string.
     *  @param[in] str A zero terminated C string. When 0 an empty string is created.
     */
    QCString( const char *str ) : m_rep(str)
    {
    }

    /** creates a string from \a str and copies over the first \a maxlen characters. */
    QCString( const char *str, uint maxlen ) : m_rep(str,maxlen)
    {
    }

    /** replaces the contents by that of string \a s. */
    QCString &operator=( const QCString &s )
    {
      m_rep = s.m_rep;
      return *this;
    }

    /** replaces the contents by that of C string \a str. */
    QCString &operator=( const char *str)
    {
      m_rep = str;
      return *this;
    }

    /** Returns TRUE iff the string is empty. Equivalent to isEmpty(). */
    bool isNull() const
    {
      return m_rep.isEmpty();
    }

    /** Returns TRUE iff the string is empty */
    bool isEmpty() const
    {
      return m_rep.isEmpty();
    }

    /** Returns the length of the string, excluding the 0-terminator. Equivalent to size(). */
    uint length() const
    {
      return m_rep.length();
    }

    /** Returns the length of the string, excluding the 0-terminator. */
    uint size() const
    {
      return m_rep.length();
    }

    /** Returns a pointer to the contents of the string in the form of a 0-terminated C string */
    const char *data() const
    {
      return m_rep.data();
    }

    /** Returns a writable pointer to the data.
     *  @warning if the string is shared it will modifying the string directly and
     *  this will overwrite all copies as well!
     */
    char *rawData() const
    {
      return m_rep.rawData();
    }

    /** Resizes the string to hold \a newlen characters
     *  (this value should include the 0-terminator). If the string is enlarged the contents will
     *  be left unmodified.
     */
    bool resize( uint newlen )
    {
      m_rep.resize(newlen);
      return TRUE;
    }

    /** Truncates the string at position \a pos. */
    bool truncate( uint pos )
    {
      return resize(pos+1);
    }

    /** Fills a string with a predefined character
     *  @param[in] c the character used to fill the string with.
     *  @param[in] len the number of character to fill. Use -1 to fill the whole string.
     *  @note the string will be resized to contain \a len characters. The contents of the
     *  string will be lost.
     */
    bool fill( char c, int len = -1 )
    {
      m_rep.fill(c,len);
      return TRUE;
    }

    /** Returns a deep copy of the string. */
    QCString copy() const
    {
      if (length()==0) return QCString();
      QCString cs(length()+1);
      memcpy(cs.rawData(),data(),length());
      return cs;
    }

    QCString &sprintf( const char *format, ... );
    int	find( char c, int index=0, bool cs=TRUE ) const;
    int	find( const char *str, int index=0, bool cs=TRUE ) const;
    int find( const QCString &str, int index=0, bool cs=TRUE ) const;
    int	find( const QRegExp &rx, int index=0 ) const;
    int	findRev( char c, int index=-1, bool cs=TRUE) const;
    int	findRev( const char *str, int index=-1, bool cs=TRUE) const;
    int	findRev( const QRegExp &rx, int index=-1 ) const;
    int	contains( char c, bool cs=TRUE ) const;
    int	contains( const char *str, bool cs=TRUE ) const;
    int contains( const QRegExp &rx ) const;
    bool stripPrefix(const char *prefix);
    QCString left( uint len ) const;
    QCString right( uint len ) const;
    QCString mid( uint index, uint len=0xffffffff) const;
    QCString lower() const;
    QCString upper() const;
    QCString stripWhiteSpace() const;
    QCString simplifyWhiteSpace() const;
    QCString &assign( const char *str );
    QCString &insert( uint index, const char *s );
    QCString &insert( uint index, char c);
    QCString &append( const char *s );
    QCString &prepend( const char *s );
    QCString &remove( uint index, uint len );
    QCString &replace( uint index, uint len, const char *s);
    QCString &replace( const QRegExp &rx, const char *str );
    short toShort( bool *ok=0 ) const;
    ushort toUShort( bool *ok=0 ) const;
    int	toInt( bool *ok=0 ) const;
    uint toUInt( bool *ok=0 ) const;
    long toLong( bool *ok=0 ) const;
    ulong toULong( bool *ok=0 )	const;
    uint64 toUInt64( bool *ok=0 ) const;
    QCString &setNum(short n);
    QCString &setNum(ushort n);
    QCString &setNum(int n);
    QCString &setNum(uint n);
    QCString &setNum(long n);
    QCString &setNum(ulong n);

    /** Converts the string to a plain C string */
    operator const char *() const
    {
      return (const char *)data();
    }

    /** Appends string \a str to this string and returns a reference to the result. */
    QCString &operator+=( const char *str )
    {
      if (!str) return *this;
      int len1 = length();
      int len2 = (int)strlen(str);
      resize(len1+len2+1);
      memcpy(rawData()+len1,str,len2);
      return *this;
    }

    /** Appends character \a c to this string and returns a reference to the result. */
    QCString &operator+=( char c )
    {
      int len = length();
      resize(len+2);
      rawData()[len]=c;
      return *this;
    }

    /** Returns a reference to the character at index \a i. */
    char &at( uint i) const
    {
      return m_rep.at(i);
    }

    /** Indexing operator. Equavalent to at(). */
    char &operator[]( int i ) const
    {
      return m_rep.at((uint)i);
    }

  private:

    struct LSData;

    // long string representation
    struct LongStringRep
    {
      uchar isShort; // : 1;  // should be shared with ShortStringRep
      //uchar : 7;
      LSData *d;
    };

#define SHORT_STR_CAPACITY ((int)sizeof(LongStringRep)-2)
#define SHORT_STR_MAX_LEN (SHORT_STR_CAPACITY-1)

    // short string representation
    struct ShortStringRep
    {
      uchar isShort; // : 1;  // should be shared with LongStringRep
      uchar len; //     : 7;
      char str[SHORT_STR_CAPACITY]; // size including 0-terminator
    };

    // ref counting string header
    struct LSHeader
    {
      int           len;      // length of string without 0 terminator
      int           refCount; // -1=leaked, 0=one ref & non-cost, n>0, n+1 refs, const
    };
    // ref counting string data and methods
    struct LSData : public LSHeader
    {
      char *toStr()
      {
        return (char*)(this+1); // string data starts after the header
      }

      // creates a LSData item with room for size bytes (which includes the 0 terminator!)
      // if size is zero, an empty string will be created.
      static LSData *create(int size)
      {
        LSData *data;
        data = (LSData*)malloc(sizeof(LSHeader)+size);
        data->len = size-1;
        data->refCount = 0;
        data->toStr()[size-1] = 0;
        return data;
      }
      // remove out reference to the data. Frees memory if no more users
      void dispose()
      {
        if (--refCount<0) free(this);
      }

      // resizes LSData so it can hold size bytes (which includes the 0 terminator!)
      // Since this is for long strings only, size should be > SHORT_STR_CAPACITY
      static LSData *resize(LSData *d,int size)
      {
        if (d->len>0 && d->refCount==0) // non-const, non-empty
        {
          d = (LSData*)realloc(d,sizeof(LSHeader)+size);
          d->len = size-1;
          d->toStr()[size-1] = 0;
          return d;
        }
        else // need to make a copy
        {
          LSData *newData = LSData::create(size);
          int len = d->len;
          if (len>=size) len=size-1;
          memcpy(newData->toStr(),d->toStr(),len);
          newData->toStr()[len]=0;
          d->dispose();
          return newData;
        }
      }
    };

    class StringRep
    {
      public:
        StringRep()
        {
          initEmpty();
        }
        void initEmpty()
        {
          u.s.isShort=TRUE;
          u.s.len=0;
          //memset(u.s.str,0,SHORT_STR_CAPACITY);
        }
       ~StringRep()
        {
          if (!u.s.isShort)
          {
            u.l.d->dispose();
          }
        }
        StringRep(const StringRep &s)
        {
          if (&s!=this)
          {
            u.s.isShort = s.u.s.isShort;
            if (s.u.s.isShort)
            {
              u.s.len = s.u.s.len;
              memcpy(u.s.str,s.u.s.str,s.u.s.len+1);
            }
            else
            {
              u.l.d = s.u.l.d;
              u.l.d->refCount++;
            }
          }
          else // self-assignment
          {
            u = s.u; // avoid uninitialized warning from gcc
          }
        }
        StringRep(int size)
        {
          u.s.isShort = size<=SHORT_STR_CAPACITY;
          if (size<=SHORT_STR_CAPACITY) // init short string
          {
            if (size>0)
            {
              u.s.len = size-1;
              u.s.str[size-1]='\0';
            }
            else
            {
              u.s.len = 0;
            }
          }
          else // int long string
          {
            u.l.d = LSData::create(size);
          }
        }
        StringRep(const char *str)
        {
          if (str)
          {
            int len = (int)strlen(str);
            u.s.isShort = len<SHORT_STR_CAPACITY;
            if (len<SHORT_STR_CAPACITY)
            {
              u.s.len = len;
              qstrncpy(u.s.str,str,SHORT_STR_CAPACITY);
            }
            else
            {
              u.l.d = LSData::create(len+1);
              memcpy(u.l.d->toStr(),str,u.l.d->len);
            }
          }
          else // create empty string
          {
            initEmpty();
          }
        }
        StringRep( const char *str, uint maxlen )
        {
          if (str && maxlen>0)
          {
            uint len=(uint)strlen(str);
            if (len>maxlen) len=maxlen;
            u.s.isShort = len<=SHORT_STR_MAX_LEN;
            if (u.s.isShort)
            {
              u.s.len = len;
              memcpy(u.s.str,str,len);
              u.s.str[len]='\0';
            }
            else
            {
              u.l.d = LSData::create(len+1);
              memcpy(u.l.d->toStr(),str,len);
            }
          }
          else // create empty string
          {
            initEmpty();
          }
        }
        StringRep &operator=(const StringRep &s)
        {
          if (&s!=this)
          {
            if (!u.s.isShort)
            {
              u.l.d->dispose();
            }
            u.s.isShort = s.u.s.isShort;
            if (u.s.isShort) // copy by value
            {
              u.s.len = s.u.s.len;
              memcpy(u.s.str,s.u.s.str,s.u.s.len+1);
            }
            else // copy by reference
            {
              u.l.d = s.u.l.d;
              u.l.d->refCount++;
            }
          }
          else // self-assignment
          {
            u = s.u; // avoid uninitialized warning from gcc
          }
          return *this;
        }
        StringRep &operator=(const char *str)
        {
          if (!u.s.isShort)
          {
            u.l.d->dispose();
          }
          if (str)
          {
            int len = (int)strlen(str);
            u.s.isShort = len<SHORT_STR_CAPACITY;
            if (len<SHORT_STR_CAPACITY)
            {
              u.s.len = len;
              qstrncpy(u.s.str,str,SHORT_STR_CAPACITY);
            }
            else
            {
              u.l.d = LSData::create(len+1);
              memcpy(u.l.d->toStr(),str,len);
            }
          }
          else
          {
            initEmpty();
          }
          return *this;
        }
        bool isEmpty() const
        {
          return u.s.isShort && u.s.len==0;
        }
        uint length() const
        {
          uint l = u.s.isShort ? u.s.len : u.l.d->len;
          return l;
        }
        const char *data() const
        {
          if (u.s.isShort)
          {
            return u.s.len==0 ? 0 : u.s.str;
          }
          else
          {
            return u.l.d->len==0 ? 0 : u.l.d->toStr();
          }
        }
        char *rawData() const
        {
          if (u.s.isShort)
          {
            return u.s.len==0 ? 0 : (char*)u.s.str;
          }
          else
          {
            //assert(u.l.d->refCount==0); // string may not be shared when accessed raw
            return u.l.d->len==0 ? 0 : u.l.d->toStr();
          }
        }
        char &at(int i) const
        {
          if (u.s.isShort)
          {
            return (char&)u.s.str[i];
          }
          else
          {
            return u.l.d->toStr()[i];
          }
        }
        bool resize( uint newlen )
        {
          if (u.s.isShort && newlen<=SHORT_STR_CAPACITY) // resize short string
          {
            if (newlen>0)
            {
              u.s.len = newlen-1;
              u.s.str[newlen-1]='\0';
            }
            else // string becomes empty
            {
              initEmpty();
            }
          }
          else if (u.s.isShort) // turn short string into long string
          {
            StringRep tmp = *this;
            u.s.isShort=FALSE;
            u.l.d = LSData::create(newlen);
            if (tmp.u.s.len>0)
            {
              memcpy(u.l.d->toStr(),tmp.u.s.str,tmp.u.s.len+1);
            }
            else
            {
              u.l.d->toStr()[0]='\0';
            }
          }
          else if (!u.s.isShort && newlen<=SHORT_STR_CAPACITY) // turn long string into short string
          {
            if (newlen>0)
            {
              StringRep tmp(newlen); // copy short part into tmp buffer
              memcpy(tmp.u.s.str,u.l.d->toStr(),newlen-1);
              tmp.u.s.str[newlen-1]='\0';
              u.l.d->dispose();
              u.s = tmp.u.s;
            }
            else
            {
              u.l.d->dispose();
              initEmpty();
            }
          }
          else // resize long string
          {
            u.l.d = u.l.d->resize(u.l.d,newlen);
          }
          return TRUE;
        }
        bool fill( char c, int len )
        {
          if (len<0) len=length();
          if (!u.s.isShort) // detach from shared string
          {
            resize(len+1);
          }
          else if (len!=(int)length())
          {
            if (len>0)
            {
              resize(len+1);
            }
          }
          if (len>0)
          {
            memset(rawData(),c,len);
          }
          return TRUE;
        }
      private:
        union ShortOrLongStringSelector
        {
          ShortStringRep s;
          LongStringRep  l;
        } u;
    };
    StringRep m_rep;

};

/*****************************************************************************
  QCString stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const QCString & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QCString & );
#endif

/*****************************************************************************
  QCString non-member operators
 *****************************************************************************/

Q_EXPORT inline bool operator==( const QCString &s1, const QCString &s2 )
{ return qstrcmp(s1.data(),s2.data()) == 0; }

Q_EXPORT inline bool operator==( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) == 0; }

Q_EXPORT inline bool operator==( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) == 0; }

Q_EXPORT inline bool operator!=( const QCString &s1, const QCString &s2 )
{ return qstrcmp(s1.data(),s2.data()) != 0; }

Q_EXPORT inline bool operator!=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) != 0; }

Q_EXPORT inline bool operator!=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) != 0; }

Q_EXPORT inline bool operator<( const QCString &s1, const QCString& s2 )
{ return qstrcmp(s1.data(),s2.data()) < 0; }

Q_EXPORT inline bool operator<( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) < 0; }

Q_EXPORT inline bool operator<( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) < 0; }

Q_EXPORT inline bool operator<=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) <= 0; }

Q_EXPORT inline bool operator<=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) <= 0; }

Q_EXPORT inline bool operator>( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) > 0; }

Q_EXPORT inline bool operator>( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) > 0; }

Q_EXPORT inline bool operator>=( const QCString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) >= 0; }

Q_EXPORT inline bool operator>=( const char *s1, const QCString &s2 )
{ return qstrcmp(s1,s2.data()) >= 0; }

Q_EXPORT inline QCString operator+( const QCString &s1, const QCString &s2 )
{
    QCString tmp(s1);
    tmp += s2;
    return tmp;
}


inline QCString operator+( const QCString &s1, const QGString &s2 );
inline QCString operator+( const QGString &s1, const QCString &s2 );


Q_EXPORT inline QCString operator+( const QCString &s1, const char *s2 )
{
    QCString tmp(s1);
    tmp += s2;
    return tmp;
}

Q_EXPORT inline QCString operator+( const char *s1, const QCString &s2 )
{
    QCString tmp(s1);
    tmp += s2;
    return tmp;
}

Q_EXPORT inline QCString operator+( const QCString &s1, char c2 )
{
    QCString tmp( s1.data() );
    tmp += c2;
    return tmp;
}

Q_EXPORT inline QCString operator+( char c1, const QCString &s2 )
{
    QCString tmp;
    tmp += c1;
    tmp += s2;
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

#endif // QCSTRING_H

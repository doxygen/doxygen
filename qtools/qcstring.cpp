/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include "qcstring.h"
#include "qgstring.h"

#include <qstring.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdatastream.h>

QCString &QCString::sprintf( const char *format, ... )
{
  va_list ap;
  va_start( ap, format );
  const int minlen=256;
  int l = length();
  if (l<minlen) { resize(minlen); l=minlen; }
  int n=vsnprintf( rawData(), l, format, ap);
  if (n<0) n=l;
  resize(n+1);
  va_end( ap );
  return *this;
}

int QCString::find( char c, int index, bool cs ) const
{
  if (index<0 || index>=(int)length()) return -1; // index outside string
  const char *pos;
  if (cs)
  {
    pos = strchr(data()+index,c);
  }
  else
  {
    pos = data()+index;
    c = tolower((unsigned char)c);
    while (*pos && tolower((unsigned char)*pos)!=c) pos++;
    if (!*pos && c) pos=0; // not found
  }
  return pos ? (int)(pos - data()) : -1;
}

int QCString::find( const char *str, int index, bool cs ) const
{
  int l = length();
  if (index<0 || index>=l) return -1; // index outside string
  if (!str)  return -1;               // no string to search for
  if (!*str) return index;           // empty string matching at index
  const char *pos;
  if (cs) // case sensitive
  {
    pos = strstr(data()+index,str);
  }
  else // case insensitive
  {
    pos = data();
    int len = qstrlen(str);
    while (*pos)
    {
      if (qstrnicmp(pos,str,len)==0) break;
      pos++;
    }
    if (!*pos) pos = 0; // not found
  }
  return pos ? (int)(pos - data()) : -1;
}

int QCString::find( const QCString &str, int index, bool cs ) const
{
  return find(str.data(),index,cs);
}

int QCString::find( const QRegExp &rx, int index ) const
{
  if ( index < 0 )
    index += length();
  return rx.match( data(), index );
}

int QCString::findRev( char c, int index, bool cs) const
{
  const char *b = data();
  const char *pos;
  int len = length();
  if (len==0) return -1; // empty string
  if (index<0) // start from end
  {
    if (cs)
    {
      pos = strrchr(b,c);
      return pos ? (int)(pos - b) : -1;
    }
    index=len;
  }
  else if (index>len) // bad index
  {
    return -1;
  }
  pos = b+index;
  if (cs)
  {
    while ( pos>=b && *pos!=c) pos--;
  }
  else
  {
    c = tolower((unsigned char)c);
    while ( pos>=b && tolower((unsigned char)*pos)!=c) pos--;
  }
  return pos>=b ? (int)(pos - b) : -1;
}

int QCString::findRev( const char *str, int index, bool cs) const
{
  int slen = qstrlen(str);
  int len = length();
  if (index<0) index = len-slen; // start from end
  else if (index>len) return -1; // bad index
  else if (index+slen>len) index=len-slen; // str would be too long
  if (index<0) return -1; // no match possible
  const char *pos = data()+index;
  if (cs) // case sensitive
  {
    for (int i=index; i>=0; i--) if (qstrncmp(pos--,str,slen)==0) return i;
  }
  else // case insensitive
  {
    for (int i=index; i>=0; i--) if (qstrnicmp(pos,str,slen)==0) return i;
  }
  return -1;
}

int QCString::findRev( const QRegExp &rx, int index ) const
{
  if ( index < 0 )			// neg index ==> start from end
    index += length();
  if ( (uint)index > length() )		// bad index
    return -1;
  while( index >= 0 ) {
    if ( rx.match( data(), index ) == index )
      return index;
    index--;
  }
  return -1;
}

int QCString::contains( char c, bool cs ) const
{
  if (length()==0) return 0;
  int count=0;
  const char *pos = data();
  if (cs)
  {
    while (*pos) if (*pos++ == c) count++;
  }
  else
  {
    c = tolower((unsigned char)c);
    while (*pos)
    {
      if (tolower((unsigned char)*pos)==c) count++;
      pos++;
    }
  }
  return count;
}

int QCString::contains( const char *str, bool cs ) const
{
  if (str==0 || length()==0) return 0;
  int count=0;
  const char *pos = data();
  int len = qstrlen(str);
  while (*pos)
  {
    if (cs)
    {
      if (qstrncmp(pos,str,len)==0) count++;
    }
    else
    {
      if (qstrnicmp(pos,str,len)==0) count++;
    }
    pos++;
  }
  return count;
}

int QCString::contains( const QRegExp &rx ) const
{
  if ( isEmpty() )
    return rx.match( data() ) < 0 ? 0 : 1;
  int count = 0;
  int index = -1;
  int len = length();
  while ( index < len-1 ) {			// count overlapping matches
    index = rx.match( data(), index+1 );
    if ( index < 0 )
      break;
    count++;
  }
  return count;
}

bool QCString::startsWith( const char *s ) const
{
  const char *p = data();
  if (p==0 || s==0) return s==0;
  while (*p!=0 && *p==*s) p++,s++;
  return *s==0;
}

bool QCString::stripPrefix(const char *prefix)
{
  if (prefix==0 || length()==0) return FALSE;
  int len = qstrlen(prefix);
  if (qstrncmp(prefix,data(),len)==0)
  {
    m_rep=mid(len,length()-len).m_rep; // need to make a deep copy
    return TRUE;
  }
  return FALSE;
}

QCString QCString::left( uint len )  const
{
  if (isEmpty())
  {
    return QCString();
  }
  else if (len>=length())
  {
    return QCString(data());
  }
  else
  {
    QCString s( len+1 );
    memcpy( s.rawData(), data(), len);
    return s;
  }
}

QCString QCString::right( uint len ) const
{
  if (isEmpty())
  {
    return QCString();
  }
  else
  {
    int l = length();
    if ((int)len>l) len=l;
    const char *pos = data() + (l-len);
    return QCString(pos);
  }
}

QCString QCString::mid( uint index, uint len) const
{
  uint slen = (uint)length();
  if (len==0xffffffff) len = slen-index;
  if (isEmpty() || index>=slen || len==0)
  {
    return QCString();
  }
  else
  {
    const char *p = data()+index;
    QCString s(len+1);
    qstrncpy( s.rawData(), p, len+1 );
    return s;
  }
}

QCString QCString::lower() const
{
  if (length()==0) return QCString();
  QCString s(data());
  char *pos = s.rawData();
  if (pos)
  {
    while (*pos)
    {
      *pos = tolower((unsigned char)*pos);
      pos++;
    }
  }
  return s;
}

QCString QCString::upper() const
{
  if (length()==0) return QCString();
  QCString s(data());
  char *pos = s.rawData();
  if (pos)
  {
    while (*pos)
    {
      *pos = toupper((unsigned char)*pos);
      pos++;
    }
  }
  return s;
}

QCString QCString::stripWhiteSpace() const
{
  if ( isEmpty() )                            // nothing to do
    return *this;

  const char *cs = data();
  int reslen = length();
  if ( !isspace((uchar)cs[0]) && !isspace((uchar)cs[reslen-1]) )
    return *this;                             // returns a copy

  QCString result(cs);
  char *s = result.rawData();
  int start = 0;
  int end = reslen - 1;
  while ( isspace((uchar) s[start]) )                 // skip white space from start
    start++;
  if ( s[start] == '\0' )
  {                                                   // only white space
    return QCString();
  }
  while ( end && isspace((uchar) s[end]) )            // skip white space from end
    end--;
  end -= start - 1;
  qmemmove( s, &s[start], end );
  result.resize( end + 1 );
  return result;
}

QCString QCString::simplifyWhiteSpace() const
{
  if ( isEmpty() )                            // nothing to do
    return *this;

  QCString result( length()+1 );
  const char *from  = data();
  char *to    = result.rawData();
  char *first = to;
  while ( TRUE )
  {
    while ( *from && isspace((uchar) *from) )
      from++;
    while ( *from && !isspace((uchar)*from) )
      *to++ = *from++;
    if ( *from )
      *to++ = 0x20;                       // ' '
    else
      break;
  }
  if ( to > first && *(to-1) == 0x20 )
    to--;
  *to = '\0';
  result.resize( (int)(to - result.data()) + 1 );
  return result;
}

QCString &QCString::assign( const char *str )
{
  return operator=(str);
}

QCString &QCString::insert( uint index, const char *s )
{
  int len = s ? qstrlen(s) : 0;
  if ( len == 0 ) return *this;
  int olen = length();
  int nlen = olen + len;
  if ((int)index>=olen)
  {
    resize(nlen+index-olen+1);
    memset(rawData()+olen, ' ', index-olen);
    memcpy(rawData()+index,s, len+1);
  }
  else
  {
    resize(nlen+1);
    qmemmove(rawData()+index+len,data()+index,olen-index+1);
    memcpy(rawData()+index,s,len);
  }
  return *this;
}

QCString &QCString::insert( uint index, char c)
{
  char buf[2];
  buf[0] = c;
  buf[1] = '\0';
  return insert( index, buf );
}
QCString &QCString::append( const char *s )
{
  return operator+=(s);
}
QCString &QCString::prepend( const char *s )
{
  return insert(0,s);
}
QCString &QCString::remove( uint index, uint len )
{
  uint olen = length();
  if ( index + len >= olen ) // range problems
  {
    if ( index < olen )  // index ok
    {
      resize( index+1 );
    }
  }
  else if ( len != 0 )
  {
    QCString tmp(olen-index-len+1);
    qmemmove( tmp.rawData(), data()+index+len, olen-index-len+1 );
    resize( olen-len+1 );
    memcpy( rawData()+index,tmp.data(),tmp.length() );
  }
  return *this;
}

QCString &QCString::replace( uint index, uint len, const char *s)
{
  remove( index, len );
  insert( index, s );
  return *this;
}

QCString &QCString::replace( const QRegExp &rx, const char *str )
{
  if ( isEmpty() )
    return *this;
  int index = 0;
  int slen  = qstrlen(str);
  int len;
  while ( index < (int)length() ) {
    index = rx.match( data(), index, &len, FALSE );
    if ( index >= 0 ) {
      replace( index, len, str );
      index += slen;
      if ( !len )
        break;	// Avoid infinite loop on 0-length matches, e.g. [a-z]*
    }
    else
      break;
  }
  return *this;
}

static bool ok_in_base( char c, int base )
{
    if ( base <= 10 )
	return c>='0' && c<='9' && (c-'0') < base;
    else
	return (c>='0' && c<='9') ||
               (c >= 'a' && c < char('a'+base-10)) ||
               (c >= 'A' && c < char('A'+base-10));
}

short QCString::toShort(bool *ok, int base) const
{
  long v = toLong( ok, base );
  if ( ok && *ok && (v < -32768 || v > 32767) ) {
    *ok = FALSE;
    v = 0;
  }
  return (short)v;
}

ushort QCString::toUShort(bool *ok,int base) const
{
  ulong v = toULong( ok, base );
  if ( ok && *ok && (v > 65535) ) {
    *ok = FALSE;
    v = 0;
  }
  return (ushort)v;
}

int QCString::toInt(bool *ok, int base) const
{
  return (int)toLong( ok, base );
}

uint QCString::toUInt(bool *ok,int base) const
{
  return (uint)toULong( ok, base );
}


long QCString::toLong(bool *ok,int base) const
{
  const char *p = data();
  long val=0;
  int l = length();
  const long max_mult = INT_MAX / base;
  bool is_ok = FALSE;
  int neg = 0;
  if ( !p )
    goto bye;
  while ( l && isspace(*p) )			// skip leading space
    l--,p++;
  if ( l && *p == '-' ) {
    l--;
    p++;
    neg = 1;
  } else if ( *p == '+' ) {
    l--;
    p++;
  }

  // NOTE: toULong() code is similar
  if ( !l || !ok_in_base(*p,base) )
    goto bye;
  while ( l && ok_in_base(*p,base) ) {
    l--;
    int dv;
    if ( *p>='0' && *p<='9' ) {
      dv = *p-'0';
    } else {
      if ( *p >= 'a' && *p <= 'z' )
        dv = *p - 'a' + 10;
      else
        dv = *p - 'A' + 10;
    }
    if ( val > max_mult || (val == max_mult && dv > (INT_MAX%base)+neg) )
      goto bye;
    val = base*val + dv;
    p++;
  }
  if ( neg )
    val = -val;
  while ( l && isspace(*p) )			// skip trailing space
    l--,p++;
  if ( !l )
    is_ok = TRUE;
bye:
  if ( ok )
    *ok = is_ok;
  return is_ok ? val : 0;
}

ulong QCString::toULong(bool *ok,int base) const
{
  const char *p = data();
  ulong val=0;
  int l = length();
  const ulong max_mult = 429496729;		// UINT_MAX/10, rounded down
  bool is_ok = FALSE;
  if ( !p )
    goto bye;
  while ( l && isspace(*p) )			// skip leading space
    l--,p++;
  if ( *p == '+' )
    l--,p++;

  // NOTE: toLong() code is similar
  if ( !l || !ok_in_base(*p,base) )
    goto bye;
  while ( l && ok_in_base(*p,base) ) {
    l--;
    uint dv;
    if ( *p>='0' && *p<='9' ) {
      dv = *p-'0';
    } else {
      if ( *p >= 'a' && *p <= 'z' )
        dv = *p - 'a' + 10;
      else
        dv = *p - 'A' + 10;
    }
    if ( val > max_mult || (val == max_mult && dv > (UINT_MAX%base)) )
      goto bye;
    val = base*val + dv;
    p++;
  }

  while ( l && isspace(*p) )			// skip trailing space
    l--,p++;
  if ( !l )
    is_ok = TRUE;
bye:
  if ( ok )
    *ok = is_ok;
  return is_ok ? val : 0;
}

uint64 QCString::toUInt64(bool *ok,int base) const
{
  const char *p = data();
  uint64 val=0;
  int l = length();
  const uint64 max_mult = 1844674407370955161ULL;  // ULLONG_MAX/10, rounded down
  bool is_ok = FALSE;
  if ( !p )
    goto bye;
  while ( l && isspace(*p) )		 	   // skip leading space
    l--,p++;
  if ( *p == '+' )
    l--,p++;

  // NOTE: toULong() code is similar
  if ( !l || !ok_in_base(*p,base) )
    goto bye;
  while ( l && ok_in_base(*p,base) ) {
    l--;
    uint dv;
    if ( *p>='0' && *p<='9' ) {
      dv = *p-'0';
    } else {
      if ( *p >= 'a' && *p <= 'z' )
        dv = *p - 'a' + 10;
      else
        dv = *p - 'A' + 10;
    }
    if ( val > max_mult || (val == max_mult && dv > (ULLONG_MAX%base)) )
      goto bye;
    val = base*val + dv;
    p++;
  }

  while ( l && isspace(*p) )			// skip trailing space
    l--,p++;
  if ( !l )
    is_ok = TRUE;
bye:
  if ( ok )
    *ok = is_ok;
  return is_ok ? val : 0;
}

QCString &QCString::setNum(short n)
{
  return setNum((long)n);
}

QCString &QCString::setNum(ushort n)
{
  return setNum((ulong)n);
}

QCString &QCString::setNum(int n)
{
  return setNum((long)n);
}

QCString &QCString::setNum(uint n)
{
  return setNum((ulong)n);
}

QCString &QCString::setNum(long n)
{
  char buf[20];
  char *p = &buf[19];
  bool neg;
  if ( n < 0 )
  {
    neg = TRUE;
    n = -n;
  }
  else
  {
    neg = FALSE;
  }
  *p = '\0';
  do
  {
    *--p = ((int)(n%10)) + '0';
    n /= 10;
  } while ( n );
  if ( neg ) *--p = '-';
  operator=( p );
  return *this;
}

QCString &QCString::setNum( ulong n)
{
  char buf[20];
  char *p = &buf[19];
  *p = '\0';
  do
  {
    *--p = ((int)(n%10)) + '0';
    n /= 10;
  } while ( n );
  operator=( p );
  return *this;
}

//-------------------------------------------------

void *qmemmove( void *dst, const void *src, uint len )
{
    char *d;
    char *s;
    if ( dst > src ) {
	d = (char *)dst + len - 1;
	s = (char *)src + len - 1;
	while ( len-- )
	    *d-- = *s--;
    } else if ( dst < src ) {
	d = (char *)dst;
	s = (char *)src;
	while ( len-- )
	    *d++ = *s++;
    }
    return dst;
}

char *qstrdup( const char *str )
{
    if ( !str )
	return 0;
    char *dst = new char[qstrlen(str)+1];
    CHECK_PTR( dst );
    return strcpy( dst, str );
}

char *qstrncpy( char *dst, const char *src, uint len )
{
    if ( !src )
	return 0;
    strncpy( dst, src, len );
    if ( len > 0 )
	dst[len-1] = '\0';
    return dst;
}

int qstricmp( const char *str1, const char *str2 )
{
    const uchar *s1 = (const uchar *)str1;
    const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return s1 == s2 ? 0 : (int)(s2 - s1);
    for ( ; !(res = (c=tolower(*s1)) - tolower(*s2)); s1++, s2++ )
	if ( !c )				// strings are equal
	    break;
    return res;
}

int qstrnicmp( const char *str1, const char *str2, uint len )
{
    const uchar *s1 = (const uchar *)str1;
    const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return (int)(s2 - s1);
    for ( ; len--; s1++, s2++ ) {
	if ( (res = (c=tolower(*s1)) - tolower(*s2)) )
	    return res;
	if ( !c )				// strings are equal
	    break;
    }
    return 0;
}

#ifndef QT_NO_DATASTREAM

QDataStream &operator<<( QDataStream &s, const QByteArray &a )
{
    return s.writeBytes( a.data(), a.size() );
}

QDataStream &operator>>( QDataStream &s, QByteArray &a )
{
    Q_UINT32 len;
    s >> len;					// read size of array
    if ( len == 0 || s.eof() ) {		// end of file reached
	a.resize( 0 );
	return s;
    }
    if ( !a.resize( (uint)len ) ) {		// resize array
#if defined(CHECK_NULL)
	qWarning( "QDataStream: Not enough memory to read QByteArray" );
#endif
	len = 0;
    }
    if ( len > 0 )				// not null array
	s.readRawBytes( a.data(), (uint)len );
    return s;
}

QDataStream &operator<<( QDataStream &s, const QCString &str )
{
    return s.writeBytes( str.data(), str.size() );
}

QDataStream &operator>>( QDataStream &s, QCString &str )
{
    Q_UINT32 len;
    s >> len;					// read size of string
    if ( len == 0 || s.eof() ) {		// end of file reached
	str.resize( 0 );
	return s;
    }
    if ( !str.resize( (uint)len )) {// resize string
#if defined(CHECK_NULL)
	qWarning( "QDataStream: Not enough memory to read QCString" );
#endif
	len = 0;
    }
    if ( len > 0 )				// not null array
	s.readRawBytes( str.rawData(), (uint)len );
    return s;
}

#endif //QT_NO_DATASTREAM

inline QCString operator+( const QCString &s1, const QGString &s2 )
{
    QCString tmp(s1);
    tmp += s2.data();
    return tmp;
}

inline QCString operator+( const QGString &s1, const QCString &s2 )
{
    QCString tmp(s1.data());
    tmp += s2;
    return tmp;
}


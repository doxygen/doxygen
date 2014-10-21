/******************************************************************************
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

// with this switch you can choose between the original qcstring implementation,
// which implicitly shares data so copying is faster, but requires at least 12 bytes, and
// the new implementation in this file, which has a smaller footprint (only 4 bytes for
// an empty string), but always copies strings.
#define SMALLSTRING

#include "qcstring.h"
#ifndef SMALLSTRING
#include "qcstring.cpp"
#else
#define SCString QCString

#include <qstring.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdatastream.h>


SCString::SCString(int size)
{
  if (size>0)
  {
    m_data = (char *)malloc(size);
    if (m_data)
    {
      if (size>1) memset(m_data,' ',size-1);
      m_data[size-1]='\0';
    }
  }
  else
  {
    m_data=0;
  }
}

SCString::SCString( const SCString &s ) 
{ 
  duplicate(s);
}

SCString::SCString( const char *str )
{ 
  duplicate(str);
}

SCString::SCString( const char *str, uint maxlen )
{ 
  uint l;
  if (str && ( l = QMIN(qstrlen(str),maxlen) )) 
  { 
    m_data=(char *)malloc(l+1);
    strncpy(m_data,str,l+1);
    m_data[l]='\0';
  } 
  else
  {
    m_data=0;
  }
}

SCString::~SCString()
{
  if (m_data) free(m_data);
  m_data=0;
}

SCString &SCString::assign( const char *str )
{
  if (m_data==str) return *this;
  if (m_data) free(m_data);
  duplicate(str);
  return *this;
}

bool SCString::resize( uint newlen )
{
  if (newlen==0)
  {
    if (m_data) { free(m_data); m_data=0; }
    return TRUE;
  }
  if (m_data==0) // newlen>0
  {
    m_data = (char *)malloc(newlen);
  }
  else
  {
    m_data = (char *)realloc(m_data,newlen);
  }
  if (m_data==0) return FALSE;
  m_data[newlen-1]='\0';
  return TRUE;
}

bool SCString::fill( char c, int len )
{
  uint l=length();
  if (len<0) len=l;
  if ((uint)len!=l) 
  {
    if (m_data) free(m_data);
    if (len>0)
    {
      m_data=(char *)malloc(len+1);
      if (m_data==0) return FALSE;
      m_data[len]='\0';
    }
    else
    {
      m_data=0;
    }
  }
  if (len>0)
  {
    uint i;
    for (i=0;i<(uint)len;i++) m_data[i]=c;
  }
  return TRUE;
}

SCString &SCString::sprintf( const char *format, ... )
{
  va_list ap;
  va_start( ap, format );
  uint l = length();
  const uint minlen=4095;
  if (l<minlen)
  {
    if (m_data) 
      m_data = (char *)realloc(m_data,minlen+1);
    else
      m_data = (char *)malloc(minlen+1);
    m_data[minlen]='\0';
  }
  vsnprintf( m_data, minlen, format, ap );
  resize( qstrlen(m_data) + 1 );              // truncate
  va_end( ap );
  return *this;
}


int SCString::find( char c, int index, bool cs ) const
{
  uint len = length();
  if ( m_data==0 || (uint)index>len )               // index outside string
    return -1;
  register const char *d;
  if ( cs ) // case sensitive
  {  
    d = strchr( m_data+index, c );
  } 
  else 
  {
    d = m_data+index;
    c = tolower( (uchar) c );
    while ( *d && tolower((uchar) *d) != c )
      d++;
    if ( !*d && c )                         // not found
      d = 0;
  }
  return d ? (int)(d - m_data) : -1;
}

int SCString::find( const char *str, int index, bool cs ) const
{
  uint l = length();
  if ( m_data==0 || (uint)index > l )         // index outside string
    return -1;
  if ( !str )                                 // no search string
    return -1;
  if ( !*str )                                // zero-length search string
    return index;
  register const char *d;
  if ( cs )                                   // case sensitive
  {              
    d = strstr( m_data+index, str );
  } 
  else                                        // case insensitive
  {
    d = m_data+index;
    int len = qstrlen( str );
    while ( *d ) 
    {
      if ( qstrnicmp(d, str, len) == 0 )
        break;
      d++;
    }
    if ( !*d )                              // not found
      d = 0;
  }
  return d ? (int)(d - m_data) : -1;
}

int SCString::find( const QCString &str, int index, bool cs ) const
{
  return find(str.data(),index,cs);
}

int SCString::find( const QRegExp &rx, int index ) const
{
  QString d = QString::fromLatin1( m_data );
  return d.find( rx, index );
}

int SCString::findRev( char c, int index, bool cs) const
{
  const char *b = m_data;
  const char *d;
  uint len = length();
  if ( b == 0 ) return -1;  // empty string
  if ( index < 0 )          // neg index ==> start from end
  {
    if ( len == 0 ) return -1;
    if ( cs ) 
    {
      d = strrchr( b, c );
      return d ? (int)(d - b) : -1;
    }
    index = len;
  } 
  else if ( (uint)index > len )  // bad index
  {      
    return -1;
  }
  d = b+index;
  if ( cs )   // case sensitive
  {
    while ( d >= b && *d != c )
      d--;
  } 
  else  // case insensitive
  {
    c = tolower( (uchar) c );
    while ( d >= b && tolower((uchar) *d) != c )
      d--;
  }
  return d >= b ? (int)(d - b) : -1;
}

int SCString::findRev( const char *str, int index, bool cs) const
{
  int slen = qstrlen(str);
  uint len = length();
  if ( index < 0 )                           // neg index ==> start from end
    index = len-slen;
  else if ( (uint)index > len )              // bad index
    return -1;
  else if ( (uint)(index + slen) > len )     // str would be too long
    index = len - slen;
  if ( index < 0 )
    return -1;

  register char *d = m_data + index;
  if ( cs )                     // case sensitive 
  {      
    for ( int i=index; i>=0; i-- )
      if ( qstrncmp(d--,str,slen)==0 )
        return i;
  } 
  else                          // case insensitive
  {           
    for ( int i=index; i>=0; i-- )
      if ( qstrnicmp(d--,str,slen)==0 )
        return i;
  }
  return -1;

}

int SCString::findRev( const QRegExp &rx, int index ) const
{
  QString d = QString::fromLatin1( m_data );
  return d.findRev( rx, index );
}

int SCString::contains( char c, bool cs ) const
{
  int count = 0;
  char *d = m_data;
  if ( !d )
    return 0;                               
  if ( cs )                                // case sensitive
  {
    while ( *d )
      if ( *d++ == c )
        count++;                        
  } 
  else                                     // case insensitive
  {
    c = tolower( (uchar) c );
    while ( *d ) {
      if ( tolower((uchar) *d) == c )
        count++;
      d++;
    }
  }
  return count;
}

int SCString::contains( const char *str, bool cs ) const
{
  int count = 0;
  char *d = data();
  if ( !d )
    return 0;
  int len = qstrlen( str );
  while ( *d )   // counts overlapping strings
  {
    if ( cs ) 
    {
      if ( qstrncmp( d, str, len ) == 0 )
        count++;
    } 
    else 
    {
      if ( qstrnicmp(d, str, len) == 0 )
        count++;
    }
    d++;
  }
  return count;
}

int SCString::contains( const QRegExp &rx ) const
{ 
  QString d = QString::fromLatin1( m_data );
  return d.contains( rx );
}

SCString SCString::left( uint len )  const
{
  if ( isEmpty() ) 
  {
    return SCString();
  } 
  else if ( len >= length() ) 
  {
    return *this;
  } 
  else 
  {
    SCString s( len+1 );
    strncpy( s.data(), m_data, len );
    *(s.data()+len) = '\0';
    return s;
  }
}

SCString SCString::right( uint len ) const
{
  if ( isEmpty() ) 
  {
    return SCString();
  } 
  else 
  {
    uint l = length();
    if ( len > l ) len = l;
    char *p = m_data + (l - len); 
    return SCString( p ); 
  }       
}

SCString SCString::mid( uint index, uint len) const
{
  uint slen = length();
  if ( len == 0xffffffff ) len = slen-index;
  if ( isEmpty() || index >= slen ) 
  {
    return SCString();
  } 
  else 
  {
    register char *p = data()+index;
    SCString s( len+1 );
    strncpy( s.data(), p, len );
    *(s.data()+len) = '\0';
    return s;
  }
}

SCString SCString::lower() const
{
  SCString s( m_data );
  register char *p = s.data();
  if ( p ) 
  {
    while ( *p ) 
    {
      *p = tolower((uchar) *p);
      p++;
    }
  }
  return s;
}

SCString SCString::upper() const
{
  SCString s( m_data );
  register char *p = s.data();
  if ( p ) {
    while ( *p ) {
      *p = toupper((uchar)*p);
      p++;
    }
  }
  return s;
}

SCString SCString::stripWhiteSpace()	const
{
  if ( isEmpty() )                            // nothing to do
    return *this;

  register char *s = m_data;
  int reslen = length();
  if ( !isspace((uchar) s[0]) && !isspace((uchar) s[reslen-1]) )
    return *this;                           // returns a copy

  SCString result(s);
  s = result.data(); 
  int start = 0;
  int end = reslen - 1;
  while ( isspace((uchar) s[start]) )                 // skip white space from start
    start++; 
  if ( s[start] == '\0' ) 
  {                                                   // only white space
    return SCString();
  }
  while ( end && isspace((uchar) s[end]) )            // skip white space from end
    end--;
  end -= start - 1;
  memmove( result.data(), &s[start], end );
  result.resize( end + 1 );
  return result;
}

SCString SCString::simplifyWhiteSpace() const
{
  if ( isEmpty() )                            // nothing to do
    return *this;

  SCString result( length()+1 );
  char *from  = data();
  char *to    = result.data();
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
  result.resize( (int)((long)to - (long)result.data()) + 1 );
  return result;
}

SCString &SCString::insert( uint index, const char *s )
{   
  int len = qstrlen(s);
  if ( len == 0 )
    return *this;
  uint olen = length();
  int nlen = olen + len;                      
  if ( index >= olen )  // insert after end of string
  {                     
    m_data = (char *)realloc(m_data,nlen+index-olen+1);
    if ( m_data ) 
    {
      memset( m_data+olen, ' ', index-olen );
      memcpy( m_data+index, s, len+1 );
    }
  } 
  else if ( (m_data = (char *)realloc(m_data,nlen+1)) )  // normal insert
  { 
    memmove( m_data+index+len, m_data+index, olen-index+1 );
    memcpy( m_data+index, s, len );
  }
  return *this;
}

SCString &SCString::insert( uint index, char c )        // insert char
{
  char buf[2];
  buf[0] = c;
  buf[1] = '\0';
  return insert( index, buf );
}

SCString& SCString::operator+=( const char *str )
{
  if ( !str ) return *this;  // nothing to append
  uint len1 = length();
  uint len2 = qstrlen(str);
  char *newData = (char *)realloc( m_data, len1 + len2 + 1 );
  if (newData)
  {
    m_data = newData;
    memcpy( m_data + len1, str, len2 + 1 );
  }
  return *this;      
}

SCString &SCString::operator+=( char c )
{
  uint len = length();
  char *newData = (char *)realloc( m_data, length()+2 );
  if (newData)
  {
    m_data = newData;
    m_data[len] = c;
    m_data[len+1] = '\0';
  }
  return *this;
}

SCString &SCString::remove( uint index, uint len )
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
    memmove( m_data+index, m_data+index+len, olen-index-len+1 );
    resize( olen-len+1 );
  }
  return *this;
}

SCString &SCString::replace( uint index, uint len, const char *s )
{
  remove( index, len );
  insert( index, s );
  return *this;
}

SCString &SCString::replace( const QRegExp &rx, const char *str )
{
  QString d = QString::fromLatin1( m_data );
  QString r = QString::fromLatin1( str );
  d.replace( rx, r );
  return assign(d.ascii());
}

long  SCString::toLong( bool *ok ) const
{
  QString s(m_data);
  return s.toLong(ok);
}

ulong SCString::toULong( bool *ok ) const
{
  QString s(m_data);
  return s.toULong(ok);
}

short SCString::toShort( bool *ok )	const
{
  QString s(m_data);
  return s.toShort(ok);
}

ushort SCString::toUShort( bool *ok ) const
{
  QString s(m_data);
  return s.toUShort(ok);
}

int SCString::toInt( bool *ok ) const
{
  QString s(m_data);
  return s.toInt(ok);
}

uint SCString::toUInt( bool *ok ) const
{
  QString s(m_data);
  return s.toUInt(ok);
}

SCString &SCString::setNum( long n )
{
  char buf[20];
  register char *p = &buf[19];
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

SCString &SCString::setNum( ulong n )
{
  char buf[20];
  register char *p = &buf[19];
  *p = '\0';
  do 
  {
    *--p = ((int)(n%10)) + '0';
    n /= 10;
  } while ( n );
  operator=( p );
  return *this;
}

void SCString::msg_index( uint index )
{
#if defined(CHECK_RANGE)
    qWarning( "SCString::at: Absolute index %d out of range", index );
#else
    Q_UNUSED( index )
#endif
}

bool SCString::stripPrefix(const char *prefix)
{
  if (prefix==0) return FALSE;
  uint plen   = qstrlen(prefix);
  if (m_data && qstrncmp(prefix,m_data,plen)==0) // prefix matches
  {
    uint len    = qstrlen(m_data);
    uint newlen = len-plen+1;
    qmemmove(m_data,m_data+plen,newlen);
    resize(newlen);
    return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------

void *qmemmove( void *dst, const void *src, uint len )
{
    register char *d;
    register char *s;
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
    char *dst = new char[strlen(str)+1];
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
    register const uchar *s1 = (const uchar *)str1;
    register const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return s1 == s2 ? 0 : (int)((long)s2 - (long)s1);
    for ( ; !(res = (c=tolower(*s1)) - tolower(*s2)); s1++, s2++ )
	if ( !c )				// strings are equal
	    break;
    return res;
}

int qstrnicmp( const char *str1, const char *str2, uint len )
{
    register const uchar *s1 = (const uchar *)str1;
    register const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return (int)((long)s2 - (long)s1);
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

QDataStream &operator<<( QDataStream &s, const SCString &str )
{
    return s.writeBytes( str.data(), str.size() );
}

QDataStream &operator>>( QDataStream &s, SCString &str )
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
	s.readRawBytes( str.data(), (uint)len );
    return s;
}

#endif //QT_NO_DATASTREAM



#endif

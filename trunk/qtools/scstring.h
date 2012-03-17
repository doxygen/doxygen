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

#ifndef SCSTRING_H
#define SCSTRING_H

#include <stdlib.h>

class QRegExp;

/** This is an alternative implementation of QCString. It provides basically
 *  the same functions but uses less memory for administration. This class
 *  is just a wrapper around a plain C string requiring only 4 bytes "overhead".
 *  QCString features sharing of data and stores the string length, but 
 *  requires 4 + 12 bytes for this (even for the empty string). As doxygen 
 *  uses a LOT of string during a run it saves a lot of memory to use a 
 *  more memory efficient implementation at the cost of relatively low
 *  runtime overhead.
 */
class SCString 
{
public:
    SCString() : m_data(0) {}		// make null string
    SCString( const SCString &s );
    SCString( int size );
    SCString( const char *str );
    SCString( const char *str, uint maxlen );
    ~SCString();

    SCString    &operator=( const SCString &s );// deep copy
    SCString    &operator=( const char *str );	// deep copy

    bool        isNull()        const;
    bool	isEmpty()	const;
    uint	length()	const;
    uint        size()          const { return m_data ? length()+1 : 0; }
    char *      data()          const { return m_data; }
    bool	resize( uint newlen );
    bool	truncate( uint pos );
    bool	fill( char c, int len = -1 );

    SCString	copy()	const;

    SCString    &sprintf( const char *format, ... );

    int		find( char c, int index=0, bool cs=TRUE ) const;
    int		find( const char *str, int index=0, bool cs=TRUE ) const;
    int		find( const QRegExp &, int index=0 ) const;
    int         find( const QCString &str, int index, bool cs ) const;
    int		findRev( char c, int index=-1, bool cs=TRUE) const;
    int		findRev( const char *str, int index=-1, bool cs=TRUE) const;
    int		findRev( const QRegExp &, int index=-1 ) const;
    int		contains( char c, bool cs=TRUE ) const;
    int		contains( const char *str, bool cs=TRUE ) const;
    int		contains( const QRegExp & ) const;
    bool        stripPrefix(const char *prefix);

    SCString	left( uint len )  const;
    SCString	right( uint len ) const;
    SCString	mid( uint index, uint len=0xffffffff) const;

    SCString	lower() const;
    SCString	upper() const;

    SCString	stripWhiteSpace()	const;
    SCString	simplifyWhiteSpace()	const;

    SCString    &assign( const char *str );
    SCString    &insert( uint index, const char * );
    SCString    &insert( uint index, char );
    SCString    &append( const char *s );
    SCString    &prepend( const char *s );
    SCString    &remove( uint index, uint len );
    SCString    &replace( uint index, uint len, const char * );
    SCString    &replace( const QRegExp &, const char * );

    short	toShort( bool *ok=0 )	const;
    ushort	toUShort( bool *ok=0 )	const;
    int		toInt( bool *ok=0 )	const;
    uint	toUInt( bool *ok=0 )	const;
    long	toLong( bool *ok=0 )	const;
    ulong	toULong( bool *ok=0 )	const;

    SCString    &setNum( short );
    SCString    &setNum( ushort );
    SCString    &setNum( int );
    SCString    &setNum( uint );
    SCString    &setNum( long );
    SCString    &setNum( ulong );
    QCString    &setNum( float, char f='g', int prec=6 );
    QCString    &setNum( double, char f='g', int prec=6 );

		operator const char *() const;
    SCString    &operator+=( const char *str );
    SCString    &operator+=( char c );
    char &at( uint index ) const;
    char &operator[]( int i ) const { return at(i); }
    
  private:
    static void msg_index( uint );
    void duplicate( const SCString &s );
    void duplicate( const char *str);
    SCString &duplicate( const char *str, int);

    char *      m_data;
};

inline char &SCString::at( uint index ) const
{
  return m_data[index];
}

inline void SCString::duplicate( const SCString &s )
{
  if (!s.isEmpty()) 
  {
    uint l = strlen(s.data());
    m_data = (char *)malloc(l+1);
    if (m_data) memcpy(m_data,s.data(),l+1);
  }
  else 
    m_data=0; 
}
inline void SCString::duplicate( const char *str)
{
  if (str && str[0]!='\0') 
  {
    uint l = strlen(str);
    m_data = (char *)malloc(l+1);
    if (m_data) memcpy(m_data,str,l+1);
  }
  else 
    m_data=0;
}
inline SCString &SCString::duplicate( const char *str, int)
{
  if (m_data) free(m_data);
  duplicate(str);
  return *this;
}

#endif


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

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

inline char toLowerChar(char c)
{
  return c>='A' && c<='Z' ? c|0x20 : c;
}

QCString &QCString::sprintf( const char *format, ... )
{
  va_list ap;
  va_start( ap, format );
  const size_t minlen=256;
  size_t l = length();
  if (l<minlen) { resize(minlen+1); l=minlen; }
  int n=vsnprintf( rawData(), l+1, format, ap);
  if (n<0) n=static_cast<int>(l);
  resize(n);
  va_end( ap );
  return *this;
}

int QCString::find( char c, int index, bool cs ) const
{
  if (index<0 || index>=static_cast<int>(length())) return -1; // index outside string
  const char *pos = nullptr;
  if (cs)
  {
    pos = strchr(data()+index,c);
  }
  else
  {
    pos = data()+index;
    c = toLowerChar(c);
    while (*pos && toLowerChar(*pos)!=c) pos++;
    if (!*pos && c) pos=nullptr; // not found
  }
  return pos ? static_cast<int>(pos - data()) : -1;
}

int QCString::find( const char *str, int index, bool cs ) const
{
  int l = static_cast<int>(length());
  if (index<0 || index>=l) return -1; // index outside string
  if (!str)  return -1;               // no string to search for
  if (!*str) return index;           // empty string matching at index
  const char *pos = nullptr;
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
    if (!*pos) pos = nullptr; // not found
  }
  return pos ? static_cast<int>(pos - data()) : -1;
}

int QCString::find( const std::string &str, int index, bool cs ) const
{
  return find(str.c_str(),index,cs);
}

int QCString::find( const QCString &str, int index, bool cs ) const
{
  return find(str.data(),index,cs);
}

int QCString::findRev( char c, int index, bool cs) const
{
  const char *b = data();
  const char *pos = nullptr;
  int len = static_cast<int>(length());
  if (len==0) return -1; // empty string
  if (index<0) // start from end
  {
    if (cs)
    {
      pos = strrchr(b,c);
      return pos ? static_cast<int>(pos - b) : -1;
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
    c = toLowerChar(c);
    while ( pos>=b && toLowerChar(*pos)!=c) pos--;
  }
  return pos>=b ? static_cast<int>(pos - b) : -1;
}

int QCString::findRev( const char *str, int index, bool cs) const
{
  int slen = static_cast<int>(qstrlen(str));
  int len = static_cast<int>(length());
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
    c = toLowerChar(c);
    while (*pos)
    {
      if (toLowerChar(*pos)==c) count++;
      pos++;
    }
  }
  return count;
}

int QCString::contains( const char *str, bool cs ) const
{
  if (str==nullptr || length()==0) return 0;
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

QCString QCString::simplifyWhiteSpace() const
{
  if ( isEmpty() )                            // nothing to do
    return *this;

  QCString result( length(), ExplicitSize );
  const char *from  = data();
  char *to    = result.rawData();
  char *first = to;
  while ( TRUE )
  {
    while ( *from && qisspace(*from) )
      from++;
    while ( *from && !qisspace(*from) )
      *to++ = *from++;
    if ( *from )
      *to++ = 0x20;                       // ' '
    else
      break;
  }
  if ( to > first && *(to-1) == 0x20 )
    to--;
  *to = '\0';
  result.resize( static_cast<int>(to - result.data()) );
  return result;
}

QCString &QCString::replace( size_t index, size_t len, const char *s)
{
  remove( index, len );
  insert( index, s );
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
  return static_cast<short>(v);
}

uint16_t QCString::toUShort(bool *ok,int base) const
{
  unsigned long v = toULong( ok, base );
  if ( ok && *ok && (v > 65535) ) {
    *ok = FALSE;
    v = 0;
  }
  return static_cast<uint16_t>(v);
}

int QCString::toInt(bool *ok, int base) const
{
  return static_cast<int>(toLong( ok, base ));
}

uint32_t QCString::toUInt(bool *ok,int base) const
{
  return static_cast<uint32_t>(toULong( ok, base ));
}


long QCString::toLong(bool *ok,int base) const
{
  const char *p = data();
  long val=0;
  int l = static_cast<int>(length());
  const long max_mult = INT_MAX / base;
  bool is_ok = FALSE;
  int neg = 0;
  if ( !p )
    goto bye;
  while ( l && qisspace(*p) )			// skip leading space
  {
    l--;
    p++;
  }
  if ( l && *p == '-' )
  {
    l--;
    p++;
    neg = 1;
  } else if ( *p == '+' )
  {
    l--;
    p++;
  }

  // NOTE: toULong() code is similar
  if ( !l || !ok_in_base(*p,base) )
  {
    goto bye;
  }
  while ( l && ok_in_base(*p,base) )
  {
    l--;
    int dv = 0;
    if ( *p>='0' && *p<='9' )
    {
      dv = *p-'0';
    }
    else
    {
      if ( *p >= 'a' && *p <= 'z' )
      {
        dv = *p - 'a' + 10;
      }
      else
      {
        dv = *p - 'A' + 10;
      }
    }
    if ( val > max_mult || (val == max_mult && dv > (INT_MAX%base)+neg) )
    {
      goto bye;
    }
    val = base*val + dv;
    p++;
  }
  if ( neg )
  {
    val = -val;
  }
  while ( l && qisspace(*p) )			// skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = TRUE;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

unsigned long QCString::toULong(bool *ok,int base) const
{
  const char *p = data();
  unsigned long val=0;
  int l = static_cast<int>(length());
  const unsigned long max_mult = 429496729;		// UINT_MAX/10, rounded down
  bool is_ok = FALSE;
  if ( !p )
  {
    goto bye;
  }
  while ( l && qisspace(*p) )			// skip leading space
  {
    l--;
    p++;
  }
  if ( *p == '+' )
  {
    l--;
    p++;
  }

  // NOTE: toLong() code is similar
  if ( !l || !ok_in_base(*p,base) )
  {
    goto bye;
  }
  while ( l && ok_in_base(*p,base) )
  {
    l--;
    uint32_t dv = 0;
    if ( *p>='0' && *p<='9' )
    {
      dv = *p-'0';
    }
    else
    {
      if ( *p >= 'a' && *p <= 'z' )
      {
        dv = *p - 'a' + 10;
      }
      else
      {
        dv = *p - 'A' + 10;
      }
    }
    if ( val > max_mult || (val == max_mult && dv > (UINT_MAX%base)) )
    {
      goto bye;
    }
    val = base*val + dv;
    p++;
  }

  while ( l && qisspace(*p) )			// skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = TRUE;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

uint64_t QCString::toUInt64(bool *ok,int base) const
{
  const char *p = data();
  uint64_t val=0;
  int l = static_cast<int>(length());
  const uint64_t max_mult = 1844674407370955161ULL;  // ULLONG_MAX/10, rounded down
  bool is_ok = FALSE;
  if ( !p )
  {
    goto bye;
  }
  while ( l && qisspace(*p) )		 	   // skip leading space
  {
    l--;
    p++;
  }
  if ( *p == '+' )
  {
    l--;
    p++;
  }

  // NOTE: toULong() code is similar
  if ( !l || !ok_in_base(*p,base) )
  {
    goto bye;
  }
  while ( l && ok_in_base(*p,base) )
  {
    l--;
    uint32_t dv = 0;
    if ( *p>='0' && *p<='9' )
    {
      dv = *p-'0';
    }
    else
    {
      if ( *p >= 'a' && *p <= 'z' )
      {
        dv = *p - 'a' + 10;
      }
      else
      {
        dv = *p - 'A' + 10;
      }
    }
    if ( val > max_mult || (val == max_mult && dv > (ULLONG_MAX%base)) )
    {
      goto bye;
    }
    val = base*val + dv;
    p++;
  }

  while ( l && qisspace(*p) )			// skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = TRUE;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

//-------------------------------------------------

void *qmemmove( void *dst, const void *src, size_t len )
{
  if ( dst > src )
  {
    char *d = static_cast<char *>(dst) + len - 1;
    const char *s = static_cast<const char *>(src) + len - 1;
    while ( len-- )
    {
      *d-- = *s--;
    }
  }
  else if ( dst < src )
  {
    char *d = static_cast<char *>(dst);
    const char *s = static_cast<const char *>(src);
    while ( len-- )
    {
      *d++ = *s++;
    }
  }
  return dst;
}

char *qstrdup( const char *str )
{
  if ( !str ) return nullptr;
  char *dst = new char[qstrlen(str)+1];
  return strcpy( dst, str );
}

void qstrfree( const char *str )
{
  delete [](str);
}

char *qstrncpy( char *dst, const char *src, size_t len )
{
  if ( !src ) return nullptr;
  strncpy( dst, src, len );
  if ( len > 0 ) dst[len-1] = '\0';
  return dst;
}

int qstricmp( const char *s1, const char *s2 )
{
  if ( !s1 || !s2 )
  {
    return s1 == s2 ? 0 : static_cast<int>(s2 - s1);
  }
  int res = 0;
  char c = 0;
  for ( ; !(res = ((c=toLowerChar(*s1)) - toLowerChar(*s2))); s1++, s2++ )
  {
    if ( !c ) // strings are equal
    {
      break;
    }
  }
  return res;
}

int qstrnicmp( const char *s1, const char *s2, size_t len )
{
  if ( !s1 || !s2 )
  {
    return static_cast<int>(s2 - s1);
  }
  for ( ; len--; s1++, s2++ )
  {
    char c = toLowerChar(*s1);
    int res = c-toLowerChar(*s2);
    if ( res!=0 ) // strings are not equal
    {
      return res;
    }
    if ( c==0 ) // strings are equal
    {
      break;
    }
  }
  return 0;
}

/// substitute all occurrences of \a src in \a s by \a dst
QCString substitute(const QCString &s,const QCString &src,const QCString &dst)
{
  if (s.isEmpty() || src.isEmpty()) return s;
  const char *q = nullptr, *p = nullptr;
  size_t srcLen = src.length();
  size_t dstLen = dst.length();
  size_t resLen = 0;
  if (srcLen!=dstLen)
  {
    int count = 0;
    for (p = s.data(); (q=strstr(p,src.data()))!=nullptr; p=q+srcLen) count++;
    resLen = s.length()+count*(dstLen-srcLen);
  }
  else // result has same size as s
  {
    resLen = s.length();
  }
  QCString result(resLen, QCString::ExplicitSize);
  char *r = result.rawData();
  for (p = s.data(); (q=strstr(p,src.data()))!=nullptr; p=q+srcLen)
  {
    int l = static_cast<int>(q-p);
    memcpy(r,p,l);
    r+=l;

    if (dstLen>0) memcpy(r,dst.data(),dstLen);
    r+=dstLen;
  }
  if (r)
  {
    qstrcpy(r,p);
  }
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}


/// substitute all occurrences of \a src in \a s by \a dst, but skip
/// each consecutive sequence of \a src where the number consecutive
/// \a src matches \a skip_seq; if \a skip_seq is negative, skip any
/// number of consecutive \a src
QCString substitute(const QCString &s,const QCString &src,const QCString &dst,int skip_seq)
{
  if (s.isEmpty() || src.isEmpty()) return s;
  const char *p = nullptr, *q = nullptr;
  size_t srcLen = src.length();
  size_t dstLen = dst.length();
  size_t resLen = 0;
  if (srcLen!=dstLen)
  {
    int count = 0;
    for (p=s.data(); (q=strstr(p,src.data()))!=nullptr; p=q+srcLen) count++;
    resLen = s.length()+count*(dstLen-srcLen);
  }
  else // result has same size as s
  {
    resLen = s.length();
  }
  QCString result(resLen, QCString::ExplicitSize);
  char *r = result.rawData();
  for (p = s.data(); (q=strstr(p,src.data()))!=nullptr; p=q+srcLen)
  {
    // search a consecutive sequence of src
    int seq = 0, skip = 0;
    if (skip_seq)
    {
      for (const char *n=q+srcLen; qstrncmp(n,src.data(),srcLen)==0; seq=1+skip, n+=srcLen)
        ++skip; // number of consecutive src after the current one

      // verify the allowed number of consecutive src to skip
      if (skip_seq > 0 && skip_seq != seq)
        seq = skip = 0;
    }

    // skip a consecutive sequence of src when necessary
    int l = static_cast<int>((q + seq * srcLen)-p);
    memcpy(r,p,l);
    r+=l;

    if (skip)
    {
      // skip only the consecutive src found after the current one
      q += skip * srcLen;
      // the next loop will skip the current src, aka (p=q+srcLen)
      continue;
    }

    if (dstLen>0) memcpy(r,dst.data(),dstLen);
    r+=dstLen;
  }
  qstrcpy(r,p);
  result.resize(strlen(result.data()));
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}

QCString QCString::stripLeadingAndTrailingEmptyLines() const
{
  if (isEmpty()) return QCString();
  const std::string &s = m_rep;
  int end=static_cast<int>(s.length());
  int start=0,p=0;
  // skip leading empty lines
  for (;;)
  {
    int c;
    while ((c=s[p]) && (c==' ' || c=='\t')) p++;
    if (s[p]=='\n')
    {
      start=++p;
    }
    else
    {
      break;
    }
  }
  // skip trailing empty lines
  p=end-1;
  if (p>=start && s.at(p)=='\n') p--;
  while (p>=start)
  {
    int c;
    while ((c=s[p]) && (c==' ' || c=='\t')) p--;
    if (s[p]=='\n')
    {
      end=p+1;
    }
    else
    {
      break;
    }
    p--;
  }
  //printf("stripLeadingAndTrailingEmptyLines(%d-%d)\n",start,end);
  return s.substr(start,end-start);
}


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

#include "dstring.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

inline char toLowerChar(char c)
{
  return c>='A' && c<='Z' ? c|0x20 : c;
}

DString &DString::sprintf( const char *format, ... )
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

size_t DString::rfind_insensitive( char c, size_t index) const
{
  const char *b = data();
  const char *pos = nullptr;
  size_t len = length();
  if (len==0) return -1; // empty string
  if (index==npos) // start from end
  {
    index=len;
  }
  else if (index>len) // bad index
  {
    return DString::npos;
  }
  pos = b+index;
  c = toLowerChar(c);
  while ( pos>=b && toLowerChar(*pos)!=c) pos--;
  return pos>=b ? static_cast<size_t>(pos-b) : DString::npos;
}

size_t DString::rfind_insensitive( const char *str, size_t index) const
{
  size_t slen = dstrlen(str);
  size_t len  = length();
  if (slen>len) return DString::npos; // length of search string is longer than this string
  if (index==DString::npos) index = len-slen; // start from end
  else if (index>len) return DString::npos; // bad index
  else if (index+slen>len) index = len-slen; // str would be too long
  const char *pos = data()+index;
  for (size_t i=index+1; i>0; )
  {
    --i;
    if (dstrnicmp(pos--,str,slen)==0) return i;
  }
  return DString::npos;
}

int DString::contains( char c, bool cs ) const
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

int DString::contains( const char *str, bool cs ) const
{
  if (str==nullptr || length()==0) return 0;
  int count=0;
  const char *pos = data();
  int len = dstrlen(str);
  while (*pos)
  {
    if (cs)
    {
      if (dstrncmp(pos,str,len)==0) count++;
    }
    else
    {
      if (dstrnicmp(pos,str,len)==0) count++;
    }
    pos++;
  }
  return count;
}

DString DString::simplifyWhiteSpace() const
{
  if ( empty() )                            // nothing to do
    return *this;

  DString result( length(), ExplicitSize );
  const char *from  = data();
  char *to    = result.rawData();
  char *first = to;
  while ( true )
  {
    while ( *from && disspace(*from) )
      from++;
    while ( *from && !disspace(*from) )
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

DString &DString::replace( size_t index, size_t len, const char *s)
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

short DString::toShort(bool *ok, int base) const
{
  long v = toLong( ok, base );
  if ( ok && *ok && (v < -32768 || v > 32767) ) {
    *ok = false;
    v = 0;
  }
  return static_cast<short>(v);
}

uint16_t DString::toUShort(bool *ok,int base) const
{
  unsigned long v = toULong( ok, base );
  if ( ok && *ok && (v > 65535) ) {
    *ok = false;
    v = 0;
  }
  return static_cast<uint16_t>(v);
}

int DString::toInt(bool *ok, int base) const
{
  return static_cast<int>(toLong( ok, base ));
}

uint32_t DString::toUInt(bool *ok,int base) const
{
  return static_cast<uint32_t>(toULong( ok, base ));
}


long DString::toLong(bool *ok,int base) const
{
  const char *p = data();
  long val=0;
  int l = static_cast<int>(length());
  const long max_mult = INT_MAX / base;
  bool is_ok = false;
  int neg = 0;
  if ( !p )
    goto bye;
  while ( l && disspace(*p) )  // skip leading space
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
  while ( l && disspace(*p) )  // skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = true;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

unsigned long DString::toULong(bool *ok,int base) const
{
  const char *p = data();
  unsigned long val=0;
  int l = static_cast<int>(length());
  const unsigned long max_mult = 429496729;  // UINT_MAX/10, rounded down
  bool is_ok = false;
  if ( !p )
  {
    goto bye;
  }
  while ( l && disspace(*p) )  // skip leading space
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

  while ( l && disspace(*p) )  // skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = true;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

uint64_t DString::toUInt64(bool *ok,int base) const
{
  const char *p = data();
  uint64_t val=0;
  int l = static_cast<int>(length());
  const uint64_t max_mult = 1844674407370955161ULL;  // ULLONG_MAX/10, rounded down
  bool is_ok = false;
  if ( !p )
  {
    goto bye;
  }
  while ( l && disspace(*p) )  // skip leading space
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

  while ( l && disspace(*p) )  // skip trailing space
  {
    l--;
    p++;
  }
  if ( !l )
  {
    is_ok = true;
  }
bye:
  if ( ok )
  {
    *ok = is_ok;
  }
  return is_ok ? val : 0;
}

//-------------------------------------------------

char *dstrdup( const char *str )
{
  if ( !str ) return nullptr;
  char *dst = new char[dstrlen(str)+1];
  return strcpy( dst, str );
}

void dstrfree( const char *str )
{
  delete [](str);
}

char *dstrncpy( char *dst, const char *src, size_t len )
{
  if ( !src ) return nullptr;
  strncpy( dst, src, len );
  if ( len > 0 ) dst[len-1] = '\0';
  return dst;
}

int dstricmp( const char *s1, const char *s2 )
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

int dstrnicmp( const char *s1, const char *s2, size_t len )
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
DString substitute(const DString &s,const DString &src,const DString &dst)
{
  if (s.empty() || src.empty()) return s;
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
  DString result(resLen, DString::ExplicitSize);
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
    dstrcpy(r,p);
  }
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}


/// substitute all occurrences of \a src in \a s by \a dst, but skip
/// each consecutive sequence of \a src where the number consecutive
/// \a src matches \a skip_seq; if \a skip_seq is negative, skip any
/// number of consecutive \a src
DString substitute(const DString &s,const DString &src,const DString &dst,int skip_seq)
{
  if (s.empty() || src.empty()) return s;
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
  DString result(resLen, DString::ExplicitSize);
  char *r = result.rawData();
  for (p = s.data(); (q=strstr(p,src.data()))!=nullptr; p=q+srcLen)
  {
    // search a consecutive sequence of src
    int seq = 0, skip = 0;
    if (skip_seq)
    {
      for (const char *n=q+srcLen; dstrncmp(n,src.data(),srcLen)==0; seq=1+skip, n+=srcLen)
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
  dstrcpy(r,p);
  result.resize(strlen(result.data()));
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}

DString DString::stripLeadingAndTrailingEmptyLines() const
{
  if (empty()) return DString();
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

DString DString::integerToAlpha(int n, bool upper)
{
  DString result;
  int residual = n;

  char modVal[2];
  modVal[1] = 0;
  while (residual > 0)
  {
    modVal[0] = (upper ? 'A': 'a') + (residual-1)%26;
    result = modVal + result;
    residual = (residual-1) / 26;
  }
  return result;
}

DString DString::integerToRoman(int n, bool upper)
{
  static const char *str_romans_upper[] = {  "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
  static const char *str_romans_lower[] = {  "m", "cm", "d", "cd", "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i" };
  static const int values[]             = { 1000,  900, 500,  400, 100,   90,  50,   40,  10,    9,   5,    4,   1 };
  static const char **str_romans = upper ? str_romans_upper : str_romans_lower;

  DString result;
  int residual = n;

  for (int i = 0; i < 13; ++i)
  {
    while (residual - values[i] >= 0)
    {
      result += str_romans[i];
      residual -= values[i];
    }
  }

  return result;
}



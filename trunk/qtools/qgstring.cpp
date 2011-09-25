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

#include <stdio.h>
#include "qgstring.h"

#include <assert.h>

#define BLOCK_SIZE 64
#define ROUND_SIZE(x) ((x)+BLOCK_SIZE-1)&~(BLOCK_SIZE-1)

#define DBG_STR(x) do { } while(0)
//#define DBG_STR(x) printf x

QGString::QGString() // make null string
  : m_data(0), m_len(0), m_memSize(0) 
{
  DBG_STR(("%p: QGString::QGString() %d:%s\n",this,m_len,m_data?m_data:"<none>"));
} 

QGString::QGString(uint size)
{
  if (size==0)
  {
    m_data=0;
    m_len=0;
  }
  else
  {
    m_memSize = ROUND_SIZE(size+1);
    m_data = (char*)malloc(m_memSize);
    memset(m_data,' ',size);
    m_data[size]='\0';
    m_len=size;
  }
  DBG_STR(("%p: QGString::QGString(uint size=%d) %d:%s\n",
      this,size,m_len,m_data?m_data:"<none>"));
}

QGString::QGString( const QGString &s ) 
{ 
  if (s.m_memSize==0)
  {
    m_data    = 0;
    m_len     = 0;
    m_memSize = 0;
  }
  else
  {
    m_data    = (char *)malloc(s.m_memSize); 
    m_len     = s.m_len;
    m_memSize = s.m_memSize;
    qstrcpy(m_data,s.m_data);
  }
  DBG_STR(("%p: QGString::QGString(const QGString &) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
} 

QGString::QGString( const char *str )
{
  if (str==0)
  {
    m_data=0;
    m_len=0;
    m_memSize=0;
  }
  else
  {
    m_len = qstrlen(str);
    m_memSize = ROUND_SIZE(m_len+1);
    assert(m_memSize>=m_len+1);
    m_data = (char *)malloc(m_memSize);
    qstrcpy(m_data,str);
  }
  DBG_STR(("%p: QGString::QGString(const char *) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
}

QGString::~QGString() 
{ 
  free(m_data); 
  m_data=0; 
  DBG_STR(("%p: QGString::~QGString() %d:%s\n",this,m_len,m_data?m_data:"<none>"));
}

bool QGString::resize( uint newlen )
{
  m_len = 0;
  if (newlen==0)
  {
    if (m_data) { free(m_data); m_data=0; }
    m_memSize=0;
    DBG_STR(("%p: 1.QGString::resize() %d:%s\n",this,m_len,m_data?m_data:"<none>"));
    return TRUE;
  }
  m_memSize = ROUND_SIZE(newlen+1);
  assert(m_memSize>=newlen+1);
  if (m_data==0)
  {
    m_data = (char *)malloc(m_memSize);
  }
  else
  {
    m_data = (char *)realloc(m_data,m_memSize);
  }
  if (m_data==0) 
  {
    DBG_STR(("%p: 2.QGString::resize() %d:%s\n",this,m_len,m_data?m_data:"<none>"));
    return FALSE;
  }
  m_data[newlen]='\0';
  m_len = qstrlen(m_data);
  DBG_STR(("%p: 3.QGString::resize() %d:%s\n",this,m_len,m_data?m_data:"<none>"));
  return TRUE;
}

bool QGString::enlarge( uint newlen )
{
  if (newlen==0)
  {
    if (m_data) { free(m_data); m_data=0; }
    m_memSize=0;
    m_len=0;
    return TRUE;
  }
  uint newMemSize = ROUND_SIZE(newlen+1);
  if (newMemSize==m_memSize) return TRUE;
  m_memSize = newMemSize;
  if (m_data==0)
  {
    m_data = (char *)malloc(m_memSize);
  }
  else
  {
    m_data = (char *)realloc(m_data,m_memSize);
  }
  if (m_data==0) 
  {
    return FALSE;
  }
  m_data[newlen-1]='\0';
  if (m_len>newlen) m_len=newlen;
  return TRUE;
}

void QGString::setLen( uint newlen )
{
  m_len = newlen<=m_memSize ? newlen : m_memSize;
}

QGString &QGString::operator=( const QGString &s ) 
{
  if (m_data) free(m_data);
  if (s.m_memSize==0) // null string
  {
    m_data    = 0;
    m_len     = 0;
    m_memSize = 0;
  }
  else
  {
    m_len     = s.m_len;
    m_memSize = s.m_memSize;
    m_data    = (char*)malloc(m_memSize);
    qstrcpy(m_data,s.m_data);
  }
  DBG_STR(("%p: QGString::operator=(const QGString &%p) %d:%s\n",
      this,&s,m_len,m_data?m_data:"<none>"));
  return *this;
}

QGString &QGString::operator=( const char *str ) 
{ 
  if (m_data) free(m_data);
  if (str==0) // null string
  {
    m_data    = 0;
    m_len     = 0;
    m_memSize = 0;
  }
  else
  {
    m_len     = qstrlen(str);
    m_memSize = ROUND_SIZE(m_len+1);
    assert(m_memSize>=m_len+1);
    m_data    = (char*)malloc(m_memSize);
    qstrcpy(m_data,str);
  }
  DBG_STR(("%p: QGString::operator=(const char *) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
  return *this;
}

QGString &QGString::operator+=( const QGString &s )
{
  if (s.m_memSize==0) return *this;
  uint len1 = length();
  uint len2 = s.length();
  uint memSize = ROUND_SIZE(len1 + len2 + 1);
  assert(memSize>=len1+len2+1);
  char *newData = memSize!=m_memSize ? (char*)realloc( m_data, memSize ) : m_data;
  m_memSize = memSize;
  if (m_data)
  {
    m_data = newData;
    memcpy( m_data + len1, s, len2 + 1 );
  }
  m_len = len1+len2;
  DBG_STR(("%p: QGString::operator+=(const QGString &) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
  return *this;
}

QGString &QGString::operator+=( const char *str )
{
  if (!str) return *this;
  uint len1 = length();
  uint len2 = qstrlen(str);
  uint memSize = ROUND_SIZE(len1 + len2 + 1);
  assert(memSize>=len1+len2+1);
  char *newData = memSize!=m_memSize ? (char *)realloc( m_data, memSize ) : m_data;
  m_memSize = memSize;
  if (newData)
  {
    m_data = newData;
    memcpy( m_data + len1, str, len2 + 1 );
  }
  m_len+=len2;
  DBG_STR(("%p: QGString::operator+=(const char *) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
  return *this;
}

QGString &QGString::operator+=( char c )
{
  uint len = m_len;
  uint memSize = ROUND_SIZE(len+2);
  assert(memSize>=len+2);
  char *newData = memSize!=m_memSize ? (char *)realloc( m_data, memSize ) : m_data;
  m_memSize = memSize;
  if (newData)
  {
    m_data = newData;
    m_data[len] = c;
    m_data[len+1] = '\0';
  }
  m_len++;
  DBG_STR(("%p: QGString::operator+=(char s) %d:%s\n",this,m_len,m_data?m_data:"<none>"));
  return *this;
}


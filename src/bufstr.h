/******************************************************************************
 *
 *
 *
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
#ifndef BUFSTR_H
#define BUFSTR_H

#include <cstdlib>
#include "qcstring.h"

/*! @brief Buffer used to store strings
 *
 *  This buffer is used append characters and strings. It will automatically
 *  resize itself, yet provide efficient random access to the content.
 */
class BufStr
{
  public:
    BufStr(size_t size)
      : m_size(size), m_writeOffset(0), m_spareRoom(10240), m_buf(0)
    {
      m_buf = static_cast<char *>(calloc(size,1));
    }
    ~BufStr()
    {
      free(m_buf);
    }
    void addChar(char c)
    {
      makeRoomFor(1);
      m_buf[m_writeOffset++]=c;
    }
    void addArray(const char *a,size_t len)
    {
      makeRoomFor(len);
      memcpy(m_buf+m_writeOffset,a,len);
      m_writeOffset+=len;
    }
    void skip(size_t s)
    {
      makeRoomFor(s);
      m_writeOffset+=s;
    }
    void shrink( size_t newlen )
    {
      m_writeOffset=newlen;
      resize(newlen);
    }
    void resize( size_t newlen )
    {
      size_t oldsize = m_size;
      m_size=newlen;
      if (m_writeOffset>=m_size) // offset out of range -> enlarge
      {
        m_size=m_writeOffset+m_spareRoom;
      }
      m_buf = static_cast<char *>(realloc(m_buf,m_size));
      if (m_size>oldsize)
      {
        memset(m_buf+oldsize,0,m_size-oldsize);
      }
    }
    size_t size() const
    {
      return m_size;
    }
    char *data() const
    {
      return m_buf;
    }
    char &at(size_t i) const
    {
      return m_buf[i];
    }
    bool isEmpty() const
    {
      return m_writeOffset==0;
    }
    operator const char *() const
    {
      return m_buf;
    }
    size_t curPos() const
    {
      return m_writeOffset;
    }
    void dropFromStart(size_t bytes)
    {
      if (bytes>m_size) bytes=m_size;
      if (bytes>0) qmemmove(m_buf,m_buf+bytes,m_size-bytes);
      m_size-=bytes;
      m_writeOffset-=bytes;
    }
    void addTerminalCharIfMissing(char c)
    {
      if (m_buf && m_writeOffset>1 && m_buf[m_writeOffset-2]!=c && m_buf[m_writeOffset-1]=='\0')
      {
        // add missing terminal character and 0 terminator
        m_buf[m_writeOffset-1] = '\n';
        addChar('\0');
      }
      else if (m_buf && m_writeOffset>0 && m_buf[m_writeOffset-1]!='\0' && m_buf[m_writeOffset-1]!=c)
      {
        // add missing terminal character without 0 terminator
        addChar(c);
      }
    }
  private:
    void makeRoomFor(size_t size)
    {
      if (m_writeOffset+size>=m_size)
      {
        resize(m_size+size+m_spareRoom);
      }
    }
    size_t m_size;
    size_t m_writeOffset;
    const size_t m_spareRoom; // 10Kb extra room to avoid frequent resizing
    char *m_buf;
};


#endif

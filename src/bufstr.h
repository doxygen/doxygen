/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#ifndef _BUFSTR_H
#define _BUFSTR_H

#include <qglobal.h>
#include <qcstring.h>
#include <stdlib.h>

/*! @brief Buffer used to store strings
 *  
 *  This buffer is used append characters and strings. It will automatically
 *  resize itself, yet provide efficient random access to the content.
 */
class BufStr 
{
  public:
    BufStr(int size) 
      : m_size(size), m_writeOffset(0), m_spareRoom(10240), m_buf(0) 
    {
      m_buf = (char *)malloc(size);
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
    void addArray(const char *a,int len)
    {
      makeRoomFor(len);
      memcpy(m_buf+m_writeOffset,a,len);
      m_writeOffset+=len;
    }
    void skip(uint s)
    {
      makeRoomFor(s);
      m_writeOffset+=s;
    }
    void shrink( uint newlen )
    {
      m_writeOffset=newlen;
      resize(newlen);
    }
    void resize( uint newlen )
    {
      m_size=newlen;
      if (m_writeOffset>=m_size) // offset out of range -> enlarge
      {
        m_size=m_writeOffset+m_spareRoom;
      }
      m_buf = (char *)realloc(m_buf,m_size);
    }
    int size() const
    {
      return m_size;
    }
    char *data() const
    {
      return m_buf;
    }
    char &at(uint i) const
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
    uint curPos() const
    { 
      return m_writeOffset; 
    }
    void dropFromStart(uint bytes)
    {
      if (bytes>m_size) bytes=m_size;
      if (bytes>0) qmemmove(m_buf,m_buf+bytes,m_size-bytes);
      m_size-=bytes;
      m_writeOffset-=bytes;
    }
  private:
    void makeRoomFor(uint size)
    {
      if (m_writeOffset+size>=m_size) 
      {
        resize(m_size+size+m_spareRoom);
      }
    }
    uint m_size;
    uint m_writeOffset;
    const int m_spareRoom; // 10Kb extra room to avoid frequent resizing
    char *m_buf;
};


#endif

/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include "qtbc.h"

/*! String that can deal more efficiently with large large numbers
 *  of resizing. 
 */
class BufStr : public QCString
{
  public:
    BufStr(int size) : QCString(size), offset(0), spareRoom(10240) {}
    void addChar(char c)
    {
      if (offset>=size()) 
      {
        QCString::resize(size()+spareRoom);
      }
      data()[offset++]=c;
    }
    void addArray(const char *a,int len)
    {
      if (offset+len>=size()) 
      {
        QCString::resize(size()+len+spareRoom);
      }
      memcpy(data()+offset,a,len);
      offset+=len;
    }
    uint curPos() { return offset; }
    void skip(uint s)
    {
      if (offset+s>=size()) 
      {
        QCString::resize(size()+s+spareRoom);
      }
      offset+=s;
    }
    void resize( uint newlen )
    {
      //QCString::resize(newlen);
      //if (offset>newlen)
      //{
      //  offset=newlen;
      //}
      offset=newlen;
      if (offset>=size()) 
      {
        QCString::resize(size()+spareRoom);
      }
    }
  private:
    uint offset;
    const int spareRoom; // 10Kb extra room to avoid frequent resizing
};

#endif

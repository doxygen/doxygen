/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef GROWBUF_H
#define GROWBUF_H

#include <utility>
#include <stdlib.h>
#include <string.h>
#include <string>

#define GROW_AMOUNT 1024*4

/** Class representing a string buffer optimized for growing. */
class GrowBuf
{
  public:
    GrowBuf() : m_str(nullptr), m_pos(0), m_len(0) {}
    GrowBuf(size_t initialSize) : m_pos(0), m_len(initialSize) { m_str=static_cast<char*>(malloc(m_len)); }
   ~GrowBuf()         { free(m_str); }
    GrowBuf(const GrowBuf &other)
    {
      m_len = other.m_len;
      m_pos = other.m_pos;
      m_str = static_cast<char*>(malloc(m_len));
      memcpy(m_str,other.m_str,m_len);
    }
    GrowBuf &operator=(const GrowBuf &other)
    {
      if (this!=&other)
      {
        free(m_str);
        m_len = other.m_len;
        m_pos = other.m_pos;
        m_str = static_cast<char*>(malloc(m_len));
        memcpy(m_str,other.m_str,m_len);
      }
      return *this;
    }
    GrowBuf(GrowBuf &&other)
      : m_str(std::exchange(other.m_str,static_cast<char*>(nullptr)))
      , m_pos(std::exchange(other.m_pos,0))
      , m_len(std::exchange(other.m_len,0))
    {
    }
    GrowBuf &operator=(GrowBuf &&other)
    {
      if (this==&other)
        return *this;
      m_len = std::exchange(other.m_len,0);
      m_pos = std::exchange(other.m_pos,0);
      m_str = std::exchange(other.m_str,static_cast<char*>(nullptr));
      return *this;
    }
    void reserve(size_t size) { if (m_len<size) { m_len = size; m_str = static_cast<char*>(realloc(m_str,m_len)); } }
    void clear()      { m_pos=0; }
    void addChar(char c)  { if (m_pos>=m_len) { m_len+=GROW_AMOUNT; m_str = static_cast<char*>(realloc(m_str,m_len)); }
                        m_str[m_pos++]=c;
                      }
    void addStr(const QCString &s) {
                        if (!s.isEmpty())
                        {
                          size_t l=s.length();
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = static_cast<char*>(realloc(m_str,m_len)); }
                          strcpy(&m_str[m_pos],s.data());
                          m_pos+=l;
                        }
                      }
    void addStr(const std::string &s) {
                        if (!s.empty())
                        {
                          size_t l=s.length();
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = static_cast<char*>(realloc(m_str,m_len)); }
                          strcpy(&m_str[m_pos],s.c_str());
                          m_pos+=l;
                        }
                      }
    void addStr(const char *s) {
                        if (s)
                        {
                          size_t l=strlen(s);
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = static_cast<char*>(realloc(m_str,m_len)); }
                          strcpy(&m_str[m_pos],s);
                          m_pos+=l;
                        }
                      }
    void addStr(const char *s,size_t n) {
                        if (s)
                        {
                          size_t l=strlen(s);
                          if (n<l) l=n;
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = static_cast<char*>(realloc(m_str,m_len)); }
                          strncpy(&m_str[m_pos],s,n);
                          m_pos+=l;
                        }
                      }
    void addInt(const char *fmt,int value) {
                      char tmp[50];
                      snprintf(tmp,50,fmt,value);
                      addStr(tmp);
    }
    char *get() { return m_str; }
    const char *get() const { return m_str; }
    size_t getPos() const   { return m_pos; }
    void setPos(size_t newPos) { m_pos = newPos; }
    const char &at(size_t i) const { return m_str[i]; }
    bool empty() const { return m_pos==0; }
  private:
    char *m_str;
    size_t m_pos;
    size_t m_len;
};

#endif

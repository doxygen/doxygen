#ifndef GROWBUF_H
#define GROWBUF_H

#include <utility>
#include <stdlib.h>
#include <string.h>
#include <string>

#define GROW_AMOUNT 1024*4

/** Class representing a string buffer optimised for growing. */
class GrowBuf
{
  public:
    GrowBuf() : m_str(0), m_pos(0), m_len(0) {}
    GrowBuf(uint initialSize) : m_pos(0), m_len(initialSize) { m_str=(char*)malloc(m_len); }
   ~GrowBuf()         { free(m_str); }
    GrowBuf(const GrowBuf &other)
    {
      m_len = other.m_len;
      m_pos = other.m_pos;
      m_str = (char*)malloc(m_len);
      memcpy(m_str,other.m_str,m_len);
    }
    GrowBuf &operator=(const GrowBuf &other)
    {
      if (this!=&other)
      {
        free(m_str);
        m_len = other.m_len;
        m_pos = other.m_pos;
        m_str = (char*)malloc(m_len);
        memcpy(m_str,other.m_str,m_len);
      }
      return *this;
    }
    GrowBuf(GrowBuf &&other)
      : m_str(std::exchange(other.m_str,(char*)0))
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
      m_str = std::exchange(other.m_str,(char*)0);
      return *this;
    }
    void reserve(uint size) { if (m_len<size) { m_len = size; m_str = (char*)realloc(m_str,m_len); } }
    void clear()      { m_pos=0; }
    void addChar(char c)  { if (m_pos>=m_len) { m_len+=GROW_AMOUNT; m_str = (char*)realloc(m_str,m_len); }
                        m_str[m_pos++]=c;
                      }
    void addStr(const QCString &s) {
                        if (!s.isEmpty())
                        {
                          uint l=s.length();
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = (char*)realloc(m_str,m_len); }
                          strcpy(&m_str[m_pos],s.data());
                          m_pos+=l;
                        }
                      }
    void addStr(const std::string &s) {
                        if (!s.empty())
                        {
                          uint l=(uint)s.length();
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = (char*)realloc(m_str,m_len); }
                          strcpy(&m_str[m_pos],s.c_str());
                          m_pos+=l;
                        }
                      }
    void addStr(const char *s) {
                        if (s)
                        {
                          uint l=(uint)strlen(s);
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = (char*)realloc(m_str,m_len); }
                          strcpy(&m_str[m_pos],s);
                          m_pos+=l;
                        }
                      }
    void addStr(const char *s,uint n) {
                        if (s)
                        {
                          uint l=(uint)strlen(s);
                          if (n<l) l=n;
                          if (m_pos+l>=m_len) { m_len+=l+GROW_AMOUNT; m_str = (char*)realloc(m_str,m_len); }
                          strncpy(&m_str[m_pos],s,n);
                          m_pos+=l;
                        }
                      }
    char *get() { return m_str; }
    const char *get() const { return m_str; }
    uint getPos() const   { return m_pos; }
    void setPos(uint newPos) { m_pos = newPos; }
    char at(uint i) const { return m_str[i]; }
    bool empty() const { return m_pos==0; }
  private:
    char *m_str;
    uint m_pos;
    uint m_len;
};

#endif

#ifndef VHDLSTRING_H
#define VHDLSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VhdlParser.h"
// super class for VhdlParserTokenManager
// is generated in vhdlparser.jj
// -option TOKEN_MANAGER_SUPER_CLASS = "TokenParser"
// sets the Vhdlparser in vhdljjparser.cpp
// tokenManager->setLexParser(vhdlParser);

namespace vhdl {
  namespace parser {
     class TokenParser {
    public:
      VhdlParser* parser = nullptr;
      void   setLexParser(VhdlParser* p)
      {
        parser = p;
      }
    };
  }
}


/** @brief Minimal string class with std::string like behavior that fulfills the JavaCC
 *  string requirements.
 */

class VhdlString
{
  public:
    VhdlString()
    {
      init();
    }
    VhdlString(const VhdlString &other)
    {
      m_str = (char*)malloc(other.m_len+1);
      memcpy(m_str,other.m_str,other.m_len);
      m_len = other.m_len;
      m_str[m_len]=0;
    }
    VhdlString &operator=(const VhdlString &other)
    {
      if (this!=&other)
      {
        free(m_str);
        m_str = (char*)malloc(other.m_len+1);
        memcpy(m_str,other.m_str,other.m_len);
        m_len = other.m_len;
        m_str[m_len]=0;
      }
      return *this;
    }
    VhdlString(const char *s)
    {
      m_len = (int)strlen(s);
      m_str=(char*)malloc(m_len+1);
      memcpy(m_str,s,m_len+1);
    }
    VhdlString(const char *s,int size)
    {
      m_str = (char*)malloc(size+1);
      memcpy(m_str,s,size);
      m_str[size]=0;
      m_len=size;
    }
   ~VhdlString()
    {
      free(m_str);
    }
    VhdlString& append(const char *s,int size)
    {
      int oldlen = m_len;
      m_len+=size+1;
      if (m_len)
      {
        m_str = (char*)realloc(m_str,m_len);
        memcpy(m_str+oldlen,s,m_len-oldlen-1);
        m_str[m_len-1]=0;
      }
      return *this;
    }
    VhdlString& append(const char *s)
    {
      return append(s,(int)strlen(s));
    }
    VhdlString& append(const VhdlString &other)
    {
      return append(other.m_str,other.m_len);
    }
    VhdlString substr(int pos=0,int len=-1)
    {
      return VhdlString(m_str ? m_str+pos : nullptr, len==-1 ? m_len-pos : m_len);
    }
    int copy(char *s,int len,int pos=0) const
    {
      if (len==0) return 0;
      if (pos>=m_len) { s[0]=0; return 0; }
      int r=m_len<pos+len ? m_len-pos : len;
      memcpy(s,m_str+pos,r);
      return r;
    }
    const char *c_str() const           { return m_str; }
    const char *data() const            { return m_str; }
    int         size() const            { return m_len; }
    int         length() const          { return m_len; }
    char &      operator[](int i)       { return m_str[i]; }
    const char &operator[](int i) const { return m_str[i]; }
    void        clear()                 { free(m_str); init(); }
    VhdlString  &operator+=(char c)      { char s[2]; s[0]=c; s[1]=0; return append(s); }
    VhdlString  &operator+=(const char *s) { return append(s); }
    VhdlString  &operator+=(VhdlString s) { return append(s); }
    VhdlString   operator+ (const char *s) { return append(s); }

  private:
    void init() { m_str=(char*)calloc(1,1); m_len=0; }
    char *m_str;
    int   m_len;
};

inline VhdlString   operator+ (const char *s, VhdlString v) { return VhdlString(s).append(v); }

#endif

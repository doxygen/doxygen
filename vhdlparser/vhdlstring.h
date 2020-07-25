

/** 
 *  @brief Minimal string class  that fulfills the JavaCC
 *  string requirements for unicode .
 */


#ifndef VHDLSTRING
#define  VHDLSTRING

#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <codecvt>
#include <cwchar>
#include <iostream>
#include <ostream>
#include <algorithm>
#include "message.h"

using namespace std;

class VhdlString
{
 
  public:
 
    VhdlString(){}
    
    VhdlString(const VhdlString &other) {  m_wcs.append(other.c_str(),other.size()); }

    VhdlString(const wchar_t *s,int size) { m_wcs.append(s,size); }
    
    VhdlString(const wchar_t *s) { m_wcs.append(s);}

    VhdlString(const char *s){ charToWChar(s); }

    ~VhdlString(){}

    VhdlString &operator=(const VhdlString &other)
    {
      clear();
      m_wcs.append(other.c_str());
      return *this;
    }

	VhdlString &operator=(const char* s)
	{
     clear();
     charToWChar(s);
     return *this;
	}

   VhdlString& prepend(const char *s )
   {
     wstring tmp(m_wcs);
     m_wcs.clear();
     charToWChar(s);
     m_wcs += tmp;
	 return *this;
   }

    VhdlString& append(const wchar_t *s,int size){ m_wcs.append(s, size);  return *this;}

    VhdlString& append(const wchar_t *s) {  return append(s,wcslen(s)); }

    VhdlString& append(const VhdlString &other) { m_wcs.append(other.c_str()); return *this;}

	VhdlString substr(int pos=0,int len=-1)
    {
     wstring w = m_wcs.substr(pos, len);
     return VhdlString(w.c_str());
    }    

    int copy(wchar_t *s,int len,int pos=0) const { return m_wcs.copy(s, len, pos); }

    bool contains(const char * s)
    {
	   VhdlString tmp(s);
	   string::size_type found = m_wcs.find(tmp.c_str()); 
	   return (found != string::npos);
    }

   VhdlString lower() const
   {
    wstring tmp(c_str());
    transform(tmp.begin(), tmp.end(), tmp.begin(),::tolower);
    return VhdlString(tmp.data());
   }

   VhdlString upper() const
   {
	wstring tmp(c_str());
    transform(tmp.begin(), tmp.end(), tmp.begin(),::toupper);
    return VhdlString(tmp.data());
  }
  
   const char* data()
   {
     m_str.clear();
     wcharToChar(m_str);
     return m_str.c_str();
   }

    const wchar_t* c_str() const { return m_wcs.c_str(); }
    size_t         size() const            { return m_wcs.size(); }
    size_t         length() const          { return   m_wcs.size(); }
    void clear() { m_wcs.clear(); m_str.clear();}
    wchar_t &      operator[](int i)       { return m_wcs.at(i); }
    const wchar_t &operator[](int i) const { return m_wcs.at(i); }
	  bool isEmpty(){ return size()==0; }
   
    VhdlString  &operator+=(wchar_t c)     { wchar_t s[2]; s[0]=c; s[1]=0; return append(s); }  
    VhdlString  &operator+=(const wchar_t *s) { return append(s); }
    VhdlString  &operator+=(const VhdlString &s) { return append(s.c_str()); }
    VhdlString  &operator+=(const char *s) { return append(s);}

    // returns an error , if the  string contains invalid multibyte characters
    int charToWChar(const char *str) 
    {
     if(!str) return 0;
	   mbstate_t state = mbstate_t();
     int len = 1 + mbsrtowcs(NULL, &str, 0, &state);
    
     //mbsrtowcs sets errno = EILSEQ  if an error occurs during conversion 
     if (errno == EILSEQ)
     {
       errno = 0;
       return EILSEQ;
     }
     
     wchar_t *p=(wchar_t*)malloc(len*sizeof(wchar_t));
     mbsrtowcs(p, &str, len, &state);
	   m_wcs.append(p);
	   free(p);
	 return 0;
	}
     
    void wcharToChar(string & wc) 
    {
     if(m_wcs.size()==0) return;
     mbstate_t state = mbstate_t();
     const wchar_t *s=m_wcs.c_str();
     size_t len = 1 +  wcsrtombs(nullptr, &s, 0, &state);
     char *p=(char*)malloc(len*sizeof(char));
     wcsrtombs(p, &s, len, &state);
     wc.append(p);
     free(p);
    }
   
    private:   
      wstring m_wcs;
      string m_str;
 
};

inline VhdlString operator+(const VhdlString& s1, const VhdlString& s2) {
  VhdlString tmp(s1);
  tmp += s2;
  return tmp;
}

inline VhdlString operator+(const VhdlString& s1, const wchar_t& s2) {
  VhdlString tmp(s1);
  tmp += s2;
  return tmp;
}



inline VhdlString operator+(const VhdlString& s1, const char* s2) {
  VhdlString tmp(s1);
  tmp += s2;
  return tmp;
}

inline VhdlString operator+(const char* s1, const VhdlString& s2) {
  VhdlString tmp(s1);
  tmp += s2;
  return tmp;
}
 


#endif

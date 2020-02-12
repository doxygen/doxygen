#ifndef GROWBUF_H
#define GROWBUF_H

#include <stdlib.h>
#include <string.h>

#define GROW_AMOUNT 1024

/** Class representing a string buffer optimised for growing. */
class GrowBuf
{
  public:
    GrowBuf() : str(0), pos(0), len(0) {}
    GrowBuf(int initialSize) : pos(0), len(initialSize) { str=(char*)malloc(len); }
   ~GrowBuf()         { free(str); str=0; pos=0; len=0; }
    void clear()      { pos=0; }
    void addChar(char c)  { if (pos>=len) { len+=GROW_AMOUNT; str = (char*)realloc(str,len); } 
                        str[pos++]=c; 
                      }
    void addStr(const QCString &s) {
                        if (!s.isEmpty())
                        {
                          int l=s.length();
                          if (pos+l>=len) { len+=l+GROW_AMOUNT; str = (char*)realloc(str,len); }
                          strcpy(&str[pos],s.data());
                          pos+=l;
                        }
                      }
    void addStr(const char *s) {
                        if (s)
                        {
                          int l=(int)strlen(s);
                          if (pos+l>=len) { len+=l+GROW_AMOUNT; str = (char*)realloc(str,len); }
                          strcpy(&str[pos],s);
                          pos+=l;
                        }
                      }
    void addStr(const char *s,int n) {
                        if (s)
                        {
                          int l=(int)strlen(s);
                          if (n<l) l=n;
                          if (pos+l>=len) { len+=l+GROW_AMOUNT; str = (char*)realloc(str,len); }
                          strncpy(&str[pos],s,n);
                          pos+=l;
                        }
                      }
    const char *get()     { return str; }
    int getPos() const    { return pos; }
    void setPos(const int newPos) { pos = newPos; }
    char at(int i) const  { return str[i]; }
  private:
    char *str;
    int pos;
    int len;
};

#endif

#ifndef GROWBUF_H
#define GROWBUF_H

#include <stdlib.h>
#include <string.h>

/** Class representing a string buffer optimised for growing. */
class GrowBuf
{
  public:
    GrowBuf() : str(0), pos(0), len(0) {}
   ~GrowBuf()         { free(str); str=0; pos=0; len=0; }
    void clear()      { pos=0; }
    void addChar(char c)  { if (pos>=len) { len+=1024; str = (char*)realloc(str,len); } 
                        str[pos++]=c; 
                      }
    void addStr(const char *s) {
                        int l=strlen(s);
                        if (pos+l>=len) { len+=l+1024; str = (char*)realloc(str,len); }
                        strcpy(&str[pos],s);
                        pos+=l;
                      }
    void addStr(const char *s,int n) {
                        int l=strlen(s);
                        if (n<l) l=n;
                        if (pos+l>=len) { len+=l+1024; str = (char*)realloc(str,len); }
                        strncpy(&str[pos],s,n);
                        pos+=l;
                      }
    const char *get()     { return str; }
    int getPos() const    { return pos; }
    char at(int i) const  { return str[i]; }
  private:
    char *str;
    int pos;
    int len;
};

#endif

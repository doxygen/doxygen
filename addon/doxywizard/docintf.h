#ifndef DOCINTF_H
#define DOCINTF_H

class DocIntf
{
  public:
    virtual ~DocIntf() {}
    virtual void setHeader(const char *header) = 0;
    virtual void add(const char *name,const char *docs) = 0;
};

#endif

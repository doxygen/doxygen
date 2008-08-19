#ifndef STRINGIMPL_H
#define STRINGIMPL_H

#include <qstring.h>
#include "doxmlintf.h"

class StringImpl : public QString, public IString
{
  public:
    StringImpl() {}
    StringImpl(const QString &str) : QString(str) {}
    StringImpl &operator=(const QString &str) 
    { QString::operator=(str); return *this; }
    virtual ~StringImpl() {}

    // IString 
    const char *latin1() const 
    { return QString::latin1(); }
    const char *utf8() const 
    { return QString::utf8(); }
    unsigned short unicodeCharAt(int index) const 
    { return QString::unicode()[index].unicode(); } 
    bool isEmpty() const 
    { return QString::isEmpty(); }
    int length() const 
    { return QString::length(); }
};

#endif


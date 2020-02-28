#ifndef STRINGIMPL_H
#define STRINGIMPL_H

#include <qstring.h>
#include "doxmlintf.h"

class StringImpl : public IString
{
  public:
    StringImpl() {}
    StringImpl(const QString &str) : m_str(str) {}
    StringImpl &operator=(const QString &str) 
    { m_str=str; return *this; }
    virtual ~StringImpl() {}
    const char *data() const 
    { return m_str.data(); }

    // IString 
    const char *latin1() const 
    { return m_str.latin1(); }
    const char *utf8() const 
    { m_cstr = m_str.utf8(); return m_cstr.data(); }
    unsigned short unicodeCharAt(int index) const 
    { return m_str.unicode()[index].unicode(); } 
    bool isEmpty() const 
    { return m_str.isEmpty(); }
    int length() const 
    { return m_str.length(); }

    operator QString() const { return m_str; }

  private:
    QString m_str;
    mutable QCString m_cstr; // used as a cache for m_str.utf8() to avoid returning a temporary
};

#endif


#ifndef _INPUT_H
#define _INPUT_H

class QObject;

class IInput
{
  public:
    virtual ~IInput() {}
    virtual void init() = 0;
    virtual void setEnabled(bool) = 0;
    virtual QObject *qobject() = 0;
};

#endif

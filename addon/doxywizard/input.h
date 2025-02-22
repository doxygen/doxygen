/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef INPUT_H
#define INPUT_H

#include <QVariant>
#include "adapter.h"

class QTextStream;
class QTextStream;

class Input
{
  public:
    enum Kind
    {
      Bool,
      Int,
      String,
      StrList,
      Obsolete
    };
    virtual ~Input() {}
    virtual QVariant &value() = 0;
    virtual void update() = 0;
    virtual Kind kind() const = 0;
    virtual QString docs() const = 0;
    virtual QString id() const = 0;
    virtual QString templateDocs() const = 0;
    virtual void addDependency(Input *option) = 0;
    virtual void setEnabled(bool) = 0;
    virtual void updateDependencies() = 0;
    virtual void reset() = 0;
    virtual bool isDefault() = 0;
    virtual void writeValue(QTextStream &t,TextCodecAdapter *codec,bool convert) = 0;
    virtual void setTemplateDocs(const QString &docs) = 0;
    virtual bool isEmpty() { return false; };
};


#endif

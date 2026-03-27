/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef INPUTOBSOLETE_H
#define INPUTOBSOLETE_H

#include "input.h"

class InputObsolete : public Input
{
  public:
    InputObsolete(const QString &id, Kind orgKind) : m_id(id), m_orgKind(orgKind) {}
    QVariant &value()            { return m_value; }
    void update()                {}
    Kind kind() const            { return Obsolete; }
    QString docs() const         { return QString(); }
    QString id() const           { return m_id; }
    QString templateDocs() const { return QString(); }
    void addDependency(Input *)  {}
    void setEnabled(bool)        {}
    void updateDependencies()    {}
    void reset()                 {}
    bool isDefault()             { return false; }
    void writeValue(QTextStream &,TextCodecAdapter *,bool) {}
    void setTemplateDocs(const QString &) {}
    bool isEmpty()               { return false; };
    Kind orgKind() const         { return m_orgKind; }
  private:
    QString   m_id;
    Kind      m_orgKind;
    QVariant  m_value;
};

#endif


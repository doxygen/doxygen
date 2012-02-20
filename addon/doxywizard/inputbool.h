/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _INPUTBOOL_H
#define _INPUTBOOL_H

#include "input.h"
#include <QObject>

class QCheckBox;
class QGridLayout;
class QLabel;

class InputBool : public QObject, public Input
{
  Q_OBJECT

  public:
    InputBool(QGridLayout *layout,int &row,const QString &id, 
              bool enabled, const QString &docs );

    // Input
    QVariant &value();
    void update();
    Kind kind() const { return Bool; }
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    void addDependency(Input *option) { m_dependencies+=option; }
    void setEnabled(bool);
    void updateDependencies();
    void writeValue(QTextStream &t,QTextCodec *codec);

  public slots:
    void reset();
    void setValue(bool);

  signals:
    void changed();
    void toggle(QString,bool);
    void showHelp(Input *);

  private slots:
    void help();

  private:
    void updateDefault();
    bool m_state;
    bool m_default;
    bool m_enabled;
    QVariant m_value;
    QCheckBox *m_cb;
    QString m_docs;
    QList<Input*> m_dependencies;
    QString m_id;
    QLabel *m_lab;

};

#endif

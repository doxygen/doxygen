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

#ifndef _INPUTINT_H
#define _INPUTINT_H

#include "input.h"
#include <QObject>

class QGridLayout;
class QLabel;
class QSpinBox;

class InputInt : public QObject, public Input
{
  Q_OBJECT

  public:
    InputInt( QGridLayout *layout,int &row,
              const QString &id, int defVal, 
              int minVal, int maxVal,
              const QString &docs );
    ~InputInt(){};

    // Input
    QVariant &value();
    void update();
    Kind kind() const { return Int; }
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    void addDependency(Input *) { Q_ASSERT(false); }
    void setEnabled(bool);
    void updateDependencies() {}
    void writeValue(QTextStream &t,QTextCodec *codec);

  public slots:
    void reset();
    void setValue(int val); 

  private slots:
    void help();

  signals:
    void changed();
    void showHelp(Input *);

  private:
    QLabel   *m_lab;
    QSpinBox *m_sp;
    int       m_val;
    int       m_default;
    int       m_minVal;
    int       m_maxVal;
    QVariant  m_value;
    QString   m_docs;
    QString   m_id;
};

#endif

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _INPUTSTRING_H
#define _INPUTSTRING_H

#include <qwidget.h>
#include <qstring.h>
#include <qdict.h>

#include "qtbc.h"
#include "input.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class InputString : public QWidget, public IInput
{
  Q_OBJECT

  public:
    enum StringMode { StringFree=0, 
                      StringFile=1, 
                      StringDir=2, 
                      StringFixed=3
                    };

    InputString( const QString &text, QWidget *parent, QCString &s, 
                 StringMode m=StringFree );
    ~InputString();
    void setEnabled(bool);
    void addValue(const char *s);
    void init();
    QObject *qobject() { return this; }

  private:
    QLabel *lab;
    QLineEdit *le;
    QPushButton *br;
    QComboBox *com;
    QCString &str;
    StringMode sm;
    QDict<int> *m_values;
    int m_index; 

  signals:
    void changed();

  private slots:
    void textChanged(const QString&);
    void browse();
    void clear();

};

#endif

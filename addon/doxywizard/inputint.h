/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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

#include <qwidget.h>
#include <qstring.h>

#include "input.h"

class QLabel;
class QSpinBox;
class PageWidget;

class InputInt : public QWidget, public IInput
{
  Q_OBJECT

  public:
    InputInt( const QString &text, PageWidget *parent, int &val, 
              int minVal, int maxVal );
    ~InputInt(){};
    void setEnabled(bool);
    void init();
    QObject *qobject() { return this; }

  private:
    QLabel *lab;
    QSpinBox *sp;
    int &m_val;
    int m_minVal;
    int m_maxVal;

  signals:
    void changed();

  private slots:
    void valueChanged(int val); 

};

#endif

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

#ifndef _INPUTBOOL_H
#define _INPUTBOOL_H

#include <qwidget.h>
#include <qcheckbox.h>

#include "input.h"

class PageWidget;

class InputBool : public QWidget, /*QCheckBox,*/ public IInput
{
  Q_OBJECT

  public:
    InputBool( const QString &text, PageWidget *parent, bool &flag );
    ~InputBool(){};
    void init();
    void setEnabled(bool b) { cb->setEnabled(b); }
    QObject *qobject() { return this; }
    bool getState() const { return state; }

  signals:
    void changed();
    void toggle(const char *,bool);

  private slots:
    void setState(bool);

  private:
    bool &state;
    QCheckBox *cb;

};

#endif

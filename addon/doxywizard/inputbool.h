/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include <qcheckbox.h>

class PageWidget;

class InputBool : public QCheckBox
{
  Q_OBJECT

  public:
    InputBool( const QString &text, PageWidget *parent, bool &flag );
    ~InputBool(){};
    void init();

  signals:
    void changed();

  private slots:
    void setState(bool);

  private:
    bool &state;

};

#endif

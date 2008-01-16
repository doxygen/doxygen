/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _INPUTSTRLIST_H
#define _INPUTSTRLIST_H

#include <qwidget.h>
#include <qstring.h>

#include "input.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QListBox;
class QStrList;

class InputStrList : public QWidget, public IInput
{
  Q_OBJECT

  public:
    enum ListMode { ListString=0, 
                    ListFile=1, 
                    ListDir=2, 
                    ListFileDir=ListFile|ListDir 
                  };
    
    InputStrList( const QString &text, QWidget *parent, 
                  QStrList &sl, ListMode v=ListString );
    ~InputStrList() {};
    void setEnabled(bool);
    void init();
    QObject *qobject() { return this; }

  private:
    QLabel *lab;
    QLineEdit *le;
    QPushButton *add;
    QPushButton *del;
    QPushButton *upd;
    QPushButton *brFile;
    QPushButton *brDir;
    QListBox *lb;
    QStrList &strList;

  signals:
    void changed();

  private slots:
    void addString(); 
    void delString(); 
    void updateString(); 
    void selectText(const QString &s);
    void browseFiles();
    void browseDir();

};

#endif

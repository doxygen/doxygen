/******************************************************************************
 *
 * $Id$
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

#ifndef _DOXWIZARD_H
#define _DOXWIZARD_H

#include <qmainwindow.h>

class QToolBar;
class ConfigFile;
class InputBool;
class InputString;
class InputStrList;
class InputFile;
class InputDir;
class InputInt;

class Wizard : public QMainWindow
{
  Q_OBJECT

  public:
    Wizard(int argc,char **argv);
    ~Wizard(){};
    void loadFile(const char *s);

  private slots:
    void newFile();
    void loadFile();
    void saveFile();
    void saveAsFile();
    void quit();
    void changed();
    void about();
    void runDoxygen(); // Micha

  signals:

  private:
    void refreshCaption();
    QToolBar *fileTools;
    bool hasChanged;
    ConfigFile *cw;
};

class ConfigFile : public QWidget
{
  Q_OBJECT

  public:
    ConfigFile(QWidget *parent=0);
    ~ConfigFile();
    void init();

  signals:
    void changed();
    
  private slots:
#CONFIG WidgetSlots

  private:
#CONFIG WidgetVars
};

#endif

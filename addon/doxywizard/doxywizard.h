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
#include <qlist.h>
#include <qdict.h>

class IInput;
class QToolBar;
class QWidget;
class QObject;
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

class ConfigWidget
{
  public:
    enum Kind { Int, String, StrList, Bool };
    
    ConfigWidget(QWidget *w,Kind k) : m_widget(w), m_kind(k) {}
    
    Kind kind() const { return m_kind; }
    
    InputString *getString() const 
    { return m_kind==String ? (InputString *)m_widget : 0; }
    
    InputBool *getBool() const 
    { return m_kind==Bool ? (InputBool *)m_widget : 0; }
    
    InputStrList *getStrList() const 
    { return m_kind==StrList ? (InputStrList *)m_widget : 0; }
    
    InputInt *getInt() const 
    { return m_kind==Int ? (InputInt *)m_widget : 0; }
    
  private:
    QWidget *m_widget;
    Kind m_kind;
};

class ConfigFile : public QWidget
{
  Q_OBJECT

  public:
    ConfigFile(QWidget *parent=0);
    ~ConfigFile();
    void init();
    void addDependency(QDict<QObject> *switches,
                       const QCString &dep,const QCString &name);

  signals:
    void changed();
    
  private slots:
    void toggle(const char *,bool);

  private:
    QDict<IInput>          *m_inputWidgets;
    QDict< QList<IInput> > *m_dependencies;
    QDict<QObject>         *m_switches;
};

#endif

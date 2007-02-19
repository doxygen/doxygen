/******************************************************************************
 *
 * 
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

#ifndef EXPERT_H
#define EXPERT_H

#include <qtabdialog.h>
#include <qlist.h>
#include <qdict.h>

class IInput;
class QWidget;
class QObject;
class InputBool;
class InputString;
class InputStrList;
class InputFile;
class InputDir;
class InputInt;

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

class Expert : public QTabDialog
{
  Q_OBJECT

  public:
    Expert(QWidget *parent=0);
    ~Expert();
    void init();
    void addDependency(QDict<QObject> *switches,
                       const QCString &dep,const QCString &name);
    bool hasChanged() const { return m_changed; }

  private slots:
    void toggle(const char *,bool);
    void handleHelp();
    void changed();

  private:
    QDict<IInput>          *m_inputWidgets;
    QDict< QList<IInput> > *m_dependencies;
    QDict<QObject>         *m_switches;
    bool                    m_changed;
};

#endif

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qfileinfo.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qscrollview.h>
#include <qlayout.h>

#include "expert.h"
#include "inputbool.h"
#include "inputstring.h"
#include "inputstrlist.h"
#include "inputint.h"
#include "config.h"
#include "version.h"

//-------------------------------------------------------------------------

Expert::Expert( QWidget *parent ) : QTabDialog( parent )
{

  m_dependencies = new QDict< QList<IInput> >(257);
  m_dependencies->setAutoDelete(TRUE);
  m_inputWidgets = new QDict< IInput >;
  m_switches = new QDict< QObject >;

  setHelpButton();
  
  QListIterator<ConfigOption> options = Config::instance()->iterator();
  QVBoxLayout *pageLayout = 0;
  QFrame *page = 0;
  ConfigOption *option = 0;
  for (options.toFirst();(option=options.current());++options)
  {
    switch(option->kind())
    {
      case ConfigOption::O_Info:
        {
          if (pageLayout) pageLayout->addStretch(1);
          QScrollView *view = new QScrollView(this);
          view->setVScrollBarMode(QScrollView::Auto);
          view->setHScrollBarMode(QScrollView::AlwaysOff);
          view->setResizePolicy(QScrollView::AutoOneFit);
          page = new QFrame( view->viewport(), option->name() );
          pageLayout = new QVBoxLayout(page);
          pageLayout->setMargin(10);
          view->addChild(page);
          addTab(view,option->name());
          QWhatsThis::add(page, option->docs().simplifyWhiteSpace() );
        }
        break;
      case ConfigOption::O_String:
        {
          ASSERT(page!=0);
          InputString::StringMode sm=InputString::StringFree;
          switch(((ConfigString *)option)->widgetType())
          {
            case ConfigString::String: sm=InputString::StringFree; break;
            case ConfigString::File:   sm=InputString::StringFile; break;
            case ConfigString::Dir:    sm=InputString::StringDir;  break;
          }
          InputString *inputString = new InputString( 
                         option->name(),                        // name
                         page,                                  // widget
                         *((ConfigString *)option)->valueRef(), // variable 
                         sm                                     // type
                       ); 
          pageLayout->addWidget(inputString);
          QWhatsThis::add(inputString, option->docs().simplifyWhiteSpace() );
          connect(inputString,SIGNAL(changed()),SIGNAL(changed()));
          m_inputWidgets->insert(option->name(),inputString);
          addDependency(m_switches,option->dependsOn(),option->name());
        }
        break;
      case ConfigOption::O_Enum:
        {
          ASSERT(page!=0);
          InputString *inputString = new InputString( 
                         option->name(),                        // name
                         page,                                  // widget
                         *((ConfigEnum *)option)->valueRef(),   // variable 
                         InputString::StringFixed               // type
                       ); 
          pageLayout->addWidget(inputString);
          QStrListIterator sli=((ConfigEnum *)option)->iterator();
          for (sli.toFirst();sli.current();++sli)
          {
            inputString->addValue(sli.current());
          }
          inputString->init();
          QWhatsThis::add(inputString, option->docs().simplifyWhiteSpace() );
          connect(inputString,SIGNAL(changed()),SIGNAL(changed()));
          m_inputWidgets->insert(option->name(),inputString);
          addDependency(m_switches,option->dependsOn(),option->name());
        }
        break;
      case ConfigOption::O_List:
        {
          ASSERT(page!=0);
          InputStrList::ListMode lm=InputStrList::ListString;
          switch(((ConfigList *)option)->widgetType())
          {
            case ConfigList::String:     lm=InputStrList::ListString;  break;
            case ConfigList::File:       lm=InputStrList::ListFile;    break;
            case ConfigList::Dir:        lm=InputStrList::ListDir;     break;
            case ConfigList::FileAndDir: lm=InputStrList::ListFileDir; break;
          }
          InputStrList *inputStrList = new InputStrList(
                          option->name(),                         // name
                          page,                                   // widget
                          *((ConfigList *)option)->valueRef(),    // variable
                          lm                                      // type
                        );
          pageLayout->addWidget(inputStrList);
          QWhatsThis::add(inputStrList, option->docs().simplifyWhiteSpace() );
          connect(inputStrList,SIGNAL(changed()),SIGNAL(changed()));
          m_inputWidgets->insert(option->name(),inputStrList);
          addDependency(m_switches,option->dependsOn(),option->name());
        }
        break;
        break;
      case ConfigOption::O_Bool:
        {
          ASSERT(page!=0);
          InputBool *inputBool = new InputBool(
                          option->name(),                         // name
                          page,                                   // widget
                          *((ConfigBool *)option)->valueRef()     // variable
                        );
          pageLayout->addWidget(inputBool);
          QWhatsThis::add(inputBool, option->docs().simplifyWhiteSpace() );
          connect(inputBool,SIGNAL(changed()),SIGNAL(changed()));
          m_inputWidgets->insert(option->name(),inputBool);
          addDependency(m_switches,option->dependsOn(),option->name());
        }
        break;
      case ConfigOption::O_Int:
        {
          ASSERT(page!=0);
          InputInt *inputInt = new InputInt(
                          option->name(),                         // name
                          page,                                   // widget
                          *((ConfigInt *)option)->valueRef(),     // variable
                          ((ConfigInt *)option)->minVal(),        // min value
                          ((ConfigInt *)option)->maxVal()         // max value
                        );
          pageLayout->addWidget(inputInt);
          QWhatsThis::add(inputInt, option->docs().simplifyWhiteSpace() );
          connect(inputInt,SIGNAL(changed()),SIGNAL(changed()));
          m_inputWidgets->insert(option->name(),inputInt);
          addDependency(m_switches,option->dependsOn(),option->name());
        }
        break;
      case ConfigOption::O_Obsolete:
        break;
    } 
  }
  if (pageLayout) pageLayout->addStretch(1);

  QDictIterator<QObject> di(*m_switches);
  QObject *obj = 0;
  for (di.toFirst();(obj=di.current());++di)
  {
    connect(obj,SIGNAL(toggle(const char *,bool)),SLOT(toggle(const char *,bool)));
    // UGLY HACK: assumes each item depends on a boolean without checking!
    emit toggle(di.currentKey(),((InputBool *)obj)->getState());
  }

  connect(this,SIGNAL(helpButtonPressed()),
          this,SLOT(handleHelp()));
  
}

Expert::~Expert()
{
  delete m_dependencies;
  delete m_inputWidgets;
  delete m_switches;
}

void Expert::handleHelp()
{
  QWhatsThis::enterWhatsThisMode();
}

void Expert::addDependency(QDict<QObject> *switches,
                               const QCString &dep,const QCString &name)
{
  if (!dep.isEmpty())
  {
    //printf("Expert::addDependency(%s)\n",name.data());
    IInput *parent = m_inputWidgets->find(dep);
    ASSERT(parent!=0);
    IInput *child = m_inputWidgets->find(name);
    ASSERT(child!=0);
    if (switches->find(dep)==0)
    {
      switches->insert(dep,parent->qobject());
    }
    QList<IInput> *list = m_dependencies->find(dep);
    if (list==0)
    {
      list = new QList<IInput>;
      m_dependencies->insert(dep,list);
    }
    list->append(child);
  }
}

void Expert::toggle(const char *name,bool state)
{
  QList<IInput> *inputs = m_dependencies->find(name);
  ASSERT(inputs!=0);
  IInput *input = inputs->first();
  while (input)
  {
    input->setEnabled(state);
    input = inputs->next();
  }
}

void Expert::init()
{
  QDictIterator<IInput> di(*m_inputWidgets);
  IInput *input = 0;
  for (di.toFirst();(input=di.current());++di)
  {
    input->init();
  }
  QDictIterator<QObject> dio(*m_switches);
  QObject *obj = 0;
  for (dio.toFirst();(obj=dio.current());++dio)
  {
    connect(obj,SIGNAL(toggle(const char *,bool)),SLOT(toggle(const char *,bool)));
    // UGLY HACK: assumes each item depends on a boolean without checking!
    emit toggle(dio.currentKey(),((InputBool *)obj)->getState());
  }
  
}


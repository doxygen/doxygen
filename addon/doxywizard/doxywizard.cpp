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

#include "doxywizard.h"
#include "pagewidget.h"
#include "inputbool.h"
#include "inputstring.h"
#include "inputstrlist.h"
#include "inputint.h"
#include "config.h"
#include "version.h"
#include "pixmaps.h"

QString configFileName;

const int messageTimeout = 4000; //!< status bar message timeout in millisec.

//-------------------------------------------------------------------------

static bool loadConfig( QString loadFile )
{
  QFileInfo fi( loadFile );
  if ( !fi.exists() )
  {
    QMessageBox::warning(0,
                         "Warning","Input file "+loadFile+
                         " does not exist! Starting new file.",
                         "ok"
                        );
    return FALSE;
  }

  // set config values to their defaults
  Config::instance()->init();

  // parse the config file
  // this will initialize the various Config data members
  if (!Config::instance()->parse(loadFile))
  {
    QMessageBox::warning(0,
                         "Warning","Cannot open or read input "+loadFile+"!",
                         "abort"
                        );
    return FALSE;
  }
    
  Config::instance()->convertStrToVal();

  return TRUE;
}

static bool saveConfig( QString saveFile )
{
  QFile f( saveFile );
  if ( !f.open(IO_WriteOnly) ) 
  {   
    QMessageBox::warning(0,
                         "Warning","Cannot open file "+saveFile+
                         " for writing. Nothing saved!.",
                         "ok"
                        );
    return FALSE; // failure
  }

  QTextStream t(&f);
  Config::instance()->writeTemplate(t,TRUE,FALSE); // write brief config file

  return TRUE; // success
}

//--------------------------------------------------------------------------

Wizard::Wizard(int argc,char **argv) : 
    QMainWindow( 0, (QCString)"DoxyWizard v"+versionString )
{
  // File popupmenu

  hasChanged=FALSE;
   
  fileTools = new QToolBar( this, "file operations" );
  fileTools->setLabel( "File Operations" );

       new QToolButton( QPixmap( file_xpm ), "New File", QString::null,
                        this, SLOT(newFile()), fileTools, "new file" );

  //QToolButton * fileOpen = 
       new QToolButton( QPixmap( fileopen_xpm ), "Open File", QString::null,
                        this, SLOT(loadFile()), fileTools, "open file" );

  //QToolButton * fileSave = 
       new QToolButton( QPixmap( filesave_xpm ), "Save File", QString::null,
                        this, SLOT(saveFile()), fileTools, "save file" );

 //Micha: QToolButton *  =
       new QToolButton( QPixmap( rundoxygen_xpm ), "Save and Run doxygen", QString::null,
                        this, SLOT(runDoxygen()), fileTools, "save and run doxygen" );


  QWhatsThis::whatsThisButton( fileTools );     
  
  QPopupMenu* file = new QPopupMenu;
  file->insertItem( QIconSet(QPixmap(file_xpm)), 
                    "&New",  this, SLOT(newFile()),  CTRL+Key_N );
  file->insertItem( QIconSet(QPixmap(fileopen_xpm)),
                    "&Load", this, SLOT(loadFile()), CTRL+Key_O );
  file->insertSeparator();
  file->insertItem( QIconSet(QPixmap(filesave_xpm)),
                    "&Save", this, SLOT(saveFile()), CTRL+Key_S );
  file->insertItem( "&Save as ...", this, SLOT(saveAsFile()) );
  file->insertSeparator();
  file->insertItem( "&Quit", this, SLOT(quit()),     CTRL+Key_Q );

  // Doxygen popupmenu
  QPopupMenu* doxygen = new QPopupMenu;
  int itemIndex = doxygen->insertItem( QIconSet(QPixmap(rundoxygen_xpm)), 
                                       "&Run", this, SLOT(runDoxygen()), CTRL+Key_R );
  doxygen->setWhatsThis ( itemIndex, "Run doxygen with the current configuration file..." );

  // Help popupmenu
  QPopupMenu* help = new QPopupMenu;
  help->insertItem( "&About", this, SLOT(about()), Key_F1 );
  help->insertItem( "What's &This", this , SLOT(whatsThis()), SHIFT+Key_F1);

  // menubar definition
  menuBar()->insertItem( "&File", file );
  menuBar()->insertItem( "&Doxygen", doxygen );
  menuBar()->insertSeparator();
  menuBar()->insertItem( "&Help", help ); 

  statusBar()->message("Welcome to DoxyWizard",messageTimeout);

  cw = new ConfigFile( this );
  connect(cw,SIGNAL(changed()),this,SLOT(changed()));
  setCentralWidget( cw );
  cw->show();

  if (argc==2)
  {
    loadFile(argv[1]);
  }
  else
  {
    newFile();
  }
  refreshCaption();
  
  resize( 640, 480 );

}

void Wizard::newFile()
{
  if (hasChanged)
  {
    switch( QMessageBox::warning( this, "DoxyWizard", 
            "Start a new file and lose changes?",
            "Yes", "No", 0, 0, 1 ))
    {
      case 0: 
        break;
      default: // command aborted by the user 
        return;
    }
  }
  Config::instance()->init();
  configFileName="Doxyfile";
  cw->init();
}

void Wizard::loadFile(const char *fileName)
{
  if (fileName==0)
  {
    configFileName = QFileDialog::getOpenFileName();
  }
  else
  {
    configFileName = fileName;
  }

  if ( !configFileName.isNull() )
  {
    //initData( configFileName );
    loadConfig(configFileName);
    cw->init();

    statusBar()->message(configFileName, messageTimeout);			
  } 
}

void Wizard::loadFile()
{
  if (hasChanged)
  {
    switch( QMessageBox::warning( 
            this, "DoxyWizard", "Load a new file and lose changes?",
            "Yes", "No", 0, 0, 1 ))
    {
      case 0: 
        break;
      default: // command aborted by the user 
        return;
    }
  }
  loadFile(0);
}

void Wizard::saveFile()
{
  //printf("configFileName=`%s'\n",configFileName.data());
  if (configFileName.isEmpty())
  {
    saveAsFile();
    return;
  }
  else
  {
    saveConfig(configFileName);
    statusBar()->message(configFileName + " saved", messageTimeout);
  }
  hasChanged = FALSE;
  refreshCaption();
}

void Wizard::saveAsFile()
{
  configFileName = QFileDialog::getSaveFileName();
  if (configFileName.isNull())
  {
    statusBar()->message("Save aborted", messageTimeout );
    return; // operation cancelled by the user
  }

  saveConfig(configFileName);
  statusBar()->message("Saved as  "+ configFileName, messageTimeout );
}

//Micha
// TODO: Use QProcess to show the output in a window
void Wizard::runDoxygen()
{
  saveFile();
  QString orgDir = QDir::currentDirPath();
  QFileInfo fi(configFileName);
  // cd to the config dir
  QDir::setCurrent(fi.dirPath());
  // run doxygen on the config file
  QString s = "doxygen \"" + fi.fileName() + "\"";
  // actually run the exe
  system((const char*)s);  
  statusBar()->message("doxygen completed: "+ configFileName, messageTimeout );
  hasChanged = FALSE;
  refreshCaption();
  QDir::setCurrent(orgDir);
} 


void Wizard::quit()
{
  if (hasChanged)
  {
    switch( QMessageBox::warning( this, "DoxyWizard", "Quit and lose changes?",
            "Yes", "No", 0, 0, 1 ))
    {
      case 0: 
        qApp->quit();
        break;
      case 1:
        break;
    }
  }
  else
  {
    qApp->quit();
  }
}

void Wizard::changed()
{
  hasChanged = TRUE;
  refreshCaption();
}

void Wizard::refreshCaption()
{
  QString s;
  if (hasChanged) s=" *";
  setCaption("Doxywizard - ["+configFileName+s+"]");
}

void Wizard::about()
{
  QCString text(4096);
  text.sprintf( "<qt><center>A tool to create and edit configuration files "
                "that can be read by doxygen version %s.</center><p>"
                "<center>Written by Dimitri van Heesch</center><p>"
                "<center>(c) 2000-2003</center></qt>",versionString
              );
  QMessageBox::about(this, "DoxyWizard",text);
}

//----------------------------------------------------------------------


ConfigFile::ConfigFile( QWidget *parent ) : QWidget( parent )
{

  m_dependencies = new QDict< QList<IInput> >(257);
  m_dependencies->setAutoDelete(TRUE);
  m_inputWidgets = new QDict< IInput >;
  m_switches = new QDict< QObject >;
  
  QVBoxLayout *layout = new QVBoxLayout( this );

  // QTabWidget definition 
  QTabWidget *tab = new QTabWidget( this );
  layout->addWidget( tab );

  QListIterator<ConfigOption> options = Config::instance()->iterator();
  PageWidget *page = 0;
  ConfigOption *option = 0;
  for (options.toFirst();(option=options.current());++options)
  {
    switch(option->kind())
    {
      case ConfigOption::O_Info:
        if (page) page->addStretch(1);
        page = new PageWidget( tab, option->name() );
        QWhatsThis::add(page, option->docs().simplifyWhiteSpace() );
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
          QStrListIterator sli=((ConfigEnum *)option)->iterator();
          for (sli.toFirst();sli.current();++sli)
          {
            inputString->addValue(sli.current());
          }
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
  if (page) page->addStretch(1);

  QDictIterator<QObject> di(*m_switches);
  QObject *obj = 0;
  for (di.toFirst();(obj=di.current());++di)
  {
    connect(obj,SIGNAL(toggle(const char *,bool)),SLOT(toggle(const char *,bool)));
    // UGLY HACK: assumes each item depends on a boolean without checking!
    emit toggle(di.currentKey(),((InputBool *)obj)->getState());
  }
  
}

ConfigFile::~ConfigFile()
{
  delete m_dependencies;
  delete m_inputWidgets;
  delete m_switches;
}

void ConfigFile::addDependency(QDict<QObject> *switches,
                               const QCString &dep,const QCString &name)
{
  if (!dep.isEmpty())
  {
    //printf("ConfigFile::addDependency(%s)\n",name.data());
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

void ConfigFile::toggle(const char *name,bool state)
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

void ConfigFile::init()
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

//----------------------------------------------------------------------

int main(int argc,char **argv)
{ 
  QApplication::setColorSpec( QApplication::NormalColor );
  QApplication app(argc,argv);
  Wizard wizard(argc,argv);
  app.setMainWidget( &wizard );
  wizard.show();  
  return app.exec();
}

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <stdio.h>
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
  QFile f( loadFile );
  if ( !f.open(IO_ReadOnly) )
  {
    QMessageBox::warning(0,
                         "Warning","Cannot open input "+loadFile+
                         " for reading!.",
                         "abort"
                        );
    exit(1);
  }

  // set config values to their defaults
  Config::init();

  // read file into a string buffer
  int fsize = fi.size();
  QCString contents(fsize+1); // add room for \0 terminator
  f.readBlock(contents.data(),fsize);
  contents[fsize]='\0';

  // parse the config file
  // this will initialize the various Config data members
  parseConfig(contents);
  configStrToVal();

  f.close();
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

  writeTemplateConfig(&f,TRUE); // write brief config file

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
  //QPopupMenu* doxygen = new QPopupMenu;
  //int itemIndex = doxygen->insertItem( "&Run" );
  //doxygen->setWhatsThis ( itemIndex, "Run doxygen with the current configuration file..." );
  //doxygen->setItemEnabled( itemIndex, FALSE );

  // Help popupmenu
  QPopupMenu* help = new QPopupMenu;
  help->insertItem( "&About", this, SLOT(about()), Key_F1 );
  help->insertItem( "What's &This", this , SLOT(whatsThis()), SHIFT+Key_F1);

  // menubar definition
  menuBar()->insertItem( "&File", file );
  //menuBar()->insertItem( "&Doxygen", doxygen );
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
    switch( QMessageBox::warning( this, "DoxyWizard", "Start a new file and lose changes?\n",
            "Yes", "No", 0, 0, 1 ))
    {
      case 0: 
        break;
      default: // command aborted by the user 
        return;
    }
  }
  Config::init();
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
            this, "DoxyWizard", "Load a new file and lose changes?\n",
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
  statusBar()->message("Saved as "+ configFileName, messageTimeout );
  hasChanged = FALSE;
  refreshCaption();
}

void Wizard::quit()
{
  if (hasChanged)
  {
    switch( QMessageBox::warning( this, "DoxyWizard", "Quit and lose changes?\n",
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
  QMessageBox::about(this, "DoxyWizard",
                      "<qt><center>A tool to create and edit configuration files "
                      "that can be read by doxygen.</center><p>"
                      "<center>Written by Dimitri van Heesch</center><p>"
                      "<center>(c) 2000</center></qt>"
                    );
}

//----------------------------------------------------------------------

ConfigFile::ConfigFile( QWidget *parent ) : QWidget( parent )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  // QTabWidget definition 
  QTabWidget *tab = new QTabWidget( this );
  layout->addWidget( tab );

#CONFIG Widgets0
#CONFIG Widgets1
  
}

ConfigFile::~ConfigFile()
{
}

#CONFIG WidgetSlotImpl

void ConfigFile::init()
{
#CONFIG WidgetInit
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

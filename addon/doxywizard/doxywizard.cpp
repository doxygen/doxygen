#include "doxywizard.h"
#include "version.h"
#include "expert.h"
#include "wizard.h"

#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QStatusBar>
#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QTextStream>
#include <QDebug>

#ifdef WIN32
#include <windows.h>
#endif

#define MAX_RECENT_FILES 10

const int messageTimeout = 5000; //!< status bar message timeout in milliseconds.

MainWindow &MainWindow::instance()
{
  static MainWindow *theInstance = new MainWindow;
  return *theInstance;
}

MainWindow::MainWindow()
  : m_settings(QString::fromLatin1("Doxygen.org"), QString::fromLatin1("Doxywizard"))
{
  QMenu *file = menuBar()->addMenu(tr("File"));
  file->addAction(tr("Open..."), 
                  this, SLOT(openConfig()), Qt::CTRL+Qt::Key_O);
  m_recentMenu = file->addMenu(tr("Open recent"));
  file->addAction(tr("Save"), 
                  this, SLOT(saveConfig()), Qt::CTRL+Qt::Key_S);
  file->addAction(tr("Save as..."), 
                  this, SLOT(saveConfigAs()), Qt::SHIFT+Qt::CTRL+Qt::Key_S);
  file->addAction(tr("Quit"),  
                  this, SLOT(quit()), Qt::CTRL+Qt::Key_Q);

  QMenu *settings = menuBar()->addMenu(tr("Settings"));
  settings->addAction(tr("Reset to factory defaults"),
                  this,SLOT(resetToDefaults()));
  settings->addAction(tr("Use current settings at startup"),
                  this,SLOT(makeDefaults()));
  settings->addAction(tr("Clear recent list"),
                  this,SLOT(clearRecent()));

  QMenu *help = menuBar()->addMenu(tr("Help"));
  help->addAction(tr("Online manual"), 
                  this, SLOT(manual()), Qt::Key_F1);
  help->addAction(tr("About"), 
                  this, SLOT(about()) );

  m_expert = new Expert;
  m_wizard = new Wizard(m_expert->modelData());

  // ----------- top part ------------------
  QWidget *topPart = new QWidget;
  QVBoxLayout *rowLayout = new QVBoxLayout(topPart);

  // select working directory
  QHBoxLayout *dirLayout = new QHBoxLayout;
  m_workingDir = new QLineEdit;
  m_selWorkingDir = new QPushButton(tr("Select..."));
  dirLayout->addWidget(m_workingDir);
  dirLayout->addWidget(m_selWorkingDir);

  //------------- bottom part --------------
  QWidget *runTab = new QWidget;
  QVBoxLayout *runTabLayout = new QVBoxLayout(runTab);

  // run doxygen
  QHBoxLayout *runLayout = new QHBoxLayout;
  m_run = new QPushButton(tr("Run doxygen"));
  m_run->setEnabled(false);
  m_runStatus = new QLabel(tr("Status: not running"));
  m_saveLog = new QPushButton(tr("Save log..."));
  m_saveLog->setEnabled(false);
  QPushButton *showSettings = new QPushButton(tr("Show configuration"));
  runLayout->addWidget(m_run);
  runLayout->addWidget(m_runStatus);
  runLayout->addStretch(1);
  runLayout->addWidget(showSettings);
  runLayout->addWidget(m_saveLog);

  // output produced by Doxygen
  runTabLayout->addLayout(runLayout);
  runTabLayout->addWidget(new QLabel(tr("Output produced by doxygen")));
  QGridLayout *grid = new QGridLayout;
  m_outputLog = new QTextEdit;
  m_outputLog->setReadOnly(true);
  m_outputLog->setFontFamily(QString::fromLatin1("courier"));
  m_outputLog->setMinimumWidth(600);
  grid->addWidget(m_outputLog,0,0);
  grid->setColumnStretch(0,1);
  grid->setRowStretch(0,1);
  QHBoxLayout *launchLayout = new QHBoxLayout;
  m_launchHtml = new QPushButton(tr("Show HTML output"));
  launchLayout->addWidget(m_launchHtml);

  launchLayout->addStretch(1);
  grid->addLayout(launchLayout,1,0);
  runTabLayout->addLayout(grid);

  m_tabs = new QTabWidget;
  m_tabs->addTab(m_wizard,tr("Wizard"));
  m_tabs->addTab(m_expert,tr("Expert"));
  m_tabs->addTab(runTab,tr("Run"));

  rowLayout->addWidget(new QLabel(tr("Step 1: Specify the working directory from which doxygen will run")));
  rowLayout->addLayout(dirLayout);
  rowLayout->addWidget(new QLabel(tr("Step 2: Configure doxygen using the Wizard and/or Expert tab, then switch to the Run tab to generate the documentation")));
  rowLayout->addWidget(m_tabs);

  setCentralWidget(topPart);
  statusBar()->showMessage(tr("Welcome to Doxygen"),messageTimeout);

  m_runProcess = new QProcess;
  m_running = false;
  m_timer = new QTimer;

  // connect signals and slots
  connect(m_tabs,SIGNAL(currentChanged(int)),SLOT(selectTab(int)));
  connect(m_selWorkingDir,SIGNAL(clicked()),SLOT(selectWorkingDir()));
  connect(m_recentMenu,SIGNAL(triggered(QAction*)),SLOT(openRecent(QAction*)));
  connect(m_workingDir,SIGNAL(returnPressed()),SLOT(updateWorkingDir()));
  connect(m_runProcess,SIGNAL(readyReadStandardOutput()),SLOT(readStdout()));
  connect(m_runProcess,SIGNAL(finished(int, QProcess::ExitStatus)),SLOT(runComplete()));
  connect(m_timer,SIGNAL(timeout()),SLOT(readStdout()));
  connect(m_run,SIGNAL(clicked()),SLOT(runDoxygen()));
  connect(m_launchHtml,SIGNAL(clicked()),SLOT(showHtmlOutput()));
  connect(m_saveLog,SIGNAL(clicked()),SLOT(saveLog()));
  connect(showSettings,SIGNAL(clicked()),SLOT(showSettings()));
  connect(m_expert,SIGNAL(changed()),SLOT(configChanged()));
  connect(m_wizard,SIGNAL(done()),SLOT(selectRunTab()));
  connect(m_expert,SIGNAL(done()),SLOT(selectRunTab()));

  loadSettings();
  updateLaunchButtonState();
  m_modified = false;
  updateTitle();
  m_wizard->refresh();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (discardUnsavedChanges())
  {
    saveSettings();
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void MainWindow::quit()
{
  if (discardUnsavedChanges())
  {
    saveSettings();
  }
  QApplication::exit(0);
}

void MainWindow::setWorkingDir(const QString &dirName)
{
    QDir::setCurrent(dirName);
    m_workingDir->setText(dirName);
    m_run->setEnabled(!dirName.isEmpty());
}

void MainWindow::selectWorkingDir()
{
  QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Select working directory"),m_workingDir->text());
  if (!dirName.isEmpty())
  {
    setWorkingDir(dirName);
  }
}

void MainWindow::updateWorkingDir()
{
  setWorkingDir(m_workingDir->text());
}

void MainWindow::manual()
{
  QDesktopServices::openUrl(QUrl(QString::fromLatin1("http://www.doxygen.org/manual.html")));
}

void MainWindow::about()
{
  QString msg;
  QTextStream t(&msg,QIODevice::WriteOnly);
  t << QString::fromLatin1("<qt><center>A tool to configure and run doxygen version ")+
       QString::fromLatin1(versionString)+
       QString::fromLatin1(" on your source files.</center><p><br>"
       "<center>Written by<br> Dimitri van Heesch<br>&copy; 2000-2015</center><p>"
       "</qt>");
  QMessageBox::about(this,tr("Doxygen GUI"),msg);
}

void MainWindow::openConfig()
{
  if (discardUnsavedChanges(false))
  {
    QString fn = QFileDialog::getOpenFileName(this,
        tr("Open configuration file"),
        m_workingDir->text());
    if (!fn.isEmpty())
    {
      loadConfigFromFile(fn);
    }
  }
}

void MainWindow::updateConfigFileName(const QString &fileName)
{
  if (m_fileName!=fileName)
  {
    m_fileName = fileName;
    QString curPath = QFileInfo(fileName).path();
    setWorkingDir(curPath);
    addRecentFile(fileName);
    updateTitle();
  }
}

void MainWindow::loadConfigFromFile(const QString & fileName)
{
  // save full path info of original file
  QString absFileName = QFileInfo(fileName).absoluteFilePath();
  // updates the current directory
  updateConfigFileName(fileName);
  // open the specified configuration file
  m_expert->loadConfig(absFileName);
  m_wizard->refresh();
  updateLaunchButtonState();
  m_modified = false;
  updateTitle();
}

void MainWindow::saveConfig(const QString &fileName)
{
  if (fileName.isEmpty()) return;
  QFile f(fileName);
  if (!f.open(QIODevice::WriteOnly)) 
  {
    QMessageBox::warning(this,
        tr("Error saving"),
        tr("Error: cannot open the file ")+fileName+tr(" for writing!\n")+
        tr("Reason given: ")+f.error());
    return;
  }
  QTextStream t(&f);
  m_expert->writeConfig(t,false);
  updateConfigFileName(fileName);
  m_modified = false;
  updateTitle();
}

bool MainWindow::saveConfig()
{
  if (m_fileName.isEmpty())
  {
    return saveConfigAs();
  }
  else
  {
    saveConfig(m_fileName);
    return true;
  }
}

bool MainWindow::saveConfigAs()
{
  QString fileName = QFileDialog::getSaveFileName(this, QString(), 
             m_workingDir->text()+QString::fromLatin1("/Doxyfile"));
  if (fileName.isEmpty()) return false;
  saveConfig(fileName);
  return true;
}

void MainWindow::makeDefaults()
{
  if (QMessageBox::question(this,tr("Use current setting at startup?"),
                        tr("Do you want to save the current settings "
                           "and use them next time Doxywizard starts?"),
                        QMessageBox::Save|
                        QMessageBox::Cancel)==QMessageBox::Save)
  {
    //printf("MainWindow:makeDefaults()\n");
    m_expert->saveSettings(&m_settings);
    m_settings.setValue(QString::fromLatin1("wizard/loadsettings"), true);
    m_settings.sync();
  }
}

void MainWindow::clearRecent()
{
  if (QMessageBox::question(this,tr("Clear the list of recent files?"),
                        tr("Do you want to clear the list of recently "
                           "loaded configuration files?"),
                        QMessageBox::Yes|
                        QMessageBox::Cancel)==QMessageBox::Yes)
  {
    m_recentMenu->clear();
    m_recentFiles.clear();
    for (int i=0;i<MAX_RECENT_FILES;i++)
    {
      m_settings.setValue(QString().sprintf("recent/config%d",i++),QString::fromLatin1(""));
    }
    m_settings.sync();
  }
  
}

void MainWindow::resetToDefaults()
{
  if (QMessageBox::question(this,tr("Reset settings to their default values?"),
                        tr("Do you want to revert all settings back "
                           "to their original values?"),
                        QMessageBox::Reset|
                        QMessageBox::Cancel)==QMessageBox::Reset)
  {
    //printf("MainWindow:resetToDefaults()\n");
    m_expert->resetToDefaults();
    m_settings.setValue(QString::fromLatin1("wizard/loadsettings"), false);
    m_settings.sync();
    m_wizard->refresh();
  }
}

void MainWindow::loadSettings()
{
  QVariant geometry     = m_settings.value(QString::fromLatin1("main/geometry"), QVariant::Invalid);
  QVariant state        = m_settings.value(QString::fromLatin1("main/state"),    QVariant::Invalid);
  QVariant wizState     = m_settings.value(QString::fromLatin1("wizard/state"),  QVariant::Invalid);
  QVariant loadSettings = m_settings.value(QString::fromLatin1("wizard/loadsettings"),  QVariant::Invalid);
  QVariant workingDir   = m_settings.value(QString::fromLatin1("wizard/workingdir"), QVariant::Invalid);

  if (geometry  !=QVariant::Invalid) restoreGeometry(geometry.toByteArray());
  if (state     !=QVariant::Invalid) restoreState   (state.toByteArray());
  if (wizState  !=QVariant::Invalid) m_wizard->restoreState(wizState.toByteArray());
  if (loadSettings!=QVariant::Invalid && loadSettings.toBool())
  {
    m_expert->loadSettings(&m_settings);
    if (workingDir!=QVariant::Invalid && QDir(workingDir.toString()).exists())
    {
      setWorkingDir(workingDir.toString());
    }
  }

  for (int i=0;i<MAX_RECENT_FILES;i++)
  {
    QString entry = m_settings.value(QString().sprintf("recent/config%d",i)).toString();
    if (!entry.isEmpty() && QFileInfo(entry).exists())
    {
      addRecentFile(entry);
    }
  }

}

void MainWindow::saveSettings()
{
  QSettings settings(QString::fromLatin1("Doxygen.org"), QString::fromLatin1("Doxywizard"));

  m_settings.setValue(QString::fromLatin1("main/geometry"), saveGeometry());
  m_settings.setValue(QString::fromLatin1("main/state"),    saveState());
  m_settings.setValue(QString::fromLatin1("wizard/state"),  m_wizard->saveState());
  m_settings.setValue(QString::fromLatin1("wizard/workingdir"), m_workingDir->text());
}

void MainWindow::selectTab(int id)
{
  if (id==0) m_wizard->refresh();
  else if (id==1) m_expert->refresh();
}

void MainWindow::selectRunTab()
{
  m_tabs->setCurrentIndex(2);
}

void MainWindow::addRecentFile(const QString &fileName)
{
  int i=m_recentFiles.indexOf(fileName);
  if (i!=-1) m_recentFiles.removeAt(i);
  
  // not found
  if (m_recentFiles.count() < MAX_RECENT_FILES) // append
  {
    m_recentFiles.prepend(fileName);
  }
  else // add + drop last item
  {
    m_recentFiles.removeLast();
    m_recentFiles.prepend(fileName);
  }
  m_recentMenu->clear();
  i=0;
  foreach( QString str, m_recentFiles ) 
  {
    m_recentMenu->addAction(str);
    m_settings.setValue(QString().sprintf("recent/config%d",i++),str);
  }
  for (;i<MAX_RECENT_FILES;i++)
  {
    m_settings.setValue(QString().sprintf("recent/config%d",i++),QString::fromLatin1(""));
  }
}

void MainWindow::openRecent(QAction *action)
{
  if (discardUnsavedChanges(false))
  {
    loadConfigFromFile(action->text());
  }
}

void MainWindow::runDoxygen()
{
  if (!m_running)
  {
    QString doxygenPath; 
#if defined(Q_OS_MACX)
    doxygenPath = qApp->applicationDirPath()+QString::fromLatin1("/../Resources/");
    qDebug() << tr("Doxygen path: ") << doxygenPath;
    if ( !QFile(doxygenPath + QString::fromLatin1("doxygen")).exists() ) 
    {
      // No Doxygen binary in the resources, if there is a system Doxygen binary, use that instead
      if ( QFile(QString::fromLatin1("/usr/local/bin/doxygen")).exists() )
      {
        doxygenPath = QString::fromLatin1("/usr/local/bin/");
      }
      else 
      {
        qDebug() << tr("Can't find the doxygen command, make sure it's in your $$PATH");
        doxygenPath = QString::fromLatin1("");
      }
    }
    qDebug() << tr("Getting doxygen from: ") << doxygenPath;
#endif

    m_runProcess->setReadChannel(QProcess::StandardOutput);
    m_runProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_runProcess->setWorkingDirectory(m_workingDir->text());
    QStringList env=QProcess::systemEnvironment();
    // set PWD environment variable to m_workingDir
    env.replaceInStrings(QRegExp(QString::fromLatin1("^PWD=(.*)"),Qt::CaseInsensitive), 
                         QString::fromLatin1("PWD=")+m_workingDir->text());
    m_runProcess->setEnvironment(env);

    QStringList args;
    args << QString::fromLatin1("-b"); // make stdout unbuffered
    args << QString::fromLatin1("-");  // read config from stdin

    m_outputLog->clear();
    m_runProcess->start(doxygenPath + QString::fromLatin1("doxygen"), args);

    if (!m_runProcess->waitForStarted())
    {
      m_outputLog->append(QString::fromLatin1("*** Failed to run doxygen\n"));
      return;
    }
    QTextStream t(m_runProcess);
    m_expert->writeConfig(t,false);
    m_runProcess->closeWriteChannel();

    if (m_runProcess->state() == QProcess::NotRunning)
    {
      m_outputLog->append(QString::fromLatin1("*** Failed to run doxygen\n"));
    }
    else
    {
      m_saveLog->setEnabled(false);
      m_running=true;
      m_run->setText(tr("Stop doxygen"));
      m_runStatus->setText(tr("Status: running"));
      m_timer->start(1000);
    }
  }
  else
  {
    m_running=false;
    m_run->setText(tr("Run doxygen"));
    m_runStatus->setText(tr("Status: not running"));
    m_runProcess->kill();
    m_timer->stop();
    //updateRunnable(m_workingDir->text());
  }
}

void MainWindow::readStdout()
{
  if (m_running)
  {
    QByteArray data = m_runProcess->readAllStandardOutput();
    QString text = QString::fromUtf8(data);
    if (!text.isEmpty())
    {
      m_outputLog->append(text.trimmed());
    }
  }
}

void MainWindow::runComplete()
{
  if (m_running)
  {
    m_outputLog->append(tr("*** Doxygen has finished\n"));
  }
  else
  {
    m_outputLog->append(tr("*** Cancelled by user\n"));
  }
  m_outputLog->ensureCursorVisible();
  m_run->setText(tr("Run doxygen"));
  m_runStatus->setText(tr("Status: not running"));
  m_running=false;
  updateLaunchButtonState();
  //updateRunnable(m_workingDir->text());
  m_saveLog->setEnabled(true);
}

void MainWindow::updateLaunchButtonState()
{
  m_launchHtml->setEnabled(m_expert->htmlOutputPresent(m_workingDir->text()));
#if 0
  m_launchPdf->setEnabled(m_expert->pdfOutputPresent(m_workingDir->text()));
#endif
}

void MainWindow::showHtmlOutput()
{
  QString indexFile = m_expert->getHtmlOutputIndex(m_workingDir->text());
  QFileInfo fi(indexFile);
  // TODO: the following doesn't seem to work with IE
#ifdef WIN32
  //QString indexUrl(QString::fromLatin1("file:///"));
  ShellExecute(NULL, L"open", (LPCWSTR)fi.absoluteFilePath().utf16(), NULL, NULL, SW_SHOWNORMAL);
#else
  QString indexUrl(QString::fromLatin1("file://"));
  indexUrl+=fi.absoluteFilePath();
  QDesktopServices::openUrl(QUrl(indexUrl));
#endif
}

void MainWindow::saveLog()
{
  QString fn = QFileDialog::getSaveFileName(this, tr("Save log file"), 
        m_workingDir->text()+
        QString::fromLatin1("/doxygen_log.txt"));
  if (!fn.isEmpty())
  {
    QFile f(fn);
    if (f.open(QIODevice::WriteOnly))
    {
      QTextStream t(&f);
      t << m_outputLog->toPlainText();
      statusBar()->showMessage(tr("Output log saved"),messageTimeout);
    }
    else
    {
      QMessageBox::warning(0,tr("Warning"),
          tr("Cannot open file ")+fn+tr(" for writing. Nothing saved!"),tr("ok"));
    }
  }
}

void MainWindow::showSettings()
{
  QString text;
  QTextStream t(&text);
  m_expert->writeConfig(t,true);
  m_outputLog->clear();
  m_outputLog->append(text);
  m_outputLog->ensureCursorVisible();
  m_saveLog->setEnabled(true);
}

void MainWindow::configChanged()
{
  m_modified = true;
  updateTitle();
}

void MainWindow::updateTitle()
{
  QString title = tr("Doxygen GUI frontend");
  if (m_modified)
  {
    title+=QString::fromLatin1(" +");
  }
  if (!m_fileName.isEmpty())
  {
    title+=QString::fromLatin1(" (")+m_fileName+QString::fromLatin1(")");
  }
  setWindowTitle(title);
}

bool MainWindow::discardUnsavedChanges(bool saveOption)
{
  if (m_modified)
  {
    QMessageBox::StandardButton button;
    if (saveOption)
    {
      button = QMessageBox::question(this,
          tr("Unsaved changes"),
          tr("Unsaved changes will be lost! Do you want to save the configuration file?"),
          QMessageBox::Save    |
          QMessageBox::Discard |
          QMessageBox::Cancel
          );
      if (button==QMessageBox::Save)
      {
        return saveConfig();
      }
    }
    else
    {
      button = QMessageBox::question(this,
          tr("Unsaved changes"),
          tr("Unsaved changes will be lost! Do you want to continue?"),
          QMessageBox::Discard |
          QMessageBox::Cancel
          );
    }
    return button==QMessageBox::Discard;
  }
  return true;
}

//-----------------------------------------------------------------------
int main(int argc,char **argv)
{
  QApplication a(argc,argv);
  if (argc == 2)
  {
    if (!qstrcmp(argv[1],"--help"))
    {
      QMessageBox msgBox;
      msgBox.setText(QString().sprintf("Usage: %s [config file]",argv[0]));
      msgBox.exec();
      exit(0);
    }
  }
  if (argc > 2)
  {
    QMessageBox msgBox;
    msgBox.setText(QString().sprintf("Too many arguments specified\n\nUsage: %s [config file]",argv[0]));
    msgBox.exec();
    exit(1);
  }
  else
  {
    MainWindow &main = MainWindow::instance();
    if (argc==2 && argv[1][0]!='-') // name of config file as an argument
    {
      main.loadConfigFromFile(QString::fromLocal8Bit(argv[1]));
    }
    main.show();
    return a.exec();
  }
}

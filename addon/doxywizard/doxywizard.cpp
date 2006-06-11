#include <qlabel.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qprocess.h>
#include <qtimer.h>
#include <qstatusbar.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtooltip.h>

#include <stdlib.h>

#if defined(Q_OS_MACX)
#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>
#undef check
#endif

#include "doxywizard.h"
#include "expert.h"
#include "config.h"
#include "version.h"

const int messageTimeout = 5000; //!< status bar message timeout in millisec.

#if defined(Q_OS_MACX)
QCString getResourcePath()
{
  // todo: use qApp->applicationDirPath()
  QCString result;
  CFURLRef pluginRef  = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, kCFURLPOSIXPathStyle);
  result = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  result+="/Contents/Resources/";
  return result;
}

#define GRAPHVIZ_PATH    "/Applications/Graphviz.app"
#define DOT_PATH         GRAPHVIZ_PATH "/Contents/MacOS"
#define DOT_LOCATION     DOT_PATH "/dot"

bool checkIfDotInstalled()
{
  QFileInfo fi(GRAPHVIZ_PATH);
  if (fi.exists() && fi.isDir())
  {
    fi.setFile(DOT_LOCATION);
    if (fi.exists() && fi.isFile())
    {
      return TRUE;
    }
  }
  return FALSE;
}

void setDotPath()
{
  if (checkIfDotInstalled())
  {
    Config_getString("DOT_PATH")=DOT_PATH;
    Config_getBool("HAVE_DOT")=TRUE;
  }
}

#endif


//==========================================================================

Step1::Step1(QWidget *parent) : QWidget(parent,"Step1") 
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(4);
  layout->setSpacing(8);
  layout->addWidget(new QLabel( "Provide some information "
              "about the project you are documenting",this));
  QWidget *w      = new QWidget( this );
  QHBoxLayout *bl = new QHBoxLayout(w,10);

  QVBox *col1      = new QVBox( w );
  col1->setSpacing(8);
  (new QLabel("Project name:",col1))->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  (new QLabel("Project version or id:",col1))->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  QVBox *col2      = new QVBox( w );
  col2->setSpacing(8);
  m_projName = new QLineEdit(col2);
  m_projNumber = new QLineEdit(col2);

  bl->addWidget(col1);
  bl->addWidget(col2);

  layout->addWidget(w);

  //---------------------------------------------------
  QFrame *f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);
  
  layout->addWidget(new QLabel( "Specify the directory to "
              "scan for source code", this));
  QHBox *row = new QHBox( this );
  row->setSpacing(10);
  new QLabel("Source code directory:",row);
  m_sourceDir = new QLineEdit(row);
  m_srcSelectDir = new QPushButton("Select...",row);
  layout->addWidget(row);

  m_recursive = new QCheckBox("Scan recursively",this);
  m_recursive->setChecked(TRUE);
  layout->addWidget(m_recursive);

  //---------------------------------------------------
  f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);

  layout->addWidget(new QLabel(
              "Specify the directory where doxygen should "
              "put the generated documentation",this
            ));
  row = new QHBox( this );
  row->setSpacing(10);
  new QLabel("Destination directory:",row);
  m_destDir = new QLineEdit(row);
  m_dstSelectDir = new QPushButton("Select...",row);
  layout->addWidget(row);
  layout->addStretch(1);

  connect(m_srcSelectDir,SIGNAL(clicked()),
          this,SLOT(selectSourceDir()));
  connect(m_dstSelectDir,SIGNAL(clicked()),
          this,SLOT(selectDestinationDir()));
}

void Step1::selectSourceDir()
{
  m_sourceDir->setText(QFileDialog::getExistingDirectory(m_sourceDir->text(),this));
}

void Step1::selectDestinationDir()
{
  m_destDir->setText(QFileDialog::getExistingDirectory(m_destDir->text(),this));
}

QString Step1::getProjectName() const 
{ 
  return m_projName->text(); 
}

QString Step1::getProjectNumber() const 
{ 
  return m_projNumber->text(); 
}

QString Step1::getSourceDir() const
{
  return m_sourceDir->text();
}

bool Step1::scanRecursively() const
{
  return m_recursive->isChecked();
}

QString Step1::getDestinationDir() const
{
  return m_destDir->text();
}

void Step1::setProjectName(const QString &name)
{
  m_projName->setText(name);
}

void Step1::setProjectNumber(const QString &num)
{
  m_projNumber->setText(num);
}

void Step1::setSourceDir(const QString &dir)
{
  m_sourceDir->setText(dir);
}

void Step1::setRecursiveScan(bool enable)
{
  m_recursive->setChecked(enable);
}

void Step1::setDestinationDir(const QString &dir)
{
  m_destDir->setText(dir);
}


//==========================================================================

Step2::Step2(QWidget *parent) : QWidget(parent,"Step2") 
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(8);

  m_extractMode = new QButtonGroup("Select the desired extraction mode:",this);
  QGridLayout *gbox = new QGridLayout( m_extractMode, 4, 1, 8, 0 );
  gbox->addRowSpacing( 0, fontMetrics().lineSpacing()+2 );
  gbox->addWidget(new QRadioButton("Documented entities only",m_extractMode),1,0);
  gbox->addWidget(new QRadioButton("All entities",m_extractMode),2,0);
  gbox->addWidget(m_crossRef = new QCheckBox("Include cross-referenced source code in the output",m_extractMode),3,0);
  m_extractMode->setButton(0);
  layout->addWidget(m_extractMode);
  
  //---------------------------------------------------
  QFrame *f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);

  m_optimizeLang = new QButtonGroup("Select programming language to optimize the results for",this);
  gbox = new QGridLayout( m_optimizeLang, 4, 1, 8, 0 );
  gbox->addRowSpacing( 0, fontMetrics().lineSpacing()+2 );
  gbox->addWidget(new QRadioButton("Optimize for C++ output",m_optimizeLang),1,0);
  gbox->addWidget(new QRadioButton("Optimize for Java output",m_optimizeLang),2,0);
  gbox->addWidget(new QRadioButton("Optimize for C output",m_optimizeLang),3,0);
  m_optimizeLang->setButton(0);
  layout->addWidget(m_optimizeLang);

  layout->addStretch(1);

  connect(m_crossRef,SIGNAL(stateChanged(int)),
          parent,SLOT(changeCrossRefState(int)));
}

bool Step2::crossReferencingEnabled() const
{
  //printf("Step2::crossReferencingEnabled()=%d\n",m_crossRef->isOn());
  return m_crossRef->isOn();
}

void Step2::enableCrossReferencing()
{
  //printf("Step2::enableCrossReferencing()\n");
  m_crossRef->setChecked(TRUE);
}

bool Step2::extractAll() const
{
  //printf("Step2::extractAll()=%d\n",m_extractMode->find(1)->isOn());
  return m_extractMode->find(1)->isOn();
}

bool Step2::crossReferencing() const
{
  return m_crossRef->isChecked();
}

OptLang Step2::optimizeFor() const
{
  switch (m_optimizeLang->selectedId())
  {
    case 0:  return Lang_Cpp;
    case 1:  return Lang_Java;
    case 2:  return Lang_C;
  }
  return Lang_Cpp;
}

void Step2::setExtractAll(bool enable)
{
  //printf("Step2::setExtractAll(%d)\n",enable);
  m_extractMode->setButton(enable?1:0);
}

void Step2::setCrossReferencing(bool enable)
{
  //printf("Step2::setCrossReferencing(%d)\n",enable);
  m_crossRef->setChecked(enable);
}

void Step2::setOptimizeFor(OptLang lang)
{
  switch (lang)
  {
    case Lang_Cpp:  m_optimizeLang->setButton(0); break;
    case Lang_Java: m_optimizeLang->setButton(1); break;
    case Lang_C:    m_optimizeLang->setButton(2); break;
  }
}

//==========================================================================

Step3::Step3(QWidget *parent) : QWidget(parent,"Step3") 
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  QButtonGroup *bg = new QButtonGroup("Output format(s) to generate",this);
  QGridLayout *gbox = new QGridLayout( bg, 8, 1, 8, 0 );
  gbox->addRowSpacing( 0, fontMetrics().lineSpacing()+2 );
  gbox->addWidget(m_htmlEnabled=new QCheckBox("HTML",bg),1,0);
  QWidget *w = new QWidget(bg);
  QHBoxLayout *bl = new QHBoxLayout(w);
    m_htmlOptions = new QButtonGroup(w);
    m_htmlOptions->hide();
    m_htmlOptionBox = new QVBox(w);
    m_htmlOptions->insert(new QRadioButton("plain HTML",m_htmlOptionBox));
    m_htmlOptions->insert(new QRadioButton("with frames and a navigation tree",m_htmlOptionBox));
    m_htmlOptions->insert(new QRadioButton("prepare for compressed HTML (.chm)",m_htmlOptionBox));
    m_htmlOptions->insert(m_searchEnabled=new QCheckBox("With search function (requires PHP enabled web server)",m_htmlOptionBox));
    bl->addSpacing(30);
    bl->addWidget(m_htmlOptionBox);
    m_htmlOptions->setButton(0);
    m_htmlOptionBox->setEnabled(TRUE);
  gbox->addWidget(w,2,0);

  gbox->addWidget(m_latexEnabled=new QCheckBox("LaTeX",bg),3,0);
  w = new QWidget(bg);
  bl = new QHBoxLayout(w);
    m_texOptions = new QButtonGroup(w);
    m_texOptions->hide();
    m_texOptionBox = new QVBox(w);
    m_texOptions->insert(new QRadioButton("as intermediate format for hyperlinked PDF",m_texOptionBox));
    m_texOptions->insert(new QRadioButton("as intermediate format for PDF",m_texOptionBox));
    m_texOptions->insert(new QRadioButton("as intermediate format for PostScript",m_texOptionBox));
    bl->addSpacing(30);
    bl->addWidget(m_texOptionBox);
    m_texOptions->setButton(0);
    m_texOptionBox->setEnabled(FALSE);
  gbox->addWidget(w,4,0);

  gbox->addWidget(m_manEnabled=new QCheckBox("Man pages",bg),5,0);
  gbox->addWidget(m_rtfEnabled=new QCheckBox("Rich Text Format (RTF)",bg),6,0);
  gbox->addWidget(m_xmlEnabled=new QCheckBox("XML",bg),7,0);
  bg->setButton(0);
  layout->addWidget(bg);

  layout->addStretch(1);

  connect(m_latexEnabled,SIGNAL(stateChanged(int)),
          this,SLOT(latexStateChange(int)));
  connect(m_htmlEnabled,SIGNAL(stateChanged(int)),
          this,SLOT(htmlStateChange(int)));
}

void Step3::latexStateChange(int state)
{
  if (state==QButton::On)
  {
    m_texOptionBox->setEnabled(TRUE);
  }
  else if (state==QButton::Off)
  {
    m_texOptionBox->setEnabled(FALSE);
  }
}

void Step3::htmlStateChange(int state)
{
  if (state==QButton::On)
  {
    m_htmlOptionBox->setEnabled(TRUE);
  }
  else if (state==QButton::Off)
  {
    m_htmlOptionBox->setEnabled(FALSE);
  }
}

bool Step3::htmlEnabled() const
{
  return m_htmlEnabled->isChecked();
}

bool Step3::latexEnabled() const
{
  return m_latexEnabled->isChecked();
}

bool Step3::manEnabled() const
{
  return m_manEnabled->isChecked();
}

bool Step3::rtfEnabled() const
{
  return m_rtfEnabled->isChecked();
}

bool Step3::xmlEnabled() const
{
  return m_xmlEnabled->isChecked();
}

bool Step3::searchEnabled() const   
{ 
  return m_searchEnabled->isChecked();   
}

HtmlStyle Step3::htmlStyle() const   
{ 
  if (m_htmlOptions->find(0)->isOn())
  {
    return HS_Plain;
  }
  else if (m_htmlOptions->find(1)->isOn())
  {
    return HS_TreeView;
  }
  else if (m_htmlOptions->find(2)->isOn())
  {
    return HS_CHM;
  }
  // broken radio button logic
  return HS_Plain;
}

TexStyle Step3::texStyle() const   
{ 
  switch (m_texOptions->selectedId())
  {
    case 0: return TS_PDFHyper;
    case 1: return TS_PDF;
    case 2: return TS_PS;
  }
  return TS_PDFHyper;
}

void Step3::setHtmlEnabled(bool enable)
{
  m_htmlEnabled->setChecked(enable);
}

void Step3::setLatexEnabled(bool enable)
{
  m_latexEnabled->setChecked(enable);
}

void Step3::setManEnabled(bool enable)
{
  m_manEnabled->setChecked(enable);
}

void Step3::setRtfEnabled(bool enable)
{
  m_rtfEnabled->setChecked(enable);
}

void Step3::setXmlEnabled(bool enable)
{
  m_xmlEnabled->setChecked(enable);
}

void Step3::setSearchEnabled(bool enable)
{
  m_searchEnabled->setChecked(enable);
}

void Step3::setHtmlStyle(HtmlStyle style)
{
  switch(style)
  {
    case HS_Plain:    m_htmlOptions->setButton(0); break;
    case HS_TreeView: m_htmlOptions->setButton(1); break;
    case HS_CHM:      m_htmlOptions->setButton(2); break;
  }
}

void Step3::setTexStyle(TexStyle style)
{
  switch(style)
  {
    case TS_PDFHyper: m_texOptions->setButton(0); break;
    case TS_PDF:      m_texOptions->setButton(1); break;
    case TS_PS:       m_texOptions->setButton(2); break;
  }
}


//==========================================================================

Step4::Step4(QWidget *parent) : QWidget(parent,"Step4") 
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  m_diagramMode = new QButtonGroup("Diagrams to generate",this);
  QGridLayout *gbox = new QGridLayout( m_diagramMode, 5, 1, 8, 0 );
  gbox->addRowSpacing( 0, fontMetrics().lineSpacing()+2 );
  gbox->addWidget(new QRadioButton("No diagrams",m_diagramMode),1,0);
  gbox->addWidget(new QRadioButton("Use built-in class diagram generator",m_diagramMode),2,0);
  gbox->addWidget(new QRadioButton("Use dot tool from the GraphViz package to generate",m_diagramMode),3,0);
  QWidget *w = new QWidget(m_diagramMode);
  QHBoxLayout *bl = new QHBoxLayout(w);
    QButtonGroup *dotGroup = new QButtonGroup(w);
    dotGroup->hide();
    m_dotOptions = new QVBox(w);
    dotGroup->insert(m_dotClass=new QCheckBox("Class diagrams",m_dotOptions));
    dotGroup->insert(m_dotCollaboration=new QCheckBox("Collaboration diagrams",m_dotOptions));
    dotGroup->insert(m_dotInclude=new QCheckBox("Include dependency graphs",m_dotOptions));
    dotGroup->insert(m_dotIncludedBy=new QCheckBox("Included by dependency graphs",m_dotOptions));
    dotGroup->insert(m_dotInheritance=new QCheckBox("Overall Class hierarchy",m_dotOptions));
    dotGroup->insert(m_dotCall=new QCheckBox("Call graphs",m_dotOptions));
    bl->addSpacing(30);
    bl->addWidget(m_dotOptions);
    dotGroup->setButton(0);
    m_dotOptions->setEnabled(FALSE);
  gbox->addWidget(w,4,0);
  m_diagramMode->setButton(1);
  layout->addWidget(m_diagramMode);
  layout->addStretch(1);

  connect(m_diagramMode,SIGNAL(clicked(int)),
          this,SLOT(diagramModeChanged(int)));
  connect(m_dotCall,SIGNAL(stateChanged(int)),
          parent,SLOT(changeCallGraphState(int)));
}

void Step4::diagramModeChanged(int buttonId)
{
  m_dotOptions->setEnabled(buttonId==2);
}

void Step4::disableCallGraphs()
{
  m_dotCall->setChecked(FALSE);
}

bool Step4::callGraphEnabled() const
{
  return m_dotCall->isOn();
}

DiagramMode Step4::diagramMode() const
{
  switch(m_diagramMode->selectedId())
  {
    case 0: return DM_None;
    case 1: return DM_Builtin;
    case 2: return DM_Dot;
  }
  return DM_None;
}

bool Step4::classDiagram() const
{
  return m_dotClass->isChecked();
}

bool Step4::collaborationDiagram() const
{
  return m_dotCollaboration->isChecked();
}

bool Step4::includeGraph() const
{
  return m_dotInclude->isChecked();
}

bool Step4::includedByGraph() const
{
  return m_dotIncludedBy->isChecked();
}

bool Step4::inheritanceGraph() const
{
  return m_dotInheritance->isChecked();
}

bool Step4::callGraph() const
{
  return m_dotCall->isChecked();
}

void Step4::setDiagramMode(DiagramMode mode)
{
  switch(mode)
  {
    case DM_None:    m_diagramMode->setButton(0); diagramModeChanged(0); break;
    case DM_Builtin: m_diagramMode->setButton(1); diagramModeChanged(1); break;
    case DM_Dot:     m_diagramMode->setButton(2); diagramModeChanged(2); break;
  }
}

void Step4::setClassDiagram(bool enable)
{
  m_dotClass->setChecked(enable);
}

void Step4::setCollaborationDiagram(bool enable)
{
  m_dotCollaboration->setChecked(enable);
}

void Step4::setIncludeGraph(bool enable)
{
  m_dotInclude->setChecked(enable);
}

void Step4::setIncludedByGraph(bool enable)
{
  m_dotIncludedBy->setChecked(enable);
}

void Step4::setInheritanceGraph(bool enable)
{
  m_dotInheritance->setChecked(enable);
}

void Step4::setCallGraph(bool enable)
{
  m_dotCall->setChecked(enable);
}


//==========================================================================

Wizard::Wizard(QWidget *parent=0) : QTabDialog(parent) 
{
  addTab( m_step1 = new Step1(this),"Project");
  addTab( m_step2 = new Step2(this),"Mode");
  addTab( m_step3 = new Step3(this),"Output");
  addTab( m_step4 = new Step4(this),"Diagrams");
  setCancelButton();
}

void Wizard::changeCallGraphState(int state)
{
  if (state==QButton::On && !m_step2->crossReferencingEnabled())
  {
    if (QMessageBox::question(this,"This option depends on another option",
                              "The call graph option requires that cross-referencing "
                              "of source code is enabled.\nDo you want to enable this "
                              "option?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes)
    {
      m_step2->enableCrossReferencing();
    }
    else
    {
      m_step4->disableCallGraphs();
    }
  }
}

void Wizard::changeCrossRefState(int state)
{
  if (state==QButton::Off && m_step4->callGraphEnabled())
  {
    if (QMessageBox::question(this,"Another option depends on this one",
                              "The call graph option requires that cross-referencing "
                              "of source code is enabled.\nDo you want to disable the call "
                              "graph option?",QMessageBox::Yes,QMessageBox::No
                             )==QMessageBox::Yes
       )
    {
      m_step4->disableCallGraphs();
    }
    else
    {
      m_step2->enableCrossReferencing();
    }
  }
}

//==========================================================================

MainWidget::MainWidget(QWidget *parent) 
  : QMainWindow(parent,"MainWidget")
{
  setCaption("Doxygen GUI frontend");

  // initialize config settings
  Config::instance()->init();
  Config::instance()->check();
#if defined(Q_OS_MACX)
  setDotPath();
#endif
  
  QWidget *w = new QWidget(this);
  setCentralWidget(w);
  QVBoxLayout *layout = new QVBoxLayout(w);
  layout->setMargin(10);

  QPopupMenu *file = new QPopupMenu(this);
  file->insertItem("Open...",  this, SLOT(loadConfigFile()), CTRL+Key_O);
  file->insertItem("Save As...",  this, SLOT(saveConfigFile()), CTRL+Key_S);
  m_recentMenu = new QPopupMenu;
  file->insertItem( "&Recent configurations", m_recentMenu );
  connect(m_recentMenu,SIGNAL(activated(int)),this,SLOT(openRecent(int)));
  file->insertItem("Set as default...",this,SLOT(saveDefaults()) );
  file->insertItem("Reset...",this,SLOT(resetConfig()) );
  file->insertItem("Quit",  this, SLOT(quit()), CTRL+Key_Q);
  QPopupMenu *help = new QPopupMenu(this);
#if defined(Q_OS_MACX)
  help->insertItem("Online manual", this, SLOT(manual()), Key_F1);
#endif
  help->insertItem("About", this, SLOT(about()) );
  menuBar()->insertItem("File",file);
  menuBar()->insertItem("Help",help);

  //---------------------------------------------------
  QVBox *heading = new QVBox(w);
  QLabel *l = new QLabel("Step 1: Configure doxygen",heading);
  QFrame *f = new QFrame( heading );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  //f->setFixedSize(l->sizeHint().width(),f->sizeHint().height());
  layout->addWidget(heading);
  layout->addSpacing(10);

  l = new QLabel("Choose one of the following ways to configure doxygen",w);
  layout->addWidget(l);
  layout->addSpacing(5);
  QHBoxLayout *hbl = new QHBoxLayout(layout);
  hbl->setSpacing(8);

  m_wizard = new QPushButton("Wizard...",w);
  QToolTip::add(m_wizard,"Quickly configure the most important settings");
  hbl->addWidget(m_wizard);

  m_expert = new QPushButton("Expert...",w);
  QToolTip::add(m_expert,"Determine all configuration settings in detail");
  hbl->addWidget(m_expert);

  m_load = new QPushButton("Load...",w);
  QToolTip::add(m_load,"Load the configuration settings from file");
  hbl->addWidget(m_load);

  hbl->addStretch(1);

  //---------------------------------------------------
  heading = new QVBox(w);
  l = new QLabel("Step 2: Save the configuration file",heading);
  f = new QFrame( heading );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  //f->setFixedSize(l->sizeHint().width(),f->sizeHint().height());
  layout->addSpacing(15);
  layout->addWidget(heading);
  layout->addSpacing(5);

  QGridLayout *grid = new QGridLayout(layout,1,2,10);
  m_save = new QPushButton("Save...",w);
  m_save->setFixedSize(m_save->sizeHint());
  grid->addWidget(m_save,0,0);
  grid->addWidget(m_saveStatus=new QLabel(w),0,1);
  m_saveStatus->setTextFormat(Qt::RichText);

  //---------------------------------------------------
  heading = new QVBox(w);
  l = new QLabel("Step 3: Specify the directory from which to run doxygen",heading);
  f = new QFrame( heading );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  //f->setFixedSize(l->sizeHint().width(),f->sizeHint().height());
  layout->addSpacing(15);
  layout->addWidget(heading);
  layout->addSpacing(5);

  QHBoxLayout *row = new QHBoxLayout( layout );
  row->setSpacing(10);
  row->addWidget(new QLabel("Working directory:",w));
  m_workingDir = new QLineEdit(w);
  m_selWorkingDir = new QPushButton("Select...",w);
  row->addWidget(m_workingDir,1);
  row->addWidget(m_selWorkingDir);

  //---------------------------------------------------
  heading = new QVBox(w);
  l = new QLabel("Step 4: Run doxygen",heading);
  f = new QFrame( heading );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  //f->setFixedSize(l->sizeHint().width(),f->sizeHint().height());
  layout->addSpacing(15);
  layout->addWidget(heading);
  layout->addSpacing(5);

  row = new QHBoxLayout( layout );
  row->setSpacing(10);
  m_run = new QPushButton("Start",w);
  row->addWidget(m_run);
  row->addWidget(m_runStatus = new QLabel("Status: not running",w));
  row->addStretch(1);
  m_run->setEnabled(FALSE);
  m_runStatus->setEnabled(FALSE);

  m_saveLog = new QPushButton("Save log...",w);
  m_saveLog->setEnabled(FALSE);
  row->addWidget(m_saveLog);

  layout->addSpacing(8);
  layout->addWidget(m_outputHeading=new QLabel("Output produced by doxygen",w));
  grid = new QGridLayout(layout,1,1,3);
  m_outputLog = new QTextEdit(w);
  m_outputLog->setReadOnly(TRUE);
  m_outputLog->setMinimumWidth(600);
  grid->addWidget(m_outputLog,0,0);
  grid->setColStretch(0,1);
  grid->setRowStretch(0,1);
  m_outputLog->setEnabled(FALSE);
  m_outputHeading->setEnabled(FALSE);

  // make connections
  connect(m_wizard,SIGNAL(clicked()),
          this,SLOT(launchWizard()));
  connect(m_expert,SIGNAL(clicked()),
          this,SLOT(launchExpert()));
  connect(m_load,SIGNAL(clicked()),
          this,SLOT(loadConfigFile()));
  connect(m_save,SIGNAL(clicked()),
          this,SLOT(saveConfigFile()));
  connect(m_selWorkingDir,SIGNAL(clicked()),
          this,SLOT(selectWorkingDir()));
  connect(m_workingDir,SIGNAL(textChanged(const QString&)),
          this,SLOT(updateRunnable(const QString&)));
  connect(m_run,SIGNAL(clicked()),
          this,SLOT(runDoxygen()));
  connect(m_saveLog,SIGNAL(clicked()),
          this,SLOT(saveLog()));

  // load default settings
  m_settings.setPath("www.doxygen.org","Doxygen GUI");
  bool ok;
  QString config = m_settings.readEntry("/doxywizard/config/default",QString::null,&ok);
  if (ok && !config.isEmpty())
  {
    Config::instance()->parseString("default settings",config);
  }
  QString workingDir = m_settings.readEntry("/doxywizard/config/workingdir",QString::null,&ok);
  if (ok && !workingDir.isEmpty())
  {
    m_workingDir->setText(workingDir);
  }
  setConfigSaved(FALSE);
  for (int i=0;i<10;i++)
  {
    QString entry = m_settings.readEntry(QString().sprintf("/doxywizard/recent/config%d",i));
    if (!entry.isEmpty())
    {
      addRecentFile(entry);
    }
  }

  m_runProcess = new QProcess;
  connect(m_runProcess,SIGNAL(readyReadStdout()),this,SLOT(readStdout()));
  connect(m_runProcess,SIGNAL(processExited()),this,SLOT(runComplete()));
  m_running = FALSE;
  m_timer = new QTimer;
  connect(m_timer,SIGNAL(timeout()),this,SLOT(readStdout()));

  statusBar()->message("Welcome to Doxygen",messageTimeout);
}

MainWidget::~MainWidget()
{
  delete m_runProcess;
}

void MainWidget::selectWorkingDir()
{
  QString selectedDir = QFileDialog::getExistingDirectory(m_workingDir->text());
  if (!selectedDir.isEmpty())
  {
    m_workingDir->setText(selectedDir);
  }
}

void MainWidget::updateRunnable(const QString &newDirName)
{
  if (m_running) return;
  QFileInfo fi(newDirName);
  bool isRunnable = fi.exists() && fi.isDir() && m_configSaved;
  m_run->setEnabled(isRunnable);
  m_runStatus->setEnabled(isRunnable);
  m_outputHeading->setEnabled(isRunnable);
  m_outputLog->setEnabled(isRunnable);
  m_saveLog->setEnabled(isRunnable && !m_outputLog->text().isEmpty());
}

void MainWidget::launchWizard()
{
  Wizard wizard(this);

  // --------  Initialize the dialog ----------------
  
  // step1
  wizard.setProjectName(Config_getString("PROJECT_NAME"));
  wizard.setProjectNumber(Config_getString("PROJECT_NUMBER"));
  if (Config_getList("INPUT").count()>0)
  {
    QString dirName=Config_getList("INPUT").getFirst();
    QFileInfo fi(dirName);
    if (fi.exists() && fi.isDir())
    {
      wizard.setSourceDir(dirName);
    }
  }
  wizard.setRecursiveScan(Config_getBool("RECURSIVE"));
  wizard.setDestinationDir(Config_getString("OUTPUT_DIRECTORY"));

  // step2
  wizard.setExtractAll(Config_getBool("EXTRACT_ALL"));
  wizard.setCrossReferencing(Config_getBool("SOURCE_BROWSER"));
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    wizard.setOptimizeFor(Lang_Java);
  }
  else if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
  {
    wizard.setOptimizeFor(Lang_C);
  }
  else
  {
    wizard.setOptimizeFor(Lang_Cpp);
  }

  // step3
  wizard.setHtmlEnabled(Config_getBool("GENERATE_HTML"));
  wizard.setLatexEnabled(Config_getBool("GENERATE_LATEX"));
  wizard.setManEnabled(Config_getBool("GENERATE_MAN"));
  wizard.setRtfEnabled(Config_getBool("GENERATE_RTF"));
  wizard.setXmlEnabled(Config_getBool("GENERATE_XML"));
  wizard.setSearchEnabled(Config_getBool("SEARCHENGINE"));
  if (Config_getBool("GENERATE_HTMLHELP"))
  {
    wizard.setHtmlStyle(HS_CHM);
  }
  else if (Config_getBool("GENERATE_TREEVIEW"))
  {
    wizard.setHtmlStyle(HS_TreeView);
  }
  else
  {
    wizard.setHtmlStyle(HS_Plain);
  }
  if (Config_getBool("USE_PDFLATEX"))
  {
    if (Config_getBool("PDF_HYPERLINKS"))
    {
      wizard.setTexStyle(TS_PDFHyper);
    }
    else
    {
      wizard.setTexStyle(TS_PDF);
    }
  }
  else
  {
    wizard.setTexStyle(TS_PS);
  }

  // step4
  if (Config_getBool("HAVE_DOT"))
  {
    wizard.setDiagramMode(DM_Dot);
  }
  else if (Config_getBool("CLASS_DIAGRAMS"))
  {
    wizard.setDiagramMode(DM_Builtin);
  }
  else
  {
    wizard.setDiagramMode(DM_None);
  }
  wizard.setClassDiagram(Config_getBool("CLASS_GRAPH"));
  wizard.setCollaborationDiagram(Config_getBool("COLLABORATION_GRAPH"));
  wizard.setIncludeGraph(Config_getBool("INCLUDE_GRAPH"));
  wizard.setIncludedByGraph(Config_getBool("INCLUDED_BY_GRAPH"));
  wizard.setInheritanceGraph(Config_getBool("GRAPHICAL_HIERARCHY"));
  wizard.setCallGraph(Config_getBool("CALL_GRAPH"));
  
  // --------  Run the dialog ----------------

  if (wizard.exec()==QDialog::Accepted)
  {
    // -------- Store the results ----------------

    // step1
    Config_getString("PROJECT_NAME")=wizard.getProjectName();
    Config_getString("PROJECT_NUMBER")=wizard.getProjectNumber();
    Config_getList("INPUT").clear();
    Config_getList("INPUT").append(wizard.getSourceDir());
    Config_getBool("RECURSIVE")=wizard.scanRecursively();
    Config_getString("OUTPUT_DIRECTORY")=wizard.getDestinationDir();

    // step2
    if (wizard.extractAll())
    {
      Config_getBool("EXTRACT_ALL")=TRUE;
      Config_getBool("EXTRACT_PRIVATE")=TRUE;
      Config_getBool("EXTRACT_STATIC")=TRUE;
      Config_getBool("EXTRACT_LOCAL_CLASSES")=TRUE;
      Config_getBool("HIDE_UNDOC_MEMBERS")=FALSE;
      Config_getBool("HIDE_UNDOC_CLASSES")=FALSE;
    } 
    else
    {
      Config_getBool("EXTRACT_ALL")=FALSE;
      Config_getBool("EXTRACT_PRIVATE")=FALSE;
      Config_getBool("EXTRACT_STATIC")=FALSE;
      Config_getBool("EXTRACT_LOCAL_CLASSES")=TRUE;
      Config_getBool("HIDE_UNDOC_MEMBERS")=TRUE;
      Config_getBool("HIDE_UNDOC_CLASSES")=TRUE;
    }
    
    if (wizard.crossReferencing())
    {
      Config_getBool("SOURCE_BROWSER")=TRUE;
      Config_getBool("REFERENCED_BY_RELATION")=TRUE;
      Config_getBool("REFERENCES_RELATION")=TRUE;
      Config_getBool("VERBATIM_HEADERS")=TRUE;
    }
    else
    {
      Config_getBool("SOURCE_BROWSER")=FALSE;
      Config_getBool("REFERENCED_BY_RELATION")=FALSE;
      Config_getBool("REFERENCES_RELATION")=FALSE;
      Config_getBool("VERBATIM_HEADERS")=FALSE;
    }

    switch(wizard.optimizeFor())
    {
      case Lang_Cpp:
        Config_getBool("OPTIMIZE_OUTPUT_FOR_C")=FALSE;
        Config_getBool("OPTIMIZE_OUTPUT_JAVA")=FALSE;
        break;
      case Lang_Java:
        Config_getBool("OPTIMIZE_OUTPUT_FOR_C")=FALSE;
        Config_getBool("OPTIMIZE_OUTPUT_JAVA")=TRUE;
        break;
      case Lang_C:
        Config_getBool("OPTIMIZE_OUTPUT_FOR_C")=TRUE;
        Config_getBool("OPTIMIZE_OUTPUT_JAVA")=FALSE;
        break;
    }

    // step3
    Config_getBool("GENERATE_HTML")=wizard.htmlEnabled();
    Config_getBool("GENERATE_LATEX")=wizard.latexEnabled();
    Config_getBool("GENERATE_MAN")=wizard.manEnabled();
    Config_getBool("GENERATE_RTF")=wizard.rtfEnabled();
    Config_getBool("GENERATE_XML")=wizard.xmlEnabled();
    Config_getBool("SEARCHENGINE")=wizard.searchEnabled();
    if (wizard.htmlEnabled())
    {
      switch (wizard.htmlStyle())
      {
        case HS_Plain:
          Config_getBool("GENERATE_HTMLHELP")=FALSE;
          Config_getBool("GENERATE_TREEVIEW")=FALSE;
          break;
        case HS_TreeView:
          Config_getBool("GENERATE_HTMLHELP")=FALSE;
          Config_getBool("GENERATE_TREEVIEW")=TRUE;
          break;
        case HS_CHM:
          Config_getBool("GENERATE_HTMLHELP")=TRUE;
          Config_getBool("GENERATE_TREEVIEW")=FALSE;
          break;
      }
    }
    if (wizard.latexEnabled())
    {
      switch (wizard.texStyle())
      {
        case TS_PDFHyper:
          Config_getBool("USE_PDFLATEX")=TRUE;
          Config_getBool("PDF_HYPERLINKS")=TRUE;
          break;
        case TS_PDF:
          Config_getBool("USE_PDFLATEX")=TRUE;
          Config_getBool("PDF_HYPERLINKS")=FALSE;
          break;
        case TS_PS:
          Config_getBool("USE_PDFLATEX")=FALSE;
          Config_getBool("PDF_HYPERLINKS")=FALSE;
          break;
      }
    }

    // step4
    switch (wizard.diagramMode())
    {
      case DM_None:
        Config_getBool("CLASS_DIAGRAMS")=FALSE;
        Config_getBool("HAVE_DOT")=FALSE;
        break;
      case DM_Builtin:
        Config_getBool("CLASS_DIAGRAMS")=TRUE;
        Config_getBool("HAVE_DOT")=FALSE;
        break;
      case DM_Dot:
        Config_getBool("CLASS_DIAGRAMS")=FALSE;
        Config_getBool("HAVE_DOT")=TRUE;
        Config_getBool("CLASS_GRAPH")=wizard.classDiagram();
        Config_getBool("COLLABORATION_GRAPH")=wizard.collaborationDiagram();
        Config_getBool("INCLUDE_GRAPH")=wizard.includeGraph();
        Config_getBool("INCLUDED_BY_GRAPH")=wizard.includedByGraph();
        Config_getBool("GRAPHICAL_HIERARCHY")=wizard.inheritanceGraph();
        Config_getBool("CALL_GRAPH")=wizard.callGraph();
        break;
    }
#if defined(Q_OS_MACX)
    if (Config_getBool("HAVE_DOT"))
    {
      setDotPath();
    }
#endif
    setConfigSaved(FALSE);
  }
}

void MainWidget::loadConfigFromFile(const QString &fn)
{
  if (!Config::instance()->parse(fn))
  {
    QMessageBox::warning(0,"Warning",
        "Could not open or read config file "+fn+"!",
        "abort"
        );
  }
  else
  {
    Config::instance()->convertStrToVal();
#if defined(Q_OS_MACX)
    if (checkIfDotInstalled() && 
        qstricmp(Config_getString("DOT_PATH"),DOT_PATH)!=0
       )
    {
      Config_getString("DOT_PATH")=DOT_PATH;
      setConfigSaved(FALSE);
    }
    else
    {
      setConfigSaved(TRUE);
    }
#else
    setConfigSaved(TRUE);
#endif
    addRecentFile(fn);
    m_workingDir->setText(QFileInfo(fn).dirPath(TRUE));
    m_configFileName = fn;
    statusBar()->message("New configuration loaded",messageTimeout);
  }
}

void MainWidget::loadConfigFile()
{
  QString fn = QFileDialog::getOpenFileName(m_workingDir->text(),QString::null,this);
  if (!fn.isEmpty())
  {
    loadConfigFromFile(fn);
    // set current dir to where the config file is located
    QDir::setCurrent(QFileInfo(fn).dirPath(TRUE));
  }
}

void MainWidget::launchExpert()
{
  Expert expert(this);
  expert.init();
  expert.exec();
#if defined(Q_OS_MACX)
  setDotPath();
#endif
  if (expert.hasChanged()) setConfigSaved(FALSE);
}

void MainWidget::saveDefaults()
{
  if (QMessageBox::question(this,"Store settings as defaults",
                "Do you wish to use the "
                "current configuration settings as the default settings?",
                QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
  {
    QString newConfig;
    QTextStream t(&newConfig,IO_WriteOnly);
    Config::instance()->writeTemplate(t,TRUE,FALSE);
    m_settings.writeEntry("/doxywizard/config/default",newConfig);
    m_settings.writeEntry("/doxywizard/config/workingdir",m_workingDir->text());
    statusBar()->message("Current configuration saved as default",messageTimeout);
  }
}

void MainWidget::saveConfigFile()
{
  QString fn = QFileDialog::getSaveFileName(m_workingDir->text()+"/Doxyfile",QString::null,this);
  if (!fn.isEmpty())
  {
    QFile f(fn);
    if (f.exists() && QMessageBox::question(this,
          "Overwrite file? -- Doxywizard",
          "A file called "+fn+" already exists. Do you want to overwrite it?",
          "&Yes","&no",
          QString::null, 0, 1)
       )
    {
      statusBar()->message("Save request aborted",messageTimeout);
      return;
    }
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
      Config::instance()->writeTemplate(t,TRUE,FALSE);
      m_configFileName = fn;
      setConfigSaved(TRUE);
      addRecentFile(fn);
      statusBar()->message("Configuration saved",messageTimeout);
    }
    else
    {
      QMessageBox::warning(0,"Warning",
          "Cannot open file "+fn+" for writing. Nothing saved!","ok");
    }
  }
}

void MainWidget::resetConfig()
{
  if (QMessageBox::question(this,"Reset settings to their defaults",
                "Do you wish to reset the "
                "current configuration settings to their factory defaults?",
                QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
  {
    // initialize config settings
    Config::instance()->init();
#if defined(Q_OS_MACX)
    setDotPath();
#endif

    m_configFileName = "";
    setConfigSaved(FALSE);
    statusBar()->message("Configuration settings reset to their defaults",messageTimeout);
  }
}

void MainWidget::setConfigSaved(bool enable)
{
  m_configSaved=enable;
  if (m_configSaved)
  {
    m_saveStatus->setText("Status: saved");
  }
  else
  {
    m_saveStatus->setText("Status: <font color=\"red\">not saved</font>");
  }
  updateRunnable(m_workingDir->text());
}

void MainWidget::runDoxygen()
{
  if (!m_running)
  {
    QString doxygenPath; 
#if defined(Q_OS_MACX)
    doxygenPath = getResourcePath();
#else
    // TODO: get the right value if not in the search path
#endif
    //printf("Trying to run doxygen from path %s\n",doxygenPath.data());

    m_runProcess->setCommunication(QProcess::Stdout|
                                   QProcess::Stderr|
                                   QProcess::DupStderr
                                  );
    m_runProcess->setWorkingDirectory(QDir(m_workingDir->text()));
    m_runProcess->clearArguments();
    m_runProcess->addArgument(doxygenPath+"doxygen");
    m_runProcess->addArgument("-b");
    m_runProcess->addArgument(m_configFileName);
    m_outputLog->clear();
    if (!m_runProcess->start())
    {
      m_outputLog->append(QString("*** Failed to run doxygen\n"));
    }
    else
    {
      m_running=TRUE;
      m_run->setText("Cancel");
      m_runStatus->setText("Status: running");
      m_timer->start(1000);
    }
  }
  else
  {
    m_running=FALSE;
    m_run->setText("Start");
    m_runStatus->setText("Status: not running");
    m_runProcess->kill();
    m_timer->stop();
    updateRunnable(m_workingDir->text());
  }
}

void MainWidget::readStdout()
{
  m_outputLog->append(QString(m_runProcess->readStdout()));
}

void MainWidget::runComplete()
{
  if (m_running)
  {
    m_outputLog->append(QString("*** Doxygen has finished\n"));
  }
  else
  {
    m_outputLog->append(QString("*** Cancelled by user\n"));
  }
  m_run->setText("Start");
  m_runStatus->setText("Status: not running");
  m_running=FALSE;
  updateRunnable(m_workingDir->text());
}

void MainWidget::about()
{
  QString msg;
  QTextStream t(&msg,IO_WriteOnly);
  t << QString("<qt><center>A tool to configure and run doxygen version ")+versionString+
       " on your source files.</center><p><br>"
       "<center>Written by<br> Dimitri van Heesch<br>&copy; 2000-2005</center><p>"
       "</qt>";
  QMessageBox::about(this,"Doxygen GUI",msg);
}

void MainWidget::manual()
{
#if defined(Q_OS_MACX)
  const char *urlBytes = "http://www.doxygen.org/manual.html";
  CFURLRef url = CFURLCreateWithBytes (
      NULL, // allocator 
      (UInt8*)urlBytes, // bytes 
      strlen(urlBytes), // length
      kCFStringEncodingASCII, // encoding
      NULL // baseURL
      );
  LSOpenCFURLRef(url,0);
  CFRelease(url);
#endif
}

void MainWidget::addRecentFile(const QString &file)
{
  QStringList::Iterator it = m_recentFiles.find(file);
  if (it!=m_recentFiles.end())
  {
    // already found => move to top
    m_recentFiles.remove(it);
    m_recentFiles.prepend(file);
  }
  else
  {
    // not found
    if (m_recentFiles.count() < 10) // append
    {
      m_recentFiles.prepend(file);
    }
    else // add + drop last item
    {
      m_recentFiles.remove(m_recentFiles.last());
      m_recentFiles.prepend(file);
    }
  }

  m_recentMenu->clear();
  int i=0;
  for ( it = m_recentFiles.begin(); it != m_recentFiles.end(); ++it, ++i )
  {
    m_recentMenu->insertItem(*it);
    m_settings.writeEntry(QString().sprintf("/doxywizard/recent/config%d",i),*it);
  }
}


void MainWidget::openRecent(int id)
{
  loadConfigFromFile(m_recentMenu->text(id)); 
}

void MainWidget::quit()
{
  if (!m_configSaved &&
      QMessageBox::warning( this, "DoxyWizard", "Quit and lose changes?",
        QMessageBox::Yes, QMessageBox::No)==QMessageBox::No
     ) return; // Quit cancelled by user

  qApp->quit();
}

void MainWidget::saveLog()
{
  QString fn = QFileDialog::getSaveFileName(m_workingDir->text()+"/doxygen_log.txt",QString::null,this);
  if (!fn.isEmpty())
  {
    QFile f(fn);
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
      t << m_outputLog->text();
      statusBar()->message("Output log saved",messageTimeout);
    }
    else
    {
      QMessageBox::warning(0,"Warning",
          "Cannot open file "+fn+" for writing. Nothing saved!","ok");
    }
  }
}

//==========================================================================

int main(int argc,char **argv)
{
  QApplication a(argc,argv);
  MainWidget main;
  if (argc==2 && argv[1][0]!='-') // name of config file as an argument
  {
    main.loadConfigFromFile(argv[1]);
  }
  else if (argc>1)
  {
    printf("Usage: %s [config file]\n",argv[0]);
    exit(1);
  }
  a.setMainWidget(&main);
  main.show();
  return a.exec();
}


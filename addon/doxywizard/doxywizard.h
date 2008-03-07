#ifndef DOXYWIZARD_H
#define DOXYWIZARD_H

#include <qvbox.h>
#include <qtabdialog.h>
#include <qmainwindow.h>
#include <qsettings.h>
#include <qstringlist.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;
class QButtonGroup;
class QTextEdit;
class QLabel;
class QProcess;
class QTimer;
class QPopupMenu;

enum OptLang { Lang_Cpp, Lang_C, Lang_Java, Lang_CS };
enum HtmlStyle { HS_Plain, HS_TreeView, HS_CHM };
enum TexStyle { TS_PDFHyper, TS_PDF, TS_PS };
enum DiagramMode { DM_None, DM_Builtin, DM_Dot };

class Step1 : public QWidget
{
    Q_OBJECT

  public:
    Step1(QWidget *parent);

    QString getProjectName() const;
    QString getProjectNumber() const;
    QString getSourceDir() const;
    bool scanRecursively() const;
    QString getDestinationDir() const;

    void setProjectName(const QString &name);
    void setProjectNumber(const QString &num);
    void setSourceDir(const QString &dir);
    void setRecursiveScan(bool enable);
    void setDestinationDir(const QString &dir);

  private slots:
    void selectSourceDir(); 
    void selectDestinationDir(); 
    
  private:
    QLineEdit *m_projName;
    QLineEdit *m_projNumber;
    QLineEdit *m_sourceDir;
    QLineEdit *m_destDir;
    QCheckBox *m_recursive;
    QPushButton *m_srcSelectDir;
    QPushButton *m_dstSelectDir;
};

class Step2 : public QWidget
{
  public:
    Step2(QWidget *parent);
    bool crossReferencingEnabled() const;
    void enableCrossReferencing();

    bool extractAll() const;
    bool crossReferencing() const;
    OptLang optimizeFor() const;

    void setExtractAll(bool enable);
    void setCrossReferencing(bool enable);
    void setOptimizeFor(OptLang lang);

  private:
    QButtonGroup *m_extractMode;
    QButtonGroup *m_optimizeLang;
    QCheckBox *m_crossRef;
};

class Step3 : public QWidget
{
    Q_OBJECT

  public:
    Step3(QWidget *parent);

    bool htmlEnabled() const;
    bool latexEnabled() const;
    bool manEnabled() const;
    bool rtfEnabled() const;
    bool xmlEnabled() const;
    bool searchEnabled() const;
    HtmlStyle htmlStyle() const;
    TexStyle texStyle() const;

    void setHtmlEnabled(bool);
    void setLatexEnabled(bool);
    void setManEnabled(bool);
    void setRtfEnabled(bool);
    void setXmlEnabled(bool);
    void setSearchEnabled(bool);
    void setHtmlStyle(HtmlStyle);
    void setTexStyle(TexStyle);


  private slots:
    void latexStateChange(int state);
    void htmlStateChange(int state);

  private:
    QButtonGroup *m_texOptions;
    QVBox *m_texOptionBox;
    QButtonGroup *m_htmlOptions;
    QVBox *m_htmlOptionBox;
    QCheckBox *m_htmlEnabled;
    QCheckBox *m_latexEnabled;
    QCheckBox *m_manEnabled;
    QCheckBox *m_rtfEnabled;
    QCheckBox *m_xmlEnabled;
    QCheckBox *m_searchEnabled;
};

class Step4 : public QWidget
{
    Q_OBJECT

  public:
    Step4(QWidget *parent);
    //void disableCallGraphs();
    //bool callGraphEnabled() const;

    DiagramMode diagramMode() const;
    bool classDiagram() const;
    bool collaborationDiagram() const;
    bool includeGraph() const;
    bool includedByGraph() const;
    bool inheritanceGraph() const;
    bool callGraph() const;

    void setDiagramMode(DiagramMode mode);
    void setClassDiagram(bool enable);
    void setCollaborationDiagram(bool enable);
    void setIncludeGraph(bool enable);
    void setIncludedByGraph(bool enable);
    void setInheritanceGraph(bool enable);
    void setCallGraph(bool enable);

  private slots:
    void diagramModeChanged(int);
    
  private:
    QButtonGroup *m_diagramMode;
    QVBox *m_dotOptions;
    QCheckBox *m_dotClass;
    QCheckBox *m_dotCollaboration;
    QCheckBox *m_dotInclude;
    QCheckBox *m_dotIncludedBy;
    QCheckBox *m_dotInheritance;
    QCheckBox *m_dotCall;
};

class Wizard : public QTabDialog
{
    Q_OBJECT

  public:
    Wizard(QWidget *parent);
    //void disableCallGraphs();

    // step1
    QString getProjectName() const    { return m_step1->getProjectName(); }
    QString getProjectNumber() const  { return m_step1->getProjectNumber(); }
    QString getSourceDir() const      { return m_step1->getSourceDir(); }
    bool scanRecursively() const      { return m_step1->scanRecursively(); }
    QString getDestinationDir() const { return m_step1->getDestinationDir(); }

    void setProjectName(const QString &name)   { m_step1->setProjectName(name); }
    void setProjectNumber(const QString &num)  { m_step1->setProjectNumber(num); }
    void setSourceDir(const QString &dir)      { m_step1->setSourceDir(dir); }
    void setRecursiveScan(bool enable)         { m_step1->setRecursiveScan(enable); }
    void setDestinationDir(const QString &dir) { m_step1->setDestinationDir(dir); }

    // step2
    bool extractAll() const       { return m_step2->extractAll();       }
    bool crossReferencing() const { return m_step2->crossReferencing(); }
    OptLang optimizeFor() const   { return m_step2->optimizeFor();      }

    void setExtractAll(bool enable)       { m_step2->setExtractAll(enable);       }
    void setCrossReferencing(bool enable) { m_step2->setCrossReferencing(enable); }
    void setOptimizeFor(OptLang lang)     { m_step2->setOptimizeFor(lang);        }
    
    // step3
    bool htmlEnabled() const      { return m_step3->htmlEnabled();   }
    bool latexEnabled() const     { return m_step3->latexEnabled();  }
    bool manEnabled() const       { return m_step3->manEnabled();    }
    bool rtfEnabled() const       { return m_step3->rtfEnabled();    }
    bool xmlEnabled() const       { return m_step3->xmlEnabled();    }
    bool searchEnabled() const    { return m_step3->searchEnabled(); }
    HtmlStyle htmlStyle() const   { return m_step3->htmlStyle();     }
    TexStyle texStyle() const     { return m_step3->texStyle();      }

    void setHtmlEnabled(bool enable)   { m_step3->setHtmlEnabled(enable);   }
    void setLatexEnabled(bool enable)  { m_step3->setLatexEnabled(enable);  }
    void setManEnabled(bool enable)    { m_step3->setManEnabled(enable);    }
    void setRtfEnabled(bool enable)    { m_step3->setRtfEnabled(enable);    }
    void setXmlEnabled(bool enable)    { m_step3->setXmlEnabled(enable);    }
    void setSearchEnabled(bool enable) { m_step3->setSearchEnabled(enable); }
    void setHtmlStyle(HtmlStyle style) { m_step3->setHtmlStyle(style);      }
    void setTexStyle(TexStyle style)   { m_step3->setTexStyle(style);       }

    // step4
    DiagramMode diagramMode() const   { return m_step4->diagramMode(); }
    bool classDiagram() const         { return m_step4->classDiagram(); }
    bool collaborationDiagram() const { return m_step4->collaborationDiagram(); }
    bool includeGraph() const         { return m_step4->includeGraph(); }
    bool includedByGraph() const      { return m_step4->includedByGraph(); }
    bool inheritanceGraph() const     { return m_step4->inheritanceGraph(); }
    bool callGraph() const            { return m_step4->callGraph(); }
    
    void setDiagramMode(DiagramMode mode)     { m_step4->setDiagramMode(mode); }
    void setClassDiagram(bool enable)         { m_step4->setClassDiagram(enable); }
    void setCollaborationDiagram(bool enable) { m_step4->setCollaborationDiagram(enable); }
    void setIncludeGraph(bool enable)         { m_step4->setIncludeGraph(enable); }
    void setIncludedByGraph(bool enable)      { m_step4->setIncludedByGraph(enable); }
    void setInheritanceGraph(bool enable)     { m_step4->setInheritanceGraph(enable); }
    void setCallGraph(bool enable)            { m_step4->setCallGraph(enable); }

  private:
    Step1 *m_step1;
    Step2 *m_step2;
    Step3 *m_step3;
    Step4 *m_step4;
};

class MainWidget : public QMainWindow
{
    Q_OBJECT
  public:
    MainWidget(QWidget *parent=0);
   ~MainWidget();
    void loadConfigFromFile(const QString &);

  private slots:
    void launchWizard();
    void launchExpert();
    void loadConfigFile();
    void selectWorkingDir();
    void updateRunnable(const QString &newDirName);
    void saveDefaults();
    void saveConfigFile();
    void runDoxygen();
    void readStdout();
    void runComplete();
    void about();
    void manual();
    void resetConfig();
    void openRecent(int);
    void quit();
    void saveLog();
    
  private:
    void setConfigSaved(bool);
    void addRecentFile(const QString &);

    QPushButton *m_wizard;
    QPushButton *m_expert;
    QPushButton *m_load;
    QPushButton *m_save;
    QPushButton *m_defaults;
    QLineEdit   *m_workingDir;
    QPushButton *m_selWorkingDir;
    QPushButton *m_run;
    QPushButton *m_saveLog;
    QLabel      *m_runStatus;
    QProcess    *m_runProcess;
    QLabel      *m_saveStatus;
    QLabel      *m_outputHeading;
    QTextEdit   *m_outputLog;
    QTimer      *m_timer;
    QSettings    m_settings;
    QPopupMenu  *m_recentMenu;
    bool m_configSaved;
    bool m_running;
    QString      m_configFileName;
    QStringList  m_recentFiles;
};

#endif

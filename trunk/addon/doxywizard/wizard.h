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

#ifndef WIZARD_H
#define WIZARD_H

#include <QSplitter>
#include <QHash>

class Input;
class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QGroupBox;
class QButtonGroup;
class Wizard;

enum OptLang     { Lang_Cpp, Lang_C, Lang_Java, Lang_CS };
enum HtmlStyle   { HS_Plain, HS_TreeView, HS_CHM };
enum TexStyle    { TS_PDFHyper, TS_PDF, TS_PS };
enum DiagramMode { DM_None, DM_Builtin, DM_Dot };

class Step1 : public QWidget
{
    Q_OBJECT

  public:
    Step1(Wizard *parent,const QHash<QString,Input*> &modelData);
    void init();

  private slots:
    void selectSourceDir(); 
    void selectDestinationDir(); 
    void setProjectName(const QString &name);
    void setProjectNumber(const QString &num);
    void setSourceDir(const QString &dir);
    void setDestinationDir(const QString &dir);
    void setRecursiveScan(int);

  private:
    QLineEdit *m_projName;
    QLineEdit *m_projNumber;
    QLineEdit *m_sourceDir;
    QLineEdit *m_destDir;
    QCheckBox *m_recursive;
    QPushButton *m_srcSelectDir;
    QPushButton *m_dstSelectDir;
    Wizard    *m_wizard;
    const QHash<QString,Input *> &m_modelData;
};

class Step2 : public QWidget
{
    Q_OBJECT
  public:
    Step2(Wizard *parent,const QHash<QString,Input*> &modelData);
    void init();

  private slots:
    void optimizeFor(int choice);
    void extractMode(int choice);
    void changeCrossRefState(int choice);

  private:
    QGroupBox    *m_extractMode;
    QGroupBox    *m_optimizeLang;
    QButtonGroup *m_extractModeGroup;
    QButtonGroup *m_optimizeLangGroup;
    QCheckBox    *m_crossRef;
    Wizard       *m_wizard;
    const QHash<QString,Input *> &m_modelData;
};

class Step3 : public QWidget
{
    Q_OBJECT

  public:
    Step3(Wizard *parent,const QHash<QString,Input*> &modelData);
    void init();

  private slots:
    void setHtmlEnabled(bool);
    void setLatexEnabled(bool);
    void setManEnabled(int);
    void setRtfEnabled(int);
    void setXmlEnabled(int);
    void setSearchEnabled(int);
    void setHtmlOptions(int);
    void setLatexOptions(int);

  private:
    QGroupBox *m_texOptions;
    QButtonGroup *m_texOptionsGroup;
    QGroupBox *m_htmlOptions;
    QButtonGroup *m_htmlOptionsGroup;
    QCheckBox *m_htmlEnabled;
    QCheckBox *m_latexEnabled;
    QCheckBox *m_manEnabled;
    QCheckBox *m_rtfEnabled;
    QCheckBox *m_xmlEnabled;
    QCheckBox *m_searchEnabled;
    Wizard    *m_wizard;
    const QHash<QString,Input *> &m_modelData;
};

class Step4 : public QWidget
{
    Q_OBJECT

  public:
    Step4(Wizard *parent,const QHash<QString,Input*> &modelData);
    void init();

  private slots:
    void diagramModeChanged(int);
    void setClassGraphEnabled(int state);
    void setCollaborationGraphEnabled(int state);
    void setGraphicalHierarchyEnabled(int state);
    void setIncludeGraphEnabled(int state);
    void setIncludedByGraphEnabled(int state);
    void setCallGraphEnabled(int state);
    void setCallerGraphEnabled(int state);

  private:
    QGroupBox *m_diagramMode;
    QButtonGroup *m_diagramModeGroup;
    QGroupBox *m_dotGroup;
    QCheckBox *m_dotClass;
    QCheckBox *m_dotCollaboration;
    QCheckBox *m_dotInclude;
    QCheckBox *m_dotIncludedBy;
    QCheckBox *m_dotInheritance;
    QCheckBox *m_dotCall;
    QCheckBox *m_dotCaller;
    Wizard    *m_wizard;
    const QHash<QString,Input *> &m_modelData;
};

class Wizard : public QSplitter
{
    Q_OBJECT
  public:
    Wizard(const QHash<QString,Input*> &modelData, QWidget *parent=0);
   ~Wizard();

  public slots:
    void refresh();

  private slots:
    void activateTopic(QTreeWidgetItem *item,QTreeWidgetItem *);
    void nextTopic();
    void prevTopic();

  private:
    const QHash<QString,Input *> &m_modelData;
    QTreeWidget    *m_treeWidget;
    QStackedWidget *m_topicStack;
    Step1          *m_step1;
    Step2          *m_step2;
    Step3          *m_step3;
    Step4          *m_step4;
    QPushButton    *m_next;
    QPushButton    *m_prev;
};

#endif

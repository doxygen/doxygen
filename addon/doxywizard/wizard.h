/******************************************************************************
 *
 * Copyright (C) 1997-2025 by Dimitri van Heesch.
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
#include <QDialog>
#include <QImage>

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
class QTabWidget;
class Wizard;
class QLabel;

class TuneColorDialog : public QDialog
{
    Q_OBJECT

  public:
    TuneColorDialog(int hue,int sat,int gamma,QWidget *parent=nullptr);
    int getHue() const;
    int getSaturation() const;
    int getGamma() const;

  private slots:
    void updateImage(int hue,int sat,int val);
    void tabChanged(int);

  private:
    QImage m_imageLight;
    QImage m_imageDark;
    QLabel *m_imageLabLight = nullptr;
    QLabel *m_imageLabDark = nullptr;
    QTabWidget *m_tab = nullptr;
    int m_hue = 0;
    int m_sat = 0;
    int m_gam = 0;
};

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    enum Mode { Hue, Saturation, Gamma };
    ColorPicker(Mode m);
    ~ColorPicker();

public slots:
    void setCol(int h, int s, int g);
    //void setCol(int h, int s);

signals:
    void newHsv(int h, int s, int g);

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int y2hue(int y);
    int y2sat(int y);
    int y2gam(int y);
    int hue2y(int hue);
    int sat2y(int sat);
    int gam2y(int gamma);
    void setHue(int v);
    void setSat(int v);
    void setGam(int v);

    QPixmap *m_pix = nullptr;
    Mode m_mode = Hue;
    int m_gam = 0;
    int m_hue = 0;
    int m_sat = 0;

};


class Step1 : public QWidget
{
    Q_OBJECT

  public:
    Step1(Wizard *parent,const QHash<QString,Input*> &modelData);
    void init();

  private slots:
    void selectSourceDir();
    void selectDestinationDir();
    void selectProjectIcon();
    void setProjectName(const QString &name);
    void setProjectBrief(const QString &desc);
    void setProjectNumber(const QString &num);
    void setSourceDir(const QString &dir);
    void setDestinationDir(const QString &dir);
    void setRecursiveScan(int);

  private:
    QLineEdit *m_projName = nullptr;
    QLineEdit *m_projBrief = nullptr;
    QLineEdit *m_projNumber = nullptr;
    QLineEdit *m_sourceDir = nullptr;
    QLineEdit *m_destDir = nullptr;
    QLabel    *m_projIconLab = nullptr;
    QCheckBox *m_recursive = nullptr;
    QPushButton *m_srcSelectDir = nullptr;
    QPushButton *m_dstSelectDir = nullptr;
    Wizard    *m_wizard = nullptr;
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
    QGroupBox    *m_extractMode = nullptr;
    QGroupBox    *m_optimizeLang = nullptr;
    QButtonGroup *m_extractModeGroup = nullptr;
    QButtonGroup *m_optimizeLangGroup = nullptr;
    QCheckBox    *m_crossRef = nullptr;
    Wizard       *m_wizard = nullptr;
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
    void setDocbookEnabled(int);
    void setSearchEnabled(int);
    void setHtmlOptions(int);
    void setLatexOptions(int);
    void tuneColorDialog();

  private:
    QGroupBox *m_texOptions = nullptr;
    QButtonGroup *m_texOptionsGroup = nullptr;
    QGroupBox *m_htmlOptions = nullptr;
    QButtonGroup *m_htmlOptionsGroup = nullptr;
    QCheckBox *m_htmlEnabled = nullptr;
    QCheckBox *m_latexEnabled = nullptr;
    QCheckBox *m_manEnabled = nullptr;
    QCheckBox *m_rtfEnabled = nullptr;
    QCheckBox *m_xmlEnabled = nullptr;
    QCheckBox *m_docbookEnabled = nullptr;
    QCheckBox *m_searchEnabled = nullptr;
    QPushButton *m_tuneColor = nullptr;
    Wizard    *m_wizard = nullptr;
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
    QGroupBox *m_diagramMode = nullptr;
    QButtonGroup *m_diagramModeGroup = nullptr;
    QGroupBox *m_dotGroup = nullptr;
    QCheckBox *m_dotClass = nullptr;
    QCheckBox *m_dotCollaboration = nullptr;
    QCheckBox *m_dotInclude = nullptr;
    QCheckBox *m_dotIncludedBy = nullptr;
    QCheckBox *m_dotInheritance = nullptr;
    QCheckBox *m_dotCall = nullptr;
    QCheckBox *m_dotCaller = nullptr;
    Wizard    *m_wizard = nullptr;
    const QHash<QString,Input *> &m_modelData;
};

class Wizard : public QSplitter
{
    Q_OBJECT
  public:
    Wizard(const QHash<QString,Input*> &modelData, QWidget *parent=nullptr);
   ~Wizard();

  public slots:
    void refresh();

  private slots:
    void activateTopic(QTreeWidgetItem *item,QTreeWidgetItem *);
    void nextTopic();
    void prevTopic();

  signals:
    void done();

  private:
    const QHash<QString,Input *> &m_modelData;
    QTreeWidget    *m_treeWidget = nullptr;
    QStackedWidget *m_topicStack = nullptr;
    Step1          *m_step1 = nullptr;
    Step2          *m_step2 = nullptr;
    Step3          *m_step3 = nullptr;
    Step4          *m_step4 = nullptr;
    QPushButton    *m_next = nullptr;
    QPushButton    *m_prev = nullptr;
};

#endif

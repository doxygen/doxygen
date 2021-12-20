/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
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
class QImage;
class QLabel;

class TuneColorDialog : public QDialog
{
    Q_OBJECT

  public:
    TuneColorDialog(int hue,int sat,int gamma,QWidget *parent=0);
    int getHue() const;
    int getSaturation() const;
    int getGamma() const;

  private slots:
    void updateImage(int hue,int sat,int val);

  private:
    QImage *m_image = 0;
    QLabel *m_imageLab = 0;
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

    QPixmap *m_pix = 0;
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
    QLineEdit *m_projName = 0;
    QLineEdit *m_projBrief = 0;
    QLineEdit *m_projNumber = 0;
    QLineEdit *m_sourceDir = 0;
    QLineEdit *m_destDir = 0;
    QLabel    *m_projIconLab = 0;
    QCheckBox *m_recursive = 0;
    QPushButton *m_srcSelectDir = 0;
    QPushButton *m_dstSelectDir = 0;
    Wizard    *m_wizard = 0;
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
    QGroupBox    *m_extractMode = 0;
    QGroupBox    *m_optimizeLang = 0;
    QButtonGroup *m_extractModeGroup = 0;
    QButtonGroup *m_optimizeLangGroup = 0;
    QCheckBox    *m_crossRef = 0;
    Wizard       *m_wizard = 0;
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
    QGroupBox *m_texOptions = 0;
    QButtonGroup *m_texOptionsGroup = 0;
    QGroupBox *m_htmlOptions = 0;
    QButtonGroup *m_htmlOptionsGroup = 0;
    QCheckBox *m_htmlEnabled = 0;
    QCheckBox *m_latexEnabled = 0;
    QCheckBox *m_manEnabled = 0;
    QCheckBox *m_rtfEnabled = 0;
    QCheckBox *m_xmlEnabled = 0;
    QCheckBox *m_docbookEnabled = 0;
    QCheckBox *m_searchEnabled = 0;
    QPushButton *m_tuneColor = 0;
    Wizard    *m_wizard = 0;
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
    QGroupBox *m_diagramMode = 0;
    QButtonGroup *m_diagramModeGroup = 0;
    QGroupBox *m_dotGroup = 0;
    QCheckBox *m_dotClass = 0;
    QCheckBox *m_dotCollaboration = 0;
    QCheckBox *m_dotInclude = 0;
    QCheckBox *m_dotIncludedBy = 0;
    QCheckBox *m_dotInheritance = 0;
    QCheckBox *m_dotCall = 0;
    QCheckBox *m_dotCaller = 0;
    Wizard    *m_wizard = 0;
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

  signals:
    void done();

  private:
    const QHash<QString,Input *> &m_modelData;
    QTreeWidget    *m_treeWidget = 0;
    QStackedWidget *m_topicStack = 0;
    Step1          *m_step1 = 0;
    Step2          *m_step2 = 0;
    Step3          *m_step3 = 0;
    Step4          *m_step4 = 0;
    QPushButton    *m_next = 0;
    QPushButton    *m_prev = 0;
};

#endif

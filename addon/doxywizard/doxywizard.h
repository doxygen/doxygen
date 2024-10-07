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

#ifndef DOXYWIZARD_H
#define DOXYWIZARD_H

#include <QMainWindow>
#include <QSettings>
#include <QStringList>

class Expert;
class Wizard;
class QLabel;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QTextBrowser;
class QMenu;
class QProcess;
class QTimer;
class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    static MainWindow &instance();
    void loadConfigFromFile(const QString &fileName);
    void loadSettings();
    void saveSettings();
    void closeEvent(QCloseEvent *event);
    QString configFileName() const { return m_fileName; }
    void updateTitle();
    // access routines for outputLog pane
    void outputLogStart();
    void outputLogText(QString text);
    void outputLogFinish();

  public slots:
    void manual();
    void about();
    void openConfig();
    bool saveConfig();
    bool saveConfigAs();
    void makeDefaults();
    void resetToDefaults();
    void selectTab(int);
    void quit();

  private slots:
    void openRecent(QAction *action);
    void selectWorkingDir();
    void updateWorkingDir();
    void runDoxygen();
    void runDoxygenMenu();
    void readStdout();
    void runComplete();
    void showHtmlOutput();
    void saveLog();
    void showSettings();
    void configChanged();
    void clearRecent();
    void selectRunTab();

  private:
    MainWindow();
    void saveConfig(const QString &fileName);
    void addRecentFile(const QString &fileName);
    void addRecentFileList(const QString &fileName);
    void updateRecentFile(void);
    void updateConfigFileName(const QString &fileName);
    void setWorkingDir(const QString &dirName);
    void updateLaunchButtonState();
    bool discardUnsavedChanges(bool saveOption=true);

    QLineEdit *m_workingDir;
    QLineEdit *m_runOptions;
    QPushButton *m_selWorkingDir;
    QPushButton *m_run;
    QAction *m_runMenu;
    QPushButton *m_saveLog;
    QCheckBox   *m_showCondensedSettings;
    QPushButton *m_launchHtml;
    QPushButton *m_launchPdf;
    QTextBrowser *m_outputLog;
    QLabel *m_runStatus;
    Expert *m_expert;
    Wizard *m_wizard;
    QWidget *m_runTab;
    QString m_fileName;
    QSettings m_settings;
    QMenu *m_recentMenu;
    QStringList m_recentFiles;
    QAction *m_resetDefault;
    QAction *m_clearRecent;
    QProcess *m_runProcess;
    QTimer *m_timer;
    QTabWidget *m_tabs;
    int m_outputLogTextCount = 0;
    bool m_running;
    bool m_modified;
};

/*! \brief This class serves as a namespace for global variables used by the doxygen wizard.
 *
 *  All fields in this class are public and static, so they can be used directly.
 */
class DoxygenWizard
{
  public:
    static bool debugFlag;
};
#endif

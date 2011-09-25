#ifndef DOXYWIZARD_H
#define DOXYWIZARD_H

#include <QMainWindow>
#include <QSettings>
#include <QStringList>

class Expert;
class Wizard;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QMenu;
class QProcess;
class QTimer;

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
    void readStdout();
    void runComplete();
    void showHtmlOutput();
    void saveLog();
    void showSettings();
    void configChanged();
    void clearRecent();
    
  private:
    MainWindow();
    void saveConfig(const QString &fileName);
    void addRecentFile(const QString &fileName);
    void updateConfigFileName(const QString &fileName);
    void setWorkingDir(const QString &dirName);
    void updateLaunchButtonState();
    bool discardUnsavedChanges(bool saveOption=true);

    QLineEdit *m_workingDir;
    QPushButton *m_selWorkingDir;
    QPushButton *m_run;
    QPushButton *m_saveLog;
    QPushButton *m_launchHtml;
    QPushButton *m_launchPdf;
    QTextEdit *m_outputLog;
    QLabel *m_runStatus;
    Expert *m_expert;
    Wizard *m_wizard;
    QString m_fileName;
    QSettings m_settings;
    QMenu *m_recentMenu;
    QStringList m_recentFiles;
    QProcess *m_runProcess;
    QTimer *m_timer;
    bool m_running;
    bool m_modified;
};

#endif

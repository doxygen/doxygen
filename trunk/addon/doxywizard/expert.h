#ifndef EXPERT_H
#define EXPERT_H

#include <QSplitter>
#include <QDomElement>
#include <QHash>

class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QSettings;
class QTextEdit;
class QTextCodec;
class QPushButton;
class Input;

class Expert : public QSplitter
{
    Q_OBJECT

  public:
    Expert();
   ~Expert();
    void loadSettings(QSettings *);
    void saveSettings(QSettings *);
    void loadConfig(const QString &fileName);
    bool writeConfig(QTextStream &t,bool brief);
    QByteArray saveInnerState () const;
    bool restoreInnerState ( const QByteArray & state );
    const QHash<QString,Input*> &modelData() const { return m_options; }
    void resetToDefaults();
    bool htmlOutputPresent(const QString &workingDir) const;
    bool pdfOutputPresent(const QString &workingDir) const;
    QString getHtmlOutputIndex(const QString &workingDir) const;
  
  public slots:
    void activateTopic(QTreeWidgetItem *,QTreeWidgetItem *);
    QWidget *createTopicWidget(QDomElement &elem);

  private slots:
    void showHelp(Input *);
    void nextTopic();
    void prevTopic();

  signals:
    void changed();

  private:
    void createTopics(const QDomElement &);
    void saveTopic(QTextStream &t,QDomElement &elem,QTextCodec *codec,bool brief);

    QSplitter               *m_splitter;
    QTextEdit               *m_helper;
    QTreeWidget             *m_treeWidget;
    QStackedWidget          *m_topicStack;
    QHash<QString,QWidget *> m_topics;
    QHash<QString,QObject *> m_optionWidgets;
    QHash<QString,Input *>   m_options;
    QPushButton             *m_next;
    QPushButton             *m_prev;
    QDomElement              m_rootElement;
    bool                     m_inShowHelp;
};

#endif

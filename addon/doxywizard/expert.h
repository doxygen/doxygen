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

#ifndef EXPERT_H
#define EXPERT_H

#include <QSplitter>
#include <QDomElement>
#include <QHash>

#include "docintf.h"
#include "adapter.h"

class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QSettings;
class QTextBrowser;
class QPushButton;
class Input;

class Expert : public QSplitter, public DocIntf
{
    Q_OBJECT

  public:
    Expert();
   ~Expert();
    void loadSettings(QSettings *);
    void saveSettings(QSettings *);
    void loadConfig(const QString &fileName);
    bool writeConfig(QTextStream &t,bool brief,bool condensed, bool convert);
    QByteArray saveInnerState () const;
    bool restoreInnerState ( const QByteArray & state );
    const QHash<QString,Input*> &modelData() const { return m_options; }
    void resetToDefaults();
    bool htmlOutputPresent(const QString &workingDir) const;
    bool pdfOutputPresent(const QString &workingDir) const;
    QString getHtmlOutputIndex(const QString &workingDir) const;

    // DocIntf methods
    void setHeader(const char *name);
    void add(const char *name,const char *doc);

  public slots:
    void activateTopic(QTreeWidgetItem *,QTreeWidgetItem *);
    QWidget *createTopicWidget(QDomElement &elem);
    void refresh();

  private slots:
    void showHelp(Input *);
    void nextTopic();
    void prevTopic();

  signals:
    void changed();
    void done();

  private:
    void createTopics(const QDomElement &);
    void saveTopic(QTextStream &t,QDomElement &elem,TextCodecAdapter *codec,bool brief,bool dondensed,bool convert);

    QSplitter               *m_splitter;
    QTextBrowser            *m_helper;
    QTreeWidget             *m_treeWidget;
    QStackedWidget          *m_topicStack;
    QHash<QString,QWidget *> m_topics;
    QHash<QString,QObject *> m_optionWidgets;
    QHash<QString,Input *>   m_options;
    QPushButton             *m_next;
    QPushButton             *m_prev;
    QDomElement              m_rootElement;
    bool                     m_inShowHelp;
    QString                  m_header;
};

#endif

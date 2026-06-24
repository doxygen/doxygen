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

#include <QWidget>
#include <QDomElement>
#include <QHash>
#include <QList>

#include "docintf.h"
#include "adapter.h"

class QTreeWidget;
class QTreeWidgetItem;
class QScrollArea;
class QVBoxLayout;
class QLineEdit;
class QSplitter;
class QSettings;
class QTextStream;
class QLabel;
class QFrame;
class QPushButton;
class QTimer;
class Input;

class Expert : public QWidget, public DocIntf
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
    void dump();
    void setDocumentationVisibility(bool hidden);

    // DocIntf methods
    void setHeader(const char *name);
    void add(const char *name,const char *doc);

  public slots:
    void createMessagesList(QDomElement &elem);
    void refresh();

  private slots:
    void filterChanged(const QString &text);
    void groupSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void prevGroup();
    void nextGroup();

  signals:
    void changed();
    void done();

  private:
    // Per-option bookkeeping
    struct OptionEntry
    {
      QString           groupName;
      QString           id;
      QString           docs;
      QWidget          *card;             ///< card widget (docs label + control holder)
      QLabel           *docsLabel;        ///< docs label inside the card
      QFrame           *sep;              ///< separator settings inside the card
      QTreeWidgetItem  *treeItem;         ///< child item in the left tree (nullptr when showAll)
      Input            *input;            ///< the Input object
      bool              labelHighlighted = false; ///< true when docsLabel shows highlighted text
    };

    // Per-group bookkeeping
    struct GroupEntry
    {
      QString            name;
      QString            translatedName;
      QString            docs;
      QDomElement        domElement;  ///< stored for lazy card creation
      bool               cardsCreated = false;
      QWidget           *section;    ///< the group section container widget
      QLabel            *header;     ///< the group name header label
      QTreeWidgetItem   *treeItem;
      QList<OptionEntry> options;
    };

    void createGroups(const QDomElement &rootElem);
    void createOptionCard(GroupEntry &group, const QDomElement &child, QString &mode);
    void ensureGroupCardsCreated(GroupEntry &group, QString &mode);
    void ensureAllGroupsCreated(QString &mode);
    void wireDependencies();
    void activateGroup(int index);
    void saveTopic(QTextStream &t, QDomElement &elem, TextCodecAdapter *codec,
                   bool brief, bool condensed, bool convert);
    QString getDocsForNode(const QDomElement &child, QString &mode) const;

    QSplitter              *m_splitter;
    QTreeWidget            *m_treeWidget;
    QLineEdit              *m_searchBox;
    QScrollArea            *m_scrollArea;
    QWidget                *m_rightContainer;
    QVBoxLayout            *m_rightLayout;
    QWidget                *m_navBar;
    QPushButton            *m_prevButton;
    QPushButton            *m_nextButton;
    QTimer                 *m_filterTimer;
    int                     m_currentGroupIndex = -1;

    QList<GroupEntry>       m_groups;
    QHash<QString,Input *>  m_options;
    QHash<QString,QString>  m_messages;
    QDomElement             m_rootElement;
    QString                 m_header;
};

#endif

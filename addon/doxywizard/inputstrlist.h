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

#ifndef INPUTSTRLIST_H
#define INPUTSTRLIST_H

#include "input.h"

#include <QObject>
#include <QStringList>

class QLabel;
class QLineEdit;
class QPushButton;
class QListWidget;
class QGridLayout;
class QAction;

class InputStrList : public QObject, public Input
{
  Q_OBJECT

  public:
    enum ListMode { ListString  = 0,
                    ListFile    = 1,
                    ListDir     = 2,
                    ListFileDir = ListFile | ListDir
                  };

    InputStrList( QGridLayout *layout,int &row,
                  const QString &id, const QStringList &sl,
                  ListMode v, const QString &docs);
    void setValue(const QStringList &sl);

    QVariant &value();
    void update();
    Kind kind() const { return StrList; }
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    QString templateDocs() const { return m_tdocs; }
    void addDependency(Input *) { Q_ASSERT(false); }
    void setEnabled(bool);
    void updateDependencies() {}
    bool isDefault();
    void writeValue(QTextStream &t,TextCodecAdapter *codec,bool convert);
    void setTemplateDocs(const QString &docs) { m_tdocs = docs; }
    bool isEmpty();

  public slots:
    void reset();

  signals:
    void changed();
    void showHelp(Input *);

  private slots:
    void addString();
    void delString();
    void updateString();
    void selectText(const QString &s);
    void browseFiles();
    void browseDir();
    void help();

  private:
    void updateDefault();
    QLabel       *m_lab;
    QLineEdit    *m_le;
    QAction      *m_add;
    QAction      *m_del;
    QAction      *m_upd;
    QAction      *m_brFile;
    QAction      *m_brDir;
    QListWidget  *m_lb;
    QStringList   m_default;
    QStringList   m_strList;
    QVariant      m_value;
    QString       m_docs;
    QString       m_id;
    QString       m_tdocs;
};

#endif

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

#ifndef INPUTSTRING_H
#define INPUTSTRING_H

#include "input.h"

#include <QObject>
#include <QMap>
#include <QStringList>

class QLabel;
class QLineEdit;
class QToolBar;
class QComboBox;
class QGridLayout;
class QAction;

class InputString : public QObject, public Input
{
  Q_OBJECT

  public:
    enum StringMode { StringFree=0,
                      StringFile=1,
                      StringDir=2,
                      StringFixed=3,
                      StringImage=4,
                      StringFileDir=5
                    };

    InputString( QGridLayout *layout,int &row,
                 const QString &id, const QString &s,
                 StringMode m,
                 const QString &docs,
                 const QString &absPath = QString() );
    ~InputString();
    void addValue(QString s);
    void setDefault();

    // Input
    QVariant &value();
    void update();
    Kind kind() const { return String; }
    StringMode stringMode() const { return m_sm; }
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    QString templateDocs() const { return m_tdocs; }
    void addDependency(Input *) { Q_ASSERT(false); }
    void setEnabled(bool);
    void updateDependencies() {}
    bool isDefault();
    void writeValue(QTextStream &t,TextCodecAdapter *codec,bool convert);
    void setTemplateDocs(const QString &docs) { m_tdocs = docs; }
    bool isEmpty() { return m_str.isEmpty(); }
    QString checkEnumVal(const QString &value);

  public slots:
    void reset();
    void setValue(const QString&);

  signals:
    void changed();
    void showHelp(Input *);

  private slots:
    void browseFile();
    void browseDir();
    void clear();
    void help();

  private:
    void updateDefault();
    QLabel       *m_lab;
    QLineEdit    *m_le;
    QLabel       *m_im;
    QToolBar     *m_br;
    QAction      *m_brFile;
    QAction      *m_brDir;
    QComboBox    *m_com;
    QString       m_str;
    QString       m_default;
    StringMode    m_sm;
    QStringList   m_values;
    int           m_index;
    QVariant      m_value;
    QString       m_docs;
    QString       m_id;
    bool          m_absPath;
    QString       m_tdocs;
};

#endif

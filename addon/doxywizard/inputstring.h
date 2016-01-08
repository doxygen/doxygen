/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _INPUTSTRING_H
#define _INPUTSTRING_H

#include "input.h"

#include <QObject>
#include <QMap>
#include <QStringList>

class QLabel;
class QLineEdit;
class QToolBar;
class QComboBox;
class QGridLayout;

class InputString : public QObject, public Input
{
  Q_OBJECT

  public:
    enum StringMode { StringFree=0, 
                      StringFile=1, 
                      StringDir=2, 
                      StringFixed=3,
                      StringImage=4
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
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    QString templateDocs() const { return m_tdocs; }
    void addDependency(Input *option) { m_dependencies+=option; }
    void setEnabled(bool);
    void updateDependencies();
    void writeValue(QTextStream &t,QTextCodec *codec);
    void setTemplateDocs(const QString &docs) { m_tdocs = docs; }

  public slots:
    void reset();
    void setValue(const QString&);

  signals:
    void changed();
    void showHelp(Input *);

  private slots:
    void browse();
    void clear();
    void help();

  private:
    void updateDefault();
    QLabel       *m_lab;
    QLineEdit    *m_le;
    QLabel       *m_im;
    QToolBar     *m_br;
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
    QList<Input*> m_dependencies;
};

#endif

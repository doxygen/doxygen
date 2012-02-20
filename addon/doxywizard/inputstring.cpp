/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "inputstring.h"
#include "helplabel.h"
#include "doxywizard.h"
#include "config.h"

#include <QtGui>

InputString::InputString( QGridLayout *layout,int &row,
                          const QString & id, const QString &s, 
                          StringMode m, const QString &docs,
                          const QString &absPath )
  : m_default(s), m_sm(m), m_index(0), m_docs(docs), m_id(id),
    m_absPath(absPath==QString::fromAscii("1"))
{
  m_lab = new HelpLabel(id);
  if (m==StringFixed)
  {
    layout->addWidget( m_lab, row, 0 );
    m_com = new QComboBox; 
    layout->addWidget( m_com, row, 1, 1, 3, Qt::AlignLeft );
    m_le=0;
    m_br=0;
    row++;
  }
  else
  {
    layout->addWidget( m_lab, row, 0 );
    m_le = new QLineEdit;
    m_le->setText( s );
    //layout->setColumnMinimumWidth(2,150);
    if (m==StringFile || m==StringDir)
    {
      layout->addWidget( m_le, row, 1 );
      m_br = new QToolBar;
      m_br->setIconSize(QSize(24,24));
      if (m==StringFile) 
      {
        QAction *file = m_br->addAction(QIcon(QString::fromAscii(":/images/file.png")),QString(),this,SLOT(browse()));
        file->setToolTip(tr("Browse to a file"));
      }
      else 
      {
        QAction *dir = m_br->addAction(QIcon(QString::fromAscii(":/images/folder.png")),QString(),this,SLOT(browse()));
        dir->setToolTip(tr("Browse to a folder"));
      }
      layout->addWidget( m_br,row,2 );
    }
    else
    {
      layout->addWidget( m_le, row, 1, 1, 2 );
      m_br=0;
    }
    m_com=0;
    row++;
  }

  if (m_le)  connect( m_le,   SIGNAL(textChanged(const QString&)), 
                      this,   SLOT(setValue(const QString&)) );
  if (m_com) connect( m_com,  SIGNAL(activated(const QString &)), 
                      this,   SLOT(setValue(const QString &)) );
  m_str = s+QChar::fromAscii('!'); // force update
  setValue(s);
  connect( m_lab, SIGNAL(enter()), SLOT(help()) );
  connect( m_lab, SIGNAL(reset()), SLOT(reset()) );
}

void InputString::help()
{
  showHelp(this);
}


InputString::~InputString()
{
}


void InputString::setValue(const QString &s)
{
  if (m_str!=s)
  {
    m_str = s;
    m_value = m_str;
    if (m_str==m_default)
    {
      m_lab->setText(QString::fromAscii("<qt>")+m_id+QString::fromAscii("</qt"));
    }
    else
    {
      m_lab->setText(QString::fromAscii("<qt><font color='red'>")+m_id+QString::fromAscii("</font></qt>"));
    }
    if (m_le && m_le->text()!=m_str) m_le->setText( m_str );
    emit changed();
  }
}

void InputString::setEnabled(bool state)
{
  m_lab->setEnabled(state);
  if (m_le)  m_le->setEnabled(state);
  if (m_br)  m_br->setEnabled(state);
  if (m_com) m_com->setEnabled(state);
}

void InputString::browse()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  if (m_sm==StringFile)
  {
    QString fileName = QFileDialog::getOpenFileName(&MainWindow::instance(),
        tr("Select file"),path);
    if (!fileName.isNull()) 
    {
      QDir dir(path);
      if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
      {
        fileName = m_absPath ? fileName : dir.relativeFilePath(fileName);
      }
      setValue(fileName);
    }
  }
  else // sm==StringDir
  {
    QString dirName = QFileDialog::getExistingDirectory(&MainWindow::instance(),
        tr("Select directory"),path);
    if (!dirName.isNull())
    {
      QDir dir(path);
      if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
      {
        dirName = m_absPath ? dirName : dir.relativeFilePath(dirName);
      }
      setValue(dirName);
    }
  }
}

void InputString::clear()
{
  setValue(QString());
}

void InputString::addValue(QString s)
{
  if (m_sm==StringFixed)
  {
    m_values.append(s);
    m_com->addItem(s);
  }
}

void InputString::setDefault()
{
  int index = m_values.indexOf(m_str);
  if (index!=-1 && m_com) m_com->setCurrentIndex(index);
}

QVariant &InputString::value() 
{
  return m_value;
}

void InputString::update()
{
  setValue(m_value.toString().trimmed());
  setDefault();
}

void InputString::reset()
{
  setValue(m_default);
  setDefault();
}

void InputString::writeValue(QTextStream &t,QTextCodec *codec)
{
  writeStringValue(t,codec,m_str);
}


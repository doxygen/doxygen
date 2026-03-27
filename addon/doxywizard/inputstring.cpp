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

#include "inputstring.h"
#include "helplabel.h"
#include "doxywizard.h"
#include "config_msg.h"
#include "config.h"

#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QWheelEvent>
#include <QToolBar>
#include <QFileInfo>
#include <QFileDialog>

class NoWheelComboBox : public QComboBox
{
  protected:
    void wheelEvent(QWheelEvent *e)
    {
      e->ignore();
    }
};


InputString::InputString( QGridLayout *layout,int &row,
                          const QString & id, const QString &s,
                          StringMode m, const QString &docs,
                          const QString &absPath )
  : m_default(s), m_sm(m), m_index(0), m_docs(docs), m_id(id),
    m_absPath(absPath==QString::fromLatin1("1"))
{
  m_lab = new HelpLabel(id);
  m_brFile = nullptr;
  m_brDir = nullptr;
  if (m==StringFixed)
  {
    layout->addWidget( m_lab, row, 0 );
    m_com = new NoWheelComboBox;
    layout->addWidget( m_com, row, 1, 1, 3, Qt::AlignLeft );
    m_le=nullptr;
    m_br=nullptr;
    m_im=nullptr;
    row++;
  }
  else
  {
    layout->addWidget( m_lab, row, 0 );
    m_le = new QLineEdit;
    m_le->setText( s );
    m_im = nullptr;
    //layout->setColumnMinimumWidth(2,150);
    if (m==StringFile || m==StringDir || m==StringImage || m==StringFileDir)
    {
      QHBoxLayout *rowLayout = new QHBoxLayout;
      rowLayout->addWidget( m_le);
      m_br = new QToolBar;
      m_br->setIconSize(QSize(24,24));
      if (m==StringFile || m==StringImage || m==StringFileDir)
      {
        m_brFile = m_br->addAction(QIcon(QString::fromLatin1(":/images/file.svg")),QString(),this,SLOT(browseFile()));
        m_brFile->setToolTip(tr("Browse to a file"));
        if (m==StringImage)
        {
          m_im = new QLabel;
          m_im->setMinimumSize(1,55);
          m_im->setAlignment(Qt::AlignLeft|Qt::AlignTop);
          row++;
          layout->addWidget( m_im,row,1 );
        }
      }
      if (m==StringDir || m==StringFileDir)
      {
        m_brDir = m_br->addAction(QIcon(QString::fromLatin1(":/images/folder.svg")),QString(),this,SLOT(browseDir()));
        m_brDir->setToolTip(tr("Browse to a folder"));
      }
      rowLayout->addWidget( m_br);
      layout->addLayout( rowLayout, m==StringImage?row-1:row, 1, 1, 2 );
    }
    else
    {
      layout->addWidget( m_le, row, 1, 1, 2 );
      m_br=nullptr;
      m_im=nullptr;
    }
    m_com=nullptr;
    row++;
  }

  if (m_le)  connect( m_le,   SIGNAL(textChanged(const QString&)),
                      this,   SLOT(setValue(const QString&)) );
  if (m_com) connect( m_com,  SIGNAL(textActivated(const QString &)),
                      this,   SLOT(setValue(const QString &)) );
  m_str = s+QChar::fromLatin1('!'); // force update
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
    updateDefault();
  }
}
void InputString::updateDefault()
{
  {
    if (m_str==m_default || !m_lab->isEnabled())
    {
      m_lab->setText(QString::fromLatin1("<qt>")+m_id+QString::fromLatin1("</qt>"));
    }
    else
    {
      m_lab->setText(QString::fromLatin1("<qt><font color='red'>")+m_id+QString::fromLatin1("</font></qt>"));
    }
    if (m_im)
    {
      if (m_str.isEmpty())
      {
        m_im->setText(tr("No Project logo selected."));
      }
      else
      {
        QFile Fout(m_str);
        if(!Fout.exists())
        {
          m_im->setText(tr("Sorry, cannot find file(")+m_str+QString::fromLatin1(");"));
        }
        else
        {
          QPixmap pm(m_str);
          if (!pm.isNull())
          {
            m_im->setPixmap(pm.scaledToHeight(55,Qt::SmoothTransformation));
          }
          else
          {
            m_im->setText(tr("Sorry, no preview available (")+m_str+QString::fromLatin1(");"));
          }
        }
      }
    }
    if (m_le && m_le->text()!=m_str) m_le->setText( m_str );
    emit changed();
  }
}

void InputString::setEnabled(bool state)
{
  m_lab->setEnabled(state);
  if (m_le)  m_le->setEnabled(state);
  if (m_im)  m_im->setEnabled(state);
  if (m_br)  m_br->setEnabled(state);
  if (m_brFile)  m_brFile->setEnabled(state);
  if (m_brDir)  m_brDir->setEnabled(state);
  if (m_com) m_com->setEnabled(state);
  updateDefault();
}

void InputString::browseFile()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
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
void InputString::browseDir()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
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

void InputString::writeValue(QTextStream &t,TextCodecAdapter *codec,bool convert)
{
  writeStringValue(t,codec,m_str,convert);
}

bool InputString::isDefault()
{
  return m_str == m_default;
}

QString InputString::checkEnumVal(const QString &value)
{
  QString val = value.trimmed().toLower();
  QStringList::Iterator it;
  for ( it= m_values.begin(); it != m_values.end(); ++it )
  {
    QString enumVal = *it;
    if (enumVal.toLower() == val) return enumVal;
  }

  config_warn("argument '%s' for option %s is not a valid enum value."
              " Using the default: %s!",qPrintable(value),qPrintable(m_id),qPrintable(m_default));
  return m_default;
}

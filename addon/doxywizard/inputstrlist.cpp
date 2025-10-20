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

#include "inputstrlist.h"
#include "helplabel.h"
#include "doxywizard.h"
#include "config.h"

#include <QToolBar>
#include <QGridLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>

InputStrList::InputStrList( QGridLayout *layout,int &row,
                            const QString & id,
                            const QStringList &sl, ListMode lm,
                            const QString & docs)
  : m_default(sl), m_strList(sl), m_docs(docs), m_id(id)
{
  m_lab = new HelpLabel( id );

  m_le  = new QLineEdit;
  m_le->clear();

  QToolBar *toolBar = new QToolBar;
  toolBar->setIconSize(QSize(24,24));
  m_add = toolBar->addAction(QIcon(QString::fromLatin1(":/images/add.svg")),QString(),
                             this,SLOT(addString()));
  m_add->setToolTip(tr("Add item"));
  m_del = toolBar->addAction(QIcon(QString::fromLatin1(":/images/del.svg")),QString(),
                             this,SLOT(delString()));
  m_del->setToolTip(tr("Delete selected item"));
  m_upd = toolBar->addAction(QIcon(QString::fromLatin1(":/images/refresh.svg")),QString(),
                             this,SLOT(updateString()));
  m_upd->setToolTip(tr("Update selected item"));

  m_lb  = new QListWidget;
  //m_lb->setMinimumSize(400,100);
  foreach (QString s, m_strList) m_lb->addItem(s);

  m_brFile=nullptr;
  m_brDir=nullptr;
  if (lm!=ListString)
  {
    if (lm&ListFile)
    {
      m_brFile = toolBar->addAction(QIcon(QString::fromLatin1(":/images/file.svg")),QString(),
                                    this,SLOT(browseFiles()));
      m_brFile->setToolTip(tr("Browse to a file"));
    }
    if (lm&ListDir)
    {
      m_brDir = toolBar->addAction(QIcon(QString::fromLatin1(":/images/folder.svg")),QString(),
                                   this,SLOT(browseDir()));
      m_brDir->setToolTip(tr("Browse to a folder"));
    }
  }
  QHBoxLayout *rowLayout = new QHBoxLayout;
  rowLayout->addWidget( m_le );
  rowLayout->addWidget( toolBar );
  layout->addWidget( m_lab,      row,0 );
  layout->addLayout( rowLayout,  row,1,1,2 );
  layout->addWidget( m_lb,       row+1,1,1,2 );
  row+=2;

  m_value = m_strList;

  connect(m_le,   SIGNAL(returnPressed()),
          this, SLOT(addString()) );
  connect(m_lb,   SIGNAL(currentTextChanged(const QString &)),
          this, SLOT(selectText(const QString &)));
  connect( m_lab, SIGNAL(enter()), SLOT(help()) );
  connect( m_lab, SIGNAL(reset()), SLOT(reset()) );
}

void InputStrList::help()
{
  showHelp(this);
}


void InputStrList::addString()
{
  if (!m_le->text().isEmpty())
  {
    m_lb->addItem(m_le->text());
    m_strList.append(m_le->text());
    m_value = m_strList;
    updateDefault();
    emit changed();
    m_le->clear();
  }
}

void InputStrList::delString()
{
  if (m_lb->currentRow()!=-1)
  {
    int itemIndex = m_lb->currentRow();
    delete m_lb->currentItem();
    m_strList.removeAt(itemIndex);
    m_value = m_strList;
    updateDefault();
    emit changed();
  }
}

void InputStrList::updateString()
{
  if (m_lb->currentRow()!=-1 && !m_le->text().isEmpty())
  {
    m_lb->currentItem()->setText(m_le->text());
    m_strList.insert(m_lb->currentRow(),m_le->text());
    m_strList.removeAt(m_lb->currentRow()+1);
    m_value = m_strList;
    updateDefault();
    emit changed();
  }
}

void InputStrList::selectText(const QString &s)
{
  m_le->setText(s);
}

void InputStrList::setEnabled(bool state)
{
  m_lab->setEnabled(state);
  m_le->setEnabled(state);
  m_add->setEnabled(state);
  m_del->setEnabled(state);
  m_upd->setEnabled(state);
  m_lb->setEnabled(state);
  if (m_brFile) m_brFile->setEnabled(state);
  if (m_brDir)  m_brDir->setEnabled(state);
  updateDefault();
}

void InputStrList::browseFiles()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  QStringList fileNames = QFileDialog::getOpenFileNames();

  if (!fileNames.isEmpty())
  {
    QStringList::Iterator it;
    for ( it= fileNames.begin(); it != fileNames.end(); ++it )
    {
      QString fileName;
      QDir dir(path);
      if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
      {
        fileName = dir.relativeFilePath(*it);
      }
      if (fileName.isEmpty())
      {
        fileName = *it;
      }
      m_lb->addItem(fileName);
      m_strList.append(fileName);
      m_value = m_strList;
      updateDefault();
      emit changed();
    }
    m_le->setText(m_strList[0]);
  }
}

void InputStrList::browseDir()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  QString dirName = QFileDialog::getExistingDirectory();

  if (!dirName.isNull())
  {
    QDir dir(path);
    if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
    {
      dirName = dir.relativeFilePath(dirName);
    }
    if (dirName.isEmpty())
    {
      dirName=QString::fromLatin1(".");
    }
    m_lb->addItem(dirName);
    m_strList.append(dirName);
    m_value = m_strList;
    updateDefault();
    emit changed();
    m_le->setText(dirName);
  }
}

void InputStrList::setValue(const QStringList &sl)
{
  m_le->clear();
  m_lb->clear();
  m_strList = sl;
  for (int i=0;i<m_strList.size();i++)
  {
    m_lb->addItem(m_strList[i].trimmed());
  }
  updateDefault();
}

QVariant &InputStrList::value()
{
  return m_value;
}

void InputStrList::update()
{
  setValue(m_value.toStringList());
}

void InputStrList::updateDefault()
{
  if (isDefault() || !m_lab->isEnabled())
  {
    m_lab->setText(QString::fromLatin1("<qt>")+m_id+QString::fromLatin1("</qt>"));
  }
  else
  {
    m_lab->setText(QString::fromLatin1("<qt><font color='red'>")+m_id+QString::fromLatin1("</font></qt>"));
  }
}

void InputStrList::reset()
{
  setValue(m_default);
}

void InputStrList::writeValue(QTextStream &t,TextCodecAdapter *codec,bool convert)
{
  bool first=true;
  foreach (QString s, m_strList)
  {
    if (!first)
    {
      t << " \\\n";
      t << "                         ";
    }
    first=false;
    writeStringValue(t,codec,s,convert);
  }
}

bool InputStrList::isDefault()
{
  if (m_strList==m_default) return true;

  auto it1 = m_strList.begin();
  auto it2 = m_default.begin();
  while (it1!=m_strList.end() && (*it1).isEmpty())
  {
    ++it1;
  }
  while (it2!=m_default.end() && (*it2).isEmpty())
  {
    ++it2;
  }
  // both lists are empty
  if (it1==m_strList.end() && it2==m_default.end()) return true;

  // one list is empty but the other is not
  if (it1==m_strList.end()) return false;
  if (it2==m_default.end()) return false;

  it1 = m_strList.begin();
  it2 = m_default.begin();
  while (it1!=m_strList.end() && it2!=m_default.end())
  {
    // skip over empty values
    while (it1!=m_strList.end() && (*it1).isEmpty())
    {
      ++it1;
    }
    while (it2!=m_default.end() && (*it2).isEmpty())
    {
      ++it2;
    }
    if ((it1!=m_strList.end()) && (it2!=m_default.end()))
    {
      if ((*it1).trimmed()!= (*it2).trimmed()) // difference so not the default
      {
        return false;
      }
      ++it1;
      ++it2;
    }
    else if ((it1!=m_strList.end()) || (it2!=m_default.end()))
    {
      // one list empty so cannot be the default
      return false;
    }
  }

  return true;
}

bool InputStrList::isEmpty()
{
  foreach (QString s, m_strList)
  {
    if (!s.isEmpty()) return false;
  }
  return true;
}

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

#include <QtGui>

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
  m_add = toolBar->addAction(QIcon(QString::fromAscii(":/images/add.png")),QString(),
                             this,SLOT(addString()));
  m_add->setToolTip(tr("Add item"));
  m_del = toolBar->addAction(QIcon(QString::fromAscii(":/images/del.png")),QString(),
                             this,SLOT(delString()));
  m_del->setToolTip(tr("Delete selected item"));
  m_upd = toolBar->addAction(QIcon(QString::fromAscii(":/images/refresh.png")),QString(),
                             this,SLOT(updateString()));
  m_upd->setToolTip(tr("Update selected item"));

  m_lb  = new QListWidget;
  //m_lb->setMinimumSize(400,100);
  foreach (QString s, m_strList) m_lb->addItem(s);
  
  m_brFile=0;
  m_brDir=0;
  if (lm!=ListString)
  {
    if (lm&ListFile)
    {
      m_brFile = toolBar->addAction(QIcon(QString::fromAscii(":/images/file.png")),QString(),
                                    this,SLOT(browseFiles()));
      m_brFile->setToolTip(tr("Browse to a file"));
    } 
    if (lm&ListDir)
    {
      m_brDir = toolBar->addAction(QIcon(QString::fromAscii(":/images/folder.png")),QString(),
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
      dirName=QString::fromAscii(".");
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
  if (m_strList==m_default)
  {
    m_lab->setText(QString::fromAscii("<qt>")+m_id+QString::fromAscii("</qt"));
  }
  else
  {
    m_lab->setText(QString::fromAscii("<qt><font color='red'>")+m_id+QString::fromAscii("</font></qt>"));
  }
}

void InputStrList::reset()
{
  setValue(m_default);
}

void InputStrList::writeValue(QTextStream &t,QTextCodec *codec)
{
  bool first=TRUE;
  foreach (QString s, m_strList) 
  {
    if (!first) 
    {
      t << " \\" << endl;
      t << "                         ";
    }
    first=FALSE;
    writeStringValue(t,codec,s);
  }
}


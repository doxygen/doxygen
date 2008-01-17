/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "inputstrlist.h"
//#include "pixmaps.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qfiledialog.h>
#include <qtooltip.h>

InputStrList::InputStrList( const QString & label, 
                            QWidget *parent, QStrList &sl, ListMode lm)
  : QWidget(parent), strList(sl)
{
  QGridLayout *layout = new QGridLayout( this, 2, 2, 5 );
  lab = new QLabel( label, this );
  lab->setMinimumSize( lab->sizeHint() );
  layout->addWidget( lab,0,0 );

  QWidget *dw = new QWidget(this); /* dummy widget used for layouting */
  QHBoxLayout *boxlayout = new QHBoxLayout( dw, 0, 5 );
  le  = new QLineEdit( dw );
  le->setMinimumSize( le->sizeHint() );
  boxlayout->addWidget( le, 1 );

  add = new QPushButton( dw );
  //add->setPixmap( QPixmap( add_xpm ));
  add->setText( "+" );
  add->setMinimumSize( add->sizeHint() );
  QToolTip::add(add,"Add item");
  boxlayout->addWidget( add );

  del = new QPushButton( dw );
  //del->setPixmap( QPixmap( del_xpm ));
  del->setText( "-" );
  del->setMinimumSize( del->sizeHint() );
  QToolTip::add(del,"Delete selected item");
  boxlayout->addWidget( del );

  upd = new QPushButton( dw ); 
  //upd->setPixmap( QPixmap( update_xpm ));
  upd->setText( "*" );
  upd->setMinimumSize( upd->sizeHint() );
  QToolTip::add(upd,"Update selected item");
  boxlayout->addWidget( upd );

  lb  = new QListBox( this );
  lb->setMinimumSize(400,100);
  init();
  lb->setVScrollBarMode(QScrollView::Auto);
  lb->setHScrollBarMode(QScrollView::Auto);

  brFile=0;
  brDir=0;
  if (lm!=ListString)
  {
    if (lm&ListFile)
    {
      brFile = new QPushButton(dw);
      //brFile->setPixmap( QPixmap(file_xpm) );
      brFile->setText("Select...");
      brFile->setMinimumSize(brFile->sizeHint());
      QToolTip::add(brFile,"Browse to a file");
      boxlayout->addWidget( brFile );
    } 
    if (lm&ListDir)
    {
      brDir = new QPushButton(dw);
      //brDir->setPixmap( QPixmap(folder_xpm) );
      brDir->setText("Select...");
      brDir->setMinimumSize(brDir->sizeHint());
      QToolTip::add(brDir,"Browse to a folder");
      boxlayout->addWidget( brDir );
    }
  }
  layout->addWidget( dw, 0,1 );
  layout->addWidget( lb,1,1 );
  layout->activate();
  setMinimumSize( sizeHint() );

  connect(le,   SIGNAL(returnPressed()), 
          this, SLOT(addString()) );
  connect(add,  SIGNAL(clicked()), 
          this, SLOT(addString()) );
  connect(del,  SIGNAL(clicked()), 
          this, SLOT(delString()) );
  connect(upd,  SIGNAL(clicked()), 
          this, SLOT(updateString()) );
  if (brFile)
  {
    connect(brFile, SIGNAL(clicked()),
            this, SLOT(browseFiles()));
  }
  if (brDir)
  {
    connect(brDir, SIGNAL(clicked()),
            this, SLOT(browseDir()));
  }
  connect(lb,   SIGNAL(selected(const QString &)), 
          this, SLOT(selectText(const QString &)));
}

void InputStrList::addString()
{
  if (!le->text().isEmpty())
  {
    lb->insertItem(le->text());
    strList.append(le->text());
    emit changed();
    le->clear();
  }
}

void InputStrList::delString()
{
  if (lb->currentItem()!=-1)
  {
    int itemIndex = lb->currentItem();
    lb->removeItem(itemIndex);
    strList.remove(itemIndex);
    emit changed();
  }
}

void InputStrList::updateString()
{
  if (lb->currentItem()!=-1 && !le->text().isEmpty())
  {
    lb->changeItem(le->text(),lb->currentItem());
    strList.insert(lb->currentItem(),le->text());
    strList.remove(lb->currentItem()+1);
    emit changed();
  }
}

void InputStrList::selectText(const QString &s)
{
  le->setText(s);
}

void InputStrList::setEnabled(bool state)
{
  lab->setEnabled(state);
  le->setEnabled(state);
  add->setEnabled(state);
  del->setEnabled(state);
  upd->setEnabled(state);
  lb->setEnabled(state);
  if (brFile) brFile->setEnabled(state);
  if (brDir)  brDir->setEnabled(state);
}

void InputStrList::browseFiles()
{
  QStringList fileNames = QFileDialog::getOpenFileNames();	

  if (!fileNames.isEmpty()) 
  {
    QStringList::Iterator it;
    for ( it= fileNames.begin(); it != fileNames.end(); ++it )
    {
      lb->insertItem(*it);
      strList.append(*it);
      emit changed();
    }
    le->setText(*fileNames.begin());
  }
}

void InputStrList::browseDir()
{	
  QString dirName = QFileDialog::getExistingDirectory();	

  if (!dirName.isNull()) 
  {
    lb->insertItem(dirName);
    strList.append(dirName);
    emit changed();
    le->setText(dirName);
  }
}

void InputStrList::init()
{
  le->clear();
  lb->clear();
  char *s = strList.first();
  while (s)
  {
    lb->insertItem(s);
    s = strList.next();
  }
}

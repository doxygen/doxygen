/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "inputstring.h"
#include "pagewidget.h"
#include "pixmaps.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qwindowsstyle.h>
#include <qcstring.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qtooltip.h>


InputString::InputString( const QString & label, 
                          PageWidget *parent, QCString &s, StringMode m )
  : QWidget( parent->getLayout() ), str(s), sm(m), m_values(0), m_index(0)
{
  if (m==StringFixed)
  {
    QHBoxLayout *layout = new QHBoxLayout( this, 5);
    lab = new QLabel( label, this );
    lab->setMinimumSize( lab->sizeHint() );
    layout->addWidget( lab );
    com = new QComboBox( this ); 
    layout->addWidget( com );
    layout->addStretch( 1 );
    le=0;
    br=0;
    layout->activate();
    setMinimumSize( sizeHint() );
  }
  else
  {
    QGridLayout *layout = new QGridLayout( this, 1, m==StringFree ? 1 : 3, 5 );
    lab = new QLabel( label, this );
    lab->setMinimumSize( lab->sizeHint() );
    layout->addWidget( lab,0,0 );
    le = new QLineEdit( this );
    le->setMinimumSize( le->sizeHint() );
    le->setText( s );
    layout->addWidget( le,0,1 );
    if (m==StringFile || m==StringDir)
    {
      QPixmap pixmap = QPixmap(m==StringFile ? 
                               file_xpm :
                               folder_xpm );
      br = new QPushButton( this );
      br->setPixmap(pixmap);
      br->setMinimumSize( br->sizeHint() );  
      if (m==StringFile) 
        QToolTip::add(br,"Browse to a file");
      else 
        QToolTip::add(br,"Browse to a folder");
      layout->addWidget( br,0,2 );
    }
    else
    {
      br=0;
    }
    com=0;
    layout->activate();
    setMinimumSize( sizeHint() );
  }

  if (le)  connect( le,   SIGNAL(textChanged(const QString&)), 
                    this, SLOT(textChanged(const QString&)) );
  if (br)  connect( br,   SIGNAL(clicked()), this, SLOT(browse()) );
  if (com) connect( com,  SIGNAL(activated(const QString &)), 
                    this, SLOT(textChanged(const QString &)) );

  parent->addWidget(this);
}

InputString::~InputString()
{
  if (m_values) delete m_values;
}


void InputString::textChanged(const QString &s)
{
  if (str!=(const char *)s)
  {
    str = s;
    emit changed();
  }
}

void InputString::setEnabled(bool state)
{
  lab->setEnabled(state);
  if (le) le->setEnabled(state);
  if (br) br->setEnabled(state);
  if (com) com->setEnabled(state);
}

void InputString::browse()
{
  if (sm==StringFile)
  {
    QString fileName = QFileDialog::getOpenFileName();

    if (!fileName.isNull()) 
    {
      le->setText( fileName );
      if (str!=(const char *)le->text())
      {
        str = le->text(); 
        emit changed();
      }
    }
  }
  else // sm==StringDir
  {
    QString dirName = QFileDialog::getExistingDirectory();

    if (!dirName.isNull())
    {
      le->setText( dirName ); 	
      if (str!=(const char *)le->text())
      {
        str = le->text();
        emit changed();
      }
    }	
  }
}

void InputString::clear()
{
  le->setText("");
  if (!str.isEmpty())
  {
    emit changed();
    str = "";
  }
}

void InputString::addValue(const char *s)
{
  if (sm==StringFixed)
  {
    if (m_values==0) m_values = new QDict<int>;
    m_values->setAutoDelete(TRUE);
    m_values->insert(s,new int(m_index++));
    com->insertItem(s);
  }
}

void InputString::init()
{
  if (sm==StringFixed)
  {
    int *itemIndex = m_values->find(str);
    if (itemIndex)
    {
      com->setCurrentItem(*itemIndex);
    } 
    else
    {
      com->setCurrentItem(0);
    }
  }
  else
  {
    le->setText(str);
  }
}

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

#include "inputbool.h"
#include "pagewidget.h"
#include <qwindowsstyle.h>
#include <qlayout.h>

InputBool::InputBool( const QString & text, PageWidget * parent, bool &flag )
  : /*QCheckBox( text, parent->getLayout() )*/ QWidget(parent->getLayout()), state(flag)
{
  QBoxLayout *layout = new QHBoxLayout(this);
  cb = new QCheckBox(text,this);
  layout->addWidget(cb);
  layout->addStretch(10);

  QWindowsStyle *winStyle = new QWindowsStyle();
  cb->setChecked( flag );
  cb->setStyle( winStyle );
  cb->setMinimumSize( sizeHint() );

  connect( cb, SIGNAL(toggled(bool)), SLOT(setState(bool)) );

  parent->addWidget(this);

  layout->activate();
}

void InputBool::init()
{
  cb->setChecked(state);
}

void InputBool::setState( bool s )
{
  if (state!=s) 
  {
    emit changed();
    emit toggle(cb->text(),s);
  }
  state=s;
}


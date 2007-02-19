/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "inputbool.h"
//#if QT_VERSION >= 300
//#include <qstylefactory.h>
//#else
//#include <qwindowsstyle.h>
//#endif
#include <qlayout.h>

InputBool::InputBool( const QString & text, QWidget * parent, bool &flag )
  : QWidget(parent), state(flag)
{
  QBoxLayout *layout = new QHBoxLayout(this);
  cb = new QCheckBox(text,this);
  layout->addWidget(cb);
  layout->addStretch(10);

//#if QT_VERSION >= 300
//  QStyle *winStyle = QStyleFactory::create("windows");
//#else
//  QWindowsStyle *winStyle = new QWindowsStyle();
//#endif
  cb->setChecked( flag );
//  if (winStyle) cb->setStyle( winStyle );
  cb->setMinimumSize( sizeHint() );

  connect( cb, SIGNAL(toggled(bool)), SLOT(setState(bool)) );

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


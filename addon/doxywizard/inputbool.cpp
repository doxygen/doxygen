/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

InputBool::InputBool( const QString & text, PageWidget * parent, bool &flag )
  : QCheckBox( text, parent->getLayout() ), state(flag)
{
  QWindowsStyle *winStyle = new QWindowsStyle();
  setChecked( flag );
  setStyle( winStyle );
  setMinimumSize( sizeHint() );

  connect( this, SIGNAL(toggled(bool)), SLOT(setState(bool)) );

  parent->addWidget(this);
}

void InputBool::init()
{
  setChecked(state);
}

void InputBool::setState( bool s )
{
  if (state!=s) 
  {
    emit changed();
    emit toggle(text(),s);
  }
  state=s;
}


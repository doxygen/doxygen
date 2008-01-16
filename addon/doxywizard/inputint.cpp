/******************************************************************************
 *
 * $Id$
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

#include "inputint.h"

#include <stdio.h>
#include <stdlib.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwindowsstyle.h>
#include <qspinbox.h>

InputInt::InputInt( const QString & label, QWidget *parent, int &val, int minVal,int maxVal )
  : QWidget( parent), m_val(val), m_minVal(minVal), m_maxVal(maxVal)
{
  QHBoxLayout *layout = new QHBoxLayout( this, 5 );

  lab = new QLabel( label, this );
  lab->setMinimumSize( lab->sizeHint() );

  sp = new QSpinBox( minVal,maxVal,1,this );
  sp->setMinimumSize( sp->sizeHint() );

  init();
  
  layout->addWidget( lab );
  layout->addWidget( sp );
  layout->addStretch(1);
  layout->activate();
  setMinimumSize( sizeHint() );

  connect(sp,   SIGNAL(valueChanged(int)), 
          this, SLOT(valueChanged(int)) );

}

void InputInt::valueChanged(int val)
{
  if (val!=m_val) emit changed(); 
  m_val = val;
}

void InputInt::setEnabled(bool state)
{
  lab->setEnabled(state);
  sp->setEnabled(state);
}

void InputInt::init()
{
  m_val = QMAX(m_minVal,m_val);
  m_val = QMIN(m_maxVal,m_val);
  sp->setValue(m_val);
}

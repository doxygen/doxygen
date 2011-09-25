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

#include "inputint.h"
#include "helplabel.h"

#include <QtGui>

InputInt::InputInt( QGridLayout *layout,int &row,
                    const QString & id, 
                    int defVal, int minVal,int maxVal,
                    const QString & docs )
  : m_default(defVal), m_minVal(minVal), m_maxVal(maxVal), m_docs(docs), m_id(id)
{
  m_lab = new HelpLabel(id);
  m_sp  = new QSpinBox;
  m_sp->setMinimum(minVal);
  m_sp->setMaximum(maxVal);
  m_sp->setSingleStep(1);
  m_val=defVal-1; // force update
  setValue(defVal);

  layout->addWidget( m_lab, row, 0 );
  layout->addWidget( m_sp, row, 1 );

  connect(m_sp, SIGNAL(valueChanged(int)), 
          this, SLOT(setValue(int)) );
  connect( m_lab, SIGNAL(enter()), SLOT(help()) );
  connect( m_lab, SIGNAL(reset()), SLOT(reset()) );
  row++;
}

void InputInt::help()
{
  showHelp(this);
}


void InputInt::setValue(int val)
{
  val = qMax(m_minVal,val);
  val = qMin(m_maxVal,val);
  if (val!=m_val) 
  {
    m_val = val;
    m_sp->setValue(val);
    m_value = m_val;
    if (m_val==m_default)
    {
      m_lab->setText(QString::fromAscii("<qt>")+m_id+QString::fromAscii("</qt"));
    }
    else
    {
      m_lab->setText(QString::fromAscii("<qt><font color='red'>")+m_id+QString::fromAscii("</font></qt>"));
    }
    emit changed(); 
  }
}

void InputInt::setEnabled(bool state)
{
  m_lab->setEnabled(state);
  m_sp->setEnabled(state);
}

QVariant &InputInt::value() 
{
  return m_value;
}

void InputInt::update()
{
  setValue(m_value.toInt());
}

void InputInt::reset()
{
  setValue(m_default);
}

void InputInt::writeValue(QTextStream &t,QTextCodec *)
{
  t << m_val;
}


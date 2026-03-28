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

#include "inputint.h"
#include "helplabel.h"
#include "config_msg.h"

#include <QSpinBox>
#include <QGridLayout>
#include <QWheelEvent>
#include <QTextStream>

class NoWheelSpinBox : public QSpinBox
{
  protected:
    void wheelEvent(QWheelEvent *e)
    {
      e->ignore();
    }
};

InputInt::InputInt( QGridLayout *layout,int &row,
                    const QString & id,
                    int defVal, int minVal,int maxVal,
                    const QString & docs )
  : m_default(defVal), m_minVal(minVal), m_maxVal(maxVal), m_docs(docs), m_id(id)
{
  m_lab = new HelpLabel(id);
  m_sp  = new NoWheelSpinBox;
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
  int newVal = val;
  newVal = qMax(m_minVal,newVal);
  newVal = qMin(m_maxVal,newVal);
  if (val != newVal)
  {
    config_warn("argument '%d' for option %s is not a valid number in the range [%d..%d]!"
                " Using the default: %d!\n",val,qPrintable(m_id),m_minVal,m_maxVal,m_default);
    newVal = m_default;
  }
  if (newVal!=m_val)
  {
    m_val = newVal;
    m_sp->setValue(newVal);
    m_value = m_val;
    updateDefault();
  }
}

void InputInt::updateDefault()
{
  {
    if (m_val==m_default || !m_lab->isEnabled())
    {
      m_lab->setText(QString::fromLatin1("<qt>")+m_id+QString::fromLatin1("</qt>"));
    }
    else
    {
      m_lab->setText(QString::fromLatin1("<qt><font color='red'>")+m_id+QString::fromLatin1("</font></qt>"));
    }
    emit changed();
  }
}

void InputInt::setEnabled(bool state)
{
  m_lab->setEnabled(state);
  m_sp->setEnabled(state);
  updateDefault();
}

QVariant &InputInt::value()
{
  return m_value;
}

void InputInt::update()
{
  bool ok;
  int newVal = m_value.toInt(&ok);
  if (!ok)
  {
    config_warn("argument '%s' for option %s is not a valid number in the range [%d..%d]!"
                " Using the default: %d!\n",qPrintable(m_value.toString()),qPrintable(m_id),m_minVal,m_maxVal,m_default);
    newVal = m_default;
  }
  setValue(newVal);
}

void InputInt::reset()
{
  setValue(m_default);
}

void InputInt::writeValue(QTextStream &t,TextCodecAdapter *,bool)
{
  t << m_val;
}


bool InputInt::isDefault()
{
  return m_val == m_default;
}

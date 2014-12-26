/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "inputbool.h"
#include "helplabel.h"

#include <QCheckBox>
#include <QTextStream>
#include <QTextCodec>
#include <QGridLayout>

InputBool::InputBool( QGridLayout *layout, int &row, 
                      const QString &id, bool checked,
                      const QString &docs )
  : m_default(checked), m_docs(docs), m_id(id)
{
  m_lab = new HelpLabel(id);
  m_cb = new QCheckBox;
  layout->addWidget(m_lab,row, 0);
  layout->addWidget(m_cb,row, 1);
  m_enabled = true;
  m_state=!checked; // force update
  setValue(checked);
  connect( m_cb, SIGNAL(toggled(bool)), SLOT(setValue(bool)) );
  connect( m_lab, SIGNAL(enter()), SLOT(help()) );
  connect( m_lab, SIGNAL(reset()), SLOT(reset()) );
  row++;
}

void InputBool::help()
{
  showHelp(this);
}

void InputBool::setEnabled(bool b)
{ 
  m_enabled = b;
  m_cb->setEnabled(b); 
  m_lab->setEnabled(b); 
  updateDefault();
  updateDependencies();
}

void InputBool::updateDependencies()
{
  for (int i=0;i<m_dependencies.count();i++)
  {
    m_dependencies[i]->setEnabled(m_enabled && m_state);
  }
}

void InputBool::setValue( bool s )
{
  if (m_state!=s) 
  {
    m_state=s;
    updateDefault();
    updateDependencies();
    m_cb->setChecked( s );
    m_value = m_state;
    emit changed();
  }
}

void InputBool::updateDefault()
{
  if (m_state==m_default || !m_lab->isEnabled())
  {
    m_lab->setText(QString::fromLatin1("<qt>")+m_id+QString::fromLatin1("</qt"));
  }
  else
  {
    m_lab->setText(QString::fromLatin1("<qt><font color='red'>")+m_id+QString::fromLatin1("</font></qt>"));
  }
}

QVariant &InputBool::value()
{
  return m_value;
}

void InputBool::update()
{
  QString v = m_value.toString().toLower();
  m_state = (v==QString::fromLatin1("yes")  || 
             v==QString::fromLatin1("true") || 
             v==QString::fromLatin1("1"));
  m_cb->setChecked( m_state );
  updateDefault();
  updateDependencies();
}

void InputBool::reset()
{
  setValue(m_default);
}

void InputBool::writeValue(QTextStream &t,QTextCodec *codec)
{
  if (m_state) 
    t << codec->fromUnicode(QString::fromLatin1("YES")); 
  else 
    t << codec->fromUnicode(QString::fromLatin1("NO"));
}


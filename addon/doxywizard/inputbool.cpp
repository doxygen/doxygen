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

#include "inputbool.h"
#include "helplabel.h"
#include "config_msg.h"

#include <QCheckBox>
#include <QTextStream>
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
    m_lab->setText(QString::fromLatin1("<qt>")+m_id+QString::fromLatin1("</qt>"));
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

bool InputBool::convertToBool(const QVariant &value,bool &isValid)
{
  QString v = value.toString().toLower();
  if (v==QString::fromLatin1("yes") || v==QString::fromLatin1("true") ||
      v==QString::fromLatin1("1")   || v==QString::fromLatin1("all"))
  {
    isValid = true;
    return true;
  }
  else if (v==QString::fromLatin1("no") || v==QString::fromLatin1("false") ||
           v==QString::fromLatin1("0")  || v==QString::fromLatin1("none"))
  {
    isValid = true;
    return false;
  }
  else
  {
    isValid = false;
    return false;
  }
}

void InputBool::update()
{
  bool isValid=false;
  bool b = convertToBool(m_value,isValid);
  if (isValid)
  {
    m_state = b;
  }
  else
  {
    config_warn("argument '%s' for option %s is not a valid boolean value."
                " Using the default: %s!",qPrintable(m_value.toString()),qPrintable(m_id),m_default?"YES":"NO");
    m_state = m_default;
  }
  m_cb->setChecked( m_state );
  updateDefault();
  updateDependencies();
}

void InputBool::reset()
{
  setValue(m_default);
}

void InputBool::writeValue(QTextStream &t,TextCodecAdapter *codec,bool)
{
  if (m_state)
    t << codec->encode(QString::fromLatin1("YES"));
  else
    t << codec->encode(QString::fromLatin1("NO"));
}

bool InputBool::isDefault()
{
  return m_state == m_default;
}


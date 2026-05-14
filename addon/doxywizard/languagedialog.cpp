/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "languagedialog.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>

LanguageDialog::LanguageDialog(const QString &currentLocale,
                               QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle(tr("Select Language"));
  setModal(true);

  auto *mainLayout = new QVBoxLayout(this);

  // Explanatory note
  auto *note = new QLabel(tr("Changing the language requires a restart of the application."), this);
  note->setWordWrap(true);
  mainLayout->addWidget(note);
  mainLayout->addSpacing(10);

  // Radio buttons inside a group box
  auto *groupBox    = new QGroupBox(tr("Available Languages"), this);
  auto *groupLayout = new QVBoxLayout(groupBox);

  auto *rb = new QRadioButton(QString::fromLatin1("English"), groupBox);
  groupLayout->addWidget(rb);
  m_buttons.append(rb);
  m_languages.append({QString::fromLatin1("English"),QString::fromLatin1("en")});

  // add additional languages based on embedded info
  QDir resourceDir(QString::fromLatin1(":/i18n"));
  QFileInfoList fileList = resourceDir.entryInfoList();
  foreach (QFileInfo fileInfo, fileList)
  {
    QString filename     = fileInfo.fileName();
    const int underscore = filename.indexOf(QChar::fromLatin1('_'));
    const int dot        = filename.lastIndexOf(QChar::fromLatin1('.'));
    if (filename.startsWith(QString::fromLatin1("config")) && underscore!=-1 && dot>underscore)
    {
      QString langCode = filename.mid(underscore+1, dot-underscore-1);
      QFile trFile(QString::fromLatin1(":/i18n/config_%1.xml").arg(langCode));
      if (trFile.open(QIODevice::ReadOnly))
      {
        QDomDocument trConfigXml;
        QString err = tr("Error");
        int errLine=0,errCol=0;
        if (trConfigXml.setContent(&trFile,false,&err,&errLine,&errCol))
        {
          QString langLabel = trConfigXml.documentElement().attribute(QString::fromLatin1("lang"));
          auto *rb = new QRadioButton(langLabel, groupBox);
          if (langCode==currentLocale)
          {
            rb->setChecked(true);
          }
          groupLayout->addWidget(rb);
          m_buttons.append(rb);
          m_languages.append({langLabel,langCode});
        }
      }
    }
  }

  // Fallback: check the first button if nothing matched
  if (!m_buttons.isEmpty())
  {
    bool anyChecked = std::any_of(m_buttons.cbegin(), m_buttons.cend(),
        [](QRadioButton *b){ return b->isChecked(); });
    if (!anyChecked)
    {
      m_buttons.first()->setChecked(true);
    }
  }

  mainLayout->addWidget(groupBox);

  // Buttons: Cancel  |  Select & Quit
  auto *buttonBox = new QDialogButtonBox(this);
  auto *cancelBtn = buttonBox->addButton(QDialogButtonBox::Cancel);
  auto *selectBtn = buttonBox->addButton(tr("Select and Quit"),
      QDialogButtonBox::AcceptRole);
  selectBtn->setDefault(true);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);
}

QString LanguageDialog::selectedLocale() const
{
  for (int i = 0; i < m_buttons.size(); ++i)
  {
    if (m_buttons[i]->isChecked())
    {
      return m_languages[i].locale;
    }
  }
  return {};
}

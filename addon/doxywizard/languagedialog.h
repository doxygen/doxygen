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

#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

class QRadioButton;

class LanguageDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit LanguageDialog(const QString &currentLocale, QWidget *parent = nullptr);

    /** Returns the locale code chosen by the user. */
    QString selectedLocale() const;

  private:
    struct LanguageInfo
    {
        QString label;
        QString locale;
    };
    QList<QRadioButton *> m_buttons;
    QList<LanguageInfo>   m_languages;
};

#endif // LANGUAGEDIALOG_H

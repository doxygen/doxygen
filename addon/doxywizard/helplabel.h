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

#ifndef HELPLABEL_H
#define HELPLABEL_H

#include <QLabel>
#include <QMenu>

class HelpLabel : public QLabel
{
    Q_OBJECT
  public:
    HelpLabel(const QString &text) : QLabel(text)
    { setContextMenuPolicy(Qt::CustomContextMenu);
      connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),
              this,SLOT(showMenu(const QPoint&)));
    }
  signals:
    void enter();
    void reset();
  private slots:
    void showMenu(const QPoint &p)
    {
      QMenu menu(this);
      QAction *a = menu.addAction(tr("Reset to default"));
      if (menu.exec(mapToGlobal(p))==a)
      {
        reset();
      }
    }
  protected:
    void enterEvent(
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QEvent * event
#else
        QEnterEvent * event
#endif
    ) { enter(); QLabel::enterEvent(event); }
};

#endif

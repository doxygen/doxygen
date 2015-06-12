#include "helplabel.h"

HelpLabel::HelpLabel(const QString &text)
  : QLabel(text)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showMenu(const QPoint&)));
}

void HelpLabel::showMenu(const QPoint &p)
{
    QMenu menu(this);
    QAction *a = menu.addAction(tr("Reset to default"));
    if (menu.exec(mapToGlobal(p))==a)
    {
        reset();
    }
}

void HelpLabel::enterEvent( QEvent * event )
{
    enter();
    QLabel::enterEvent(event);
}

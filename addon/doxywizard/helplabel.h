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
    void enterEvent( QEvent * event ) { enter(); QLabel::enterEvent(event); }
};

#endif

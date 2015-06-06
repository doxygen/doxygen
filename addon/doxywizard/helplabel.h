#ifndef HELPLABEL_H
#define HELPLABEL_H

#include <QLabel>
#include <QMenu>

class HelpLabel : public QLabel
{
    Q_OBJECT
  public:
    HelpLabel(const QString &text);
  signals:
    void enter();
    void reset();
  private slots:
    void showMenu(const QPoint &p);
  protected:
    void enterEvent( QEvent * event );
};

#endif

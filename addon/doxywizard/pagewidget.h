/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _PAGEWIDGET_H
#define _PAGEWIDGET_H

#include <qscrollview.h>

class QTabWidget;
class QVBoxLayout;

class FrameLayout : public QFrame
{
  Q_OBJECT

  public:
    FrameLayout(QWidget *parent=0, const char *name=0, WFlags g=0);
   ~FrameLayout();
    void addStretch(int s=0);
    void addWidget(QWidget *w);

  private:
    QVBoxLayout *lay;
};

class PageWidget : public QScrollView 
{
  Q_OBJECT

  public:
    PageWidget( QTabWidget *parent, const char *name );
    ~PageWidget() {};
    //void frameChanged() 
    //{ 
    //  if (!layout()) return; 
    //  layout()->setMargin(frameWidth()); 
    //}
    void addStretch(int stretch=0);
    void addWidget(QWidget *w);
    QWidget *getLayout() const;

  protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

  private:
    FrameLayout *lay;
    QTabWidget *m_parent;

};

#endif

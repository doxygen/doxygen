/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <stdio.h>

#include <qtabwidget.h>
#include <qlayout.h>
#include "pagewidget.h"

FrameLayout::FrameLayout(QWidget *parent,const char *name,WFlags f)
  : QFrame(parent,name,f)
{
  lay = new QVBoxLayout(this,5,3,name);
}

FrameLayout::~FrameLayout()
{
}

void FrameLayout::addStretch(int stretch)
{
  lay->addStretch(stretch);
}

void FrameLayout::addWidget(QWidget *w)
{
  lay->addWidget(w);
}

PageWidget::PageWidget( QTabWidget *parent, const char *name )
  : QScrollView ( parent,name )
{
  // the Qt docs say that one has to insert the viewport() as a parent
  // here but this does not work! Insert 0 does give the proper result!
  lay = new FrameLayout(this);
  addChild(lay);
  setVScrollBarMode(QScrollView::Auto);
  setHScrollBarMode(QScrollView::AlwaysOff);
  m_parent=parent;
  m_parent->addTab( this, name );
}

void PageWidget::addStretch(int stretch) 
{ 
  lay->addStretch(stretch); 
  //printf("Viewport SizeHint %d,%d\n",viewport()->sizeHint().width(),viewport()->sizeHint().height());
}

void PageWidget::addWidget(QWidget *w) 
{
  //printf("Adding widget height=%d\n",w->sizeHint().height());
  lay->addWidget(w);
}

QWidget *PageWidget::getLayout() const
{
  return lay;
}

void PageWidget::resizeEvent(QResizeEvent *e)
{
  QScrollView::resizeEvent(e);
  //printf("PageWidget::resizeEvent!\n");
  lay->resize(viewport()->width(),QMAX(lay->height(),viewport()->height()));
}

void PageWidget::paintEvent(QPaintEvent *e)
{
  QScrollView::paintEvent(e);
  //printf("PageWidget::paintEvent()\n");
  lay->resize(viewport()->width(),QMAX(lay->height(),viewport()->height()));
}




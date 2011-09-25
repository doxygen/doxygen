/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdio.h>

#include "qglobal.h"

#include "qmutex.h"
#include "qmutex_p.h"

QMutex::QMutex() : d(new QMutexPrivate())
{
}

QMutex::~QMutex()
{
  delete d;
}

void QMutex::lock()
{
    //printf("%p: QMutex::lock(): enter\n",this);
    bool isLocked;
    isLocked = d->contenders.testAndSet(0, 1);
    if (!isLocked)
    {
      isLocked = d->contenders.fetchAndAdd(1)==0;
      if (!isLocked)
      {
        // didn't get the lock, wait for it
        //printf("%p: QMutex::lock(): wait() %d\n",this,(int)d->contenders);
        d->wait();

        // release lock
        d->contenders.fetchAndAdd(-1);
      }
    }
    //printf("%p: QMutex::lock(): leave\n",this);
}

bool QMutex::tryLock()
{
    bool isLocked = d->contenders.testAndSet(0, 1);
    return isLocked;
}

void QMutex::unlock()
{
    //printf("%p: QMutex::unlock(): enter %d\n",this,(int)d->contenders);
    if (!d->contenders.testAndSet(1, 0))
    {
         //printf("%p: QMutex::unlock(): wakeUp()\n",this);
         d->wakeUp();
    }
    //printf("%p: QMutex::unlock(): leave\n",this);
}


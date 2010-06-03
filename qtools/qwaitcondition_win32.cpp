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

#include <windows.h>
#include "qwaitcondition.h"
#include "qmutex.h"
#include "qlist.h"

//***********************************************************************
// QWaitConditionPrivate
// **********************************************************************

class QWaitConditionEvent
{
public:
    QWaitConditionEvent() : priority(0), wokenUp(false)
    {
        event = CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    ~QWaitConditionEvent() { CloseHandle(event); }
    int priority;
    bool wokenUp;
    HANDLE event;
};

class EventQueue : public QList<QWaitConditionEvent>
{
  public:
    EventQueue() { setAutoDelete(TRUE); }
   ~EventQueue() {}
};

class QWaitConditionPrivate
{
public:
    QMutex mtx;
    EventQueue queue;
    EventQueue freeQueue;

    QWaitConditionEvent *pre();
    void wait(QWaitConditionEvent *wce);
    void post(QWaitConditionEvent *wce);
};

QWaitConditionEvent *QWaitConditionPrivate::pre()
{
    mtx.lock();
    QWaitConditionEvent *wce =
        freeQueue.isEmpty() ? new QWaitConditionEvent : freeQueue.take(0);
    wce->priority = GetThreadPriority(GetCurrentThread());
    wce->wokenUp = FALSE;

    // insert 'wce' into the queue (sorted by priority)
    uint index = 0;
    for (; index < queue.count(); ++index)
    {
        QWaitConditionEvent *current = queue.at(index);
        if (current->priority < wce->priority)
            break;
    }
    queue.insert(index, wce);
    mtx.unlock();

    return wce;
}

void QWaitConditionPrivate::wait(QWaitConditionEvent *wce)
{
    WaitForSingleObject(wce->event, INFINITE);
}

void QWaitConditionPrivate::post(QWaitConditionEvent *wce)
{
    mtx.lock();

    // remove 'wce' from the queue
    int idx = queue.find(wce);
    ASSERT(idx!=-1);
    queue.take(idx);
    ResetEvent(wce->event);
    freeQueue.append(wce);

    // wakeups delivered after the timeout should be forwarded to the next waiter
    if (wce->wokenUp && !queue.isEmpty())
    {
        QWaitConditionEvent *other = queue.getFirst();
        SetEvent(other->event);
        other->wokenUp = TRUE;
    }

    mtx.unlock();
}

//***********************************************************************
// QWaitCondition implementation
//***********************************************************************

QWaitCondition::QWaitCondition()
{
    d = new QWaitConditionPrivate;
}

QWaitCondition::~QWaitCondition()
{
    if (!d->queue.isEmpty())
    {
        qWarning("QWaitCondition: Destroyed while threads are still waiting");
    }
    delete d;
}

void QWaitCondition::wait(QMutex *mutex)
{
    if (!mutex) return;

    QWaitConditionEvent *wce = d->pre();
    mutex->unlock();
    d->wait(wce);
    mutex->lock();
    d->post(wce);
}

void QWaitCondition::wakeOne()
{
    // wake up the first waiting thread in the queue
    QMutexLocker locker(&d->mtx);
    for (uint i = 0; i < d->queue.count(); ++i)
    {
        QWaitConditionEvent *current = d->queue.at(i);
        if (current->wokenUp) continue;
        SetEvent(current->event);
        current->wokenUp = TRUE;
        break;
    }
}

void QWaitCondition::wakeAll()
{
    // wake up the all threads in the queue
    QMutexLocker locker(&d->mtx);
    for (uint i = 0; i < d->queue.count(); ++i) 
    {
        QWaitConditionEvent *current = d->queue.at(i);
        SetEvent(current->event);
        current->wokenUp = TRUE;
    }
}


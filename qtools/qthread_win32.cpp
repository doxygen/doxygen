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

#include "qthread.h"
#include "qthread_p.h"

/**************************************************************************
 ** QThreadPrivate
 *************************************************************************/

QThreadPrivate::QThreadPrivate() :
  running(FALSE), finished(FALSE), terminated(FALSE), stackSize(0)
{
  handle = NULL;
  waiters = 0;
}

QThreadPrivate::~QThreadPrivate()
{
}

unsigned int __stdcall QThreadPrivate::start(void *arg)
{
    QThread *thr = reinterpret_cast<QThread *>(arg);
    thr->started();
    thr->run();
    finish(arg);
    return 0;
}

void QThreadPrivate::finish(void *arg,bool lockAnyway)
{
    QThread *thr = reinterpret_cast<QThread *>(arg);
    QThreadPrivate *d = thr->d;

    if (lockAnyway) d->mutex.lock();

    d->running = FALSE;
    d->finished = TRUE;
    if (d->terminated) thr->terminated();
    d->terminated = FALSE;
    thr->finished();

    if (!d->waiters)
    {
      CloseHandle(d->handle);
      d->handle = 0;
    }

    if (lockAnyway) d->mutex.unlock();
}

/**************************************************************************
 ** QThread
 *************************************************************************/

void QThread::start()
{
    QMutexLocker locker(&d->mutex);

    if (d->running) return;

    d->running = TRUE;
    d->finished = FALSE;
    d->terminated = FALSE;

    d->handle = CreateThread(NULL,d->stackSize,
                    (LPTHREAD_START_ROUTINE)QThreadPrivate::start,this,0,NULL);

    if (!d->handle) 
    {
        qWarning("QThread::start: Failed to create thread: errno=%d",errno);
        d->running = FALSE;
        d->finished = TRUE;
        return;
    }
}

void QThread::terminate()
{
    QMutexLocker locker(&d->mutex);
    if (!d->running) return;
    TerminateThread(d->handle, 0);
    d->terminated = TRUE;
    QThreadPrivate::finish(this);
}

void QThread::wait()
{
    QMutexLocker locker(&d->mutex);
    if (d->finished || !d->running) return;

    ++d->waiters;
    locker.mutex()->unlock();

    WaitForSingleObject(d->handle,INFINITE);

    locker.mutex()->lock();
    --d->waiters;
    if (!d->finished) // thread was terminated by someone else
    {
      d->terminated = TRUE;
      QThreadPrivate::finish(this);
    }

    if (d->finished && d->waiters)
    {
      CloseHandle(d->handle);
      d->handle = 0;
    }
}

int QThread::idealThreadCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}



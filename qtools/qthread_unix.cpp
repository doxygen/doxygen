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

#include "qglobal.h"

#if defined(_OS_HPUX_)
#include <sys/pstat.h>
#elif defined(_OS_MAC_)
#undef DEBUG
#include <CoreServices/CoreServices.h>
#elif defined(_OS_BSDI_)
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#include <unistd.h>

#include "qthread.h"
#include "qthread_p.h"


/**************************************************************************
 ** QThreadPrivate
 *************************************************************************/

QThreadPrivate::QThreadPrivate() :
  running(FALSE), finished(FALSE), terminated(FALSE), stackSize(0)
{
  thread_id = 0;
}

QThreadPrivate::~QThreadPrivate()
{
}

void *QThreadPrivate::start(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_cleanup_push(QThreadPrivate::finish, arg);

    QThread *thr = reinterpret_cast<QThread *>(arg);

    thr->started();
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    thr->run();

    pthread_cleanup_pop(1);
    return 0;
}

void QThreadPrivate::finish(void *arg)
{
    QThread *thr = reinterpret_cast<QThread *>(arg);
    QThreadPrivate *d = thr->d;
    QMutexLocker locker(&d->mutex);

    d->running = FALSE;
    d->finished = TRUE;
    if (d->terminated)
        thr->terminated();
    d->terminated = FALSE;
    thr->finished();

    d->thread_id = 0;
    d->thread_done.wakeAll();
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

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
    if (d->stackSize>0)
    {
#if defined(_POSIX_THREAD_ATTR_STACKSIZE) && (_POSIX_THREAD_ATTR_STACKSIZE-0>0)
      pthread_attr_setstacksize(&attr,d->stackSize);
#endif
    }
    int code = pthread_create(&d->thread_id, &attr, QThreadPrivate::start, this);
    pthread_attr_destroy(&attr);

    if (code)
    {
        qWarning("QThread::start: Thread creation error: %d", code);

        d->running = FALSE;
        d->finished = FALSE;
        d->thread_id = 0;
    }
}

void QThread::terminate()
{
    QMutexLocker locker(&d->mutex);

    if (!d->thread_id) return;

    int code = pthread_cancel(d->thread_id);
    if (code)
    {
        qWarning("QThread::start: Thread termination error: %d", code);
    }
    else
    {
        d->terminated = TRUE;
    }
}

void QThread::wait()
{
    QMutexLocker locker(&d->mutex);
    if (d->finished || !d->running) return;

    while (d->running)
    {
      d->thread_done.wait(locker.mutex());
    }
}

#if defined(QT_LINUXBASE) && !defined(_SC_NPROCESSORS_ONLN)
// LSB doesn't define _SC_NPROCESSORS_ONLN.
#  define _SC_NPROCESSORS_ONLN 84
#endif

int QThread::idealThreadCount()
{
    int cores = -1;
#if defined(_OS_MAC_)
    // Mac OS X
    cores = MPProcessorsScheduled();
#elif defined(_OS_HPUX_)
    // HP-UX
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), 1, 0) == -1) 
    {
      perror("pstat_getdynamic");
      cores = -1;
    } 
    else 
    {
      cores = (int)psd.psd_proc_cnt;
    }
#elif defined(_OS_BSDI_)
    // FreeBSD, OpenBSD, NetBSD, BSD/OS
    size_t len = sizeof(cores);
    int mib[2];
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    if (sysctl(mib, 2, &cores, &len, NULL, 0) != 0) 
    {
      perror("sysctl");
      cores = -1;
    }
#elif defined(_OS_IRIX_)
    // IRIX
    cores = (int)sysconf(_SC_NPROC_ONLN);
#else
    // the rest: Linux, Solaris, AIX, Tru64
    cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
    return cores;  
}


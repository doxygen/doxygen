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

#include <errno.h>
#include <pthread.h>

#include "qglobal.h"
#include "qmutex.h"
#include "qmutex_p.h"

static pthread_mutex_t qAtomicMutex = PTHREAD_MUTEX_INITIALIZER;

static void report_error(int code, const char *where, const char *what)
{
    if (code != 0)
        qWarning("%s: %s failure: %d", where, what, code);
}


QMutexPrivate::QMutexPrivate()
    : contenders(0), wakeup(FALSE)
{
    report_error(pthread_mutex_init(&mutex, NULL), "QMutex", "mutex init");
    report_error(pthread_cond_init(&cond, NULL), "QMutex", "cv init");
}

QMutexPrivate::~QMutexPrivate()
{
    report_error(pthread_cond_destroy(&cond), "QMutex", "cv destroy");
    report_error(pthread_mutex_destroy(&mutex), "QMutex", "mutex destroy");
}

void QMutexPrivate::wait()
{
    report_error(pthread_mutex_lock(&mutex), "QMutex::lock", "mutex lock");
    int errorCode = 0;
    while (!wakeup)
    {
        errorCode = pthread_cond_wait(&cond, &mutex);
        if (errorCode)
        {
            report_error(errorCode, "QMutex::lock()", "cv wait");
        }
    }
    wakeup = FALSE;
    report_error(pthread_mutex_unlock(&mutex), "QMutex::lock", "mutex unlock");
}

void QMutexPrivate::wakeUp()
{
    report_error(pthread_mutex_lock(&mutex), "QMutex::unlock", "mutex lock");
    wakeup = TRUE;
    report_error(pthread_cond_signal(&cond), "QMutex::unlock", "cv signal");
    report_error(pthread_mutex_unlock(&mutex), "QMutex::unlock", "mutex unlock");
}

bool QAtomicInt::testAndSet(int expectedValue,int newValue)
{
  bool returnValue = false;
  pthread_mutex_lock(&qAtomicMutex);
  if (m_value == expectedValue)
  {
    m_value = newValue;
    returnValue = true;
  }
  pthread_mutex_unlock(&qAtomicMutex);
  return returnValue;
}

int QAtomicInt::fetchAndAdd(int valueToAdd)
{
  int returnValue;
  pthread_mutex_lock(&qAtomicMutex);
  returnValue = m_value;
  m_value += valueToAdd;
  pthread_mutex_unlock(&qAtomicMutex);
  return returnValue;
}


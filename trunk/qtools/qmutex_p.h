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

#ifndef QMUTEX_P_H
#define QMUTEX_P_H

#include "qglobal.h"

#if defined(_OS_UNIX_) || defined(_OS_MAC_)
#include <pthread.h>
#elif defined(_OS_WIN32_)
#include <windows.h>
#endif

class QAtomicInt
{
  public:
    QAtomicInt(int v=0) : m_value(v) {}
    bool testAndSet(int expectedValue,int newValue);
    int  fetchAndAdd(int valueToAdd);
    operator int () const { return m_value; }
    bool operator==(int value) const { return m_value == value; }
    bool operator!=(int value) const { return m_value != value; }
    bool operator!() const { return m_value == 0; }

  private:
    volatile int m_value;
};

class QMutexPrivate
{
public:
    QMutexPrivate();
    ~QMutexPrivate();

    void wait();
    void wakeUp();

    QAtomicInt contenders;

#if defined(_OS_UNIX_) || defined(_OS_MAC_)
    volatile bool wakeup;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#elif defined(_OS_WIN32_)
    HANDLE event;
#else
#error "unsupported platform"
#endif
};

#endif // QMUTEX_P_H

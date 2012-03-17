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

#include "qmutex.h"
#include "qmutex_p.h"

QMutexPrivate::QMutexPrivate()
    : contenders(0)
{
  event = CreateEvent(0, FALSE, FALSE, 0);
  if (!event)
    qWarning("QMutexPrivate::QMutexPrivate: Cannot create event");
}

QMutexPrivate::~QMutexPrivate()
{
  CloseHandle(event);
}

void QMutexPrivate::wait()
{
  WaitForSingleObject(event, INFINITE);
}

void QMutexPrivate::wakeUp()
{
  SetEvent(event);
}

//----------------------------------------------------------------------

class QCriticalSection
{
  public:
    QCriticalSection()  { InitializeCriticalSection(&section); }
    ~QCriticalSection() { DeleteCriticalSection(&section); }
    void lock()         { EnterCriticalSection(&section); }
    void unlock()       { LeaveCriticalSection(&section); }

  private:
    CRITICAL_SECTION section;
};

static QCriticalSection qAtomicCriticalSection;

bool QAtomicInt::testAndSet(int expectedValue,int newValue)
{
  bool returnValue = false;
  qAtomicCriticalSection.lock();
  if (m_value == expectedValue)
  {
    m_value = newValue;
    returnValue = true;
  }
  qAtomicCriticalSection.unlock();
  return returnValue;
}

int QAtomicInt::fetchAndAdd(int valueToAdd)
{
  int returnValue;
  qAtomicCriticalSection.lock();
  returnValue = m_value;
  m_value += valueToAdd;
  qAtomicCriticalSection.unlock();
  return returnValue;
}


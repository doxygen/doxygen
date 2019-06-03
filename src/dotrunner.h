/******************************************************************************
*
* 
*
*
* Copyright (C) 1997-2015 by Dimitri van Heesch.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation under the terms of the GNU General Public License is hereby 
* granted. No representations are made about the suitability of this software 
* for any purpose. It is provided "as is" without express or implied warranty.
* See the GNU General Public License for more details.
*
* Documents produced by Doxygen are derivative works derived from the
* input used in their production; they are not affected by this license.
*
*/

#ifndef _DOT_RUNNER_H
#define _DOT_RUNNER_H

#include "qcstring.h"
#include "qlist.h"
#include "qwaitcondition.h"
#include "qthread.h"
#include "qqueue.h"
#include "qmutex.h"

/** Minimal constant string class that is thread safe, once initialized. */
class DotConstString
{
public:
  DotConstString()                                   { m_str=0; m_pdfstr=0;}
  ~DotConstString()                                   { delete[] m_str; delete[] m_pdfstr;}
  DotConstString(const QCString &s, const QCString &p = NULL) : m_str(0), m_pdfstr(0)       { set(s); setpdf(p);}
  DotConstString(const DotConstString &s) : m_str(0), m_pdfstr(0) { set(s.data()); }
  const char *data() const                           { return m_str; }
  const char *pdfData() const                        { return m_pdfstr; }
  bool isEmpty() const                               { return m_str==0 || m_str[0]=='\0'; }
  void set(const QCString &s)
  {
    delete[] m_str;
    m_str=0;
    if (!s.isEmpty())
    {
      m_str=new char[s.length()+1];
      qstrcpy(m_str,s.data());
    }
  }
  void setpdf(const QCString &p)
  {
    delete[] m_pdfstr;
    m_pdfstr=0;
    if (!p.isEmpty())
    {
      m_pdfstr=new char[p.length()+1];
      qstrcpy(m_pdfstr,p.data());
    }
  }
private:
  DotConstString &operator=(const DotConstString &);
  char *m_str;
  char *m_pdfstr;
};

/** Helper class to run dot from doxygen.
*/
class DotRunner
{
public:
  struct CleanupItem
  {
    DotConstString path;
    DotConstString file;
  };

  /** Creates a runner for a dot \a file. */
  DotRunner(const QCString& absDotName, const QCString& path, const QCString& md5Hash,
    bool checkResult, const QCString &imageName = QCString());

  /** Adds an additional job to the run.
  *  Performing multiple jobs one file can be faster.
  */
  void addJob(const char *format,const char *output, const char *base = NULL);

  void addPostProcessing(const char *cmd,const char *args);

  void preventCleanUp() { m_cleanUp = FALSE; }

  /** Runs dot for all jobs added. */
  bool run();
  const CleanupItem &cleanup() const { return m_cleanupItem; }

  DotConstString const& getFileName() { return m_file; }
  DotConstString const& getPath() { return m_path; }
  DotConstString const& getMd5Hash() { return m_md5Hash; }

private:
  QList<DotConstString> m_jobs;
  DotConstString m_postArgs;
  DotConstString m_postCmd;
  DotConstString m_file;
  DotConstString m_path;
  bool m_checkResult;
  DotConstString m_imageName;
  DotConstString m_imgExt;
  bool m_cleanUp;
  CleanupItem m_cleanupItem;
  DotConstString m_md5Hash;

  static bool DOT_CLEANUP;
  static bool DOT_MULTI_TARGETS;
  static DotConstString DOT_EXE;
  friend void initDot();

};

/** Queue of dot jobs to run. */
class DotRunnerQueue
{
public:
  void enqueue(DotRunner *runner);
  DotRunner *dequeue();
  uint count() const;
private:
  QWaitCondition  m_bufferNotEmpty;
  QQueue<DotRunner> m_queue;
  mutable QMutex  m_mutex;
};

/** Worker thread to execute a dot run */
class DotWorkerThread : public QThread
{
public:
  DotWorkerThread(DotRunnerQueue *queue);
  void run();
  void cleanup();
private:
  DotRunnerQueue *m_queue;
  QList<DotRunner::CleanupItem> m_cleanupItems;
};

void checkDotResult(const char* imgExt, const char* imgName);

#endif

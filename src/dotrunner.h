/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#ifndef DOTRUNNER_H
#define DOTRUNNER_H

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
    DotConstString()                                   { m_str=0;}
   ~DotConstString()                                   { delete[] m_str;}
    DotConstString(char const* s)           : m_str(0) { set(s); }
    DotConstString(const QCString &s)       : m_str(0) { set(s); }
    DotConstString(const DotConstString &s) : m_str(0) { set(s.data()); }
    const char *data() const                           { return m_str; }
    bool isEmpty() const                               { return m_str==0 || m_str[0]=='\0'; }

  private:
    void set(char const* s)
    {
      delete[] m_str;
      m_str=0;
      if (s)
      {
        m_str=new char[strlen(s) + 1];
        qstrcpy(m_str,s);
      }
    }

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

    DotConstString &operator=(const DotConstString &);

    char *m_str;
};

/** Helper class to run dot from doxygen from multiple threads.  */
class DotRunner
{
  public:
    struct DotJob
    {
      DotJob(const DotConstString & format,
             const DotConstString & output,
             const DotConstString & args)
        : format(format), output(output), args(args) {}
      DotConstString format;
      DotConstString output;
      DotConstString args;
    };

    /** Creates a runner for a dot \a file. */
    DotRunner(const QCString& absDotName, const QCString& md5Hash);

    /** Adds an additional job to the run.
     *  Performing multiple jobs one file can be faster.
     */
    void addJob(const char *format,const char *output);

    /** Prevent cleanup of the dot file (for user provided dot files) */
    void preventCleanUp() { m_cleanUp = FALSE; }

    /** Runs dot for all jobs added. */
    bool run();

    //  DotConstString const& getFileName() { return m_file; }
    DotConstString const& getMd5Hash() { return m_md5Hash; }

    static bool readBoundingBox(const char* fileName, int* width, int* height, bool isEps);

  private:
    DotConstString m_file;
    DotConstString m_md5Hash;
    DotConstString m_dotExe;
    bool           m_cleanUp;
    QList<DotJob>  m_jobs;
};

/** Queue of dot jobs to run. */
// all methods are thread save
class DotRunnerQueue
{
  public:
    void enqueue(DotRunner *runner);
    DotRunner *dequeue();
    uint count() const;
  private:
    QWaitCondition    m_bufferNotEmpty;
    QQueue<DotRunner> m_queue;
    mutable QMutex    m_mutex;
};

/** Worker thread to execute a dot run */
class DotWorkerThread : public QThread
{
  public:
    DotWorkerThread(DotRunnerQueue *queue);
    void run();
  private:
    DotRunnerQueue *m_queue;
};

#endif

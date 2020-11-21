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

#include <string>
#include <thread>
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

/** Helper class to run dot from doxygen from multiple threads.  */
class DotRunner
{
    struct DotJob
    {
      DotJob(std::string f, std::string o, std::string a, std::string s, int l)
        : format(f), output(o), args(a), srcFile(s), srcLine(l) {}
      std::string format;
      std::string output;
      std::string args;
      std::string srcFile;
      int srcLine = -1;
    };

  public:
    /** Creates a runner for a dot \a file. */
    DotRunner(const std::string& absDotName, const std::string& md5Hash = std::string());

    /** Adds an additional job to the run.
     *  Performing multiple jobs one file can be faster.
     */
    void addJob(const char *format,const char *output, const char *s = "", const int l = -1);

    /** Prevent cleanup of the dot file (for user provided dot files) */
    void preventCleanUp() { m_cleanUp = false; }

    /** Runs dot for all jobs added. */
    bool run();

    //  DotConstString const& getFileName() { return m_file; }
    std::string const & getMd5Hash() { return m_md5Hash; }

    static bool readBoundingBox(const char* fileName, int* width, int* height, bool isEps);

  private:
    std::string m_file;
    std::string m_md5Hash;
    std::string m_dotExe;
    bool        m_cleanUp;
    std::vector<DotJob>  m_jobs;
};

/** Queue of dot jobs to run. */
// all methods are thread save
class DotRunnerQueue
{
  public:
    void enqueue(DotRunner *runner);
    DotRunner *dequeue();
    size_t size() const;
  private:
    std::condition_variable m_bufferNotEmpty;
    std::queue<DotRunner *> m_queue;
    mutable std::mutex    m_mutex;
};

/** Worker thread to execute a dot run */
class DotWorkerThread
{
  public:
    DotWorkerThread(DotRunnerQueue *queue);
   ~DotWorkerThread();
    void run();
    void start();
    bool isRunning() { return m_thread && m_thread->joinable(); }
    void wait() { m_thread->join(); }
  private:
    std::unique_ptr<std::thread> m_thread;
    DotRunnerQueue *m_queue;
};

#endif

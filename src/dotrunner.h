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
  public:
    class DotJob
    {
    public:
      DotJob(std::string format, std::string output)
        : format(std::move(format)), output(std::move(output)) {}
      DotJob(std::string format, std::string output, std::string cachefile)
        : format(std::move(format)), output(std::move(output)), cachefile(std::move(cachefile)) {}
      std::string format;
      std::string output;
      std::string cachefile;
      std::string getarg() const {return std::string("-T") + format + " -o \"" + output + "\""; }
      
    };

  public:
    /** Creates a runner for a dot \a file. */
    DotRunner(std::string absDotName, std::string md5Hash = std::string());

    /** Adds an additional job to the run.
     *  Performing multiple jobs one file can be faster.
     *  \param format The output format. This is forwarded to dot.
     *  \param output The target file
     */
    void addJob(const char *format,const char *output, const char * cachefile = "");

    /** Prevent cleanup of the dot file (for user provided dot files) */
    void preventCleanUp() { m_cleanUp = false; }

    /** Runs dot for all jobs added.
     * @return True if the execution was successful. False on an error.
     */
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

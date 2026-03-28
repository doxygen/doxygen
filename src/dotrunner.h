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

#include "qcstring.h"

/** Helper class to run dot from doxygen from multiple threads.  */
class DotRunner
{
    struct DotJob
    {
      DotJob(const QCString &f, const QCString &o, const QCString &a,
             const QCString &s,int l)
        : format(f), output(o), args(a), srcFile(s), srcLine(l) {}
      QCString format;
      QCString output;
      QCString args;
      QCString srcFile;
      int srcLine;
    };

  public:
    /** Creates a runner for a dot \a file. */
    DotRunner(const QCString & absDotName, const QCString& md5Hash = QCString());

    /** Adds an additional job to the run.
     *  Performing multiple jobs one file can be faster.
     */
    void addJob(const QCString &format,const QCString &output,const QCString &srcFile,int srcLine);

    /** Prevent cleanup of the dot file (for user provided dot files) */
    void preventCleanUp() { m_cleanUp = false; }

    /** Runs dot for all jobs added. */
    bool run();

    QCString getMd5Hash() { return m_md5Hash; }

    static bool readBoundingBox(const QCString &fileName, int* width, int* height, bool isEps);

  private:
    QCString m_file;
    QCString m_md5Hash;
    QCString m_dotExe;
    bool     m_cleanUp;
    std::vector<DotJob>  m_jobs;
};

#endif

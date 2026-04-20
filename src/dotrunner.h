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

#include <vector>

#include "qcstring.h"
#include "dotjob.h"

/** Helper class to run dot from doxygen. Holds a queue of jobs, each
 *  specifying an input .dot file and output format. Call run() to execute
 *  all queued jobs, batched as a single dot invocation per output format
 *  using the -O flag for automatic output file naming.
 */
class DotRunner
{
  public:
    DotRunner();

    /** Queues a job to be performed by this runner. */
    void addJob(const DotJob &job);

    /** Runs dot for all queued jobs. For each unique format, a single dot
     *  invocation is made with -O and all input files for that format.
     */
    bool run();

    static bool readBoundingBox(const QCString &fileName, int* width, int* height, bool isEps);

  private:
    QCString            m_dotExe;
    std::vector<DotJob> m_jobs;
};

#endif

/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef DOT_H
#define DOT_H

#include <map>
#include <vector>

#include "construct.h"
#include "dotfilepatcher.h"
#include "dotgraph.h" // only for GraphOutputFormat
#include "dotjob.h"
#include "dotrunner.h"
#include "doxygen.h"

class DString;
class TextStream;

/** Singleton that manages dot invocations and patching files for embedding image maps */
class DotManager
{
  public:
    static DotManager *instance();
    void addJob(const DotJob &newJob);
    DotFilePatcher *createFilePatcher(const DString &fileName);
    bool needToRun() const { return m_jobs.size()+m_filePatchers.size()>0; }
    bool run();

  private:
    DotManager();
    virtual ~DotManager();
    NON_COPYABLE(DotManager)

    DotRunner                             m_runner;
    std::map<std::string, DotFilePatcher> m_filePatchers;
    DotJobs                               m_jobs;

};

void writeDotGraphFromFile(const DString &inFile,const DString &outDir,
                           const DString &outFile,GraphOutputFormat format,
                           const DString &srcFile,int srcLine,bool toIndex);
void writeDotImageMapFromFile(TextStream &t,
                              const DString &inFile, const DString& outDir,
                              const DString &relPath,const DString& baseName,
                              const DString &context,int graphId,
                              const DString &srcFile,int srcLine,bool newFile);

#endif // DOT_H

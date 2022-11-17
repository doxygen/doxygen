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

#ifndef DOT_H
#define DOT_H

#include <map>

#include "qcstring.h"
#include "dotgraph.h" // only for GraphOutputFormat
#include "dotfilepatcher.h"
#include "dotrunner.h"
#include "doxygen.h"

class DotRunner;
class DotRunnerQueue;
class TextStream;

using DotWorkerThreadPtr = std::unique_ptr< DotWorkerThread, NonTerminatingDeleter<DotWorkerThread > >;

/** Singleton that manages parallel dot invocations and patching files for embedding image maps */
class DotManager
{
  public:
    static DotManager *instance();
    //static void deleteInstance();
    DotRunner*      createRunner(const QCString& absDotName, const QCString& md5Hash);
    DotFilePatcher *createFilePatcher(const QCString &fileName);
    bool run() const;

  private:
    DotManager();
    virtual ~DotManager();

    std::map<std::string, std::unique_ptr<DotRunner> > m_runners;
    std::map<std::string, DotFilePatcher>              m_filePatchers;
    DotRunnerQueue                                    *m_queue;
    std::vector< DotWorkerThreadPtr >                  m_workers;
};

void writeDotGraphFromFile(const QCString &inFile,const QCString &outDir,
                           const QCString &outFile,GraphOutputFormat format,
                           const QCString &srcFile,int srcLine);
void writeDotImageMapFromFile(TextStream &t,
                              const QCString &inFile, const QCString& outDir,
                              const QCString &relPath,const QCString& baseName,
                              const QCString &context,int graphId,
                              const QCString &srcFile,int srcLine);

#endif

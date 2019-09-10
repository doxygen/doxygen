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

#include <qlist.h>
#include <qdict.h>
#include <qcstring.h>

#include "sortdict.h"

#include "dotgraph.h" // only for GraphOutputFormat

class FTextStream;
class DotRunner;
class DotRunnerQueue;
class DotWorkerThread;
class DotFilePatcher;

/** Singleton that manages parallel dot invocations and patching files for embedding image maps */
class DotManager
{
  public:
    static DotManager *instance();
    DotRunner*      createRunner(const QCString& absDotName, const QCString& md5Hash);
    DotFilePatcher *createFilePatcher(const QCString &fileName);
    bool run() const;

  private:
    DotManager();
    virtual ~DotManager();

    QDict<DotRunner>       m_runners;
    SDict<DotFilePatcher>  m_filePatchers;
    static DotManager     *m_theInstance;
    DotRunnerQueue        *m_queue;
    QList<DotWorkerThread> m_workers;
};

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format);
void writeDotImageMapFromFile(FTextStream &t,
                              const QCString& inFile, const QCString& outDir,
                              const QCString& relPath,const QCString& baseName,
                              const QCString& context,int graphId=-1);

#endif

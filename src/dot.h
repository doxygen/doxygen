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

#ifndef _DOT_H
#define _DOT_H

#include <qlist.h>
#include <qdict.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <qqueue.h>
#include <qthread.h>
#include "sortdict.h"
#include "classdef.h"
#include "qgstring.h"
#include "qdir.h"
#include "qcstring.h"

#include "dotgraph.h"
#include "dotnode.h"

class FileDef;
class FTextStream;
class DotNodeList;
class ClassSDict;
class MemberDef;
class Definition;
class DirDef;
class GroupDef;
class DotGroupCollaboration;
class DotRunner;
class DotRunnerQueue;
class DotWorkerThread;
class DotNode;
class DotFilePatcher;

// the graphicx LaTeX has a limitation of maximum size of 16384
// To be on the save side we take it a little bit smaller i.e. 150 inch * 72 dpi
// It is anyway hard to view these size of images
#define MAX_LATEX_GRAPH_INCH  150
#define MAX_LATEX_GRAPH_SIZE  (MAX_LATEX_GRAPH_INCH * 72)



/** Singleton that manages dot relation actions */
class DotManager
{
  public:
    static DotManager *instance();
    void addRun(DotRunner *run);
    int  addMap(const QCString &file,const QCString &mapFile,
                const QCString &relPath,bool urlOnly,
                const QCString &context,const QCString &label);
    int addFigure(const QCString &file,const QCString &baseName,
                  const QCString &figureName,bool heightCheck);
    int addSVGConversion(const QCString &file,const QCString &relPath,
               bool urlOnly,const QCString &context,bool zoomable,int graphId);
    int addSVGObject(const QCString &file,const QCString &baseName,
                     const QCString &figureNAme,const QCString &relPath);
    bool run();

    bool containsRun(const QCString& absDotName, const QCString& md5Hash);

  private:
    DotManager();
    virtual ~DotManager();
    QList<DotRunner>       m_dotRuns;
    SDict<DotFilePatcher> m_dotMaps;
    static DotManager     *m_theInstance;
    DotRunnerQueue        *m_queue;
    QList<DotWorkerThread> m_workers;
};

void initDot();


/** Generated a graphs legend page */
void generateGraphLegend(const char *path);

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format);
void writeDotImageMapFromFile(FTextStream &t,
                              const QCString& inFile, const QCString& outDir,
                              const QCString& relPath,const QCString& baseName,
                              const QCString& context,int graphId=-1);
bool writeSVGFigureLink(FTextStream &out,const QCString &relPath,
                        const QCString &baseName,const QCString &absImgName);
bool convertMapFile(FTextStream &t,const char *mapName,
                    const QCString relPath, bool urlOnly=FALSE,
                    const QCString &context=QCString());
bool writeVecGfxFigure(FTextStream &out,const QCString &baseName,
                       const QCString &figureName);

#endif

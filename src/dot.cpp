/*****************************************************************************
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

#include <stdlib.h>

#include <qdir.h>

#include "config.h"
#include "dot.h"
#include "dotrunner.h"
#include "dotfilepatcher.h"
#include "util.h"
#include "portable.h"
#include "message.h"
#include "ftextstream.h"
#include "doxygen.h"
#include "language.h"
#include "index.h"

#define MAP_CMD "cmapx"

//--------------------------------------------------------------------

static QCString g_dotFontPath;

static void setDotFontPath(const char *path)
{
  ASSERT(g_dotFontPath.isEmpty());
  g_dotFontPath = Portables::getenv("DOTFONTPATH");
  QCString newFontPath = Config_getString(DOT_FONTPATH);
  QCString spath = path;
  if (!newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath.prepend(spath+Portables::pathListSeparator());
  }
  else if (newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath=path;
  }
  else
  {
    Portables::unsetenv("DOTFONTPATH");
    return;
  }
  Portables::setenv("DOTFONTPATH",newFontPath);
}

static void unsetDotFontPath()
{
  if (g_dotFontPath.isEmpty())
  {
    Portables::unsetenv("DOTFONTPATH");
  }
  else
  {
    Portables::setenv("DOTFONTPATH",g_dotFontPath);
  }
  g_dotFontPath="";
}

//--------------------------------------------------------------------

DotManager *DotManager::m_theInstance = 0;

DotManager *DotManager::instance()
{
  if (!m_theInstance)
  {
    m_theInstance = new DotManager;
  }
  return m_theInstance;
}

DotManager::DotManager() : m_runners(1009), m_filePatchers(1009)
{
  m_runners.setAutoDelete(TRUE);
  m_filePatchers.setAutoDelete(TRUE);
  m_queue = new DotRunnerQueue;
  int i;
  int dotNumThreads = Config_getInt(DOT_NUM_THREADS);
  if (dotNumThreads!=1)
  {
    for (i=0;i<dotNumThreads;i++)
    {
      DotWorkerThread *thread = new DotWorkerThread(m_queue);
      thread->start();
      if (thread->isRunning())
      {
        m_workers.append(thread);
      }
      else // no more threads available!
      {
        delete thread;
      }
    }
    ASSERT(m_workers.count()>0);
  }
}

DotManager::~DotManager()
{
  delete m_queue;
}

DotRunner* DotManager::createRunner(const QCString& absDotName, const QCString& md5Hash)
{
  DotRunner * run = m_runners.find(absDotName);
  if (run == 0)
  {
    run = new DotRunner(absDotName, md5Hash);
    m_runners.insert(absDotName, run);
  }
  else
  {
    // we have a match
    if (md5Hash != QCString(run->getMd5Hash().data()))
    {
      err("md5 hash does not match for two different runs of %s !\n", absDotName.data());
    }
  }
  return run;
}

DotFilePatcher *DotManager::createFilePatcher(const QCString &fileName)
{
  DotFilePatcher *patcher = m_filePatchers.find(fileName);
  if (patcher==0)
  {
    patcher = new DotFilePatcher(fileName);
    m_filePatchers.append(fileName,patcher);
  }
  return patcher;
}

bool DotManager::run() const
{
  uint numDotRuns = m_runners.count();
  uint numFilePatchers = m_filePatchers.count();
  if (numDotRuns+numFilePatchers>1)
  {
    if (m_workers.count()==0)
    {
      msg("Generating dot graphs in single threaded mode...\n");
    }
    else
    {
      msg("Generating dot graphs using %d parallel threads...\n",QMIN(numDotRuns+numFilePatchers,m_workers.count()));
    }
  }
  int i=1;
  QDictIterator<DotRunner> li(m_runners);

  bool setPath=FALSE;
  if (Config_getBool(GENERATE_HTML))
  {
    setDotFontPath(Config_getString(HTML_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_LATEX))
  {
    setDotFontPath(Config_getString(LATEX_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_RTF))
  {
    setDotFontPath(Config_getString(RTF_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_DOCBOOK))
  {
    setDotFontPath(Config_getString(DOCBOOK_OUTPUT));
    setPath=TRUE;
  }
  Portables::sysTimerStart();
  // fill work queue with dot operations
  DotRunner *dr;
  int prev=1;
  if (m_workers.count()==0) // no threads to work with
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
      dr->run();
      prev++;
    }
  }
  else // use multiple threads to run instances of dot in parallel
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      m_queue->enqueue(dr);
    }
    // wait for the queue to become empty
    while ((i=m_queue->count())>0)
    {
      i = numDotRuns - i;
      while (i>=prev)
      {
        msg("Running dot for graph %d/%d\n",prev,numDotRuns);
        prev++;
      }
      Portables::sleep(100);
    }
    while ((int)numDotRuns>=prev)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
      prev++;
    }
    // signal the workers we are done
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_queue->enqueue(0); // add terminator for each worker
    }
    // wait for the workers to finish
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->wait();
    }
  }
  Portables::sysTimerStop();
  if (setPath)
  {
    unsetDotFontPath();
  }

  // patch the output file and insert the maps and figures
  i=1;
  SDict<DotFilePatcher>::Iterator di(m_filePatchers);
  const DotFilePatcher *fp;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and 
  // then the other files. 
  for (di.toFirst();(fp=di.current());++di)
  {
    if (fp->isSVGFile())
    {
      msg("Patching output file %d/%d\n",i,numFilePatchers);
      if (!fp->run()) return FALSE;
      i++;
    }
  }
  for (di.toFirst();(fp=di.current());++di)
  {
    if (!fp->isSVGFile())
    {
      msg("Patching output file %d/%d\n",i,numFilePatchers);
      if (!fp->run()) return FALSE;
      i++;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format)
{
  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir); exit(1);
  }

  QCString imgExt = getDotImageExtension();
  QCString imgName = (QCString)outFile+"."+imgExt;
  QCString absImgName = d.absPath().utf8()+"/"+imgName;
  QCString absOutFile = d.absPath().utf8()+"/"+outFile;

  DotRunner dotRun(inFile, QCString());
  if (format==GOF_BITMAP)
  {
    dotRun.addJob(Config_getEnum(DOT_IMAGE_FORMAT),absImgName);
  }
  else // format==GOF_EPS
  {
    if (Config_getBool(USE_PDFLATEX))
    {
      dotRun.addJob("pdf",absOutFile+".pdf");
    }
    else
    {
      dotRun.addJob("ps",absOutFile+".eps");
    }
  }

  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
     return;
  }

  Doxygen::indexList->addImageFile(imgName);

}

/*! Writes user defined image map to the output.
 *  \param t text stream to write to
 *  \param inFile just the basename part of the filename
 *  \param outDir output directory
 *  \param relPath relative path the to root of the output dir
 *  \param baseName the base name of the output files
 *  \param context the scope in which this graph is found (for resolving links)
 *  \param graphId a unique id for this graph, use for dynamic sections
 */
void writeDotImageMapFromFile(FTextStream &t,
                            const QCString &inFile, const QCString &outDir,
                            const QCString &relPath, const QCString &baseName,
                            const QCString &context,int graphId)
{

  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir.data()); exit(1);
  }

  QCString mapName = baseName+".map";
  QCString imgExt = getDotImageExtension();
  QCString imgName = baseName+"."+imgExt;
  QCString absOutFile = d.absPath().utf8()+"/"+mapName;

  DotRunner dotRun(inFile, QCString());
  dotRun.addJob(MAP_CMD,absOutFile);
  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    QCString svgName=outDir+"/"+baseName+".svg";
    DotFilePatcher::writeSVGFigureLink(t,relPath,baseName,svgName);
    DotFilePatcher patcher(svgName);
    patcher.addSVGConversion("",TRUE,context,TRUE,graphId);
    patcher.run();
  }
  else // bitmap graphics
  {
    QGString result;
    FTextStream tt(&result);

    t << "<img src=\"" << relPath << imgName << "\" alt=\""
      << imgName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>" << endl;
    DotFilePatcher::convertMapFile(tt, absOutFile, relPath ,TRUE, context);
    if (!result.isEmpty())
    {
      t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">";
      t << result;
      t << "</map>" << endl;
    }
  }
  d.remove(absOutFile);
}

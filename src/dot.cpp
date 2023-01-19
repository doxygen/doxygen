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

#include <cstdlib>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <mutex>

#include "config.h"
#include "dot.h"
#include "dotrunner.h"
#include "dotfilepatcher.h"
#include "util.h"
#include "portable.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"
#include "indexlist.h"
#include "dir.h"

#define MAP_CMD "cmapx"

//--------------------------------------------------------------------

static QCString g_dotFontPath;

static std::mutex g_dotManagerMutex;

static void setDotFontPath(const QCString &path)
{
  ASSERT(g_dotFontPath.isEmpty());
  g_dotFontPath = Portable::getenv("DOTFONTPATH");
  QCString newFontPath = Config_getString(DOT_FONTPATH);
  if (!newFontPath.isEmpty() && !path.isEmpty())
  {
    newFontPath.prepend(path+Portable::pathListSeparator());
  }
  else if (newFontPath.isEmpty() && !path.isEmpty())
  {
    newFontPath=path;
  }
  else
  {
    Portable::unsetenv("DOTFONTPATH");
    return;
  }
  Portable::setenv("DOTFONTPATH",newFontPath);
}

static void unsetDotFontPath()
{
  if (g_dotFontPath.isEmpty())
  {
    Portable::unsetenv("DOTFONTPATH");
  }
  else
  {
    Portable::setenv("DOTFONTPATH",g_dotFontPath);
  }
  g_dotFontPath="";
}

//--------------------------------------------------------------------

DotManager *DotManager::instance()
{
  static DotManager theInstance;
  return &theInstance;
}

DotManager::DotManager() : m_runners(), m_filePatchers()
{
  m_queue = new DotRunnerQueue;
  int i;
  int dotNumThreads = Config_getInt(DOT_NUM_THREADS);
  if (dotNumThreads!=1)
  {
    for (i=0;i<dotNumThreads;i++)
    {
      DotWorkerThreadPtr thread(new DotWorkerThread(m_queue));
      thread->start();
      if (thread->isRunning())
      {
        m_workers.push_back(std::move(thread));
      }
      else // no more threads available!
      {
      }
    }
    ASSERT(m_workers.size()>0);
  }
}

DotManager::~DotManager()
{
  if (!Doxygen::terminating) delete m_queue;
}

DotRunner* DotManager::createRunner(const QCString &absDotName, const QCString& md5Hash)
{
  std::lock_guard<std::mutex> lock(g_dotManagerMutex);
  DotRunner* rv = nullptr;
  auto const runit = m_runners.find(absDotName.str());
  if (runit == m_runners.end())
  {
    auto insobj = std::make_unique<DotRunner>(absDotName, md5Hash);
    rv = insobj.get();
    m_runners.emplace(absDotName.str(), std::move(insobj));
  }
  else
  {
    // we have a match
    if (md5Hash != runit->second->getMd5Hash())
    {
      err("md5 hash does not match for two different runs of %s !\n", qPrint(absDotName));
    }
    rv = runit->second.get();
  }
  assert(rv);
  return rv;
}

DotFilePatcher *DotManager::createFilePatcher(const QCString &fileName)
{
  std::lock_guard<std::mutex> lock(g_dotManagerMutex);
  auto patcher = m_filePatchers.find(fileName.str());

  if (patcher != m_filePatchers.end()) return &(patcher->second);

  auto rv = m_filePatchers.emplace(std::make_pair(fileName.str(), fileName));
  assert(rv.second);
  return &(rv.first->second);
}

bool DotManager::run() const
{
  size_t numDotRuns = m_runners.size();
  size_t numFilePatchers = m_filePatchers.size();
  if (numDotRuns+numFilePatchers>1)
  {
    if (m_workers.size()==0)
    {
      msg("Generating dot graphs in single threaded mode...\n");
    }
    else
    {
      msg("Generating dot graphs using %zu parallel threads...\n",std::min(numDotRuns+numFilePatchers,m_workers.size()));
    }
  }
  size_t i=1;

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
  // fill work queue with dot operations
  size_t prev=1;
  if (m_workers.size()==0) // no threads to work with
  {
    for (auto & dr : m_runners)
    {
      msg("Running dot for graph %zu/%zu\n",prev,numDotRuns);
      dr.second->run();
      prev++;
    }
  }
  else // use multiple threads to run instances of dot in parallel
  {
    for (auto & dr: m_runners)
    {
      m_queue->enqueue(dr.second.get());
    }
    // wait for the queue to become empty
    while ((i=m_queue->size())>0)
    {
      i = numDotRuns - i;
      while (i>=prev)
      {
        msg("Running dot for graph %zu/%zu\n",prev,numDotRuns);
        prev++;
      }
      Portable::sleep(100);
    }
    while (numDotRuns>=prev)
    {
      msg("Running dot for graph %zu/%zu\n",prev,numDotRuns);
      prev++;
    }
    // signal the workers we are done
    for (i=0;i<m_workers.size();i++)
    {
      m_queue->enqueue(0); // add terminator for each worker
    }
    // wait for the workers to finish
    for (i=0;i<m_workers.size();i++)
    {
      m_workers.at(i)->wait();
    }
  }
  if (setPath)
  {
    unsetDotFontPath();
  }

  // patch the output file and insert the maps and figures
  i=1;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and
  // then the other files.
  for (auto & fp : m_filePatchers)
  {
    if (fp.second.isSVGFile())
    {
      msg("Patching output file %zu/%zu\n",i,numFilePatchers);
      if (!fp.second.run()) return FALSE;
      i++;
    }
  }
  for (auto& fp : m_filePatchers)
  {
    if (!fp.second.isSVGFile())
    {
      msg("Patching output file %zu/%zu\n",i,numFilePatchers);
      if (!fp.second.run()) return FALSE;
      i++;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------

void writeDotGraphFromFile(const QCString &inFile,const QCString &outDir,
                           const QCString &outFile,GraphOutputFormat format,
                           const QCString &srcFile,int srcLine)
{
  Dir d(outDir.str());
  if (!d.exists())
  {
    term("Output dir %s does not exist!\n",qPrint(outDir));
  }

  QCString imgExt = getDotImageExtension();
  QCString imgName = QCString(outFile)+"."+imgExt;
  QCString absImgName = QCString(d.absPath())+"/"+imgName;
  QCString absOutFile = QCString(d.absPath())+"/"+outFile;

  DotRunner dotRun(inFile);
  if (format==GOF_BITMAP)
  {
    dotRun.addJob(Config_getEnumAsString(DOT_IMAGE_FORMAT),absImgName,srcFile,srcLine);
  }
  else // format==GOF_EPS
  {
    if (Config_getBool(USE_PDFLATEX))
    {
      dotRun.addJob("pdf",absOutFile+".pdf",srcFile,srcLine);
    }
    else
    {
      dotRun.addJob("ps",absOutFile+".eps",srcFile,srcLine);
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
 *  \param srcFile the source file
 *  \param srcLine the line number in the source file
 */
void writeDotImageMapFromFile(TextStream &t,
                            const QCString &inFile, const QCString &outDir,
                            const QCString &relPath, const QCString &baseName,
                            const QCString &context,int graphId,
                            const QCString &srcFile,int srcLine)
{

  Dir d(outDir.str());
  if (!d.exists())
  {
    term("Output dir %s does not exist!\n",qPrint(outDir));
  }

  QCString mapName = baseName+".map";
  QCString imgExt = getDotImageExtension();
  QCString imgName = baseName+"."+imgExt;
  QCString absOutFile = QCString(d.absPath())+"/"+mapName;

  DotRunner dotRun(inFile);
  dotRun.addJob(MAP_CMD,absOutFile,srcFile,srcLine);
  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    QCString svgName = outDir+"/"+baseName+".svg";
    DotFilePatcher::writeSVGFigureLink(t,relPath,baseName,svgName);
    DotFilePatcher patcher(svgName);
    patcher.addSVGConversion("",TRUE,context,TRUE,graphId);
    patcher.run();
  }
  else // bitmap graphics
  {
    TextStream tt;
    t << "<img src=\"" << relPath << imgName << "\" alt=\""
      << imgName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>\n";
    DotFilePatcher::convertMapFile(tt, absOutFile, relPath ,TRUE, context);
    if (!tt.empty())
    {
      t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">";
      t << tt.str();
      t << "</map>\n";
    }
  }
  d.remove(absOutFile.str());
}

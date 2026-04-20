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

DotManager::DotManager() : m_runner(), m_filePatchers()
{
}

DotManager::~DotManager()
{
}

void DotManager::addJob(const QCString &absPath, const QCString &relDotName, const QCString &format,
                        const QCString &md5Hash, const QCString &srcFile, int srcLine)
{
  std::lock_guard<std::mutex> lock(g_dotManagerMutex);
  for (const auto &job : m_jobs)
  {
    if (job.absPath == absPath && job.relDotName == relDotName && job.format == format) return; // already queued
    if (job.absPath == absPath && job.relDotName == relDotName && job.md5Hash != md5Hash)
    {
      err("md5 hash does not match for two different runs of {}{} !\n", absPath, relDotName);
      return;
    }
  }
  m_jobs.emplace_back(format, absPath, relDotName, md5Hash, srcFile, srcLine);
}

DotFilePatcher *DotManager::createFilePatcher(const QCString &fileName)
{
  std::lock_guard<std::mutex> lock(g_dotManagerMutex);
  auto patcher = m_filePatchers.find(fileName.str());

  if (patcher != m_filePatchers.end()) return &(patcher->second);

  auto rv = m_filePatchers.emplace(fileName.str(), fileName);
  assert(rv.second);
  return &(rv.first->second);
}

bool DotManager::run()
{
  for (const auto &job: m_jobs)
  {
    m_runner.addJob(job);
  }

  size_t numFilePatchers = m_filePatchers.size();
  if (numFilePatchers > 0)
  {
    msg("Generating dot graphs...\n");
  }

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

  bool ok = m_runner.run();

  if (setPath)
  {
    unsetDotFontPath();
  }

  if (!ok) return FALSE;

  // patch the output file and insert the maps and figures
  size_t i=1;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and
  // then the other files.
  for (auto & fp : m_filePatchers)
  {
    if (fp.second.isSVGFile())
    {
      msg("Patching output file {}/{}\n",i,numFilePatchers);
      if (!fp.second.run()) return FALSE;
      i++;
    }
  }
  for (auto& fp : m_filePatchers)
  {
    if (!fp.second.isSVGFile())
    {
      msg("Patching output file {}/{}\n",i,numFilePatchers);
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
    term("Output dir {} does not exist!\n",outDir);
  }

  QCString imgExt = getDotImageExtension();
  QCString imgName = outFile+"."+imgExt;
  QCString absImgName = d.absPath()+"/"+imgName;
  QCString absOutFile = d.absPath()+"/"+outFile;

  QCString dotArgs;
  if (format==GraphOutputFormat::BITMAP)
  {
    dotArgs = QCString("-T") + Config_getEnumAsString(DOT_IMAGE_FORMAT) + " -o \"" + absImgName + "\" \"" + inFile + "\"";
  }
  else // format==GraphOutputFormat::EPS
  {
    if (Config_getBool(USE_PDFLATEX))
    {
      dotArgs = QCString("-Tpdf -o \"") + absOutFile + ".pdf\" \"" + inFile + "\"";
    }
    else
    {
      dotArgs = QCString("-Tps -o \"") + absOutFile + ".eps\" \"" + inFile + "\"";
    }
  }

  if (Portable::system(Doxygen::verifiedDotPath, dotArgs, FALSE) != 0)
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
 *  \param newFile signal whether or not the file has been generated before (value `false`) or not.
 */
void writeDotImageMapFromFile(TextStream &t,
                            const QCString &inFile, const QCString &outDir,
                            const QCString &relPath, const QCString &baseName,
                            const QCString &context,int graphId,
                            const QCString &srcFile,int srcLine, bool newFile)
{

  Dir d(outDir.str());
  if (!d.exists())
  {
    term("Output dir {} does not exist!\n",outDir);
  }

  QCString mapName = baseName+".map";
  QCString imgExt = getDotImageExtension();
  QCString imgName = baseName+"."+imgExt;
  QCString absOutFile = d.absPath()+"/"+mapName;

  QCString dotArgs = QCString("-T" MAP_CMD " -o \"") + absOutFile + "\" \"" + inFile + "\"";
  if (Portable::system(Doxygen::verifiedDotPath, dotArgs, FALSE) != 0)
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    QCString svgName = outDir+"/"+baseName+".svg";
    DotFilePatcher::writeSVGFigureLink(t,relPath,baseName,svgName);
    if (newFile)
    {
      DotFilePatcher patcher(svgName);
      patcher.addSVGConversion("",TRUE,context,TRUE,graphId);
      patcher.run();
    }
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

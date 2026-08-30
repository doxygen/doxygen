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

// own header
#include "dot.h"

// standard includes
#include <algorithm>
#include <cassert>
#include <mutex>
#include <sstream>

// other includes
#include "config.h"
#include "dir.h"
#include "doxygen.h"
#include "indexlist.h"
#include "message.h"
#include "portable.h"
#include "textstream.h"
#include "util.h"

#define MAP_CMD "cmapx"

//--------------------------------------------------------------------

static DString g_dotFontPath;

static std::mutex g_dotManagerMutex;

static void setDotFontPath(const DString &path)
{
  ASSERT(g_dotFontPath.empty());
  g_dotFontPath = Portable::getenv("DOTFONTPATH");
  DString newFontPath = Config_getString(DOT_FONTPATH);
  if (!newFontPath.empty() && !path.empty())
  {
    newFontPath.prepend(path+Portable::pathListSeparator());
  }
  else if (newFontPath.empty() && !path.empty())
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
  if (g_dotFontPath.empty())
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

DotManager::DotManager() : m_filePatchers()
{
}

DotManager::~DotManager()
{
}

void DotManager::addJob(const DotJob &newJob)
{
  std::lock_guard<std::mutex> lock(g_dotManagerMutex);
  for (const auto &job : m_jobs)
  {
    if (job.absPath == newJob.absPath && job.relDotName == newJob.relDotName && job.format == newJob.format) return; // already queued
    if (job.absPath == newJob.absPath && job.relDotName == newJob.relDotName && job.md5Hash != newJob.md5Hash)
    {
      err("md5 hash does not match for two different runs of {}{} !\n", newJob.absPath, newJob.relDotName);
      return;
    }
  }
  m_jobs.push_back(newJob);
}

DotFilePatcher *DotManager::createFilePatcher(const DString &fileName)
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
  size_t numFilePatchers = m_filePatchers.size();
  if (m_jobs.size()+numFilePatchers > 0)
  {
    if (Config_getInt(DOT_NUM_THREADS)<=1)
    {
      msg("Generating {:d} dot graphs in single threaded mode with batch size {:d}...\n",
          m_jobs.size(),Config_getInt(DOT_BATCH_SIZE));
    }
    else
    {
      msg("Generating {:d} dot graphs using {:d} parallel threads with batch size {:d}...\n",
          m_jobs.size(),Config_getInt(DOT_NUM_THREADS),Config_getInt(DOT_BATCH_SIZE));
    }
  }

  bool setPath=false;
  if (Config_getBool(GENERATE_HTML))
  {
    setDotFontPath(Config_getString(HTML_OUTPUT));
    setPath=true;
  }
  else if (Config_getBool(GENERATE_LATEX))
  {
    setDotFontPath(Config_getString(LATEX_OUTPUT));
    setPath=true;
  }
  else if (Config_getBool(GENERATE_RTF))
  {
    setDotFontPath(Config_getString(RTF_OUTPUT));
    setPath=true;
  }
  else if (Config_getBool(GENERATE_DOCBOOK))
  {
    setDotFontPath(Config_getString(DOCBOOK_OUTPUT));
    setPath=true;
  }

  bool ok = m_runner.run(m_jobs);

  if (setPath)
  {
    unsetDotFontPath();
  }

  if (!ok) return false;

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
      if (!fp.second.run()) return false;
      i++;
    }
  }
  for (auto& fp : m_filePatchers)
  {
    if (!fp.second.isSVGFile())
    {
      msg("Patching output file {}/{}\n",i,numFilePatchers);
      if (!fp.second.run()) return false;
      i++;
    }
  }
  return true;
}

//--------------------------------------------------------------------

void writeDotGraphFromFile(const DString &inFile,const DString &outDir,
                           const DString &outFile,GraphOutputFormat format,
                           const DString &srcFile,int srcLine,bool toIndex)
{
  Dir d(outDir.str());
  if (!d.exists())
  {
    term("Output dir {} does not exist!\n",outDir);
  }

  DString imgExt = getDotImageExtension();
  DString imgName = outFile+"."+imgExt;
  DString absImgName = d.absPath()+"/"+imgName;
  DString absOutFile = d.absPath()+"/"+outFile;

  DString dotArgs;
  if (format==GraphOutputFormat::BITMAP)
  {
    dotArgs = DString("-T") + Config_getEnumAsString(DOT_IMAGE_FORMAT) + " -o \"" + absImgName + "\" \"" + inFile + "\"";
  }
  else // format==GraphOutputFormat::EPS
  {
    if (Config_getBool(USE_PDFLATEX))
    {
      dotArgs = DString("-Tpdf -o \"") + absOutFile + ".pdf\" \"" + inFile + "\"";
    }
    else
    {
      dotArgs = DString("-Teps -o \"") + absOutFile + ".eps\" \"" + inFile + "\"";
    }
  }

  if (Portable::system(Doxygen::verifiedDotPath, dotArgs, false) != 0)
  {
    return;
  }

  if (toIndex) Doxygen::indexList->addImageFile(imgName);

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
                            const DString &inFile, const DString &outDir,
                            const DString &relPath, const DString &baseName,
                            const DString &context,int graphId,
                            const DString &srcFile,int srcLine, bool newFile)
{

  Dir d(outDir.str());
  if (!d.exists())
  {
    term("Output dir {} does not exist!\n",outDir);
  }

  DString mapName = baseName+".cmapx";
  DString imgExt = getDotImageExtension();
  DString imgName = baseName+"."+imgExt;
  DString absOutFile = d.absPath()+"/"+mapName;

  DString dotArgs = DString("-T" MAP_CMD " -o \"") + absOutFile + "\" \"" + inFile + "\"";
  if (Portable::system(Doxygen::verifiedDotPath, dotArgs, false) != 0)
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    DString svgName = outDir+"/"+baseName+".svg";
    DotFilePatcher::writeSVGFigureLink(t,relPath,baseName,svgName);
    if (newFile)
    {
      DotFilePatcher patcher(svgName);
      patcher.addSVGConversion("",true,context,true,graphId);
      patcher.run();
    }
  }
  else // bitmap graphics
  {
    TextStream tt;
    t << "<img src=\"" << relPath << imgName << "\" alt=\""
      << imgName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>\n";
    DotFilePatcher::convertMapFile(tt, absOutFile, relPath ,true, context);
    if (!tt.empty())
    {
      t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">";
      t << tt.str();
      t << "</map>\n";
    }
  }
  d.remove(absOutFile.str());
}

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

#include <mutex>
#include <fstream>

#include "mermaid.h"
#include "util.h"
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "message.h"
#include "debug.h"
#include "dir.h"
#include "indexlist.h"
#include "threadpool.h"

static std::mutex g_mermaidMutex;
static int        g_mermaidIndex = 1;

MermaidManager &MermaidManager::instance()
{
  static MermaidManager theInstance;
  return theInstance;
}

MermaidManager::MermaidManager()
{
}

QCString MermaidManager::imageExtension(OutputFormat format)
{
  switch (format)
  {
    case OutputFormat::Bitmap: return ".png";
    case OutputFormat::SVG:    return ".svg";
    case OutputFormat::PDF:    return ".pdf";
  }
  return ".png";
}

QCString MermaidManager::writeMermaidSource(const QCString &outDirArg, const QCString &fileName,
                                            const QCString &content, OutputFormat format,
                                            const QCString &srcFile, int srcLine)
{
  QCString outDir(outDirArg);
  QCString baseName;

  // strip any trailing slashes and backslashes
  while (!outDir.isEmpty() && (outDir.at(outDir.length()-1)=='/' || outDir.at(outDir.length()-1)=='\\'))
  {
    outDir = outDir.left(outDir.length()-1);
  }

  if (fileName.isEmpty())
  {
    std::lock_guard<std::mutex> lock(g_mermaidMutex);
    baseName = outDir + "/inline_mermaid_" + QCString().setNum(g_mermaidIndex++);
  }
  else
  {
    baseName = fileName;
    int i = baseName.findRev('.');
    if (i != -1) baseName = baseName.left(i);
    baseName.prepend(outDir + "/");
  }

  QCString mmdName = baseName + ".mmd";

  Debug::print(Debug::Mermaid, 0, "*** writeMermaidSource baseName: {}\n", baseName);
  Debug::print(Debug::Mermaid, 0, "*** writeMermaidSource mmdName: {}\n", mmdName);

  // Write the .mmd source file
  std::ofstream file = Portable::openOutputStream(mmdName);
  if (!file.is_open())
  {
    err_full(srcFile, srcLine, "Could not open file {} for writing", mmdName);
    return baseName;
  }
  file.write(content.data(), content.length());
  file.close();

  // Store for batch processing in run()
  m_diagrams.emplace_back(format,MermaidDiagramInfo(baseName, content, outDir, srcFile, srcLine));

  return baseName;
}

void MermaidManager::generateMermaidOutput(const QCString &baseName, const QCString &/*outDir*/,
                                           OutputFormat format)
{
  QCString imgName = baseName;
  int i = imgName.findRev('/');
  if (i != -1)
  {
    imgName = imgName.mid(i + 1);
  }
  imgName += imageExtension(format);
  Doxygen::indexList->addImageFile(imgName);
}

static void runMermaid(const MermaidManager::DiagramList &diagrams)
{
  //printf("runMermaidContent for %zu images\n",contentList.size());
  if (diagrams.empty()) return;

  QCString mmdc = Config_getString(MERMAID_PATH);
  if (!mmdc.isEmpty() && mmdc.at(mmdc.length()-1) != '/' && mmdc.at(mmdc.length()-1) != '\\')
  {
    mmdc += "/";
  }
  mmdc += "mmdc";

  QCString mermaidConfigFile = Config_getString(MERMAID_CONFIG_FILE);

  struct MermaidCmd
  {
    MermaidCmd(const QCString &mmdc_,const QCString &args_,const QCString &ext_,const QCString &srcFile_,int srcLine_) :
      mmdc(mmdc_), args(args_), ext(ext_), srcFile(srcFile_), srcLine(srcLine_) {}
    QCString mmdc;
    QCString args;
    QCString ext;
    QCString srcFile;
    int srcLine;
  };
  std::vector<MermaidCmd> mermaidCmds;

  for (const auto &diagram : diagrams)
  {
    //printf("content=%s\n",qPrint(mc.content));
    if (diagram.info.content.isEmpty()) continue;

    QCString ext;
    switch (diagram.format)
    {
      case MermaidManager::OutputFormat::Bitmap: ext = "png"; break;
      case MermaidManager::OutputFormat::SVG:    ext = "svg"; break;
      case MermaidManager::OutputFormat::PDF:    ext = "pdf"; break;
    }

    QCString inputFile  = diagram.info.baseName + ".mmd";
    QCString outputFile = diagram.info.baseName + "." + ext;

    // Check if content has changed since last run (caching)
    FileInfo fi(outputFile.str());
    if (fi.exists())
    {
      QCString cachedContent = fileToString(inputFile);
      if (cachedContent == diagram.info.content)
      {
        continue;
      }
    }

    // Build the mmdc command arguments
    QCString args;
    args += "-q -i \"" + inputFile + "\" ";
    args += "-o \"" + outputFile + "\" ";

    if (!mermaidConfigFile.isEmpty())
    {
      args += "-c \"" + mermaidConfigFile + "\" ";
    }

    mermaidCmds.emplace_back(mmdc, args,ext, diagram.info.srcFile, diagram.info.srcLine);
  }

  std::size_t numThreads = static_cast<std::size_t>(Config_getInt(DOT_NUM_THREADS));
  size_t offset=0;
  size_t total=mermaidCmds.size();
  msg("Generating {} Mermaid files using {} threads\n", total, numThreads);
  if (numThreads>1) // multi threaded version
  {
    ThreadPool threadPool(numThreads);
    std::vector< std::future<int> > results;

    // queue the work
    for (const auto &cmd : mermaidCmds)
    {
      auto processFile = [&cmd]()
      {
        Debug::print(Debug::Mermaid, 0, "*** MermaidManager::run Running: {} {}\n", cmd.mmdc, cmd.args);
        int exitCode = Portable::system(cmd.mmdc.data(), cmd.args.data(), TRUE);
        if (exitCode != 0)
        {
          err_full(cmd.srcFile, cmd.srcLine,
              "Problems running Mermaid (mmdc). Verify that the command '{} {}' works from the command line. Exit code: {}.",
              cmd.mmdc, cmd.args, exitCode);
        }
        return exitCode;
      };
      results.emplace_back(threadPool.queue(processFile));
    }

    // wait for the results
    for (auto &f : results)
    {
      offset++;
      msg("Generating Mermaid file {}/{}\n", offset, total);
      f.get();
    }
  }
  else // single threaded version
  {
    for (const auto &cmd : mermaidCmds)
    {
      offset++;
      msg("Generating Mermaid file {}/{}\n", offset, total);
      Debug::print(Debug::Mermaid, 0, "*** MermaidManager::run Running: {} {}\n", cmd.mmdc, cmd.args);

      int exitCode = Portable::system(cmd.mmdc.data(), cmd.args.data(), TRUE);
      if (exitCode != 0)
      {
        err_full(cmd.srcFile, cmd.srcLine,
            "Problems running Mermaid (mmdc). Verify that the command '{} {}' works from the command line. Exit code: {}.",
            cmd.mmdc, cmd.args, exitCode);
      }
    }
  }
}

void MermaidManager::run()
{
  Debug::print(Debug::Mermaid, 0, "*** MermaidManager::run\n");
  runMermaid(m_diagrams);
}

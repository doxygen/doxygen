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
    case MERM_BITMAP: return ".png";
    case MERM_SVG:    return ".svg";
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
  MermaidContent mc(baseName, content, outDir, srcFile, srcLine);
  switch (format)
  {
    case MERM_BITMAP: m_pngContent.push_back(mc); break;
    case MERM_SVG:    m_svgContent.push_back(mc); break;
  }

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

static void runMermaidContent(const MermaidManager::ContentList &contentList,
                              MermaidManager::OutputFormat format)
{
  //printf("runMermaidContent for %zu images\n",contentList.size());
  if (contentList.empty()) return;

  QCString mmdc = Config_getString(MERMAID_PATH);
  if (!mmdc.isEmpty() && mmdc.at(mmdc.length()-1) != '/' && mmdc.at(mmdc.length()-1) != '\\')
  {
    mmdc += "/";
  }
  mmdc += "mmdc";

  QCString mermaidConfigFile = Config_getString(MERMAID_CONFIG_FILE);

  QCString ext;
  switch (format)
  {
    case MermaidManager::MERM_BITMAP: ext = "png"; break;
    case MermaidManager::MERM_SVG:    ext = "svg"; break;
  }

  for (const auto &mc : contentList)
  {
    //printf("content=%s\n",qPrint(mc.content));
    if (mc.content.isEmpty()) continue;

    QCString inputFile  = mc.baseName + ".mmd";
    QCString outputFile = mc.baseName + "." + ext;

    // Check if content has changed since last run (caching)
    FileInfo fi(outputFile.str());
    if (fi.exists())
    {
      QCString cachedContent = fileToString(inputFile);
      if (cachedContent == mc.content) continue;
    }

    // Build the mmdc command arguments
    QCString args;
    args += "-i \"" + inputFile + "\" ";
    args += "-o \"" + outputFile + "\" ";

    if (!mermaidConfigFile.isEmpty())
    {
      args += "-c \"" + mermaidConfigFile + "\" ";
    }

    msg("Generating Mermaid {} file {}\n", ext, outputFile);
    Debug::print(Debug::Mermaid, 0, "*** MermaidManager::run Running: {} {}\n", mmdc, args);

    int exitCode = Portable::system(mmdc.data(), args.data(), TRUE);
    if (exitCode != 0)
    {
      err_full(mc.srcFile, mc.srcLine,
               "Problems running Mermaid (mmdc). Verify that the command '{}' works from the command line. Exit code: {}.",
               mmdc, exitCode);
    }
  }
}

void MermaidManager::run()
{
  Debug::print(Debug::Mermaid, 0, "*** MermaidManager::run\n");
  runMermaidContent(m_pngContent, MERM_BITMAP);
  runMermaidContent(m_svgContent, MERM_SVG);
}

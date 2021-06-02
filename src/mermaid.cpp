/******************************************************************************
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

#include "mermaid.h"
#include "util.h"
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "message.h"
#include "debug.h"
#include "fileinfo.h"
#include "dir.h"

/*
Notes

Cloned structure from PlantUML implementation.
Works, but hackish. I need to understand better how this is going to be used to generate SVG and PDF from
the other format types. Perhaps set the config to make SVG or a latex version and see what happens?

I'm not sure if the cache feature is useful for Mermaid since they need to be in separate files. Remove it.
Need to remove the 'engine' functionality as it's PlantUML specific.

The visitor calls six times because there are two diagrams, 2 copies of jExecute (new and old) and the
new one gets 2 entries for some reason (differ by a g and a h in the filename).
 */

QCString MermaidManager::writeMermaidSource(const QCString &outDirArg,const QCString &fileName,
                                            const QCString &content,OutputFormat format, const QCString &engine,
                                            const QCString &srcFile,int srcLine)
{
  QCString baseName;
  QCString mmdName;
  QCString imgName;
  QCString outDir(outDirArg);
  static int umlindex=1;
  msg("mermaid: writeMermaidSource fileName %s outDir %s\n",qPrint(fileName),qPrint(outDir));
  Debug::print(Debug::Mermaid,0,"*** %s fileName: %s\n","writeMermaidSource",qPrint(fileName));
  Debug::print(Debug::Mermaid,0,"*** %s outDir: %s\n","writeMermaidSource",qPrint(outDir));

  // strip any trailing slashes and backslashes
  uint l;
  while ((l=outDir.length())>0 && (outDir.at(l-1)=='/' || outDir.at(l-1)=='\\'))
  {
    outDir = outDir.left(l-1);
  }

  if (fileName.isEmpty()) // generate name
  {
    mmdName = "inline_mermaidgraph_"+QCString().setNum(umlindex);
    baseName = outDir+"/inline_mermaidgraph_"+QCString().setNum(umlindex++);
  }
  else // user specified name
  {
    baseName = fileName;
    int i=baseName.findRev('.');
    if (i!=-1) baseName = baseName.left(i);
    mmdName = baseName;
    baseName.prepend(outDir+"/");
  }

  switch (format)
  {
    case MERMAID_BITMAP:
      imgName = mmdName+".png";
      break;
    case MERMAID_PDF:
      imgName = mmdName+".pdf";
      break;
    case MERMAID_SVG:
      imgName = mmdName+".svg";
      break;
  }

  Debug::print(Debug::Mermaid,0,"*** %s baseName: %s\n","writeMermaidSource",qPrint(baseName));
  Debug::print(Debug::Mermaid,0,"*** %s mmdName: %s\n","writeMermaidSource",qPrint(mmdName));
  Debug::print(Debug::Mermaid,0,"*** %s imgName: %s\n","writeMermaidSource",qPrint(imgName));
  msg("mermaid: writeMermaidSource baseName %s mmdName %s imgName %s\n",qPrint(baseName),qPrint(mmdName),qPrint(imgName));
  QCString text = "%% ";
  text += mmdName;
  text += "\n";
  text += content;
  text += "\n";

  QCString qcOutDir(outDir);
  uint pos = qcOutDir.findRev("/");
  QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
  Debug::print(Debug::Mermaid,0,"*** %s generateType: %s\n","writeMermaidSource",qPrint(generateType));
  MermaidManager::instance().insert(mmdName.str(),mmdName.str(),outDir,format,text,srcFile,srcLine);
  Debug::print(Debug::Mermaid,0,"*** %s generateType: %s\n","writeMermaidSource",qPrint(generateType));
  std::cout << "Mermaid filname " << mmdName.str() << std::endl;
  return baseName;
}

void MermaidManager::generateMermaidOutput(const QCString &baseName,const QCString &outDir,OutputFormat format)
{
  //QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  //QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  //QCString dotPath = Config_getString(DOT_PATH);
  msg("mermaid: generateMermaidOutput\n");
  QCString imgName = baseName;
  // The basename contains path, we need to strip the path from the filename in order
  // to create the image file name which should be included in the index.qhp (Qt help index file).
  int i;
  if ((i=imgName.findRev('/'))!=-1) // strip path
  {
    imgName=imgName.right(imgName.length()-i-1);
  }
  switch (format)
  {
    case MERMAID_BITMAP:
      imgName+=".png";
      break;
    case MERMAID_PDF:
      imgName+=".pdf";
      break;
    case MERMAID_SVG:
      imgName+=".svg";
      break;
  }

  Doxygen::indexList->addImageFile(imgName);
}

//--------------------------------------------------------------------


MermaidManager &MermaidManager::instance()
{
  static MermaidManager theInstance;
  return theInstance;
}

MermaidManager::MermaidManager()
{
  QCString outputFilename = Config_getString(OUTPUT_DIRECTORY) + "/" + MERMAID_CACHE_FILENAME;
  FileInfo fi(outputFilename.str());
  if (fi.exists())
  {
    //m_cachedMermaidAllContent = fileToString(outputFilename);
  }
  Debug::print(Debug::Mermaid,0,"*** instance() : m_cachedMermaidAllContent = [%s]\n",qPrint(m_cachedMermaidAllContent));
}

static void runMermaidContent(const MermaidManager::FilesMap &mermaidFiles,
                               const MermaidManager::ContentMap &mermaidContent,
                               MermaidManager::OutputFormat format)
{
    msg("mermaid: runMermaidContent\n");
  /* example : running: mmdc -o "test_doxygen/DOXYGEN_OUTPUT/html/A.png"
                             -i "test_doxygen/DOXYGEN_OUTPUT/html/A.mmd"
                             -b "transparent" */
  int exitCode;
  //QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  //QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);

  QCString mermaidExe = "mmdc";
  QCString mermaidArgs = "";
  QCString mermaidType = "";
  QCString mmdOutDir = "";

  switch (format)
  {
    case MermaidManager::MERMAID_BITMAP:
      mermaidType="png";
      break;
    case MermaidManager::MERMAID_PDF:
      mermaidType="pdf";
      break;
    case MermaidManager::MERMAID_SVG:
      mermaidType="svg";
      break;
  }

  {
    for (const auto &kv : mermaidContent)
    {
      const MermaidContent &nb = kv.second;
      QCString mermaidArguments = mermaidArgs;
      msg("mermaid: Generating Mermaid %s Files in %s\n",qPrint(mermaidType),kv.first.c_str());
      mermaidArguments+="-o \"";
      mermaidArguments+=nb.outDir;
      mermaidArguments+="/";
      mermaidArguments+=kv.first.c_str();
      mermaidArguments+=".";
      mermaidArguments+=mermaidType;
      mermaidArguments+="\" ";

      QCString mmdFileName("");
      mmdFileName+=nb.outDir;
      mmdFileName+="/";
      mmdOutDir=mmdFileName;
      mmdFileName+=mermaidType;
      mmdFileName+=kv.first.c_str();
      mmdFileName+=".mmd";

      mermaidArguments+="-i \"";
      mermaidArguments+=mmdFileName;
      mermaidArguments+="\" -b \"transparent\"";

      std::ofstream file(mmdFileName.str(),std::ofstream::out | std::ofstream::binary);
      if (!file.is_open())
      {
        err_full(nb.srcFile,nb.srcLine,"Could not open file %s for writing\n",mmdFileName.data());
      }
      file.write( nb.content.data(), nb.content.length() );
      file.close();
      Debug::print(Debug::Mermaid,0,"*** %s Running Mermaid arguments:%s\n","MermaidManager::runMermaidContent",qPrint(mermaidArguments));

      Portable::sysTimerStart();
      msg("mermaid: running mermaid as %s %s\n",mermaidExe.data(),mermaidArguments.data());
      if ((exitCode=Portable::system(mermaidExe.data(),mermaidArguments.data(),TRUE))!=0)
      {
        err_full(nb.srcFile,nb.srcLine,"Problems running Mermaid. Verify that the command 'mmdc -h' works from the command line. Exit code: %d\n",
                 exitCode);
      }
      Portable::sysTimerStop();
    }
  }
}

void MermaidManager::run()
{
    msg("mermaid MermaidManager::run\n");

  Debug::print(Debug::Mermaid,0,"*** %s\n","MermaidManager::run");
  if (m_currentMermaidAllContent.isEmpty())
  {
      msg("mermaid: No mermaid content\n");
      return;
  }
  runMermaidContent(m_pngMermaidFiles, m_pngMermaidContent, MERMAID_BITMAP);
  runMermaidContent(m_svgMermaidFiles, m_svgMermaidContent, MERMAID_SVG);
  runMermaidContent(m_pdfMermaidFiles, m_pdfMermaidContent, MERMAID_PDF);
  QCString outputFilename = Config_getString(OUTPUT_DIRECTORY) + "/" + MERMAID_CACHE_FILENAME;
  std::ofstream file(outputFilename.str(),std::ofstream::out | std::ofstream::binary);
  if (!file.is_open())
  {
    err("Could not open file %s for writing\n",MERMAID_CACHE_FILENAME);
  }
  file.write( m_currentMermaidAllContent.data(), m_currentMermaidAllContent.length() );
  file.close();
}

static void print(const MermaidManager::FilesMap &mermaidFiles)
{
  if (Debug::isFlagSet(Debug::Mermaid))
  {
    for (const auto &kv : mermaidFiles)
    {
      Debug::print(Debug::Mermaid,0,"*** %s MermaidFiles key:%s size:%zu\n","MermaidManager::print Files",kv.first.c_str(),kv.second.size());
      for (const auto &s : kv.second)
      {
        Debug::print(Debug::Mermaid,0,"*** %s             list:%s\n","MermaidManager::print",s.c_str());
      }
    }
  }
}

static void print(const MermaidManager::ContentMap &mermaidContent)
{
  if (Debug::isFlagSet(Debug::Mermaid))
  {
    for (const auto &kv : mermaidContent)
    {
      Debug::print(Debug::Mermaid,0,"*** %s MermaidContent key:%s\n","MermaidManager::print Content",kv.first.c_str());
      Debug::print(Debug::Mermaid,0,"*** %s                 Content :%s\n","MermaidManager::print",kv.second.content.data());
    }
  }
}

static void addMermaidFiles(MermaidManager::FilesMap &mermaidFiles,
                            const std::string &key, const std::string &value)
{
    msg("mermaid: addMermaidFiles key %s\n",qPrint(key));
  auto kv = mermaidFiles.find(key);
  if (kv==mermaidFiles.end())
  {
    kv = mermaidFiles.insert(std::make_pair(key,StringVector())).first;
  }
  kv->second.push_back(value);
}

static void addMermaidContent(MermaidManager::ContentMap &mermaidContent,
                              const std::string &key, const QCString &outDir, const QCString &mmdContent,
                              const QCString &srcFile,int srcLine)
{
    msg("mermaid: addMermaidContent key %s\n", qPrint(key));
  auto kv = mermaidContent.find(key);
  if (kv==mermaidContent.end())
  {
    kv = mermaidContent.insert(std::make_pair(key,MermaidContent("",outDir,srcFile,srcLine))).first;
  }
  kv->second.content+=mmdContent;
}

void MermaidManager::insert(const std::string &key, const std::string &value,
                            const QCString &outDir,OutputFormat format,const QCString &mmdContent,
                            const QCString &srcFile,int srcLine)
{
  int find;
  msg("mermaid: MermaidManager::insert \n");
  Debug::print(Debug::Mermaid,0,"*** %s key:%s ,value:%s\n","MermaidManager::insert",qPrint(key),qPrint(value));

  m_currentMermaidAllContent+=mmdContent;

  find = m_cachedMermaidAllContent.find(mmdContent);
  Debug::print(Debug::Mermaid,0,"*** %s find: %d\n","MermaidManager::addMermaidContent",find);
  if (find >=0)
  {         // matched in cache. so we skip to run mermaid-cli for this content
      msg("mermaid: MermaidManager::insert found %s in cache\n", qPrint(mmdContent));
      return ;
  }

  switch (format)
  {
    case MERMAID_BITMAP:
      addMermaidFiles(m_pngMermaidFiles,key,value);
      print(m_pngMermaidFiles);
      addMermaidContent(m_pngMermaidContent,key,outDir,mmdContent,srcFile,srcLine);
      print(m_pngMermaidContent);
      break;
    case MERMAID_PDF:
      addMermaidFiles(m_pdfMermaidFiles,key,value);
      print(m_pdfMermaidFiles);
      addMermaidContent(m_pdfMermaidContent,key,outDir,mmdContent,srcFile,srcLine);
      print(m_pdfMermaidContent);
      break;
    case MERMAID_SVG:
      addMermaidFiles(m_svgMermaidFiles,key,value);
      print(m_svgMermaidFiles);
      addMermaidContent(m_svgMermaidContent,key,outDir,mmdContent,srcFile,srcLine);
      print(m_svgMermaidContent);
      break;
  }
}

//--------------------------------------------------------------------

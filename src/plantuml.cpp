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

#include <mutex>
#include "plantuml.h"
#include "util.h"
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "message.h"
#include "debug.h"
#include "fileinfo.h"
#include "dir.h"
#include "indexlist.h"
#include "stringutil.h"

static std::mutex g_PlantUmlMutex;

StringVector PlantumlManager::writePlantUMLSource(const QCString &outDirArg,const QCString &fileName,
                                              const QCString &content,OutputFormat format, const QCString &engine,
                                              const QCString &srcFile,int srcLine,bool inlineCode)
{
  StringVector baseNameVector;
  QCString baseName;
  QCString puName;
  QCString imgName;
  QCString outDir(outDirArg);

  Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource fileName: {}\n",fileName);
  Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource outDir: {}\n",outDir);

  // strip any trailing slashes and backslashes
  size_t l = 0;
  while ((l=outDir.length())>0 && (outDir.at(l-1)=='/' || outDir.at(l-1)=='\\'))
  {
    outDir = outDir.left(l-1);
  }

  generatePlantUmlFileNames(fileName,format,outDir,baseName,puName,imgName);

  Debug::print(Debug::Plantuml,0,"*** writePlantUMLSourcebaseName: {}\n",baseName);
  Debug::print(Debug::Plantuml,0,"*** writePlantUMLSourcebaseName puName: {}\n",puName);
  Debug::print(Debug::Plantuml,0,"*** writePlantUMLSourcebaseName imgName: {}\n",imgName);

  QCString text;
  if (inlineCode) text = "@start"+engine+" "+imgName+"\n";
  text.reserve(text.length()+content.length()+100); // add room for image name and end marker
  const char *p = content.data();
  if (p)
  {
    char c = 0;
    bool insideComment = false;
    QCString locEngine;
    while ((c=*p++))
    {
      text+=c;
      switch (c)
      {
        case '\'': insideComment=true; break;
        case '\n': insideComment=false; break;
        case '\t': break;
        case ' ':  break;
        case '@':
          if (!insideComment && literal_at(p,"start")) // @start...
          {
            locEngine.clear();
            p+=5;
            text += "start";
            while ((c=*p++) && isId(c))
            {
              locEngine += c;
              text+=c;
            }
            QCString inpName;
            QCString rest;

            // skip leading whitespace
            if (*p && (c==' ' || c=='\t'))
            {
              while ((c=*p++) && (c==' ' || c=='\t')) {}
            }
            // get everything till end or endOfLine, and split into inpName (without extension) and rest
            enum State { InName, InExt, InRest };
            State state = InName;
            while (*p && c!='\n')
            {
              switch (state)
              {
                case InName: // looking for the name part
                  if      (isId(c) || c=='-') { inpName+=c; }
                  else if (c=='.')            { state=InExt; }
                  else                        { rest+=c; state=InRest; }
                  break;
                case InExt: // skipping over extension part
                  if (!isId(c) && c!='-')     { rest+=c; state=InRest; }
                  break;
                case InRest: // gather rest until new line
                  rest+=c;
                  break;
              }
              c = *p++;
            }
            //printf("inpName='%s' rest='%s'\n",qPrint(inpName),qPrint(rest));
            generatePlantUmlFileNames(inpName,format,outDir,baseName,puName,imgName);

            // insert the image name
            text+=' ';
            text+=imgName;

            if (!rest.isEmpty())
            {
              text += '\n';
              text += rest;
            }
            if (c) text+=c;
          }
          else if (!insideComment && strncmp(p,("end"+locEngine).data(), 3+strlen(engine.data()))==0) // @end...
          {
            text += "end"+locEngine+"\n";
            p+=3+locEngine.length();
            if (!inlineCode)
            {
              QCString qcOutDir(substitute(outDir,"\\","/"));
              uint32_t pos = qcOutDir.findRev("/");
              QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
              Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource generateType: {}\n",generateType);
              PlantumlManager::instance().insert(generateType.str(),puName.str(),outDir,format,text,srcFile,srcLine);
              Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource generateType: {}\n",generateType);
              baseNameVector.push_back(baseName.str());
              text.clear();
            }
          }
          break;
        default:
          break;
      }
    }
    text+='\n';
  }
  if (inlineCode)
  {
    text +="@end"+engine+"\n";
    //printf("content\n====\n%s\n=====\n->\n-----\n%s\n------\n",qPrint(content),qPrint(text));
    QCString qcOutDir(substitute(outDir,"\\","/"));
    uint32_t pos = qcOutDir.findRev("/");
    QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
    Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource generateType: {}\n",generateType);
    PlantumlManager::instance().insert(generateType.str(),puName.str(),outDir,format,text,srcFile,srcLine);
    Debug::print(Debug::Plantuml,0,"*** writePlantUMLSource generateType: {}\n",generateType);
    baseNameVector.push_back(baseName.str());
  }

  return baseNameVector;
}

void PlantumlManager::generatePlantUmlFileNames(const QCString &fileName,OutputFormat format,const QCString &outDir,
                                                QCString &baseName,QCString &puName,QCString &imgName)
{
  static int umlindex=1;

  if (fileName.isEmpty()) // generate name
  {
    std::lock_guard<std::mutex> lock(g_PlantUmlMutex);
    puName = "inline_umlgraph_"+QCString().setNum(umlindex);
    baseName = outDir+"/inline_umlgraph_"+QCString().setNum(umlindex++);
  }
  else // user specified name
  {
    baseName = fileName;
    int i=baseName.findRev('.');
    if (i!=-1) baseName = baseName.left(i);
    puName = baseName;
    baseName.prepend(outDir+"/");
  }

  switch (format)
  {
    case PUML_BITMAP:
      imgName =puName+".png";
      break;
    case PUML_EPS:
      imgName =puName+".eps";
      break;
    case PUML_SVG:
      imgName =puName+".svg";
      break;
  }
}

void PlantumlManager::generatePlantUMLOutput(const QCString &baseName,const QCString &/* outDir */,OutputFormat format)
{
  QCString imgName = baseName;
  // The basename contains path, we need to strip the path from the filename in order
  // to create the image file name which should be included in the index.qhp (Qt help index file).
  int i = imgName.findRev('/');
  if (i!=-1) // strip path
  {
    imgName=imgName.mid(i+1);
  }
  switch (format)
  {
    case PUML_BITMAP:
      imgName+=".png";
      break;
    case PUML_EPS:
      imgName+=".eps";
      break;
    case PUML_SVG:
      imgName+=".svg";
      break;
  }

  Doxygen::indexList->addImageFile(imgName);
}

//--------------------------------------------------------------------


PlantumlManager &PlantumlManager::instance()
{
  static PlantumlManager theInstance;
  return theInstance;
}

PlantumlManager::PlantumlManager()
{
}

static void runPlantumlContent(const PlantumlManager::FilesMap &plantumlFiles,
                               const PlantumlManager::ContentMap &plantumlContent,
                               PlantumlManager::OutputFormat format)
{
  /* example : running: java -Djava.awt.headless=true
               -jar "/usr/local/bin/plantuml.jar"
               -o "test_doxygen/DOXYGEN_OUTPUT/html"
               -tpng
               "test_doxygen/DOXYGEN_OUTPUT/html/A.pu"
               -charset UTF-8
               outDir:test_doxygen/DOXYGEN_OUTPUT/html
               test_doxygen/DOXYGEN_OUTPUT/html/A
   */
  int exitCode = 0;
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);

  QCString pumlExe = "java";
  QCString pumlArgs = "";
  QCString pumlType = "";
  QCString pumlOutDir = "";

  const StringVector &pumlIncludePathList = Config_getList(PLANTUML_INCLUDE_PATH);
  {
    auto it = pumlIncludePathList.begin();
    if (it!=pumlIncludePathList.end())
    {
      pumlArgs += "-Dplantuml.include.path=\"";
      pumlArgs += it->c_str();
      ++it;
    }
    while (it!=pumlIncludePathList.end())
    {
      pumlArgs += Portable::pathListSeparator();
      pumlArgs += it->c_str();
      ++it;
    }
  }
  if (!pumlIncludePathList.empty()) pumlArgs += "\" ";
  pumlArgs += "-Djava.awt.headless=true -jar \""+plantumlJarPath+"\" ";
  if (!plantumlConfigFile.isEmpty())
  {
    pumlArgs += "-config \"";
    pumlArgs += plantumlConfigFile;
    pumlArgs += "\" ";
  }
  // the -graphvizdot option expects a relative or absolute path to the dot executable, so
  // we need to use the unverified DOT_PATH option and check if it points to an existing file.
  QCString dotPath = Config_getString(DOT_PATH);
  FileInfo dp(dotPath.str());
  if (Config_getBool(HAVE_DOT) && dp.exists() && dp.isFile())
  {
    pumlArgs += "-graphvizdot \"";
    pumlArgs += dotPath;
    pumlArgs += "\" ";
  }
  switch (format)
  {
    case PlantumlManager::PUML_BITMAP:
      pumlType="png";
      break;
    case PlantumlManager::PUML_EPS:
      pumlType="eps";
      break;
    case PlantumlManager::PUML_SVG:
      pumlType="svg";
      break;
  }

  {
    for (const auto &[name,nb] : plantumlContent)
    {
      if (nb.content.isEmpty()) continue;

      QCString pumlArguments = pumlArgs;
      msg("Generating PlantUML {} Files in {}\n",pumlType,name);
      pumlArguments+="-o \"";
      pumlArguments+=nb.outDir;
      pumlArguments+="\" ";
      pumlArguments+="-charset UTF-8 -t";
      pumlArguments+=pumlType;
      pumlArguments+=" ";

      QCString puFileName("");
      puFileName+=nb.outDir;
      puFileName+="/";
      pumlOutDir=puFileName;
      puFileName+="inline_umlgraph_";
      puFileName+=pumlType;
      puFileName+=name.c_str();
      puFileName+=".pu";

      pumlArguments+="\"";
      pumlArguments+=puFileName;
      pumlArguments+="\" ";


      QCString cachedContent;
      FileInfo fi(puFileName.str());
      if (fi.exists())
      {
        cachedContent = fileToString(puFileName);
      }

      std::ofstream file = Portable::openOutputStream(puFileName);
      if (!file.is_open())
      {
        err_full(nb.srcFile,nb.srcLine,"Could not open file {} for writing",puFileName);
      }
      file.write( nb.content.data(), nb.content.length() );
      file.close();
      Debug::print(Debug::Plantuml,0,"*** PlantumlManager::runPlantumlContent Running Plantuml arguments:{}\n",pumlArguments);

      if (cachedContent == nb.content) continue;

      if ((exitCode=Portable::system(pumlExe.data(),pumlArguments.data(),TRUE))!=0)
      {
        err_full(nb.srcFile,nb.srcLine,"Problems running PlantUML. Verify that the command 'java -jar \"{}\" -h' works from the command line. Exit code: {}.",
            plantumlJarPath,exitCode);
      }

      if ( (format==PlantumlManager::PUML_EPS) && (Config_getBool(USE_PDFLATEX)) )
      {
        Debug::print(Debug::Plantuml,0,"*** PlantumlManager::runPlantumlContent Running epstopdf\n");
        auto files_kv = plantumlFiles.find(name);
        if (files_kv!=plantumlFiles.end())
        {
          for (const auto &str : files_kv->second)
          {
            const int maxCmdLine = 40960;
            QCString epstopdfArgs(maxCmdLine, QCString::ExplicitSize);
            epstopdfArgs.sprintf("\"%s%s.eps\" --outfile=\"%s%s.pdf\"",
                pumlOutDir.data(),str.c_str(), pumlOutDir.data(),str.c_str());
            if ((exitCode=Portable::system("epstopdf",epstopdfArgs.data()))!=0)
            {
              err_full(nb.srcFile,nb.srcLine,"Problems running epstopdf. Check your TeX installation! Exit code: {}.",exitCode);
            }
            else
            {
              Dir().remove(pumlOutDir.str()+str+".eps");
            }
          }
        }
      }
    }
  }
}

void PlantumlManager::run()
{
  Debug::print(Debug::Plantuml,0,"*** PlantumlManager::run\n");
  runPlantumlContent(m_pngPlantumlFiles, m_pngPlantumlContent, PUML_BITMAP);
  runPlantumlContent(m_svgPlantumlFiles, m_svgPlantumlContent, PUML_SVG);
  runPlantumlContent(m_epsPlantumlFiles, m_epsPlantumlContent, PUML_EPS);
}

static void print(const PlantumlManager::FilesMap &plantumlFiles)
{
  if (Debug::isFlagSet(Debug::Plantuml))
  {
    for (const auto &[key,list] : plantumlFiles)
    {
      Debug::print(Debug::Plantuml,0,"*** PlantumlManager::print Files PlantumlFiles key:{} size:{}\n",key,list.size());
      for (const auto &s : list)
      {
        Debug::print(Debug::Plantuml,0,"*** PlantumlManager::print                     list:{}\n",s);
      }
    }
  }
}

static void print(const PlantumlManager::ContentMap &plantumlContent)
{
  if (Debug::isFlagSet(Debug::Plantuml))
  {
    for (const auto &[key,content] : plantumlContent)
    {
      Debug::print(Debug::Plantuml,0,"*** PlantumlManager::print Content PlantumlContent key: {}\n",key);
      Debug::print(Debug::Plantuml,0,"*** PlantumlManager::print Content:\n{}\n",content.content);
    }
  }
}

static void addPlantumlFiles(PlantumlManager::FilesMap &plantumlFiles,
                             const std::string &key, const std::string &value)
{
  auto kv = plantumlFiles.find(key);
  if (kv==plantumlFiles.end())
  {
    kv = plantumlFiles.emplace(key,StringVector()).first;
  }
  kv->second.push_back(value);
}

static void addPlantumlContent(PlantumlManager::ContentMap &plantumlContent,
                               const std::string &key, const QCString &outDir, const QCString &puContent,
                               const QCString &srcFile,int srcLine)
{
  auto kv = plantumlContent.find(key);
  if (kv==plantumlContent.end())
  {
    kv = plantumlContent.emplace(key,PlantumlContent("",outDir,srcFile,srcLine)).first;
  }
  kv->second.content+=puContent;
}

void PlantumlManager::insert(const std::string &key, const std::string &value,
                             const QCString &outDir,OutputFormat format,const QCString &puContent,
                             const QCString &srcFile,int srcLine)
{
  Debug::print(Debug::Plantuml,0,"*** PlantumlManager::insert key:{} ,value:{}\n",key,value);

  switch (format)
  {
    case PUML_BITMAP:
      addPlantumlFiles(m_pngPlantumlFiles,key,value);
      print(m_pngPlantumlFiles);
      addPlantumlContent(m_pngPlantumlContent,key,outDir,puContent,srcFile,srcLine);
      print(m_pngPlantumlContent);
      break;
    case PUML_EPS:
      addPlantumlFiles(m_epsPlantumlFiles,key,value);
      print(m_epsPlantumlFiles);
      addPlantumlContent(m_epsPlantumlContent,key,outDir,puContent,srcFile,srcLine);
      print(m_epsPlantumlContent);
      break;
    case PUML_SVG:
      addPlantumlFiles(m_svgPlantumlFiles,key,value);
      print(m_svgPlantumlFiles);
      addPlantumlContent(m_svgPlantumlContent,key,outDir,puContent,srcFile,srcLine);
      print(m_svgPlantumlContent);
      break;
  }
}

//--------------------------------------------------------------------

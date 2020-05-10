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

#include "plantuml.h"
#include "util.h"
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "index.h"
#include "message.h"
#include "debug.h"

#include <qdir.h>
#include <qdict.h>
#include <qlist.h>


QCString PlantumlManager::writePlantUMLSource(const QCString &outDirArg,const QCString &fileName,const QCString &content,OutputFormat format)
{
  QCString baseName;
  QCString puName;
  QCString imgName;
  QCString outDir(outDirArg);
  static int umlindex=1;

  Debug::print(Debug::Plantuml,0,"*** %s fileName: %s\n","writePlantUMLSource",qPrint(fileName));
  Debug::print(Debug::Plantuml,0,"*** %s outDir: %s\n","writePlantUMLSource",qPrint(outDir));

  while ((outDir.findRev('/') == outDir.length()-1) || (outDir.findRev('\\') == outDir.length()-1))
  {
    outDir = outDir.left(outDir.length()-1);
  }

  if (fileName.isEmpty()) // generate name
  {
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

  Debug::print(Debug::Plantuml,0,"*** %s baseName: %s\n","writePlantUMLSource",qPrint(baseName));
  Debug::print(Debug::Plantuml,0,"*** %s puName: %s\n","writePlantUMLSource",qPrint(puName));
  Debug::print(Debug::Plantuml,0,"*** %s imgName: %s\n","writePlantUMLSource",qPrint(imgName));

  QCString text = "@startuml "+imgName+"\n";
  text+=content;
  text+="\n@enduml\n";

  QCString qcOutDir(outDir);
  uint pos = qcOutDir.findRev("/");
  QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
  Debug::print(Debug::Plantuml,0,"*** %s generateType: %s\n","writePlantUMLSource",qPrint(generateType));
  PlantumlManager::instance()->insert(generateType,puName,outDir,format,text);
  Debug::print(Debug::Plantuml,0,"*** %s generateType: %s\n","writePlantUMLSource",qPrint(generateType));

  return baseName;
}

void PlantumlManager::generatePlantUMLOutput(const char *baseName,const char *outDir,OutputFormat format)
{
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  QCString dotPath = Config_getString(DOT_PATH);

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

PlantumlManager *PlantumlManager::m_theInstance = 0;

PlantumlManager *PlantumlManager::instance()
{
  if (!m_theInstance)
  {
    m_theInstance = new PlantumlManager;
    QCString outputFilename = Config_getString(OUTPUT_DIRECTORY) + "/" + CACHE_FILENAME;
    QFileInfo fi(outputFilename);
    if (fi.exists())
    {
        m_theInstance->m_cachedPlantumlAllContent = fileToString(outputFilename);
    }
    else
    {
        m_theInstance->m_cachedPlantumlAllContent = "";
    }
    Debug::print(Debug::Plantuml,0,"*** instance() : m_cachedPlantumlAllContent = [%s]\n",qPrint(m_theInstance->m_cachedPlantumlAllContent));
    m_theInstance->m_pngPlantumlContent.clear();
    m_theInstance->m_svgPlantumlContent.clear();
    m_theInstance->m_epsPlantumlContent.clear();
  }
  return m_theInstance;
}

PlantumlManager::PlantumlManager()
{
}

PlantumlManager::~PlantumlManager()
{
  {
    QDictIterator< QList<QCString> > it( m_pngPlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      (*list).clear();
    }
    m_pngPlantumlFiles.clear();
    m_pngPlantumlContent.clear();
  }
  {
    QDictIterator< QList<QCString> > it( m_epsPlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      (*list).clear();
    }
    m_epsPlantumlFiles.clear();
    m_epsPlantumlContent.clear();
  }
  {
    QDictIterator< QList<QCString> > it( m_svgPlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      (*list).clear();
    }
    m_svgPlantumlFiles.clear();
    m_svgPlantumlContent.clear();
  }
}

static void runPlantumlContent(const QDict< QList <QCString> > &plantumlFiles,
                               const QDict< PlantumlContent > &plantumlContent, 
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
  int exitCode;
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  QCString dotPath = Config_getString(DOT_PATH);

  QCString pumlExe = "java";
  QCString pumlArgs = "";
  QCString pumlType = "";
  QCString pumlOutDir = "";

  QStrList &pumlIncludePathList = Config_getList(PLANTUML_INCLUDE_PATH);
  char *s=pumlIncludePathList.first();
  if (s)
  {
    pumlArgs += "-Dplantuml.include.path=\"";
    pumlArgs += s;
    s = pumlIncludePathList.next(); 
  }
  while (s)
  {
    pumlArgs += Portable::pathListSeparator();
    pumlArgs += s;
    s = pumlIncludePathList.next(); 
  }
  if (pumlIncludePathList.first()) pumlArgs += "\" ";
  pumlArgs += "-Djava.awt.headless=true -jar \""+plantumlJarPath+"plantuml.jar\" ";
  if (!plantumlConfigFile.isEmpty())
  {
    pumlArgs += "-config \"";
    pumlArgs += plantumlConfigFile;
    pumlArgs += "\" ";
  }
  if (Config_getBool(HAVE_DOT) && !dotPath.isEmpty())
  {
    pumlArgs += "-graphvizdot \"";
    pumlArgs += dotPath;
    pumlArgs += "dot";
    pumlArgs += Portable::commandExtension();
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
    QDictIterator< PlantumlContent > it( plantumlContent); // See QDictIterator
    PlantumlContent *nb;
    for (it.toFirst();(nb=it.current());++it)
    {
      QCString pumlArguments(pumlArgs);
      msg("Generating PlantUML %s Files in %s\n",qPrint(pumlType),qPrint(it.currentKey()));
      pumlArguments+="-o \"";
      pumlArguments+=nb->outDir.data();
      pumlArguments+="\" ";
      pumlArguments+="-charset UTF-8 -t";
      pumlArguments+=pumlType;
      pumlArguments+=" ";

      QCString puFileName("");
      puFileName+=nb->outDir.data();
      puFileName+="/";
      pumlOutDir=puFileName;
      puFileName+="inline_umlgraph_";
      puFileName+=pumlType;
      puFileName+=it.currentKey();
      puFileName+=".pu";

      pumlArguments+="\"";
      pumlArguments+=puFileName;
      pumlArguments+="\" ";

      QFile file(puFileName);
      if (!file.open(IO_WriteOnly))
      {
        err("Could not open file %s for writing\n",puFileName.data());
      }
      file.writeBlock( nb->content, nb->content.length() );
      file.close();
      Debug::print(Debug::Plantuml,0,"*** %s Running Plantuml arguments:%s\n","PlantumlManager::runPlantumlContent",qPrint(pumlArguments));

      Portable::sysTimerStart();
      if ((exitCode=Portable::system(pumlExe,pumlArguments,TRUE))!=0)
      {
        err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
            plantumlJarPath.data(),exitCode);
      }
      else if (Config_getBool(DOT_CLEANUP))
      {
        Debug::print(Debug::Plantuml,0,"*** %s Remove %s file\n","PlantumlManager::runPlantumlContent",qPrint(puFileName));
        file.remove();
      }
      Portable::sysTimerStop();

      if ( (format==PlantumlManager::PUML_EPS) && (Config_getBool(USE_PDFLATEX)) )
      {
        Debug::print(Debug::Plantuml,0,"*** %s Running epstopdf\n","PlantumlManager::runPlantumlContent");
        QList<QCString> *list = plantumlFiles[it.currentKey()];
        if (list)
        {
          QListIterator<QCString> li(*list);
          QCString *str_p;
          for (li.toFirst();(str_p=li.current());++li)
          {
            const int maxCmdLine = 40960;
            QCString epstopdfArgs(maxCmdLine);
            epstopdfArgs.sprintf("\"%s%s.eps\" --outfile=\"%s%s.pdf\"",qPrint(pumlOutDir),qPrint(*str_p),qPrint(pumlOutDir),qPrint(*str_p));
            Portable::sysTimerStart();
            if ((exitCode=Portable::system("epstopdf",epstopdfArgs))!=0)
            {
              err("Problems running epstopdf. Check your TeX installation! Exit code: %d\n",exitCode);
            }
            Portable::sysTimerStop();
          }
        }
      }
    }
  }
}

void PlantumlManager::run()
{
  Debug::print(Debug::Plantuml,0,"*** %s\n","PlantumlManager::run");
  if (m_currentPlantumlAllContent.isEmpty()) return;
  runPlantumlContent(m_pngPlantumlFiles, m_pngPlantumlContent, PUML_BITMAP);
  runPlantumlContent(m_svgPlantumlFiles, m_svgPlantumlContent, PUML_SVG);
  runPlantumlContent(m_epsPlantumlFiles, m_epsPlantumlContent, PUML_EPS);
  QCString outputFilename = Config_getString(OUTPUT_DIRECTORY) + "/" + CACHE_FILENAME;
  QFile file(outputFilename);
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",CACHE_FILENAME);
  }
  file.writeBlock( m_currentPlantumlAllContent, m_currentPlantumlAllContent.length() );
  file.close();
}

static void print(const QDict< QList <QCString> > &plantumlFiles)
{
  if (Debug::isFlagSet(Debug::Plantuml))
  {
    QDictIterator< QList<QCString> > it( plantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      Debug::print(Debug::Plantuml,0,"*** %s PlantumlFiles key:%s size:%d\n","PlantumlManager::print Files",qPrint(it.currentKey()),(*list).count());
      QListIterator<QCString> li(*list);
      QCString *nb;
      for (li.toFirst();(nb=li.current());++li)
      {
        Debug::print(Debug::Plantuml,0,"*** %s             list:%s\n","PlantumlManager::print",qPrint(*nb));
      }
    }
  }
}

static void print(const QDict<PlantumlContent> &plantumlContent)
{
  if (Debug::isFlagSet(Debug::Plantuml))
  {
    QDictIterator< PlantumlContent > it( plantumlContent); // See QDictIterator
    PlantumlContent *nb;
    for (it.toFirst();(nb=it.current());++it)
    {
      Debug::print(Debug::Plantuml,0,"*** %s PlantumlContent key:%s\n","PlantumlManager::print Content",qPrint(it.currentKey()));
      Debug::print(Debug::Plantuml,0,"*** %s                 Content :%s\n","PlantumlManager::print",qPrint(nb->content));
    }
  }
}

static void addPlantumlFiles(QDict< QList<QCString> > &plantumlFiles,
                                       const QCString &key , const QCString &value)
{
  QList<QCString> *list = plantumlFiles.find(key);
  if (list==0)
  {
    list = new QList<QCString>;
    plantumlFiles.insert(key,list);
  }
  list->append(new QCString(value));
}

static void addPlantumlContent(QDict< PlantumlContent > &plantumlContent, 
                               const QCString &key, const QCString &outDir, const QCString &puContent)
{
  PlantumlContent* content = plantumlContent.find(key);
  if (content == 0)
  {
    content = new PlantumlContent("",outDir);
    plantumlContent.insert(key,content);
  }
  (content->content)+=puContent;
}



void PlantumlManager::insert(const QCString &key, const QCString &value,
                             const QCString &outDir,OutputFormat format,const QCString &puContent)
{
  int find;

  Debug::print(Debug::Plantuml,0,"*** %s key:%s ,value:%s\n","PlantumlManager::insert",qPrint(key),qPrint(value));

  m_currentPlantumlAllContent+=puContent;

  find = m_cachedPlantumlAllContent.find(puContent);
  Debug::print(Debug::Plantuml,0,"*** %s find: %d\n","PlantumlManager::addPlantumlContent",find);
  if (find >=0)
  {         // matched in cache. so we skip to run java for this plantuml
      return ;
  }

  switch (format)
  {
    case PUML_BITMAP:
      addPlantumlFiles(m_pngPlantumlFiles,key,value);
      print(m_pngPlantumlFiles);
      addPlantumlContent(m_pngPlantumlContent,key,outDir,puContent);
      print(m_pngPlantumlContent);
      break;
    case PUML_EPS:
      addPlantumlFiles(m_epsPlantumlFiles,key,value);
      print(m_epsPlantumlFiles);
      addPlantumlContent(m_epsPlantumlContent,key,outDir,puContent);
      print(m_epsPlantumlContent);
      break;
    case PUML_SVG:
      addPlantumlFiles(m_svgPlantumlFiles,key,value);
      print(m_svgPlantumlFiles);
      addPlantumlContent(m_svgPlantumlContent,key,outDir,puContent);
      print(m_svgPlantumlContent);
      break;
  }
}

//--------------------------------------------------------------------

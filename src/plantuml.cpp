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
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "index.h"
#include "message.h"
#include "debug.h"

#include <qdir.h>
#include <qdict.h>
#include <qlist.h>

static const int maxCmdLine = 40960;

QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content,PlantUMLOutputFormat format)
{
  QCString baseName(4096);
  QCString puName(4096);
  QCString imgName(4096);
  static int umlindex=1;

  Debug::print(Debug::Plantuml,0,"*** %s fileName: %s\n","writePlantUMLSource",qPrint(fileName));
  Debug::print(Debug::Plantuml,0,"*** %s outDir: %s\n","writePlantUMLSource",qPrint(outDir));

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

  QFile file(baseName+".pu");
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",baseName.data());
  }
  QCString text = "@startuml "+imgName+"\n";
  text+=content;
  text+="\n@enduml\n";
  file.writeBlock( text, text.length() );
  file.close();
  //Debug::print(Debug::Plantuml,0,"*** %s baseFileName text: %s\n","writePlantUMLSource",qPrint(text));

  if(Config_getBool(PLANTUML_RUN_JAVA_ONCE)){
    QCString qcOutDir(outDir);
    uint pos = qcOutDir.findRev("/");
    QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
    Debug::print(Debug::Plantuml,0,"*** %s generateType: %s\n","writePlantUMLSource",qPrint(generateType));
    PlantumlManager::instance()->insert(generateType,puName,format,text);
  } 

  return baseName;
}

void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format)
{
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  QCString dotPath = Config_getString(DOT_PATH);

  QCString pumlExe = "java";
  QCString pumlArgs = "";

  //printf("*** %s %s\n","generatePlantUMLOutput",baseName);
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
    pumlArgs += portable_pathListSeparator();
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
    pumlArgs += portable_commandExtension();
    pumlArgs += "\" ";
  }
  pumlArgs+="-o \"";
  pumlArgs+=outDir;
  pumlArgs+="\" ";
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
      pumlArgs+="-tpng";
      imgName+=".png";
      break;
    case PUML_EPS:
      pumlArgs+="-teps";
      imgName+=".eps";
      break;
    case PUML_SVG:
      pumlArgs+="-tsvg";
      imgName+=".svg";
      break;
  }
  pumlArgs+=" \"";
  pumlArgs+=baseName;
  pumlArgs+=".pu\" ";
  pumlArgs+="-charset UTF-8 ";
  int exitCode;
  msg("Running PlantUML on generated file %s.pu\n",baseName);
  portable_sysTimerStart();
  if(!Config_getBool(PLANTUML_RUN_JAVA_ONCE)){    // ! Config_getBool(PLANTUML_RUN_JAVA_ONCE)
    Debug::print(Debug::Plantuml,0,"*** running: %s %s outDir:%s %s\n",qPrint(pumlExe),qPrint(pumlArgs),outDir,baseName);
    if ((exitCode=portable_system(pumlExe,pumlArgs,TRUE))!=0)
    {
      err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
          plantumlJarPath.data(),exitCode);
    }
    else if (Config_getBool(DOT_CLEANUP))
    {
      QFile(QCString(baseName)+".pu").remove();
    }
  }
  portable_sysTimerStop();
  if ( (format==PUML_EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",baseName,baseName);
    portable_sysTimerStart();
    if ((exitCode=portable_system("epstopdf",epstopdfArgs))!=0)
    {
      err("Problems running epstopdf. Check your TeX installation! Exit code: %d\n",exitCode);
    }
    portable_sysTimerStop();
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

void PlantumlManager::runPlantumlContent(QDict< QList <QCString> > &PlantumlFiles,QDict< QCString > &PlantumlContent, const char *type)
{
  /* example : running: java -Djava.awt.headless=true -jar "/Users/cheoljoo/code/common_telltale/GP/Apps/Src/MgrTelltale/tools/plantuml.jar" -o "/Users/cheoljoo/Code/LG/test_doxygen/DOXYGEN_OUTPUT/html" -tpng "/Users/cheoljoo/Code/LG/test_doxygen/DOXYGEN_OUTPUT/html/A.pu" -charset UTF-8  outDir:/Users/cheoljoo/Code/LG/test_doxygen/DOXYGEN_OUTPUT/html /Users/cheoljoo/Code/LG/test_doxygen/DOXYGEN_OUTPUT/html/A
   */
  int exitCode;
  QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  QCString dotPath = Config_getString(DOT_PATH);

  QCString pumlExe = "java";
  QCString pumlArgs = "";

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
    pumlArgs += portable_pathListSeparator();
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
    pumlArgs += portable_commandExtension();
    pumlArgs += "\" ";
  }

  {
    QDictIterator< QCString > it( PlantumlContent); // See QDictIterator
    QCString *nb;
    for (it.toFirst();(nb=it.current());++it)
    {
      QCString pumlArguments(pumlArgs);
      msg("Running PlantUML on png PlantumlFiles in %s\n",qPrint(it.currentKey()));
      pumlArguments+="-o \"";
      pumlArguments+=Config_getString(OUTPUT_DIRECTORY);
      pumlArguments+="/";
      pumlArguments+=it.currentKey();
      pumlArguments+="\" ";
      pumlArguments+="-charset UTF-8 -t";
      pumlArguments+=type;
      pumlArguments+=" ";

      QCString puFileName("");
      puFileName+=Config_getString(OUTPUT_DIRECTORY);
      puFileName+="/";
      puFileName+=it.currentKey();
      puFileName+="/";
      puFileName+="inline_umlgraph_";
      puFileName+=type;
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
      file.writeBlock( *nb, nb->length() );
      file.close();
      Debug::print(Debug::Plantuml,0,"*** %s Running Plantuml arguments:%s\n","PlantumlManager::runPlantumlContent",qPrint(pumlArguments));
      if ((exitCode=portable_system(pumlExe,pumlArguments,TRUE))!=0)
      {
        err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
            plantumlJarPath.data(),exitCode);
      }
      else if (Config_getBool(DOT_CLEANUP))
      {
        file.remove();
      }
    }
  }

  {
    QDictIterator< QList<QCString> > it( PlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      QListIterator<QCString> li(*list);
      QCString *nb;
      if (Config_getBool(DOT_CLEANUP))
      {
        for (li.toFirst();(nb=li.current());++li)
        {
          QCString pumlName = "";
          pumlName+=Config_getString(OUTPUT_DIRECTORY);
          pumlName+="/";
          pumlName+=it.currentKey();
          pumlName+="/";
          pumlName+=*nb;
          pumlName+=".pu";
          QFile(pumlName).remove();
        }
      }
    }
  }
}

void PlantumlManager::run()
{
  Debug::print(Debug::Plantuml,0,"*** %s\n","PlantumlManager::run");
  runPlantumlContent(m_pngPlantumlFiles, m_pngPlantumlContent, "png");
  runPlantumlContent(m_svgPlantumlFiles, m_svgPlantumlContent, "svg");
  runPlantumlContent(m_epsPlantumlFiles, m_epsPlantumlContent, "eps");
}

void PlantumlManager::print(QDict< QList <QCString> > &PlantumlFiles)
{
  if (Debug::isFlagSet(Debug::Plantuml)){
    QDictIterator< QList<QCString> > it( PlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      Debug::print(Debug::Plantuml,0,"*** %s PlantumlFiles key:%s\n","PlantumlManager::print",qPrint(it.currentKey()));
      QListIterator<QCString> li(*list);
      QCString *nb;
      for (li.toFirst();(nb=li.current());++li)
      {
        Debug::print(Debug::Plantuml,0,"*** %s             list:%s\n","PlantumlManager::print",qPrint(*nb));
      }
    }
  }
}

void PlantumlManager::print(QDict<QCString> &PlantumlContent)
{
  if (Debug::isFlagSet(Debug::Plantuml)){
    QDictIterator< QCString > it( PlantumlContent); // See QDictIterator
    QCString *nb;
    for (it.toFirst();(nb=it.current());++it)
    {
      Debug::print(Debug::Plantuml,0,"*** %s PlantumlContent key:%s\n","PlantumlManager::print",qPrint(it.currentKey()));
      Debug::print(Debug::Plantuml,0,"*** %s                 Content :%s\n","PlantumlManager::print",qPrint(*nb));
    }
  }
}

void PlantumlManager::addPlantumlFiles(QDict< QList <QCString> > &PlantumlFiles,const QCString key , const QCString value)
{
  QList<QCString> *list = PlantumlFiles.find(key);
  if (list==0)
  {
    list = new QList<QCString>;
    PlantumlFiles.insert(key,list);
  }
  list->append(new QCString(value));
}

void PlantumlManager::addPlantumlContent(QDict< QCString > &PlantumlContent,const QCString key , const QCString &puContent)
{
  QCString* content = PlantumlContent.find(key);
  if(content == 0){
    content = new QCString("");
    PlantumlContent.insert(key,content);
  }
  (*content)+=puContent;
}

void PlantumlManager::insert(const QCString key , const QCString value, PlantUMLOutputFormat format,const QCString &puContent)
{
  Debug::print(Debug::Plantuml,0,"*** %s key:%s ,value:%s\n","PlantumlManager::insert",qPrint(key),qPrint(value));
  switch (format)
  {
    case PUML_BITMAP:
      addPlantumlFiles(m_pngPlantumlFiles,key,value);
      print(m_pngPlantumlFiles);
      addPlantumlContent(m_pngPlantumlContent,key,puContent);
      print(m_pngPlantumlContent);
      break;
    case PUML_EPS:
      addPlantumlFiles(m_epsPlantumlFiles,key,value);
      print(m_epsPlantumlFiles);
      addPlantumlContent(m_epsPlantumlContent,key,puContent);
      print(m_epsPlantumlContent);
      break;
    case PUML_SVG:
      addPlantumlFiles(m_svgPlantumlFiles,key,value);
      print(m_svgPlantumlFiles);
      addPlantumlContent(m_svgPlantumlContent,key,puContent);
      print(m_svgPlantumlContent);
      break;
  }
}

//--------------------------------------------------------------------

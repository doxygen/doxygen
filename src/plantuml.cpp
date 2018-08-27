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

QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content)
{
  QCString baseName(4096);
  static int umlindex=1;

  Debug::print(Debug::Plantuml,0,"*** %s fileName: %s\n",__PRETTY_FUNCTION__,qPrint(fileName));
  Debug::print(Debug::Plantuml,0,"*** %s outDir: %s\n",__PRETTY_FUNCTION__,qPrint(outDir));

  if (fileName.isEmpty()) // generate name
  {
    baseName = outDir+"/inline_umlgraph_"+QCString().setNum(umlindex++);
  }
  else // user specified name
  {
    baseName = fileName;
    int i=baseName.findRev('.');
    if (i!=-1) baseName = baseName.left(i);
    baseName.prepend(outDir+"/");
  }

  Debug::print(Debug::Plantuml,0,"*** %s baseName: %s\n",__PRETTY_FUNCTION__,qPrint(baseName));
  QFile file(baseName+".pu");
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",baseName.data());
  }
  QCString text = "@startuml\n";
  text+=content;
  text+="\n@enduml\n";
  file.writeBlock( text, text.length() );
  file.close();
  return baseName;
}

void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format)
{
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
  if(Config_getBool(PLANTUML_RUN_JAVA_ONCE)){
    QCString qcOutDir(outDir);
    uint pos = qcOutDir.findRev("/");
    QCString generateType(qcOutDir.right(qcOutDir.length() - (pos + 1)) );
    Debug::print(Debug::Plantuml,0,"*** %s generateType: %s\n",__PRETTY_FUNCTION__,qPrint(generateType));
    QCString qcBaseName(baseName);
    pos = qcBaseName.findRev("/");
    QCString baseFileName(qcBaseName.right(qcBaseName.length() - (pos + 1)) );
    Debug::print(Debug::Plantuml,0,"*** %s baseFileName: %s\n",__PRETTY_FUNCTION__,qPrint(baseFileName));
    PlantumlManager::instance()->insert(generateType,baseFileName,format);
  } else {  // ! Config_getBool(PLANTUML_RUN_JAVA_ONCE)
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
  }
}

void PlantumlManager::runPlantumlFiles(QDict< QList <QCString> > &PlantumlFiles,const char *type)
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


  QDictIterator< QList<QCString> > it( PlantumlFiles); // See QDictIterator
  QList<QCString> *list;
  for (it.toFirst();(list=it.current());++it)
  {
    QCString pumlArguments(pumlArgs);
    msg("Running PlantUML on png PlantumlFiles in %s\n",qPrint(it.currentKey()));
    pumlArguments+="-o \"";
    pumlArguments+=Config_getString(OUTPUT_DIRECTORY);
    pumlArguments+="/";
    pumlArguments+=it.currentKey();
    pumlArguments+="\" ";
    pumlArguments+="-charset UTF-8 ";
    pumlArguments+=type;
    pumlArguments+=" ";
    QListIterator<QCString> li(*list);
    QCString *nb;
    for (li.toFirst();(nb=li.current());++li)
    {
      pumlArguments+="\"";
      pumlArguments+=Config_getString(OUTPUT_DIRECTORY);
      pumlArguments+="/";
      pumlArguments+=it.currentKey();
      pumlArguments+="/";
      pumlArguments+=*nb;
      pumlArguments+=".pu";
      pumlArguments+="\" ";
    }
    Debug::print(Debug::Plantuml,0,"*** %s Running Plantuml arguments:%s\n",__PRETTY_FUNCTION__,qPrint(pumlArguments));
    if ((exitCode=portable_system(pumlExe,pumlArguments,TRUE))!=0)
    {
      err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
          plantumlJarPath.data(),exitCode);
    }
    else if (Config_getBool(DOT_CLEANUP))
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

void PlantumlManager::run()
{
  Debug::print(Debug::Plantuml,0,"*** %s\n",__PRETTY_FUNCTION__);
  runPlantumlFiles(m_pngPlantumlFiles, "-tpng");
  runPlantumlFiles(m_svgPlantumlFiles, "-tsvg");
  runPlantumlFiles(m_epsPlantumlFiles, "-teps");
}

void PlantumlManager::print(QDict< QList <QCString> > &PlantumlFiles)
{
  if (Debug::isFlagSet(Debug::Plantuml)){
    QDictIterator< QList<QCString> > it( PlantumlFiles); // See QDictIterator
    QList<QCString> *list;
    for (it.toFirst();(list=it.current());++it)
    {
      Debug::print(Debug::Plantuml,0,"*** %s PlantumlFiles key:%s\n",__PRETTY_FUNCTION__,qPrint(it.currentKey()));
      QListIterator<QCString> li(*list);
      QCString *nb;
      for (li.toFirst();(nb=li.current());++li)
      {
        Debug::print(Debug::Plantuml,0,"*** %s PlantumlFiles         list:%s\n",__PRETTY_FUNCTION__,qPrint(*nb));
      }
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
  Debug::print(Debug::Plantuml,0,"*** %s append : png key:%s ,value:%s\n",__PRETTY_FUNCTION__,qPrint(key),qPrint(value));
}

void PlantumlManager::insert(const QCString key , const QCString value, PlantUMLOutputFormat format)
{
  Debug::print(Debug::Plantuml,0,"*** %s key:%s ,value:%s\n",__PRETTY_FUNCTION__,qPrint(key),qPrint(value));
  switch (format)
  {
    case PUML_BITMAP:
      addPlantumlFiles(m_pngPlantumlFiles,key,value);
      print(m_pngPlantumlFiles);
      break;
    case PUML_EPS:
      addPlantumlFiles(m_epsPlantumlFiles,key,value);
      print(m_epsPlantumlFiles);
      break;
    case PUML_SVG:
      addPlantumlFiles(m_svgPlantumlFiles,key,value);
      print(m_svgPlantumlFiles);
      break;
  }
}

//--------------------------------------------------------------------

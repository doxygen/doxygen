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

#include <qdir.h>

static const int maxCmdLine = 40960;

QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content)
{
  QCString baseName(4096);
  static int umlindex=1;

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
  static QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  static QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  static QCString dotPath = Config_getString(DOT_PATH);

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
  //printf("*** running: %s %s outDir:%s %s\n",pumlExe.data(),pumlArgs.data(),outDir,baseName);
  msg("Running PlantUML on generated file %s.pu\n",baseName);
  portable_sysTimerStart();
  if ((exitCode=portable_system(pumlExe,pumlArgs,TRUE))!=0)
  {
    err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
        plantumlJarPath.data(),exitCode);
  }
  else if (Config_getBool(DOT_CLEANUP))
  {
    QFile(QCString(baseName)+".pu").remove();
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


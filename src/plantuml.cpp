/******************************************************************************
 *
 * Copyright (C) 1997-2014 by Dimitri van Heesch.
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
#include "message.h"

#include <qdir.h>

//static const int maxCmdLine = 40960;

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
  text+="@enduml\n";
  file.writeBlock( text, text.length() );
  file.close();
  return baseName;
}

void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format)
{
  static QCString plantumlJarPath = Config_getString("PLANTUML_JAR_PATH");

  QCString pumlExe = "java";
  QCString pumlArgs = "-Djava.awt.headless=true -jar \""+plantumlJarPath+"plantuml.jar\" ";
  pumlArgs+="-o \"";
  pumlArgs+=outDir;
  pumlArgs+="\" ";
  QCString extension;
  switch (format)
  {
    case PUML_BITMAP:
      pumlArgs+="-tpng";
      extension=".png";
      break;
    case PUML_EPS:
      pumlArgs+="-teps";
      extension=".eps";
      break;
    case PUML_SVG:
      pumlArgs+="-tsvg";
      extension=".svg";
      break;
  }
  pumlArgs+=" \"";
  pumlArgs+=baseName;
  pumlArgs+=".pu\" ";
  int exitCode;
  //printf("*** running: %s %s outDir:%s %s\n",pumlExe.data(),pumlArgs.data(),outDir,outFile);
  msg("Running PlantUML on generated file %s.pu\n",baseName);
  portable_sysTimerStart();
  if ((exitCode=portable_system(pumlExe,pumlArgs,FALSE))!=0)
  {
    err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line\n",
        plantumlJarPath.data());
  }
  else if (Config_getBool("DOT_CLEANUP"))
  {
    QFile(QCString(baseName)+".pu").remove();
  }
  portable_sysTimerStop();
}


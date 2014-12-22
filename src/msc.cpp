/******************************************************************************
 *
 * 
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

#include "msc.h"
#include "portable.h"
#include "config.h"
#include "message.h"
#include "docparser.h"
#include "doxygen.h"
#include "util.h"
#include "ftextstream.h"

#include <qdir.h>

static const int maxCmdLine = 40960;

static bool convertMapFile(FTextStream &t,const char *mapName,const QCString relPath,
                           const QCString &context)
{
  QFile f(mapName);
  if (!f.open(IO_ReadOnly))
  {
    err("failed to open map file %s for inclusion in the docs!\n"
        "If you installed Graphviz/dot after a previous failing run, \n"
        "try deleting the output directory and rerun doxygen.\n",mapName);
    return FALSE;
  }
  const int maxLineLen=1024;
  char buf[maxLineLen];
  char url[maxLineLen];
  char ref[maxLineLen];
  int x1,y1,x2,y2;
  while (!f.atEnd())
  {
    bool isRef = FALSE;
    int numBytes = f.readLine(buf,maxLineLen);
    buf[numBytes-1]='\0';
    //printf("ReadLine `%s'\n",buf);
    if (qstrncmp(buf,"rect",4)==0)
    {
      // obtain the url and the coordinates in the order used by graphviz-1.5
      sscanf(buf,"rect %s %d,%d %d,%d",url,&x1,&y1,&x2,&y2);

      if (qstrcmp(url,"\\ref")==0 || qstrcmp(url,"@ref")==0)
      {
        isRef = TRUE;
        sscanf(buf,"rect %s %s %d,%d %d,%d",ref,url,&x1,&y1,&x2,&y2);
      }

      // sanity checks
      if (y2<y1) { int temp=y2; y2=y1; y1=temp; }
      if (x2<x1) { int temp=x2; x2=x1; x1=temp; }

      t << "<area href=\"";

      if ( isRef )
      {
        // handle doxygen \ref tag URL reference
        DocRef *df = new DocRef( (DocNode*) 0, url, context );
        t << externalRef(relPath,df->ref(),TRUE);
        if (!df->file().isEmpty()) t << df->file() << Doxygen::htmlFileExtension;
        if (!df->anchor().isEmpty()) t << "#" << df->anchor();
        delete df;
      }
      else
      {
        t << url;
      }
      t << "\" shape=\"rect\" coords=\""
        << x1 << "," << y1 << "," << x2 << "," << y2 << "\""
        << " alt=\"\"/>" << endl;
    }
  }

  return TRUE;
}

void writeMscGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,MscOutputFormat format)
{
  QCString absOutFile = outDir;
  absOutFile+=portable_pathSeparator();
  absOutFile+=outFile;

  // chdir to the output dir, so dot can find the font file.
  QCString oldDir = QDir::currentDirPath().utf8();
  // go to the html output directory (i.e. path)
  QDir::setCurrent(outDir);
  //printf("Going to dir %s\n",QDir::currentDirPath().data());
  QCString mscExe = Config_getString("MSCGEN_PATH")+"mscgen"+portable_commandExtension();
  QCString mscArgs;
  QCString extension;
  switch (format)
  {
    case MSC_BITMAP:
      mscArgs+="-T png";
      extension=".png";
      break;
    case MSC_EPS:
      mscArgs+="-T eps";
      extension=".eps";
      break;
    case MSC_SVG:
      mscArgs+="-T svg";
      extension=".svg";
      break;
    default:
      goto error; // I am not very fond of goto statements, but when in Rome...
  }
  mscArgs+=" -i \"";
  mscArgs+=inFile;
 
  mscArgs+="\" -o \"";
  mscArgs+=outFile;
  mscArgs+=extension+"\"";
  int exitCode;
//  printf("*** running: %s %s outDir:%s %s\n",mscExe.data(),mscArgs.data(),outDir,outFile);
  portable_sysTimerStart();
  if ((exitCode=portable_system(mscExe,mscArgs,FALSE))!=0)
  {
    portable_sysTimerStop();
    goto error;
  }
  portable_sysTimerStop();
  if ( (format==MSC_EPS) && (Config_getBool("USE_PDFLATEX")) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         outFile,outFile);
    portable_sysTimerStart();
    if (portable_system("epstopdf",epstopdfArgs)!=0)
    {
      err("Problems running epstopdf. Check your TeX installation!\n");
    }
    portable_sysTimerStop();
  }

  QDir::setCurrent(oldDir);
  return;

error:
  err("Problems running mscgen: exit code=%d, command='%s', arguments='%s'\n",
      exitCode,mscExe.data(),mscArgs.data());
  QDir::setCurrent(oldDir);
  return;
}

QCString getMscImageMapFromFile(const QCString& inFile, const QCString& outDir,
                                const QCString& relPath,const QCString& context)
{
  QCString outFile = inFile + ".map";


  //printf("*** running:getMscImageMapFromFile \n");
  // chdir to the output dir, so dot can find the font file.
  QCString oldDir = QDir::currentDirPath().utf8();
  // go to the html output directory (i.e. path)
  QDir::setCurrent(outDir);
  //printf("Going to dir %s\n",QDir::currentDirPath().data());

  QCString mscExe = Config_getString("MSCGEN_PATH")+"mscgen"+portable_commandExtension();
  QCString mscArgs = "-T ismap -i \"";
  mscArgs+=inFile;
  QFileInfo fi(inFile);
  mscArgs+="\" -o \"";
  mscArgs+=outFile + "\"";

  int exitCode;
  portable_sysTimerStart();
  if ((exitCode=portable_system(mscExe,mscArgs,FALSE))!=0)
  {
    portable_sysTimerStop();
    QDir::setCurrent(oldDir);
    return "";
  }
  portable_sysTimerStop();
  
  QGString result;
  FTextStream tmpout(&result);
  convertMapFile(tmpout, outFile, relPath, context);
  QDir().remove(outFile);

  QDir::setCurrent(oldDir);
  return result.data();
}

void writeMscImageMapFromFile(FTextStream &t,const QCString &inFile,
                              const QCString &outDir,
                              const QCString &relPath,
                              const QCString &baseName,
                              const QCString &context,
			      MscOutputFormat format
 			    )
{
  QCString mapName = baseName+".map";
  QCString mapFile = inFile+".map";
  t << "<img src=\"" << relPath << baseName << ".";
  switch (format)
  {
    case MSC_BITMAP:
      t << "png";
      break;
    case MSC_EPS:
      t << "eps";
      break;
    case MSC_SVG:
      t << "svg";
      break;
    default:
      t << "unknown";
  }
  t << "\" alt=\""
    << baseName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>" << endl;
  QCString imap = getMscImageMapFromFile(inFile,outDir,relPath,context);
  t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">" << imap << "</map>" << endl;
}


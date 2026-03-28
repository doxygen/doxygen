/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include "dia.h"
#include "portable.h"
#include "config.h"
#include "message.h"
#include "util.h"
#include "dir.h"


static const int maxCmdLine = 40960;

void writeDiaGraphFromFile(const QCString &inFile,const QCString &outDir,
                           const QCString &outFile,DiaOutputFormat format,
                           const QCString &srcFile,int srcLine)
{
  QCString absOutFile = outDir;
  absOutFile+=Portable::pathSeparator();
  absOutFile+=outFile;

  // chdir to the output dir, so dot can find the font file.
  std::string oldDir = Dir::currentDirPath();
  // go to the html output directory (i.e. path)
  Dir::setCurrent(outDir.str());
  //printf("Going to dir %s\n",Dir::currentDirPath().c_str());
  QCString diaExe = Config_getString(DIA_PATH)+"dia"+Portable::commandExtension();
  QCString diaArgs;
  QCString extension;
  diaArgs+="-n ";
  if (format==DiaOutputFormat::BITMAP)
  {
    diaArgs+="-t png-libart";
    extension=".png";
  }
  else if (format==DiaOutputFormat::EPS)
  {
    diaArgs+="-t eps";
    extension=".eps";
  }

  diaArgs+=" -e \"";
  diaArgs+=outFile;
  diaArgs+=extension+"\"";

  diaArgs+=" \"";
  diaArgs+=inFile;
  diaArgs+="\"";

  //printf("*** running: %s %s outDir:%s %s\n",qPrint(diaExe),qPrint(diaArgs),outDir,outFile);
  if (Portable::system(diaExe,diaArgs,FALSE)!=0)
  {
    err_full(srcFile,srcLine,"Problems running {}. Check your installation or look typos in you dia file {}",
        diaExe,inFile);
    goto error;
  }
  if ( (format==DiaOutputFormat::EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine, QCString::ExplicitSize);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         qPrint(outFile),qPrint(outFile));
    if (Portable::system("epstopdf",epstopdfArgs)!=0)
    {
      err("Problems running epstopdf. Check your TeX installation!\n");
    }
    else
    {
      Dir().remove(outFile.str()+".eps");
    }
  }

error:
  Dir::setCurrent(oldDir);
}


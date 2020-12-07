/******************************************************************************
 *
 * 
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

#include "msc.h"
#include "portable.h"
#include "config.h"
#include "message.h"
#include "docparser.h"
#include "doxygen.h"
#include "index.h"
#include "util.h"
#include "ftextstream.h"
#include "mscgen_api.h"

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
    //printf("ReadLine '%s'\n",buf);
    if (qstrncmp(buf,"rect",4)==0)
    {
      sscanf(buf,"rect %s",ref); // lets get the first word after 'rect'
      if (qstrcmp(ref,"\\ref")==0 || qstrcmp(ref,"@ref")==0)
      {
        isRef = TRUE;
        int j = sscanf(buf,"rect %s %s %d,%d %d,%d",ref,url,&x1,&y1,&x2,&y2);
        if (j!=6) continue;
      }
      else
      {
        // obtain the url and the coordinates in the order used by graphviz-1.5
        int i = sscanf(buf,"rect %s %d,%d %d,%d",url,&x1,&y1,&x2,&y2);
        if (i!=5) continue;
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
  absOutFile+=Portable::pathSeparator();
  absOutFile+=outFile;

  mscgen_format_t msc_format;
  QCString imgName = absOutFile;
  switch (format)
  {
    case MSC_BITMAP:
      msc_format = mscgen_format_png;
      imgName+=".png";
      break;
    case MSC_EPS:
      msc_format = mscgen_format_eps;
      imgName+=".eps";
      break;
    case MSC_SVG:
      msc_format = mscgen_format_svg;
      imgName+=".svg";
      break;
    default:
      return;
  }
  int code;
  if ((code=mscgen_generate(inFile,imgName,msc_format))!=0)
  {
    err("Problems generating msc output (error=%s). Look for typos in you msc file %s\n",
        mscgen_error2str(code),inFile);
    return;
  }

  if ( (format==MSC_EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         absOutFile.data(),absOutFile.data());
    Portable::sysTimerStart();
    if (Portable::system("epstopdf",epstopdfArgs)!=0)
    {
      err("Problems running epstopdf. Check your TeX installation!\n");
    }
    Portable::sysTimerStop();
  }

  Doxygen::indexList->addImageFile(imgName);

}

static QCString getMscImageMapFromFile(const QCString& inFile, const QCString& outDir,
                                const QCString& relPath,const QCString& context,
                                bool writeSVGMap)
{
  QCString outFile = inFile + ".map";

  int code;
  if ((code=mscgen_generate(inFile,outFile,
                            writeSVGMap ? mscgen_format_svgmap : mscgen_format_pngmap))!=0)
  {
    err("Problems generating msc output (error=%s). Look for typos in you msc file %s\n",
        mscgen_error2str(code),inFile.data());
    return "";
  }

  QGString result;
  FTextStream tmpout(&result);
  convertMapFile(tmpout, outFile, relPath, context);
  QDir().remove(outFile);

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
  QCString imap = getMscImageMapFromFile(inFile,outDir,relPath,context,format==MSC_SVG);
  if (!imap.isEmpty())
  {
    t << "\" alt=\""
      << baseName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>" << endl;
    t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">" << imap << "</map>" << endl;
  }
  else
  {
    t << "\" alt=\"" << baseName << "\" border=\"0\"/>" << endl;
  }
}


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

#include <sstream>

#include "msc.h"
#include "portable.h"
#include "config.h"
#include "message.h"
#include "docparser.h"
#include "docnode.h"
#include "doxygen.h"
#include "index.h"
#include "util.h"
#include "mscgen_api.h"
#include "dir.h"
#include "textstream.h"

static const int maxCmdLine = 40960;

static bool convertMapFile(TextStream &t,const QCString &mapName,const QCString &relPath,
                           const QCString &context)
{
  std::ifstream f(mapName.str(),std::ifstream::in);
  if (!f.is_open())
  {
    err("failed to open map file %s for inclusion in the docs!\n"
        "If you installed Graphviz/dot after a previous failing run, \n"
        "try deleting the output directory and rerun doxygen.\n",qPrint(mapName));
    return false;
  }
  const int maxLineLen=1024;
  char url[maxLineLen];
  char ref[maxLineLen];
  int x1,y1,x2,y2;
  std::string line;
  while (getline(f,line))
  {
    bool isRef = false;
    //printf("ReadLine '%s'\n",line.c_str());
    if (qstrncmp(line.c_str(),"rect",4)==0)
    {
      sscanf(line.c_str(),"rect %s",ref); // lets get the first word after 'rect'
      if (qstrcmp(ref,"\\ref")==0 || qstrcmp(ref,"@ref")==0)
      {
        isRef = true;
        int j = sscanf(line.c_str(),"rect %s %s %d,%d %d,%d",ref,url,&x1,&y1,&x2,&y2);
        if (j!=6) continue;
      }
      else
      {
        // obtain the url and the coordinates in the order used by graphviz-1.5
        int i = sscanf(line.c_str(),"rect %s %d,%d %d,%d",url,&x1,&y1,&x2,&y2);
        if (i!=5) continue;
      }

      // sanity checks
      if (y2<y1) { int temp=y2; y2=y1; y1=temp; }
      if (x2<x1) { int temp=x2; x2=x1; x1=temp; }

      t << "<area href=\"";

      if ( isRef )
      {
        // handle doxygen \ref tag URL reference

        auto parser { createDocParser() };
        auto dfAst  { createRef( *parser.get(), url, context ) };
        auto dfAstImpl = dynamic_cast<const DocNodeAST*>(dfAst.get());
        const DocRef *df = std::get_if<DocRef>(&dfAstImpl->root);
        t << externalRef(relPath,df->ref(),TRUE);
        if (!df->file().isEmpty()) t << addHtmlExtensionIfMissing(df->file());
        if (!df->anchor().isEmpty()) t << "#" << df->anchor();
      }
      else
      {
        t << url;
      }
      t << "\" shape=\"rect\" coords=\""
        << x1 << "," << y1 << "," << x2 << "," << y2 << "\""
        << " alt=\"\"/>\n";
    }
  }

  return true;
}

void writeMscGraphFromFile(const QCString &inFile,const QCString &outDir,
                           const QCString &outFile,MscOutputFormat format,
                           const QCString &srcFile,int srcLine
                          )
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
  if ((code=mscgen_generate(inFile.data(),imgName.data(),msc_format))!=0)
  {
    err_full(srcFile,srcLine,"Problems generating msc output (error=%s). Look for typos in you msc file %s\n",
        mscgen_error2str(code),qPrint(inFile));
    return;
  }

  if ( (format==MSC_EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         qPrint(absOutFile),qPrint(absOutFile));
    Portable::sysTimerStart();
    if (Portable::system("epstopdf",epstopdfArgs)!=0)
    {
      err_full(srcFile,srcLine,"Problems running epstopdf when processing '%s.eps'. Check your TeX installation!\n",
          qPrint(absOutFile));
    }
    Portable::sysTimerStop();
  }

  Doxygen::indexList->addImageFile(imgName);

}

static QCString getMscImageMapFromFile(const QCString& inFile, const QCString& outDir,
                                const QCString& relPath,const QCString& context,
                                bool writeSVGMap,const QCString &srcFile,int srcLine)
{
  QCString outFile = inFile + ".map";

  int code;
  if ((code=mscgen_generate(inFile.data(),outFile.data(),
                            writeSVGMap ? mscgen_format_svgmap : mscgen_format_pngmap))!=0)
  {
    err_full(srcFile,srcLine,"Problems generating msc output (error=%s). Look for typos in you msc file %s\n",
        mscgen_error2str(code),qPrint(inFile));
    return "";
  }

  TextStream t;
  convertMapFile(t, outFile, relPath, context);

  Dir().remove(outFile.str());

  return t.str();
}

void writeMscImageMapFromFile(TextStream &t,const QCString &inFile,
                              const QCString &outDir,
                              const QCString &relPath,
                              const QCString &baseName,
                              const QCString &context,
			      MscOutputFormat format,
                              const QCString &srcFile,
                              int srcLine
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
  QCString imap = getMscImageMapFromFile(inFile,outDir,relPath,context,format==MSC_SVG,srcFile,srcLine);
  if (!imap.isEmpty())
  {
    t << "\" alt=\""
      << baseName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>\n";
    t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">" << imap << "</map>\n";
  }
  else
  {
    t << "\" alt=\"" << baseName << "\" border=\"0\"/>\n";
  }
}


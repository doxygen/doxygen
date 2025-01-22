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

#include "msc.h"
#include "portable.h"
#include "config.h"
#include "message.h"
#include "docparser.h"
#include "docnode.h"
#include "doxygen.h"
#include "indexlist.h"
#include "util.h"
#include "mscgen_api.h"
#include "dir.h"
#include "textstream.h"
#include "stringutil.h"

static const int maxCmdLine = 40960;

static bool convertMapFile(TextStream &t,const QCString &mapName,const QCString &relPath,
                           const QCString &context,const QCString &srcFile,int srcLine)
{
  std::ifstream f = Portable::openInputStream(mapName);
  if (!f.is_open())
  {
    err("failed to open map file {} for inclusion in the docs!\n"
        "If you installed Graphviz/dot after a previous failing run, \n"
        "try deleting the output directory and rerun doxygen.\n",mapName);
    return false;
  }
  const int maxLineLen=1024;
  char url[maxLineLen];
  char ref[maxLineLen];
  int x1=0, y1=0, x2=0, y2=0;
  std::string line;
  while (getline(f,line))
  {
    bool isRef = false;
    //printf("ReadLine '%s'\n",line.c_str());
    if (literal_at(line.c_str(),"rect"))
    {
      // obtain the url and the coordinates in the order used by graphviz-1.5
      sscanf(line.c_str(),"rect %s %d,%d %d,%d",url,&x1,&y1,&x2,&y2);

      if (qstrcmp(url,"\\ref")==0 || qstrcmp(url,"@ref")==0)
      {
        isRef = true;
        sscanf(line.c_str(),"rect %s %s %d,%d %d,%d",ref,url,&x1,&y1,&x2,&y2);
      }

      // sanity checks
      if (y2<y1) { int temp=y2; y2=y1; y1=temp; }
      if (x2<x1) { int temp=x2; x2=x1; x1=temp; }


      bool link = false;
      if ( isRef )
      {
        // handle doxygen \ref tag URL reference

        auto parser { createDocParser() };
        auto dfAst  { createRef( *parser.get(), url, context, srcFile, srcLine) };
        auto dfAstImpl = dynamic_cast<const DocNodeAST*>(dfAst.get());
        const DocRef *df = std::get_if<DocRef>(&dfAstImpl->root);
        if (!df->file().isEmpty() || !df->anchor().isEmpty())
        {
          link = true;
          t << "<area href=\"";
          t << externalRef(relPath,df->ref(),TRUE);
        }
        if (!df->file().isEmpty())
        {
          QCString fn = df->file();
          addHtmlExtensionIfMissing(fn);
          t << fn;
        }
        if (!df->anchor().isEmpty())
        {
          t << "#" << df->anchor();
        }
      }
      else
      {
        link = true;
        t << "<area href=\"";
        t << url;
      }
      if (link)
      {
        t << "\" shape=\"rect\" coords=\""
          << x1 << "," << y1 << "," << x2 << "," << y2 << "\""
          << " alt=\"\"/>\n";
      }
    }
  }

  return true;
}

static bool do_mscgen_generate(const QCString& inFile,const QCString& outFile,mscgen_format_t msc_format,
                               const QCString &srcFile,int srcLine)
{
  auto mscgen_tool = Config_getString(MSCGEN_TOOL).stripWhiteSpace();
  if (!mscgen_tool.isEmpty()) // use external mscgen tool
  {
    QCString type;
    switch (msc_format)
    {
      case mscgen_format_png:
        type = "png";
        break;
      case mscgen_format_eps:
        type = "eps";
        break;
      case mscgen_format_svg:
        type = "svg";
        break;
      case mscgen_format_pngmap:
      case mscgen_format_svgmap:
        type = "ismap";
        break;
    }
    int exitcode = Portable::system(mscgen_tool,"-T"+type+" -o "+outFile+" "+inFile);
    if (exitcode!=0)
    {
      err_full(srcFile,srcLine,"Problems running external tool {} given via MSCGEN_TOOL (exit status: {})."
          " Look for typos in your msc file and check error messages above.",
          mscgen_tool,exitcode);
      return false;
    }
  }
  else // use built-in mscgen tool
  {
    int code = mscgen_generate(inFile.data(),outFile.data(),msc_format);
    if (code!=0)
    {
      err_full(srcFile,srcLine,"Problems generating msc output (error={}). Look for typos in you msc file '{}'",
          mscgen_error2str(code),inFile);
      return false;
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

  mscgen_format_t msc_format = mscgen_format_png;
  QCString imgName = absOutFile;
  switch (format)
  {
    case MscOutputFormat::BITMAP:
      msc_format = mscgen_format_png;
      imgName+=".png";
      break;
    case MscOutputFormat::EPS:
      msc_format = mscgen_format_eps;
      imgName+=".eps";
      break;
    case MscOutputFormat::SVG:
      msc_format = mscgen_format_svg;
      imgName+=".svg";
      break;
    default:
      return;
  }
  if (!do_mscgen_generate(inFile,imgName,msc_format,srcFile,srcLine))
  {
    return;
  }

  if ( (format==MscOutputFormat::EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine, QCString::ExplicitSize);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                         qPrint(absOutFile),qPrint(absOutFile));
    if (Portable::system("epstopdf",epstopdfArgs)!=0)
    {
      err_full(srcFile,srcLine,"Problems running epstopdf when processing '{}.eps'. Check your TeX installation!", absOutFile);
    }
    else
    {
      Dir().remove((absOutFile + ".eps").data());
    }
  }

  int i=std::max(imgName.findRev('/'),imgName.findRev('\\'));
  if (i!=-1) // strip path
  {
    imgName=imgName.right(imgName.length()-i-1);
  }
  Doxygen::indexList->addImageFile(imgName);

}

static QCString getMscImageMapFromFile(const QCString& inFile, const QCString& /* outDir */,
                                const QCString& relPath,const QCString& context,
                                bool writeSVGMap,const QCString &srcFile,int srcLine)
{
  QCString outFile = inFile + ".map";

  if (!do_mscgen_generate(inFile,outFile,
                            writeSVGMap ? mscgen_format_svgmap : mscgen_format_pngmap,
                            srcFile,srcLine))
    return "";

  TextStream t;
  convertMapFile(t, outFile, relPath, context, srcFile, srcLine);

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
    case MscOutputFormat::BITMAP:
      t << "png";
      break;
    case MscOutputFormat::EPS:
      t << "eps";
      break;
    case MscOutputFormat::SVG:
      t << "svg";
      break;
    default:
      t << "unknown";
  }
  QCString imap = getMscImageMapFromFile(inFile,outDir,relPath,context,format==MscOutputFormat::SVG,srcFile,srcLine);
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


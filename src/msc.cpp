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

static bool convertMapFile(TextStream &t,const DString &mapName,const DString &relPath,
                           const DString &context,const DString &srcFile,int srcLine)
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

      if (dstrcmp(url,"\\ref")==0 || dstrcmp(url,"@ref")==0)
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
        if (!df->file().empty() || !df->anchor().empty())
        {
          link = true;
          t << "<area href=\"";
          t << externalRef(relPath,df->ref(),true);
        }
        if (!df->file().empty())
        {
          DString fn = df->file();
          addHtmlExtensionIfMissing(fn);
          t << fn;
        }
        if (!df->anchor().empty())
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

static bool do_mscgen_generate(const DString& inFile,const DString& outFile,mscgen_format_t msc_format,
                               const DString &srcFile,int srcLine)
{
  auto mscgen_tool = Config_getString(MSCGEN_TOOL).stripWhiteSpace();
  if (!mscgen_tool.empty()) // use external mscgen tool
  {
    DString type;
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

void writeMscGraphFromFile(const DString &inFile,const DString &outDir,
                           const DString &outFile,MscOutputFormat format,
                           const DString &srcFile,int srcLine,bool toIndex
                          )
{
  DString absOutFile = outDir;
  absOutFile+=Portable::pathSeparator();
  absOutFile+=outFile;

  mscgen_format_t msc_format = mscgen_format_png;
  DString imgName = absOutFile;
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
    DString epstopdfArgs(maxCmdLine, DString::ExplicitSize);
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

  size_t i0 = imgName.rfind('/');
  size_t i1 = imgName.rfind('\\');
  size_t i  = i0!=DString::npos && i1!=DString::npos ? std::max(i0,i1) :
              i0!=DString::npos ? i0 : i1;
  if (i!=DString::npos) // strip path
  {
    imgName=imgName.mid(i+1);
  }
  if (toIndex) Doxygen::indexList->addImageFile(imgName);

}

static DString getMscImageMapFromFile(const DString& inFile, const DString& /* outDir */,
                                const DString& relPath,const DString& context,
                                bool writeSVGMap,const DString &srcFile,int srcLine)
{
  DString outFile = inFile + ".map";

  if (!do_mscgen_generate(inFile,outFile,
                            writeSVGMap ? mscgen_format_svgmap : mscgen_format_pngmap,
                            srcFile,srcLine))
    return "";

  TextStream t;
  convertMapFile(t, outFile, relPath, context, srcFile, srcLine);

  Dir().remove(outFile.str());

  return t.str();
}

void writeMscImageMapFromFile(TextStream &t,const DString &inFile,
                              const DString &outDir,
                              const DString &relPath,
                              const DString &baseName,
                              const DString &context,
			      MscOutputFormat format,
                              const DString &srcFile,
                              int srcLine
 			    )
{
  DString mapName = baseName+".map";
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
  DString imap = getMscImageMapFromFile(inFile,outDir,relPath,context,format==MscOutputFormat::SVG,srcFile,srcLine);
  if (!imap.empty())
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


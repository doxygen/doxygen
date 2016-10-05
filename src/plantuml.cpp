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

#include <stdlib.h>

#include <qdir.h>
#include <qfile.h>
#include <qqueue.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

#include "plantuml.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"
#include "config.h"
#include "language.h"
#include "defargs.h"
#include "docparser.h"
#include "debug.h"
#include "pagedef.h"
#include "portable.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "ftextstream.h"
#include "md5.h"
#include "memberlist.h"
#include "groupdef.h"
#include "classlist.h"
#include "filename.h"
#include "namespacedef.h"
#include "memberdef.h"
#include "membergroup.h"

// Get the length of the longest possible command-line.  This is
// probably not complete, but I've done what I could based on
// http://www.in-ulm.de/~mascheck/various/argmax/
// autoconf has a check for this, but Doxygen doesn't use autoconf
#if defined(_OS_AIX_) || defined(_OS_NETBSD_) || defined(_OS_OPENBSD_) || defined(_OS_BSDI_) || defined(_OS_IRIX_) || defined(_OS_SOLARIS_) || defined(_OS_SUN_) || defined(_OS_ULTRIX_)
// BSD, AIX
#include <sys/param.h>
#define MAX_CMD_LEN NCARGS
#elif defined(_OS_CYGWIN_) || defined(_OS_HPUX_) || defined(_OS_UNIXWARE_) || defined(_OS_SCO_)
#include <limits.h>
#define MAX_CMD_LEN ARG_MAX
#elif defined(_OS_MAC_) || defined(_OS_OSF_)
#include <sys/syslimits.h>
#define MAX_CMD_LEN ARG_MAX
#elif defined(_OS_LINUX_)
#include <linux/limits.h>
#define MAX_CMD_LEN ARG_MAX
#else
// Use a fairly conservative default
// SMELL What's the right include file/token for windows? beos? qnx? dgux?
#define MAX_CMD_LEN 40960
#endif

enum UMLDetail { Suppressed, Analysis, Implementation };

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
  static QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  static QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);

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
  pumlArgs+="-charset UTF-8 ";
  int exitCode;
  //printf("*** running: %s %s outDir:%s %s\n",pumlExe.data(),pumlArgs.data(),outDir,outFile);
  msg("Running PlantUML on generated file %s.pu\n",baseName);
  portable_sysTimerStart();
  if ((exitCode=portable_system(pumlExe,pumlArgs,TRUE))!=0)
  {
    err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
        qPrint(plantumlJarPath),exitCode);
  }
  else if (Config_getBool(DOT_CLEANUP))
  {
    QFile(QCString(baseName)+".pu").remove();
  }
  portable_sysTimerStop();
  if ( (format==PUML_EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
     QCString epstopdfArgs(MAX_CMD_LEN);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",baseName,baseName);
    portable_sysTimerStart();
    if ((exitCode=portable_system("epstopdf",epstopdfArgs))!=0)
    {
      err("Problems running epstopdf. Check your TeX installation! Exit code: %d\n",exitCode);
    }
    portable_sysTimerStop();
  }
}


QCString stripQualifiers(const QCString& s)
{
  bool done;
  QCString rv(s);
  do
  {
    done=TRUE;
    if (rv.stripPrefix("friend "))
    {
      done=FALSE;
    }
    if (rv.stripPrefix("inline "))
    {
      done=FALSE;
    }
    if (rv.stripPrefix("explicit "))
    {
      done=FALSE;
    }
    if (rv.stripPrefix("immutable "))
    {
      done=FALSE;
    }
    if (rv.stripPrefix("mutable "))
    {
      done=FALSE;
    }
    if (rv.stripPrefix("virtual "))
    {
      done=FALSE;
    }
  } while (!done);
  return rv;
}

// get the detail level configuration setting as a true enum
UMLDetail getDetailLevel()
{
  static const QCString detailStr = Config_getEnum(PLANTUML_DETAIL_LEVEL);
  if (detailStr == "suppressed") return Suppressed;
  if (detailStr == "analysis") return Analysis;
  return Implementation;
}

// return a copy of the given string with all white space removed
QCString removeWhiteSpace(const QCString& str)
{
  QCString result(str);
  const char *from = result.data();
  char *to = result.rawData();
  while (*from)
  {
    if (!isspace(*from))
    {
      if (from != to) *to = *from;
      to++;
    }
    from++;
  }
  result.resize((int)((long)to - (long)result.data()) + 1 );
  return result;
}

//--------------------------------------------------------------------

static const char svgZoomHeader[] =
"<svg id=\"main\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xml:space=\"preserve\" onload=\"init(evt)\">\n"
"<style type=\"text/css\"><![CDATA[\n"
".edge:hover path { stroke: red; }\n"
".edge:hover polygon { stroke: red; fill: red; }\n"
"]]></style>\n"
"<script type=\"text/javascript\"><![CDATA[\n"
"var edges = document.getElementsByTagName('g');\n"
"if (edges && edges.length) {\n"
"  for (var i=0;i<edges.length;i++) {\n"
"    if (edges[i].id.substr(0,4)=='edge') {\n"
"      edges[i].setAttribute('class','edge');\n"
"    }\n"
"  }\n"
"}\n"
"]]></script>\n"
"        <defs>\n"
"                <circle id=\"rim\" cx=\"0\" cy=\"0\" r=\"7\"/>\n"
"                <circle id=\"rim2\" cx=\"0\" cy=\"0\" r=\"3.5\"/>\n"
"                <g id=\"zoomPlus\">\n"
"                        <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                                <set attributeName=\"fill\" to=\"#808080\" begin=\"zoomplus.mouseover\" end=\"zoomplus.mouseout\"/>\n"
"                        </use>\n"
"                        <path d=\"M-4,0h8M0,-4v8\" fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" pointer-events=\"none\"/>\n"
"                </g>\n"
"                <g id=\"zoomMin\">\n"
"                        <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                                <set attributeName=\"fill\" to=\"#808080\" begin=\"zoomminus.mouseover\" end=\"zoomminus.mouseout\"/>\n"
"                        </use>\n"
"                        <path d=\"M-4,0h8\" fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" pointer-events=\"none\"/>\n"
"                </g>\n"
"                <g id=\"dirArrow\">\n"
"                        <path fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" d=\"M0,-3.0v7 M-2.5,-0.5L0,-3.0L2.5,-0.5\"/>\n"
"                </g>\n"
"               <g id=\"resetDef\">\n"
"                       <use xlink:href=\"#rim2\" fill=\"#404040\">\n"
"                               <set attributeName=\"fill\" to=\"#808080\" begin=\"reset.mouseover\" end=\"reset.mouseout\"/>\n"
"                       </use>\n"
"               </g>\n"
"        </defs>\n"
"\n"
"<script type=\"text/javascript\">\n"
;

static const char svgZoomFooter[] =
// navigation panel
"        <g id=\"navigator\" transform=\"translate(0 0)\" fill=\"#404254\">\n"
"                <rect fill=\"#f2f5e9\" fill-opacity=\"0.5\" stroke=\"#606060\" stroke-width=\".5\" x=\"0\" y=\"0\" width=\"60\" height=\"60\"/>\n"
// zoom in
"                <use id=\"zoomplus\" xlink:href=\"#zoomPlus\" x=\"17\" y=\"9\" onmousedown=\"handleZoom(evt,'in')\"/>\n"
// zoom out
"                <use id=\"zoomminus\" xlink:href=\"#zoomMin\" x=\"42\" y=\"9\" onmousedown=\"handleZoom(evt,'out')\"/>\n"
// reset zoom
"                <use id=\"reset\" xlink:href=\"#resetDef\" x=\"30\" y=\"36\" onmousedown=\"handleReset()\"/>\n"
// arrow up
"                <g id=\"arrowUp\" xlink:href=\"#dirArrow\" transform=\"translate(30 24)\" onmousedown=\"handlePan(0,-1)\">\n"
"                  <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                        <set attributeName=\"fill\" to=\"#808080\" begin=\"arrowUp.mouseover\" end=\"arrowUp.mouseout\"/>\n"
"                  </use>\n"
"                  <path fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" d=\"M0,-3.0v7 M-2.5,-0.5L0,-3.0L2.5,-0.5\"/>\n"
"                </g>\n"
// arrow right
"                <g id=\"arrowRight\" xlink:href=\"#dirArrow\" transform=\"rotate(90) translate(36 -43)\" onmousedown=\"handlePan(1,0)\">\n"
"                  <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                        <set attributeName=\"fill\" to=\"#808080\" begin=\"arrowRight.mouseover\" end=\"arrowRight.mouseout\"/>\n"
"                  </use>\n"
"                  <path fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" d=\"M0,-3.0v7 M-2.5,-0.5L0,-3.0L2.5,-0.5\"/>\n"
"                </g>\n"
// arrow down
"                <g id=\"arrowDown\" xlink:href=\"#dirArrow\" transform=\"rotate(180) translate(-30 -48)\" onmousedown=\"handlePan(0,1)\">\n"
"                  <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                        <set attributeName=\"fill\" to=\"#808080\" begin=\"arrowDown.mouseover\" end=\"arrowDown.mouseout\"/>\n"
"                  </use>\n"
"                  <path fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" d=\"M0,-3.0v7 M-2.5,-0.5L0,-3.0L2.5,-0.5\"/>\n"
"                </g>\n"
// arrow left
"                <g id=\"arrowLeft\" xlink:href=\"#dirArrow\" transform=\"rotate(270) translate(-36 17)\" onmousedown=\"handlePan(-1,0)\">\n"
"                  <use xlink:href=\"#rim\" fill=\"#404040\">\n"
"                        <set attributeName=\"fill\" to=\"#808080\" begin=\"arrowLeft.mouseover\" end=\"arrowLeft.mouseout\"/>\n"
"                  </use>\n"
"                  <path fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" d=\"M0,-3.0v7 M-2.5,-0.5L0,-3.0L2.5,-0.5\"/>\n"
"                </g>\n"
"        </g>\n"
// link to orginial SVG
"        <svg viewBox=\"0 0 15 15\" width=\"100%\" height=\"30px\" preserveAspectRatio=\"xMaxYMin meet\">\n"
"         <g id=\"arrow_out\" transform=\"scale(0.3 0.3)\">\n"
"          <a xlink:href=\"$orgname\" target=\"_base\">\n"
"           <rect id=\"button\" ry=\"5\" rx=\"5\" y=\"6\" x=\"6\" height=\"38\" width=\"38\"\n"
"                fill=\"#f2f5e9\" fill-opacity=\"0.5\" stroke=\"#606060\" stroke-width=\"1.0\"/>\n"
"           <path id=\"arrow\"\n"
"             d=\"M 11.500037,31.436501 C 11.940474,20.09759 22.043105,11.32322 32.158766,21.979434 L 37.068811,17.246167 C 37.068811,17.246167 37.088388,32 37.088388,32 L 22.160133,31.978069 C 22.160133,31.978069 26.997745,27.140456 26.997745,27.140456 C 18.528582,18.264221 13.291696,25.230495 11.500037,31.436501 z\"\n"
"             style=\"fill:#404040;\"/>\n"
"          </a>\n"
"         </g>\n"
"        </svg>\n"
"</svg>\n"
;

//--------------------------------------------------------------------


static void writeGraphHeader(FTextStream &t,const QCString &title=QCString())
{
  static bool interactiveSVG = Config_getBool(INTERACTIVE_SVG);
  t << "@startuml" << endl;
  if (!title.isEmpty())
  {
    t << "title " << convertToXML(title) << endl;
  }
  if (interactiveSVG) // insert a comment to force regeneration when this
                      // option is toggled
  {
    t << " ' INTERACTIVE_SVG=YES" << endl;
  }
/*
  if (Config_getBool(PLANTUML_TRANSPARENT))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\","
       "labelfontname=\"" << FONTNAME << "\","
       "labelfontsize=\"" << FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\",shape=record];\n";
*/
}

static void writeGraphFooter(FTextStream &t)
{
  t << "@enduml" << endl;
}

static QCString replaceRef(const QCString &buf,const QCString relPath,
       bool urlOnly,const QCString &context,const QCString &target=QCString())
{
  // search for href="...", store ... part in link
  QCString href = "href";
  //bool isXLink=FALSE;
  int len = 6;
  int indexS = buf.find("href=\""), indexE;
  if (indexS>5 && buf.find("xlink:href=\"")!=-1) // XLink href (for SVG)
  {
    indexS-=6;
    len+=6;
    href.prepend("xlink:");
    //isXLink=TRUE;
  }
  if (indexS>=0 && (indexE=buf.find('"',indexS+len))!=-1)
  {
    QCString link = buf.mid(indexS+len,indexE-indexS-len);
    QCString result;
    if (urlOnly) // for user defined plantUML graphs
    {
      if (link.left(5)=="\\ref " || link.left(5)=="@ref ") // \ref url
      {
        result=href+"=\"";
        // fake ref node to resolve the url
        DocRef *df = new DocRef( (DocNode*) 0, link.mid(5), context );
        result+=externalRef(relPath,df->ref(),TRUE);
        if (!df->file().isEmpty())  
          result += df->file().data() + Doxygen::htmlFileExtension;
        if (!df->anchor().isEmpty()) 
          result += "#" + df->anchor();
        delete df;
        result += "\"";
      }
      else
      {
        result = href+"=\"" + link + "\"";
      }
    }
    else // ref$url (external ref via tag file), or $url (local ref)
    {
      int marker = link.find('$');
      if (marker!=-1)
      {
        QCString ref = link.left(marker);
        QCString url = link.mid(marker+1);
        if (!ref.isEmpty())
        {
          result = externalLinkTarget() + externalRef(relPath,ref,FALSE);
        }
        result+= href+"=\"";
        result+=externalRef(relPath,ref,TRUE);
        result+= url + "\"";
      }
      else // should not happen, but handle properly anyway
      {
        result = href+"=\"" + link + "\"";
      }
    }
    if (!target.isEmpty())
    {
      result+=" target=\""+target+"\"";
    }
    QCString leftPart = buf.left(indexS);
    QCString rightPart = buf.mid(indexE+1);
    return leftPart + result + rightPart;
  }
  else
  {
    return buf;
  }
}

static QArray<int> s_newNumber;
static int s_max_newNumber=0;

inline int reNumberNode(int number, bool doReNumbering)
{
  if (!doReNumbering) 
  {
    return number;
  } 
  else 
  {
    int s = s_newNumber.size();
    if (number>=s) 
    {
      int ns=0;
      ns = s * 3 / 2 + 5; // new size
      if (number>=ns) // number still doesn't fit
      {
        ns = number * 3 / 2 + 5;
      }
      s_newNumber.resize(ns);
      for (int i=s;i<ns;i++) // clear new part of the array
      {
        s_newNumber.at(i)=0;
      }
    }
    int i = s_newNumber.at(number);
    if (i == 0) // not yet mapped
    {
      i = ++s_max_newNumber; // start from 1
      s_newNumber.at(number) = i;
    }
    return i;
  }
}

static void resetReNumbering() 
{
  s_max_newNumber=0;
  s_newNumber.resize(s_max_newNumber);
}


static bool readBoundingBox(const char *fileName,int *width,int *height,bool isEps)
{
  QCString bb = isEps ? QCString("%%PageBoundingBox:") : QCString("/MediaBox [");
  QFile f(fileName);
  if (!f.open(IO_ReadOnly|IO_Raw)) 
  {
    //printf("readBoundingBox: could not open %s\n",fileName);
    return FALSE;
  }
  const int maxLineLen=1024;
  char buf[maxLineLen];
  while (!f.atEnd())
  {
    int numBytes = f.readLine(buf,maxLineLen-1); // read line
    if (numBytes>0)
    {
      buf[numBytes]='\0';
      const char *p = strstr(buf,bb);
      if (p) // found PageBoundingBox or /MediaBox string
      {
        int x,y;
        if (sscanf(p+bb.length(),"%d %d %d %d",&x,&y,width,height)!=4)
        {
          //printf("readBoundingBox sscanf fail\n");
          return FALSE;
        }
        return TRUE;
      }
    }
    else // read error!
    {
      //printf("Read error %d!\n",numBytes);
      return FALSE;
    }
  }
  err("Failed to extract bounding box from generated diagram file %s\n",fileName);
  return FALSE;
}

static bool writeVecGfxFigure(FTextStream &out,const QCString &baseName,
                           const QCString &figureName)
{
  int width=400,height=550;
  static bool usePdfLatex = Config_getBool(USE_PDFLATEX);
  if (usePdfLatex)
  {
    if (!readBoundingBox(figureName+".pdf",&width,&height,FALSE))
    {
      //printf("writeVecGfxFigure()=0\n");
      return FALSE;
    }
  }
  else
  {
    if (!readBoundingBox(figureName+".eps",&width,&height,TRUE))
    {
      //printf("writeVecGfxFigure()=0\n");
      return FALSE;
    }
  }
  //printf("Got PDF/EPS size %d,%d\n",width,height);
  int maxWidth  = 350;  /* approx. page width in points, excl. margins */
  int maxHeight = 550;  /* approx. page height in points, excl. margins */ 
  out << "\\nopagebreak\n"
         "\\begin{figure}[H]\n"
         "\\begin{center}\n"
         "\\leavevmode\n";
  if (width>maxWidth || height>maxHeight) // figure too big for page
  {
    // c*width/maxWidth > c*height/maxHeight, where c=maxWidth*maxHeight>0
    if (width*maxHeight>height*maxWidth)
    {
      out << "\\includegraphics[width=" << maxWidth << "pt]";
    }
    else
    {
      out << "\\includegraphics[height=" << maxHeight << "pt]";
    }
  }
  else
  {
     out << "\\includegraphics[width=" << width << "pt]";
  }

  out << "{" << baseName << "}\n"
         "\\end{center}\n"
         "\\end{figure}\n";

  //printf("writeVecGfxFigure()=1\n");
  return TRUE;
}

// extract size from a plantUML generated SVG file
static bool readSVGSize(const QCString &fileName,int *width,int *height)
{
  bool found=FALSE;
  QFile f(fileName);
  if (!f.open(IO_ReadOnly))
  {
    return FALSE;
  }
  const int maxLineLen=4096;
  char buf[maxLineLen];
  while (!f.atEnd() && !found)
  {
    int numBytes = f.readLine(buf,maxLineLen-1); // read line
    if (numBytes>0)
    {
      buf[numBytes]='\0';
      if (qstrncmp(buf,"<!--zoomable ",13)==0)
      {
        *width=-1;
        *height=-1;
        sscanf(buf,"<!--zoomable %d",height);
        //printf("Found zoomable for %s!\n",fileName.data());
        found=TRUE;
      }
      else if (sscanf(buf,"<svg width=\"%dpt\" height=\"%dpt\"",width,height)==2)
      {
        //printf("Found fixed size %dx%d for %s!\n",*width,*height,fileName.data());
        found=TRUE;
      }
    }
    else // read error!
    {
      //printf("Read error %d!\n",numBytes);
      return FALSE;
    }
  }
  return TRUE;
}

static void writeSVGNotSupported(FTextStream &out)
{
  out << "<p><b>This browser is not able to show SVG: try Firefox, Chrome, Safari, or Opera instead.</b></p>";
}

// check if a reference to a SVG figure can be written and does so if possible.
// return FALSE if not possible (for instance because the SVG file is not yet generated).
static bool writeSVGFigureLink(FTextStream &out,const QCString &relPath,
                           const QCString &baseName,const QCString &absImgName)
{
  int width=600,height=600;
  if (!readSVGSize(absImgName,&width,&height))
  {
    return FALSE;
  }
  if (width==-1)
  {
    if (height<=60) 
      height=300;
    else 
      height+=300; // add some extra space for zooming
    if (height>600) height=600; // clip to maximum height of 600 pixels
    out << "<div class=\"zoom\">";
    //out << "<object type=\"image/svg+xml\" data=\"" 
    //out << "<embed type=\"image/svg+xml\" src=\"" 
    out << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" 
        << relPath << baseName << ".svg\" width=\"100%\" height=\"" << height << "\">";
  }
  else
  {
    //out << "<object type=\"image/svg+xml\" data=\"" 
    //out << "<embed type=\"image/svg+xml\" src=\"" 
    out << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" 
        << relPath << baseName << ".svg\" width=\"" 
        << ((width*96+48)/72) << "\" height=\"" 
        << ((height*96+48)/72) << "\">";
  }
  writeSVGNotSupported(out);
  //out << "</object>";
  //out << "</embed>";
  out << "</iframe>";
  if (width==-1)
  {
    out << "</div>";
  }

  return TRUE;
}


static void removePlantUMLGraph(const QCString &plantUMLName)
{
  static bool plantUMLCleanUp = Config_getBool(DOT_CLEANUP); 
  if (plantUMLCleanUp)
  {
    QDir d;
    d.remove(plantUMLName);
  }
}



/*! Checks if a file "baseName".md5 exists. If so the contents
 *  are compared with \a md5. If equal FALSE is returned. If the .md5
 *  file does not exist or its contents are not equal to \a md5, 
 *  a new .md5 is generated with the \a md5 string as contents.
 */
static bool checkAndUpdateMd5Signature(const QCString &baseName,
            const QCString &md5)
{
  QFile f(baseName+".md5");
  if (f.open(IO_ReadOnly))
  {
    // read checksum
    QCString md5stored(33);
    int bytesRead=f.readBlock(md5stored.rawData(),32);
    md5stored[32]='\0';
    // compare checksum
    if (bytesRead==32 && md5==md5stored)
    {
      // bail out if equal
      return FALSE;
    }
  }
  f.close();
  // create checksum file
  if (f.open(IO_WriteOnly))
  {
    f.writeBlock(md5.data(),32); 
    f.close();
  }
  return TRUE;
}

static bool checkDeliverables(const QCString &file1,
                              const QCString &file2=QCString())
{
  bool file1Ok = TRUE;
  bool file2Ok = TRUE;
  if (!file1.isEmpty())
  {
    QFileInfo fi(file1);
    file1Ok = (fi.exists() && fi.size()>0);
  }
  if (!file2.isEmpty())
  {
    QFileInfo fi(file2);
    file2Ok = (fi.exists() && fi.size()>0);
  }
  return file1Ok && file2Ok;
}

//--------------------------------------------------------------------

/** Class representing a list of PlantUMLNode objects. */
class PlantUMLNodeList : public QList<PlantUMLNode>
{
  public:
    PlantUMLNodeList() : QList<PlantUMLNode>() {}
   ~PlantUMLNodeList() {}
  private:
    int compareValues(const PlantUMLNode *n1,const PlantUMLNode *n2) const
    {
      return qstricmp(n1->m_label,n2->m_label);
    }
};

//--------------------------------------------------------------------

PlantUMLRunner::PlantUMLRunner()
{
  clear();
}

bool PlantUMLRunner::addFile(const QCString& fileName)
{
  if ((m_cmd.length() + fileName.length() + 2) > MAX_CMD_LEN)
    return false;
  //printf("      PlantUMLRunner::addFile(%s)\n", qPrint(fileName));
  m_cmd += " " + fileName;
  return true;
}

void PlantUMLRunner::clear()
{
  static QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  m_cmd = "-Djava.awt.headless=true -jar \""+
    plantumlJarPath+"plantuml.jar\"";
}

bool PlantUMLRunner::run()
{
  //printf("PlantUMLRunner::run()\n");
  static QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  QCString pumlExe = "java";
  QCString pumlArgs;

  // plantuml supports processing multiple input files at once, but
  // will only generate a single target file format in a given
  // process.

  //printf("PlantUMLRunner::run()  m_file:%s  m_path:%s  m_imageName:%s  m_postCmd:%s  m_postArgs:%s  m_checkResult:%d  m_cleanUp:%d\n", qPrint(m_file), qPrint(m_path), qPrint(m_imageName), qPrint(m_postCmd), qPrint(m_postArgs), m_checkResult, m_cleanUp);

  int exitCode=0;

  //printf("Running: %s %s\n",qPrint(pumlExe),qPrint(m_cmd));
  if ((exitCode=portable_system(pumlExe,m_cmd,FALSE))!=0)
  {
    err("(2) Problems running PlantUML: exit code=%d, command='%s', arguments='%s'\n",
        exitCode,qPrint(pumlExe),qPrint(m_cmd));
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------------

PlantUMLFilePatcher::PlantUMLFilePatcher(const char *patchFile) 
  : m_patchFile(patchFile)
{
  m_maps.setAutoDelete(TRUE);
}

QCString PlantUMLFilePatcher::file() const
{
  return m_patchFile;
}

int PlantUMLFilePatcher::addFigure(const QCString &baseName,
                              const QCString &figureName,bool heightCheck)
{
  int id = m_maps.count();
  Map *map = new Map;
  map->mapFile  = figureName;
  map->urlOnly  = heightCheck;
  map->label    = baseName;
  map->zoomable = FALSE;
  map->graphId  = -1;
  m_maps.append(map);
  return id;
}

int PlantUMLFilePatcher::addSVGConversion(const QCString &relPath,bool urlOnly,
                                     const QCString &context,bool zoomable,
                                     int graphId)
{
  int id = m_maps.count();
  Map *map = new Map;
  map->relPath  = relPath;
  map->urlOnly  = urlOnly;
  map->context  = context;
  map->zoomable = zoomable;
  map->graphId  = graphId;
  m_maps.append(map);
  return id;
}

int PlantUMLFilePatcher::addSVGObject(const QCString &baseName,
                                 const QCString &absImgName,
                                 const QCString &relPath)
{
  int id = m_maps.count();
  Map *map = new Map;
  map->mapFile  = absImgName;
  map->relPath  = relPath;
  map->label    = baseName;
  map->zoomable = FALSE;
  map->graphId  = -1;
  m_maps.append(map);
  return id;
}

bool PlantUMLFilePatcher::run()
{
  //printf("PlantUMLFilePatcher::run(): %s\n",m_patchFile.data());
  static bool interactiveSVG = Config_getBool(INTERACTIVE_SVG);
  bool isSVGFile = m_patchFile.right(4)==".svg";
  int graphId = -1;
  QCString relPath;
  if (isSVGFile)
  {
    Map *map = m_maps.at(0); // there is only one 'map' for a SVG file
    interactiveSVG = interactiveSVG && map->zoomable;
    graphId = map->graphId;
    relPath = map->relPath;
    //printf("PlantUMLFilePatcher::addSVGConversion: file=%s zoomable=%d\n",
    //    m_patchFile.data(),map->zoomable);
  }
  QString tmpName = QString::fromUtf8(m_patchFile+".tmp");
  QString patchFile = QString::fromUtf8(m_patchFile);
  if (!QDir::current().rename(patchFile,tmpName))
  {
    err("Failed to rename file %s to %s!\n",m_patchFile.data(),tmpName.data());
    return FALSE;
  }
  QFile fi(tmpName);
  QFile fo(patchFile);
  if (!fi.open(IO_ReadOnly)) 
  {
    err("problem opening file %s for patching!\n",tmpName.data());
    QDir::current().rename(tmpName,patchFile);
    return FALSE;
  }
  if (!fo.open(IO_WriteOnly))
  {
    err("problem opening file %s for patching!\n",m_patchFile.data());
    QDir::current().rename(tmpName,patchFile);
    return FALSE;
  }
  FTextStream t(&fo);
  const int maxLineLen=100*1024;
  int lineNr=1;
  int width,height;
  bool insideHeader=FALSE;
  bool replacedHeader=FALSE;
  bool foundSize=FALSE;
  while (!fi.atEnd()) // foreach line
  {
    QCString line(maxLineLen);
    int numBytes = fi.readLine(line.rawData(),maxLineLen);
    if (numBytes<=0)
    {
      break;
    }

    //printf("line=[%s]\n",line.stripWhiteSpace().data());
    int i;
    ASSERT(numBytes<maxLineLen);
    if (isSVGFile)
    {
      if (interactiveSVG) 
      {
        if (line.find("<svg")!=-1 && !replacedHeader)
        {
          int count;
          count = sscanf(line.data(),"<svg width=\"%dpt\" height=\"%dpt\"",&width,&height);
          //printf("width=%d height=%d\n",width,height);
          foundSize = count==2 && (width>500 || height>450);
          if (foundSize) insideHeader=TRUE;
        }
        else if (insideHeader && !replacedHeader && line.find("<title>")!=-1)
        {
          if (foundSize)
          {
            // insert special replacement header for interactive SVGs
            t << "<!--zoomable " << height << " -->\n";
            t << svgZoomHeader;
            t << "var viewWidth = " << width << ";\n";
            t << "var viewHeight = " << height << ";\n";
            if (graphId>=0)
            {
              t << "var sectionId = 'dynsection-" << graphId << "';\n";
            }
            t << "</script>\n";
            t << "<script xlink:href=\"" << relPath << "svgpan.js\"/>\n";
            t << "<svg id=\"graph\" class=\"graph\">\n";
            t << "<g id=\"viewport\">\n";
          }
          insideHeader=FALSE;
          replacedHeader=TRUE;
        }
      }
      if (!insideHeader || !foundSize) // copy SVG and replace refs, 
                                       // unless we are inside the header of the SVG.
                                       // Then we replace it with another header.
      {
        Map *map = m_maps.at(0); // there is only one 'map' for a SVG file
        t << replaceRef(line,map->relPath,map->urlOnly,map->context,"_top");
      }
    }
    else if ((i=line.find("<!-- SVG"))!=-1 || (i=line.find("[!-- SVG"))!=-1)
    {
      //printf("Found marker at %d\n",i);
      int mapId=-1;
      t << line.left(i);
      int n = sscanf(line.data()+i+1,"!-- SVG %d",&mapId);
      if (n==1 && mapId>=0 && mapId<(int)m_maps.count())
      {
        int e = QMAX(line.find("--]"),line.find("-->"));
        Map *map = m_maps.at(mapId);
        //printf("PlantUMLFilePatcher::writeSVGFigure: file=%s zoomable=%d\n",
        //  m_patchFile.data(),map->zoomable);
        if (!writeSVGFigureLink(t,map->relPath,map->label,map->mapFile))
        {
          err("Problem extracting size from SVG file %s\n",map->mapFile.data());
        }
        if (e!=-1) t << line.mid(e+3);
      }
      else // error invalid map id!
      {
        err("Found invalid SVG id in file %s!\n",m_patchFile.data());
        t << line.mid(i);
      }
    }
    else if ((i=line.find("% FIG"))!=-1)
    {
      int mapId=-1;
      int n = sscanf(line.data()+i+2,"FIG %d",&mapId);
      //printf("line='%s' n=%d\n",line.data()+i,n);
      if (n==1 && mapId>=0 && mapId<(int)m_maps.count())
      {
        Map *map = m_maps.at(mapId);
        //printf("patching FIG %d in file %s with contents of %s\n",
        //   mapId,m_patchFile.data(),map->mapFile.data());
        writeVecGfxFigure(t,map->label,map->mapFile);
      }
      else // error invalid map id!
      {
        err("Found invalid bounding FIG %d in file %s!\n",mapId,m_patchFile.data());
        t << line;
      }
    }
    else
    {
      t << line;
    }
    lineNr++;
  }
  fi.close();
  if (isSVGFile && interactiveSVG && replacedHeader)
  {
    QCString orgName=m_patchFile.left(m_patchFile.length()-4)+"_org.svg";
    t << substitute(svgZoomFooter,"$orgname",stripPath(orgName));
    fo.close();
    // keep original SVG file so we can refer to it, we do need to replace
    // dummy link by real ones
    QFile fi(tmpName);
    QFile fo(orgName);
    if (!fi.open(IO_ReadOnly)) 
    {
      err("problem opening file %s for reading!\n",tmpName.data());
      return FALSE;
    }
    if (!fo.open(IO_WriteOnly))
    {
      err("problem opening file %s for writing!\n",orgName.data());
      return FALSE;
    }
    FTextStream t(&fo);
    while (!fi.atEnd()) // foreach line
    {
      QCString line(maxLineLen);
      int numBytes = fi.readLine(line.rawData(),maxLineLen);
      if (numBytes<=0)
      {
        break;
      }
      Map *map = m_maps.at(0); // there is only one 'map' for a SVG file
      t << replaceRef(line,map->relPath,map->urlOnly,map->context,"_top");
    }
    fi.close();
    fo.close();
  }
  // remove temporary file
  QDir::current().remove(tmpName);
  return TRUE;
}

//--------------------------------------------------------------------

void PlantUMLRunnerQueue::enqueue(PlantUMLRunner *runner)
{
  QMutexLocker locker(&m_mutex);
  m_queue.enqueue(runner);
  m_bufferNotEmpty.wakeAll();
}

PlantUMLRunner *PlantUMLRunnerQueue::dequeue()
{
  QMutexLocker locker(&m_mutex);
  while (m_queue.isEmpty())
  {
    // wait until something is added to the queue
    m_bufferNotEmpty.wait(&m_mutex);
  }
  PlantUMLRunner *result = m_queue.dequeue();
  return result;
}

uint PlantUMLRunnerQueue::count() const
{
  QMutexLocker locker(&m_mutex);
  return m_queue.count();
}

//--------------------------------------------------------------------

PlantUMLWorkerThread::PlantUMLWorkerThread(PlantUMLRunnerQueue *queue)
      : m_queue(queue)
{
  m_cleanupItems.setAutoDelete(TRUE);
}

void PlantUMLWorkerThread::run()
{
  PlantUMLRunner *runner;
  while ((runner=m_queue->dequeue()))
  {
    runner->run();
    PlantUMLRunner::CleanupItem cleanup = runner->cleanup();
    if (!cleanup.file.isEmpty())
    {
      m_cleanupItems.append(new PlantUMLRunner::CleanupItem(cleanup));
    }
  }
}

void PlantUMLWorkerThread::cleanup()
{
  QListIterator<PlantUMLRunner::CleanupItem> it(m_cleanupItems);
  PlantUMLRunner::CleanupItem *ci;
  for (;(ci=it.current());++it)
  {
    QDir(ci->path).remove(ci->file);
  }
}

//--------------------------------------------------------------------

PlantUMLManager *PlantUMLManager::m_theInstance = 0;

PlantUMLManager *PlantUMLManager::instance()
{
  if (!m_theInstance)
  {
    m_theInstance = new PlantUMLManager;
  }
  return m_theInstance;
}

PlantUMLManager::PlantUMLManager() : m_plantUMLMaps(1009)
{
  m_plantUMLRuns.setAutoDelete(TRUE);
  m_plantUMLMaps.setAutoDelete(TRUE);
  m_queue = new PlantUMLRunnerQueue;
  int i;
  int numThreads = QMIN(32,Config_getInt(PLANTUML_NUM_THREADS));
  if (numThreads!=1)
  {
    if (numThreads==0) numThreads = QMAX(2,QThread::idealThreadCount()+1);
    for (i=0;i<numThreads;i++)
    {
      PlantUMLWorkerThread *thread = new PlantUMLWorkerThread(m_queue);
      thread->start();
      if (thread->isRunning())
      {
        m_workers.append(thread);
      }
      else // no more threads available!
      {
        delete thread;
      }
    }
    ASSERT(m_workers.count()>0);
  }
}

PlantUMLManager::~PlantUMLManager()
{
  delete m_queue;
}

//void PlantUMLManager::addRun(PlantUMLRunner *run)
void PlantUMLManager::addRun(const char *format,const char *input)
{
  QList<QCString> *runList;
  QCString key(format);
  if ((runList = m_plantUMLRuns.find(key)) == NULL)
  {
    runList = new QList<QCString>;
    m_plantUMLRuns.append(key, runList);
  }
  runList->append(new QCString(input));
}

int PlantUMLManager::addFigure(const QCString &file,const QCString &baseName,
                          const QCString &figureName,bool heightCheck)
{
  PlantUMLFilePatcher *map = m_plantUMLMaps.find(file);
  if (map==0)
  {
    map = new PlantUMLFilePatcher(file);
    m_plantUMLMaps.append(file,map);
  }
  return map->addFigure(baseName,figureName,heightCheck);
}

int PlantUMLManager::addSVGConversion(const QCString &file,const QCString &relPath,
                       bool urlOnly,const QCString &context,bool zoomable,
                       int graphId)
{
  PlantUMLFilePatcher *map = m_plantUMLMaps.find(file);
  if (map==0)
  {
    map = new PlantUMLFilePatcher(file);
    m_plantUMLMaps.append(file,map);
  }
  return map->addSVGConversion(relPath,urlOnly,context,zoomable,graphId);
}

int PlantUMLManager::addSVGObject(const QCString &file,const QCString &baseName,
                             const QCString &absImgName,const QCString &relPath)
{
  PlantUMLFilePatcher *map = m_plantUMLMaps.find(file);
  if (map==0)
  {
    map = new PlantUMLFilePatcher(file);
    m_plantUMLMaps.append(file,map);
  }
  return map->addSVGObject(baseName,absImgName,relPath);
}

bool PlantUMLManager::run()
{
  uint numPlantUMLRuns = 0;
  uint numPlantUMLMaps = m_plantUMLMaps.count();
  SDict<QList<QCString> >::Iterator li(m_plantUMLRuns);
  QList<QCString> *runList;

  for (li.toFirst();(runList=li.current());++li)
    numPlantUMLRuns += runList->count();

  if (numPlantUMLRuns+numPlantUMLMaps>1)
  {
    if (m_workers.count()==0)
    {
      msg("Generating plantUML graphs in single threaded mode...\n");
    }
    else
    {
      msg("Generating plantUML graphs using %d parallel threads...\n",QMIN(numPlantUMLRuns+numPlantUMLMaps,m_workers.count()));
    }
  }
  int i=1;

  portable_sysTimerStart();
  // fill work queue with plantUML operations
  int prev=1, first=1;
  if (m_workers.count()==0) // no threads to work with
  {
    for (li.toFirst();(runList=li.current());++li)
    {
      QListIterator<QCString> fi(*runList);
      QCString *fileName;
      PlantUMLRunner runner;
      for (fi.toFirst();(fileName=fi.current());++fi)
      {
        if (!runner.addFile(*fileName))
        {
          msg("Running PlantUML for graphs %d - %d/%d\n",first,prev,numPlantUMLRuns);
          runner.run();
          runner.clear();
          first=prev+1;
        } // if (!runner.addFile(*fileName))
        prev++;
      } // for (fi.toFirst();(fileName=fi.current());++fi)
      msg("Running plantUML for graphs %d - %d/%d\n",first,prev-1,numPlantUMLRuns);
      runner.run();
      runner.clear();
    } // for (li.toFirst();(runList=li.current());++li)
  } // if (m_workers.count()==0)
  else // use multiple threads to run instances of plantUML in parallel
  {
    uint maxFilesPerWorker = numPlantUMLRuns / m_workers.count();
    uint numRunnerFiles = 0;
    PlantUMLRunner *runner = new PlantUMLRunner;
    //printf("PlantUMLManager::run() starting thread queueing\n");
    for (li.toFirst();(runList=li.current());++li)
    {
      //printf("  PlantUMLManager::run() new run list\n");
      QListIterator<QCString> fi(*runList);
      QCString *fileName;
      for (fi.toFirst();(fileName=fi.current());++fi)
      {
        //printf("    PlantUMLManager::run() adding %s (?)\n", qPrint(*fileName));
        if ((++numRunnerFiles >= maxFilesPerWorker) ||
            (!runner->addFile(*fileName)))
        {
          //printf("  PlantUMLManager::run() runner queued (a)\n");
          m_queue->enqueue(runner);
          runner = new PlantUMLRunner;
          numRunnerFiles = 1;
          runner->addFile(*fileName);
        }
      }
      //printf("  PlantUMLManager::run() runner queued (b)\n");
      m_queue->enqueue(runner);
    }
    // wait for the queue to become empty
    while ((i=m_queue->count())>0)
    {
      i = numPlantUMLRuns - i;
      while (i>=prev)
      {
        msg("Running plantUML for graph %d/%d\n",prev,numPlantUMLRuns);
        prev++;
      }
      portable_sleep(100);
    }
    while ((int)numPlantUMLRuns>=prev)
    {
      msg("Running plantUML for graph %d/%d\n",prev,numPlantUMLRuns);
      prev++;
    }
    // signal the workers we are done
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_queue->enqueue(0); // add terminator for each worker
    }
    // wait for the workers to finish
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->wait();
    }
    // clean up plantUML files from main thread
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->cleanup();
    }
  }
  portable_sysTimerStop();

  // patch the output file and insert the maps and figures
  i=1;
  SDict<PlantUMLFilePatcher>::Iterator di(m_plantUMLMaps);
  PlantUMLFilePatcher *map;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and 
  // then the other files. 
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)==".svg")
    {
      msg("Patching output file %d/%d for PlantUML\n",i,numPlantUMLMaps);
      if (!map->run()) return FALSE;
      i++;
    }
  }
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)!=".svg")
    {
      msg("Patching output file %d/%d for PlantUML\n",i,numPlantUMLMaps);
      if (!map->run()) return FALSE;
      i++;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------


/*! helper function that deletes all nodes in a connected graph, given
 *  one of the graph's nodes
 */
static void deleteNodes(PlantUMLNode *node,SDict<PlantUMLNode> *skipNodes=0)
{
  //printf("deleteNodes skipNodes=%p\n",skipNodes);
  static PlantUMLNodeList deletedNodes;
  deletedNodes.setAutoDelete(TRUE);
  node->deleteNode(deletedNodes,skipNodes); // collect nodes to be deleted.
  deletedNodes.clear(); // actually remove the nodes.
}

PlantUMLNode::PlantUMLNode(int n,const char *lab,const char *tip, const char *url,
                 bool isRoot,ClassDef *cd)
  : m_subgraphId(-1)
  , m_number(n)
  , m_label(lab)
  , m_tooltip(tip)
  , m_url(url)
  , m_parents(0)
  , m_children(0)
  , m_edgeInfo(0)
  , m_deleted(FALSE)
  , m_written(FALSE)
  , m_hasDoc(FALSE)
  , m_isRoot(isRoot)
  , m_classDef(cd)
  , m_visible(FALSE)
  , m_truncated(Unknown)
  , m_distance(1000)
{
}

PlantUMLNode::PlantUMLNode(const PlantUMLNode& right)
  : m_subgraphId(right.m_subgraphId)
  , m_number(right.m_number)
  , m_label(right.m_label)
  , m_tooltip(right.m_tooltip)
  , m_url(right.m_url)
    // initialize the list pointers to NULL, copy later if needed
  , m_parents(0)
  , m_children(0)
  , m_edgeInfo(0)
  , m_deleted(right.m_deleted)
  , m_written(right.m_written)
  , m_hasDoc(right.m_hasDoc)
  , m_isRoot(right.m_isRoot)
    // m_classDef is not destroyed by PlantUMLNode so it is probably safe
    // to do a shallow copy.
  , m_classDef(right.m_classDef)
  , m_visible(right.m_visible)
  , m_truncated(right.m_truncated)
  , m_distance(right.m_distance)
{
  // must do a deep copy of m_parents, m_children, m_edgeInfo
  if (right.m_parents)
  {
    m_parents = new QList<PlantUMLNode>(*right.m_parents);
  }
  if (right.m_children)
  {
    m_children = new QList<PlantUMLNode>(*right.m_children);
  }
  if (right.m_edgeInfo)
  {
    m_edgeInfo = new QList<PlantUMLEdgeInfo>(*right.m_edgeInfo);
  }
}

PlantUMLNode::~PlantUMLNode()
{
  delete m_children;
  delete m_parents;
  delete m_edgeInfo;
}

void PlantUMLNode::addChild(PlantUMLNode *n,
                            PlantUMLEdgeInfo::Relation relation,
                            const char *edgeLab,
                            const char *edgeURL,
                            const char *leftLab,
                            const char *rightLab
                            )
{
  if (m_children==0)
  {
    m_children = new QList<PlantUMLNode>;
    m_edgeInfo = new QList<PlantUMLEdgeInfo>;
    m_edgeInfo->setAutoDelete(TRUE);
  }
  m_children->append(n);
  PlantUMLEdgeInfo *ei = new PlantUMLEdgeInfo;
  ei->m_edgeType = relation;
  ei->m_label = edgeLab;
  ei->m_labelLeft = leftLab;
  ei->m_labelRight = rightLab;
  /// @todo add support for multiplicity annotations, if possible
  ei->m_url   = edgeURL;
  m_edgeInfo->append(ei);
}

void PlantUMLNode::addParent(PlantUMLNode *n)
{
  if (m_parents==0)
  {
    m_parents = new QList<PlantUMLNode>;
  }
  m_parents->append(n);
}

void PlantUMLNode::removeChild(PlantUMLNode *n)
{
  if (m_children) m_children->remove(n);
}

void PlantUMLNode::removeParent(PlantUMLNode *n)
{
  if (m_parents) m_parents->remove(n);
}

void PlantUMLNode::deleteNode(PlantUMLNodeList &deletedList,SDict<PlantUMLNode> *skipNodes)
{
  if (m_deleted) return; // avoid recursive loops in case the graph has cycles
  m_deleted=TRUE;
  if (m_parents!=0) // delete all parent nodes of this node
  {
    QListIterator<PlantUMLNode> dnlip(*m_parents);
    PlantUMLNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      //pn->removeChild(this);
      pn->deleteNode(deletedList,skipNodes);
    }
  }
  if (m_children!=0) // delete all child nodes of this node
  {
    QListIterator<PlantUMLNode> dnlic(*m_children);
    PlantUMLNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      //cn->removeParent(this);
      cn->deleteNode(deletedList,skipNodes);
    }
  }
  // add this node to the list of deleted nodes.
  //printf("skipNodes=%p find(%p)=%p\n",skipNodes,this,skipNodes ? skipNodes->find((int)this) : 0);
  if (skipNodes==0 || skipNodes->find((char*)this)==0)
  {
    //printf("deleting\n");
    deletedList.append(this);
  }
}

void PlantUMLNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance = distance;
}

static QCString convertLabel(const QCString &l, bool stripTemplate = false)
{
  QCString result;
  QCString bBefore("\\_/<({[: =-+@%#~?$"); // break before character set
  QCString bAfter(">]),:;|");              // break after  character set
  char cs[2];
  cs[1]=0;
  int len=l.length();
  if (len==0) return result;
  int charsLeft=len;
  unsigned templCount = 0;
  int lastScope = l.findRev("::");
  // Make sure not to incorrectly and partially erase labels where the
  // template argument itself has a class/namespace scope.  This is
  // mostly done when Doxygen fails to identify a template
  // instance. (e.g. DefinitionContext<ClassContext::Private>).
  if (l.findRev(">") > lastScope)
    lastScope = -1;
  for (int i = 0; i < len; i++)
  {
    QCString replacement;
    switch(l[i])
    {
      case '<':
        templCount++;
        cs[0]=l[i];
        replacement=cs;
        break;
      case '>':
        templCount--;
        // This could be the final closing mark for a template, so do
        // some extra checking to make sure it isn't inappropriately
        // added to the output.
        if (stripTemplate || (i < lastScope))
        {
          replacement="";
        }
        else
        {
          cs[0]=l[i];
          replacement=cs;
        }
        break;
      case '\\': replacement="\\\\"; break;
      case '\n': replacement="\\n"; break;
      case '|':  replacement="\\|"; break;
      case '{':  replacement="\\{"; break;
      case '}':  replacement="\\}"; break;
      case '"':  replacement="\\\""; break;
      default:   cs[0]=l[i]; replacement=cs; break;
    } // switch(l[i])
    // Remove template parameters if requested (e.g. for arrows)
    // or if necessary (e.g. nested classes within templates).
    if ((templCount > 0) &&
        (stripTemplate ||
         (i < lastScope)))
    {
      // do nothing
    }
    else
    {
      result+=replacement;
    }
    charsLeft--;
  } // for (int i = 0; i < len; i++)
  return result;
}


static void writeBoxMemberList(FTextStream &t,
            char prot,MemberList *ml,ClassDef *scope,
            bool isStatic=FALSE,const QDict<void> *skipNames=0)
{
  static const UMLDetail detailLevel = getDetailLevel();
  (void)isStatic;
  if (ml)
  {
    MemberListIterator mlia(*ml);
    MemberDef *mma;
    int totalCount=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef()==scope && 
          (skipNames==0 || skipNames->find(mma->name())==0))
      {
        totalCount++;
      }
    }

    int count=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef() == scope &&
          (skipNames==0 || skipNames->find(mma->name())==0))
      {
        static int limit = Config_getInt(UML_LIMIT_NUM_FIELDS);
        if (limit>0 && (totalCount>limit*3/2 && count>=limit))
        {
          /** @todo this doesn't work right with PlantUML as it sticks
           * all such messages in with the data members.
           * Additionally, because each group (data vs methods)
           * consists of multiple calls to this method, we can't
           * simply insert our own division markers as that would
           * result in one division per protection level x method vs
           * data.  Messy.  Finally, I personally don't like the fact
           * that the way this is implemented means that each
           * protection level has its own limit.  I'd much rather have
           * a limit for all data members and all methods, possibly
           * combined. */
          //t << theTranslator->trAndMore(QCString().sprintf("%d",totalCount-count)) << endl;
          break;
        }
        else
        {
          t << "  ";
          if (detailLevel == Implementation)
            t << prot << " ";
          if (mma->isStatic())
            t << "{static} ";
          else if (mma->isAbstract())
            t << "{abstract} ";
          t << convertLabel(mma->name());
          if (!mma->isObjCMethod() && 
              (mma->isFunction() || mma->isSlot() || mma->isSignal()))
          {
            if (detailLevel == Implementation)
            {
              t << "(";
              ArgumentList *alist(mma->declArgumentList());
              if (alist)
              {
                Argument *arg;
                QListIterator<Argument> ali(*alist);
                bool comma = false; // insert a comma into the string
                for (ali.toFirst();(arg=ali.current());++ali)
                {
                  if (comma)
                    t << ", ";
                  comma = true;
                  t << arg->name <<  ": " << arg->type;
                }
              }
              t << ")";
            }
            else
            {
              t << "()";
            }
          }
          if (mma->typeString())
          {
            QCString typeStr = stripQualifiers(mma->typeString());
            t << ": " << typeStr;
          }
          t << endl;
          count++;
        }
      }
    }
    // write member groups within the memberlist
    MemberGroupList *mgl = ml->getMemberGroupList();
    if (mgl)
    {
      MemberGroupListIterator mgli(*mgl);
      MemberGroup *mg;
      for (mgli.toFirst();(mg=mgli.current());++mgli)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,prot,mg->members(),scope,isStatic,skipNames);
        }
      }
    }
  }
}

static QCString stripProtectionPrefix(const QCString &s)
{
  if (!s.isEmpty() && (s[0]=='-' || s[0]=='+' || s[0]=='~' || s[0]=='#'))
  {
    return s.mid(1);
  }
  else
  {
    return s;
  }
}

void PlantUMLNode::writeBox(FTextStream &t,GraphType gt)
{
  static const UMLDetail detailLevel = getDetailLevel();
/*
  Debug::print(
      Debug::Classes,0,
      "PlantUMLNode::writeBox(t,%d,?,%d,%d) %s\n",
      gt, hasNonReachableChildren, reNumber, qPrint(m_label));
*/
  if (m_classDef->isAbstract())
    t << "abstract ";
  if (m_classDef->compoundType() == ClassDef::Interface)
    t << "interface ";
  else
    t << "class ";
  t << convertLabel(m_label);

  if (m_classDef && (gt==Inheritance || gt==Collaboration))
  {
    // add names shown as relations to a dictionary, so we don't show
    // them as attributes as well
    QDict<void> arrowNames(17);
    if (m_edgeInfo)
    {
      // for each edge
      QListIterator<PlantUMLEdgeInfo> li(*m_edgeInfo);
      PlantUMLEdgeInfo *ei;
      for (li.toFirst();(ei=li.current());++li)
      {
        if (!ei->m_label.isEmpty()) // labels joined by \n
        {
          int li=ei->m_label.find('\n');
          int p=0;
          QCString lab;
          while ((li=ei->m_label.find('\n',p))!=-1)
          {
            lab = stripProtectionPrefix(ei->m_label.mid(p,li-p));
            arrowNames.insert(lab,(void*)0x8);
            p=li+1;
          }
          lab = stripProtectionPrefix(ei->m_label.right(ei->m_label.length()-p));
          arrowNames.insert(lab,(void*)0x8);
        }
      }
    }

    //printf("PlantUMLNode::writeBox for %s\n",m_classDef->name().data());

    if (detailLevel == Suppressed)
    {
      t << endl; // skip all the class details
      return;
    }

    t << " {" << endl;
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticAttribs),m_classDef,TRUE,&arrowNames);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_properties),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticAttribs),m_classDef,TRUE,&arrowNames);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticAttribs),m_classDef,TRUE,&arrowNames);
    if (detailLevel == Implementation)
    {
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priAttribs),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticAttribs),m_classDef,TRUE,&arrowNames);
    }
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubMethods),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubSlots),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacMethods),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proMethods),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proSlots),m_classDef);
    if (detailLevel == Implementation)
    {
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priMethods),m_classDef);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticMethods),m_classDef,TRUE);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priSlots),m_classDef);
    }
    t << "}" << endl;
  }
}

void PlantUMLNode::writeArrow(FTextStream &t,
                              PlantUMLNode *cn,
                              PlantUMLEdgeInfo *ei)
{
/*
  Debug::print(
      Debug::Classes,0,
      "PlantUMLNode::writeArrow(t,%d,%d,%s,ei,%d,%d,%d) %s\n",
      gt, format, qPrint(cn->m_label), topDown, pointBack, reNumber,
      qPrint(m_label));
*/
  t << convertLabel(cn->m_label,true);

  if (ei && !ei->m_labelLeft.isEmpty())
    t << " \"" << convertLabel(ei->m_labelLeft) << "\"";

  t << " " << ei->getArrow();

  if (ei && !ei->m_labelRight.isEmpty())
    t << " \"" << convertLabel(ei->m_labelRight) << "\"";

  t << " " << convertLabel(m_label,true);

  if (ei && !ei->m_label.isEmpty())
    t << " : \"" << convertLabel(ei->m_label) << "\"";

  t << endl;
}

void PlantUMLNode::write(FTextStream &t,
                    GraphType gt,
                    PlantUMLOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows,
                    bool reNumber
                   )
{
  //printf("PlantUMLNode::write(%d) name=%s this=%p written=%d\n",distance,m_label.data(),this,m_written);
  //printf("PlantUMLNode::write(stream,GraphType=%d,Format=%d,topDown=%d,toChildren=%d,backArrows=%d,reNumber=%d) %s\n", gt, format, topDown, toChildren, backArrows, reNumber, qPrint(m_label));
  if (m_written) return; // node already written to the output
  if (!m_visible) return; // node is not visible
  writeBox(t,gt);
  m_written=TRUE;
  QList<PlantUMLNode> *nl = toChildren ? m_children : m_parents; 
  //printf("PlantUMLNode::write() nl?%d\n", (nl!=NULL));
  if (nl)
  {
    if (toChildren)
    {
      QListIterator<PlantUMLNode>  dnli1(*nl);
      QListIterator<PlantUMLEdgeInfo> dnli2(*m_edgeInfo);
      PlantUMLNode *cn;
      for (dnli1.toFirst();(cn=dnli1.current());++dnli1,++dnli2)
      {
        if (cn->isVisible())
        {
          //printf("write arrow 1 %s%s%s\n",qPrint(label()),backArrows?"<-":"->",qPrint(cn->label()));
          writeArrow(t,cn,dnli2.current());
        }
        cn->write(t,gt,format,topDown,toChildren,backArrows,reNumber);
      }
    }
    else // render parents
    {
      QListIterator<PlantUMLNode> dnli(*nl);
      PlantUMLNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->isVisible())
        {
          //printf("write arrow 2 %s%s%s\n",qPrint(label()),backArrows?"<-":"->",qPrint(pn->label()));
          writeArrow(t,pn,pn->m_edgeInfo->at(pn->m_children->findRef(this)));
        }
        pn->write(t,gt,format,TRUE,FALSE,backArrows,reNumber);
      }
    }
  }
  //printf("end PlantUMLNode::write(%d) name=%s\n",distance,m_label.data());
}

void PlantUMLNode::writeXML(FTextStream &t,bool isClassGraph)
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << "        <link refid=\"" << convertToXML(urlPtr) << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << convertToXML(refPtr) << "\"";
      }
      t << "/>" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<PlantUMLNode> nli(*m_children);
    QListIterator<PlantUMLEdgeInfo> eli(*m_edgeInfo);
    PlantUMLNode *childNode;
    PlantUMLEdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        t << edgeInfo->getEdgeType();
      }
      else // include graph
      {
        t << "include"; 
      }
      t << "\">" << endl;
      if (!edgeInfo->m_label.isEmpty()) 
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->m_label.find('\n',p))!=-1)
        {
          t << "          <edgelabel>" 
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>" 
          << convertToXML(edgeInfo->m_label.right(edgeInfo->m_label.length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    } 
  }
  t << "      </node>" << endl;
}

void PlantUMLNode::writeDocbook(FTextStream &t,bool isClassGraph)
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << "        <link refid=\"" << convertToXML(urlPtr) << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << convertToXML(refPtr) << "\"";
      }
      t << "/>" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<PlantUMLNode> nli(*m_children);
    QListIterator<PlantUMLEdgeInfo> eli(*m_edgeInfo);
    PlantUMLNode *childNode;
    PlantUMLEdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        t << edgeInfo->getEdgeType();
      }
      else // include graph
      {
        t << "include";
      }
      t << "\">" << endl;
      if (!edgeInfo->m_label.isEmpty())
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->m_label.find('\n',p))!=-1)
        {
          t << "          <edgelabel>"
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>"
          << convertToXML(edgeInfo->m_label.right(edgeInfo->m_label.length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    }
  }
  t << "      </node>" << endl;
}


void PlantUMLNode::writeDEF(FTextStream &t)
{
  const char* nodePrefix = "        node-";

  t << "      node = {" << endl;
  t << nodePrefix << "id    = " << m_number << ';' << endl;
  t << nodePrefix << "label = '" << m_label << "';" << endl;

  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << nodePrefix << "link = {" << endl << "  "
        << nodePrefix << "link-id = '" << urlPtr << "';" << endl;

      if (*refPtr!='\0')
      {
        t << "  " << nodePrefix << "link-external = '"
          << refPtr << "';" << endl;
      }
      t << "        };" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<PlantUMLNode> nli(*m_children);
    QListIterator<PlantUMLEdgeInfo> eli(*m_edgeInfo);
    PlantUMLNode *childNode;
    PlantUMLEdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        node-child = {" << endl;
      t << "          child-id = '" << childNode->m_number << "';" << endl;
      t << "          relation = ";
      t << edgeInfo->getEdgeType();
      t << ';' << endl;

      if (!edgeInfo->m_label.isEmpty()) 
      {
        t << "          edgelabel = <<_EnD_oF_dEf_TeXt_" << endl
          << edgeInfo->m_label << endl
          << "_EnD_oF_dEf_TeXt_;" << endl;
      }
      t << "        }; /* node-child */" << endl;
    } /* for (;childNode...) */
  }
  t << "      }; /* node */" << endl;
}


void PlantUMLNode::clearWriteFlag()
{
  m_written=FALSE;
  if (m_parents!=0)
  {
    QListIterator<PlantUMLNode> dnlip(*m_parents);
    PlantUMLNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->m_written)
      {
        pn->clearWriteFlag();
      }
    }
  }
  if (m_children!=0)
  {
    QListIterator<PlantUMLNode> dnlic(*m_children);
    PlantUMLNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->m_written)
      {
        cn->clearWriteFlag();
      }
    }
  }
}

void PlantUMLNode::colorConnectedNodes(int curColor)
{ 
  if (m_children)
  {
    QListIterator<PlantUMLNode> dnlic(*m_children);
    PlantUMLNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->m_subgraphId==-1) // uncolored child node
      {
        cn->m_subgraphId=curColor;
        cn->markAsVisible();
        cn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",cn->m_label.data(),cn,cn->m_subgraphId);
      }
    }
  }

  if (m_parents)
  {
    QListIterator<PlantUMLNode> dnlip(*m_parents);
    PlantUMLNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->m_subgraphId==-1) // uncolored parent node
      {
        pn->m_subgraphId=curColor;
        pn->markAsVisible();
        pn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",pn->m_label.data(),pn,pn->m_subgraphId);
      }
    }
  }
}

const PlantUMLNode *PlantUMLNode::findDocNode() const
{
  if (!m_url.isEmpty()) return this;
  //printf("findDocNode(): `%s'\n",m_label.data());
  if (m_parents)
  {
    QListIterator<PlantUMLNode> dnli(*m_parents);
    PlantUMLNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (!pn->m_hasDoc)
      {
        pn->m_hasDoc=TRUE;
        const PlantUMLNode *dn = pn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  if (m_children)
  {
    QListIterator<PlantUMLNode> dnli(*m_children);
    PlantUMLNode *cn;
    for (dnli.toFirst();(cn=dnli.current());++dnli)
    {
      if (!cn->m_hasDoc)
      {
        cn->m_hasDoc=TRUE;
        const PlantUMLNode *dn = cn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  return 0;
}

//--------------------------------------------------------------------

int PlantUMLClassGraph::m_curNodeNumber = 0;

void PlantUMLClassGraph::addClass(
    ClassDef *cd,PlantUMLNode *n,PlantUMLEdgeInfo::Relation relation,
    const char *label,const char *usedName,const char *templSpec,bool base,
    int distance,const char *leftLab,const char *rightLab)
{
  if (Config_getBool(HIDE_UNDOC_CLASSES) && !cd->isLinkable()) return;

/*
  Debug::print(Debug::Classes,0,
               "PlantUMLClassGraph::addClass(%s,%s,%d,%s,%s,%s,%d,%d)\n",
               qPrint(cd->className()), qPrint(n->label()), prot,
               label ? label : "",
               usedName ? usedName : "", templSpec ? templSpec : "", base,
               distance);
*/

  QCString className;
  if (usedName) // name is a typedef
  {
    className=usedName;
  }
  else if (templSpec) // name has a template part
  {
    className=insertTemplateSpecifierInScope(cd->name(),templSpec);
  }
  else // just a normal name
  {
    className=cd->displayName();
  }
  //printf("PlantUMLClassGraph::addClass(class=`%s',parent=%s,prot=%d,label=%s,dist=%d,usedName=%s,templSpec=%s,base=%d)\n",
  //                                 className.data(),n->m_label.data(),prot,label,distance,usedName,templSpec,base);
  PlantUMLNode *bn = m_usedNodes->find(className);
  if (bn) // class already inserted
  {
    if (base)
    {
      n->addChild(bn,relation,label,NULL,leftLab,rightLab);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,relation,label,NULL,leftLab,rightLab);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    //printf(" add exiting node %s of %s\n",bn->m_label.data(),n->m_label.data());
  }
  else // new class
  {
    QCString displayName=className;
    if (Config_getBool(HIDE_SCOPE_NAMES)) displayName=stripScope(displayName);
    QCString tmp_url;
    if (cd->isLinkable() && !cd->isHidden()) 
    {
      tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
      if (!cd->anchor().isEmpty())
      {
        tmp_url+="#"+cd->anchor();
      }
    }
    QCString tooltip = cd->briefDescriptionAsTooltip();
    bn = new PlantUMLNode(m_curNodeNumber++,
        displayName,
        tooltip,
        tmp_url.data(),
        FALSE,        // rootNode
        cd
       );
    if (base)
    {
      n->addChild(bn,relation,label,NULL,leftLab,rightLab);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,relation,label,NULL,leftLab,rightLab);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    m_usedNodes->insert(className,bn);
    //printf(" add new child node `%s' to %s hidden=%d url=%s\n",
    //    className.data(),n->m_label.data(),cd->isHidden(),tmp_url.data());
    //printf("PlantUMLClassGraph::addClass calling buildGraph\n");
    buildGraph(cd,bn,base,distance+1);
  }
}

void PlantUMLClassGraph::determineTruncatedNodes(QList<PlantUMLNode> &queue,bool includeParents)
{
  while (queue.count()>0)
  {
    PlantUMLNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==PlantUMLNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->m_children)
      {
        QListIterator<PlantUMLNode> li(*n->m_children);
        PlantUMLNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          if (!dn->isVisible()) 
            truncated = TRUE;
          else 
            queue.append(dn);
        }
      }
      if (n->m_parents && includeParents)
      {
        QListIterator<PlantUMLNode> li(*n->m_parents);
        PlantUMLNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          if (!dn->isVisible()) 
            truncated = TRUE;
          else 
            queue.append(dn);
        }
      }
      n->markAsTruncated(truncated);
    }
  }
}

bool PlantUMLClassGraph::determineVisibleNodes(PlantUMLNode *rootNode,
                                          int maxNodes,bool includeParents)
{
  QList<PlantUMLNode> childQueue;
  QList<PlantUMLNode> parentQueue;
  QArray<int> childTreeWidth;
  QArray<int> parentTreeWidth;
  childQueue.append(rootNode);
  if (includeParents) parentQueue.append(rootNode);
  bool firstNode=TRUE; // flag to force reprocessing rootNode in the parent loop 
                       // despite being marked visible in the child loop
  while ((childQueue.count()>0 || parentQueue.count()>0) && maxNodes>0)
  {
    static int maxDistance = Config_getInt(MAX_PLANTUML_GRAPH_DEPTH);
    if (childQueue.count()>0)
    {
      PlantUMLNode *n = childQueue.take(0);
      int distance = n->distance();
      if (!n->isVisible() && distance<=maxDistance) // not yet processed
      {
        if (distance>0)
        {
          int oldSize=(int)childTreeWidth.size();
          if (distance>oldSize)
          {
            childTreeWidth.resize(QMAX(childTreeWidth.size(),(uint)distance));
            int i; for (i=oldSize;i<distance;i++) childTreeWidth[i]=0;
          }
          childTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct children
        if (n->m_children)
        {
          QListIterator<PlantUMLNode> li(*n->m_children);
          PlantUMLNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            childQueue.append(dn);
          }
        }
      }
    }
    if (includeParents && parentQueue.count()>0)
    {
      PlantUMLNode *n = parentQueue.take(0);
      if ((!n->isVisible() || firstNode) && n->distance()<=maxDistance) // not yet processed
      {
        firstNode=FALSE;
        int distance = n->distance();
        if (distance>0)
        {
          int oldSize = (int)parentTreeWidth.size();
          if (distance>oldSize)
          {
            parentTreeWidth.resize(QMAX(parentTreeWidth.size(),(uint)distance));
            int i; for (i=oldSize;i<distance;i++) parentTreeWidth[i]=0;
          }
          parentTreeWidth[distance-1]+=n->label().length();
        }
        n->markAsVisible();
        maxNodes--;
        // add direct parents
        if (n->m_parents)
        {
          QListIterator<PlantUMLNode> li(*n->m_parents);
          PlantUMLNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            parentQueue.append(dn);
          }
        }
      }
    }
  }
  if (Config_getBool(UML_LOOK)) return FALSE; // UML graph are always top to bottom
  int maxWidth=0;
  int maxHeight=(int)QMAX(childTreeWidth.size(),parentTreeWidth.size());
  uint i;
  for (i=0;i<childTreeWidth.size();i++)
  {
    if (childTreeWidth.at(i)>maxWidth) maxWidth=childTreeWidth.at(i);
  }
  for (i=0;i<parentTreeWidth.size();i++)
  {
    if (parentTreeWidth.at(i)>maxWidth) maxWidth=parentTreeWidth.at(i);
  }
  //printf("max tree width=%d, max tree height=%d\n",maxWidth,maxHeight);
  return maxWidth>80 && maxHeight<12; // used metric to decide to render the tree
                                      // from left to right instead of top to bottom,
                                      // with the idea to render very wide trees in
                                      // left to right order.
}



QCString getTemplateBindings(ClassDef *cd)
{
  QCString result;
  ClassDef *templMaster = cd->templateMaster();
  if (templMaster == NULL)
    return "";
  ArgumentList *argList = templMaster->templateArguments();
  if (argList == NULL)
    return "";
  QListIterator<Argument> ali(*argList);
  ali.toFirst();
  // Need to get the template argument bindings, which are not
  // presently stored in ClassDef other than in the name strings.
  // A couple of options here, it seems:
  // 1) Iterate through the template master's instances to find "cd"
  //    and get the key in that QDict
  // 2) Just get the template instance's name and ignore the template
  //    name at the start.  This seems more economical.
  //
  // It would probably be easier to do all this during the initial
  // processing rather than here.  As it is, we need to process the
  // template argument string to make sure we properly handle other
  // templates used as arguments.
  QCString className(cd->className());
  // Get the outer-most template spec
  int openTemp = className.find('<');
  int closeTemp = className.findRev('>');
  if (openTemp < 0)
    return ""; // ignore if not a template
  QCString argStr(className.mid(openTemp+1, closeTemp-openTemp-1).stripWhiteSpace());

  const char *cstr = argStr.data();
  int templCount = 0, startidx = 0;
  bool comma = false; // insert a comma into the string
  for (unsigned int i = 0; i < argStr.length(); i++)
  {
    if (((cstr[i] == ',') || (i == argStr.length()-1)) && (templCount == 0))
    {
      if (comma)
        result += ", ";
      comma = true;
      result += ali.current()->name;
      result += " -> ";
      result += argStr.mid(startidx, 1+i-startidx);
      startidx = i+1;
      ++ali;
    }
    else if (cstr[i] == '<')
    {
      templCount++;
    }
    else if (cstr[i] == '>')
    {
      templCount--;
    }
  }
  return result;
}

void PlantUMLClassGraph::buildGraph(ClassDef *cd,PlantUMLNode *n,bool base,int distance)
{
  static bool templateRelations = Config_getBool(TEMPLATE_RELATIONS);
  // printf("PlantUMLClassGraph::buildGraph(%s,distance=%d,base=%d)\n",
  //        cd->name().data(),distance,base);
  // dumpTemplateArgs1(cd);
  // dumpTemplateArgs2(cd);

  // ---- Add inheritance relations

  if (m_graphType == PlantUMLNode::Inheritance || m_graphType==PlantUMLNode::Collaboration)
  {
    BaseClassList *bcl = base ? cd->baseClasses() : cd->subClasses();
    if (bcl)
    {
      BaseClassListIterator bcli(*bcl);
      BaseClassDef *bcd;
      for ( ; (bcd=bcli.current()) ; ++bcli )
      {
        ClassDef *templMaster=bcd->classDef->templateMaster();
        // template relations for base classes
        if (templateRelations && base && (templMaster != NULL))
        {
          // At this point we know that bcd is a base class of cd,
          // and is a template instance.
          //dumpTemplateBindings(bcd->classDef);
          //printf("PlantUMLClassGraph::buildGraph calling addClass(%s) %d  isTemplate=%d  instances?%d  master?%d\n", qPrint(templMaster->className()), __LINE__, templMaster->isTemplate(), (templMaster->getTemplateInstances() != NULL), (templMaster->templateMaster() != NULL));
          QCString edgeLabel = "<<bind>>\n<";
          edgeLabel += getTemplateBindings(bcd->classDef);
          edgeLabel += ">";
          PlantUMLEdgeInfo::Relation r = PlantUMLEdgeInfo::Realization;
          if (templMaster->compoundType() == ClassDef::Interface)
            r = PlantUMLEdgeInfo::InterfaceRealization;
          addClass(templMaster,n,r,edgeLabel,0,0,base,distance);
        }
        else
        {
          //printf("PlantUMLClassGraph::buildGraph calling addClass(%s) %d  isTemplate=%d  instances?%d  master?%d\n", qPrint(bcd->classDef->className()), __LINE__, bcd->classDef->isTemplate(), (bcd->classDef->getTemplateInstances() != NULL), (bcd->classDef->templateMaster() != NULL));
          addClass(bcd->classDef,n,PlantUMLEdgeInfo::Generalization,0,
                   bcd->usedName,bcd->templSpecifiers,base,distance); 
        }
      }
    }
  }
  if (m_graphType == PlantUMLNode::Collaboration)
  {
    // ---- Add usage relations
    
    UsesClassDict *dict = 
    base ? cd->usedImplementationClasses() : 
    cd->usedByImplementationClasses()
    ;
    if (dict)
    {
      UsesClassDictIterator ucdi(*dict);
      UsesClassDef *ucd;
      for (;(ucd=ucdi.current());++ucdi)
      {
        QCString label;
        QDictIterator<void> dvi(*ucd->accessors);
        const char *s;
        bool first=TRUE;
        int count=0;
        int maxLabels=10;
        /** @todo Figure out how to get template argument bindings for
         * such members.  This looks to be currently impossible
         * because the accessors are stored by name alone. */
        for (;(s=dvi.currentKey()) && count<maxLabels;++dvi,++count)
        {
          if (first) 
          {
            label=s;
            first=FALSE;
          }
          else
          {
            label+=QCString("\n")+s;
          }
        }
        if (count==maxLabels) label+="\n...";
        //printf("addClass: %s templSpec=%s\n",ucd->classDef->name().data(),ucd->templSpecifiers.data());
        if (templateRelations && (ucd->classDef->templateMaster() != NULL))
        {
          //printf("PlantUMLClassGraph::buildGraph calling addClass(%s) %d  isTemplate=%d  instances?%d  master?%d\n", qPrint(ucd->classDef->templateMaster()->className()), __LINE__, ucd->classDef->isTemplate(), (ucd->classDef->getTemplateInstances() != NULL), (ucd->classDef->templateMaster() != NULL));

          // This is a member relationship where n is the source node
          // in the relation, and ucd->classDef->templateMaster() is
          // the target.  The target is a template class.  To be more
          // specific, this member is an instance of an instatiation
          // of a template class.  If that makes sense.
          // e.g. QList<Definition> m_someThing;
          // vs. 
          //   class DefList : public QList<Definition> {};
          //   DefList m_someThing
          // The latter is handled in the inheritance relations above.

          // SMELL I'm not sure how this relationship SHOULD be
          // documented.  I can't find anything that looks applicable
          // in the OMG reference nor can I find any examples on-line.
          addClass(ucd->classDef->templateMaster(),n,
                   PlantUMLEdgeInfo::Realization,0,0,ucd->templSpecifiers,
                   base,distance,0,label);
        }
        else
        {
          //printf("PlantUMLClassGraph::buildGraph calling addClass(%s) %d  isTemplate=%d  instances?%d  master?%d\n", qPrint(ucd->classDef->className()), __LINE__, ucd->classDef->isTemplate(), (ucd->classDef->getTemplateInstances() != NULL), (ucd->classDef->templateMaster() != NULL));
          /** @todo distinguish between composition ("by value") and
           * aggregation ("by reference/pointer").  I've dug through
           * the existing Doxygen code and found the following:
           * \li member information is stored via
           *     ClassDef::addUsedClass in
           *     ClassDefImpl::usesImplClassDict as a UsesClassDef
           *     object.
           * \li UsesClassDef ONLY stores the member name and protection
           * \li name, protection and class definition are all that
           *     are passed to addUsedClass.
           * \li no language-independent information exists to identify
           *     pointers and references.
           * \li The ClassDef objects do not distinguish between a
           *     pointer, reference or value.
           */
          addClass(ucd->classDef,n,PlantUMLEdgeInfo::Composition,0,0,
                   ucd->templSpecifiers,base,distance,0,label);
        }
      }
    }
  }

}

PlantUMLClassGraph::PlantUMLClassGraph(ClassDef *cd,PlantUMLNode::GraphType t)
{
  //printf("--------------- PlantUMLClassGraph::PlantUMLClassGraph `%s'\n",cd->displayName().data());
  m_graphType = t;
  QCString tmp_url="";
  if (cd->isLinkable() && !cd->isHidden()) 
  {
    tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
    if (!cd->anchor().isEmpty())
    {
      tmp_url+="#"+cd->anchor();
    }
  }
  QCString className = cd->displayName();
  QCString tooltip = cd->briefDescriptionAsTooltip();
  m_startNode = new PlantUMLNode(m_curNodeNumber++,
                            className,
                            tooltip,
                            tmp_url.data(),
                            TRUE,                      // is a root node
                            cd
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<PlantUMLNode>(1009);
  m_usedNodes->insert(className,m_startNode);

  //printf("Root node %s\n",cd->name().data());
  //if (m_recDepth>0) 
  //{
  //printf("PlantUMLClassGraph() calling buildGraph 1\n");
  buildGraph(cd,m_startNode,TRUE,1);
  //printf("PlantUMLClassGraph() calling buildGraph 2\n");
  if (t==PlantUMLNode::Inheritance) buildGraph(cd,m_startNode,FALSE,1);
  //}

  static int maxNodes = Config_getInt(PLANTUML_GRAPH_MAX_NODES);
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (t==PlantUMLNode::Inheritance && m_startNode->m_parents!=0)
  //  directChildNodes+=m_startNode->m_parents->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  //openNodeQueue.append(m_startNode);
  m_lrRank = determineVisibleNodes(m_startNode,maxNodes,t==PlantUMLNode::Inheritance);
  QList<PlantUMLNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue,t==PlantUMLNode::Inheritance);

  m_diskName = cd->getOutputFileBase().copy();
}

bool PlantUMLClassGraph::isTrivial() const
{
  static bool umlLook = Config_getBool(UML_LOOK);
  if (m_graphType==PlantUMLNode::Inheritance)
    return m_startNode->m_children==0 && m_startNode->m_parents==0;
  else
    return !umlLook && m_startNode->m_children==0;
}

bool PlantUMLClassGraph::isTooBig() const
{
  static int maxNodes = Config_getInt(PLANTUML_GRAPH_MAX_NODES);
  int numNodes = 0;
  numNodes+= m_startNode->m_children ? m_startNode->m_children->count() : 0;
  if (m_graphType==PlantUMLNode::Inheritance)
  {
    numNodes+= m_startNode->m_parents ? m_startNode->m_parents->count() : 0;
  }
  return numNodes>=maxNodes;
}

PlantUMLClassGraph::~PlantUMLClassGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

/*! Computes a 16 byte md5 checksum for a given plantUML graph.
 *  The md5 checksum is returned as a 32 character ASCII string.
 */
QCString computeMd5Signature(PlantUMLNode *root,
                   PlantUMLNode::GraphType gt,
                   PlantUMLOutputFormat format,
                   bool renderParents,
                   bool backArrows,
                   const QCString &title,
                   QCString &graphStr
                  )
{
  bool reNumber=TRUE;
    
  //printf("PlantUML computeMd5Signature root=%s  renderParents=%d  have_parents?%d\n", qPrint(root->label()), renderParents, (root->m_parents != NULL));
  QGString buf;
  FTextStream md5stream(&buf);
  writeGraphHeader(md5stream,title);
  if (root->m_classDef->getLanguage() == SrcLangExt_Cpp)
    md5stream << "set namespaceSeparator ::" << endl;
  root->clearWriteFlag();
  root->write(md5stream, 
      gt,
      format,
      gt!=PlantUMLNode::CallGraph && gt!=PlantUMLNode::Dependency,
      TRUE,
      backArrows,
      reNumber);
  if (renderParents && root->m_parents) 
  {
    QListIterator<PlantUMLNode>  dnli(*root->m_parents);
    PlantUMLNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (pn->isVisible()) 
      {
        //printf("write arrow 3 %s%s%s\n",qPrint(root->label()),backArrows?"<-":"->",qPrint(pn->label()));
        root->writeArrow(md5stream,                              // stream
            pn,                                                  // child node
            pn->m_edgeInfo->at(pn->m_children->findRef(root))    // edge info
            );
      }
      pn->write(md5stream,      // stream
                gt,             // graph type
                format,         // output format
                TRUE,           // topDown?
                FALSE,          // toChildren?
                backArrows,     // backward pointing arrows?
                reNumber        // renumber nodes?
               );
    }
  }
  writeGraphFooter(md5stream);
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)buf.data(),buf.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.rawData(),33);
  if (reNumber)
  {
    resetReNumbering();
  }
  graphStr=buf.data();
  //printf("PlantUML computeMd5Signature graphStr=%s\n", qPrint(graphStr));
  //printf("md5: %s | file: %s\n",sigStr,baseName.data());
  return sigStr;
}

static bool updatePlantUMLGraph(PlantUMLNode *root,
                           PlantUMLNode::GraphType gt,
                           const QCString &baseName,
                           PlantUMLOutputFormat format,
                           bool renderParents,
                           bool backArrows,
                           const QCString &title=QCString()
                          )
{
  QCString theGraph;
  /// @todo write graph to theGraph, then compute md5 checksum
  QCString md5 = computeMd5Signature(
                   root,gt,format,renderParents,
                   backArrows,title,theGraph);
  QFile f(baseName+".pu");
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    t << theGraph;
  }
  return checkAndUpdateMd5Signature(baseName,md5); // graph needs to be regenerated
}

QCString PlantUMLClassGraph::diskName() const
{
  QCString result=m_diskName.copy();
  switch (m_graphType)
  {
    case PlantUMLNode::Collaboration:
      result+="_coll_puml";
      break;
    //case Interface:
    //  result+="_intf_puml";
    //  break;
    case PlantUMLNode::Inheritance:
      result+="_inherit_puml";
      break;
    default:
      ASSERT(0);
      break;
  }
  return result;
}

QCString PlantUMLClassGraph::writeGraph(FTextStream &out,
                               PlantUMLOutputFormat graphFormat,
                               EmbeddedOutputFormat textFormat,
                               const char *path,
                               const char *fileName,
                               const char *relPath,
                               bool /*isTBRank*/,
                               bool generateImageMap,
                               int graphId) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }
  static bool usePDFLatex = Config_getBool(USE_PDFLATEX);

  QCString baseName;
  QCString mapName;
  switch (m_graphType)
  {
    case PlantUMLNode::Collaboration:
      mapName="coll_map";
      break;
    //case Interface:
    //  mapName="intf_map";
    //  break;
    case PlantUMLNode::Inheritance:
      mapName="inherit_map";
      break;
    default:
      ASSERT(0);
      break;
  }
  baseName = convertNameToFile(diskName());

  // derive target file names from baseName
  QCString imgExt = Config_getEnum(PLANTUML_IMAGE_FORMAT);
  QCString absBaseName = d.absPath().utf8()+"/"+baseName;
  QCString absPlantUMLName  = absBaseName+".pu";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  QCString absImgName  = absBaseName+"."+imgExt;

/*
  Debug::print(
      Debug::Classes,0,
      "PlantUMLClassGraph::writeGraph(out,%d,%d,%s,%s,%s,?,%d,%d) : %s\n",
      graphFormat, textFormat, path, fileName, relPath, generateImageMap,
      graphId, absPlantUMLName.data());
  Debug::print(Debug::Classes,0, "PlantUMLClassGraph::writeGraph type=%d\n",
               m_graphType);
*/

  bool regenerate = FALSE;
  if (updatePlantUMLGraph(m_startNode,
                 m_graphType,
                 absBaseName,
                 graphFormat,
                 m_graphType==PlantUMLNode::Inheritance,
                 TRUE,
                 m_startNode->label()
                ) ||
      !checkDeliverables(graphFormat==PUML_BITMAP ? absImgName : 
                         usePDFLatex    ? absPdfName : absEpsName)
     )
  {
    regenerate=TRUE;
    if (graphFormat==PUML_BITMAP) // run plantUML to create a bitmap image
    {
      QCString plantUMLArgs(MAX_CMD_LEN);

      // PlantUMLRunner *plantUMLRun = new PlantUMLRunner(absPlantUMLName,
      //                         d.absPath().data(),TRUE,absImgName);
      // plantUMLRun->addJob(imgExt,absImgName);
      PlantUMLManager::instance()->addRun(imgExt,absPlantUMLName);

    }
    else if (graphFormat==PUML_EPS) // run plantUML to create a .eps image
    {
      //PlantUMLRunner *plantUMLRun = new PlantUMLRunner(absPlantUMLName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        //plantUMLRun->addJob("pdf",absPdfName);
        PlantUMLManager::instance()->addRun("pdf",absPlantUMLName);
      }
      else
      {
        //plantUMLRun->addJob("ps",absEpsName);
        PlantUMLManager::instance()->addRun("ps",absPlantUMLName);
      }
      //PlantUMLManager::instance()->addRun(plantUMLRun);
    }
  }
  Doxygen::indexList->addImageFile(baseName+"."+imgExt);

  if (graphFormat==PUML_BITMAP && textFormat==EOF_DocBook)
  {
    out << "<para>" << endl;
    out << "    <figure>" << endl;
    out << "        <title>";
    switch (m_graphType)
    {
      case PlantUMLNode::Collaboration:
        out << "Collaboration graph";
        break;
      case PlantUMLNode::Inheritance:
        out << "Inheritance graph";
        break;
      default:
        ASSERT(0);
        break;
    }
    out << "</title>" << endl;
    out << "        <mediaobject>" << endl;
    out << "            <imageobject>" << endl;
    out << "                <imagedata";
    out << " width=\"50%\" align=\"center\" valign=\"middle\" scalefit=\"1\" fileref=\"" << relPath << baseName << "." << imgExt << "\">";
    out << "</imagedata>" << endl;
    out << "            </imageobject>" << endl;
    out << "        </mediaobject>" << endl;
    out << "    </figure>" << endl;
    out << "</para>" << endl;
  }
  else if (graphFormat==PUML_BITMAP && generateImageMap) // produce HTML to include the image
  {
    QCString mapLabel = escapeCharsInString(m_startNode->m_label,FALSE)+"_"+
                        escapeCharsInString(mapName,FALSE);
    if (imgExt=="svg") // add link to SVG file without map file
    {
      out << "<div class=\"center\">";
      if (regenerate || !writeSVGFigureLink(out,relPath,baseName,absImgName)) // need to patch the links in the generated SVG file
      {
        if (regenerate)
        {
          PlantUMLManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = PlantUMLManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
        out << "<!-- SVG " << mapId << " -->" << endl;
      }
      out << "</div>" << endl;
    }
    else // add link to bitmap file with image map
    {
      out << "<div class=\"center\">";
      out << "<img src=\"" << relPath << baseName << "." 
        << imgExt << "\" border=\"0\" usemap=\"#" 
        << mapLabel << "\" alt=\"";
      switch (m_graphType)
      {
        case PlantUMLNode::Collaboration:
          out << "Collaboration graph";
          break;
        case PlantUMLNode::Inheritance:
          out << "Inheritance graph";
          break;
        default:
          ASSERT(0);
          break;
      }
      out << "\"/>";
      out << "</div>" << endl;
    }
  }
  else if (graphFormat==PUML_EPS) // produce tex to include the .eps image
  {
    if (regenerate || !writeVecGfxFigure(out,baseName,absBaseName))
    {
      int figId = PlantUMLManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE /*TRUE*/);
      out << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removePlantUMLGraph(absPlantUMLName);

  return baseName;
}

//--------------------------------------------------------------------

QCString PlantUMLEdgeInfo::asString(Relation r)
{
  switch (r)
  {
    case Aggregation:          return "aggregation";
    case Association:          return "association";
    case Composition:          return "composition";
    case Dependency:           return "dependency";
    case Generalization:       return "generalization";
    case InterfaceRealization: return "interface-realization";
    case Realization:          return "realization";
    case Usage:                return "usage";
    case PackageMerge:         return "package-merge";
    case PackageImportPublic:  return "package-import-public";
    case PackageImportPrivate: return "package-import-private";
    default:                   return "unknown";
  }
}


QCString PlantUMLEdgeInfo::getArrow() const
{
  // The comments below indicate typical usage of relation types per
  // OMG UML superstructure formal specification 2.4.1 section 7.4.
  // C = class diagrams
  // P = package diagrams
  // O = object diagrams
  // Labels on relations are indicated to the right.
  switch (m_edgeType)
  {
    case Aggregation:          return "o--";  // C
    case Association:          return "<--";  // C O
    case Composition:          return "*--";  // C
    case Dependency:           return "<..";  // CP
    case Generalization:       return "<|--"; // C
    case InterfaceRealization: return "<|.."; // C
    case Realization:          return "<|.."; // C
    case Usage:                return "<..";  //    <<use>>
    case PackageMerge:         return "<..";  //  P <<merge>>
    case PackageImportPublic:  return "<..";  //  P <<import>>
    case PackageImportPrivate: return "<..";  //  P <<access>>
    default:                   return "--";
  }
};

//--------------------------------------------------------------------

void PlantUMLClassGraph::writeXML(FTextStream &t)
{
  QDictIterator<PlantUMLNode> dni(*m_usedNodes);
  PlantUMLNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,TRUE);
  }
}

void PlantUMLClassGraph::writeDocbook(FTextStream &t)
{
  QDictIterator<PlantUMLNode> dni(*m_usedNodes);
  PlantUMLNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDocbook(t,TRUE);
  }
}

void PlantUMLClassGraph::writeDEF(FTextStream &t)
{
  QDictIterator<PlantUMLNode> dni(*m_usedNodes);
  PlantUMLNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDEF(t);
  }
}

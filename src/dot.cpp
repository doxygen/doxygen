/*****************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifdef _WIN32
#include <windows.h>
#define BITMAP W_BITMAP
#endif

#include <stdlib.h>

#include <qdir.h>
#include <qfile.h>
#include <qqueue.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

#include "dot.h"
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

#define MAP_CMD "cmapx"

//#define FONTNAME "Helvetica"
#define FONTNAME getDotFontName()
#define FONTSIZE getDotFontSize()

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

static const int maxCmdLine = 40960;

/*! mapping from protection levels to color names */
static const char *normalEdgeColorMap[] =
{
  "midnightblue",  // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "darkorchid3",   // "use" relation
  "grey75",        // Undocumented
  "orange"         // template relation
};

static const char *normalArrowStyleMap[] =
{
  "empty",         // Public
  "empty",         // Protected
  "empty",         // Private
  "open",          // "use" relation
  0,               // Undocumented
  0                // template relation
};

static const char *normalEdgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

static const char *umlEdgeColorMap[] =
{
  "midnightblue",  // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "grey25",        // "use" relation
  "grey75",        // Undocumented
  "orange"         // template relation
};

static const char *umlArrowStyleMap[] =
{
  "onormal",         // Public
  "onormal",         // Protected
  "onormal",         // Private
  "odiamond",        // "use" relation
  0,                 // Undocumented
  0                  // template relation
};

static const char *umlEdgeStyleMap[] =
{
  "solid",         // inheritance
  "solid"          // usage
};

/** Helper struct holding the properties of a edge in a dot graph. */
struct EdgeProperties
{
  const char * const *edgeColorMap;
  const char * const *arrowStyleMap;
  const char * const *edgeStyleMap;
};

static EdgeProperties normalEdgeProps = 
{
  normalEdgeColorMap, normalArrowStyleMap, normalEdgeStyleMap
};

static EdgeProperties umlEdgeProps =
{
  umlEdgeColorMap, umlArrowStyleMap, umlEdgeStyleMap
};


static QCString getDotFontName()
{
  static QCString dotFontName = Config_getString("DOT_FONTNAME");
  if (dotFontName.isEmpty()) 
  {
    //dotFontName="FreeSans.ttf";
    dotFontName="Helvetica";
  }
  return dotFontName;
}

static int getDotFontSize()
{
  static int dotFontSize = Config_getInt("DOT_FONTSIZE");
  if (dotFontSize<4) dotFontSize=4;
  return dotFontSize;
}

static void writeGraphHeader(FTextStream &t,const QCString &title=QCString())
{
  static bool interactiveSVG = Config_getBool("INTERACTIVE_SVG");
  t << "digraph ";
  if (title.isEmpty())
  {
    t << "\"Dot Graph\"";
  }
  else
  {
    t << "\"" << convertToXML(title) << "\"";
  }
  t << endl << "{" << endl;
  if (interactiveSVG) // insert a comment to force regeneration when this
                      // option is toggled
  {
    t << " // INTERACTIVE_SVG=YES\n";
  }
  if (Config_getBool("DOT_TRANSPARENT"))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\","
       "labelfontname=\"" << FONTNAME << "\","
       "labelfontsize=\"" << FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << FONTNAME << "\","
       "fontsize=\"" << FONTSIZE << "\",shape=record];\n";
}

static void writeGraphFooter(FTextStream &t)
{
  t << "}" << endl;
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
    if (urlOnly) // for user defined dot graphs
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

/*! converts the rectangles in a client site image map into a stream
 *  \param t the stream to which the result is written.
 *  \param mapName the name of the map file.
 *  \param relPath the relative path to the root of the output directory
 *                 (used in case CREATE_SUBDIRS is enabled).
 *  \param urlOnly if FALSE the url field in the map contains an external
 *                 references followed by a $ and then the URL.
 *  \param context the context (file, class, or namespace) in which the
 *                 map file was found
 *  \returns TRUE if successful.
 */
static bool convertMapFile(FTextStream &t,const char *mapName,
                           const QCString relPath, bool urlOnly=FALSE,
                           const QCString &context=QCString())
{
  QFile f(mapName);
  if (!f.open(IO_ReadOnly)) 
  {
    err("problems opening map file %s for inclusion in the docs!\n"
        "If you installed Graphviz/dot after a previous failing run, \n"
        "try deleting the output directory and rerun doxygen.\n",mapName);
    return FALSE;
  }
  const int maxLineLen=10240;
  while (!f.atEnd()) // foreach line
  {
    QCString buf(maxLineLen);
    int numBytes = f.readLine(buf.data(),maxLineLen);
    buf[numBytes-1]='\0';

    if (buf.left(5)=="<area")
    {
      t << replaceRef(buf,relPath,urlOnly,context);
    }
  }
  return TRUE;
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

static QCString g_dotFontPath;

static void setDotFontPath(const char *path)
{
  ASSERT(g_dotFontPath.isEmpty());
  g_dotFontPath = portable_getenv("DOTFONTPATH");
  QCString newFontPath = Config_getString("DOT_FONTPATH");
  QCString spath = path;
  if (!newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath.prepend(spath+portable_pathListSeparator());
  }
  else if (newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath=path;
  }
  else
  {
    portable_unsetenv("DOTFONTPATH");
    return;
  }
  portable_setenv("DOTFONTPATH",newFontPath);
}

static void unsetDotFontPath()
{
  if (g_dotFontPath.isEmpty())
  {
    portable_unsetenv("DOTFONTPATH");
  }
  else
  {
    portable_setenv("DOTFONTPATH",g_dotFontPath);
  }
  g_dotFontPath="";
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
  static bool usePdfLatex = Config_getBool("USE_PDFLATEX");
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

// extract size from a dot generated SVG file
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

// since dot silently reproduces the input file when it does not
// support the PNG format, we need to check the result.
static void checkDotResult(const QCString &imgName)
{
  if (Config_getEnum("DOT_IMAGE_FORMAT")=="png")
  {
    FILE *f = portable_fopen(imgName,"rb");
    if (f)
    {
      char data[4];
      if (fread(data,1,4,f)==4)
      {
        if (!(data[1]=='P' && data[2]=='N' && data[3]=='G'))
        {
          err("Image `%s' produced by dot is not a valid PNG!\n"
              "You should either select a different format "
              "(DOT_IMAGE_FORMAT in the config file) or install a more "
              "recent version of graphviz (1.7+)\n",imgName.data()
             );
        }
      }
      else
      {
        err("Could not read image `%s' generated by dot!\n",imgName.data());
      }
      fclose(f);
    }
    else
    {
      err("Could not open image `%s' generated by dot!\n",imgName.data());
    }
  }
}

static bool insertMapFile(FTextStream &out,const QCString &mapFile,
                          const QCString &relPath,const QCString &mapLabel)
{
  QFileInfo fi(mapFile);
  if (fi.exists() && fi.size()>0) // reuse existing map file
  {
    QGString tmpstr;
    FTextStream tmpout(&tmpstr);
    convertMapFile(tmpout,mapFile,relPath);
    if (!tmpstr.isEmpty())
    {
      out << "<map name=\"" << mapLabel << "\" id=\"" << mapLabel << "\">" << endl;
      out << tmpstr;
      out << "</map>" << endl;
    }
    return TRUE;
  }
  return FALSE; // no map file yet, need to generate it
}

static void removeDotGraph(const QCString &dotName)
{
  static bool dotCleanUp = Config_getBool("DOT_CLEANUP"); 
  if (dotCleanUp)
  {
    QDir d;
    d.remove(dotName);
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
    int bytesRead=f.readBlock(md5stored.data(),32);
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

/** Class representing a list of DotNode objects. */
class DotNodeList : public QList<DotNode>
{
  public:
    DotNodeList() : QList<DotNode>() {}
   ~DotNodeList() {}
   int compareItems(QCollection::Item item1,QCollection::Item item2)
   {
     return qstricmp(((DotNode *)item1)->m_label,((DotNode *)item2)->m_label);
   }
};

//--------------------------------------------------------------------

DotRunner::DotRunner(const QCString &file,const QCString &path,
                     bool checkResult,const QCString &imageName) 
  : m_file(file), m_path(path), 
    m_checkResult(checkResult), m_imageName(imageName)
{
  static bool dotCleanUp = Config_getBool("DOT_CLEANUP"); 
  m_cleanUp = dotCleanUp;
  m_jobs.setAutoDelete(TRUE);
}

void DotRunner::addJob(const char *format,const char *output)
{
  QCString args = QCString("-T")+format+" -o \""+output+"\"";
  m_jobs.append(new QCString(args));
}

void DotRunner::addPostProcessing(const char *cmd,const char *args)
{
  m_postCmd = cmd;
  m_postArgs = args;
}

bool DotRunner::run()
{
  int exitCode=0;
  QCString dotExe   = Config_getString("DOT_PATH")+"dot";
  bool multiTargets = Config_getBool("DOT_MULTI_TARGETS");
  QCString dotArgs;
  QListIterator<QCString> li(m_jobs);
  QCString *s;
  QCString file      = m_file;
  QCString path      = m_path;
  QCString imageName = m_imageName;
  QCString postCmd   = m_postCmd;
  QCString postArgs  = m_postArgs;
  bool checkResult   = m_checkResult;
  bool cleanUp       = m_cleanUp;
  if (multiTargets)
  {
    dotArgs="\""+file+"\"";
    for (li.toFirst();(s=li.current());++li)
    {
      dotArgs+=' ';
      dotArgs+=*s;
    }
    if ((exitCode=portable_system(dotExe,dotArgs,FALSE))!=0)
    {
      goto error;
    }
  }
  else
  {
    for (li.toFirst();(s=li.current());++li)
    {
      dotArgs="\""+file+"\" "+*s;
      if ((exitCode=portable_system(dotExe,dotArgs,FALSE))!=0)
      {
        goto error;
      }
    }
  }
  if (!postCmd.isEmpty() && portable_system(postCmd,postArgs)!=0)
  {
    err("Problems running '%s' as a post-processing step for dot output\n",m_postCmd.data());
    return FALSE;
  }
  if (checkResult) checkDotResult(imageName);
  if (cleanUp) 
  {
    //printf("removing dot file %s\n",m_file.data());
    //QDir(path).remove(file);
    m_cleanupItem.file = file;
    m_cleanupItem.path = path;
  }
  return TRUE;
error:
  err("Problems running dot: exit code=%d, command='%s', arguments='%s'\n",
      exitCode,dotExe.data(),dotArgs.data());
  return FALSE;
}

//--------------------------------------------------------------------

DotFilePatcher::DotFilePatcher(const char *patchFile) 
  : m_patchFile(patchFile)
{
  m_maps.setAutoDelete(TRUE);
}

QCString DotFilePatcher::file() const
{
  return m_patchFile;
}

int DotFilePatcher::addMap(const QCString &mapFile,const QCString &relPath,
               bool urlOnly,const QCString &context,const QCString &label)
{
  int id = m_maps.count();
  Map *map = new Map;
  map->mapFile  = mapFile;
  map->relPath  = relPath;
  map->urlOnly  = urlOnly;
  map->context  = context;
  map->label    = label;
  map->zoomable = FALSE;
  map->graphId  = -1;
  m_maps.append(map);
  return id;
}

int DotFilePatcher::addFigure(const QCString &baseName,
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

int DotFilePatcher::addSVGConversion(const QCString &relPath,bool urlOnly,
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

int DotFilePatcher::addSVGObject(const QCString &baseName,
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

bool DotFilePatcher::run()
{
  //printf("DotFilePatcher::run(): %s\n",m_patchFile.data());
  static bool interactiveSVG = Config_getBool("INTERACTIVE_SVG");
  bool isSVGFile = m_patchFile.right(4)==".svg";
  int graphId = -1;
  QCString relPath;
  if (isSVGFile)
  {
    Map *map = m_maps.at(0); // there is only one 'map' for a SVG file
    interactiveSVG = interactiveSVG && map->zoomable;
    graphId = map->graphId;
    relPath = map->relPath;
    //printf("DotFilePatcher::addSVGConversion: file=%s zoomable=%d\n",
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
    int numBytes = fi.readLine(line.data(),maxLineLen);
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
        //printf("DotFilePatcher::writeSVGFigure: file=%s zoomable=%d\n",
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
    else if ((i=line.find("<!-- MAP"))!=-1)
    {
      int mapId=-1;
      t << line.left(i);
      int n = sscanf(line.data()+i,"<!-- MAP %d",&mapId);
      if (n==1 && mapId>=0 && mapId<(int)m_maps.count())
      {
        Map *map = m_maps.at(mapId);
        //printf("patching MAP %d in file %s with contents of %s\n",
        //   mapId,m_patchFile.data(),map->mapFile.data());
        t << "<map name=\"" << map->label << "\" id=\"" << map->label << "\">" << endl;
        convertMapFile(t,map->mapFile,map->relPath,map->urlOnly,map->context);
        t << "</map>" << endl;
      }
      else // error invalid map id!
      {
        err("Found invalid MAP id in file %s!\n",m_patchFile.data());
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
        err("Found invalid bounding FIG id in file %s!\n",mapId,m_patchFile.data());
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
      int numBytes = fi.readLine(line.data(),maxLineLen);
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

void DotRunnerQueue::enqueue(DotRunner *runner)
{
  QMutexLocker locker(&m_mutex);
  m_queue.enqueue(runner);
  m_bufferNotEmpty.wakeAll();
}

DotRunner *DotRunnerQueue::dequeue()
{
  QMutexLocker locker(&m_mutex);
  while (m_queue.isEmpty())
  {
    // wait until something is added to the queue
    m_bufferNotEmpty.wait(&m_mutex);
  }
  DotRunner *result = m_queue.dequeue();
  return result;
}

uint DotRunnerQueue::count() const
{
  QMutexLocker locker(&m_mutex);
  return m_queue.count();
}

//--------------------------------------------------------------------

DotWorkerThread::DotWorkerThread(int id,DotRunnerQueue *queue)
      : m_id(id), m_queue(queue)
{
  m_cleanupItems.setAutoDelete(TRUE);
}

void DotWorkerThread::run()
{
  DotRunner *runner;
  while ((runner=m_queue->dequeue()))
  {
    runner->run();
    DotRunner::CleanupItem cleanup = runner->cleanup();
    if (!cleanup.file.isEmpty())
    {
      m_cleanupItems.append(new DotRunner::CleanupItem(cleanup));
    }
  }
}

void DotWorkerThread::cleanup()
{
  QListIterator<DotRunner::CleanupItem> it(m_cleanupItems);
  DotRunner::CleanupItem *ci;
  for (;(ci=it.current());++it)
  {
    QDir(ci->path).remove(ci->file);
  }
}

//--------------------------------------------------------------------

DotManager *DotManager::m_theInstance = 0;

DotManager *DotManager::instance()
{
  if (!m_theInstance)
  {
    m_theInstance = new DotManager;
  }
  return m_theInstance;
}

DotManager::DotManager() : m_dotMaps(1007)
{
  m_dotRuns.setAutoDelete(TRUE);
  m_dotMaps.setAutoDelete(TRUE);
  m_queue = new DotRunnerQueue;
  int i;
  int numThreads = QMIN(32,Config_getInt("DOT_NUM_THREADS"));
  if (numThreads!=1)
  {
    if (numThreads==0) numThreads = QMAX(2,QThread::idealThreadCount()+1);
    for (i=0;i<numThreads;i++)
    {
      DotWorkerThread *thread = new DotWorkerThread(i,m_queue);
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

DotManager::~DotManager()
{
  delete m_queue;
}

void DotManager::addRun(DotRunner *run)
{
  m_dotRuns.append(run);
}

int DotManager::addMap(const QCString &file,const QCString &mapFile,
                const QCString &relPath,bool urlOnly,const QCString &context,
                const QCString &label)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addMap(mapFile,relPath,urlOnly,context,label);
}

int DotManager::addFigure(const QCString &file,const QCString &baseName,
                          const QCString &figureName,bool heightCheck)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addFigure(baseName,figureName,heightCheck);
}

int DotManager::addSVGConversion(const QCString &file,const QCString &relPath,
                       bool urlOnly,const QCString &context,bool zoomable,
                       int graphId)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addSVGConversion(relPath,urlOnly,context,zoomable,graphId);
}

int DotManager::addSVGObject(const QCString &file,const QCString &baseName,
                             const QCString &absImgName,const QCString &relPath)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addSVGObject(baseName,absImgName,relPath);
}

bool DotManager::run()
{
  uint numDotRuns = m_dotRuns.count();
  uint numDotMaps = m_dotMaps.count();
  if (numDotRuns+numDotMaps>1)
  {
    if (m_workers.count()==0)
    {
      msg("Generating dot graphs in single threaded mode...\n");
    }
    else
    {
      msg("Generating dot graphs using %d parallel threads...\n",QMIN(numDotRuns+numDotMaps,m_workers.count()));
    }
  }
  int i=1;
  QListIterator<DotRunner> li(m_dotRuns);

  bool setPath=FALSE;
  if (Config_getBool("GENERATE_HTML"))
  {
    setDotFontPath(Config_getString("HTML_OUTPUT"));
    setPath=TRUE;
  }
  else if (Config_getBool("GENERATE_LATEX"))
  {
    setDotFontPath(Config_getString("LATEX_OUTPUT"));
    setPath=TRUE;
  }
  else if (Config_getBool("GENERATE_RTF"))
  {
    setDotFontPath(Config_getString("RTF_OUTPUT"));
    setPath=TRUE;
  }
  portable_sysTimerStart();
  // fill work queue with dot operations
  DotRunner *dr;
  int prev=1;
  if (m_workers.count()==0) // no threads to work with
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
      dr->run();
      prev++;
    }
  }
  else // use multiple threads to run instances of dot in parallel
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      m_queue->enqueue(dr);
    }
    // wait for the queue to become empty
    while ((i=m_queue->count())>0)
    {
      i = numDotRuns - i;
      while (i>=prev)
      {
        msg("Running dot for graph %d/%d\n",prev,numDotRuns);
        prev++;
      }
      portable_sleep(100);
    }
    while ((int)numDotRuns>=prev)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
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
    // clean up dot files from main thread
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->cleanup();
    }
  }
  portable_sysTimerStop();
  if (setPath)
  {
    unsetDotFontPath();
  }

  // patch the output file and insert the maps and figures
  i=1;
  SDict<DotFilePatcher>::Iterator di(m_dotMaps);
  DotFilePatcher *map;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and 
  // then the other files. 
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)==".svg")
    {
      msg("Patching output file %d/%d\n",i,numDotMaps);
      if (!map->run()) return FALSE;
      i++;
    }
  }
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)!=".svg")
    {
      msg("Patching output file %d/%d\n",i,numDotMaps);
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
static void deleteNodes(DotNode *node,SDict<DotNode> *skipNodes=0)
{
  //printf("deleteNodes skipNodes=%p\n",skipNodes);
  static DotNodeList deletedNodes;
  deletedNodes.setAutoDelete(TRUE);
  node->deleteNode(deletedNodes,skipNodes); // collect nodes to be deleted.
  deletedNodes.clear(); // actually remove the nodes.
}

DotNode::DotNode(int n,const char *lab,const char *tip, const char *url,
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

DotNode::~DotNode()
{
  delete m_children;
  delete m_parents;
  delete m_edgeInfo;
}

void DotNode::addChild(DotNode *n,
                       int edgeColor,
                       int edgeStyle,
                       const char *edgeLab,
                       const char *edgeURL,
                       int edgeLabCol
                      )
{
  if (m_children==0)
  {
    m_children = new QList<DotNode>;
    m_edgeInfo = new QList<EdgeInfo>;
    m_edgeInfo->setAutoDelete(TRUE);
  }
  m_children->append(n);
  EdgeInfo *ei = new EdgeInfo;
  ei->m_color = edgeColor;
  ei->m_style = edgeStyle; 
  ei->m_label = edgeLab;
  ei->m_url   = edgeURL;
  if (edgeLabCol==-1)
    ei->m_labColor=edgeColor;
  else
    ei->m_labColor=edgeLabCol;
  m_edgeInfo->append(ei);
}

void DotNode::addParent(DotNode *n)
{
  if (m_parents==0)
  {
    m_parents = new QList<DotNode>;
  }
  m_parents->append(n);
}

void DotNode::removeChild(DotNode *n)
{
  if (m_children) m_children->remove(n);
}

void DotNode::removeParent(DotNode *n)
{
  if (m_parents) m_parents->remove(n);
}

void DotNode::deleteNode(DotNodeList &deletedList,SDict<DotNode> *skipNodes)
{
  if (m_deleted) return; // avoid recursive loops in case the graph has cycles
  m_deleted=TRUE;
  if (m_parents!=0) // delete all parent nodes of this node
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      //pn->removeChild(this);
      pn->deleteNode(deletedList,skipNodes);
    }
  }
  if (m_children!=0) // delete all child nodes of this node
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
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

void DotNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance = distance;
}

static QCString convertLabel(const QCString &l)
{
  QCString result;
  QCString bBefore("\\_/<({[: =-+@%#~?$"); // break before character set
  QCString bAfter(">]),:;|");              // break after  character set
  const char *p=l.data();
  if (p==0) return result;
  char c,pc=0;
  char cs[2];
  cs[1]=0;
  int len=l.length();
  int charsLeft=len;
  int sinceLast=0;
  int foldLen=17; // ideal text length
  while ((c=*p++))
  {
    QCString replacement;
    switch(c)
    {
      case '\\': replacement="\\\\"; break;
      case '\n': replacement="\\n"; break;
      case '<':  replacement="\\<"; break;
      case '>':  replacement="\\>"; break;
      case '|':  replacement="\\|"; break;
      case '{':  replacement="\\{"; break;
      case '}':  replacement="\\}"; break;
      case '"':  replacement="\\\""; break;
      default:   cs[0]=c; replacement=cs; break;
    }
    // Some heuristics to insert newlines to prevent too long
    // boxes and at the same time prevent ugly breaks
    if (c=='\n')
    {
      result+=replacement;
      foldLen = (3*foldLen+sinceLast+2)/4;
      sinceLast=1;
    }
    else if ((pc!=':' || c!=':') && charsLeft>foldLen/3 && sinceLast>foldLen && bBefore.contains(c))
    {
      result+="\\l";
      result+=replacement;
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=1;
    }
    else if (charsLeft>1+foldLen/4 && sinceLast>foldLen+foldLen/3 && 
            !isupper(c) && isupper(*p))
    {
      result+=replacement;
      result+="\\l";
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=0;
    }
    else if (charsLeft>foldLen/3 && sinceLast>foldLen && bAfter.contains(c) && (c!=':' || *p!=':'))
    {
      result+=replacement;
      result+="\\l";
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=0;
    }
    else
    {
      result+=replacement;
      sinceLast++;
    }
    charsLeft--;
    pc=c;
  }
  return result;
}

static QCString escapeTooltip(const QCString &tooltip)
{
  QCString result;
  const char *p=tooltip.data();
  if (p==0) return result;
  char c;
  while ((c=*p++))
  {
    switch(c)
    {
      case '"': result+="\\\""; break;
      default: result+=c; break;
    }
  }
  return result;
}

static void writeBoxMemberList(FTextStream &t,
            char prot,MemberList *ml,ClassDef *scope,
            bool isStatic=FALSE,const QDict<void> *skipNames=0)
{
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
        static int limit = Config_getInt("UML_LIMIT_NUM_FIELDS");
        if (limit>0 && (totalCount>limit*3/2 && count>=limit))
        {
          t << theTranslator->trAndMore(QCString().sprintf("%d",totalCount-count)) << "\\l";
          break;
        }
        else
        {
          t << prot << " ";
          t << convertLabel(mma->name());
          if (!mma->isObjCMethod() && 
              (mma->isFunction() || mma->isSlot() || mma->isSignal())) t << "()";
          t << "\\l";
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

void DotNode::writeBox(FTextStream &t,
                       GraphType gt,
                       GraphOutputFormat /*format*/,
                       bool hasNonReachableChildren,
                       bool reNumber)
{
  const char *labCol = 
          m_url.isEmpty() ? "grey75" :  // non link
           (
            (hasNonReachableChildren) ? "red" : "black"
           );
  t << "  Node" << reNumberNode(m_number,reNumber) << " [label=\"";
  static bool umlLook = Config_getBool("UML_LOOK");

  if (m_classDef && umlLook && (gt==Inheritance || gt==Collaboration))
  {
    // add names shown as relations to a dictionary, so we don't show
    // them as attributes as well
    QDict<void> arrowNames(17);
    if (m_edgeInfo)
    {
      // for each edge
      QListIterator<EdgeInfo> li(*m_edgeInfo);
      EdgeInfo *ei;
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

    //printf("DotNode::writeBox for %s\n",m_classDef->name().data());
    static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");
    t << "{" << convertLabel(m_label);
    t << "\\n|";
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticAttribs),m_classDef,TRUE,&arrowNames);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_properties),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticAttribs),m_classDef,TRUE,&arrowNames);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proAttribs),m_classDef,FALSE,&arrowNames);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticAttribs),m_classDef,TRUE,&arrowNames);
    if (extractPrivate)
    {
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priAttribs),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticAttribs),m_classDef,TRUE,&arrowNames);
    }
    t << "|";
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubMethods),m_classDef);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubSlots),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacMethods),m_classDef);
    writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proMethods),m_classDef);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticMethods),m_classDef,TRUE);
    writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proSlots),m_classDef);
    if (extractPrivate)
    {
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priMethods),m_classDef);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticMethods),m_classDef,TRUE);
      writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priSlots),m_classDef);
    }
    if (m_classDef->getLanguage()!=SrcLangExt_Fortran &&
        m_classDef->getMemberGroupSDict())
    {
      MemberGroupSDict::Iterator mgdi(*m_classDef->getMemberGroupSDict());
      MemberGroup *mg;
      for (mgdi.toFirst();(mg=mgdi.current());++mgdi)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,'*',mg->members(),m_classDef,FALSE,&arrowNames);
        }
      }
    }
    t << "}";
  }
  else // standard look
  {
    t << convertLabel(m_label);
  }
  t << "\",height=0.2,width=0.4";
  if (m_isRoot)
  {
    t << ",color=\"black\", fillcolor=\"grey75\", style=\"filled\" fontcolor=\"black\"";
  }
  else 
  {
    static bool dotTransparent = Config_getBool("DOT_TRANSPARENT");
    if (!dotTransparent)
    {
      t << ",color=\"" << labCol << "\", fillcolor=\"";
      t << "white";
      t << "\", style=\"filled\"";
    }
    else
    {
      t << ",color=\"" << labCol << "\"";
    }
    if (!m_url.isEmpty())
    {
      int anchorPos = m_url.findRev('#');
      if (anchorPos==-1)
      {
        t << ",URL=\"" << m_url << Doxygen::htmlFileExtension << "\"";
      }
      else
      {
        t << ",URL=\"" << m_url.left(anchorPos) << Doxygen::htmlFileExtension
          << m_url.right(m_url.length()-anchorPos) << "\"";
      }
    }
    if (!m_tooltip.isEmpty())
    {
      t << ",tooltip=\"" << escapeTooltip(m_tooltip) << "\"";
    }
  }
  t << "];" << endl; 
}

void DotNode::writeArrow(FTextStream &t,
                         GraphType gt,
                         GraphOutputFormat format,
                         DotNode *cn,
                         EdgeInfo *ei,
                         bool topDown, 
                         bool pointBack,
                         bool reNumber
                        )
{
  t << "  Node";
  if (topDown) 
    t << reNumberNode(cn->number(),reNumber); 
  else 
    t << reNumberNode(m_number,reNumber);
  t << " -> Node";
  if (topDown) 
    t << reNumberNode(m_number,reNumber); 
  else 
    t << reNumberNode(cn->number(),reNumber);
  t << " [";

  static bool umlLook = Config_getBool("UML_LOOK");
  const EdgeProperties *eProps = umlLook ? &umlEdgeProps : &normalEdgeProps;
  QCString aStyle = eProps->arrowStyleMap[ei->m_color];
  bool umlUseArrow = aStyle=="odiamond";

  if (pointBack && !umlUseArrow) t << "dir=\"back\",";
  t << "color=\"" << eProps->edgeColorMap[ei->m_color] 
    << "\",fontsize=\"" << FONTSIZE << "\",";
  t << "style=\"" << eProps->edgeStyleMap[ei->m_style] << "\"";
  if (!ei->m_label.isEmpty())
  {
    t << ",label=\" " << convertLabel(ei->m_label) << "\" ";
  }
  if (umlLook &&
      eProps->arrowStyleMap[ei->m_color] && 
      (gt==Inheritance || gt==Collaboration)
     )
  {
    bool rev = pointBack;
    if (umlUseArrow) rev=!rev; // UML use relates has arrow on the start side
    if (rev) 
      t << ",arrowtail=\"" << eProps->arrowStyleMap[ei->m_color] << "\""; 
    else 
      t << ",arrowhead=\"" << eProps->arrowStyleMap[ei->m_color] << "\"";
  }

  if (format==BITMAP) t << ",fontname=\"" << FONTNAME << "\"";
  t << "];" << endl; 
}

void DotNode::write(FTextStream &t,
                    GraphType gt,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows,
                    bool reNumber
                   )
{
  //printf("DotNode::write(%d) name=%s this=%p written=%d\n",distance,m_label.data(),this,m_written);
  if (m_written) return; // node already written to the output
  if (!m_visible) return; // node is not visible
  writeBox(t,gt,format,m_truncated==Truncated,reNumber);
  m_written=TRUE;
  QList<DotNode> *nl = toChildren ? m_children : m_parents; 
  if (nl)
  {
    if (toChildren)
    {
      QListIterator<DotNode>  dnli1(*nl);
      QListIterator<EdgeInfo> dnli2(*m_edgeInfo);
      DotNode *cn;
      for (dnli1.toFirst();(cn=dnli1.current());++dnli1,++dnli2)
      {
        if (cn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",cn->label().data());
          writeArrow(t,gt,format,cn,dnli2.current(),topDown,backArrows,reNumber);
        }
        cn->write(t,gt,format,topDown,toChildren,backArrows,reNumber);
      }
    }
    else // render parents
    {
      QListIterator<DotNode> dnli(*nl);
      DotNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",pn->label().data());
          writeArrow(t,
              gt,
              format,
              pn,
              pn->m_edgeInfo->at(pn->m_children->findRef(this)),
              FALSE,
              backArrows,
              reNumber
              );
        }
        pn->write(t,gt,format,TRUE,FALSE,backArrows,reNumber);
      }
    }
  }
  //printf("end DotNode::write(%d) name=%s\n",distance,m_label.data());
}

void DotNode::writeXML(FTextStream &t,bool isClassGraph)
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    char *refPtr = url.data();
    char *urlPtr = strchr(url.data(),'$');
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
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->m_color)
        {
          case EdgeInfo::Blue:    t << "public-inheritance"; break;
          case EdgeInfo::Green:   t << "protected-inheritance"; break;
          case EdgeInfo::Red:     t << "private-inheritance"; break;
          case EdgeInfo::Purple:  t << "usage"; break;
          case EdgeInfo::Orange:  t << "template-instance"; break;
          case EdgeInfo::Grey:    ASSERT(0); break;
        }
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

void DotNode::writeDocbook(FTextStream &t,bool isClassGraph)
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    char *refPtr = url.data();
    char *urlPtr = strchr(url.data(),'$');
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
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->m_color)
        {
          case EdgeInfo::Blue:    t << "public-inheritance"; break;
          case EdgeInfo::Green:   t << "protected-inheritance"; break;
          case EdgeInfo::Red:     t << "private-inheritance"; break;
          case EdgeInfo::Purple:  t << "usage"; break;
          case EdgeInfo::Orange:  t << "template-instance"; break;
          case EdgeInfo::Grey:    ASSERT(0); break;
        }
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


void DotNode::writeDEF(FTextStream &t)
{
  const char* nodePrefix = "        node-";

  t << "      node = {" << endl;
  t << nodePrefix << "id    = " << m_number << ';' << endl;
  t << nodePrefix << "label = '" << m_label << "';" << endl;

  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    char *refPtr = url.data();
    char *urlPtr = strchr(url.data(),'$');
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
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        node-child = {" << endl;
      t << "          child-id = '" << childNode->m_number << "';" << endl;
      t << "          relation = ";

      switch(edgeInfo->m_color)
      {
        case EdgeInfo::Blue:    t << "public-inheritance"; break;
        case EdgeInfo::Green:   t << "protected-inheritance"; break;
        case EdgeInfo::Red:     t << "private-inheritance"; break;
        case EdgeInfo::Purple:  t << "usage"; break;
        case EdgeInfo::Orange:  t << "template-instance"; break;
        case EdgeInfo::Grey:    ASSERT(0); break;
      }
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


void DotNode::clearWriteFlag()
{
  m_written=FALSE;
  if (m_parents!=0)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
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
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->m_written)
      {
        cn->clearWriteFlag();
      }
    }
  }
}

void DotNode::colorConnectedNodes(int curColor)
{ 
  if (m_children)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
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
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
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

const DotNode *DotNode::findDocNode() const
{
  if (!m_url.isEmpty()) return this;
  //printf("findDocNode(): `%s'\n",m_label.data());
  if (m_parents)
  {
    QListIterator<DotNode> dnli(*m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (!pn->m_hasDoc)
      {
        pn->m_hasDoc=TRUE;
        const DotNode *dn = pn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> dnli(*m_children);
    DotNode *cn;
    for (dnli.toFirst();(cn=dnli.current());++dnli)
    {
      if (!cn->m_hasDoc)
      {
        cn->m_hasDoc=TRUE;
        const DotNode *dn = cn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  return 0;
}

//--------------------------------------------------------------------

int DotGfxHierarchyTable::m_curNodeNumber;

void DotGfxHierarchyTable::writeGraph(FTextStream &out,
                      const char *path,const char *fileName) const
{
  //printf("DotGfxHierarchyTable::writeGraph(%s)\n",name);
  //printf("m_rootNodes=%p count=%d\n",m_rootNodes,m_rootNodes->count());
  
  if (m_rootSubgraphs->count()==0) return;

  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }

  // put each connected subgraph of the hierarchy in a row of the HTML output
  out << "<table border=\"0\" cellspacing=\"10\" cellpadding=\"0\">" << endl;

  QListIterator<DotNode> dnli(*m_rootSubgraphs);
  DotNode *n;
  int count=0;
  for (dnli.toFirst();(n=dnli.current());++dnli)
  {
    QCString baseName;
    QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
    baseName.sprintf("inherit_graph_%d",count++);
    //baseName = convertNameToFile(baseName);
    QCString imgName = baseName+"."+ imgExt;
    QCString mapName = baseName+".map";
    QCString absImgName = QCString(d.absPath().data())+"/"+imgName;
    QCString absMapName = QCString(d.absPath().data())+"/"+mapName;
    QCString absBaseName = QCString(d.absPath().data())+"/"+baseName;
    QListIterator<DotNode> dnli2(*m_rootNodes);
    DotNode *node;

    // compute md5 checksum of the graph were are about to generate
    QGString theGraph;
    FTextStream md5stream(&theGraph);
    writeGraphHeader(md5stream,theTranslator->trGraphicalHierarchy());
    md5stream << "  rankdir=\"LR\";" << endl;
    for (dnli2.toFirst();(node=dnli2.current());++dnli2)
    {
      if (node->m_subgraphId==n->m_subgraphId) 
      {
        node->clearWriteFlag();
      }
    }
    for (dnli2.toFirst();(node=dnli2.current());++dnli2)
    {
      if (node->m_subgraphId==n->m_subgraphId) 
      {
        node->write(md5stream,DotNode::Hierarchy,BITMAP,FALSE,TRUE,TRUE,TRUE);
      }
    }
    writeGraphFooter(md5stream);
    resetReNumbering();
    uchar md5_sig[16];
    QCString sigStr(33);
    MD5Buffer((const unsigned char *)theGraph.data(),theGraph.length(),md5_sig);
    MD5SigToString(md5_sig,sigStr.data(),33);
    bool regenerate=FALSE;
    if (checkAndUpdateMd5Signature(absBaseName,sigStr) || 
        !checkDeliverables(absImgName,absMapName))
    {
      regenerate=TRUE;
      // image was new or has changed
      QCString dotName=absBaseName+".dot";
      QFile f(dotName);
      if (!f.open(IO_WriteOnly)) return;
      FTextStream t(&f);
      t << theGraph;
      f.close();
      resetReNumbering();

      DotRunner *dotRun = new DotRunner(dotName,d.absPath().data(),TRUE,absImgName);
      dotRun->addJob(imgExt,absImgName);
      dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);
    }
    else
    {
      removeDotGraph(absBaseName+".dot");
    }
    Doxygen::indexList->addImageFile(imgName);
    // write image and map in a table row
    QCString mapLabel = escapeCharsInString(n->m_label,FALSE);
    out << "<tr><td>";
    if (imgExt=="svg") // vector graphics
    {
      if (regenerate || !writeSVGFigureLink(out,QCString(),baseName,absImgName))
      {
        if (regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName,QCString(),
                                                   FALSE,QCString(),FALSE,0);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,
                                                         absImgName,QCString());
        out << "<!-- SVG " << mapId << " -->" << endl;
      }
    }
    else // normal bitmap
    {
      out << "<img src=\"" << imgName << "\" border=\"0\" alt=\"\" usemap=\"#"
        << mapLabel << "\"/>" << endl;

      if (regenerate || !insertMapFile(out,absMapName,QCString(),mapLabel))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,QCString(),
                                                   FALSE,QCString(),mapLabel);
        out << "<!-- MAP " << mapId << " -->" << endl;
      }
    }

    out << "</td></tr>" << endl;
  }
  out << "</table>" << endl;
}

void DotGfxHierarchyTable::addHierarchy(DotNode *n,ClassDef *cd,bool hideSuper)
{
  //printf("addHierarchy `%s' baseClasses=%d\n",cd->name().data(),cd->baseClasses()->count());
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()) ; ++bcli )
    {
      ClassDef *bClass=bcd->classDef; 
      //printf("  Trying sub class=`%s' usedNodes=%d\n",bClass->name().data(),m_usedNodes->count());
      if (bClass->isVisibleInHierarchy() && hasVisibleRoot(bClass->baseClasses()))
      {
        DotNode *bn;
        //printf("  Node `%s' Found visible class=`%s'\n",n->m_label.data(),
        //                                              bClass->name().data());
        if ((bn=m_usedNodes->find(bClass->name()))) // node already present 
        {
          if (n->m_children==0 || n->m_children->findRef(bn)==-1) // no arrow yet
          {
            n->addChild(bn,bcd->prot);
            bn->addParent(n);
            //printf("  Adding node %s to existing base node %s (c=%d,p=%d)\n",
            //       n->m_label.data(),
            //       bn->m_label.data(),
            //       bn->m_children ? bn->m_children->count() : 0,
            //       bn->m_parents  ? bn->m_parents->count()  : 0
            //     );
          }
          //else
          //{
          //  printf("  Class already has an arrow!\n");
          //}
        }
        else 
        {
          QCString tmp_url="";
          if (bClass->isLinkable() && !bClass->isHidden())
          {
            tmp_url=bClass->getReference()+"$"+bClass->getOutputFileBase();
            if (!bClass->anchor().isEmpty())
            {
              tmp_url+="#"+bClass->anchor();
            }
          }
          QCString tooltip = bClass->briefDescriptionAsTooltip();
          bn = new DotNode(m_curNodeNumber++,
              bClass->displayName(),
              tooltip,
              tmp_url.data()
              );
          n->addChild(bn,bcd->prot);
          bn->addParent(n);
          //printf("  Adding node %s to new base node %s (c=%d,p=%d)\n",
          //   n->m_label.data(),
          //   bn->m_label.data(),
          //   bn->m_children ? bn->m_children->count() : 0,
          //   bn->m_parents  ? bn->m_parents->count()  : 0
          //  );
          //printf("  inserting %s (%p)\n",bClass->name().data(),bn);
          m_usedNodes->insert(bClass->name(),bn); // add node to the used list
        }
        if (!bClass->visited && !hideSuper && bClass->subClasses())
        {
          bool wasVisited=bClass->visited;
          bClass->visited=TRUE;
          addHierarchy(bn,bClass,wasVisited);
        }
      }
    }
  }
  //printf("end addHierarchy\n");
}

void DotGfxHierarchyTable::addClassList(ClassSDict *cl)
{
  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for (cli.toLast();(cd=cli.current());--cli)
  {
    //printf("Trying %s subClasses=%d\n",cd->name().data(),cd->subClasses()->count());
    if (cd->getLanguage()==SrcLangExt_VHDL  && !(VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS)
      continue; 
    if (!hasVisibleRoot(cd->baseClasses()) &&
        cd->isVisibleInHierarchy()
       ) // root node in the forest
    {
      QCString tmp_url="";
      if (cd->isLinkable() && !cd->isHidden()) 
      {
        tmp_url=cd->getReference()+"$"+cd->getOutputFileBase();
        if (!cd->anchor().isEmpty())
        {
          tmp_url+="#"+cd->anchor();
        }
      }
      //printf("Inserting root class %s\n",cd->name().data());
      QCString tooltip = cd->briefDescriptionAsTooltip();
      DotNode *n = new DotNode(m_curNodeNumber++,
          cd->displayName(),
          tooltip,
          tmp_url.data());

      //m_usedNodes->clear();
      m_usedNodes->insert(cd->name(),n);
      m_rootNodes->insert(0,n);   
      if (!cd->visited && cd->subClasses())
      {
        addHierarchy(n,cd,cd->visited);
        cd->visited=TRUE;
      }
    }
  }
}

DotGfxHierarchyTable::DotGfxHierarchyTable()
{
  m_curNodeNumber=0;
  m_rootNodes = new QList<DotNode>;
  m_usedNodes = new QDict<DotNode>(1009); 
  m_usedNodes->setAutoDelete(TRUE);
  m_rootSubgraphs = new DotNodeList;
  
  // build a graph with each class as a node and the inheritance relations
  // as edges
  initClassHierarchy(Doxygen::classSDict);
  initClassHierarchy(Doxygen::hiddenClasses);
  addClassList(Doxygen::classSDict);
  addClassList(Doxygen::hiddenClasses);
  // m_usedNodes now contains all nodes in the graph
 
  // color the graph into a set of independent subgraphs
  bool done=FALSE; 
  int curColor=0;
  QListIterator<DotNode> dnli(*m_rootNodes);
  while (!done) // there are still nodes to color
  {
    DotNode *n;
    done=TRUE; // we are done unless there are still uncolored nodes
    for (dnli.toLast();(n=dnli.current());--dnli)
    {
      if (n->m_subgraphId==-1) // not yet colored
      {
        //printf("Starting at node %s (%p): %d\n",n->m_label.data(),n,curColor);
        done=FALSE; // still uncolored nodes
        n->m_subgraphId=curColor;
        n->markAsVisible();
        n->colorConnectedNodes(curColor);
        curColor++;
        const DotNode *dn=n->findDocNode();
        if (dn!=0) 
          m_rootSubgraphs->inSort(dn);
        else
          m_rootSubgraphs->inSort(n);
      }
    }
  }
  
  //printf("Number of independent subgraphs: %d\n",curColor);
  //QListIterator<DotNode> dnli2(*m_rootSubgraphs);
  //DotNode *n;
  //for (dnli2.toFirst();(n=dnli2.current());++dnli2)
  //{
  //  printf("Node %s color=%d (c=%d,p=%d)\n",
  //      n->m_label.data(),n->m_subgraphId,
  //      n->m_children?n->m_children->count():0,
  //      n->m_parents?n->m_parents->count():0);
  //}
}

DotGfxHierarchyTable::~DotGfxHierarchyTable()
{
  //printf("DotGfxHierarchyTable::~DotGfxHierarchyTable\n");

  //QDictIterator<DotNode> di(*m_usedNodes);
  //DotNode *n;
  //for (;(n=di.current());++di)
  //{
  //  printf("Node %p: %s\n",n,n->label().data());
  //}
  
  delete m_rootNodes;
  delete m_usedNodes;
  delete m_rootSubgraphs;
}

//--------------------------------------------------------------------

int DotClassGraph::m_curNodeNumber = 0;

void DotClassGraph::addClass(ClassDef *cd,DotNode *n,int prot,
    const char *label,const char *usedName,const char *templSpec,bool base,int distance)
{
  if (Config_getBool("HIDE_UNDOC_CLASSES") && !cd->isLinkable()) return;

  int edgeStyle = (label || prot==EdgeInfo::Orange) ? EdgeInfo::Dashed : EdgeInfo::Solid;
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
  //printf("DotClassGraph::addClass(class=`%s',parent=%s,prot=%d,label=%s,dist=%d,usedName=%s,templSpec=%s,base=%d)\n",
  //                                 className.data(),n->m_label.data(),prot,label,distance,usedName,templSpec,base);
  DotNode *bn = m_usedNodes->find(className);
  if (bn) // class already inserted
  {
    if (base)
    {
      n->addChild(bn,prot,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,prot,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    //printf(" add exiting node %s of %s\n",bn->m_label.data(),n->m_label.data());
  }
  else // new class
  {
    QCString displayName=className;
    if (Config_getBool("HIDE_SCOPE_NAMES")) displayName=stripScope(displayName);
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
    bn = new DotNode(m_curNodeNumber++,
        displayName,
        tooltip,
        tmp_url.data(),
        FALSE,        // rootNode
        cd
       );
    if (base)
    {
      n->addChild(bn,prot,edgeStyle,label);
      bn->addParent(n);
    }
    else
    {
      bn->addChild(n,prot,edgeStyle,label);
      n->addParent(bn);
    }
    bn->setDistance(distance);
    m_usedNodes->insert(className,bn);
    //printf(" add new child node `%s' to %s hidden=%d url=%s\n",
    //    className.data(),n->m_label.data(),cd->isHidden(),tmp_url.data());
    
    buildGraph(cd,bn,base,distance+1);
  }
}

void DotClassGraph::determineTruncatedNodes(QList<DotNode> &queue,bool includeParents)
{
  while (queue.count()>0)
  {
    DotNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
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
        QListIterator<DotNode> li(*n->m_parents);
        DotNode *dn;
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

bool DotClassGraph::determineVisibleNodes(DotNode *rootNode,
                                          int maxNodes,bool includeParents)
{
  QList<DotNode> childQueue;
  QList<DotNode> parentQueue;
  QArray<int> childTreeWidth;
  QArray<int> parentTreeWidth;
  childQueue.append(rootNode);
  if (includeParents) parentQueue.append(rootNode);
  bool firstNode=TRUE; // flag to force reprocessing rootNode in the parent loop 
                       // despite being marked visible in the child loop
  while ((childQueue.count()>0 || parentQueue.count()>0) && maxNodes>0)
  {
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    if (childQueue.count()>0)
    {
      DotNode *n = childQueue.take(0);
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
          QListIterator<DotNode> li(*n->m_children);
          DotNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            childQueue.append(dn);
          }
        }
      }
    }
    if (includeParents && parentQueue.count()>0)
    {
      DotNode *n = parentQueue.take(0);
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
          QListIterator<DotNode> li(*n->m_parents);
          DotNode *dn;
          for (li.toFirst();(dn=li.current());++li)
          {
            parentQueue.append(dn);
          }
        }
      }
    }
  }
  if (Config_getBool("UML_LOOK")) return FALSE; // UML graph are always top to bottom
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

void DotClassGraph::buildGraph(ClassDef *cd,DotNode *n,bool base,int distance)
{
  //printf("DocClassGraph::buildGraph(%s,distance=%d,base=%d)\n",
  //    cd->name().data(),distance,base);
  // ---- Add inheritance relations

  if (m_graphType == DotNode::Inheritance || m_graphType==DotNode::Collaboration)
  {
    BaseClassList *bcl = base ? cd->baseClasses() : cd->subClasses();
    if (bcl)
    {
      BaseClassListIterator bcli(*bcl);
      BaseClassDef *bcd;
      for ( ; (bcd=bcli.current()) ; ++bcli )
      {
        //printf("-------- inheritance relation %s->%s templ=`%s'\n",
        //            cd->name().data(),bcd->classDef->name().data(),bcd->templSpecifiers.data());
        addClass(bcd->classDef,n,bcd->prot,0,bcd->usedName,
            bcd->templSpecifiers,base,distance); 
      }
    }
  }
  if (m_graphType == DotNode::Collaboration)
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
        addClass(ucd->classDef,n,EdgeInfo::Purple,label,0,
            ucd->templSpecifiers,base,distance);
      }
    }
  }

  // ---- Add template instantiation relations

  static bool templateRelations = Config_getBool("TEMPLATE_RELATIONS");
  if (templateRelations)
  {
    if (base) // template relations for base classes
    {
      ClassDef *templMaster=cd->templateMaster();
      if (templMaster)
      {
        QDictIterator<ClassDef> cli(*templMaster->getTemplateInstances());
        ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          if (templInstance==cd)
          {
            addClass(templMaster,n,EdgeInfo::Orange,cli.currentKey(),0,
                0,TRUE,distance);
          }
        }
      }
    }
    else // template relations for super classes
    {
      QDict<ClassDef> *templInstances = cd->getTemplateInstances();
      if (templInstances)
      {
        QDictIterator<ClassDef> cli(*templInstances);
        ClassDef *templInstance;
        for (;(templInstance=cli.current());++cli)
        {
          addClass(templInstance,n,EdgeInfo::Orange,cli.currentKey(),0,
              0,FALSE,distance);
        }
      }
    }
  }
}

DotClassGraph::DotClassGraph(ClassDef *cd,DotNode::GraphType t)
{
  //printf("--------------- DotClassGraph::DotClassGraph `%s'\n",cd->displayName().data());
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
  m_startNode = new DotNode(m_curNodeNumber++,
                            className,
                            tooltip,
                            tmp_url.data(),
                            TRUE,                      // is a root node
                            cd
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(className,m_startNode);

  //printf("Root node %s\n",cd->name().data());
  //if (m_recDepth>0) 
  //{
    buildGraph(cd,m_startNode,TRUE,1);
    if (t==DotNode::Inheritance) buildGraph(cd,m_startNode,FALSE,1);
  //}

  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (t==DotNode::Inheritance && m_startNode->m_parents!=0)
  //  directChildNodes+=m_startNode->m_parents->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  //openNodeQueue.append(m_startNode);
  m_lrRank = determineVisibleNodes(m_startNode,maxNodes,t==DotNode::Inheritance);
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue,t==DotNode::Inheritance);

  m_diskName = cd->getFileBase().copy();
}

bool DotClassGraph::isTrivial() const
{
  static bool umlLook = Config_getBool("UML_LOOK");
  if (m_graphType==DotNode::Inheritance)
    return m_startNode->m_children==0 && m_startNode->m_parents==0;
  else
    return !umlLook && m_startNode->m_children==0;
}

bool DotClassGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = 0;
  numNodes+= m_startNode->m_children ? m_startNode->m_children->count() : 0;
  if (m_graphType==DotNode::Inheritance)
  {
    numNodes+= m_startNode->m_parents ? m_startNode->m_parents->count() : 0;
  }
  return numNodes>=maxNodes;
}

DotClassGraph::~DotClassGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

/*! Computes a 16 byte md5 checksum for a given dot graph.
 *  The md5 checksum is returned as a 32 character ASCII string.
 */
QCString computeMd5Signature(DotNode *root,
                   DotNode::GraphType gt,
                   GraphOutputFormat format,
                   bool lrRank,
                   bool renderParents,
                   bool backArrows,
                   const QCString &title,
                   QCString &graphStr
                  )
{
  bool reNumber=TRUE;
    
  //printf("computeMd5Signature\n");
  QGString buf;
  FTextStream md5stream(&buf);
  writeGraphHeader(md5stream,title);
  if (lrRank)
  {
    md5stream << "  rankdir=\"LR\";" << endl;
  }
  root->clearWriteFlag();
  root->write(md5stream, 
      gt,
      format,
      gt!=DotNode::CallGraph && gt!=DotNode::Dependency,
      TRUE,
      backArrows,
      reNumber);
  if (renderParents && root->m_parents) 
  {
    QListIterator<DotNode>  dnli(*root->m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (pn->isVisible()) 
      {
        root->writeArrow(md5stream,                              // stream
            gt,                                                  // graph type
            format,                                              // output format
            pn,                                                  // child node
            pn->m_edgeInfo->at(pn->m_children->findRef(root)),   // edge info
            FALSE,                                               // topDown?
            backArrows,                                          // point back?
            reNumber                                             // renumber nodes
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
  MD5SigToString(md5_sig,sigStr.data(),33);
  if (reNumber)
  {
    resetReNumbering();
  }
  graphStr=buf.data();
  //printf("md5: %s | file: %s\n",sigStr,baseName.data());
  return sigStr;
}

static bool updateDotGraph(DotNode *root,
                           DotNode::GraphType gt,
                           const QCString &baseName,
                           GraphOutputFormat format,
                           bool lrRank,
                           bool renderParents,
                           bool backArrows,
                           const QCString &title=QCString()
                          )
{
  QCString theGraph;
  // TODO: write graph to theGraph, then compute md5 checksum
  QCString md5 = computeMd5Signature(
                   root,gt,format,lrRank,renderParents,
                   backArrows,title,theGraph);
  QFile f(baseName+".dot");
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    t << theGraph;
  }
  return checkAndUpdateMd5Signature(baseName,md5); // graph needs to be regenerated
}

QCString DotClassGraph::diskName() const
{
  QCString result=m_diskName.copy();
  switch (m_graphType)
  {
    case DotNode::Collaboration:
      result+="_coll_graph";
      break;
    //case Interface:
    //  result+="_intf_graph";
    //  break;
    case DotNode::Inheritance:
      result+="_inherit_graph";
      break;
    default:
      ASSERT(0);
      break;
  }
  return result;
}

QCString DotClassGraph::writeGraph(FTextStream &out,
                               GraphOutputFormat format,
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
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");

  QCString baseName;
  QCString mapName;
  switch (m_graphType)
  {
    case DotNode::Collaboration:
      mapName="coll_map";
      break;
    //case Interface:
    //  mapName="intf_map";
    //  break;
    case DotNode::Inheritance:
      mapName="inherit_map";
      break;
    default:
      ASSERT(0);
      break;
  }
  baseName = convertNameToFile(diskName());

  // derive target file names from baseName
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString absBaseName = d.absPath().utf8()+"/"+baseName;
  QCString absDotName  = absBaseName+".dot";
  QCString absMapName  = absBaseName+".map";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  QCString absImgName  = absBaseName+"."+imgExt;

  bool regenerate = FALSE;
  if (updateDotGraph(m_startNode,
                 m_graphType,
                 absBaseName,
                 format,
                 m_lrRank,
                 m_graphType==DotNode::Inheritance,
                 TRUE,
                 m_startNode->label()
                ) ||
      !checkDeliverables(format==BITMAP ? absImgName : 
                         usePDFLatex    ? absPdfName : absEpsName,
                         format==BITMAP && generateImageMap ? absMapName : QCString())
     )
  {
    regenerate=TRUE;
    if (format==BITMAP) // run dot to create a bitmap image
    {
      QCString dotArgs(maxCmdLine);

      DotRunner *dotRun = new DotRunner(absDotName,
                              d.absPath().data(),TRUE,absImgName);
      dotRun->addJob(imgExt,absImgName);
      if (generateImageMap) dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);

    }
    else if (format==EPS) // run dot to create a .eps image
    {
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        dotRun->addJob("pdf",absPdfName);
      }
      else
      {
        dotRun->addJob("ps",absEpsName);
      }
      DotManager::instance()->addRun(dotRun);
    }
  }
  Doxygen::indexList->addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap) // produce HTML to include the image
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
          DotManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
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
        case DotNode::Collaboration:
          out << "Collaboration graph";
          break;
        case DotNode::Inheritance:
          out << "Inheritance graph";
          break;
        default:
          ASSERT(0);
          break;
      } 
      out << "\"/>";
      out << "</div>" << endl;

      if (regenerate || !insertMapFile(out,absMapName,relPath,mapLabel))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,relPath,
            FALSE,QCString(),mapLabel);
        out << "<!-- MAP " << mapId << " -->" << endl;
      }
    }
  }
  else if (format==EPS) // produce tex to include the .eps image
  {
    if (regenerate || !writeVecGfxFigure(out,baseName,absBaseName))
    {
      int figId = DotManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE /*TRUE*/);
      out << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removeDotGraph(absDotName);

  return baseName;
}

//--------------------------------------------------------------------

void DotClassGraph::writeXML(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,TRUE);
  }
}

void DotClassGraph::writeDocbook(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDocbook(t,TRUE);
  }
}

void DotClassGraph::writeDEF(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDEF(t);
  }
}

//--------------------------------------------------------------------

int DotInclDepGraph::m_curNodeNumber = 0;

void DotInclDepGraph::buildGraph(DotNode *n,FileDef *fd,int distance)
{
  QList<IncludeInfo> *includeFiles = 
     m_inverse ? fd->includedByFileList() : fd->includeFileList();
  if (includeFiles)
  {
    QListIterator<IncludeInfo> ili(*includeFiles);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      FileDef *bfd = ii->fileDef;
      QCString in  = ii->includeName;
      //printf(">>>> in=`%s' bfd=%p\n",ii->includeName.data(),bfd);
      bool doc=TRUE,src=FALSE;
      if (bfd)
      {
        in  = bfd->absFilePath();  
        doc = bfd->isLinkable() && !bfd->isHidden();
        src = bfd->generateSourceFile();
      }
      if (doc || src || !Config_getBool("HIDE_UNDOC_RELATIONS"))
      {
        QCString url="";
        if (bfd) url=bfd->getOutputFileBase().copy();
        if (!doc && src)
        {
          url=bfd->getSourceFileBase();
        }
        DotNode *bn  = m_usedNodes->find(in);
        if (bn) // file is already a node in the graph
        {
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          bn->setDistance(distance);
        }
        else
        {
          QCString tmp_url;
          QCString tooltip;
          if (bfd) 
          {
            tmp_url=doc || src ? bfd->getReference()+"$"+url : QCString();
            tooltip = bfd->briefDescriptionAsTooltip();
          }
          bn = new DotNode(
              m_curNodeNumber++, // n
              ii->includeName,   // label
              tooltip,           // tip
              tmp_url,           // url
              FALSE,             // rootNode
              0                  // cd
              );
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          m_usedNodes->insert(in,bn);
          bn->setDistance(distance);

          if (bfd) buildGraph(bn,bfd,distance+1);
        }
      }
    }
  }
}

void DotInclDepGraph::determineVisibleNodes(QList<DotNode> &queue, int &maxNodes)
{
  while (queue.count()>0 && maxNodes>0)
  {
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<=maxDistance) // not yet processed
    {
      n->markAsVisible();
      maxNodes--;
      // add direct children
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          queue.append(dn);
        }
      }
    }
  }
}

void DotInclDepGraph::determineTruncatedNodes(QList<DotNode> &queue)
{
  while (queue.count()>0)
  {
    DotNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
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


DotInclDepGraph::DotInclDepGraph(FileDef *fd,bool inverse)
{
  m_inverse = inverse;
  ASSERT(fd!=0);
  m_diskName  = fd->getFileBase().copy();
  QCString tmp_url=fd->getReference()+"$"+fd->getFileBase();
  m_startNode = new DotNode(m_curNodeNumber++,
                            fd->docName(),
                            "",
                            tmp_url.data(),
                            TRUE     // root node
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(fd->absFilePath(),m_startNode);
  buildGraph(m_startNode,fd,1);

  static int nodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int maxNodes = nodes;
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineVisibleNodes(openNodeQueue,maxNodes);
  openNodeQueue.clear();
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue);
}

DotInclDepGraph::~DotInclDepGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotInclDepGraph::diskName() const
{
  QCString result=m_diskName.copy();
  if (m_inverse) result+="_dep";
  result+="_incl";
  return convertNameToFile(result); 
}

QCString DotInclDepGraph::writeGraph(FTextStream &out,
                                 GraphOutputFormat format,
                                 const char *path,
                                 const char *fileName,
                                 const char *relPath,
                                 bool generateImageMap,
                                 int graphId
                                ) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");

  QCString baseName=m_diskName;
  if (m_inverse) baseName+="_dep";
  baseName+="_incl";
  baseName=convertNameToFile(baseName);
  QCString mapName=escapeCharsInString(m_startNode->m_label,FALSE);
  if (m_inverse) mapName+="dep";

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString absBaseName = d.absPath().utf8()+"/"+baseName;
  QCString absDotName  = absBaseName+".dot";
  QCString absMapName  = absBaseName+".map";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  QCString absImgName  = absBaseName+"."+imgExt;

  bool regenerate = FALSE;
  if (updateDotGraph(m_startNode,
                 DotNode::Dependency,
                 absBaseName,
                 format,
                 FALSE,        // lrRank
                 FALSE,        // renderParents
                 m_inverse,    // backArrows
                 m_startNode->label()
                ) ||
      !checkDeliverables(format==BITMAP ? absImgName :
                         usePDFLatex ? absPdfName : absEpsName,
                         format==BITMAP && generateImageMap ? absMapName : QCString())
     )
  {
    regenerate=TRUE;
    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),TRUE,absImgName);
      dotRun->addJob(imgExt,absImgName);
      if (generateImageMap) dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);
    }
    else if (format==EPS)
    {
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        dotRun->addJob("pdf",absPdfName);
      }
      else
      {
        dotRun->addJob("ps",absEpsName);
      }
      DotManager::instance()->addRun(dotRun);
            
    }    
  }
  Doxygen::indexList->addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    if (imgExt=="svg") // Scalable vector graphics
    {
      out << "<div class=\"center\">";
      if (regenerate || !writeSVGFigureLink(out,relPath,baseName,absImgName)) // need to patch the links in the generated SVG file
      {
        if (regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
        out << "<!-- SVG " << mapId << " -->" << endl;
      }
      out << "</div>" << endl;
    }
    else // bitmap graphics
    {
      out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
          << imgExt << "\" border=\"0\" usemap=\"#"
          << mapName << "\" alt=\"\"/>";
      out << "</div>" << endl;

      QCString absMapName = absBaseName+".map";
      if (regenerate || !insertMapFile(out,absMapName,relPath,mapName))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,relPath,
                                                 FALSE,QCString(),mapName);
        out << "<!-- MAP " << mapId << " -->" << endl;
      }
    }
  }
  else if (format==EPS) // encapsulated postscript
  {
    if (regenerate || !writeVecGfxFigure(out,baseName,absBaseName))
    {
      int figId = DotManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE);
      out << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removeDotGraph(absDotName);

  return baseName;
}

bool DotInclDepGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

bool DotInclDepGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = m_startNode->m_children ? m_startNode->m_children->count() : 0;
  return numNodes>=maxNodes;
}

void DotInclDepGraph::writeXML(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeXML(t,FALSE);
  }
}

void DotInclDepGraph::writeDocbook(FTextStream &t)
{
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *node;
  for (;(node=dni.current());++dni)
  {
    node->writeDocbook(t,FALSE);
  }
}

//-------------------------------------------------------------

int DotCallGraph::m_curNodeNumber = 0;

void DotCallGraph::buildGraph(DotNode *n,MemberDef *md,int distance)
{
  MemberSDict *refs = m_inverse ? md->getReferencedByMembers() : md->getReferencesMembers();
  if (refs)
  {
    MemberSDict::Iterator mri(*refs);
    MemberDef *rmd;
    for (;(rmd=mri.current());++mri)
    {
      if (rmd->showInCallGraph())
      {
        QCString uniqueId;
        uniqueId=rmd->getReference()+"$"+
                 rmd->getOutputFileBase()+"#"+rmd->anchor();
        DotNode *bn  = m_usedNodes->find(uniqueId);
        if (bn) // file is already a node in the graph
        {
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          bn->setDistance(distance);
        }
        else
        {
          QCString name;
          if (Config_getBool("HIDE_SCOPE_NAMES"))
          {
            name  = rmd->getOuterScope()==m_scope ? 
                    rmd->name() : rmd->qualifiedName();
          }
          else
          {
            name = rmd->qualifiedName();
          }
          QCString tooltip = rmd->briefDescriptionAsTooltip();
          bn = new DotNode(
              m_curNodeNumber++,
              linkToText(rmd->getLanguage(),name,FALSE),
              tooltip,
              uniqueId,
              0 //distance
              );
          n->addChild(bn,0,0,0);
          bn->addParent(n);
          bn->setDistance(distance);
          m_usedNodes->insert(uniqueId,bn);

          buildGraph(bn,rmd,distance+1);
        }
      }
    }
  }
}

void DotCallGraph::determineVisibleNodes(QList<DotNode> &queue, int &maxNodes)
{
  while (queue.count()>0 && maxNodes>0)
  {
    static int maxDistance = Config_getInt("MAX_DOT_GRAPH_DEPTH");
    DotNode *n = queue.take(0);
    if (!n->isVisible() && n->distance()<=maxDistance) // not yet processed
    {
      n->markAsVisible();
      maxNodes--;
      // add direct children
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
        for (li.toFirst();(dn=li.current());++li)
        {
          queue.append(dn);
        }
      }
    }
  }
}

void DotCallGraph::determineTruncatedNodes(QList<DotNode> &queue)
{
  while (queue.count()>0)
  {
    DotNode *n = queue.take(0);
    if (n->isVisible() && n->isTruncated()==DotNode::Unknown)
    {
      bool truncated = FALSE;
      if (n->m_children)
      {
        QListIterator<DotNode> li(*n->m_children);
        DotNode *dn;
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



DotCallGraph::DotCallGraph(MemberDef *md,bool inverse)
{
  m_inverse = inverse;
  m_diskName = md->getOutputFileBase()+"_"+md->anchor();
  m_scope    = md->getOuterScope();
  QCString uniqueId;
  uniqueId = md->getReference()+"$"+
             md->getOutputFileBase()+"#"+md->anchor();
  QCString name;
  if (Config_getBool("HIDE_SCOPE_NAMES"))
  {
    name = md->name();
  }
  else
  {
    name = md->qualifiedName();
  }
  m_startNode = new DotNode(m_curNodeNumber++,
                            linkToText(md->getLanguage(),name,FALSE),
                            "",
                            uniqueId.data(),
                            TRUE     // root node
                           );
  m_startNode->setDistance(0);
  m_usedNodes = new QDict<DotNode>(1009);
  m_usedNodes->insert(uniqueId,m_startNode);
  buildGraph(m_startNode,md,1);

  static int nodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int maxNodes = nodes;
  //int directChildNodes = 1;
  //if (m_startNode->m_children!=0) 
  //  directChildNodes+=m_startNode->m_children->count();
  //if (directChildNodes>maxNodes) maxNodes=directChildNodes;
  QList<DotNode> openNodeQueue;
  openNodeQueue.append(m_startNode);
  determineVisibleNodes(openNodeQueue,maxNodes);
  openNodeQueue.clear();
  openNodeQueue.append(m_startNode);
  determineTruncatedNodes(openNodeQueue);
}

DotCallGraph::~DotCallGraph()
{
  deleteNodes(m_startNode);
  delete m_usedNodes;
}

QCString DotCallGraph::writeGraph(FTextStream &out, GraphOutputFormat format,
                        const char *path,const char *fileName,
                        const char *relPath,bool generateImageMap,int
                        graphId) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");

  QCString baseName = m_diskName + (m_inverse ? "_icgraph" : "_cgraph");
  QCString mapName  = baseName;

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString absBaseName = d.absPath().utf8()+"/"+baseName;
  QCString absDotName  = absBaseName+".dot";
  QCString absMapName  = absBaseName+".map";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  QCString absImgName  = absBaseName+"."+imgExt;

  bool regenerate = FALSE;
  if (updateDotGraph(m_startNode,
                 DotNode::CallGraph,
                 absBaseName,
                 format,
                 TRUE,         // lrRank
                 FALSE,        // renderParents
                 m_inverse,    // backArrows
                 m_startNode->label()
                ) ||
      !checkDeliverables(format==BITMAP ? absImgName :
                         usePDFLatex ? absPdfName : absEpsName,
                         format==BITMAP && generateImageMap ? absMapName : QCString())
     )
  {
    regenerate=TRUE;
    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),TRUE,absImgName);
      dotRun->addJob(imgExt,absImgName);
      if (generateImageMap) dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);

    }
    else if (format==EPS)
    {
      // run dot to create a .eps image
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        dotRun->addJob("pdf",absPdfName);
      }
      else
      {
        dotRun->addJob("ps",absEpsName);
      }
      DotManager::instance()->addRun(dotRun);

    }
  }
  Doxygen::indexList->addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    if (imgExt=="svg") // Scalable vector graphics
    {
      out << "<div class=\"center\">";
      if (regenerate || !writeSVGFigureLink(out,relPath,baseName,absImgName)) // need to patch the links in the generated SVG file
      {
        if (regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
        out << "<!-- SVG " << mapId << " -->" << endl;
      }
      out << "</div>" << endl;
    }
    else // bitmap graphics
    {
      out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
          << imgExt << "\" border=\"0\" usemap=\"#"
          << mapName << "\" alt=\"";
      out << "\"/>";
      out << "</div>" << endl;

      if (regenerate || !insertMapFile(out,absMapName,relPath,mapName))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,relPath,
                                                   FALSE,QCString(),mapName);
        out << "<!-- MAP " << mapId << " -->" << endl;
      }
    }
  }
  else if (format==EPS) // encapsulated postscript
  {
    if (regenerate || !writeVecGfxFigure(out,baseName,absBaseName))
    {
      int figId = DotManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE);
      out << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removeDotGraph(absDotName);

  return baseName;
}

bool DotCallGraph::isTrivial() const
{
  return m_startNode->m_children==0;
}

bool DotCallGraph::isTooBig() const
{
  static int maxNodes = Config_getInt("DOT_GRAPH_MAX_NODES");
  int numNodes = m_startNode->m_children ? m_startNode->m_children->count() : 0;
  return numNodes>=maxNodes;
}

//-------------------------------------------------------------

DotDirDeps::DotDirDeps(DirDef *dir) : m_dir(dir)
{
}

DotDirDeps::~DotDirDeps()
{
}

QCString DotDirDeps::writeGraph(FTextStream &out,
                            GraphOutputFormat format,
                            const char *path,
                            const char *fileName,
                            const char *relPath,
                            bool generateImageMap,
                            int graphId) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");

  QCString baseName=m_dir->getOutputFileBase()+"_dep";
  QCString mapName=escapeCharsInString(baseName,FALSE);

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString absBaseName = d.absPath().utf8()+"/"+baseName;
  QCString absDotName  = absBaseName+".dot";
  QCString absMapName  = absBaseName+".map";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  QCString absImgName  = absBaseName+"."+imgExt;

  // compute md5 checksum of the graph were are about to generate
  QGString theGraph;
  FTextStream md5stream(&theGraph);
  m_dir->writeDepGraph(md5stream);
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)theGraph.data(),theGraph.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  bool regenerate=FALSE;
  if (checkAndUpdateMd5Signature(absBaseName,sigStr) ||
      !checkDeliverables(format==BITMAP ? absImgName :
                         usePDFLatex ? absPdfName : absEpsName,
                         format==BITMAP && generateImageMap ? absMapName : QCString())
     )
  {
    regenerate=TRUE;

    QFile f(absDotName);
    if (!f.open(IO_WriteOnly))
    {
      err("Cannot create file %s.dot for writing!\n",baseName.data());
    }
    FTextStream t(&f);
    t << theGraph.data();
    f.close();

    if (format==BITMAP)
    {
      // run dot to create a bitmap image
      QCString dotArgs(maxCmdLine);
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),TRUE,absImgName);
      dotRun->addJob(imgExt,absImgName);
      if (generateImageMap) dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);
    }
    else if (format==EPS)
    {
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        dotRun->addJob("pdf",absPdfName);
      }
      else
      {
        dotRun->addJob("ps",absEpsName);
      }
      DotManager::instance()->addRun(dotRun);
    }
  }
  Doxygen::indexList->addImageFile(baseName+"."+imgExt);

  if (format==BITMAP && generateImageMap)
  {
    if (imgExt=="svg") // Scalable vector graphics
    {
      out << "<div class=\"center\">";
      if (regenerate || !writeSVGFigureLink(out,relPath,baseName,absImgName)) // need to patch the links in the generated SVG file
      {
        if (regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
        out << "<!-- SVG " << mapId << " -->" << endl;
      }
      out << "</div>" << endl;
    }
    else // bitmap graphics
    {
      out << "<div class=\"center\"><img src=\"" << relPath << baseName << "." 
          << imgExt << "\" border=\"0\" usemap=\"#"
          << mapName << "\" alt=\"";
      out << convertToXML(m_dir->displayName());
      out << "\"/>";
      out << "</div>" << endl;

      if (regenerate || !insertMapFile(out,absMapName,relPath,mapName))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,relPath,
                                                   TRUE,QCString(),mapName);
        out << "<!-- MAP " << mapId << " -->" << endl;
      }
    }
  }
  else if (format==EPS)
  {
    if (regenerate || !writeVecGfxFigure(out,baseName,absBaseName))
    {
      int figId = DotManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE);
      out << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removeDotGraph(absDotName);

  return baseName;
}

bool DotDirDeps::isTrivial() const
{
  return m_dir->depGraphIsTrivial();
}

//-------------------------------------------------------------

void generateGraphLegend(const char *path)
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }

  QGString theGraph;
  FTextStream md5stream(&theGraph);
  writeGraphHeader(md5stream,theTranslator->trLegendTitle());
  md5stream << "  Node9 [shape=\"box\",label=\"Inherited\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",fillcolor=\"grey75\",style=\"filled\" fontcolor=\"black\"];\n";
  md5stream << "  Node10 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node10 [shape=\"box\",label=\"PublicBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classPublicBase" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node11 -> Node10 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node11 [shape=\"box\",label=\"Truncated\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"red\",URL=\"$classTruncated" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node13 -> Node9 [dir=\"back\",color=\"darkgreen\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node13 [shape=\"box\",label=\"ProtectedBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classProtectedBase" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node14 -> Node9 [dir=\"back\",color=\"firebrick4\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node14 [shape=\"box\",label=\"PrivateBase\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classPrivateBase" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node15 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node15 [shape=\"box\",label=\"Undocumented\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"grey75\"];\n";
  md5stream << "  Node16 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << FONTSIZE << "\",style=\"solid\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node16 [shape=\"box\",label=\"Templ< int >\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node17 -> Node16 [dir=\"back\",color=\"orange\",fontsize=\"" << FONTSIZE << "\",style=\"dashed\",label=\"< int >\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node17 [shape=\"box\",label=\"Templ< T >\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
  md5stream << "  Node18 -> Node9 [dir=\"back\",color=\"darkorchid3\",fontsize=\"" << FONTSIZE << "\",style=\"dashed\",label=\"m_usedClass\",fontname=\"" << FONTNAME << "\"];\n";
  md5stream << "  Node18 [shape=\"box\",label=\"Used\",fontsize=\"" << FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << FONTNAME << "\",color=\"black\",URL=\"$classUsed" << Doxygen::htmlFileExtension << "\"];\n";
  writeGraphFooter(md5stream);
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)theGraph.data(),theGraph.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  QCString absBaseName = (QCString)path+"/graph_legend";
  QCString absDotName  = absBaseName+".dot";
  QCString imgExt      = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName     = "graph_legend."+imgExt;
  QCString absImgName  = absBaseName+"."+imgExt;
  if (checkAndUpdateMd5Signature(absBaseName,sigStr) ||
      !checkDeliverables(absImgName))
  {
    QFile dotFile(absDotName);
    if (!dotFile.open(IO_WriteOnly))
    {
      err("Could not open file %s for writing\n",dotFile.name().data());
      return;
    }

    FTextStream dotText(&dotFile); 
    dotText << theGraph;
    dotFile.close();

    // run dot to generate the a bitmap image from the graph

    DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),TRUE,absImgName);
    dotRun->addJob(imgExt,absImgName);
    DotManager::instance()->addRun(dotRun);
  }
  else
  {
    removeDotGraph(absDotName);
  }
  Doxygen::indexList->addImageFile(imgName);

  if (imgExt=="svg")
  {
    DotManager::instance()->addSVGObject(
        absBaseName+Config_getString("HTML_FILE_EXTENSION"),
        "graph_legend",
        absImgName,QCString());
  }

}

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format)
{
  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir); exit(1);
  }

  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = (QCString)outFile+"."+imgExt;
  QCString absImgName = d.absPath().utf8()+"/"+imgName;
  QCString absOutFile = d.absPath().utf8()+"/"+outFile;

  DotRunner dotRun(inFile,d.absPath().data(),FALSE,absImgName);
  if (format==BITMAP)
    dotRun.addJob(imgExt,absImgName);
  else // format==EPS
  {
    if (Config_getBool("USE_PDFLATEX"))
    {
      dotRun.addJob("pdf",absOutFile+".pdf");
    }
    else
    {
      dotRun.addJob("ps",absOutFile+".eps");
    }
  }

  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
     return;
  }

  if (format==BITMAP) checkDotResult(absImgName);

  Doxygen::indexList->addImageFile(imgName);

}

 
/*! Writes user defined image map to the output.
 *  \param t text stream to write to
 *  \param inFile just the basename part of the filename
 *  \param outDir output directory
 *  \param relPath relative path the to root of the output dir
 *  \param baseName the base name of the output files
 *  \param context the scope in which this graph is found (for resolving links)
 *  \param graphId a unique id for this graph, use for dynamic sections
 */
void writeDotImageMapFromFile(FTextStream &t,
                            const QCString &inFile, const QCString &outDir,
                            const QCString &relPath, const QCString &baseName,
                            const QCString &context,int graphId)
{

  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir.data()); exit(1);
  }

  QCString mapName = baseName+".map";
  QCString imgExt = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString imgName = baseName+"."+imgExt;
  QCString absOutFile = d.absPath().utf8()+"/"+mapName;

  DotRunner dotRun(inFile,d.absPath().data(),FALSE);
  dotRun.addJob(MAP_CMD,absOutFile);
  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    //writeSVGFigureLink(t,relPath,inFile,inFile+".svg");
    //DotFilePatcher patcher(inFile+".svg");
    QCString svgName=outDir+"/"+baseName+".svg";
    writeSVGFigureLink(t,relPath,baseName,svgName);
    DotFilePatcher patcher(svgName);
    patcher.addSVGConversion(relPath,TRUE,context,TRUE,graphId);
    patcher.run();
  }
  else // bitmap graphics
  {
    t << "<img src=\"" << relPath << imgName << "\" alt=\""
      << imgName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>" << endl
      << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">";

    convertMapFile(t, absOutFile, relPath ,TRUE, context);

    t << "</map>" << endl;
  }
  d.remove(absOutFile);
}

//-------------------------------------------------------------

DotGroupCollaboration::DotGroupCollaboration(GroupDef* gd)
{
    m_curNodeId = 0;
    QCString tmp_url = gd->getReference()+"$"+gd->getOutputFileBase();
    m_usedNodes = new QDict<DotNode>(1009);
    m_rootNode = new DotNode(m_curNodeId++, gd->groupTitle(), "", tmp_url, TRUE );
    m_rootNode->markAsVisible();
    m_usedNodes->insert(gd->name(), m_rootNode );
    m_edges.setAutoDelete(TRUE);

    m_diskName = gd->getOutputFileBase();

    buildGraph( gd );
}

DotGroupCollaboration::~DotGroupCollaboration()
{
  delete m_usedNodes;
}

void DotGroupCollaboration::buildGraph(GroupDef* gd)
{
  QCString tmp_url;
  //===========================
  // hierarchy.

  // Write parents
  GroupList *groups = gd->partOfGroups();
  if ( groups )
  {
    GroupListIterator gli(*groups);
    GroupDef *d;
    for (gli.toFirst();(d=gli.current());++gli)
    {
      DotNode* nnode = m_usedNodes->find(d->name());
      if ( !nnode )
      { // add node
        tmp_url = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, d->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
      }
      tmp_url = "";
      addEdge( nnode, m_rootNode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  // Add subgroups
  if ( gd->getSubGroups() && gd->getSubGroups()->count() )
  {
    QListIterator<GroupDef> defli(*gd->getSubGroups());
    GroupDef *def;
    for (;(def=defli.current());++defli)
    {
      DotNode* nnode = m_usedNodes->find(def->name());
      if ( !nnode )
      { // add node
        tmp_url = def->getReference()+"$"+def->getOutputFileBase();
        QCString tooltip = def->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, def->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(def->name(), nnode );
      }
      tmp_url = "";
      addEdge( m_rootNode, nnode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  //=======================
  // Write collaboration

  // Add members
  addMemberList( gd->getMemberList(MemberListType_allMembersList) );

  // Add classes
  if ( gd->getClasses() && gd->getClasses()->count() )
  {
    ClassSDict::Iterator defli(*gd->getClasses());
    ClassDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      if (!def->anchor().isEmpty())
      {
        tmp_url+="#"+def->anchor();
      }
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tclass );          
    }
  }

  // Add namespaces
  if ( gd->getNamespaces() && gd->getNamespaces()->count() )
  {
    NamespaceSDict::Iterator defli(*gd->getNamespaces());
    NamespaceDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tnamespace );          
    }
  }

  // Add files
  if ( gd->getFiles() && gd->getFiles()->count() )
  {
    QListIterator<FileDef> defli(*gd->getFiles());
    FileDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tfile );          
    }
  }

  // Add pages
  if ( gd->getPages() && gd->getPages()->count() )
  {
    PageSDict::Iterator defli(*gd->getPages());
    PageDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tpages );          
    }
  }

  // Add directories
  if ( gd->getDirs() && gd->getDirs()->count() )
  {
    QListIterator<DirDef> defli(*gd->getDirs());
    DirDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tdir );          
    }
  }
}

void DotGroupCollaboration::addMemberList( MemberList* ml )
{
  if ( !( ml && ml->count()) ) return;
  MemberListIterator defli(*ml);
  MemberDef *def;
  for (;(def=defli.current());++defli)
  {
    QCString tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension
      +"#"+def->anchor();
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tmember );
  }
}

DotGroupCollaboration::Edge* DotGroupCollaboration::addEdge( 
    DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
    const QCString& _label, const QCString& _url )
{
  // search a existing link.
  QListIterator<Edge> lli(m_edges);
  Edge* newEdge = 0;
  for ( lli.toFirst(); (newEdge=lli.current()); ++lli)
  {
    if ( newEdge->pNStart==_pNStart && 
         newEdge->pNEnd==_pNEnd &&
         newEdge->eType==_eType 
       )
    { // edge already found
      break;
    }
  }
  if ( newEdge==0 ) // new link
  {
    newEdge = new Edge(_pNStart,_pNEnd,_eType);
    m_edges.append( newEdge );
  } 

  if (!_label.isEmpty())
  {
    newEdge->links.append(new Link(_label,_url));
  }

  return newEdge;
}

void DotGroupCollaboration::addCollaborationMember( 
    Definition* def, QCString& url, EdgeType eType )
{
  // Create group nodes
  if ( !def->partOfGroups() )
    return;
  GroupListIterator gli(*def->partOfGroups());
  GroupDef *d;
  QCString tmp_str;
  for (;(d=gli.current());++gli)
  {
    DotNode* nnode = m_usedNodes->find(d->name());
    if ( nnode != m_rootNode )
    {
      if ( nnode==0 )
      { // add node
        tmp_str = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(m_curNodeId++, d->groupTitle(), tooltip, tmp_str );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
      }
      tmp_str = def->qualifiedName();
      addEdge( m_rootNode, nnode, eType, tmp_str, url );
    }
  }
}


QCString DotGroupCollaboration::writeGraph( FTextStream &t, GraphOutputFormat format,
    const char *path, const char *fileName, const char *relPath,
    bool writeImageMap,int graphId) const
{
  QDir d(path);
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",path); exit(1);
  }
  static bool usePDFLatex = Config_getBool("USE_PDFLATEX");

  QGString theGraph;
  FTextStream md5stream(&theGraph);
  writeGraphHeader(md5stream,m_rootNode->label());

  // clean write flags
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *pn;
  for (dni.toFirst();(pn=dni.current());++dni)
  {
    pn->clearWriteFlag();
  }

  // write other nodes.
  for (dni.toFirst();(pn=dni.current());++dni)
  {
    pn->write(md5stream,DotNode::Inheritance,format,TRUE,FALSE,FALSE,FALSE);
  }

  // write edges
  QListIterator<Edge> eli(m_edges);
  Edge* edge;
  for (eli.toFirst();(edge=eli.current());++eli)
  {
    edge->write( md5stream );
  }

  writeGraphFooter(md5stream);
  resetReNumbering();
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)theGraph.data(),theGraph.length(),md5_sig);
  MD5SigToString(md5_sig,sigStr.data(),33);
  QCString imgExt      = Config_getEnum("DOT_IMAGE_FORMAT");
  QCString baseName    = m_diskName;
  QCString imgName     = baseName+"."+imgExt;
  QCString mapName     = baseName+".map";
  QCString absPath     = d.absPath().data();
  QCString absBaseName = absPath+"/"+baseName;
  QCString absDotName  = absBaseName+".dot";
  QCString absImgName  = absBaseName+"."+imgExt;
  QCString absMapName  = absBaseName+".map";
  QCString absPdfName  = absBaseName+".pdf";
  QCString absEpsName  = absBaseName+".eps";
  bool regenerate=FALSE;
  if (checkAndUpdateMd5Signature(absBaseName,sigStr) ||
      !checkDeliverables(format==BITMAP ? absImgName :
                         usePDFLatex ? absPdfName : absEpsName,
                         format==BITMAP /*&& generateImageMap*/ ? absMapName : QCString())
     )
  {
    regenerate=TRUE;

    QFile dotfile(absDotName);
    if (dotfile.open(IO_WriteOnly))
    {
      FTextStream tdot(&dotfile);
      tdot << theGraph;
      dotfile.close();
    }

    if (format==BITMAP) // run dot to create a bitmap image
    {
      QCString dotArgs(maxCmdLine);

      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      dotRun->addJob(imgExt,absImgName);
      if (writeImageMap) dotRun->addJob(MAP_CMD,absMapName);
      DotManager::instance()->addRun(dotRun);

    }
    else if (format==EPS)
    {
      DotRunner *dotRun = new DotRunner(absDotName,d.absPath().data(),FALSE);
      if (usePDFLatex)
      {
        dotRun->addJob("pdf",absPdfName);
      }
      else
      {
        dotRun->addJob("ps",absEpsName);
      }
      DotManager::instance()->addRun(dotRun);
    }

  }
  if (format==BITMAP && writeImageMap)
  {
    QCString mapLabel = escapeCharsInString(baseName,FALSE);
    t << "<center><table><tr><td>";

    if (imgExt=="svg")
    {
      t << "<div class=\"center\">";
      if (regenerate || !writeSVGFigureLink(t,relPath,baseName,absImgName)) // need to patch the links in the generated SVG file
      {
        if (regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName,relPath,FALSE,QCString(),TRUE,graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(fileName,baseName,absImgName,relPath);
        t << "<!-- SVG " << mapId << " -->" << endl;
      }
      t << "</div>" << endl;
    }
    else
    {
      t << "<img src=\"" << relPath << imgName
        << "\" border=\"0\" alt=\"\" usemap=\"#" 
        << mapLabel << "\"/>" << endl;
      if (regenerate || !insertMapFile(t,absMapName,relPath,mapLabel))
      {
        int mapId = DotManager::instance()->addMap(fileName,absMapName,relPath,
                                                   FALSE,QCString(),mapLabel);
        t << "<!-- MAP " << mapId << " -->" << endl;
      }
    }

    t << "</td></tr></table></center>" << endl;
  }
  else if (format==EPS)
  {
    if (regenerate || !writeVecGfxFigure(t,baseName,absBaseName))
    {
      int figId = DotManager::instance()->addFigure(fileName,baseName,absBaseName,FALSE);
      t << endl << "% FIG " << figId << endl;
    }
  }
  if (!regenerate) removeDotGraph(absDotName);

  return baseName;
}

void DotGroupCollaboration::Edge::write( FTextStream &t ) const
{
  const char* linkTypeColor[] = {
       "darkorchid3"
      ,"orange"
      ,"blueviolet"
      ,"darkgreen"   
      ,"firebrick4"  
      ,"grey75"
      ,"midnightblue"
  };
  QCString arrowStyle = "dir=\"none\", style=\"dashed\"";
  t << "  Node" << pNStart->number();
  t << "->";
  t << "Node" << pNEnd->number();

  t << " [shape=plaintext";
  if (links.count()>0) // there are links
  {
    t << ", ";
    // HTML-like edge labels crash on my Mac with Graphviz 2.0! and
    // are not supported by older version of dot.
    //
    //t << label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\">";
    //QListIterator<Link> lli(links);
    //Link *link;
    //for( lli.toFirst(); (link=lli.current()); ++lli)
    //{
    //  t << "<TR><TD";
    //  if ( !link->url.isEmpty() )
    //    t << " HREF=\"" << link->url << "\"";
    //  t << ">" << link->label << "</TD></TR>";
    //}
    //t << "</TABLE>>";

    t << "label=\"";
    QListIterator<Link> lli(links);
    Link *link;
    bool first=TRUE;
    int count=0;
    const int maxLabels = 10;
    for( lli.toFirst(); (link=lli.current()) && count<maxLabels; ++lli,++count)
    {
      if (first) first=FALSE; else t << "\\n"; 
      t << convertLabel(link->label);
    }
    if (count==maxLabels) t << "\\n...";
    t << "\"";

  }
  switch( eType )
  {
    case thierarchy :
      arrowStyle = "dir=\"back\", style=\"solid\"";
    default :
      t << ", color=\"" << linkTypeColor[(int)eType] << "\"";
      break;
  }
  t << ", " << arrowStyle;
  t << "];" << endl;
}

bool DotGroupCollaboration::isTrivial() const
{
  return m_usedNodes->count() <= 1;
}

void DotGroupCollaboration::writeGraphHeader(FTextStream &t,
      const QCString &title) const
{
  t << "digraph ";
  if (title.isEmpty())
  {
    t << "\"Dot Graph\"";
  }
  else
  {
    t << "\"" << convertToXML(title) << "\"";
  }
  t << endl;
  t << "{" << endl;
  if (Config_getBool("DOT_TRANSPARENT"))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << FONTNAME << "\",fontsize=\"" << FONTSIZE << "\","
    "labelfontname=\"" << FONTNAME << "\",labelfontsize=\"" << FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << FONTNAME << "\",fontsize=\"" << FONTSIZE << "\",shape=record];\n";
  t << "  rankdir=LR;\n";
}

void writeDotDirDepGraph(FTextStream &t,DirDef *dd)
{
    t << "digraph \"" << dd->displayName() << "\" {\n";
    if (Config_getBool("DOT_TRANSPARENT"))
    {
      t << "  bgcolor=transparent;\n";
    }
    t << "  compound=true\n";
    t << "  node [ fontsize=\"" << FONTSIZE << "\", fontname=\"" << FONTNAME << "\"];\n";
    t << "  edge [ labelfontsize=\"" << FONTSIZE << "\", labelfontname=\"" << FONTNAME << "\"];\n";

    QDict<DirDef> dirsInGraph(257);
    
    dirsInGraph.insert(dd->getOutputFileBase(),dd);
    if (dd->parent())
    {
      t << "  subgraph cluster" << dd->parent()->getOutputFileBase() << " {\n";
      t << "    graph [ bgcolor=\"#ddddee\", pencolor=\"black\", label=\"" 
        << dd->parent()->shortName() 
        << "\" fontname=\"" << FONTNAME << "\", fontsize=\"" << FONTSIZE << "\", URL=\"";
      t << dd->parent()->getOutputFileBase() << Doxygen::htmlFileExtension;
      t << "\"]\n";
    }
    if (dd->isCluster())
    {
      t << "  subgraph cluster" << dd->getOutputFileBase() << " {\n";
      t << "    graph [ bgcolor=\"#eeeeff\", pencolor=\"black\", label=\"\""
        << " URL=\"" << dd->getOutputFileBase() << Doxygen::htmlFileExtension 
        << "\"];\n";
      t << "    " << dd->getOutputFileBase() << " [shape=plaintext label=\"" 
        << dd->shortName() << "\"];\n";

      // add nodes for sub directories
      QListIterator<DirDef> sdi(dd->subDirs());
      DirDef *sdir;
      for (sdi.toFirst();(sdir=sdi.current());++sdi)
      {
        t << "    " << sdir->getOutputFileBase() << " [shape=box label=\"" 
          << sdir->shortName() << "\"";
        if (sdir->isCluster())
        {
          t << " color=\"red\"";
        }
        else
        {
          t << " color=\"black\"";
        }
        t << " fillcolor=\"white\" style=\"filled\"";
        t << " URL=\"" << sdir->getOutputFileBase() 
          << Doxygen::htmlFileExtension << "\"";
        t << "];\n";
        dirsInGraph.insert(sdir->getOutputFileBase(),sdir);
      }
      t << "  }\n";
    }
    else
    {
      t << "  " << dd->getOutputFileBase() << " [shape=box, label=\"" 
        << dd->shortName() << "\", style=\"filled\", fillcolor=\"#eeeeff\","
        << " pencolor=\"black\", URL=\"" << dd->getOutputFileBase() 
        << Doxygen::htmlFileExtension << "\"];\n";
    }
    if (dd->parent())
    {
      t << "  }\n";
    }

    // add nodes for other used directories
    QDictIterator<UsedDir> udi(*dd->usedDirs());
    UsedDir *udir;
    //printf("*** For dir %s\n",shortName().data());
    for (udi.toFirst();(udir=udi.current());++udi) 
      // for each used dir (=directly used or a parent of a directly used dir)
    {
      const DirDef *usedDir=udir->dir();
      DirDef *dir=dd;
      while (dir)
      {
        //printf("*** check relation %s->%s same_parent=%d !%s->isParentOf(%s)=%d\n",
        //    dir->shortName().data(),usedDir->shortName().data(),
        //    dir->parent()==usedDir->parent(),
        //    usedDir->shortName().data(),
        //    shortName().data(),
        //    !usedDir->isParentOf(this)
        //    );
        if (dir!=usedDir && dir->parent()==usedDir->parent() && 
            !usedDir->isParentOf(dd))
          // include if both have the same parent (or no parent)
        {
          t << "  " << usedDir->getOutputFileBase() << " [shape=box label=\"" 
            << usedDir->shortName() << "\"";
          if (usedDir->isCluster())
          {
            if (!Config_getBool("DOT_TRANSPARENT"))
            {
              t << " fillcolor=\"white\" style=\"filled\"";
            }
            t << " color=\"red\"";
          }
          t << " URL=\"" << usedDir->getOutputFileBase() 
            << Doxygen::htmlFileExtension << "\"];\n";
          dirsInGraph.insert(usedDir->getOutputFileBase(),usedDir);
          break;
        }
        dir=dir->parent();
      }
    }

    // add relations between all selected directories
    DirDef *dir;
    QDictIterator<DirDef> di(dirsInGraph);
    for (di.toFirst();(dir=di.current());++di) // foreach dir in the graph
    {
      QDictIterator<UsedDir> udi(*dir->usedDirs());
      UsedDir *udir;
      for (udi.toFirst();(udir=udi.current());++udi) // foreach used dir
      {
        const DirDef *usedDir=udir->dir();
        if ((dir!=dd || !udir->inherited()) &&     // only show direct dependendies for this dir
            (usedDir!=dd || !udir->inherited()) && // only show direct dependendies for this dir
            !usedDir->isParentOf(dir) &&             // don't point to own parent
            dirsInGraph.find(usedDir->getOutputFileBase())) // only point to nodes that are in the graph
        {
          QCString relationName;
          relationName.sprintf("dir_%06d_%06d",dir->dirCount(),usedDir->dirCount());
          if (Doxygen::dirRelations.find(relationName)==0)
          {
            // new relation
            Doxygen::dirRelations.append(relationName,
                new DirRelation(relationName,dir,udir));
          }
          int nrefs = udir->filePairs().count();
          t << "  " << dir->getOutputFileBase() << "->" 
                    << usedDir->getOutputFileBase();
          t << " [headlabel=\"" << nrefs << "\", labeldistance=1.5";
          t << " headhref=\"" << relationName << Doxygen::htmlFileExtension 
            << "\"];\n";
        }
      }
    }

    t << "}\n";
}

/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#include "dotfilepatcher.h"
#include "dotrunner.h"

#include "qstring.h"
#include "config.h"
#include "qdir.h"
#include "message.h"
#include "ftextstream.h"
#include "docparser.h"
#include "doxygen.h"
#include "util.h"
#include "dot.h"

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
// link to original SVG
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

static QCString replaceRef(const QCString &buf,const QCString relPath,
  bool urlOnly,const QCString &context,const QCString &target=QCString())
{
  // search for href="...", store ... part in link
  QCString href = "href";
  //bool isXLink=FALSE;
  int len = 6;
  int indexS = buf.find("href=\""), indexE;
  bool targetAlreadySet = buf.find("target=")!=-1;
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
          result = externalLinkTarget(true);
          if (!result.isEmpty())targetAlreadySet=true;
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
    if (!target.isEmpty() && !targetAlreadySet)
    {
      result+=" target=\""+target+"\"";
    }
    QCString leftPart = buf.left(indexS);
    QCString rightPart = buf.mid(indexE+1);
    //printf("replaceRef(\n'%s'\n)->\n'%s+%s+%s'\n",
    //    buf.data(),leftPart.data(),result.data(),rightPart.data());
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
bool DotFilePatcher::convertMapFile(FTextStream &t,const char *mapName,
                    const QCString relPath, bool urlOnly,
                    const QCString &context)
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
    int numBytes = f.readLine(buf.rawData(),maxLineLen);
    if (numBytes>0)
    {
      buf.resize(numBytes+1);

      if (buf.left(5)=="<area")
      {
        QCString replBuf = replaceRef(buf,relPath,urlOnly,context);
        // strip id="..." from replBuf since the id's are not needed and not unique.
        int indexS = replBuf.find("id=\""), indexE;
        if (indexS>0 && (indexE=replBuf.find('"',indexS+4))!=-1)
        {
          t << replBuf.left(indexS-1) << replBuf.right(replBuf.length() - indexE - 1);
        }
        else
        {
          t << replBuf;
        }
      }
    }
  }
  return TRUE;
}

DotFilePatcher::DotFilePatcher(const char *patchFile)
  : m_patchFile(patchFile)
{
}

bool DotFilePatcher::isSVGFile() const
{
  return m_patchFile.right(4)==".svg";
}

int DotFilePatcher::addMap(const QCString &mapFile,const QCString &relPath,
                           bool urlOnly,const QCString &context,const QCString &label)
{
  int id = (int)m_maps.size();
  m_maps.emplace_back(mapFile,relPath,urlOnly,context,label);
  return id;
}

int DotFilePatcher::addFigure(const QCString &baseName,
                              const QCString &figureName,bool heightCheck)
{
  int id = (int)m_maps.size();
  m_maps.emplace_back(figureName,"",heightCheck,"",baseName);
  return id;
}

int DotFilePatcher::addSVGConversion(const QCString &relPath,bool urlOnly,
                                     const QCString &context,bool zoomable,
                                     int graphId)
{
  int id = (int)m_maps.size();
  m_maps.emplace_back("",relPath,urlOnly,context,"",zoomable,graphId);
  return id;
}

int DotFilePatcher::addSVGObject(const QCString &baseName,
                                 const QCString &absImgName,
                                 const QCString &relPath)
{
  int id = (int)m_maps.size();
  m_maps.emplace_back(absImgName,relPath,false,"",baseName);
  return id;
}

bool DotFilePatcher::run() const
{
  //printf("DotFilePatcher::run(): %s\n",m_patchFile.data());
  bool interactiveSVG_local = Config_getBool(INTERACTIVE_SVG);
  bool isSVGFile = m_patchFile.right(4)==".svg";
  int graphId = -1;
  QCString relPath;
  if (isSVGFile)
  {
    const Map &map = m_maps.front(); // there is only one 'map' for a SVG file
    interactiveSVG_local = interactiveSVG_local && map.zoomable;
    graphId = map.graphId;
    relPath = map.relPath;
    //printf("DotFilePatcher::addSVGConversion: file=%s zoomable=%d\n",
    //    m_patchFile.data(),map->zoomable);
  }
  QCString tmpName = m_patchFile+".tmp";
  QCString patchFile = m_patchFile;
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
    line.resize(numBytes+1);

    //printf("line=[%s]\n",line.stripWhiteSpace().data());
    int i;
    ASSERT(numBytes<maxLineLen);
    if (isSVGFile)
    {
      if (interactiveSVG_local)
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
        const Map &map = m_maps.front(); // there is only one 'map' for a SVG file
        t << replaceRef(line,map.relPath,map.urlOnly,map.context,"_top");
      }
    }
    else if ((i=line.find("<!-- SVG"))!=-1 || (i=line.find("[!-- SVG"))!=-1)
    {
      //printf("Found marker at %d\n",i);
      int mapId=-1;
      t << line.left(i);
      int n = sscanf(line.data()+i+1,"!-- SVG %d",&mapId);
      if (n==1 && mapId>=0 && mapId<(int)m_maps.size())
      {
        int e = QMAX(line.find("--]"),line.find("-->"));
        const Map &map = m_maps.at(mapId);
        //printf("DotFilePatcher::writeSVGFigure: file=%s zoomable=%d\n",
        //  m_patchFile.data(),map.zoomable);
        if (!writeSVGFigureLink(t,map.relPath,map.label,map.mapFile))
        {
          err("Problem extracting size from SVG file %s\n",map.mapFile.data());
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
      if (n==1 && mapId>=0 && mapId<(int)m_maps.size())
      {
        QGString result;
        FTextStream tt(&result);
        const Map &map = m_maps.at(mapId);
        //printf("patching MAP %d in file %s with contents of %s\n",
        //   mapId,m_patchFile.data(),map.mapFile.data());
        convertMapFile(tt,map.mapFile,map.relPath,map.urlOnly,map.context);
        if (!result.isEmpty())
        {
          t << "<map name=\"" << correctId(map.label) << "\" id=\"" << correctId(map.label) << "\">" << endl;
          t << result;
          t << "</map>" << endl;
        }
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
      if (n==1 && mapId>=0 && mapId<(int)m_maps.size())
      {
        const Map &map = m_maps.at(mapId);
        //printf("patching FIG %d in file %s with contents of %s\n",
        //   mapId,m_patchFile.data(),map.mapFile.data());
        if (!writeVecGfxFigure(t,map.label,map.mapFile))
        {
          err("problem writing FIG %d figure!\n",mapId);
          return FALSE;
        }
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
  if (isSVGFile && interactiveSVG_local && replacedHeader)
  {
    QCString orgName=m_patchFile.left(m_patchFile.length()-4)+"_org.svg";
    t << substitute(svgZoomFooter,"$orgname",stripPath(orgName));
    fo.close();
    // keep original SVG file so we can refer to it, we do need to replace
    // dummy link by real ones
    fi.setName(tmpName);
    fo.setName(orgName);
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
    FTextStream to(&fo);
    while (!fi.atEnd()) // foreach line
    {
      QCString line(maxLineLen);
      int numBytes = fi.readLine(line.rawData(),maxLineLen);
      if (numBytes<=0)
      {
        break;
      }
      line.resize(numBytes+1);
      const Map &map = m_maps.front(); // there is only one 'map' for a SVG file
      to << replaceRef(line,map.relPath,map.urlOnly,map.context,"_top");
    }
    fi.close();
    fo.close();
  }
  // remove temporary file
  QDir::current().remove(tmpName);
  return TRUE;
}

//---------------------------------------------------------------------------------------------


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

/// Check if a reference to a SVG figure can be written and do so if possible.
/// Returns FALSE if not possible (for instance because the SVG file is not yet generated).
bool DotFilePatcher::writeSVGFigureLink(FTextStream &out,const QCString &relPath,
                        const QCString &baseName,const QCString &absImgName)
{
  int width=600,height=600;
  if (!readSVGSize(absImgName,&width,&height))
  {
    return FALSE;
  }
  if (width==-1)
  {
    if (height<=60) height=300; else height+=300; // add some extra space for zooming
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

bool DotFilePatcher::writeVecGfxFigure(FTextStream &out,const QCString &baseName,
                                 const QCString &figureName)
{
  int width=400,height=550;
  if (Config_getBool(USE_PDFLATEX))
  {
    if (!DotRunner::readBoundingBox(figureName+".pdf",&width,&height,FALSE))
    {
      //printf("writeVecGfxFigure()=0\n");
      return FALSE;
    }
  }
  else
  {
    if (!DotRunner::readBoundingBox(figureName+".eps",&width,&height,TRUE))
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

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
#include "config.h"
#include "message.h"
#include "docparser.h"
#include "docnode.h"
#include "doxygen.h"
#include "util.h"
#include "dot.h"
#include "dir.h"
#include "portable.h"

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

static QCString replaceRef(const QCString &buf,const QCString &relPath,
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
      if (link.startsWith("\\ref ") || link.startsWith("@ref ")) // \ref url
      {
        result=href+"=\"";
        // fake ref node to resolve the url
        auto parser { createDocParser() };
        auto dfAst  { createRef( *parser.get(), link.mid(5), context ) };
        auto dfAstImpl = dynamic_cast<const DocNodeAST*>(dfAst.get());
        const DocRef *df = std::get_if<DocRef>(&dfAstImpl->root);
        result+=externalRef(relPath,df->ref(),TRUE);
        if (!df->file().isEmpty())
          result += addHtmlExtensionIfMissing(df->file());
        if (!df->anchor().isEmpty())
          result += "#" + df->anchor();
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
    //    qPrint(buf),qPrint(leftPart),qPrint(result),qPrint(rightPart));
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
bool DotFilePatcher::convertMapFile(TextStream &t,const QCString &mapName,
                    const QCString &relPath, bool urlOnly,
                    const QCString &context)
{
  std::ifstream f = Portable::openInputStream(mapName);
  if (!f.is_open())
  {
    err("problems opening map file %s for inclusion in the docs!\n"
      "If you installed Graphviz/dot after a previous failing run, \n"
      "try deleting the output directory and rerun doxygen.\n",qPrint(mapName));
    return FALSE;
  }
  std::string line;
  while (getline(f,line)) // foreach line
  {
    QCString buf = line+'\n';
    if (buf.startsWith("<area"))
    {
      QCString replBuf = replaceRef(buf,relPath,urlOnly,context);
      // in dot version 7.0.2 the alt attribute is, incorrectly, removed.
      // see https://gitlab.com/graphviz/graphviz/-/issues/265
      int indexA = replBuf.find("alt=");
      if (indexA == -1)
      {
        replBuf = replBuf.left(5) + " alt=\"\"" + replBuf.right(replBuf.length() - 5);
      }

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
  return TRUE;
}

DotFilePatcher::DotFilePatcher(const QCString &patchFile)
  : m_patchFile(patchFile)
{
}

bool DotFilePatcher::isSVGFile() const
{
  return m_patchFile.endsWith(".svg");
}

int DotFilePatcher::addMap(const QCString &mapFile,const QCString &relPath,
                           bool urlOnly,const QCString &context,const QCString &label)
{
  size_t id = m_maps.size();
  m_maps.emplace_back(mapFile,relPath,urlOnly,context,label);
  return static_cast<int>(id);
}

int DotFilePatcher::addFigure(const QCString &baseName,
                              const QCString &figureName,bool heightCheck)
{
  size_t id = m_maps.size();
  m_maps.emplace_back(figureName,"",heightCheck,"",baseName);
  return static_cast<int>(id);
}

int DotFilePatcher::addSVGConversion(const QCString &relPath,bool urlOnly,
                                     const QCString &context,bool zoomable,
                                     int graphId)
{
  size_t id = m_maps.size();
  m_maps.emplace_back("",relPath,urlOnly,context,"",zoomable,graphId);
  return static_cast<int>(id);
}

int DotFilePatcher::addSVGObject(const QCString &baseName,
                                 const QCString &absImgName,
                                 const QCString &relPath)
{
  size_t id = m_maps.size();
  m_maps.emplace_back(absImgName,relPath,false,"",baseName);
  return static_cast<int>(id);
}

bool DotFilePatcher::run() const
{
  //printf("DotFilePatcher::run(): %s\n",qPrint(m_patchFile));
  bool interactiveSVG_local = Config_getBool(INTERACTIVE_SVG);
  bool isSVGFile = m_patchFile.endsWith(".svg");
  int graphId = -1;
  QCString relPath;
  if (isSVGFile)
  {
    const Map &map = m_maps.front(); // there is only one 'map' for a SVG file
    interactiveSVG_local = interactiveSVG_local && map.zoomable;
    graphId = map.graphId;
    relPath = map.relPath;
    //printf("DotFilePatcher::addSVGConversion: file=%s zoomable=%d\n",
    //    qPrint(m_patchFile),map->zoomable);
  }
  QCString tmpName = m_patchFile+".tmp";
  Dir thisDir;
  if (!thisDir.rename(m_patchFile.str(),tmpName.str()))
  {
    err("Failed to rename file %s to %s!\n",qPrint(m_patchFile),qPrint(tmpName));
    return FALSE;
  }
  std::ifstream fi = Portable::openInputStream(tmpName);
  std::ofstream fo = Portable::openOutputStream(m_patchFile);
  if (!fi.is_open())
  {
    err("problem opening file %s for patching!\n",qPrint(tmpName));
    thisDir.rename(tmpName.str(),m_patchFile.str());
    return FALSE;
  }
  if (!fo.is_open())
  {
    err("problem opening file %s for patching!\n",qPrint(m_patchFile));
    thisDir.rename(tmpName.str(),m_patchFile.str());
    return FALSE;
  }
  TextStream t(&fo);
  int width=0,height=0;
  bool insideHeader=FALSE;
  bool replacedHeader=FALSE;
  bool foundSize=FALSE;
  std::string lineStr;
  static const reg::Ex reSVG(R"([\[<]!-- SVG [0-9]+)");
  static const reg::Ex reMAP(R"(<!-- MAP [0-9]+)");
  static const reg::Ex reFIG(R"(% FIG [0-9]+)");

  while (getline(fi,lineStr))
  {
    QCString line = lineStr+'\n';
    //printf("line=[%s]\n",qPrint(line.stripWhiteSpace()));
    int i;
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
        else if (insideHeader && !replacedHeader && line.find("<g id=\"graph")!=-1)
        {
          line="";
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
    else if (line.find("SVG")!=-1 && (i=findIndex(line.str(),reSVG))!=-1)
    {
      //printf("Found marker at %d\n",i);
      int mapId=-1;
      t << line.left(i);
      int n = sscanf(line.data()+i+1,"!-- SVG %d",&mapId);
      if (n==1 && mapId>=0 && mapId<static_cast<int>(m_maps.size()))
      {
        int e = std::max(line.find("--]"),line.find("-->"));
        const Map &map = m_maps.at(mapId);
        //printf("DotFilePatcher::writeSVGFigure: file=%s zoomable=%d\n",
        //  qPrint(m_patchFile),map.zoomable);
        if (!writeSVGFigureLink(t,map.relPath,map.label,map.mapFile))
        {
          err("Problem extracting size from SVG file %s\n",qPrint(map.mapFile));
        }
        if (e!=-1) t << line.mid(e+3);
      }
      else // error invalid map id!
      {
        err("Found invalid SVG id in file %s!\n",qPrint(m_patchFile));
        t << line.mid(i);
      }
    }
    else if (line.find("MAP")!=-1 && (i=findIndex(line.str(),reMAP))!=-1)
    {
      int mapId=-1;
      t << line.left(i);
      int n = sscanf(line.data()+i,"<!-- MAP %d",&mapId);
      if (n==1 && mapId>=0 && mapId<static_cast<int>(m_maps.size()))
      {
        TextStream tt;
        const Map &map = m_maps.at(mapId);
        //printf("patching MAP %d in file %s with contents of %s\n",
        //   mapId,qPrint(m_patchFile),qPrint(map.mapFile));
        convertMapFile(tt,map.mapFile,map.relPath,map.urlOnly,map.context);
        if (!tt.empty())
        {
          t << "<map name=\"" << correctId(map.label) << "\" id=\"" << correctId(map.label) << "\">\n";
          t << tt.str();
          t << "</map>\n";
        }
      }
      else // error invalid map id!
      {
        err("Found invalid MAP id in file %s!\n",qPrint(m_patchFile));
        t << line.mid(i);
      }
    }
    else if (line.find("FIG")!=-1 && (i=findIndex(line.str(),reFIG))!=-1)
    {
      int mapId=-1;
      int n = sscanf(line.data()+i+2,"FIG %d",&mapId);
      //printf("line='%s' n=%d\n",qPrint(line)+i,n);
      if (n==1 && mapId>=0 && mapId<static_cast<int>(m_maps.size()))
      {
        const Map &map = m_maps.at(mapId);
        //printf("patching FIG %d in file %s with contents of %s\n",
        //   mapId,qPrint(m_patchFile),qPrint(map.mapFile));
        if (!writeVecGfxFigure(t,map.label,map.mapFile))
        {
          err("problem writing FIG %d figure!\n",mapId);
          return FALSE;
        }
      }
      else // error invalid map id!
      {
        err("Found invalid bounding FIG %d in file %s!\n",mapId,qPrint(m_patchFile));
        t << line;
      }
    }
    else
    {
      t << line;
    }
  }
  fi.close();
  if (isSVGFile && interactiveSVG_local && replacedHeader)
  {
    QCString orgName=m_patchFile.left(m_patchFile.length()-4)+"_org.svg";
    t << substitute(svgZoomFooter,"$orgname",stripPath(orgName));
    t.flush();
    fo.close();
    // keep original SVG file so we can refer to it, we do need to replace
    // dummy link by real ones
    fi = Portable::openInputStream(tmpName);
    fo = Portable::openOutputStream(orgName);
    if (!fi.is_open())
    {
      err("problem opening file %s for reading!\n",qPrint(tmpName));
      return FALSE;
    }
    if (!fo.is_open())
    {
      err("problem opening file %s for writing!\n",qPrint(orgName));
      return FALSE;
    }
    t.setStream(&fo);
    while (getline(fi,lineStr)) // foreach line
    {
      std::string line = lineStr+'\n';
      const Map &map = m_maps.front(); // there is only one 'map' for a SVG file
      t << replaceRef(line.c_str(),map.relPath,map.urlOnly,map.context,"_top");
    }
    t.flush();
    fi.close();
    fo.close();
  }
  // remove temporary file
  thisDir.remove(tmpName.str());
  return TRUE;
}

//---------------------------------------------------------------------------------------------


// extract size from a dot generated SVG file
static bool readSVGSize(const QCString &fileName,int *width,int *height)
{
  bool found=FALSE;
  std::ifstream f = Portable::openInputStream(fileName);
  if (!f.is_open())
  {
    return false;
  }
  std::string line;
  while (getline(f,line) && !found)
  {
    if (qstrncmp(line.c_str(),"<!--zoomable ",13)==0)
    {
      *width=-1;
      *height=-1;
      sscanf(line.c_str(),"<!--zoomable %d",height);
      found=true;
    }
    else if (sscanf(line.c_str(),"<svg width=\"%dpt\" height=\"%dpt\"",width,height)==2)
    {
      found=true;
    }
  }
  return true;
}

static void writeSVGNotSupported(TextStream &out)
{
  out << "<p><b>This browser is not able to show SVG: try Firefox, Chrome, Safari, or Opera instead.</b></p>";
}

/// Check if a reference to a SVG figure can be written and do so if possible.
/// Returns FALSE if not possible (for instance because the SVG file is not yet generated).
bool DotFilePatcher::writeSVGFigureLink(TextStream &out,const QCString &relPath,
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

bool DotFilePatcher::writeVecGfxFigure(TextStream &out,const QCString &baseName,
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

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

#include "config.h"
#include "doxygen.h"
#include "index.h"
#include "md5.h"
#include "message.h"
#include "util.h"

#include "dot.h"
#include "dotrunner.h"
#include "dotgraph.h"
#include "dotnode.h"

#define MAP_CMD "cmapx"

QCString DotGraph::DOT_FONTNAME; // will be initialized in initDot
int DotGraph::DOT_FONTSIZE;      // will be initialized in initDot

/*! Checks if a file "baseName".md5 exists. If so the contents
*  are compared with \a md5. If equal FALSE is returned.
*  The .md5 is created or updated after successful creation of the output file.
*/
static bool checkMd5Signature(const QCString &baseName,
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

static void removeDotGraph(const QCString &dotName)
{
  if (Config_getBool(DOT_CLEANUP))
  {
    QDir d;
    d.remove(dotName);
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
    convertMapFile(tmpout,mapFile,relPath,FALSE);
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

//--------------------------------------------------------------------

QCString DotGraph::IMG_EXT;

QCString DotGraph::imgName() const
{
  return m_baseName + ((m_graphFormat == GOF_BITMAP) ?
                      ("." + IMG_EXT) : (Config_getBool(USE_PDFLATEX) ? ".pdf" : ".eps")); 
}

QCString DotGraph::writeGraph(
        FTextStream& t,           // output stream for the code file (html, ...)
        GraphOutputFormat gf,     // bitmap(png/svg) or ps(eps/pdf)
        EmbeddedOutputFormat ef,  // html, latex, ...
        const char* path,         // output folder
        const char* fileName,     // name of the code file (for code patcher)
        const char* relPath,      // output folder relativ to code file
        bool generateImageMap,    // in case of bitmap, shall there be code generated?
        int graphId)              // number of this graph in the current code, used in svg code
{
  m_graphFormat = gf;
  m_textFormat = ef;
  m_dir = QDir(path);
  m_fileName = fileName;
  m_relPath = relPath;
  m_generateImageMap = generateImageMap;
  m_graphId = graphId;

  m_absPath  = QCString(m_dir.absPath().data()) + "/";
  m_baseName = getBaseName();

  computeTheGraph();

  m_regenerate = prepareDotFile();

  if (!m_doNotAddImageToIndex) Doxygen::indexList->addImageFile(imgName());

  generateCode(t);

  return m_baseName;
}

bool DotGraph::prepareDotFile()
{
  if (!m_dir.exists())
  {
    err("Output dir %s does not exist!\n", m_dir.path().data()); exit(1);
  }

  QCString sigStr(33);
  uchar md5_sig[16];
  // calculate md5
  MD5Buffer((const unsigned char*)m_theGraph.data(), m_theGraph.length(), md5_sig);
  // convert result to a string
  MD5SigToString(md5_sig, sigStr.rawData(), 33);

  // already queued files are processed again in case the output format has changed

  if (!checkMd5Signature(absBaseName(), sigStr) &&
      checkDeliverables(absImgName(),
                        m_graphFormat == GOF_BITMAP && m_generateImageMap ? absMapName() : QCString()
                       )
     )
  {
    // all needed files are there
    removeDotGraph(absDotName());
    return FALSE;
  }

  // need to rebuild the image

  // write .dot file because image was new or has changed
  QFile f(absDotName());
  if (!f.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",f.name().data());
    return TRUE;
  }
  FTextStream t(&f);
  t << m_theGraph;
  f.close();

  if (m_graphFormat == GOF_BITMAP)
  {
    // run dot to create a bitmap image
    DotRunner * dotRun = DotManager::instance()->createRunner(absDotName(), sigStr);
    dotRun->addJob(Config_getEnum(DOT_IMAGE_FORMAT), absImgName());
    if (m_generateImageMap) dotRun->addJob(MAP_CMD, absMapName());
  }
  else if (m_graphFormat == GOF_EPS)
  {
    // run dot to create a .eps image
    DotRunner *dotRun = DotManager::instance()->createRunner(absDotName(), sigStr);
    if (Config_getBool(USE_PDFLATEX))
    {
      dotRun->addJob("pdf",absImgName());
    }
    else
    {
      dotRun->addJob("ps",absImgName());
    }
  }
  return TRUE;
}

void DotGraph::generateCode(FTextStream &t)
{
  if (m_graphFormat==GOF_BITMAP && m_textFormat==EOF_DocBook)
  {
    t << "<para>" << endl;
    t << "    <informalfigure>" << endl;
    t << "        <mediaobject>" << endl;
    t << "            <imageobject>" << endl;
    t << "                <imagedata";
    t << " width=\"50%\" align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << m_relPath << m_baseName << "." << IMG_EXT << "\">";
    t << "</imagedata>" << endl;
    t << "            </imageobject>" << endl;
    t << "        </mediaobject>" << endl;
    t << "    </informalfigure>" << endl;
    t << "</para>" << endl;
  }
  else if (m_graphFormat==GOF_BITMAP && m_generateImageMap) // produce HTML to include the image
  {
    if (IMG_EXT=="svg") // add link to SVG file without map file
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      if (m_regenerate || !writeSVGFigureLink(t,m_relPath,m_baseName,absImgName())) // need to patch the links in the generated SVG file
      {
        if (m_regenerate)
        {
          DotManager::instance()->addSVGConversion(absImgName(),m_relPath,FALSE,QCString(),m_zoomable,m_graphId);
        }
        int mapId = DotManager::instance()->addSVGObject(m_fileName,m_baseName,absImgName(),m_relPath);
        t << "<!-- SVG " << mapId << " -->" << endl;
      }
      if (!m_noDivTag) t << "</div>" << endl;
    }
    else // add link to bitmap file with image map
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      t << "<img src=\"" << relImgName() << "\" border=\"0\" usemap=\"#" << getMapLabel() << "\" alt=\"" << getImgAltText() << "\"/>";
      if (!m_noDivTag) t << "</div>";
      t << endl;
      if (m_regenerate || !insertMapFile(t, absMapName(), m_relPath, getMapLabel()))
      {
        int mapId = DotManager::instance()->addMap(m_fileName, absMapName(), m_relPath, m_urlOnly, QCString(), getMapLabel());
        t << "<!-- MAP " << mapId << " -->" << endl;
      }
    }
  }
  else if (m_graphFormat==GOF_EPS) // produce tex to include the .eps image
  {
    if (m_regenerate || !writeVecGfxFigure(t,m_baseName,absBaseName()))
    {
      int figId = DotManager::instance()->addFigure(m_fileName,m_baseName,absBaseName(),FALSE /*TRUE*/);
      t << endl << "% FIG " << figId << endl;
    }
  }
}

void DotGraph::writeGraphHeader(FTextStream &t,const QCString &title)
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
  t << endl << "{" << endl;
  if (Config_getBool(INTERACTIVE_SVG)) // insert a comment to force regeneration when this
                       // option is toggled
  {
    t << " // INTERACTIVE_SVG=YES\n";
  }
  t << " // LATEX_PDF_SIZE\n"; // write placeholder for LaTeX PDF bounding box size repacement
  if (Config_getBool(DOT_TRANSPARENT))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << DOT_FONTNAME << "\","
         "fontsize=\"" << DOT_FONTSIZE << "\","
         "labelfontname=\"" << DOT_FONTNAME << "\","
         "labelfontsize=\"" << DOT_FONTSIZE << "\"];\n";
  t << "  node [fontname=\"" << DOT_FONTNAME << "\","
         "fontsize=\"" << DOT_FONTSIZE << "\",shape=record];\n";
}

void DotGraph::writeGraphFooter(FTextStream &t)
{
  t << "}" << endl;
}

void DotGraph::computeGraph(DotNode *root,
                            GraphType gt,
                            GraphOutputFormat format,
                            const QCString &rank, // either "LR", "RL", or ""
                            bool renderParents,
                            bool backArrows,
                            const QCString &title,
                            QGString &graphStr)
{
  //printf("computeMd5Signature\n");
  QGString buf;
  FTextStream md5stream(&buf);
  writeGraphHeader(md5stream,title);
  if (!rank.isEmpty())
  {
    md5stream << "  rankdir=\"" << rank << "\";" << endl;
  }
  root->clearWriteFlag();
  root->write(md5stream, gt, format, gt!=CallGraph && gt!=Dependency, TRUE, backArrows);
  if (renderParents && root->parents())
  {
    QListIterator<DotNode>  dnli(*root->parents());
    const DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (pn->isVisible())
      {
        root->writeArrow(md5stream,                              // stream
            gt,                                                  // graph type
            format,                                              // output format
            pn,                                                  // child node
            pn->edgeInfo()->at(pn->children()->findRef(root)),   // edge info
            FALSE,                                               // topDown?
            backArrows                                           // point back?
          );
      }
      pn->write(md5stream,      // stream
                gt,             // graph type
                format,         // output format
                TRUE,           // topDown?
                FALSE,          // toChildren?
                backArrows      // backward pointing arrows?
      );
    }
  }
  writeGraphFooter(md5stream);

  graphStr=buf.data();
}

bool DotGraph::writeVecGfxFigure(FTextStream &out,const QCString &baseName,
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

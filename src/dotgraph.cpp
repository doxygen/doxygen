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

#include <sstream>

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
#include "dotfilepatcher.h"
#include "fileinfo.h"

#define MAP_CMD "cmapx"

//QCString DotGraph::DOT_FONTNAME; // will be initialized in initDot
//int DotGraph::DOT_FONTSIZE;      // will be initialized in initDot

/*! Checks if a file "baseName".md5 exists. If so the contents
*  are compared with \a md5. If equal FALSE is returned.
*  The .md5 is created or updated after successful creation of the output file.
*/
static bool checkMd5Signature(const QCString &baseName,
                              const QCString &md5)
{
  std::ifstream f(baseName.str()+".md5",std::ifstream::in | std::ifstream::binary);
  if (f.is_open())
  {
    // read checksum
    QCString md5stored(33);
    f.read(md5stored.rawData(),32);
    md5stored[32]='\0';
    // compare checksum
    if (!f.fail() && md5==md5stored)
    {
      // bail out if equal
      return false;
    }
  }
  return true;
}

static bool checkDeliverables(const QCString &file1,
                              const QCString &file2=QCString())
{
  bool file1Ok = TRUE;
  bool file2Ok = TRUE;
  if (!file1.isEmpty())
  {
    FileInfo fi(file1.str());
    file1Ok = (fi.exists() && fi.size()>0);
  }
  if (!file2.isEmpty())
  {
    FileInfo fi(file2.str());
    file2Ok = (fi.exists() && fi.size()>0);
  }
  return file1Ok && file2Ok;
}

static bool insertMapFile(std::ostream &out,const QCString &mapFile,
                          const QCString &relPath,const QCString &mapLabel)
{
  FileInfo fi(mapFile.str());
  if (fi.exists() && fi.size()>0) // reuse existing map file
  {
    std::stringstream t;
    DotFilePatcher::convertMapFile(t,mapFile,relPath,false);
    if (t.tellg()>0)
    {
      out << "<map name=\"" << mapLabel << "\" id=\"" << mapLabel << "\">\n";
      out << t.str();
      out << "</map>\n";
    }
    return true;
  }
  return false; // no map file yet, need to generate it
}

//--------------------------------------------------------------------

QCString DotGraph::imgName() const
{
  return m_baseName + ((m_graphFormat == GOF_BITMAP) ?
                      ("." + getDotImageExtension()) : (Config_getBool(USE_PDFLATEX) ? ".pdf" : ".eps"));
}

QCString DotGraph::writeGraph(
        std::ostream& t,           // output stream for the code file (html, ...)
        GraphOutputFormat gf,     // bitmap(png/svg) or ps(eps/pdf)
        EmbeddedOutputFormat ef,  // html, latex, ...
        const char* path,         // output folder
        const char* fileName,     // name of the code file (for code patcher)
        const char* relPath,      // output folder relative to code file
        bool generateImageMap,    // in case of bitmap, shall there be code generated?
        int graphId)              // number of this graph in the current code, used in svg code
{
  m_graphFormat = gf;
  m_textFormat = ef;
  m_dir = Dir(path);
  m_fileName = fileName;
  m_relPath = relPath;
  m_generateImageMap = generateImageMap;
  m_graphId = graphId;

  m_absPath  = m_dir.absPath() + "/";
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
    term("Output dir %s does not exist!\n", m_dir.path().c_str());
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
    return FALSE;
  }

  // need to rebuild the image

  // write .dot file because image was new or has changed
  std::ofstream f(absDotName().str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Could not open file %s for writing\n",absDotName().data());
    return TRUE;
  }
  f << m_theGraph;
  f.close();

  if (m_graphFormat == GOF_BITMAP)
  {
    // run dot to create a bitmap image
    DotRunner * dotRun = DotManager::instance()->createRunner(absDotName().data(), sigStr.data());
    dotRun->addJob(Config_getEnum(DOT_IMAGE_FORMAT), absImgName());
    if (m_generateImageMap) dotRun->addJob(MAP_CMD, absMapName());
  }
  else if (m_graphFormat == GOF_EPS)
  {
    // run dot to create a .eps image
    DotRunner *dotRun = DotManager::instance()->createRunner(absDotName().data(), sigStr.data());
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

void DotGraph::generateCode(std::ostream &t)
{
  QCString imgExt = getDotImageExtension();
  if (m_graphFormat==GOF_BITMAP && m_textFormat==EOF_DocBook)
  {
    t << "<para>\n";
    t << "    <informalfigure>\n";
    t << "        <mediaobject>\n";
    t << "            <imageobject>\n";
    t << "                <imagedata";
    t << " width=\"50%\" align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" << m_relPath << m_baseName << "." << imgExt << "\">";
    t << "</imagedata>\n";
    t << "            </imageobject>\n";
    t << "        </mediaobject>\n";
    t << "    </informalfigure>\n";
    t << "</para>\n";
  }
  else if (m_graphFormat==GOF_BITMAP && m_generateImageMap) // produce HTML to include the image
  {
    if (imgExt=="svg") // add link to SVG file without map file
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      if (m_regenerate || !DotFilePatcher::writeSVGFigureLink(t,m_relPath,m_baseName,absImgName())) // need to patch the links in the generated SVG file
      {
        if (m_regenerate)
        {
          DotManager::instance()->
               createFilePatcher(absImgName().data())->
               addSVGConversion(m_relPath,FALSE,QCString(),m_zoomable,m_graphId);
        }
        int mapId = DotManager::instance()->
               createFilePatcher(m_fileName.data())->
               addSVGObject(m_baseName,absImgName(),m_relPath);
        t << "<!-- SVG " << mapId << " -->\n";
      }
      if (!m_noDivTag) t << "</div>\n";
    }
    else // add link to bitmap file with image map
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      t << "<img src=\"" << relImgName() << "\" border=\"0\" usemap=\"#" << correctId(getMapLabel()) << "\" alt=\"" << getImgAltText() << "\"/>";
      if (!m_noDivTag) t << "</div>";
      t << "\n";
      if (m_regenerate || !insertMapFile(t, absMapName(), m_relPath, correctId(getMapLabel())))
      {
        int mapId = DotManager::instance()->
          createFilePatcher(m_fileName.data())->
          addMap(absMapName(), m_relPath, m_urlOnly, QCString(), getMapLabel());
        t << "<!-- MAP " << mapId << " -->\n";
      }
    }
  }
  else if (m_graphFormat==GOF_EPS) // produce tex to include the .eps image
  {
    if (m_regenerate || !DotFilePatcher::writeVecGfxFigure(t,m_baseName,absBaseName()))
    {
      int figId = DotManager::instance()->
                  createFilePatcher(m_fileName.data())->
                  addFigure(m_baseName,absBaseName(),FALSE /*TRUE*/);
      t << "\n% FIG " << figId << "\n";
    }
  }
}

void DotGraph::writeGraphHeader(std::ostream &t,const QCString &title)
{
  int fontSize      = Config_getInt(DOT_FONTSIZE);
  QCString fontName = Config_getString(DOT_FONTNAME);
  t << "digraph ";
  if (title.isEmpty())
  {
    t << "\"Dot Graph\"";
  }
  else
  {
    t << "\"" << convertToXML(title) << "\"";
  }
  t << "\n{\n";
  if (Config_getBool(INTERACTIVE_SVG)) // insert a comment to force regeneration when this
                       // option is toggled
  {
    t << " // INTERACTIVE_SVG=YES\n";
  }
  t << " // LATEX_PDF_SIZE\n"; // write placeholder for LaTeX PDF bounding box size replacement
  if (Config_getBool(DOT_TRANSPARENT))
  {
    t << "  bgcolor=\"transparent\";\n";
  }
  t << "  edge [fontname=\"" << fontName << "\","
         "fontsize=\"" << fontSize << "\","
         "labelfontname=\"" << fontName << "\","
         "labelfontsize=\"" << fontSize << "\"];\n";
  t << "  node [fontname=\"" << fontName << "\","
         "fontsize=\"" << fontSize << "\",shape=record];\n";
}

void DotGraph::writeGraphFooter(std::ostream &t)
{
  t << "}\n";
}

void DotGraph::computeGraph(DotNode *root,
                            GraphType gt,
                            GraphOutputFormat format,
                            const QCString &rank, // either "LR", "RL", or ""
                            bool renderParents,
                            bool backArrows,
                            const QCString &title,
                            QCString &graphStr)
{
  //printf("computeMd5Signature\n");
  std::stringstream md5stream;
  writeGraphHeader(md5stream,title);
  if (!rank.isEmpty())
  {
    md5stream << "  rankdir=\"" << rank << "\";\n";
  }
  root->clearWriteFlag();
  root->write(md5stream, gt, format, gt!=CallGraph && gt!=Dependency, TRUE, backArrows);
  if (renderParents)
  {
    for (const auto &pn : root->parents())
    {
      if (pn->isVisible())
      {
        const auto &children = pn->children();
        auto child_it = std::find(children.begin(),children.end(),root);
        int index = child_it - children.begin();
        root->writeArrow(md5stream,                              // stream
            gt,                                                  // graph type
            format,                                              // output format
            pn,                                                  // child node
            &pn->edgeInfo()[index],                              // edge info
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

  graphStr=md5stream.str();
}


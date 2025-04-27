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

#include <mutex>
#include <regex>

#include "config.h"
#include "doxygen.h"
#include "indexlist.h"
#include "md5.h"
#include "message.h"
#include "util.h"

#include "dot.h"
#include "dotrunner.h"
#include "dotgraph.h"
#include "dotnode.h"
#include "dotfilepatcher.h"
#include "fileinfo.h"
#include "portable.h"

#define MAP_CMD "cmapx"

//QCString DotGraph::DOT_FONTNAME; // will be initialized in initDot
//int DotGraph::DOT_FONTSIZE;      // will be initialized in initDot

/*! Checks if a file "baseName".md5 exists. If so the contents
*  are compared with \a md5. If equal FALSE is returned.
*  The .md5 is created or updated after successful creation of the output file.
*/
static bool sameMd5Signature(const QCString &baseName,
                             const QCString &md5)
{
  bool same = false;
  char md5stored[33];
  md5stored[0]=0;
  std::ifstream f = Portable::openInputStream(baseName+".md5",true);
  if (f.is_open())
  {
    // read checksum
    f.read(md5stored,32);
    md5stored[32]='\0';
    // compare checksum
    if (!f.fail() && md5==md5stored)
    {
      same = true;
    }
    //printf("sameSignature(%s,%s==%s)=%d\n",qPrint(baseName),md5stored,qPrint(md5),same);
  }
  else
  {
    //printf("sameSignature(%s) not found\n",qPrint(baseName));
  }
  return same;
}

static bool deliverablesPresent(const QCString &file1,const QCString &file2)
{
  bool file1Ok = true;
  bool file2Ok = true;
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

static bool insertMapFile(TextStream &out,const QCString &mapFile,
                          const QCString &relPath,const QCString &mapLabel)
{
  FileInfo fi(mapFile.str());
  if (fi.exists() && fi.size()>0) // reuse existing map file
  {
    TextStream t;
    DotFilePatcher::convertMapFile(t,mapFile,relPath,false);
    if (!t.empty())
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
  return m_baseName + ((m_graphFormat == GraphOutputFormat::BITMAP) ?
                      ("." + getDotImageExtension()) : (Config_getBool(USE_PDFLATEX) ? ".pdf" : ".eps"));
}

std::mutex g_dotIndexListMutex;

QCString DotGraph::writeGraph(
        TextStream& t,            // output stream for the code file (html, ...)
        GraphOutputFormat gf,     // bitmap(png/svg) or ps(eps/pdf)
        EmbeddedOutputFormat ef,  // html, latex, ...
        const QCString &path,     // output folder
        const QCString &fileName, // name of the code file (for code patcher)
        const QCString &relPath,  // output folder relative to code file
        bool generateImageMap,    // in case of bitmap, shall there be code generated?
        int graphId)              // number of this graph in the current code, used in svg code
{
  m_graphFormat = gf;
  m_textFormat = ef;
  m_dir = Dir(path.str());
  m_fileName = fileName;
  m_relPath = relPath;
  m_generateImageMap = generateImageMap;
  m_graphId = graphId;

  m_absPath  = m_dir.absPath() + "/";
  m_baseName = getBaseName();

  computeTheGraph();

  m_regenerate = prepareDotFile();

  if (!m_doNotAddImageToIndex)
  {
    std::lock_guard<std::mutex> lock(g_dotIndexListMutex);
    Doxygen::indexList->addImageFile(imgName());
  }

  generateCode(t);

  return m_baseName;
}

bool DotGraph::prepareDotFile()
{
  if (!m_dir.exists())
  {
    term("Output dir {} does not exist!\n", m_dir.path());
  }

  char sigStr[33];
  uint8_t md5_sig[16];
  // calculate md5
  MD5Buffer(m_theGraph.data(), static_cast<unsigned int>(m_theGraph.length()), md5_sig);
  // convert result to a string
  MD5SigToString(md5_sig, sigStr);

  // already queued files are processed again in case the output format has changed

  if (sameMd5Signature(absBaseName(), sigStr) &&
      deliverablesPresent(absImgName(),
                          m_graphFormat == GraphOutputFormat::BITMAP && m_generateImageMap ? absMapName() : QCString()
                         )
     )
  {
    // all needed files are there
    return FALSE;
  }

  // need to rebuild the image

  // write .dot file because image was new or has changed
  std::ofstream f = Portable::openOutputStream(absDotName());
  if (!f.is_open())
  {
    err("Could not open file {} for writing\n",absDotName());
    return TRUE;
  }
  f << m_theGraph;
  f.close();

  if (m_graphFormat == GraphOutputFormat::BITMAP)
  {
    // run dot to create a bitmap image
    DotRunner * dotRun = DotManager::instance()->createRunner(absDotName(), sigStr);
    dotRun->addJob(Config_getEnumAsString(DOT_IMAGE_FORMAT), absImgName(), absDotName(), 1);
    if (m_generateImageMap) dotRun->addJob(MAP_CMD, absMapName(), absDotName(), 1);
  }
  else if (m_graphFormat == GraphOutputFormat::EPS)
  {
    // run dot to create a .eps image
    DotRunner *dotRun = DotManager::instance()->createRunner(absDotName(), sigStr);
    if (Config_getBool(USE_PDFLATEX))
    {
      dotRun->addJob("pdf",absImgName(),absDotName(),1);
    }
    else
    {
      dotRun->addJob("ps",absImgName(),absDotName(),1);
    }
  }
  return TRUE;
}

void DotGraph::generateCode(TextStream &t)
{
  QCString imgExt = getDotImageExtension();
  if (m_graphFormat==GraphOutputFormat::BITMAP && m_textFormat==EmbeddedOutputFormat::DocBook)
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
  else if (m_graphFormat==GraphOutputFormat::BITMAP && m_generateImageMap) // produce HTML to include the image
  {
    if (imgExt=="svg") // add link to SVG file without map file
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      if (m_regenerate || !DotFilePatcher::writeSVGFigureLink(t,m_relPath,m_baseName,absImgName())) // need to patch the links in the generated SVG file
      {
        if (m_regenerate)
        {
          DotManager::instance()->
               createFilePatcher(absImgName())->
               addSVGConversion(m_relPath,FALSE,QCString(),m_zoomable,m_graphId);
        }
        int mapId = DotManager::instance()->
               createFilePatcher(m_fileName)->
               addSVGObject(m_baseName,absImgName(),m_relPath);
        t << "<!-- " << "SVG " << mapId << " -->";
      }
      if (!m_noDivTag) t << "</div>\n";
    }
    else // add link to bitmap file with image map
    {
      if (!m_noDivTag) t << "<div class=\"center\">";
      t << "<img src=\"" << relImgName() << "\" border=\"0\" usemap=\"#" << correctId(getMapLabel()) << "\" loading=\"lazy\" alt=\"" << getImgAltText() << "\"/>";
      if (!m_noDivTag) t << "</div>";
      t << "\n";
      if (m_regenerate || !insertMapFile(t, absMapName(), m_relPath, correctId(getMapLabel())))
      {
        int mapId = DotManager::instance()->
          createFilePatcher(m_fileName)->
          addMap(absMapName(), m_relPath, m_urlOnly, QCString(), getMapLabel());
        t << "<!-- MAP " << mapId << " -->\n";
      }
    }
  }
  else if (m_graphFormat==GraphOutputFormat::EPS) // produce tex to include the .eps image
  {
    if (m_regenerate || !DotFilePatcher::writeVecGfxFigure(t,m_baseName,absBaseName()))
    {
      int figId = DotManager::instance()->
                  createFilePatcher(m_fileName)->
                  addFigure(m_baseName,absBaseName(),FALSE /*TRUE*/);
      t << "\n% FIG " << figId << "\n";
    }
  }
}

void DotGraph::writeGraphHeader(TextStream &t,const QCString &title)
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
  t << "\n{\n";
  if (Config_getBool(INTERACTIVE_SVG)) // insert a comment to force regeneration when this
                       // option is toggled
  {
    t << " // INTERACTIVE_SVG=YES\n";
  }
  t << " // LATEX_PDF_SIZE\n"; // write placeholder for LaTeX PDF bounding box size replacement
  t << "  bgcolor=\"transparent\";\n";
  QCString c = Config_getString(DOT_COMMON_ATTR);
  if (!c.isEmpty()) c += ",";
  t << "  edge [" << c << Config_getString(DOT_EDGE_ATTR) << "];\n";
  t << "  node [" << c << Config_getString(DOT_NODE_ATTR) << "];\n";
}

void DotGraph::writeGraphFooter(TextStream &t)
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
  TextStream md5stream;
  writeGraphHeader(md5stream,title);
  if (!rank.isEmpty())
  {
    md5stream << "  rankdir=\"" << rank << "\";\n";
  }
  root->clearWriteFlag();
  root->write(md5stream, gt, format, gt!=GraphType::CallGraph && gt!=GraphType::Dependency, TRUE, backArrows);
  if (renderParents)
  {
    for (const auto &pn : root->parents())
    {
      if (pn->isVisible())
      {
        const auto &children = pn->children();
        auto child_it = std::find(children.begin(),children.end(),root);
        size_t index = child_it - children.begin();
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


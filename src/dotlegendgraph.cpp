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

#include "dotlegendgraph.h"
#include "util.h"
#include "config.h"
#include "doxygen.h"
#include "dot.h"
#include "language.h"
#include "dotfilepatcher.h"

void DotLegendGraph::writeGraph(const QCString &path)
{
  TextStream ts;
  DotGraph::writeGraph(ts, GOF_BITMAP, EOF_Html, path, "", "", FALSE, 0);

  if (getDotImageExtension()=="svg")
  {
    DotManager::instance()->
      createFilePatcher(absBaseName()+Config_getString(HTML_FILE_EXTENSION))->
      addSVGObject("graph_legend", absImgName(),QCString());
  }
}

QCString DotLegendGraph::getBaseName() const
{
  return "graph_legend";
}

void DotLegendGraph::computeTheGraph()
{
  int fontSize = Config_getInt(DOT_FONTSIZE);
  QCString fontName = Config_getString(DOT_FONTNAME);
  TextStream md5stream;
  writeGraphHeader(md5stream,theTranslator->trLegendTitle());
  md5stream << "  node [shape=\"box\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\"];\n";

  md5stream << "  Node9 [label=\"Inherited\",fillcolor=\"grey75\",style=\"filled\" fontcolor=\"black\"];\n";
  md5stream << "  Node10 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node10 [label=\"PublicBase\",color=\"black\"];\n";
  md5stream << "  Node11 -> Node10 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node11 [label=\"Truncated\",color=\"red\"];\n";
  md5stream << "  Node13 -> Node9 [dir=\"back\",color=\"darkgreen\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node13 [label=\"ProtectedBase\",color=\"black\"];\n";
  md5stream << "  Node14 -> Node9 [dir=\"back\",color=\"firebrick4\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node14 [label=\"PrivateBase\",color=\"black\"];\n";
  md5stream << "  Node15 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node15 [label=\"Undocumented\",color=\"grey75\"];\n";
  md5stream << "  Node16 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node16 [label=\"Templ< int >\",color=\"black\"];\n";
  md5stream << "  Node17 -> Node16 [dir=\"back\",color=\"orange\",fontsize=\"" << fontSize << "\",style=\"dashed\",label=\"< int >\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node17 [label=\"Templ< T >\",color=\"black\"];\n";
  md5stream << "  Node18 -> Node9 [dir=\"back\",color=\"darkorchid3\",fontsize=\"" << fontSize << "\",style=\"dashed\",label=\"m_usedClass\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node18 [label=\"Used\",color=\"black\"];\n";
  writeGraphFooter(md5stream);
  m_theGraph = md5stream.str();
}

QCString DotLegendGraph::getMapLabel() const
{
  return "";
}


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

#include "dotlegendgraph.h"
#include "util.h"
#include "config.h"
#include "doxygen.h"
#include "dot.h"
#include "language.h"
#include "dotfilepatcher.h"

void DotLegendGraph::writeGraph(const char *path)
{
  FTextStream ts;
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
  FTextStream md5stream(&m_theGraph);
  writeGraphHeader(md5stream,theTranslator->trLegendTitle());
  md5stream << "  Node9 [shape=\"box\",label=\"Inherited\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",fillcolor=\"grey75\",style=\"filled\" fontcolor=\"black\"];\n";
  md5stream << "  Node10 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node10 [shape=\"box\",label=\"PublicBase\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  md5stream << "  Node11 -> Node10 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node11 [shape=\"box\",label=\"Truncated\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"red\"];\n";
  md5stream << "  Node13 -> Node9 [dir=\"back\",color=\"darkgreen\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node13 [shape=\"box\",label=\"ProtectedBase\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  md5stream << "  Node14 -> Node9 [dir=\"back\",color=\"firebrick4\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node14 [shape=\"box\",label=\"PrivateBase\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  md5stream << "  Node15 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node15 [shape=\"box\",label=\"Undocumented\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"grey75\"];\n";
  md5stream << "  Node16 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << fontSize << "\",style=\"solid\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node16 [shape=\"box\",label=\"Templ< int >\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  md5stream << "  Node17 -> Node16 [dir=\"back\",color=\"orange\",fontsize=\"" << fontSize << "\",style=\"dashed\",label=\"< int >\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node17 [shape=\"box\",label=\"Templ< T >\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  md5stream << "  Node18 -> Node9 [dir=\"back\",color=\"darkorchid3\",fontsize=\"" << fontSize << "\",style=\"dashed\",label=\"m_usedClass\",fontname=\"" << fontName << "\"];\n";
  md5stream << "  Node18 [shape=\"box\",label=\"Used\",fontsize=\"" << fontSize << "\",height=0.2,width=0.4,fontname=\"" << fontName << "\",color=\"black\"];\n";
  writeGraphFooter(md5stream);
}

QCString DotLegendGraph::getMapLabel() const
{
  return "";
}


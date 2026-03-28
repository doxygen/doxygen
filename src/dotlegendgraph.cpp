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
#include "dotnode.h"

void DotLegendGraph::writeGraph(const QCString &path)
{
  TextStream ts;
  DotGraph::writeGraph(ts, GraphOutputFormat::BITMAP, EmbeddedOutputFormat::Html, path, "", "", FALSE, 0);

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
  TextStream md5stream;
  writeGraphHeader(md5stream,theTranslator->trLegendTitle());

  DotNode{this,"Inherited", "", "", TRUE}.setNodeId(9).writeBox(md5stream, GraphType::CallGraph, GraphOutputFormat::BITMAP, false);
  md5stream << "  Node10 -> Node9 [dir=\"back\",color=\"steelblue1\",style=\"solid\" tooltip=\" \"];\n";
  DotNode{this,"PublicBase", "", DotNode::placeholderUrl}.setNodeId(10).markHasDocumentation().writeBox(md5stream, GraphType::CallGraph, GraphOutputFormat::BITMAP, false);
  md5stream << "  Node11 -> Node10 [dir=\"back\",color=\"steelblue1\",style=\"solid\" tooltip=\" \"];\n";
  DotNode{this,"Truncated", "", DotNode::placeholderUrl}.setNodeId(11).markAsTruncated().markHasDocumentation().writeBox(md5stream, GraphType::CallGraph, GraphOutputFormat::BITMAP, true);
  md5stream << "  Node13 -> Node9 [dir=\"back\",color=\"darkgreen\",style=\"solid\" tooltip=\" \"];\n";
  md5stream << "  Node13 [label=\"ProtectedBase\",color=\"gray40\",fillcolor=\"white\",style=\"filled\" tooltip=\" \"];\n";
  md5stream << "  Node14 -> Node9 [dir=\"back\",color=\"firebrick4\",style=\"solid\" tooltip=\" \"];\n";
  md5stream << "  Node14 [label=\"PrivateBase\",color=\"gray40\",fillcolor=\"white\",style=\"filled\" tooltip=\" \"];\n";
  md5stream << "  Node15 -> Node9 [dir=\"back\",color=\"steelblue1\",style=\"solid\" tooltip=\" \"];\n";
  DotNode{this,"Undocumented", "", ""}.setNodeId(15).writeBox(md5stream, GraphType::CallGraph, GraphOutputFormat::BITMAP, false);
  md5stream << "  Node16 -> Node9 [dir=\"back\",color=\"steelblue1\",style=\"solid\" tooltip=\" \"];\n";
  md5stream << "  Node16 [label=\"Templ\\< int \\>\",color=\"gray40\",fillcolor=\"white\",style=\"filled\" tooltip=\" \"];\n";
  md5stream << "  Node17 -> Node16 [dir=\"back\",color=\"orange\",style=\"dashed\",label=\"< int >\",fontcolor=\"grey\" tooltip=\" \"];\n";
  md5stream << "  Node17 [label=\"Templ\\< T \\>\",color=\"gray40\",fillcolor=\"white\",style=\"filled\" tooltip=\" \"];\n";
  md5stream << "  Node18 -> Node9 [dir=\"back\",color=\"darkorchid3\",style=\"dashed\",label=\"m_usedClass\",fontcolor=\"grey\" tooltip=\" \"];\n";
  md5stream << "  Node18 [label=\"Used\",color=\"gray40\",fillcolor=\"white\",style=\"filled\" tooltip=\" \"];\n";
  writeGraphFooter(md5stream);
  m_theGraph = md5stream.str();
}

QCString DotLegendGraph::getMapLabel() const
{
  return "";
}


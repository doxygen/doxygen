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

#include "dotdirdeps.h"
#include "util.h"
#include "doxygen.h"
#include "config.h"

using DirDefMap = std::map<std::string,const DirDef *>;

/**
 * Puts DOT code for drawing directory to stream and adds it to the list.
 * @param[in,out] outStream stream to which the DOT code is written to
 * @param[in] directory will be mapped to a node in DOT code
 * @param[in] fillBackground if the node shall be explicitly filled
 * @param[in,out] directoriesInGraph lists the directories which have been written to the output stream
 */
static void drawDirectory(std::ostream &outStream, const DirDef *const directory, const bool fillBackground,
    DirDefMap &directoriesInGraph)
{
  outStream << "  " << directory->getOutputFileBase() << " [shape=box "
      "label=\"" << directory->shortName() << "\" ";
  if (fillBackground)
  {
    outStream << "fillcolor=\"white\" style=\"filled\" ";
  }
  if (directory->isCluster())
  {
    outStream << "color=\"red\" ";
  }
  outStream << "URL=\"" << directory->getOutputFileBase() << Doxygen::htmlFileExtension << "\"];\n";
  directoriesInGraph.insert(std::make_pair(directory->getOutputFileBase().str(), directory));
}

void writeDotDirDepGraph(std::ostream &t,const DirDef *dd,bool linkRelations)
{
  int fontSize = Config_getInt(DOT_FONTSIZE);
  QCString fontName = Config_getString(DOT_FONTNAME);
  t << "digraph \"" << dd->displayName() << "\" {\n";
  if (Config_getBool(DOT_TRANSPARENT))
  {
    t << "  bgcolor=transparent;\n";
  }
  t << "  compound=true\n";
  t << "  node [ fontsize=\"" << fontSize << "\", fontname=\"" << fontName << "\"];\n";
  t << "  edge [ labelfontsize=\"" << fontSize << "\", labelfontname=\"" << fontName << "\"];\n";

  DirDefMap dirsInGraph;

  dirsInGraph.insert(std::make_pair(dd->getOutputFileBase().str(),dd));

  std::vector<const DirDef *> usedDirsNotDrawn;
  for(const auto& usedDir : dd->usedDirs())
  {
    usedDirsNotDrawn.push_back(usedDir->dir());
  }

  const auto parent = dd->parent();
  if (parent)
  {
    t << "  subgraph cluster" << dd->parent()->getOutputFileBase() << " {\n";
    t << "    graph [ bgcolor=\"#ddddee\", pencolor=\"black\", label=\""
      << dd->parent()->shortName()
      << "\" fontname=\"" << fontName << "\", fontsize=\"" << fontSize << "\", URL=\"";
    t << dd->parent()->getOutputFileBase() << Doxygen::htmlFileExtension;
    t << "\"]\n";

    {
      // draw all directories which have `dd->parent()` as parent and `dd` as dependent
      const auto newEnd = std::remove_if(usedDirsNotDrawn.begin(), usedDirsNotDrawn.end(), [&](const DirDef *const usedDir)
      {
        if (dd!=usedDir && dd->parent()==usedDir->parent())
        {
          drawDirectory(t, usedDir, usedDir->isCluster() && !Config_getBool(DOT_TRANSPARENT), dirsInGraph);
          return true;
        }
        return false;
      }
      );
      usedDirsNotDrawn.erase(newEnd, usedDirsNotDrawn.end());
    }
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
    for(const auto sdir : dd->subDirs())
    {
      drawDirectory(t, sdir, true, dirsInGraph);
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
  {
    //printf("*** For dir %s\n",shortName().data());
    for (const auto &usedDir : usedDirsNotDrawn)
      // for each used dir (=directly used or a parent of a directly used dir)
    {
      const DirDef *dir=dd;
      while (dir)
      {
        //printf("*** check relation %s->%s same_parent=%d !%s->isParentOf(%s)=%d\n",
        //    dir->shortName().data(),usedDir->shortName().data(),
        //    dir->parent()==usedDir->parent(),
        //    usedDir->shortName().data(),
        //    shortName().data(),
        //    !usedDir->isParentOf(this)
        //    );
        if (dir!=usedDir && dir->parent()==usedDir->parent())
          // include if both have the same parent (or no parent)
        {
          drawDirectory(t, usedDir, usedDir->isCluster() && !Config_getBool(DOT_TRANSPARENT), dirsInGraph);
          break;
        }
        dir=dir->parent();
      }
    }
  }

  // add relations between all selected directories
  for (const auto &kv : dirsInGraph) // foreach dir in the graph
  {
    const DirDef *dir = kv.second;
    for (const auto &udir : dir->usedDirs())
    {
      const DirDef *usedDir=udir->dir();
      if ((dir!=dd || !udir->inherited()) &&     // only show direct dependencies for this dir
        (usedDir!=dd || !udir->inherited()) && // only show direct dependencies for this dir
        !usedDir->isParentOf(dir) &&             // don't point to own parent
        dirsInGraph.find(usedDir->getOutputFileBase().str())!=dirsInGraph.end()) // only point to nodes that are in the graph
      {
        QCString relationName;
        relationName.sprintf("dir_%06d_%06d",dir->dirCount(),usedDir->dirCount());
        Doxygen::dirRelations.add(relationName,
            std::make_unique<DirRelation>(
               relationName,dir,udir.get()));
        size_t nrefs = udir->filePairs().size();
        t << "  " << dir->getOutputFileBase() << "->"
          << usedDir->getOutputFileBase();
        t << " [headlabel=\"" << (uint)nrefs << "\", labeldistance=1.5";
        if (linkRelations)
        {
          t << " headhref=\"" << relationName << Doxygen::htmlFileExtension << "\"";
        }
        t << "];\n";
      }
    }
  }

  t << "}\n";
}

DotDirDeps::DotDirDeps(const DirDef *dir) : m_dir(dir)
{
}

DotDirDeps::~DotDirDeps()
{
}

QCString DotDirDeps::getBaseName() const
{
  return m_dir->getOutputFileBase()+"_dep";

}

void DotDirDeps::computeTheGraph()
{
  // compute md5 checksum of the graph were are about to generate
  std::ostringstream md5stream(std::ios_base::ate);
  //m_dir->writeDepGraph(md5stream);
  writeDotDirDepGraph(md5stream,m_dir,m_linkRelations);
  m_theGraph = md5stream.str();
}

QCString DotDirDeps::getMapLabel() const
{
  return escapeCharsInString(m_baseName,FALSE);
}

QCString DotDirDeps::getImgAltText() const
{
  return convertToXML(m_dir->displayName());
}

QCString DotDirDeps::writeGraph(std::ostream &out,
  GraphOutputFormat graphFormat,
  EmbeddedOutputFormat textFormat,
  const char *path,
  const char *fileName,
  const char *relPath,
  bool generateImageMap,
  int graphId,
  bool linkRelations)
{
  m_linkRelations = linkRelations;
  m_urlOnly = TRUE;
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

bool DotDirDeps::isTrivial() const
{
  return m_dir->depGraphIsTrivial();
}

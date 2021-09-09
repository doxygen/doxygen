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

#include <algorithm>
#include <iterator>
#include <tuple>
#include <sstream>

#include "dotdirdeps.h"
#include "util.h"
#include "doxygen.h"
#include "config.h"

using DirDefMap = std::map<std::string,const DirDef *>;

/** Properties are used to format the directories in the graph distinctively. */
struct DotDirProperty
{
  bool isIncomplete = false; //!< true if not all successor of a cluster are drawn
  bool isOrphaned = false; //!< true if parent is not drawn
  bool isTruncated = false; //!< true has successors, none is drawn
  bool isOriginal = false; //!< true if is the directory for which the graph is drawn
  bool isPeriperal = false; //!< true if no successor of parent of original directory
};

/** Elements consist of (1) directory relation and (2) whether it is pointing only to inherited dependees. */
typedef std::vector<std::tuple<const DirRelation*, bool>> DirRelations;
typedef decltype(std::declval<DirDef>().level()) DirectoryLevel; //!< Integer for directory level.

/** Returns a DOT color name according to the directory depth. */
static QCString getDirectoryBackgroundColor(const DirectoryLevel depthIndex)
{
  return "/pastel19/" + QCString().setNum(depthIndex % 9 + 1);
}

/** Returns a DOT color name according to the directory properties. */
static const char* getDirectoryBorderColor(const DotDirProperty &property)
{
  if (property.isTruncated && property.isOrphaned)
  {
    return "darkorchid3";
  }
  else if (property.isTruncated)
  {
    return "red";
  }
  else if (property.isOrphaned)
  {
    return "grey75";
  }
  else
  {
    return "black";
  }
}

/** Returns a DOT node style according to the directory properties. */
static std::string getDirectoryBorderStyle(const DotDirProperty &property)
{
  std::string style;
  if (!property.isPeriperal)
  {
    style += "filled,";
  }
  if (property.isOriginal)
  {
    style += "bold,";
  }
  if (property.isIncomplete)
  {
    style += "dashed,";
  }
  return style;
}

/**
 * Puts DOT code for drawing directory to stream and adds it to the list.
 * @param[in,out] outStream stream to which the DOT code is written to
 * @param[in] directory will be mapped to a node in DOT code
 * @param[in] property are evaluated for formatting
 * @param[in,out] directoriesInGraph lists the directories which have been written to the output stream
 */
static void drawDirectory(TextStream &outStream, const DirDef *const directory, const DotDirProperty &property,
    DirDefMap &directoriesInGraph)
{
  outStream << "  " << directory->getOutputFileBase() << " ["
      "shape=box, "
      "label=\"" << directory->shortName() << "\", "
      "style=\"" << getDirectoryBorderStyle(property) << "\", "
      "fillcolor=\"" << getDirectoryBackgroundColor(directory->level()) << "\", "
      "color=\"" << getDirectoryBorderColor(property) << "\", "
      "URL=\"" << directory->getOutputFileBase() << Doxygen::htmlFileExtension << "\""
      "];\n";
  directoriesInGraph.insert(std::make_pair(directory->getOutputFileBase().str(), directory));
}

/** Checks, if the directory is a the maximum drawn directory level. */
static bool isAtLowerVisibilityBorder(const DirDef *const directory, const DirectoryLevel startLevel)
{
  return (directory->level() - startLevel) == Config_getInt(MAX_DOT_GRAPH_SUCCESSOR);
}

/**
 * Writes DOT code for opening a cluster subgraph to stream.
 *
 * Ancestor clusters directly get a label. Other clusters get a plain text node with a label instead.
 * This is because the plain text node can be used to draw dependency relationships.
 */
static void drawClusterOpening(TextStream &outputStream, const DirDef *const directory,
    const DotDirProperty &directoryProperty, DirDefMap &directoriesInGraph, const bool isAncestor)
{
  outputStream << "  subgraph cluster" << directory->getOutputFileBase() << " {\n"
      "    graph [ "
      "bgcolor=\"" << getDirectoryBackgroundColor(directory->level()) << "\", "
      "pencolor=\"" << getDirectoryBorderColor(directoryProperty) << "\", "
      "style=\"" << getDirectoryBorderStyle(directoryProperty) << "\", "
      "label=\"";
  if (isAncestor)
  {
    outputStream << directory->shortName();
  }
  outputStream << "\", "
      "fontname=\"" << Config_getString(DOT_FONTNAME) << "\", "
      "fontsize=\"" << Config_getInt(DOT_FONTSIZE) << "\", "
      "URL=\"" << directory->getOutputFileBase() << Doxygen::htmlFileExtension << "\""
      "]\n";
  if (!isAncestor)
  {
    outputStream << "    " << directory->getOutputFileBase() << " [shape=plaintext, "
        "label=\"" << directory->shortName() << "\""
        "];\n";
    directoriesInGraph.insert(std::make_pair(directory->getOutputFileBase().str(), directory));
  }
}

/**
 * Assembles a list of the directory relations and if they result from inheritance.
 * @param dependent is the source of the dependency
 * @param isLeaf true, if no successors are drawn for this directory
 * @return list of directory relations
 */
static auto getDependencies(const DirDef *const dependent, const bool isLeaf)
{
  DirRelations dependencies;
  for (const auto &usedDirectory : dependent->usedDirs())
  {
    const auto dependee = usedDirectory->dir();
    if (!dependee->isParentOf(dependent) && (isLeaf || !usedDirectory->isAllDependentsInherited()))
    {
      QCString relationName;
      relationName.sprintf("dir_%06d_%06d", dependent->dirCount(), dependee->dirCount());
      const auto dependency = new DirRelation(relationName, dependent, usedDirectory.get());
      dependencies.emplace_back(dependency, usedDirectory->isAllDependeesInherited(isLeaf));
    }
  }
  return dependencies;
}

/** Recursively draws directory tree. */
static DirRelations drawTree(TextStream &outputStream, const DirDef *const directory,
    const DirectoryLevel startLevel, DirDefMap &directoriesInGraph, const bool isTreeRoot)
{
  DirRelations dependencies;
  if (!directory->isCluster())
  {
    const DotDirProperty directoryProperty = { false, false, false, isTreeRoot, false };
    drawDirectory(outputStream, directory, directoryProperty, directoriesInGraph);
    const auto deps = getDependencies(directory, true);
    dependencies.insert(std::end(dependencies), std::begin(deps), std::end(deps));
  }
  else
  {
    if (isAtLowerVisibilityBorder(directory, startLevel))
    {
      const DotDirProperty directoryProperty = { false, false, true, isTreeRoot, false };
      drawDirectory(outputStream, directory, directoryProperty, directoriesInGraph);
      const auto deps = getDependencies(directory, true);
      dependencies.insert(std::end(dependencies), std::begin(deps), std::end(deps));
    }
    else
    {
      {  // open cluster
        const DotDirProperty directoryProperty = { false, false, false, isTreeRoot, false };
        drawClusterOpening(outputStream, directory, directoryProperty, directoriesInGraph, false);
        const auto deps = getDependencies(directory, false);
        dependencies.insert(std::end(dependencies), std::begin(deps), std::end(deps));
      }

      for (const auto subDirectory : directory->subDirs())
      {
        const auto deps = drawTree(outputStream, subDirectory, startLevel, directoriesInGraph, false);
        dependencies.insert(std::end(dependencies), std::begin(deps), std::end(deps));
      }
      {  //close cluster
        outputStream << "  }\n";
      }
    }
  }
  return dependencies;
}

/**
 * Write DOT code for directory dependency graph.
 *
 * Code is generated for a directory. Successors (sub-directories) of this directory are recursively drawn.
 * Recursion is limited by `MAX_DOT_GRAPH_SUCCESSOR`. The dependencies of those directories
 * are drawn.
 *
 * If a dependee is not part of directory tree above, then the dependency is drawn to the first parent of the
 * dependee, whose parent is an ancestor (sub-directory) of the original directory.
 *
 * @param t stream where the DOT code is written to
 * @param dd directory for which the graph is generated for
 * @param linkRelations if true, hyperlinks to the list of file dependencies are added
 */
void writeDotDirDepGraph(TextStream &t,const DirDef *dd,bool linkRelations)
{
  DirDefMap dirsInGraph;

  dirsInGraph.insert(std::make_pair(dd->getOutputFileBase().str(),dd));

  std::vector<const DirDef *> usedDirsNotDrawn, usedDirsDrawn;
  for(const auto& usedDir : dd->usedDirs())
  {
    usedDirsNotDrawn.push_back(usedDir->dir());
  }

  const auto parent = dd->parent();
  if (parent)
  {
    const DotDirProperty parentDirProperty = {true, parent->parent()!=nullptr, false, false, false};
    drawClusterOpening(t, parent, parentDirProperty, dirsInGraph, true);

    {
      // draw all directories which have `dd->parent()` as parent and `dd` as dependent
      const auto newEnd = std::stable_partition(usedDirsNotDrawn.begin(), usedDirsNotDrawn.end(), [&](const DirDef *const usedDir)
      {
        if (dd!=usedDir && dd->parent()==usedDir->parent())
        {
          const DotDirProperty usedDirProperty = {false, false, usedDir->isCluster(), false, false};
          drawDirectory(t, usedDir, usedDirProperty, dirsInGraph);
          return false;
        }
        return true;
      }
      );
      std::move(newEnd, std::end(usedDirsNotDrawn), std::back_inserter(usedDirsDrawn));
      usedDirsNotDrawn.erase(newEnd, usedDirsNotDrawn.end());
    }
  }

  const auto dependencies = drawTree(t, dd, dd->level(), dirsInGraph, true);

  if (dd->parent())
  {
    // close cluster subgraph
    t << "  }\n";
  }

  // add nodes for other used directories
  {
    const auto newEnd =
        std::stable_partition(usedDirsNotDrawn.begin(), usedDirsNotDrawn.end(), [&](const DirDef *const usedDir)
	        // for each used dir (=directly used or a parent of a directly used dir)
        {
          const DirDef *dir=dd;
          while (dir)
          {
            if (dir!=usedDir && dir->parent()==usedDir->parent())
            // include if both have the same parent (or no parent)
            {
              const DotDirProperty usedDirProperty = { false, usedDir->parent() != nullptr, usedDir->isCluster(), false, true};
              drawDirectory(t, usedDir, usedDirProperty, dirsInGraph);
              return false;
            }
            dir=dir->parent();
          }
          return true;
        }
        );
    std::move(newEnd, std::end(usedDirsNotDrawn), std::back_inserter(usedDirsDrawn));
    usedDirsNotDrawn.erase(newEnd, usedDirsNotDrawn.end());
  }


  // add relations between all selected directories
  {
    for (const auto relationTuple : dependencies)
    {
      const auto relation = std::get<0>(relationTuple);
      const auto udir = relation->destination();
      const auto usedDir = udir->dir();

      const bool destIsSibling = std::find(std::begin(usedDirsDrawn), std::end(usedDirsDrawn), usedDir) != std::end(usedDirsDrawn);
      const bool destIsDrawn = dirsInGraph.find(usedDir->getOutputFileBase().str())!=dirsInGraph.end(); // only point to nodes that are in the graph
      const bool notInherited = !std::get<1>(relationTuple);
      const bool atVisibilityLimit = isAtLowerVisibilityBorder(usedDir, dd->level());

      if (destIsSibling || (destIsDrawn && (notInherited || atVisibilityLimit)))
      {
        const auto relationName = relation->getOutputFileBase();
        const auto dir = relation->source();
        Doxygen::dirRelations.add(relationName,
            std::make_unique<DirRelation>(
               relationName,dir,udir));
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
  TextStream md5stream;
  writeGraphHeader(md5stream, m_dir->displayName());
  md5stream << "  compound=true\n";
  writeDotDirDepGraph(md5stream,m_dir,m_linkRelations);
  writeGraphFooter(md5stream);
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

QCString DotDirDeps::writeGraph(TextStream &out,
  GraphOutputFormat graphFormat,
  EmbeddedOutputFormat textFormat,
  const QCString &path,
  const QCString &fileName,
  const QCString &relPath,
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

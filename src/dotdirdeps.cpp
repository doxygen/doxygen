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

#include "dotdirdeps.h"
#include "util.h"
#include "doxygen.h"
#include "config.h"
#include "image.h"
#include "dotnode.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <cstdint>
#include <math.h>
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <vector>

using DirDefMap = std::map<std::string,const DirDef *>;

/** Properties are used to format the directories in the graph distinctively. */
struct DotDirProperty
{
  bool isIncomplete = false; //!< true if not all successors of a cluster are drawn
  bool isOrphaned   = false; //!< true if parent is not drawn
  bool isTruncated  = false; //!< true has successors, none is drawn
  bool isOriginal   = false; //!< true if is the directory for which the graph is drawn
  bool isPeripheral = false; //!< true if no successor of parent of original directory
};

/** Builder helper to create instances of the DotDirProperty struct */
class DotDirPropertyBuilder
{
  public:
    DotDirPropertyBuilder &makeIncomplete(bool b=true) { m_property.isIncomplete=b; return *this; }
    DotDirPropertyBuilder &makeOrphaned  (bool b=true) { m_property.isOrphaned  =b; return *this; }
    DotDirPropertyBuilder &makeTruncated (bool b=true) { m_property.isTruncated =b; return *this; }
    DotDirPropertyBuilder &makeOriginal  (bool b=true) { m_property.isOriginal  =b; return *this; }
    DotDirPropertyBuilder &makePeripheral(bool b=true) { m_property.isPeripheral=b; return *this; }
    operator DotDirProperty() { return std::move(m_property); }
  private:
    DotDirProperty m_property;
};

/** Elements consist of (1) directory relation and (2) whether it is pointing only to inherited dependees. */
typedef std::vector< std::pair< std::unique_ptr<DirRelation>, bool> > DirRelations;

/** Returns a DOT color name according to the directory depth. */
static QCString getDirectoryBackgroundColor(int depthIndex)
{
  int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  assert(depthIndex>=0 && depthIndex<=Config_getInt(DIR_GRAPH_MAX_DEPTH));
  float fraction = static_cast<float>(depthIndex)/static_cast<float>(Config_getInt(DIR_GRAPH_MAX_DEPTH));
  const char hex[] = "0123456789abcdef";
  int range = 0x40; // range from darkest color to lightest color
  int luma   = 0xef-static_cast<int>(fraction*static_cast<float>(range)); // interpolation
  double r=0, g=0, b=0;
  ColoredImage::hsl2rgb(hue/360.0,sat/255.0,
                        pow(luma/255.0,gamma/100.0),&r,&g,&b);
  int red   = static_cast<int>(r*255.0);
  int green = static_cast<int>(g*255.0);
  int blue  = static_cast<int>(b*255.0);
  assert(red>=0   && red<=255);
  assert(green>=0 && green<=255);
  assert(blue>=0  && blue<=255);
  char colStr[8];
  colStr[0]='#';
  colStr[1]=hex[red>>4];
  colStr[2]=hex[red&0xf];
  colStr[3]=hex[green>>4];
  colStr[4]=hex[green&0xf];
  colStr[5]=hex[blue>>4];
  colStr[6]=hex[blue&0xf];
  colStr[7]=0;
  //printf("i=%d max=%d fraction=%f luma=%d %02x %02x %02x -> color=%s\n",
  //    depthIndex,Config_getInt(DIR_GRAPH_MAX_DEPTH),fraction,luma,red,green,blue,colStr);
  return colStr;
}

/** Returns a DOT color name according to the directory properties. */
static const char* getDirectoryBorderColor(const DotDirProperty &property)
{
  if (property.isTruncated && property.isOrphaned)
  {
    return "red";
  }
  else if (property.isTruncated)
  {
    return "red";
  }
  else if (property.isOrphaned)
  {
    return "grey50";
  }
  else
  {
    return "grey25";
  }
}

/** Returns a DOT node style according to the directory properties. */
static std::string getDirectoryBorderStyle(const DotDirProperty &property)
{
  std::string style = "filled";
  if (property.isOriginal)
  {
    style += ",bold";
  }
  if (property.isIncomplete)
  {
    style += ",dashed";
  }
  else if (property.isTruncated && property.isOrphaned)
  {
    style += ",dashed";
  }
  return style;
}

static TextStream &common_attributes(TextStream &t, const DirDef *const dir, const DotDirProperty &prop)
{
  QCString url = dir->getOutputFileBase();
  addHtmlExtensionIfMissing(url);
  return t <<
    "style=\""   << getDirectoryBorderStyle(prop) << "\", "
    "URL=\""     << url << "\","
    "tooltip=\"" << escapeTooltip(dir->briefDescriptionAsTooltip()) << "\"";
}

/**
 * Puts DOT code for drawing directory to stream and adds it to the list.
 * @param[in,out] t stream to which the DOT code is written to
 * @param[in] directory will be mapped to a node in DOT code
 * @param[in] property are evaluated for formatting
 * @param[in,out] directoriesInGraph lists the directories which have been written to the output stream
 * @param[in] startLevel current level to calculate relative distances from to determine the background color
 */
static void drawDirectory(TextStream &t, const DirDef *const directory, const DotDirProperty &property,
    DirDefMap &directoriesInGraph,int startLevel)
{
  t << "  " << directory->getOutputFileBase() << " ["
      "label=\""     << DotNode::convertLabel(directory->shortName())                << "\", "
      "fillcolor=\"" << getDirectoryBackgroundColor(directory->level()-startLevel)   << "\", "
      "color=\""     << getDirectoryBorderColor(property)                            << "\", ";
  common_attributes(t, directory, property)
      << "];\n";
  directoriesInGraph.emplace(directory->getOutputFileBase().str(), directory);
}

/** Checks, if the directory is a the maximum drawn directory level. */
static bool isAtMaxDepth(const DirDef *const directory, const int startLevel)
{
  return (directory->level() - startLevel) >= Config_getInt(DIR_GRAPH_MAX_DEPTH);
}

/**
 * Writes DOT code for opening a cluster subgraph to stream.
 *
 * Ancestor clusters directly get a label. Other clusters get a plain text node with a label instead.
 * This is because the plain text node can be used to draw dependency relationships.
 */
static void drawClusterOpening(TextStream &outputStream, const DirDef *const directory,
    const DotDirProperty &directoryProperty, DirDefMap &directoriesInGraph, const bool isAncestor,int startLevel)
{
  outputStream << "  subgraph cluster" << directory->getOutputFileBase() << " {\n"
      "    graph [ "
      "bgcolor=\""  << getDirectoryBackgroundColor(directory->level()-startLevel) << "\", "
      "pencolor=\"" << getDirectoryBorderColor(directoryProperty) << "\", "
      "label=\"";
  if (isAncestor)
  {
    outputStream << DotNode::convertLabel(directory->shortName());
  }
  outputStream << "\", "
       << Config_getString(DOT_COMMON_ATTR) << " ";
  common_attributes(outputStream, directory, directoryProperty)
      << "]\n";
  if (!isAncestor)
  {
    outputStream << "    " << directory->getOutputFileBase() << " [shape=plaintext, "
        "label=\"" << DotNode::convertLabel(directory->shortName()) << "\""
        "];\n";
    directoriesInGraph.emplace(directory->getOutputFileBase().str(), directory);
  }
}

static void drawClusterClosing(TextStream &t)
{
  t << "  }\n";
}

/**
 * Assembles a list of the directory relations and whether or not they result from "inheritance".
 * @param dependencies Array to add the dependencies to.
 * @param srcDir is the source of the dependency.
 * @param isLeaf true, if no children are drawn for this directory.
 */
static void addDependencies(DirRelations &dependencies,const DirDef *const srcDir, bool isLeaf)
{
  for (const auto &usedDirectory : srcDir->usedDirs())
  {
    const auto dstDir = usedDirectory->dir();
    if (!dstDir->isParentOf(srcDir) && (isLeaf || usedDirectory->hasDirectSrcDeps()))
    {
      QCString relationName;
      relationName.sprintf("dir_%06d_%06d", srcDir->dirIndex(), dstDir->dirIndex());
      bool directRelation = isLeaf ? usedDirectory->hasDirectDstDeps() : usedDirectory->hasDirectDeps();
      dependencies.emplace_back(
          std::make_unique<DirRelation>(relationName, srcDir, usedDirectory.get()),
          directRelation);
    }
  }
}

/** Recursively draws directory tree. */
static void drawTree(DirRelations &dependencies, TextStream &t, const DirDef *const directory,
    int startLevel, DirDefMap &directoriesInGraph, const bool isTreeRoot)
{
  if (!directory->hasSubdirs())
  {
    const DotDirProperty directoryProperty = DotDirPropertyBuilder().makeOriginal(isTreeRoot);
    drawDirectory(t, directory, directoryProperty, directoriesInGraph,startLevel);
    addDependencies(dependencies, directory, true);
  }
  else
  {
    if (isAtMaxDepth(directory, startLevel)) // maximum nesting level reached
    {
      const DotDirProperty directoryProperty = DotDirPropertyBuilder().makeOriginal(isTreeRoot);
      drawDirectory(t, directory, directoryProperty, directoriesInGraph,startLevel);
      addDependencies(dependencies, directory, true);
    }
    else // start a new nesting level
    {
      // open cluster
      {
        const DotDirProperty directoryProperty = DotDirPropertyBuilder().makeOriginal(isTreeRoot);
        drawClusterOpening(t, directory, directoryProperty, directoriesInGraph, false, startLevel);
        addDependencies(dependencies, directory, false);
      }

      // process all sub directories
      for (const auto subDirectory : directory->subDirs())
      {
        drawTree(dependencies, t, subDirectory, startLevel, directoriesInGraph, false);
      }

      // close cluster
      {
        drawClusterClosing(t);
      }
    }
  }
}

/**
 * Write DOT code for directory dependency graph.
 *
 * Code is generated for a directory. Successors (sub-directories) of this directory are recursively drawn.
 * Recursion is limited by `DIR_GRAPH_MAX_DEPTH`. The dependencies of those directories
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

  dirsInGraph.emplace(dd->getOutputFileBase().str(),dd);

  std::vector<const DirDef *> usedDirsNotDrawn, usedDirsDrawn;
  for (const auto& usedDir : dd->usedDirs())
  {
    usedDirsNotDrawn.push_back(usedDir->dir());
  }

  auto moveDrawnDirs = [&usedDirsDrawn,&usedDirsNotDrawn](const std::vector<const DirDef *>::iterator &newEnd)
  {
    // usedDirsNotDrawn is split into two segments: [begin()....newEnd-1] and [newEnd....end()]
    // where the second segment starting at newEnd has been drawn, so append this segment to the usedDirsDrawn list and
    // remove it from the usedDirsNotDrawn list.
    std::move(newEnd, std::end(usedDirsNotDrawn), std::back_inserter(usedDirsDrawn));
    usedDirsNotDrawn.erase(newEnd, usedDirsNotDrawn.end());
  };

  // if dd has a parent draw it as the outer layer
  const auto parent = dd->parent();
  if (parent)
  {
    const DotDirProperty parentDirProperty = DotDirPropertyBuilder().
                                             makeIncomplete().
                                             makeOrphaned(parent->parent()!=nullptr);
    drawClusterOpening(t, parent, parentDirProperty, dirsInGraph, true, parent->level());

    {
      // draw all directories which have `dd->parent()` as parent and `dd` as dependent
      const auto newEnd = std::stable_partition(usedDirsNotDrawn.begin(), usedDirsNotDrawn.end(),
        [&](const DirDef *const usedDir)
        {
          if (dd!=usedDir && dd->parent()==usedDir->parent()) // usedDir and dd share the same parent
          {
            const DotDirProperty usedDirProperty = DotDirPropertyBuilder().makeTruncated(usedDir->hasSubdirs());
            drawDirectory(t, usedDir, usedDirProperty, dirsInGraph, parent->level());
            return false; // part of the drawn partition
          }
          return true; // part of the not-drawn partition
        });
      moveDrawnDirs(newEnd);
    }
  }

  // draw the directory tree with dd as root
  DirRelations dependencies;
  drawTree(dependencies, t, dd, dd->level(), dirsInGraph, true);

  if (parent)
  {
    drawClusterClosing(t);
  }

  // add nodes for other used directories (i.e. outside of the cluster of directories directly connected to dd)
  {
    const auto newEnd = std::stable_partition(usedDirsNotDrawn.begin(), usedDirsNotDrawn.end(),
     [&](const DirDef *const usedDir) // for each used dir (=directly used or a parent of a directly used dir)
     {
       const DirDef *dir=dd;
       while (dir)
       {
         if (dir!=usedDir && dir->parent()==usedDir->parent()) // include if both have the same parent (or no parent)
         {
           const DotDirProperty usedDirProperty = DotDirPropertyBuilder().
                                                  makeOrphaned(usedDir->parent()!=nullptr).
                                                  makeTruncated(usedDir->hasSubdirs()).
                                                  makePeripheral();
           drawDirectory(t, usedDir, usedDirProperty, dirsInGraph, dir->level());
           return false; // part of the drawn partition
         }
         dir=dir->parent();
       }
       return true; // part of the not-drawn partition
     });
    moveDrawnDirs(newEnd);
  }

  // add relations between all selected directories
  {
    for (const auto &relationPair : dependencies)
    {
      const auto &relation         = relationPair.first;
      const bool directRelation    = relationPair.second;
      const auto udir              = relation->destination();
      const auto usedDir           = udir->dir();
      const bool destIsSibling     = std::find(std::begin(usedDirsDrawn), std::end(usedDirsDrawn), usedDir) != std::end(usedDirsDrawn);
      const bool destIsDrawn       = dirsInGraph.find(usedDir->getOutputFileBase().str())!=dirsInGraph.end(); // only point to nodes that are in the graph
      const bool atMaxDepth        = isAtMaxDepth(usedDir, dd->level());

      if (destIsSibling || (destIsDrawn && (directRelation || atMaxDepth)))
      {
        const auto relationName = relation->getOutputFileBase();
        const auto dir = relation->source();
        Doxygen::dirRelations.add(relationName,
            std::make_unique<DirRelation>(
               relationName,dir,udir));
        size_t nrefs = udir->filePairs().size();
        t << "  " << dir->getOutputFileBase() << "->"
          << usedDir->getOutputFileBase();
        t << " [headlabel=\"" << nrefs << "\", labeldistance=1.5";
        if (linkRelations)
        {
          QCString fn = relationName;
          addHtmlExtensionIfMissing(fn);
          t << " headhref=\"" << fn << "\"";
          t << " href=\"" << fn << "\"";
        }
        t << " color=\"steelblue1\" fontcolor=\"steelblue1\"];\n";
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

QCString DotDirDeps::writeGraph(TextStream &out, GraphOutputFormat graphFormat, EmbeddedOutputFormat textFormat,
                                const QCString &path, const QCString &fileName, const QCString &relPath, bool generateImageMap,
                                int graphId, bool linkRelations)
{
  m_linkRelations = linkRelations;
  m_urlOnly = TRUE;
  return DotGraph::writeGraph(out, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

bool DotDirDeps::isTrivial() const
{
  return m_dir->depGraphIsTrivial();
}

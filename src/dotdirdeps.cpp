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

#include "ftextstream.h"
#include "util.h"
#include "doxygen.h"
#include "config.h"

void writeDotDirDepGraph(FTextStream &t,const DirDef *dd,bool linkRelations)
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

  QDict<DirDef> dirsInGraph(257);

  dirsInGraph.insert(dd->getOutputFileBase(),dd);
  if (dd->parent())
  {
    t << "  subgraph cluster" << dd->parent()->getOutputFileBase() << " {\n";
    t << "    graph [ bgcolor=\"#ddddee\", pencolor=\"black\", label=\"" 
      << dd->parent()->shortName() 
      << "\" fontname=\"" << fontName << "\", fontsize=\"" << fontSize << "\", URL=\"";
    t << dd->parent()->getOutputFileBase() << Doxygen::htmlFileExtension;
    t << "\"]\n";
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
    QListIterator<DirDef> sdi(dd->subDirs());
    const DirDef *sdir;
    for (sdi.toFirst();(sdir=sdi.current());++sdi)
    {
      t << "    " << sdir->getOutputFileBase() << " [shape=box label=\"" 
        << sdir->shortName() << "\"";
      if (sdir->isCluster())
      {
        t << " color=\"red\"";
      }
      else
      {
        t << " color=\"black\"";
      }
      t << " fillcolor=\"white\" style=\"filled\"";
      t << " URL=\"" << sdir->getOutputFileBase() 
        << Doxygen::htmlFileExtension << "\"";
      t << "];\n";
      dirsInGraph.insert(sdir->getOutputFileBase(),sdir);
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
  QDictIterator<UsedDir> udi(*dd->usedDirs());
  UsedDir *udir;
  //printf("*** For dir %s\n",shortName().data());
  for (udi.toFirst();(udir=udi.current());++udi) 
    // for each used dir (=directly used or a parent of a directly used dir)
  {
    const DirDef *usedDir=udir->dir();
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
      if (dir!=usedDir && dir->parent()==usedDir->parent() && 
        !usedDir->isParentOf(dd))
        // include if both have the same parent (or no parent)
      {
        t << "  " << usedDir->getOutputFileBase() << " [shape=box label=\"" 
          << usedDir->shortName() << "\"";
        if (usedDir->isCluster())
        {
          if (!Config_getBool(DOT_TRANSPARENT))
          {
            t << " fillcolor=\"white\" style=\"filled\"";
          }
          t << " color=\"red\"";
        }
        t << " URL=\"" << usedDir->getOutputFileBase() 
          << Doxygen::htmlFileExtension << "\"];\n";
        dirsInGraph.insert(usedDir->getOutputFileBase(),usedDir);
        break;
      }
      dir=dir->parent();
    }
  }

  // add relations between all selected directories
  const DirDef *dir;
  QDictIterator<DirDef> di(dirsInGraph);
  for (di.toFirst();(dir=di.current());++di) // foreach dir in the graph
  {
    QDictIterator<UsedDir> udi(*dir->usedDirs());
    UsedDir *udir;
    for (udi.toFirst();(udir=udi.current());++udi) // foreach used dir
    {
      const DirDef *usedDir=udir->dir();
      if ((dir!=dd || !udir->inherited()) &&     // only show direct dependencies for this dir
        (usedDir!=dd || !udir->inherited()) && // only show direct dependencies for this dir
        !usedDir->isParentOf(dir) &&             // don't point to own parent
        dirsInGraph.find(usedDir->getOutputFileBase())) // only point to nodes that are in the graph
      {
        QCString relationName;
        relationName.sprintf("dir_%06d_%06d",dir->dirCount(),usedDir->dirCount());
        if (Doxygen::dirRelations.find(relationName)==0)
        {
          // new relation
          Doxygen::dirRelations.append(relationName,
            new DirRelation(relationName,dir,udir));
        }
        int nrefs = udir->filePairs().count();
        t << "  " << dir->getOutputFileBase() << "->"
          << usedDir->getOutputFileBase();
        t << " [headlabel=\"" << nrefs << "\", labeldistance=1.5";
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
  FTextStream md5stream(&m_theGraph);
  //m_dir->writeDepGraph(md5stream);
  writeDotDirDepGraph(md5stream,m_dir,m_linkRelations);
}

QCString DotDirDeps::getMapLabel() const
{
  return escapeCharsInString(m_baseName,FALSE);
}

QCString DotDirDeps::getImgAltText() const
{
  return convertToXML(m_dir->displayName());
}

QCString DotDirDeps::writeGraph(FTextStream &out,
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

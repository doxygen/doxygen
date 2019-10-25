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

#ifndef DOTCLASSGRAPH_H
#define DOTCLASSGRAPH_H

#include "classdef.h"

#include "dotgraph.h"

/** Representation of a class inheritance or dependency graph */
class DotClassGraph : public DotGraph
{
public:
  DotClassGraph(const ClassDef *cd,GraphType t);
  ~DotClassGraph();
  bool isTrivial() const;
  bool isTooBig(int &numNodes) const;
  QCString writeGraph(FTextStream &t,GraphOutputFormat gf,EmbeddedOutputFormat ef,
    const char *path, const char *fileName, const char *relPath,
    bool TBRank=TRUE,bool imageMap=TRUE,int graphId=-1);

  void writeXML(FTextStream &t);
  void writeDocbook(FTextStream &t);
  void writeDEF(FTextStream &t);

protected:
  virtual QCString getBaseName() const;
  virtual QCString getMapLabel() const;
  virtual void computeTheGraph();
  virtual QCString getImgAltText() const;

private:
  void buildGraph(const ClassDef *cd,DotNode *n,bool base,int distance);
  bool determineVisibleNodes(DotNode *rootNode,int maxNodes,bool includeParents);
  void determineTruncatedNodes(QList<DotNode> &queue,bool includeParents);
  void addClass(const ClassDef *cd,DotNode *n,int prot,const char *label,
    const char *usedName,const char *templSpec,
    bool base,int distance);

  DotNode        *   m_startNode;
  QDict<DotNode> *   m_usedNodes;
  GraphType          m_graphType;
  QCString           m_collabFileName;
  QCString           m_inheritFileName;
  bool               m_lrRank;
};


#endif

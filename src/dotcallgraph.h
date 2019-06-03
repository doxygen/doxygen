/******************************************************************************
*
* 
*
*
* Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#ifndef _DOT_CALL_GRAPH_H
#define _DOT_CALL_GRAPH_H

#include "dotgraph.h"
#include "ftextstream.h"
#include "memberdef.h"

/** Representation of an call graph */
class DotCallGraph : public DotGraph
{
public:
  DotCallGraph(const MemberDef *md,bool inverse);
  ~DotCallGraph();
  QCString writeGraph(FTextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
    const char *path,const char *fileName,
    const char *relPath,bool writeImageMap=TRUE,
    int graphId=-1);
  void buildGraph(DotNode *n,const MemberDef *md,int distance);
  bool isTrivial() const;
  bool isTooBig() const;
  void determineVisibleNodes(QList<DotNode> &queue, int &maxNodes);
  void determineTruncatedNodes(QList<DotNode> &queue);

protected:
  virtual QCString getBaseName() const;
  virtual QCString getMapLabel() const;
  virtual void computeTheGraph();

private:
  DotNode        *m_startNode;
  QDict<DotNode> *m_usedNodes;
  bool            m_inverse;
  QCString        m_diskName;
  const Definition * m_scope;
};

#endif
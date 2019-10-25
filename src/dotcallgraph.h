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

#ifndef DOTCALLGRAPH_H
#define DOTCALLGRAPH_H

#include "dotgraph.h"
#include "ftextstream.h"
#include "memberdef.h"

/** Representation of an call graph */
class DotCallGraph : public DotGraph
{
  public:
    DotCallGraph(const MemberDef *md,bool inverse);
    ~DotCallGraph();
    bool isTrivial() const;
    bool isTooBig(int &numNodes) const;
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                        const char *path,const char *fileName,
                        const char *relPath,bool writeImageMap=TRUE,
                        int graphId=-1);

  protected:
    virtual QCString getBaseName() const;
    virtual QCString getMapLabel() const;
    virtual void computeTheGraph();

  private:
    void buildGraph(DotNode *n,const MemberDef *md,int distance);
    void determineVisibleNodes(QList<DotNode> &queue, int &maxNodes);
    void determineTruncatedNodes(QList<DotNode> &queue);
    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    bool            m_inverse;
    QCString        m_diskName;
    const Definition * m_scope;
};

#endif

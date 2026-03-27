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

#include <memory>

#include "dotnode.h"
#include "dotgraph.h"
#include "memberdef.h"

/** Representation of an call graph */
class DotCallGraph : public DotGraph
{
  public:
    DotCallGraph(const MemberDef *md,bool inverse);
    ~DotCallGraph() override;
    NON_COPYABLE(DotCallGraph)

    bool isTrivial() const;
    bool isTooBig() const;
    int numNodes() const;
    QCString writeGraph(TextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                        const QCString &path,const QCString &fileName,
                        const QCString &relPath,bool writeImageMap=TRUE,
                        int graphId=-1);
    static bool isTrivial(const MemberDef *md,bool inverse);

  protected:
    QCString getBaseName() const override;
    QCString getMapLabel() const override;
    void computeTheGraph() override;

  private:
    void buildGraph(DotNode *n,const MemberDef *md,int distance);
    void determineVisibleNodes(DotNodeDeque &queue, int &maxNodes);
    void determineTruncatedNodes(DotNodeDeque &queue);
    DotNode        *m_startNode;
    DotNodeMap      m_usedNodes;
    bool            m_inverse;
    QCString        m_diskName;
    const Definition * m_scope;
};

using DotCallGraphPtr = std::shared_ptr<DotCallGraph>;

#endif

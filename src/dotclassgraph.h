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

#include <memory>

#include "classdef.h"
#include "dotnode.h"
#include "dotgraph.h"

class TextStream;

/** Representation of a class inheritance or dependency graph */
class DotClassGraph final : public DotGraph
{
  public:
    DotClassGraph(const ClassDef *cd,GraphType t);
   ~DotClassGraph() override;
    NON_COPYABLE(DotClassGraph)

    bool isTrivial() const;
    bool isTooBig() const;
    int numNodes() const;
    DString writeGraph(TextStream &t,GraphOutputFormat gf,EmbeddedOutputFormat ef,
        const DString &path, const DString &fileName, const DString &relPath,
        bool TBRank=true,bool imageMap=true,int graphId=-1);

    void writeXML(TextStream &t);
    void writeDocbook(TextStream &t);
    void writeDEF(TextStream &t);

  protected:
    DString getBaseName() const override;
    DString getMapLabel() const override;
    void computeTheGraph() override;
    DString getImgAltText() const override;

  private:
    void buildGraph(const ClassDef *cd,DotNode *n,bool base,int distance);
    bool determineVisibleNodes(DotNode *rootNode,int maxNodes,bool includeParents);
    void determineTruncatedNodes(DotNodeDeque &queue,bool includeParents);
    void addClass(const ClassDef *cd,DotNode *n,EdgeInfo::Colors color,const DString &label,
        const DString &usedName,const DString &templSpec,
        bool base,int distance);

    DotNode        *   m_startNode;
    DotNodeMap         m_usedNodes;
    GraphType          m_graphType;
    DString           m_collabFileName;
    DString           m_inheritFileName;
    bool               m_lrRank;
};

using DotClassGraphPtr = std::shared_ptr<DotClassGraph>;

#endif

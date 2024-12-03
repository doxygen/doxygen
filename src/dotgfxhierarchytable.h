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

#ifndef DOTGFXHIERARCHYTABLE_H
#define DOTGFXHIERARCHYTABLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "classdef.h"
#include "dotgraph.h"
#include "dotnode.h"

class ClassLinkedMap;

/** Represents a graphical class hierarchy */
class DotGfxHierarchyTable : public DotGraph
{
  public:
    DotGfxHierarchyTable(const QCString &prefix="",ClassDef::CompoundType ct=ClassDef::Class);
   ~DotGfxHierarchyTable() override = default;
    NON_COPYABLE(DotGfxHierarchyTable)

    void createGraph(DotNode *rootNode,TextStream &t,const QCString &path,
                     const QCString &fileName,int id);
    void writeGraph(TextStream &t,const QCString &path, const QCString &fileName);
    const std::vector<DotNode*> subGraphs() const { return m_rootSubgraphs; }

  protected:
    QCString getBaseName() const override;
    QCString getMapLabel() const override;
    void computeTheGraph() override;

  private:
    void addHierarchy(DotNode *n,const ClassDef *cd,ClassDefSet &visited);
    void addClassList(const ClassLinkedMap &cl,ClassDefSet &visited);

    using DotNodeMap = std::unordered_multimap< std::string, std::unique_ptr<DotNode> >;
    int                    m_graphId;
    QCString               m_prefix;
    ClassDef::CompoundType m_classType;
    std::vector<DotNode*>  m_rootNodes;
    DotNodeMap             m_usedNodes;
    std::vector<DotNode*>  m_rootSubgraphs;
    DotNode *              m_rootSubgraphNode;
};

using DotGfxHierarchyTablePtr = std::shared_ptr<DotGfxHierarchyTable>;

#endif

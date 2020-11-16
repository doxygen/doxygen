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
#include "ftextstream.h"

#include "dotgraph.h"
#include "dotnode.h"

/** Represents a graphical class hierarchy */
class DotGfxHierarchyTable : public DotGraph
{
  public:
    DotGfxHierarchyTable(const char *prefix="",ClassDef::CompoundType ct=ClassDef::Class);
    void createGraph(DotNode *rootNode,FTextStream &t,const char *path,
                     const char *fileName,int id);
    void writeGraph(FTextStream &t,const char *path, const char *fileName);
    const std::vector<DotNode*> subGraphs() const { return m_rootSubgraphs; }

  protected:
    virtual QCString getBaseName() const;
    virtual QCString getMapLabel() const;
    virtual void computeTheGraph();

  private:
    void addHierarchy(DotNode *n,const ClassDef *cd,ClassDefSet &visited);
    void addClassList(const ClassSDict *cl,ClassDefSet &visited);

    using DotNodeMap = std::unordered_multimap< std::string, std::unique_ptr<DotNode> >;
    int                    m_graphId;
    QCString               m_prefix;
    ClassDef::CompoundType m_classType;
    std::vector<DotNode*>  m_rootNodes;
    DotNodeMap             m_usedNodes;
    std::vector<DotNode*>  m_rootSubgraphs;
    DotNode *              m_rootSubgraphNode;
};


#endif

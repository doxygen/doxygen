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

#ifndef DOTINCLDEPGRAPH_H
#define DOTINCLDEPGRAPH_H

#include <memory>

#include "qcstring.h"
#include "filedef.h"

#include "dotnode.h"
#include "dotgraph.h"

class TextStream;

/** Representation of an include dependency graph */
class DotInclDepGraph : public DotGraph
{
  public:
    DotInclDepGraph(const FileDef *fd,bool inverse);
    ~DotInclDepGraph() override;
    NON_COPYABLE(DotInclDepGraph)

    QCString writeGraph(TextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                        const QCString &path,const QCString &fileName,const QCString &relPath,
                        bool writeImageMap=TRUE,int graphId=-1);
    bool isTrivial() const;
    bool isTooBig() const;
    int numNodes() const;
    void writeXML(TextStream &t);
    void writeDocbook(TextStream &t);

  protected:
    QCString getBaseName() const override;
    QCString getMapLabel() const override;
    void computeTheGraph() override;

  private:
    QCString diskName() const;
    void buildGraph(DotNode *n,const FileDef *fd,int distance);
    void determineVisibleNodes(DotNodeDeque &queue,int &maxNodes);
    void determineTruncatedNodes(DotNodeDeque &queue);

    DotNode        *m_startNode;
    DotNodeMap      m_usedNodes;
    QCString        m_inclDepFileName;
    QCString        m_inclByDepFileName;
    bool            m_inverse;
};

using DotInclDepGraphPtr = std::shared_ptr<DotInclDepGraph>;

#endif

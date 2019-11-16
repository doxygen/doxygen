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

#include "qcstring.h"
#include "filedef.h"

#include "dotgraph.h"

/** Representation of an include dependency graph */
class DotInclDepGraph : public DotGraph
{
  public:
    DotInclDepGraph(const FileDef *fd,bool inverse);
    ~DotInclDepGraph();
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                        const char *path,const char *fileName,const char *relPath,
                        bool writeImageMap=TRUE,int graphId=-1);
    bool isTrivial() const;
    bool isTooBig() const;
    void writeXML(FTextStream &t);
    void writeDocbook(FTextStream &t);

  protected:
    virtual QCString getBaseName() const;
    virtual QCString getMapLabel() const;
    virtual void computeTheGraph();

  private:
    QCString diskName() const;
    void buildGraph(DotNode *n,const FileDef *fd,int distance);
    void determineVisibleNodes(QList<DotNode> &queue,int &maxNodes);
    void determineTruncatedNodes(QList<DotNode> &queue);

    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    QCString        m_inclDepFileName;
    QCString        m_inclByDepFileName;
    bool            m_inverse;
    int             m_maxDepth;
    int             m_maxNodes;
};

#endif

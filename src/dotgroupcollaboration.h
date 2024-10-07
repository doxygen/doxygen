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

#ifndef DOTGROUPCOLLABORATION_H
#define DOTGROUPCOLLABORATION_H

#include <memory>

#include "dotnode.h"
#include "dotgraph.h"
#include "groupdef.h"

class TextStream;

/** Representation of a group collaboration graph */
class DotGroupCollaboration : public DotGraph
{
  public :
    DotGroupCollaboration(const GroupDef* gd);
   ~DotGroupCollaboration() override;
    NON_COPYABLE(DotGroupCollaboration)

    QCString writeGraph(TextStream &t, GraphOutputFormat gf,EmbeddedOutputFormat ef,
                        const QCString &path,const QCString &fileName,const QCString &relPath,
                        bool writeImageMap=TRUE,int graphId=-1);
    bool isTrivial() const;
    bool isTooBig() const;
    int numNodes() const;

  protected:
    QCString getBaseName() const override;
    QCString getMapLabel() const override;
    void computeTheGraph() override;

  private :
    enum EdgeType
    {
      tmember = 0,
      tclass,
      tnamespace,
      tfile,
      tpages,
      tdir,
      thierarchy
    };

    struct Link
    {
      Link(const QCString &lab,const QCString &u) : label(lab), url(u) {}
      QCString label;
      QCString url;
    };

    struct Edge
    {
      Edge(DotNode *start,DotNode *end,EdgeType type)
        : pNStart(start), pNEnd(end), eType(type) {}

      DotNode* pNStart;
      DotNode* pNEnd;
      EdgeType eType;

      std::vector<Link> links;
      void write( TextStream &t ) const;
    };

    void buildGraph(const GroupDef* gd);
    void addCollaborationMember(const Definition* def, QCString& url, EdgeType eType );
    void addMemberList( class MemberList* ml );
    void writeGraphHeader(TextStream &t,const QCString &title) const;
    Edge* addEdge( DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
        const QCString& _label, const QCString& _url );

    DotNode        *m_rootNode;
    DotNodeMap      m_usedNodes;
    QCString        m_diskName;
    std::vector< std::unique_ptr<Edge> >     m_edges;
};

using DotGroupCollaborationPtr = std::shared_ptr<DotGroupCollaboration>;

#endif

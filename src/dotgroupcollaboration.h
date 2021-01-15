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

#include "dotnode.h"
#include "dotgraph.h"
#include "groupdef.h"

/** Representation of a group collaboration graph */
class DotGroupCollaboration : public DotGraph
{
  public :
    DotGroupCollaboration(const GroupDef* gd);
   ~DotGroupCollaboration();
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf,EmbeddedOutputFormat ef,
                        const char *path,const char *fileName,const char *relPath,
                        bool writeImageMap=TRUE,int graphId=-1);
    bool isTrivial() const;

  protected:
    virtual QCString getBaseName() const;
    virtual QCString getMapLabel() const;
    virtual void computeTheGraph();

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
      Link(const QCString lab,const QCString &u) : label(lab), url(u) {}
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
      void write( FTextStream &t ) const;
    };

    void buildGraph(const GroupDef* gd);
    void addCollaborationMember(const Definition* def, QCString& url, EdgeType eType );
    void addMemberList( class MemberList* ml );
    void writeGraphHeader(FTextStream &t,const QCString &title) const;
    Edge* addEdge( DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
        const QCString& _label, const QCString& _url );

    DotNode        *m_rootNode;
    DotNodeMap      m_usedNodes;
    QCString        m_diskName;
    std::vector< std::unique_ptr<Edge> >     m_edges;
};

#endif

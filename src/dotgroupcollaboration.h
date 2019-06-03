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

#ifndef _DOT_GROUP_COLLABORATION_H
#define _DOT_GROUP_COLLABORATION_H

#include "dotgraph.h"
#include "qlist.h"
#include "groupdef.h"

/** Representation of a group collaboration graph */
class DotGroupCollaboration : public DotGraph
{
public :
  enum EdgeType 
  {  tmember = 0,
    tclass,
    tnamespace,
    tfile,
    tpages,
    tdir,
    thierarchy 
  };

  class Link
  {
  public:
    Link(const QCString lab,const QCString &u) : label(lab), url(u) {}
    QCString label;
    QCString url;
  };

  class Edge
  {
  public :
    Edge(DotNode *start,DotNode *end,EdgeType type) 
      : pNStart(start), pNEnd(end), eType(type)
    { links.setAutoDelete(TRUE); }

    DotNode* pNStart;
    DotNode* pNEnd;
    EdgeType eType;

    QList<Link> links;
    void write( FTextStream &t ) const;
  };

  DotGroupCollaboration(const GroupDef* gd);
  ~DotGroupCollaboration();
  QCString writeGraph(FTextStream &t, GraphOutputFormat gf,EmbeddedOutputFormat ef,
    const char *path,const char *fileName,const char *relPath,
    bool writeImageMap=TRUE,int graphId=-1);
  void buildGraph(const GroupDef* gd);
  bool isTrivial() const;

protected:
  virtual QCString getBaseName() const;
  virtual QCString getMapLabel() const;
  virtual void computeTheGraph();

private :
  void addCollaborationMember(const Definition* def, QCString& url, EdgeType eType );
  void addMemberList( class MemberList* ml );
  void writeGraphHeader(FTextStream &t,const QCString &title) const;
  Edge* addEdge( DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
    const QCString& _label, const QCString& _url );

  DotNode        *m_rootNode;
  QDict<DotNode> *m_usedNodes;
  QCString        m_diskName;
  QList<Edge>     m_edges;
};

#endif

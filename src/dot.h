/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

class ClassDef;
class FileDef;
class QTextStream;
class DotNodeList;

struct EdgeInfo
{
  enum Colors { Blue=0, Green=1, Red=2, Black=3, Grey=4 };
  enum Styles { Solid=0, Dashed=1 };
  EdgeInfo() : m_color(0), m_style(0), m_labColor(0) {}
 ~EdgeInfo() {}
  int m_color;
  int m_style;
  QCString m_label;
  QCString m_url;
  int m_labColor;
};

class DotNode
{
  friend class DotGfxHierarchyTable;
  friend class DotGfxUsageGraph;
  friend class DotInclDepGraph;
  friend class DotNodeList;
  public:
    DotNode(int n,const char *lab,const char *url,int distance = 0,bool rootNode=FALSE);
   ~DotNode();
    void addChild(DotNode *n,
                  int edgeColor=EdgeInfo::Black,
                  int edgeStyle=EdgeInfo::Solid,
                  const char *edgeLab=0,
                  const char *edgeURL=0,
                  int edgeLabCol=-1
                 );
    void setDistance(int distance);
    void addParent(DotNode *n);
    void deleteNode();
    void removeChild(DotNode *n);
    void removeParent(DotNode *n);
    int  number() const { return m_number; }
    void write(QTextStream &t,int maxDistance=1000);
    int  m_subgraphId;
    void clearWriteFlag();

  private:
    void colorConnectedNodes(int curColor);
    const DotNode   *findRoot() const; // only works for acyclic graphs!
    int              m_number;
    QCString         m_label;     
    QCString         m_url;       
    QList<DotNode>  *m_parents;   //!< list of parent nodes (incoming arrows)
    QList<DotNode>  *m_children;  //!< list of child nodes (outgoing arrows)
    QList<EdgeInfo> *m_edgeInfo;  //!< edge info for each child
    bool             m_deleted;   //!< used to mark a node as deleted
    bool             m_written;   //!< used to mark a node as written
    int              m_distance;  //!< distance to the root node
    bool             m_isRoot;    //!< indicates if this is a root node
};

class DotGfxHierarchyTable
{
  public:
    DotGfxHierarchyTable();
   ~DotGfxHierarchyTable();
    void writeGraph(QTextStream &t,const char *path);
  
  private:
    void addHierarchy(DotNode *n,ClassDef *cd);

    QList<DotNode> *m_rootNodes; 
    QDict<DotNode> *m_usedNodes; 
    static int      m_curNodeNumber;
    DotNodeList    *m_rootSubgraphs;
};

class DotGfxUsageGraph
{
  public:
    DotGfxUsageGraph(ClassDef *cd,bool impl,int maxRecusionDepth=1000);
   ~DotGfxUsageGraph();
    bool isTrivial() const;
    void writeGraph(QTextStream &t,const char *path,bool TBRank=TRUE);
    QCString diskName() const;

  private:
    void buildGraph(ClassDef *cd,DotNode *n,int level);
    void addClass(ClassDef *cd,DotNode *n,int prot,const char *label,int level);
    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    static int      m_curNodeNumber;
    bool            m_impl;
    int             m_recDepth;
    QCString        m_diskName;
    int             m_maxDistance;
};

class DotInclDepGraph
{
  public:
    DotInclDepGraph(FileDef *fd);
   ~DotInclDepGraph();
    void writeGraph(QTextStream &t, const char *path);
    bool isTrivial() const;
    QCString diskName() const;

  private:
    void buildGraph(DotNode *n,FileDef *fd,int distance);
    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    static int      m_curNodeNumber;
    QCString        m_diskName;
    int             m_maxDistance;
};

/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include "sortdict.h"

class ClassDef;
class FileDef;
class QTextStream;
class DotNodeList;
class ClassSDict;

enum GraphOutputFormat { BITMAP , EPS };

struct EdgeInfo
{
  enum Colors { Blue=0, Green=1, Red=2, Purple=3, Grey=4, Orange=5 };
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
  friend class DotClassGraph;
  friend class DotInclDepGraph;
  friend class DotNodeList;
  friend void writeDotGraph(
                      DotNode *root,
                      GraphOutputFormat f,
                      const QCString &baseName,
                      bool lrRank,
                      bool renderParents,
                      int distance,
                      bool backArrow
                     );
  public:
    DotNode(int n,const char *lab,const char *url,int distance = 0,bool rootNode=FALSE);
   ~DotNode();
    void addChild(DotNode *n,
                  int edgeColor=EdgeInfo::Purple,
                  int edgeStyle=EdgeInfo::Solid,
                  const char *edgeLab=0,
                  const char *edgeURL=0,
                  int edgeLabCol=-1
                 );
    void setDistance(int distance);
    void addParent(DotNode *n);
    void deleteNode(DotNodeList &deletedList,SIntDict<DotNode> *skipNodes=0);
    void removeChild(DotNode *n);
    void removeParent(DotNode *n);
    void write(QTextStream &t,GraphOutputFormat f,bool topDown,bool toChildren,
               int maxDistance=1000,bool backArrows=TRUE);
    int  m_subgraphId;
    void clearWriteFlag();
    void writeXML(QTextStream &t,bool isClassGraph);
    void writeDEF(QTextStream &t);
    QCString label() const { return m_label; }
    int  number() const { return m_number; }

  private:
    void colorConnectedNodes(int curColor);
    void writeBox(QTextStream &t,GraphOutputFormat f,
                  bool hasNonReachableChildren);
    void writeArrow(QTextStream &t,GraphOutputFormat f,DotNode *cn,
                    EdgeInfo *ei,bool topDown, bool pointBack=TRUE);
    const DotNode   *findDocNode() const; // only works for acyclic graphs!
    int              m_number;
    QCString         m_label;     //!< label text
    QCString         m_url;       //!< url of the node (format: remote$local)
    QList<DotNode>  *m_parents;   //!< list of parent nodes (incoming arrows)
    QList<DotNode>  *m_children;  //!< list of child nodes (outgoing arrows)
    QList<EdgeInfo> *m_edgeInfo;  //!< edge info for each child
    bool             m_deleted;   //!< used to mark a node as deleted
    bool             m_written;   //!< used to mark a node as written
    bool             m_hasDoc;    //!< used to mark a node as documented
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
    void addHierarchy(DotNode *n,ClassDef *cd,bool hide);
    void addClassList(ClassSDict *cl);

    QList<DotNode> *m_rootNodes; 
    QDict<DotNode> *m_usedNodes; 
    static int      m_curNodeNumber;
    DotNodeList    *m_rootSubgraphs;
};

class DotClassGraph
{
  public:
    enum GraphType { Interface, Implementation, Inheritance };
    DotClassGraph(ClassDef *cd,GraphType t,int maxRecusionDepth=1000);
   ~DotClassGraph();
    bool isTrivial() const;
    QCString writeGraph(QTextStream &t,GraphOutputFormat f,const char *path,
                    bool TBRank=TRUE,bool imageMap=TRUE);

    void writeXML(QTextStream &t);
    void writeDEF(QTextStream &t);
    QCString diskName() const;

  private:
    void buildGraph(ClassDef *cd,DotNode *n,int level,bool base);
    void addClass(ClassDef *cd,DotNode *n,int prot,const char *label,
                  int level,const char *usedName,const char *templSpec,
                  bool base);
    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    static int      m_curNodeNumber;
    GraphType       m_graphType;
    int             m_recDepth;
    QCString        m_diskName;
    int             m_maxDistance;
};

class DotInclDepGraph
{
  public:
    DotInclDepGraph(FileDef *fd,bool inverse=FALSE);
   ~DotInclDepGraph();
    QCString writeGraph(QTextStream &t, GraphOutputFormat f,const char *path,
                    bool writeImageMap=TRUE);
    bool isTrivial() const;
    QCString diskName() const;
    void writeXML(QTextStream &t);

  private:
    void buildGraph(DotNode *n,FileDef *fd,int distance);
    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    static int      m_curNodeNumber;
    QCString        m_diskName;
    int             m_maxDistance;
    bool            m_inverse;
};

void generateGraphLegend(const char *path);
void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format);


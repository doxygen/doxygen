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

#ifndef _DOT_H
#define _DOT_H

#include <qlist.h>
#include <qdict.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <qqueue.h>
#include <qthread.h>
#include "sortdict.h"

class ClassDef;
class FileDef;
class FTextStream;
class DotNodeList;
class ClassSDict;
class MemberDef;
class Definition;
class DirDef;
class GroupDef;
class DotGroupCollaboration;
class DotRunnerQueue;

enum GraphOutputFormat    { GOF_BITMAP, GOF_EPS };
enum EmbeddedOutputFormat { EOF_Html, EOF_LaTeX, EOF_Rtf, EOF_DocBook };

/** Attributes of an edge of a dot graph */
struct EdgeInfo
{
  enum Colors { Blue=0, Green=1, Red=2, Purple=3, Grey=4, Orange=5, Orange2=6 };
  enum Styles { Solid=0, Dashed=1 };
  EdgeInfo() : m_color(0), m_style(0), m_labColor(0) {}
 ~EdgeInfo() {}
  int m_color;
  int m_style;
  QCString m_label;
  QCString m_url;
  int m_labColor;
};

/** A node in a dot graph */
class DotNode
{
  public:
    enum GraphType { Dependency, Inheritance, Collaboration, Hierarchy, CallGraph };
    enum TruncState { Unknown, Truncated, Untruncated };
    DotNode(int n,const char *lab,const char *tip,const char *url,
            bool rootNode=FALSE,ClassDef *cd=0);
   ~DotNode();
    void addChild(DotNode *n,
                  int edgeColor=EdgeInfo::Purple,
                  int edgeStyle=EdgeInfo::Solid,
                  const char *edgeLab=0,
                  const char *edgeURL=0,
                  int edgeLabCol=-1
                 );
    void addParent(DotNode *n);
    void deleteNode(DotNodeList &deletedList,SDict<DotNode> *skipNodes=0);
    void removeChild(DotNode *n);
    void removeParent(DotNode *n);
    int findParent( DotNode *n );
    void write(FTextStream &t,GraphType gt,GraphOutputFormat f,
               bool topDown,bool toChildren,bool backArrows);
    int  m_subgraphId;
    void clearWriteFlag();
    void writeXML(FTextStream &t,bool isClassGraph);
    void writeDocbook(FTextStream &t,bool isClassGraph);
    void writeDEF(FTextStream &t);
    QCString label() const { return m_label; }
    int  number() const { return m_number; }
    bool isVisible() const { return m_visible; }
    TruncState isTruncated() const { return m_truncated; }
    int distance() const { return m_distance; }
    void renumberNodes(int &number);

  private:
    void colorConnectedNodes(int curColor);
    void writeBox(FTextStream &t,GraphType gt,GraphOutputFormat f,
                  bool hasNonReachableChildren);
    void writeArrow(FTextStream &t,GraphType gt,GraphOutputFormat f,DotNode *cn,
                    EdgeInfo *ei,bool topDown, bool pointBack=TRUE);
    void setDistance(int distance);
    const DotNode   *findDocNode() const; // only works for acyclic graphs!
    void markAsVisible(bool b=TRUE) { m_visible=b; }
    void markAsTruncated(bool b=TRUE) { m_truncated=b ? Truncated : Untruncated; }
    int              m_number;
    QCString         m_label;     //!< label text
    QCString         m_tooltip;   //!< node's tooltip
    QCString         m_url;       //!< url of the node (format: remote$local)
    QList<DotNode>  *m_parents;   //!< list of parent nodes (incoming arrows)
    QList<DotNode>  *m_children;  //!< list of child nodes (outgoing arrows)
    QList<EdgeInfo> *m_edgeInfo;  //!< edge info for each child
    bool             m_deleted;   //!< used to mark a node as deleted
    bool             m_written;   //!< used to mark a node as written
    bool             m_hasDoc;    //!< used to mark a node as documented
    bool             m_isRoot;    //!< indicates if this is a root node
    ClassDef *       m_classDef;  //!< class representing this node (can be 0)
    bool             m_visible;   //!< is the node visible in the output
    TruncState       m_truncated; //!< does the node have non-visible children/parents
    int              m_distance;  //!< shortest path to the root node

    friend class DotGfxHierarchyTable;
    friend class DotClassGraph;
    friend class DotInclDepGraph;
    friend class DotNodeList;
    friend class DotCallGraph;
    friend class DotGroupCollaboration;
    friend class DotInheritanceGraph;

    friend QCString computeMd5Signature(
                      DotNode *root, GraphType gt,
                      GraphOutputFormat f, 
                      bool lrRank, bool renderParents,
                      bool backArrows,
                      const QCString &title,
                      QCString &graphStr
                     );
};

/** Class representing a list of DotNode objects. */
class DotNodeList : public QList<DotNode>
{
  public:
    DotNodeList() : QList<DotNode>() {}
   ~DotNodeList() {}
  private:
    int compareValues(const DotNode *n1,const DotNode *n2) const;
};

/** Represents a graphical class hierarchy */
class DotGfxHierarchyTable
{
  public:
    DotGfxHierarchyTable();
   ~DotGfxHierarchyTable();
    void writeGraph(FTextStream &t,const char *path, const char *fileName) const;
    void createGraph(DotNode *rootNode,FTextStream &t,const char *path,const char *fileName,int id) const;
    const DotNodeList *subGraphs() const { return m_rootSubgraphs; }
  
  private:
    void addHierarchy(DotNode *n,ClassDef *cd,bool hide);
    void addClassList(ClassSDict *cl);

    QList<DotNode> *m_rootNodes; 
    QDict<DotNode> *m_usedNodes; 
    int             m_curNodeNumber;
    DotNodeList    *m_rootSubgraphs;
};

/** Representation of a class inheritance or dependency graph */
class DotClassGraph
{
  public:
    DotClassGraph(ClassDef *cd,DotNode::GraphType t);
   ~DotClassGraph();
    bool isTrivial() const;
    bool isTooBig() const;
    QCString writeGraph(FTextStream &t,GraphOutputFormat gf,EmbeddedOutputFormat ef,
                    const char *path, const char *fileName, const char *relPath,
                    bool TBRank=TRUE,bool imageMap=TRUE,int graphId=-1) const;

    void writeXML(FTextStream &t);
    void writeDocbook(FTextStream &t);
    void writeDEF(FTextStream &t);
    static void resetNumbering();

  private:
    void buildGraph(ClassDef *cd,DotNode *n,bool base,int distance);
    bool determineVisibleNodes(DotNode *rootNode,int maxNodes,bool includeParents);
    void determineTruncatedNodes(QList<DotNode> &queue,bool includeParents);
    void addClass(ClassDef *cd,DotNode *n,int prot,const char *label,
                  const char *usedName,const char *templSpec,
                  bool base,int distance);

    DotNode        *   m_startNode;
    QDict<DotNode> *   m_usedNodes;
    static int         m_curNodeNumber;
    DotNode::GraphType m_graphType;
    QCString           m_collabFileName;
    QCString           m_inheritFileName;
    bool               m_lrRank;
};

/** Representation of an include dependency graph */
class DotInclDepGraph
{
  public:
    DotInclDepGraph(FileDef *fd,bool inverse);
   ~DotInclDepGraph();
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                    const char *path,const char *fileName,const char *relPath,
                    bool writeImageMap=TRUE,int graphId=-1) const;
    bool isTrivial() const;
    bool isTooBig() const;
    QCString diskName() const;
    void writeXML(FTextStream &t);
    void writeDocbook(FTextStream &t);
    static void resetNumbering();

  private:
    void buildGraph(DotNode *n,FileDef *fd,int distance);
    void determineVisibleNodes(QList<DotNode> &queue,int &maxNodes);
    void determineTruncatedNodes(QList<DotNode> &queue);

    DotNode        *m_startNode;
    QDict<DotNode> *m_usedNodes;
    static int      m_curNodeNumber;
    QCString        m_inclDepFileName;
    QCString        m_inclByDepFileName;
    bool            m_inverse;
};

/** Representation of an call graph */
class DotCallGraph
{
  public:
    DotCallGraph(MemberDef *md,bool inverse);
   ~DotCallGraph();
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf, EmbeddedOutputFormat ef,
                        const char *path,const char *fileName,
                        const char *relPath,bool writeImageMap=TRUE,
                        int graphId=-1) const;
    void buildGraph(DotNode *n,MemberDef *md,int distance);
    bool isTrivial() const;
    bool isTooBig() const;
    void determineVisibleNodes(QList<DotNode> &queue, int &maxNodes);
    void determineTruncatedNodes(QList<DotNode> &queue);
    static void resetNumbering();

  private:
    DotNode        *m_startNode;
    static int             m_curNodeNumber;
    QDict<DotNode> *m_usedNodes;
    bool            m_inverse;
    QCString        m_diskName;
    Definition *    m_scope;
};

/** Representation of an directory dependency graph */
class DotDirDeps
{
  public:
    DotDirDeps(DirDef *dir);
   ~DotDirDeps();
    bool isTrivial() const;
    QCString writeGraph(FTextStream &out,
                        GraphOutputFormat gf,
                        EmbeddedOutputFormat ef,
                        const char *path,
                        const char *fileName,
                        const char *relPath,
                        bool writeImageMap=TRUE,
                        int graphId=-1,
                        bool linkRelations=TRUE) const;
  private:
    DirDef *m_dir;
};

/** Representation of a group collaboration graph */
class DotGroupCollaboration
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

    DotGroupCollaboration(GroupDef* gd);
    ~DotGroupCollaboration();
    QCString writeGraph(FTextStream &t, GraphOutputFormat gf,EmbeddedOutputFormat ef,
                    const char *path,const char *fileName,const char *relPath,
                    bool writeImageMap=TRUE,int graphId=-1) const;
    void buildGraph(GroupDef* gd);
    bool isTrivial() const;
    static void resetNumbering();

  private :
    void addCollaborationMember( Definition* def, QCString& url, EdgeType eType );
    void addMemberList( class MemberList* ml );
    void writeGraphHeader(FTextStream &t,const QCString &title) const;
    Edge* addEdge( DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
                   const QCString& _label, const QCString& _url );

    DotNode        *m_rootNode;
    static int      m_curNodeNumber;
    QDict<DotNode> *m_usedNodes;
    QCString        m_diskName;
    QList<Edge>     m_edges;
};

/** Minimal constant string class that is thread safe, once initialized. */
class DotConstString
{
  public:
    DotConstString()                                   { m_str=0; }
   ~DotConstString()                                   { delete[] m_str; }
    DotConstString(const QCString &s) : m_str(0)       { set(s); }
    DotConstString(const DotConstString &s) : m_str(0) { set(s.data()); }
    const char *data() const                           { return m_str; }
    bool isEmpty() const                               { return m_str==0 || m_str[0]=='\0'; }
    void set(const QCString &s)
    {
      delete[] m_str;
      m_str=0;
      if (!s.isEmpty())
      {
        m_str=new char[s.length()+1];
        qstrcpy(m_str,s.data());
      }
    }
  private:
    DotConstString &operator=(const DotConstString &);
    char *m_str;
};

/** Helper class to run dot from doxygen.
 */
class DotRunner
{
  public:
    struct CleanupItem
    {
      DotConstString path;
      DotConstString file;
    };

    /** Creates a runner for a dot \a file. */
    DotRunner(const QCString &file,const QCString &fontPath,bool checkResult,
        const QCString &imageName = QCString());

    /** Adds an additional job to the run.
     *  Performing multiple jobs one file can be faster.
     */
    void addJob(const char *format,const char *output);

    void addPostProcessing(const char *cmd,const char *args);

    void preventCleanUp() { m_cleanUp = FALSE; }

    /** Runs dot for all jobs added. */
    bool run();
    const CleanupItem &cleanup() const { return m_cleanupItem; }

  private:
    DotConstString m_dotExe;
    bool m_multiTargets;
    QList<DotConstString> m_jobs;
    DotConstString m_postArgs;
    DotConstString m_postCmd;
    DotConstString m_file;
    DotConstString m_path;
    bool m_checkResult;
    DotConstString m_imageName;
    DotConstString m_imgExt;
    bool m_cleanUp;
    CleanupItem m_cleanupItem;
};

/** Helper class to insert a set of map file into an output file */
class DotFilePatcher
{
  public:
    struct Map
    {
      QCString mapFile;
      QCString relPath;
      bool     urlOnly;
      QCString context;
      QCString label;
      bool     zoomable;
      int      graphId;
    };
    DotFilePatcher(const char *patchFile);
    int addMap(const QCString &mapFile,const QCString &relPath,
               bool urlOnly,const QCString &context,const QCString &label);
    int addFigure(const QCString &baseName,
                  const QCString &figureName,bool heightCheck);
    int addSVGConversion(const QCString &relPath,bool urlOnly,
                         const QCString &context,bool zoomable,int graphId);
    int addSVGObject(const QCString &baseName, const QCString &figureName,
                     const QCString &relPath);
    bool run();
    QCString file() const;

  private:
    QList<Map> m_maps;
    QCString m_patchFile;
};

/** Queue of dot jobs to run. */
class DotRunnerQueue
{
  public:
    void enqueue(DotRunner *runner);
    DotRunner *dequeue();
    uint count() const;
  private:
    QWaitCondition  m_bufferNotEmpty;
    QQueue<DotRunner> m_queue;
    mutable QMutex  m_mutex;
};

/** Worker thread to execute a dot run */
class DotWorkerThread : public QThread
{
  public:
    DotWorkerThread(DotRunnerQueue *queue);
    void run();
    void cleanup();
  private:
    DotRunnerQueue *m_queue;
    QList<DotRunner::CleanupItem> m_cleanupItems;
};

/** Singleton that manages dot relation actions */
class DotManager
{
  public:
    static DotManager *instance();
    void addRun(DotRunner *run);
    int  addMap(const QCString &file,const QCString &mapFile,
                const QCString &relPath,bool urlOnly,
                const QCString &context,const QCString &label);
    int addFigure(const QCString &file,const QCString &baseName,
                  const QCString &figureName,bool heightCheck);
    int addSVGConversion(const QCString &file,const QCString &relPath,
               bool urlOnly,const QCString &context,bool zoomable,int graphId);
    int addSVGObject(const QCString &file,const QCString &baseName,
                     const QCString &figureNAme,const QCString &relPath);
    bool run();

  private:
    DotManager();
    virtual ~DotManager();
    QList<DotRunner>       m_dotRuns;
    SDict<DotFilePatcher> m_dotMaps;
    static DotManager     *m_theInstance;
    DotRunnerQueue        *m_queue;
    QList<DotWorkerThread> m_workers;
};


/** Generated a graphs legend page */
void generateGraphLegend(const char *path);

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format);
void writeDotImageMapFromFile(FTextStream &t,
                              const QCString& inFile, const QCString& outDir,
                              const QCString& relPath,const QCString& baseName,
                              const QCString& context,int graphId=-1);

void resetDotNodeNumbering();

#endif

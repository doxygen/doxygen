/******************************************************************************
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

#ifndef _PLANTUML_H
#define _PLANTUML_H

#include <qlist.h>
#include <qdict.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <qqueue.h>
#include <qthread.h>
#include "sortdict.h"
#include "dot.h"

class ClassDef;
class FileDef;
class FTextStream;
class PlantUMLNodeList;
class ClassSDict;
class MemberDef;
class Definition;
class DirDef;
class GroupDef;
class PlantUMLGroupCollaboration;
class PlantUMLRunnerQueue;

/** Attributes of an edge of a dot graph */
class PlantUMLEdgeInfo
{
  public:
  PlantUMLEdgeInfo() : m_edgeType(Unknown) {}
  ~PlantUMLEdgeInfo() {}
  /* Don't change the order here unless you also change arrowStyleMap
   * in plantuml.cpp
   *
   * Don't add or remove anything unless the OMG formal superstructure
   * specification has changed (this is version 2.4.1 as of
   * 2011-08-06). */
  enum Relation
  {
    Unknown = 0,
    Aggregation,
    Association,
    Composition,
    Dependency,
    Generalization,
    InterfaceRealization,
    Realization,
    Usage,
    PackageMerge,
    PackageImportPublic,
    PackageImportPrivate
  };
  static QCString asString(Relation r);
  QCString getEdgeType() const { return asString(m_edgeType); }
  QCString getArrow() const;
  Relation m_edgeType;
  QCString m_labelLeft;
  QCString m_label;
  QCString m_labelRight;
  QCString m_url;
};

/** Plant UML output image formats */
enum PlantUMLOutputFormat { PUML_BITMAP, PUML_EPS, PUML_SVG };

/** Write a PlantUML compatible file.
 *  @param[in] outDir   the output directory to write the file to.
 *  @param[in] fileName the name of the file. If empty a name will be chosen automatically.
 *  @param[in] content  the contents of the PlantUML file.
 *  @returns The name of the generated file.
 */
QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content);

/** Convert a PlantUML file to an image.
 *  @param[in] baseName the name of the generated file (as returned by writePlantUMLSource())
 *  @param[in] outDir   the directory to write the resulting image into.
 *  @param[in] format   the image format to generate.
 */
void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format);


/** Remove programming language qualifiers from member type names that
 *  aren't particularly useful in UML diagrams.
 *  @param[in] s The member type definition (typically MemberDefImpl::type).
 *  @return "s" stripped of qualifiers. */
QCString stripQualifiers(const QCString& s);

/** A node in a PlantUML graph */
class PlantUMLNode
{
  public:
    enum GraphType { Dependency, Inheritance, Collaboration, Hierarchy, CallGraph };
    enum TruncState { Unknown, Truncated, Untruncated };
    PlantUMLNode(int n,const char *lab,const char *tip,const char *url,
            bool rootNode=FALSE,ClassDef *cd=0);
    PlantUMLNode(const PlantUMLNode& right);
   ~PlantUMLNode();
    void addChild(PlantUMLNode *n,
                  PlantUMLEdgeInfo::Relation relation=PlantUMLEdgeInfo::Usage,
                  const char *edgeLab=0,
                  const char *edgeURL=0,
                  const char *leftLab=0,
                  const char *rightLab=0
                 );
    void addParent(PlantUMLNode *n);
    void deleteNode(PlantUMLNodeList &deletedList,SDict<PlantUMLNode> *skipNodes=0);
    void removeChild(PlantUMLNode *n);
    void removeParent(PlantUMLNode *n);
    int findParent( PlantUMLNode *n );
    void write(FTextStream &t,GraphType gt,PlantUMLOutputFormat f,
               bool topDown,bool toChildren,bool backArrows,bool reNumber);
    int  m_subgraphId;
    void clearWriteFlag();
    void writeXML(FTextStream &t,bool isClassGraph);
    void writeDocbook(FTextStream &t,bool isClassGraph);
    void writeDEF(FTextStream &t);
    QCString label() const { return m_label; }
    int  number() const { return m_number; }
    bool isVisible() const { return m_visible; }
    bool isWritten() const { return m_written; }
    TruncState isTruncated() const { return m_truncated; }
    int distance() const { return m_distance; }

  protected:
    void colorConnectedNodes(int curColor);
    void writeBox(FTextStream &t,GraphType gt);
    void writeArrow(FTextStream &t,PlantUMLNode *cn,PlantUMLEdgeInfo *ei);
    void setDistance(int distance);
    const PlantUMLNode   *findDocNode() const; // only works for acyclic graphs!
    void markAsVisible(bool b=TRUE) { m_visible=b; }
    void markAsTruncated(bool b=TRUE) { m_truncated=b ? Truncated : Untruncated; }
    int              m_number;
    QCString         m_label;     //!< label text
    QCString         m_tooltip;   //!< node's tooltip
    QCString         m_url;       //!< url of the node (format: remote$local)
    QList<PlantUMLNode>  *m_parents;   //!< list of parent nodes (incoming arrows)
    QList<PlantUMLNode>  *m_children;  //!< list of child nodes (outgoing arrows)
    QList<PlantUMLEdgeInfo> *m_edgeInfo;  //!< edge info for each child
    bool             m_deleted;   //!< used to mark a node as deleted
    bool             m_written;   //!< used to mark a node as written
    bool             m_hasDoc;    //!< used to mark a node as documented
    bool             m_isRoot;    //!< indicates if this is a root node
    ClassDef *       m_classDef;  //!< class representing this node (can be 0)
    bool             m_visible;   //!< is the node visible in the output
    TruncState       m_truncated; //!< does the node have non-visible children/parents
    int              m_distance;  //!< shortest path to the root node

    friend class PlantUMLClassGraph;
    friend class PlantUMLNodeList;
    friend class PlantUMLCallGraph;
    friend class PlantUMLGroupCollaboration;

    friend QCString computeMd5Signature(
                      PlantUMLNode *root, GraphType gt,
                      PlantUMLOutputFormat f, 
                      bool renderParents,
                      bool backArrows,
                      const QCString &title,
                      QCString &graphStr
                     );
};

inline int PlantUMLNode::findParent( PlantUMLNode *n )
{
    if( !m_parents )
        return -1;
    return m_parents->find(n);
}


/** Representation of a class inheritance or dependency graph */
class PlantUMLClassGraph
{
  public:
    PlantUMLClassGraph(ClassDef *cd,PlantUMLNode::GraphType t);
    virtual ~PlantUMLClassGraph();
    bool isTrivial() const;
    bool isTooBig() const;
    QCString writeGraph(FTextStream &t,PlantUMLOutputFormat gf,EmbeddedOutputFormat ef,
                    const char *path, const char *fileName, const char *relPath,
                    bool TBRank=TRUE,bool imageMap=TRUE,int graphId=-1) const;

    void writeXML(FTextStream &t);
    void writeDocbook(FTextStream &t);
    void writeDEF(FTextStream &t);
    QCString diskName() const;

  protected:
    void buildGraph(ClassDef *cd,PlantUMLNode *n,bool base,int distance);
    bool determineVisibleNodes(PlantUMLNode *rootNode,int maxNodes,bool includeParents);
    void determineTruncatedNodes(QList<PlantUMLNode> &queue,bool includeParents);
    void addClass(ClassDef *cd,PlantUMLNode *n,
                  PlantUMLEdgeInfo::Relation relation,const char *label,
                  const char *usedName,const char *templSpec,
                  bool base,int distance,
                  const char *leftLab=0,
                  const char *rightLab=0);

    PlantUMLNode        *   m_startNode;
    QDict<PlantUMLNode> *   m_usedNodes;
    static int         m_curNodeNumber;
    PlantUMLNode::GraphType m_graphType;
    QCString           m_diskName;
    bool               m_lrRank;
};

/** Helper class to run PlantUML from doxygen.
 */
class PlantUMLRunner
{
  public:
    struct CleanupItem
    {
      QCString path;
      QCString file;
    };

  PlantUMLRunner();

  bool addFile(const QCString& fileName);
  void clear();
  bool run();
  CleanupItem cleanup() const { return m_cleanupItem; }

  private:
  QCString m_cmd;
  CleanupItem m_cleanupItem;
};

/** Helper class to insert a set of map file into an output file */
class PlantUMLFilePatcher
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
    PlantUMLFilePatcher(const char *patchFile);
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

/** Queue of PlantUML jobs to run. */
class PlantUMLRunnerQueue
{
  public:
    void enqueue(PlantUMLRunner *runner);
    PlantUMLRunner *dequeue();
    uint count() const;
  private:
    QWaitCondition  m_bufferNotEmpty;
    QQueue<PlantUMLRunner> m_queue;
    mutable QMutex  m_mutex;
};

/** Worker thread to execute a PlantUML run */
class PlantUMLWorkerThread : public QThread
{
  public:
    PlantUMLWorkerThread(PlantUMLRunnerQueue *queue);
    void run();
    void cleanup();
  private:
    PlantUMLRunnerQueue *m_queue;
    QList<PlantUMLRunner::CleanupItem> m_cleanupItems;
};

/** Singleton that manages PlantUML relation actions */
class PlantUMLManager
{
  public:
    static PlantUMLManager *instance();
       //void addRun(PlantUMLRunner *run);
    void addRun(const char *format,const char *output);
    int  addMap(const QCString &file,const QCString &mapFile,
                const QCString &relPath,bool urlOnly,
                const QCString &context,const QCString &label);
    int addFigure(const QCString &file,const QCString &baseName,
                  const QCString &figureName,bool heightCheck);
    int addSVGConversion(const QCString &file,const QCString &relPath,
               bool urlOnly,const QCString &context,bool zoomable,int graphId);
    int addSVGObject(const QCString &file,const QCString &baseName,
                     const QCString &figureName,const QCString &relPath);
    bool run();

  private:
    PlantUMLManager();
    virtual ~PlantUMLManager();
    SDict<QList<QCString> >       m_plantUMLRuns;
    SDict<PlantUMLFilePatcher>    m_plantUMLMaps;
    static PlantUMLManager       *m_theInstance;
    PlantUMLRunnerQueue          *m_queue;
    QList<PlantUMLWorkerThread>   m_workers;
};

#endif

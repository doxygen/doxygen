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

#ifndef DOTGRAPH_H
#define DOTGRAPH_H

#include <iostream>
#include <map>

#include "qcstring.h"
#include "dir.h"
#include "construct.h"

class DotNode;
class TextStream;

enum class GraphOutputFormat    { BITMAP, EPS };
enum class EmbeddedOutputFormat { Html, LaTeX, Rtf, DocBook };
enum class GraphType            { Dependency, Inheritance, Collaboration, Hierarchy, CallGraph };

/** A dot graph */
class DotGraph
{
    friend class DotNode;
  public:
    DotGraph() : m_doNotAddImageToIndex(FALSE), m_noDivTag(FALSE),
                 m_zoomable(TRUE), m_urlOnly(FALSE) {}
    virtual ~DotGraph() = default;
    NON_COPYABLE(DotGraph)

  protected:
    /** returns the node number. */
    int getNextNodeNumber() { return ++m_curNodeNumber; }
    /** returns the edge number. */
    int getNextEdgeNumber() { return ++m_curEdgeNumber; }

    QCString writeGraph(TextStream &t,
                        GraphOutputFormat gf,
                        EmbeddedOutputFormat ef,
                        const QCString &path,
                        const QCString &fileName,
                        const QCString &relPath,
                        bool writeImageMap=TRUE,
                        int graphId=-1
                       );

    static void writeGraphHeader(TextStream& t, const QCString& title = QCString());
    static void writeGraphFooter(TextStream& t);
    static void computeGraph(DotNode* root,
                             GraphType gt,
                             GraphOutputFormat format,
                             const QCString& rank, // either "LR", "RL", or ""
                             bool renderParents,
                             bool backArrows,
                             const QCString& title,
                             QCString& graphStr
                            );

    virtual QCString getBaseName() const = 0;
    virtual QCString absMapName()  const { return m_absPath + m_baseName + ".map"; }
    virtual QCString getMapLabel() const = 0;
    virtual QCString getImgAltText() const { return ""; }

    virtual void computeTheGraph() = 0;

    QCString absBaseName() const { return m_absPath + m_baseName; }
    QCString absDotName()  const { return m_absPath + m_baseName + ".dot"; }
    QCString imgName()     const;
    QCString absImgName()  const { return m_absPath + imgName(); }
    QCString relImgName()  const { return m_relPath + imgName(); }

    // the following variables are used while writing the graph to a .dot file
    GraphOutputFormat      m_graphFormat = GraphOutputFormat::BITMAP;
    EmbeddedOutputFormat   m_textFormat = EmbeddedOutputFormat::Html;
    Dir                    m_dir;
    QCString               m_fileName;
    QCString               m_relPath;
    bool                   m_generateImageMap = false;
    int                    m_graphId = 0;

    QCString               m_absPath;
    QCString               m_baseName;
    QCString               m_theGraph;
    bool                   m_regenerate = false;
    bool                   m_doNotAddImageToIndex = false;
    bool                   m_noDivTag = false;
    bool                   m_zoomable = true;
    bool                   m_urlOnly = false;

  private:

    bool prepareDotFile();
    void generateCode(TextStream &t);

    int m_curNodeNumber = 0;
    int m_curEdgeNumber = 0;
};

#endif

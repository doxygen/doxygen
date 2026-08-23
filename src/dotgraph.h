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

#include "construct.h"
#include "dir.h"
#include "dstring.h"

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
    DotGraph() : m_doNotAddImageToIndex(false), m_noDivTag(false),
                 m_zoomable(true), m_urlOnly(false) {}
    virtual ~DotGraph() = default;
    NON_COPYABLE(DotGraph)

  protected:
    /** returns the node number. */
    int getNextNodeNumber() { return ++m_curNodeNumber; }
    /** returns the edge number. */
    int getNextEdgeNumber() { return ++m_curEdgeNumber; }

    DString writeGraph(TextStream &t,
                        GraphOutputFormat gf,
                        EmbeddedOutputFormat ef,
                        const DString &path,
                        const DString &fileName,
                        const DString &relPath,
                        bool writeImageMap=true,
                        int graphId=-1
                       );

    static void writeGraphHeader(TextStream& t, const DString& title = DString());
    static void writeGraphFooter(TextStream& t);
    static void computeGraph(DotNode* root,
                             GraphType gt,
                             GraphOutputFormat format,
                             const DString& rank, // either "LR", "RL", or ""
                             bool renderParents,
                             bool backArrows,
                             const DString& title,
                             DString& graphStr
                            );

    virtual DString getBaseName() const = 0;
    virtual DString absMapName()  const { return m_absPath + m_baseName + ".map"; }
    virtual DString getMapLabel() const = 0;
    virtual DString getImgAltText() const { return ""; }

    virtual void computeTheGraph() = 0;

    DString absBaseName() const { return m_absPath + m_baseName; }
    DString absDotName()  const { return m_absPath + m_baseName + ".dot"; }
    DString imgName()     const;
    DString absImgName()  const { return m_absPath + imgName(); }
    DString relImgName()  const { return m_relPath + imgName(); }

    // the following variables are used while writing the graph to a .dot file
    GraphOutputFormat      m_graphFormat = GraphOutputFormat::BITMAP;
    EmbeddedOutputFormat   m_textFormat = EmbeddedOutputFormat::Html;
    Dir                    m_dir;
    DString               m_fileName;
    DString               m_relPath;
    bool                   m_generateImageMap = false;
    int                    m_graphId = 0;

    DString               m_absPath;
    DString               m_baseName;
    DString               m_theGraph;
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

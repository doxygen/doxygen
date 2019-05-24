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

#include <qcstring.h>
#include <qgstring.h>
#include <qdir.h>

class FTextStream;
class DotNode;

enum GraphOutputFormat    { GOF_BITMAP, GOF_EPS };
enum EmbeddedOutputFormat { EOF_Html, EOF_LaTeX, EOF_Rtf, EOF_DocBook };
enum GraphType            { Dependency, Inheritance, Collaboration, Hierarchy, CallGraph };

/** A dot graph */
class DotGraph
{
  public:
    DotGraph() : m_curNodeNumber(0), m_doNotAddImageToIndex(FALSE), m_noDivTag(FALSE), m_zoomable(TRUE), m_urlOnly(FALSE) {}
    virtual ~DotGraph() {}

    static QCString DOT_FONTNAME; // will be initialized in initDot
    static int DOT_FONTSIZE;      // will be initialized in initDot

    static bool writeVecGfxFigure(FTextStream& out, const QCString& baseName, const QCString& figureName);

  protected:
    /** returns node numbers. The Counter is reset by the constructor */
    int getNextNodeNumber() { return ++m_curNodeNumber; }

    QCString writeGraph(FTextStream &t,
                        GraphOutputFormat gf,
                        EmbeddedOutputFormat ef,
                        const char *path,
                        const char *fileName,
                        const char *relPath,
                        bool writeImageMap=TRUE,
                        int graphId=-1
                       );

    static void writeGraphHeader(FTextStream& t, const QCString& title = QCString());
    static void writeGraphFooter(FTextStream& t);
    static void computeGraph(DotNode* root,
                             GraphType gt,
                             GraphOutputFormat format,
                             const QCString& rank, // either "LR", "RL", or ""
                             bool renderParents,
                             bool backArrows,
                             const QCString& title,
                             QGString& graphStr
                            );

    virtual QCString getBaseName() const = 0;
    virtual QCString absMapName()  const { return m_absPath + m_baseName + ".map"; }
    virtual QCString getMapLabel() const = 0;
    virtual QCString getImgAltText() const { return ""; }

    virtual void computeTheGraph() = 0;

    static QCString IMG_EXT;

    friend void initDot();

    QCString absBaseName() const { return m_absPath + m_baseName; }
    QCString absDotName()  const { return m_absPath + m_baseName + ".dot"; }
    QCString imgName()     const;
    QCString absImgName()  const { return m_absPath + imgName(); }
    QCString relImgName()  const { return m_relPath + imgName(); }

    // the following variables are used while writing the graph to a .dot file
    GraphOutputFormat      m_graphFormat;
    EmbeddedOutputFormat   m_textFormat;
    QDir                   m_dir;
    QCString               m_fileName;
    QCString               m_relPath;
    bool                   m_generateImageMap;
    int                    m_graphId;

    QCString               m_absPath;
    QCString               m_baseName;
    QGString               m_theGraph;
    bool                   m_regenerate;
    bool                   m_doNotAddImageToIndex;
    bool                   m_noDivTag;
    bool                   m_zoomable;
    bool                   m_urlOnly;

  private:
    DotGraph(const DotGraph &);
    DotGraph &operator=(const DotGraph &);

    bool prepareDotFile();
    void generateCode(FTextStream &t);

    int m_curNodeNumber;
};

#endif

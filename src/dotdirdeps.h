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

#ifndef DOTDIRDEPS_H
#define DOTDIRDEPS_H

#include "dotgraph.h"
#include "dirdef.h"

/** Representation of an directory dependency graph */
class DotDirDeps : public DotGraph
{
  public:
    DotDirDeps(const DirDef *dir);
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
                        bool linkRelations=TRUE);

  protected:
    virtual QCString getBaseName() const;
    virtual QCString getMapLabel() const;
    virtual void computeTheGraph();
    virtual QCString getImgAltText() const;

  private:
    const DirDef *m_dir;

    bool m_linkRelations;
};

#endif

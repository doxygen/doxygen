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

#include <memory>

#include "dotgraph.h"
#include "dirdef.h"

/** Representation of an directory dependency graph */
class DotDirDeps : public DotGraph
{
  public:
    DotDirDeps(const DirDef *dir);
    ~DotDirDeps() override;
    NON_COPYABLE(DotDirDeps)

    bool isTrivial() const;
    QCString writeGraph(TextStream &out,
                        GraphOutputFormat gf,
                        EmbeddedOutputFormat ef,
                        const QCString &path,
                        const QCString &fileName,
                        const QCString &relPath,
                        bool writeImageMap=TRUE,
                        int graphId=-1,
                        bool linkRelations=TRUE);

  protected:
    QCString getBaseName() const override;
    QCString getMapLabel() const override;
    void computeTheGraph() override;
    QCString getImgAltText() const override;

  private:
    const DirDef *m_dir = nullptr;

    bool m_linkRelations = false;
};

using DotDirDepsPtr = std::shared_ptr<DotDirDeps>;

#endif

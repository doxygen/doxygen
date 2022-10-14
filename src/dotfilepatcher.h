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

#ifndef DOTFILEPATCHER_H
#define DOTFILEPATCHER_H

#include <vector>

#include "qcstring.h"

class TextStream;

/** Helper class to insert a set of map file into an output file */
class DotFilePatcher
{
  public:
    DotFilePatcher(const QCString &patchFile);
    int addMap(const QCString &mapFile,const QCString &relPath,
               bool urlOnly,const QCString &context,const QCString &label);

    int addFigure(const QCString &baseName,
                  const QCString &figureName,bool heightCheck);

    int addSVGConversion(const QCString &relPath,bool urlOnly,
                         const QCString &context,bool zoomable,int graphId);

    int addSVGObject(const QCString &baseName, const QCString &figureName,
                     const QCString &relPath);
    bool run() const;
    bool isSVGFile() const;

    static bool convertMapFile(TextStream &t,const QCString &mapName,
                               const QCString &relPath, bool urlOnly=FALSE,
                               const QCString &context=QCString());

    static bool writeSVGFigureLink(TextStream &out,const QCString &relPath,
                                   const QCString &baseName,const QCString &absImgName);

    static bool writeVecGfxFigure(TextStream& out, const QCString& baseName,
                                  const QCString& figureName);

  private:
    struct Map
    {
      Map(const QCString &mf,const QCString &rp,bool uo,const QCString &ctx,
          const QCString &lab,bool zoom=false,int gId=-1) :
        mapFile(mf), relPath(rp), urlOnly(uo), context(ctx),
        label(lab), zoomable(zoom), graphId(gId) {}
      QCString mapFile;
      QCString relPath;
      bool     urlOnly;
      QCString context;
      QCString label;
      bool     zoomable;
      int      graphId;
    };
    std::vector<Map> m_maps;
    QCString m_patchFile;
};


#endif

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

#include "dstring.h"

class TextStream;

/** Helper class to insert a set of map file into an output file */
class DotFilePatcher
{
  public:
    DotFilePatcher(const DString &patchFile);
    int addMap(const DString &mapFile,const DString &relPath,
               bool urlOnly,const DString &context,const DString &label);

    int addFigure(const DString &baseName,
                  const DString &figureName,bool heightCheck);

    int addSVGConversion(const DString &relPath,bool urlOnly,
                         const DString &context,bool zoomable,int graphId);

    int addSVGObject(const DString &baseName, const DString &figureName,
                     const DString &relPath);
    bool run() const;
    bool isSVGFile() const;

    static bool convertMapFile(TextStream &t,const DString &mapName,
                               const DString &relPath, bool urlOnly=false,
                               const DString &context=DString());

    static bool writeSVGFigureLink(TextStream &out,const DString &relPath,
                                   const DString &baseName,const DString &absImgName);

    static bool writeVecGfxFigure(TextStream& out, const DString& baseName,
                                  const DString& figureName);

    /*! To adhere to the requirement that an id cannot have a digit at the first position always place an "a" in front */
    static DString mapLabelToId(const DString &mapLabel)
    {
      if (mapLabel.empty()) return mapLabel;
      return "a" + mapLabel;
    }

  private:
    struct Map
    {
      Map(const DString &mf,const DString &rp,bool uo,const DString &ctx,
          const DString &lab,bool zoom=false,int gId=-1) :
        mapFile(mf), relPath(rp), urlOnly(uo), context(ctx),
        label(lab), zoomable(zoom), graphId(gId) {}
      DString mapFile;
      DString relPath;
      bool     urlOnly;
      DString context;
      DString label;
      bool     zoomable;
      int      graphId;
    };
    std::vector<Map> m_maps;
    DString m_patchFile;
};


#endif

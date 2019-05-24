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

#include "qcstring.h"
#include "qlist.h"

/** Helper class to insert a set of map file into an output file */
class DotFilePatcher
{
  public:
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
    QList<Map> m_maps;
    QCString m_patchFile;
};

#endif

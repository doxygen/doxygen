/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <qglobal.h>

class ClassDef;
class TreeDiagram;
class FTextStream;

/** Class representing a built-in class diagram. */
class ClassDiagram
{
  public:
    ClassDiagram(ClassDef *root);
   ~ClassDiagram();
    void writeFigure(FTextStream &t,const char *path,
                     const char *file) const;
    void writeImage(FTextStream &t,const char *path,const char *relPath,
                     const char *file,bool generateMap=TRUE) const;
  private:
    TreeDiagram *base;
    TreeDiagram *super;
};

#endif


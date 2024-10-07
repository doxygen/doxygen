/******************************************************************************
 *
 *
 *
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <memory>
#include "qcstring.h"
#include "construct.h"

class ClassDef;
class TextStream;

/** Class representing a built-in class diagram. */
class ClassDiagram
{
  public:
    ClassDiagram(const ClassDef *root);
   ~ClassDiagram();
    NON_COPYABLE(ClassDiagram)
    void writeFigure(TextStream &t,const QCString &path,
                     const QCString &file) const;
    void writeImage(TextStream &t,const QCString &path,const QCString &relPath,
                     const QCString &file,bool generateMap=true) const;
  private:
    struct Private;
    std::unique_ptr<Private> p;
};

#endif


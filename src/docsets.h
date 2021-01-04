/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef DOCSETS_H
#define DOCSETS_H

#include <memory>

#include "index.h"

class QFile;
class Definition;
class FTextStream;

/** A class that generates docset files.
 *
 *  These files can be used to create context help
 *  for use within Apple's Xcode 3.0 development environment
 */
class DocSets  : public IndexIntf
{

  public:
    DocSets();
   ~DocSets();
    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir,
                         const char *name,
                         const char *ref,
                         const char *file,
                         const char *anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def
                        );
    void addIndexItem(const Definition *context,const MemberDef *md,
                      const char *sectionAnchor,const char *title);
    void addIndexFile(const char *name);
    void addImageFile(const char *) {}
    void addStyleSheetFile(const char *) {}

  private:
    void writeToken(FTextStream &t, const Definition *d,
                    const QCString &type, const QCString &lang,
                    const char *scope=0, const char *anchor=0,
                    const char *decl=0);
    struct Private;
    std::unique_ptr<Private> p;

};

#endif /* DOCSETS_H */


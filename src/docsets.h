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

#include "indexlist.h"
#include "qcstring.h"
#include "construct.h"

class TextStream;

class Definition;
class MemberDef;

/** A class that generates docset files.
 *
 *  These files can be used to create context help
 *  for use within Apple's Xcode 3.0 development environment
 */
class DocSets : public IndexIntf
{
  public:
    DocSets();
    ~DocSets();
    NON_COPYABLE(DocSets)

    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir,
                         const QCString &name,
                         const QCString &ref,
                         const QCString &file,
                         const QCString &anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def,
                         const QCString &nameAsHtml
                        );
    void addIndexItem(const Definition *context,const MemberDef *md,
                      const QCString &sectionAnchor,const QCString &title);
    void addIndexFile(const QCString &name);
    void addImageFile(const QCString &) {}
    void addStyleSheetFile(const QCString &) {}

  private:
    void writeToken(TextStream &t, const Definition *d,
                    const QCString &type, const QCString &lang,
                    const QCString &scope=QCString(), const QCString &anchor=QCString(),
                    const QCString &decl=QCString());
    struct Private;
    std::unique_ptr<Private> p;

};

#endif /* DOCSETS_H */


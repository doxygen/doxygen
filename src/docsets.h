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

#include "construct.h"
#include "indexlist.h"

class DString;
class Definition;
class MemberDef;
class TextStream;

/** A class that generates docset files.
 *
 *  These files can be used to create context help
 *  for use within Apple's Xcode 3.0 development environment
 */
class DocSets final : public IndexIntf
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
                         const DString &name,
                         const DString &ref,
                         const DString &file,
                         const DString &anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def,
                         const DString &nameAsHtml
                        );
    void addIndexItem(const Definition *context,const MemberDef *md,
                      const DString &sectionAnchor,const DString &title);
    void addIndexFile(const DString &name);
    void addImageFile(const DString &) {}
    void addStyleSheetFile(const DString &) {}

  private:
    void writeToken(TextStream &t, const Definition *d,
                    const DString &type, const DString &lang,
                    const DString &scope=DString(), const DString &anchor=DString(),
                    const DString &decl=DString());
    struct Private;
    std::unique_ptr<Private> p;

};

#endif /* DOCSETS_H */


/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef QHP_H
#define QHP_H

#include <memory>

#include "construct.h"
#include "dstring.h"
#include "indexlist.h"

class Definition;
class MemberDef;

class Qhp final : public IndexIntf
{
  public:
    Qhp();
    ~Qhp();
    NON_COPYABLE(Qhp)

    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir, const DString & name, const DString & ref,
                         const DString & file, const DString & anchor,
                         bool separateIndex, bool addToNavIndex,
                         const Definition *def, const DString &);
    void addIndexItem(const Definition *context, const MemberDef *md,
                      const DString &sectionAnchor, const DString &title);
    void addIndexFile(const DString & name);
    void addImageFile(const DString & name);
    void addStyleSheetFile(const DString & name);

    static inline const DString qhpFileName = "index.qhp";
    static DString getQchFileName();

  private:
    void addFile(const DString &);
    class Private;
    std::unique_ptr<Private> p;
};

#endif // QHP_H


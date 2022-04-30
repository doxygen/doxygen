/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

/******************************************************************************
 * ftvhelp.h,v 1.0 2000/09/06 16:09:00
 *
 * Kenney Wong <kwong@ea.com>
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 */

#ifndef FTVHELP_H
#define FTVHELP_H

#include <vector>
#include "index.h"

class Definition;
class TextStream;
struct FTVNode;

/** A class that generates a dynamic tree view side panel.
 */
class FTVHelp : public IndexIntf
{
  public:
    FTVHelp(bool LTI);
    ~FTVHelp();
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
                         const Definition *def);
    void addIndexItem(const Definition *,const MemberDef *,const QCString &,const QCString &) {}
    void addIndexFile(const QCString &) {}
    void addImageFile(const QCString &) {}
    void addStyleSheetFile(const QCString &) {}
    void generateTreeView();
    void generateTreeViewInline(TextStream &t);
    static void generateTreeViewImages();
    void generateTreeViewScripts();
  private:
    void generateTree(TextStream &t,const std::vector<FTVNode*> &nl,int level,int maxLevel,int &index);
    QCString generateIndentLabel(FTVNode *n,int level);
    void generateIndent(TextStream &t,FTVNode *n,bool opened);
    void generateLink(TextStream &t,FTVNode *n);
    std::vector< std::vector<FTVNode*> > m_indentNodes;
    int m_indent;
    bool m_topLevelIndex;
};

extern const char *JAVASCRIPT_LICENSE_TEXT;

#endif /* FTVHELP_H */


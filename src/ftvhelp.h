/******************************************************************************
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

/******************************************************************************
 * ftvhelp.h,v 1.0 2000/09/06 16:09:00
 *
 * Kenney Wong <kwong@ea.com>
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 */

#ifndef FTVHELP_H
#define FTVHELP_H

#include <qlist.h>
#include "index.h"

class QFile;
class Definition;
struct FTVNode;
class FTextStream;

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
                         const char *name,
                         const char *ref,
                         const char *file,
                         const char *anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def);
    void addIndexItem(const Definition *,const MemberDef *,const char *,const char *) {}
    void addIndexFile(const char *) {}
    void addImageFile(const char *) {}
    void addStyleSheetFile(const char *) {}
    void generateTreeView();
    void generateTreeViewInline(FTextStream &t);
    static void generateTreeViewImages();
    void generateTreeViewScripts();
  private:
    void generateTree(FTextStream &t,const QList<FTVNode> &nl,int level,int maxLevel,int &index);
    //bool generateJSTree(FTextStream &tidx,FTextStream &t,const QList<FTVNode> &nl,int level,bool &first);
    //bool generateJSTreeTopLevel(FTextStream &tidx,FTextStream &t,const QList<FTVNode> &nl,int level,bool &first);
    QCString generateIndentLabel(FTVNode *n,int level);
    void generateIndent(FTextStream &t,FTVNode *n,bool opened);
    void generateLink(FTextStream &t,FTVNode *n);
    //void generateJSLink(FTextStream &t,FTVNode *n);
    QList<FTVNode> *m_indentNodes;
    int m_indent;
    bool m_topLevelIndex;
};

#define JAVASCRIPT_LICENSE_TEXT                                                                 \
	"/*\n@ @licstart  The following is the entire license notice for the\n"                 \
	"JavaScript code in this file.\n\nCopyright (C) 1997-2019 by Dimitri van Heesch\n\n"    \
	"This program is free software; you can redistribute it and/or modify\n"                \
	"it under the terms of version 2 of the GNU General Public License as published by\n"   \
	"the Free Software Foundation\n\n"                                                      \
	"This program is distributed in the hope that it will be useful,\n"                     \
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"                      \
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"                        \
	"GNU General Public License for more details.\n\n"                                      \
	"You should have received a copy of the GNU General Public License along\n"             \
	"with this program; if not, write to the Free Software Foundation, Inc.,\n"             \
	"51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n\n"                       \
	"@licend  The above is the entire license notice\n"                                     \
	"for the JavaScript code in this file\n"                                                \
	"*/\n"

#endif /* FTVHELP_H */


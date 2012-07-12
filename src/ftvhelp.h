/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qtextstream.h>
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
                         Definition *def);
    void addIndexItem(Definition *,MemberDef *,const char *) {}
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
    void generateIndent(FTextStream &t,FTVNode *n,int level,bool opened);
    void generateLink(FTextStream &t,FTVNode *n);
    //void generateJSLink(FTextStream &t,FTVNode *n);
    QList<FTVNode> *m_indentNodes;
    int m_indent;
    bool m_topLevelIndex;
};


#endif /* FTVHELP_H */


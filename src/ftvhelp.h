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

static const char *JAVASCRIPT_LICENSE_TEXT = R"LIC(/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
)LIC";


#endif /* FTVHELP_H */


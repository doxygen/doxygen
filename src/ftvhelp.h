/******************************************************************************
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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
struct FTVNode;
class FTextStream;

struct FTVImageInfo
{
  const char *alt;
  const char *name;
  const unsigned char *data;
  unsigned int len;
  unsigned short width, height;
};

extern FTVImageInfo image_info[];

#define FTVIMG_blank        0
#define FTVIMG_doc          1
#define FTVIMG_folderclosed 2
#define FTVIMG_folderopen   3
#define FTVIMG_lastnode     4
#define FTVIMG_link         5
#define FTVIMG_mlastnode    6
#define FTVIMG_mnode        7
#define FTVIMG_node         8
#define FTVIMG_plastnode    9
#define FTVIMG_pnode       10
#define FTVIMG_vertline    11

#define FTV_S(name) #name
#define FTV_ICON_FILE(name) "ftv2" FTV_S(name) ".png"
#define FTVIMG_INDEX(name) FTVIMG_ ## name
#define FTV_INFO(name) ( image_info[FTVIMG_INDEX(name)] )
#define FTV_IMGATTRIBS(name) \
    "src=\"" FTV_ICON_FILE(name) "\" " \
    "alt=\"" << FTV_INFO(name).alt << "\" " \
    "width=\"" << FTV_INFO(name).width << "\" " \
    "height=\"" << FTV_INFO(name).height << "\" "

/*! A class that generates a dynamic tree view side panel.
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
                         const char *anchor);
    //void addIndexItem(const char *, const char *, 
    //                  const char *, const char *,
    //                  const char *, const MemberDef *) {}
    void addIndexItem(Definition *,MemberDef *,const char *,const char *) {}
    void addIndexFile(const char *) {}
    void addImageFile(const char *) {}
    void addStyleSheetFile(const char *) {}
    void generateTreeView();
    void generateTreeViewInline(FTextStream &t);
  private:
    void generateScript(FTextStream &t);
    void generateTreeViewImages();
    void generateTree(FTextStream &t,const QList<FTVNode> &nl,int level);
    void generateIndent(FTextStream &t,FTVNode *n,int level);
    void generateLink(FTextStream &t,FTVNode *n);
    QList<FTVNode> *m_indentNodes;
    int m_indent;
    bool m_topLevelIndex;
};


#endif /* FTVHELP_H */


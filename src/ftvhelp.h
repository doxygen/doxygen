/******************************************************************************
 * ftvhelp.h,v 1.0 2000/09/06 16:09:00
 *
 * Kenney Wong <kwong@ea.com>
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 * Uses the FTV structure from: 
 * http://www.geocities.com/Paris/LeftBank/2178/ftexample.html
 */

#ifndef FTVHELP_H
#define FTVHELP_H

#include "qtbc.h"
#include <qtextstream.h>
#include <qlist.h>

class QFile;
struct FTVNode;

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
    "width=" << FTV_INFO(name).width << " " \
    "height=" << FTV_INFO(name).height << " "

/*! A class that generates a dynamic tree view side panel.
 */
class FTVHelp 
{
  public:
    static FTVHelp *getInstance();
    void initialize();
    void finalize();
    int  incContentsDepth();
    int  decContentsDepth();
    /*! return the current depth of the contents tree */ 
    int  contentsDepth() const { return m_indent; }
    void addContentsItem(bool isDir,
                         const char *ref,
                         const char *file,
                         const char *anchor, 
                         const char *name);
    static void generateTreeViewImages();

  private:
    void generateTreeView();
    void generateTree(QTextStream &t,const QList<FTVNode> &nl,int level);
    void generateIndent(QTextStream &t,FTVNode *n,int level);
    void generateLink(QTextStream &t,FTVNode *n);
    FTVHelp();
   ~FTVHelp();
    //QFile *m_cf; 
    //QTextStream m_cts;
    //int m_dc;
    static FTVHelp *m_theInstance;
    QList<FTVNode> *m_indentNodes;
    int m_indent;
};


#endif /* FTVHELP_H */


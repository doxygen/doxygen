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

/*! A class that generated the FTV Help specific file.
 *  This file is used in conjunction with additional FTV web browser code
 *  that can be obtained from:
 *  http://www.geocities.com/Paris/LeftBank/2178/ftexample.html
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

  private:
    void generateTreeView();
    void generateTree(QTextStream &t,const QList<FTVNode> &nl,int level);
    void generateIndent(QTextStream &t,FTVNode *n,int level);
    void generateLink(QTextStream &t,FTVNode *n);
    FTVHelp();
   ~FTVHelp();
    QFile *m_cf; 
    QTextStream m_cts;
    //int m_dc;
    static FTVHelp *m_theInstance;
    QList<FTVNode> *m_indentNodes;
    int m_indent;
};

#endif /* FTVHELP_H */


/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "entry.h"

class ClassDef;
class DiagramRow;
class TreeDiagram;
class ClassDiagram;
class DiagramItemList;
class Image;

class DiagramItem 
{
  public:
    DiagramItem(DiagramItem *p,int number,ClassDef *cd,
                Protection prot,Specifier virt,const char *ts);
   ~DiagramItem();
    QCString label() const;
    QCString fileName() const;
    DiagramItem *parentItem() { return parent; } 
    DiagramItemList *getChildren() { return children; }
    void move(int dx,int dy) { x+=dx; y+=dy; }
    int xPos() const { return x; }
    int yPos() const { return y; }
    int avgChildPos() const;
    int numChildren() const;
    void addChild(DiagramItem *di);
    int number() const { return num; }
    Protection protection() const { return prot; }
    Specifier virtualness() const { return virt; }
    void putInList() { inList=TRUE; }
    bool isInList() const { return inList; } 
    ClassDef *getClassDef() const { return classDef; }
  private:
    DiagramItemList *children;
    DiagramItem *parent;
    int x,y;
    int num;
    Protection prot;
    Specifier virt;
    QCString templSpec;
    bool inList;
    ClassDef *classDef;
};

class DiagramItemList : public QList<DiagramItem>
{
  public:
    DiagramItemList() : QList<DiagramItem>() {}
   ~DiagramItemList() {}
};

class DiagramRow : public QList<DiagramItem> 
{
  public:
    DiagramRow(TreeDiagram *d,int l) : QList<DiagramItem>() 
    { 
      diagram=d; 
      level=l;
      setAutoDelete(TRUE); 
    }
    void insertClass(DiagramItem *parent,ClassDef *cd,bool doBases,
                     Protection prot,Specifier virt,const char *ts);
    uint number() { return level; }
  private:
    TreeDiagram *diagram;
    uint level;
};

class DiagramRowIterator : public QListIterator<DiagramRow>
{
  public:
    DiagramRowIterator(const QList<DiagramRow> &d) 
      : QListIterator<DiagramRow>(d) {}
};

class TreeDiagram : public QList<DiagramRow>
{
  public:
    TreeDiagram(ClassDef *root,bool doBases);
   ~TreeDiagram();
    void computeLayout();
    uint computeRows();
    //uint computeCols();
    void moveChildren(DiagramItem *root,int dx);
    void computeExtremes(uint *labelWidth,uint *xpos);
    void drawBoxes(QTextStream &t,Image *image,
                   bool doBase,bool bitmap,
                   uint baseRows,uint superRows,
                   uint cellWidth,uint cellHeight,
                   QCString relPath="",
                   bool generateMap=TRUE);
    void drawConnectors(QTextStream &t,Image *image,
                   bool doBase,bool bitmap,
                   uint baseRows,uint superRows,
                   uint cellWidth,uint cellheight);
  private:
    bool layoutTree(DiagramItem *root,int row);
    TreeDiagram &operator=(const TreeDiagram &);
    TreeDiagram(const TreeDiagram &);
};

class ClassDiagram
{
  public:
    ClassDiagram(ClassDef *root);
   ~ClassDiagram();
    void writeFigure(QTextStream &t,const char *path,
                     const char *file);
    void writeImage(QTextStream &t,const char *path,const char *relPath,
                     const char *file,bool generateMap=TRUE);
  private:
    TreeDiagram *base;
    TreeDiagram *super;
};

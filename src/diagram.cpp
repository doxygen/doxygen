/******************************************************************************
 *
 * 
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qarray.h>
#include "ftextstream.h"
#include <qfile.h>

#include "diagram.h"
#include "image.h"
#include "classdef.h"
#include "config.h"
#include "message.h"
#include "util.h"
#include "doxygen.h"
#include "portable.h"
#include "index.h"
#include "classlist.h"

//-----------------------------------------------------------------------------

class DiagramItemList;

/** Class representing a single node in the built-in class diagram */
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

/** Class representing a list of DiagramItem object. */
class DiagramItemList : public QList<DiagramItem>
{
  public:
    DiagramItemList() : QList<DiagramItem>() {}
   ~DiagramItemList() {}
};

/** Class representing a row in the built-in class diagram */
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

/** Class representing iterator for the rows in the built-in class diagram. */
class DiagramRowIterator : public QListIterator<DiagramRow>
{
  public:
    DiagramRowIterator(const QList<DiagramRow> &d) 
      : QListIterator<DiagramRow>(d) {}
};

/** Class represeting the tree layout for the built-in class diagram. */
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
    void drawBoxes(FTextStream &t,Image *image,
                   bool doBase,bool bitmap,
                   uint baseRows,uint superRows,
                   uint cellWidth,uint cellHeight,
                   QCString relPath="",
                   bool generateMap=TRUE);
    void drawConnectors(FTextStream &t,Image *image,
                   bool doBase,bool bitmap,
                   uint baseRows,uint superRows,
                   uint cellWidth,uint cellheight);
  private:
    bool layoutTree(DiagramItem *root,int row);
    TreeDiagram &operator=(const TreeDiagram &);
    TreeDiagram(const TreeDiagram &);
};



//-----------------------------------------------------------------------------

const uint maxTreeWidth = 8;
const int gridWidth  = 100;
const int gridHeight = 100;

const uint labelHorSpacing  = 10;  // horizontal distance between labels
const uint labelVertSpacing = 32;  // vertical distance between labels
const uint labelHorMargin   = 6;   // horiz. spacing between label and box
const uint fontHeight       = 12;  // height of a character

//static QCString escapeLatex(const char *s)
//{
//  QCString result;
//  char c;
//  while ((c=*s++))
//  {
//    if (c=='_') result+="\\_";
//           else result+=c;
//  }
//  return result;
//}

static uint protToMask(Protection p)
{
  switch(p)
  {
    case Public:    return 0xffffffff;
    case Package: // package is not possible!
    case Protected: return 0xcccccccc;
    case Private:   return 0xaaaaaaaa;
  }
  return 0;
}

static uint protToColor(Protection p)
{
  switch(p)
  {
    case Public:    return 6;
    case Package: // package is not possible!
    case Protected: return 5;
    case Private:   return 4;
  }
  return 0;
}

static QCString protToString(Protection p)
{
  switch(p)
  {
    case Public:    return "solid";
    case Package: // package is not possible!
    case Protected: return "dashed";
    case Private:   return "dotted";
  }
  return 0;
}

static uint virtToMask(Specifier p)
{
  switch(p)
  {
    case Normal:    return 0xffffffff;
    case Virtual:   return 0xf0f0f0f0;
    default:        return 0;
  }
  return 0;
}

// pre: dil is not empty
static Protection getMinProtectionLevel(DiagramItemList *dil)
{
  QListIterator<DiagramItem> it(*dil);
  DiagramItem *di=it.current();
  Protection result=di->protection();
  for (++it;(di=it.current());++it)
  {
    Protection p=di->protection();
    if (p!=result)
    {
      if (result==Protected && p==Public) result=p;
      else if (result==Private) result=p;
    }
  }
  return result;
}

static void writeBitmapBox(DiagramItem *di,Image *image,
                           int x,int y,int w,int h,bool firstRow,
                           bool hasDocs,bool children=FALSE)
{
  int colFill = hasDocs ? (firstRow ? 0 : 2) : 7;
  int colBorder = (firstRow || !hasDocs) ? 1 : 3;
  int l = Image::stringLength(di->label());
  uint mask=virtToMask(di->virtualness());
  image->fillRect(x+1,y+1,w-2,h-2,colFill,mask);
  image->drawRect(x,y,w,h,colBorder,mask);
  image->writeString(x+(w-l)/2, y+(h-fontHeight)/2, di->label(),1);
  if (children)
  {
    int i;
    for (i=0;i<5;i++)
      image->drawHorzLine(y+h+i-6,x+w-2-i,x+w-2,firstRow?1:3,0xffffffff);
  }
}

static void writeVectorBox(FTextStream &t,DiagramItem *di,
                           float x,float y,bool children=FALSE)
{
  if (di->virtualness()==Virtual) t << "dashed\n";
  t << " (" << di->label() << ") " << x << " " << y << " box\n";
  if (children) t << x << " " << y << " mark\n";
  if (di->virtualness()==Virtual) t << "solid\n";
}

static void writeMapArea(FTextStream &t,ClassDef *cd,QCString relPath,
                         int x,int y,int w,int h)
{
  if (cd->isLinkable())
  {
    QCString ref=cd->getReference();
    t << "<area ";
    if (!ref.isEmpty()) 
    {
      t << externalLinkTarget() << externalRef(relPath,ref,FALSE);
    }
    t << "href=\"";
    t << externalRef(relPath,ref,TRUE);
    t << cd->getOutputFileBase() << Doxygen::htmlFileExtension;
    if (!cd->anchor().isEmpty())
    {
      t << "#" << cd->anchor();
    }
    t << "\" ";
    QCString tooltip = cd->briefDescriptionAsTooltip();
    if (!tooltip.isEmpty())
    {
      t << "title=\"" << convertToHtml(tooltip) << "\" ";
    }
    t << "alt=\"" << convertToXML(cd->displayName()); 
    t << "\" shape=\"rect\" coords=\"" << x << "," << y << ",";
    t << (x+w) << "," << (y+h) << "\"/>" << endl;
  }
}
//-----------------------------------------------------------------------------

DiagramItem::DiagramItem(DiagramItem *p,int number,ClassDef *cd,
                         Protection pr,Specifier vi,const char *ts) 
{ 
  parent=p; 
  x=y=0; 
  //name=n;
  num=number;
  children = new DiagramItemList;
  prot=pr;
  virt=vi;
  inList=FALSE;
  classDef=cd;
  templSpec=ts;
}
 
DiagramItem::~DiagramItem() 
{ 
  delete children;
}

QCString DiagramItem::label() const
{
  QCString result;
  if (!templSpec.isEmpty())
  {
    // we use classDef->name() here and not diplayName() in order
    // to get the name used in the inheritance relation.
    QCString n = classDef->name();
    if (/*n.right(2)=="-g" ||*/ n.right(2)=="-p")
    {
      n = n.left(n.length()-2);
    }
    result=insertTemplateSpecifierInScope(n,templSpec);
  }
  else
  {
    result=classDef->displayName();
  }
  if (Config_getBool(HIDE_SCOPE_NAMES)) result=stripScope(result);
  return result;
}

QCString DiagramItem::fileName() const
{
  return classDef->getOutputFileBase();
}

int DiagramItem::avgChildPos() const
{
  DiagramItem *di;
  int c=children->count();
  if (c==0) // no children -> don't move
    return xPos();
  if ((di=children->getFirst())->isInList()) // children should be in a list
    return di->xPos();
  if (c&1) // odd number of children -> get pos of middle child
    return children->at(c/2)->xPos();
  else // even number of children -> get middle of most middle children
    return (children->at(c/2-1)->xPos()+children->at(c/2)->xPos())/2;
}

int DiagramItem::numChildren() const
{
  return children->count();
}

void DiagramItem::addChild(DiagramItem *di)
{
  children->append(di);
}

void DiagramRow::insertClass(DiagramItem *parent,ClassDef *cd,bool doBases,
                             Protection prot,Specifier virt,const char *ts)
{
  //if (cd->visited) return; // the visit check does not work in case of
                             // multiple inheritance of the same class!
  DiagramItem *di=new DiagramItem(parent, diagram->at(level)->count(), 
                                  cd,prot,virt,ts);
  //cd->visited=TRUE;
  if (parent) parent->addChild(di);
  di->move(count()*gridWidth,level*gridHeight);
  append(di);
  BaseClassList *bcl=doBases ? cd->baseClasses() : cd->subClasses();
  int count=0;
  if (bcl)
  {
    /* there are base/sub classes */
    BaseClassListIterator it(*bcl);
    BaseClassDef *bcd;
    for (;(bcd=it.current());++it)
    {
      ClassDef *ccd=bcd->classDef;
      if (ccd && ccd->isVisibleInHierarchy() /*&& !ccd->visited*/) count++;
    }
  }
  if (count>0 && (prot!=Private || !doBases))
  {
    DiagramRow *row=0;
    if (diagram->count()<=level+1) /* add new row */
    {
      row = new DiagramRow(diagram,level+1);
      diagram->append(row);
    }
    else /* get next row */
    {
      row=diagram->at(level+1);
    }
    /* insert base classes in the next row */
    BaseClassListIterator it(*bcl);
    BaseClassDef *bcd;
    for (;(bcd=it.current());++it)
    {
      ClassDef *ccd=bcd->classDef;
      if (ccd && ccd->isVisibleInHierarchy() /*&& !ccd->visited*/)
      {
        row->insertClass(di,ccd,doBases,bcd->prot,
            doBases?bcd->virt:Normal,
            doBases?bcd->templSpecifiers.data():"");
      }
    }
  }
}

TreeDiagram::TreeDiagram(ClassDef *root,bool doBases)
{
  setAutoDelete(TRUE); 
  DiagramRow *row=new DiagramRow(this,0);
  append(row);
  row->insertClass(0,root,doBases,Public,Normal,0);
}

TreeDiagram::~TreeDiagram()
{
}


void TreeDiagram::moveChildren(DiagramItem *root,int dx)
{
  DiagramItemList *dil=root->getChildren();
  QListIterator<DiagramItem> it(*dil);
  DiagramItem *di;
  for (;(di=it.current());++it)
  {
    di->move(dx,0);
    moveChildren(di,dx);
  }
}

bool TreeDiagram::layoutTree(DiagramItem *root,int r)
{
  bool moved=FALSE;
  //printf("layoutTree(%s,%d)\n",root->label().data(),r);

  DiagramItemList *dil=root->getChildren(); 
  if (dil->count()>0)
  {
    uint k;
    int pPos=root->xPos();
    int cPos=root->avgChildPos();
    if (pPos>cPos) // move children
    {
      DiagramRow *row=at(r+1);
      //printf("Moving children %d-%d in row %d\n",
      //    dil->getFirst()->number(),row->count()-1,r+1);
      for (k=dil->getFirst()->number();k<row->count();k++)
        row->at(k)->move(pPos-cPos,0);
      moved=TRUE;
    }
    else if (pPos<cPos) // move parent
    {
      DiagramRow *row=at(r);
      //printf("Moving parents %d-%d in row %d\n",
      //    root->number(),row->count()-1,r);
      for (k=root->number();k<row->count();k++)
        row->at(k)->move(cPos-pPos,0);
      moved=TRUE;
    }

    // recurse to children
    QListIterator<DiagramItem> it(*dil);
    DiagramItem *di;
    for (;(di=it.current()) && !moved && !di->isInList();++it)
    {
      moved = layoutTree(di,r+1);
    }
  }
  return moved;
}

void TreeDiagram::computeLayout()
{
  QListIterator<DiagramRow> it(*this);
  DiagramRow *row;
  for (;(row=it.current()) && row->count()<maxTreeWidth;++it) {}
  if (row)
  {
    //printf("computeLayout() list row at %d\n",row->number());
    QListIterator<DiagramItem> rit(*row);
    DiagramItem *di;
    DiagramItem *opi=0;
    int delta=0;
    bool first=TRUE;
    for (;(di=rit.current());++rit)
    {
      DiagramItem *pi=di->parentItem();
      if (pi==opi && !first) { delta-=gridWidth; }
      first = pi!=opi;
      opi=pi;
      di->move(delta,0); // collapse all items in the same
                         // list (except the first)
      di->putInList();
    }
  }

  // re-organize the diagram items
  DiagramItem *root=getFirst()->getFirst();
  while (layoutTree(root,0)) { }

  // move first items of the lists
  if (row)
  {
    QListIterator<DiagramItem> rit(*row);
    DiagramItem *di;
    while ((di=rit.current()))
    {
      DiagramItem *pi=di->parentItem();
      if (pi->getChildren()->count()>1)
      {
        di->move(gridWidth,0);
        while (di && di->parentItem()==pi) { ++rit; di=rit.current(); }
      }
      else
      {
        ++rit;
      }
    }
  }
}

uint TreeDiagram::computeRows()
{
  //printf("TreeDiagram::computeRows()=%d\n",count());
  int count=0;
  QListIterator<DiagramRow> it(*this);
  DiagramRow *row;
  for (;(row=it.current()) && !row->getFirst()->isInList();++it)
  {
    count++;
  }
  //printf("count=%d row=%p\n",count,row);
  if (row)
  {
    int maxListLen=0;
    int curListLen=0;
    DiagramItem *opi=0;
    QListIterator<DiagramItem> rit(*row);
    DiagramItem *di;
    for (;(di=rit.current());++rit)
    {
      if (di->parentItem()!=opi) curListLen=1; else curListLen++; 
      if (curListLen>maxListLen) maxListLen=curListLen;
      opi=di->parentItem();
    }
    //printf("maxListLen=%d\n",maxListLen);
    count+=maxListLen;
  }
  return count;
}

void TreeDiagram::computeExtremes(uint *maxLabelLen,uint *maxXPos)
{
  uint ml=0,mx=0;
  QListIterator<DiagramRow> it(*this);
  DiagramRow *dr;
  bool done=FALSE;
  for (;(dr=it.current()) && !done;++it)
  {
    QListIterator<DiagramItem> rit(*dr);
    DiagramItem *di;
    for (;(di=rit.current());++rit)
    {
      if (di->isInList()) done=TRUE;
      if (maxXPos) mx=QMAX(mx,(uint)di->xPos());
      if (maxLabelLen) ml=QMAX(ml,Image::stringLength(di->label()));
    }
  }
  if (maxLabelLen) *maxLabelLen=ml;
  if (maxXPos)     *maxXPos=mx;
}

void TreeDiagram::drawBoxes(FTextStream &t,Image *image, 
                            bool doBase,bool bitmap,
                            uint baseRows,uint superRows,
                            uint cellWidth,uint cellHeight,
                            QCString relPath,
                            bool generateMap)
{
  QListIterator<DiagramRow> it(*this);
  DiagramRow *dr;
  if (!doBase) ++it;
  bool done=FALSE;
  bool firstRow = doBase;
  for (;(dr=it.current()) && !done;++it)
  {
    int x=0,y=0;
    float xf=0.0f,yf=0.0f;
    QListIterator<DiagramItem> rit(*dr);
    DiagramItem *di = rit.current();
    if (di->isInList()) // put boxes in a list
    {
      DiagramItem *opi=0;
      if (doBase) rit.toLast(); else rit.toFirst();
      while ((di=rit.current()))
      {
        if (di->parentItem()==opi)
        {
          if (bitmap)
          {
            if (doBase) y -= cellHeight+labelVertSpacing;
            else        y += cellHeight+labelVertSpacing;
          }
          else
          {
            if (doBase) yf += 1.0f;
            else        yf -= 1.0f;
          }
        }
        else
        {
          if (bitmap)
          {
            x = di->xPos()*(cellWidth+labelHorSpacing)/gridWidth;
            if (doBase)
            {
              y = image->getHeight()-
                superRows*cellHeight-
                (superRows-1)*labelVertSpacing-
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
            }
            else
            {
              y = (baseRows-1)*(cellHeight+labelVertSpacing)+
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
            }
          }
          else
          {
            xf = di->xPos()/(float)gridWidth;
            if (doBase)
            {
              yf = di->yPos()/(float)gridHeight+superRows-1;
            }
            else
            {
              yf = superRows-1-di->yPos()/(float)gridHeight;
            }
          }
        }
        opi=di->parentItem();
        
        if (bitmap)
        {
          bool hasDocs=di->getClassDef()->isLinkable();
          writeBitmapBox(di,image,x,y,cellWidth,cellHeight,firstRow,
              hasDocs,di->getChildren()->count()>0); 
          if (!firstRow && generateMap) 
            writeMapArea(t,di->getClassDef(),relPath,x,y,cellWidth,cellHeight);
        }
        else
        {
          writeVectorBox(t,di,xf,yf,di->getChildren()->count()>0);
        }
        
        if (doBase) --rit; else ++rit;
      }
      done=TRUE;
    }
    else // draw a tree of boxes
    {
      for (rit.toFirst();(di=rit.current());++rit)
      {
        if (bitmap)
        {
          x = di->xPos()*(cellWidth+labelHorSpacing)/gridWidth;
          if (doBase)
          {
            y = image->getHeight()-
              superRows*cellHeight-
              (superRows-1)*labelVertSpacing-
              di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
          }
          else
          {
            y = (baseRows-1)*(cellHeight+labelVertSpacing)+
              di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
          }
          bool hasDocs=di->getClassDef()->isLinkable();
          writeBitmapBox(di,image,x,y,cellWidth,cellHeight,firstRow,hasDocs); 
          if (!firstRow && generateMap) 
            writeMapArea(t,di->getClassDef(),relPath,x,y,cellWidth,cellHeight);
        }
        else
        {
          xf=di->xPos()/(float)gridWidth;
          if (doBase)
          {
            yf = di->yPos()/(float)gridHeight+superRows-1;
          }
          else
          {
            yf = superRows-1-di->yPos()/(float)gridHeight;
          }
          writeVectorBox(t,di,xf,yf);
        }
      }
    }
    firstRow=FALSE;
  }
}

void TreeDiagram::drawConnectors(FTextStream &t,Image *image,
                                 bool doBase,bool bitmap,
                                 uint baseRows,uint superRows,
                                 uint cellWidth,uint cellHeight)
{
  QListIterator<DiagramRow> it(*this);
  DiagramRow *dr;
  bool done=FALSE;
  for (;(dr=it.current()) && !done;++it) // for each row
  {
    QListIterator<DiagramItem> rit(*dr);
    DiagramItem *di = rit.current();
    if (di->isInList()) // row consists of list connectors
    {
      int x=0,y=0,ys=0;
      float xf=0.0f,yf=0.0f,ysf=0.0f;
      for (;(di=rit.current());++rit)
      {
        DiagramItem *pi=di->parentItem();
        DiagramItemList *dil=pi->getChildren();
        DiagramItem *last=dil->getLast();
        if (di==last) // single child
        {
          if (bitmap) // draw pixels
          {
            x = di->xPos()*(cellWidth+labelHorSpacing)/gridWidth + cellWidth/2;
            if (doBase) // base classes
            {
              y = image->getHeight()-
                (superRows-1)*(cellHeight+labelVertSpacing)-
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              image->drawVertArrow(x,y,y+labelVertSpacing/2,
                                   protToColor(di->protection()),
                                   protToMask(di->protection()));
            }
            else // super classes
            {
              y = (baseRows-1)*(cellHeight+labelVertSpacing)-
                labelVertSpacing/2+
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              image->drawVertLine(x,y,y+labelVertSpacing/2,
                                  protToColor(di->protection()),
                                  protToMask(di->protection()));
            }
          }
          else // draw vectors
          {
            t << protToString(di->protection()) << endl;
            if (doBase)
            {
              t << "1 " << (di->xPos()/(float)gridWidth) << " " 
                << (di->yPos()/(float)gridHeight+superRows-1) << " in\n";
            }
            else
            {
              t << "0 " << (di->xPos()/(float)gridWidth) << " " 
                << ((float)superRows-0.25-di->yPos()/(float)gridHeight)
                << " in\n";
            }
          }
        }
        else // multiple children, put them in a vertical list
        {
          if (bitmap)
          {
            x = di->parentItem()->xPos()*
              (cellWidth+labelHorSpacing)/gridWidth+cellWidth/2;
            if (doBase) // base classes
            {
              ys = image->getHeight()-
                (superRows-1)*(cellHeight+labelVertSpacing)-
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              y = ys - cellHeight/2;
            }
            else // super classes
            {
              ys = (baseRows-1)*(cellHeight+labelVertSpacing)+
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              y = ys + cellHeight/2;
            }
          }
          else
          {
            xf = di->parentItem()->xPos()/(float)gridWidth;
            if (doBase)
            {
              ysf = di->yPos()/(float)gridHeight+superRows-1;
              yf = ysf + 0.5f;
            }
            else
            {
              ysf = (float)superRows-0.25f-di->yPos()/(float)gridHeight;
              yf = ysf - 0.25f;
            }
          }
          while (di!=last) // more children to add
          {
            if (bitmap)
            {
              if (doBase) // base classes
              {
                image->drawHorzArrow(y,x,x+cellWidth/2+labelHorSpacing,
                    protToColor(di->protection()),
                    protToMask(di->protection()));
                y -= cellHeight+labelVertSpacing;
              }
              else // super classes
              {
                image->drawHorzLine(y,x,x+cellWidth/2+labelHorSpacing,
                    protToColor(di->protection()),
                    protToMask(di->protection()));
                y += cellHeight+labelVertSpacing;
              }
            }
            else
            {
              t << protToString(di->protection()) << endl;
              if (doBase)
              {
                t << "1 " << xf << " " << yf << " hedge\n";
                yf += 1.0f;
              }
              else
              {
                t << "0 " << xf << " " << yf << " hedge\n";
                yf -= 1.0f;
              }
            }
            ++rit; di=rit.current();
          }
          // add last horizonal line and a vertical connection line
          if (bitmap)
          {
            if (doBase) // base classes
            {
              image->drawHorzArrow(y,x,x+cellWidth/2+labelHorSpacing,
                  protToColor(di->protection()),
                  protToMask(di->protection()));
              image->drawVertLine(x,y,ys+labelVertSpacing/2,
                  protToColor(getMinProtectionLevel(dil)),
                  protToMask(getMinProtectionLevel(dil)));
            }
            else // super classes
            {
              image->drawHorzLine(y,x,x+cellWidth/2+labelHorSpacing,
                  protToColor(di->protection()),
                  protToMask(di->protection()));
              image->drawVertLine(x,ys-labelVertSpacing/2,y,
                  protToColor(getMinProtectionLevel(dil)),
                  protToMask(getMinProtectionLevel(dil)));
            }
          }
          else
          {
            t << protToString(di->protection()) << endl;
            if (doBase)
            {
              t << "1 " << xf << " " << yf << " hedge\n";
            }
            else
            {
              t << "0 " << xf << " " << yf << " hedge\n";
            }
            t << protToString(getMinProtectionLevel(dil)) << endl;
            if (doBase)
            {
              t << xf << " " << ysf << " " << yf << " vedge\n";
            }
            else
            {
              t << xf << " " << (ysf + 0.25) << " " << yf << " vedge\n";
            }
          }
        }
      }
      done=TRUE; // the tree is drawn now
    }
    else // normal tree connector
    {
      for (;(di=rit.current());++rit)
      {
        int x=0,y=0;
        DiagramItemList *dil = di->getChildren();
        DiagramItem *parent  = di->parentItem();
        if (parent) // item has a parent -> connect to it
        {
          if (bitmap) // draw pixels
          {
            x = di->xPos()*(cellWidth+labelHorSpacing)/gridWidth + cellWidth/2;
            if (doBase) // base classes
            {
              y = image->getHeight()-
                (superRows-1)*(cellHeight+labelVertSpacing)-
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              /* write input line */
              image->drawVertArrow(x,y,y+labelVertSpacing/2,
                  protToColor(di->protection()),
                  protToMask(di->protection()));
            }
            else // super classes
            {
              y = (baseRows-1)*(cellHeight+labelVertSpacing)-
                labelVertSpacing/2+
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              /* write output line */
              image->drawVertLine(x,y,y+labelVertSpacing/2,
                  protToColor(di->protection()),
                  protToMask(di->protection()));
            }
          }
          else // draw pixels
          {
            t << protToString(di->protection()) << endl;
            if (doBase)
            {
              t << "1 " << di->xPos()/(float)gridWidth << " " 
                << (di->yPos()/(float)gridHeight+superRows-1) << " in\n";
            }
            else
            {
              t << "0 " << di->xPos()/(float)gridWidth << " " 
                << ((float)superRows-0.25-di->yPos()/(float)gridHeight)
                << " in\n";
            }
          }
        }
        if (dil->count()>0)
        {
          Protection p=getMinProtectionLevel(dil);
          uint mask=protToMask(p);
          uint col=protToColor(p);
          if (bitmap)
          {
            x = di->xPos()*(cellWidth+labelHorSpacing)/gridWidth + cellWidth/2;
            if (doBase) // base classes
            {
              y = image->getHeight()-
                (superRows-1)*(cellHeight+labelVertSpacing)-
                cellHeight-labelVertSpacing/2-
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              image->drawVertLine(x,y,y+labelVertSpacing/2-1,col,mask);
            }
            else // super classes
            {
              y = (baseRows-1)*(cellHeight+labelVertSpacing)+
                cellHeight+
                di->yPos()*(cellHeight+labelVertSpacing)/gridHeight;
              image->drawVertArrow(x,y,y+labelVertSpacing/2-1,col,mask);
            }
          }
          else
          {
            t << protToString(p) << endl;
            if (doBase)
            {
              t << "0 " << di->xPos()/(float)gridWidth  << " " 
                << (di->yPos()/(float)gridHeight+superRows-1) << " out\n";
            }
            else
            {
              t << "1 " << di->xPos()/(float)gridWidth  << " " 
                << ((float)superRows-1.75-di->yPos()/(float)gridHeight)
                << " out\n";
            }
          }
          /* write input line */
          DiagramItem *first = dil->getFirst();
          DiagramItem *last  = dil->getLast();
          if (first!=last && !first->isInList()) /* connect with all base classes */
          {
            if (bitmap)
            {
              int xs = first->xPos()*(cellWidth+labelHorSpacing)/gridWidth
                + cellWidth/2;
              int xe = last->xPos()*(cellWidth+labelHorSpacing)/gridWidth
                + cellWidth/2; 
              if (doBase) // base classes
              {
                image->drawHorzLine(y,xs,xe,col,mask); 
              }
              else // super classes
              {
                image->drawHorzLine(y+labelVertSpacing/2,xs,xe,col,mask); 
              }
            }
            else
            {
              t << protToString(p) << endl;
              if (doBase)
              {
                t << first->xPos()/(float)gridWidth << " " 
                  << last->xPos()/(float)gridWidth << " "
                  << (first->yPos()/(float)gridHeight+superRows-1) 
                  << " conn\n";
              }
              else
              {
                t << first->xPos()/(float)gridWidth << " " 
                  << last->xPos()/(float)gridWidth << " "
                  << ((float)superRows-first->yPos()/(float)gridHeight)
                  << " conn\n";
              }
            }
          }
        }
      }
    }
  }
}


void clearVisitFlags()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    cd->visited=FALSE;
  }
}

ClassDiagram::ClassDiagram(ClassDef *root)
{
  clearVisitFlags();
  base  = new TreeDiagram(root,TRUE);
  base->computeLayout();
  clearVisitFlags();
  super = new TreeDiagram(root,FALSE);
  super->computeLayout();
  DiagramItem *baseItem  = base->getFirst()->getFirst();
  DiagramItem *superItem = super->getFirst()->getFirst();
  int xbase  = baseItem->xPos();
  int xsuper = superItem->xPos();
  if (xbase>xsuper)
  {
    superItem->move(xbase-xsuper,0);
    super->moveChildren(superItem,xbase-xsuper);
  }
  else if (xbase<xsuper)
  {
    baseItem->move(xsuper-xbase,0);
    base->moveChildren(baseItem,xsuper-xbase);
  }
}

ClassDiagram::~ClassDiagram()
{
  delete base;
  delete super;
}

void ClassDiagram::writeFigure(FTextStream &output,const char *path,
                               const char *fileName) const
{
  uint baseRows=base->computeRows();
  uint superRows=super->computeRows();
  uint baseMaxX, baseMaxLabelWidth, superMaxX, superMaxLabelWidth;
  base->computeExtremes(&baseMaxLabelWidth,&baseMaxX);
  super->computeExtremes(&superMaxLabelWidth,&superMaxX);

  uint rows=baseRows+superRows-1;
  uint cols=(QMAX(baseMaxX,superMaxX)+gridWidth*2-1)/gridWidth;
  
  // Estimate the image aspect width and height in pixels.
  uint estHeight = rows*40;
  uint estWidth  = cols*(20+QMAX(baseMaxLabelWidth,superMaxLabelWidth));
  //printf("Estimated size %d x %d\n",estWidth,estHeight);
  
  const float pageWidth = 14.0f; // estimated page width in cm.
                                 // Somewhat lower to deal with estimation
                                 // errors. 
  
  // compute the image height in centimeters based on the estimates
  float realHeight = QMIN(rows,12); // real height in cm
  float realWidth  = realHeight * estWidth/(float)estHeight;
  if (realWidth>pageWidth) // assume that the page width is about 15 cm
  {
    realHeight*=pageWidth/realWidth; 
    realWidth=pageWidth;
  }

  //output << "}\n";
  output << "\\begin{figure}[H]\n"
            "\\begin{center}\n"
            "\\leavevmode\n";
  output << "\\includegraphics[height=" << realHeight << "cm]{" 
                                        << fileName << "}" << endl;
  output << "\\end{center}\n"
            "\\end{figure}\n";
  
  //printf("writeFigure rows=%d cols=%d\n",rows,cols);

  QCString epsBaseName=(QCString)path+"/"+fileName;
  QCString epsName=epsBaseName+".eps";
  QFile f1;
  f1.setName(epsName.data());
  if (!f1.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",f1.name().data());
    exit(1);
  }
  FTextStream t(&f1);
  
  //printf("writeEPS() rows=%d cols=%d\n",rows,cols);
  
  // generate EPS header and postscript variables and procedures
  
  t << "%!PS-Adobe-2.0 EPSF-2.0\n";
  t << "%%Title: ClassName\n";
  t << "%%Creator: Doxygen\n";
  t << "%%CreationDate: Time\n";
  t << "%%For: \n";
  t << "%Magnification: 1.00\n";
  t << "%%Orientation: Portrait\n";
  t << "%%BoundingBox: 0 0 500 " << estHeight*500.0/(float)estWidth << "\n";
  t << "%%Pages: 0\n";
  t << "%%BeginSetup\n";
  t << "%%EndSetup\n";
  t << "%%EndComments\n";
  t << "\n";
  t << "% ----- variables -----\n";
  t << "\n";
  t << "/boxwidth 0 def\n";
  t << "/boxheight 40 def\n";
  t << "/fontheight 24 def\n";
  t << "/marginwidth 10 def\n";
  t << "/distx 20 def\n";
  t << "/disty 40 def\n";
  t << "/boundaspect " << estWidth/(float)estHeight << " def  % aspect ratio of the BoundingBox (width/height)\n";
  t << "/boundx 500 def\n";
  t << "/boundy boundx boundaspect div def\n";
  t << "/xspacing 0 def\n";
  t << "/yspacing 0 def\n";
  t << "/rows " << rows << " def\n";
  t << "/cols " << cols << " def\n";
  t << "/scalefactor 0 def\n";
  t << "/boxfont /Times-Roman findfont fontheight scalefont def\n";
  t << "\n";
  t << "% ----- procedures -----\n";
  t << "\n";
  t << "/dotted { [1 4] 0 setdash } def\n";
  t << "/dashed { [5] 0 setdash } def\n";
  t << "/solid  { [] 0 setdash } def\n";
  t << "\n";
  t << "/max % result = MAX(arg1,arg2)\n";
  t << "{\n";
  t << "  /a exch def\n";
  t << "  /b exch def\n";
  t << "  a b gt {a} {b} ifelse\n";
  t << "} def\n";
  t << "\n";
  t << "/xoffset % result = MAX(0,(scalefactor-(boxwidth*cols+distx*(cols-1)))/2)\n";
  t << "{\n";
  t << "  0 scalefactor boxwidth cols mul distx cols 1 sub mul add sub 2 div max\n";
  t << "} def\n";
  t << "\n";
  t << "/cw % boxwidth = MAX(boxwidth, stringwidth(arg1))\n";
  t << "{\n";
  t << "  /str exch def\n";
  t << "  /boxwidth boxwidth str stringwidth pop max def\n";
  t << "} def\n";
  t << "\n";
  t << "/box % draws a box with text `arg1' at grid pos (arg2,arg3)\n";
  t << "{ gsave\n";
  t << "  2 setlinewidth\n";
  t << "  newpath\n";
  t << "  exch xspacing mul xoffset add\n";
  t << "  exch yspacing mul\n";
  t << "  moveto\n";
  t << "  boxwidth 0 rlineto \n";
  t << "  0 boxheight rlineto \n";
  t << "  boxwidth neg 0 rlineto \n";
  t << "  0 boxheight neg rlineto \n";
  t << "  closepath\n";
  t << "  dup stringwidth pop neg boxwidth add 2 div\n";
  t << "  boxheight fontheight 2 div sub 2 div\n";
  t << "  rmoveto show stroke\n";
  t << "  grestore\n";
  t << "} def  \n";
  t << "\n";
  t << "/mark\n";
  t << "{ newpath\n";
  t << "  exch xspacing mul xoffset add boxwidth add\n";
  t << "  exch yspacing mul\n";
  t << "  moveto\n";
  t << "  0 boxheight 4 div rlineto\n";
  t << "  boxheight neg 4 div boxheight neg 4 div rlineto\n";
  t << "  closepath\n";
  t << "  eofill\n";
  t << "  stroke\n";
  t << "} def\n";
  t << "\n";
  t << "/arrow\n";
  t << "{ newpath\n";
  t << "  moveto\n";
  t << "  3 -8 rlineto\n";
  t << "  -6 0 rlineto\n";
  t << "  3 8 rlineto\n";
  t << "  closepath\n";
  t << "  eofill\n";
  t << "  stroke\n";
  t << "} def\n";
  t << "\n";
  t << "/out % draws an output connector for the block at (arg1,arg2)\n";
  t << "{\n";
  t << "  newpath\n";
  t << "  exch xspacing mul xoffset add boxwidth 2 div add\n";
  t << "  exch yspacing mul boxheight add\n";
  t << "  /y exch def\n";
  t << "  /x exch def\n";
  t << "  x y moveto\n";
  t << "  0 disty 2 div rlineto \n";
  t << "  stroke\n";
  t << "  1 eq { x y disty 2 div add arrow } if\n";
  t << "} def\n";
  t << "\n";
  t << "/in % draws an input connector for the block at (arg1,arg2)\n";
  t << "{\n";
  t << "  newpath\n";
  t << "  exch xspacing mul xoffset add boxwidth 2 div add\n";
  t << "  exch yspacing mul disty 2 div sub\n";
  t << "  /y exch def\n";
  t << "  /x exch def\n";
  t << "  x y moveto\n";
  t << "  0 disty 2 div rlineto\n";
  t << "  stroke\n";
  t << "  1 eq { x y disty 2 div add arrow } if\n";
  t << "} def\n";
  t << "\n";
  t << "/hedge\n";
  t << "{\n";
  t << "  exch xspacing mul xoffset add boxwidth 2 div add\n";
  t << "  exch yspacing mul boxheight 2 div sub\n";
  t << "  /y exch def\n";
  t << "  /x exch def\n";
  t << "  newpath\n";
  t << "  x y moveto\n";
  t << "  boxwidth 2 div distx add 0 rlineto\n";
  t << "  stroke\n";
  t << "  1 eq\n";
  t << "  { newpath x boxwidth 2 div distx add add y moveto\n";
  t << "    -8 3 rlineto\n";
  t << "    0 -6 rlineto\n";
  t << "    8 3 rlineto\n";
  t << "    closepath\n";
  t << "    eofill\n";
  t << "    stroke\n";
  t << "  } if\n";
  t << "} def\n";
  t << "\n";
  t << "/vedge\n";
  t << "{\n";
  t << "  /ye exch def\n";
  t << "  /ys exch def\n";
  t << "  /xs exch def\n";
  t << "  newpath\n";
  t << "  xs xspacing mul xoffset add boxwidth 2 div add dup\n";
  t << "  ys yspacing mul boxheight 2 div sub\n";
  t << "  moveto\n";
  t << "  ye yspacing mul boxheight 2 div sub\n";
  t << "  lineto\n";
  t << "  stroke\n";
  t << "} def\n";
  t << "\n";
  t << "/conn % connections the blocks from col `arg1' to `arg2' of row `arg3'\n";
  t << "{\n";
  t << "  /ys exch def\n";
  t << "  /xe exch def\n";
  t << "  /xs exch def\n";
  t << "  newpath\n";
  t << "  xs xspacing mul xoffset add boxwidth 2 div add\n";
  t << "  ys yspacing mul disty 2 div sub\n";
  t << "  moveto\n";
  t << "  xspacing xe xs sub mul 0\n";
  t << "  rlineto\n";
  t << "  stroke\n";
  t << "} def\n";
  t << "\n";
  t << "% ----- main ------\n";
  t << "\n";
  t << "boxfont setfont\n";
  t << "1 boundaspect scale\n";


  bool done=FALSE;
  QListIterator<DiagramRow> bit(*base);
  DiagramRow *dr;
  for (;(dr=bit.current()) && !done;++bit)
  {
    QListIterator<DiagramItem> rit(*dr);
    DiagramItem *di;
    for (;(di=rit.current());++rit)
    {
      done=di->isInList();
      t << "(" << di->label() << ") cw\n";
    }
  }
  QListIterator<DiagramRow> sit(*super);
  ++sit;
  done=FALSE;
  for (;(dr=sit.current()) && !done;++sit)
  {
    QListIterator<DiagramItem> rit(*dr);
    DiagramItem *di;
    for (;(di=rit.current());++rit)
    {
      done=di->isInList();
      t << "(" << di->label() << ") cw\n";
    }
  }

  t << "/boxwidth boxwidth marginwidth 2 mul add def\n"
    << "/xspacing boxwidth distx add def\n"
    << "/yspacing boxheight disty add def\n"
    << "/scalefactor \n"
    << "  boxwidth cols mul distx cols 1 sub mul add\n"
    << "  boxheight rows mul disty rows 1 sub mul add boundaspect mul \n"
    << "  max def\n"
    << "boundx scalefactor div boundy scalefactor div scale\n";
  
  t << "\n% ----- classes -----\n\n";
  base->drawBoxes(t,0,TRUE,FALSE,baseRows,superRows,0,0);
  super->drawBoxes(t,0,FALSE,FALSE,baseRows,superRows,0,0);
  
  t << "\n% ----- relations -----\n\n";
  base->drawConnectors(t,0,TRUE,FALSE,baseRows,superRows,0,0);
  super->drawConnectors(t,0,FALSE,FALSE,baseRows,superRows,0,0);

  f1.close();
  if (Config_getBool(USE_PDFLATEX))
  {
    QCString epstopdfArgs(4096);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",
                   epsBaseName.data(),epsBaseName.data());
    //printf("Converting eps using `%s'\n",epstopdfArgs.data());
    portable_sysTimerStart();
    if (portable_system("epstopdf",epstopdfArgs)!=0)
    {
       err("Problems running epstopdf. Check your TeX installation!\n");
       portable_sysTimerStop();
       return;
    }
    portable_sysTimerStop();
  }
}


void ClassDiagram::writeImage(FTextStream &t,const char *path,
                              const char *relPath,const char *fileName, 
                              bool generateMap) const
{
  uint baseRows=base->computeRows();
  uint superRows=super->computeRows();
  uint rows=baseRows+superRows-1;

  uint lb,ls,xb,xs;
  base->computeExtremes(&lb,&xb);
  super->computeExtremes(&ls,&xs);
 
  uint cellWidth  = QMAX(lb,ls)+labelHorMargin*2;
  uint maxXPos    = QMAX(xb,xs);
  uint labelVertMargin = 6; //QMAX(6,(cellWidth-fontHeight)/6); // aspect at least 1:3
  uint cellHeight = labelVertMargin*2+fontHeight;
  uint imageWidth = (maxXPos+gridWidth)*cellWidth/gridWidth+
                    (maxXPos*labelHorSpacing)/gridWidth;
  uint imageHeight = rows*cellHeight+(rows-1)*labelVertSpacing;

  Image image(imageWidth,imageHeight);

  base->drawBoxes(t,&image,TRUE,TRUE,baseRows,superRows,cellWidth,cellHeight,relPath,generateMap);
  super->drawBoxes(t,&image,FALSE,TRUE,baseRows,superRows,cellWidth,cellHeight,relPath,generateMap);
  base->drawConnectors(t,&image,TRUE,TRUE,baseRows,superRows,cellWidth,cellHeight);
  super->drawConnectors(t,&image,FALSE,TRUE,baseRows,superRows,cellWidth,cellHeight);

#define IMAGE_EXT ".png"
  image.save((QCString)path+"/"+fileName+IMAGE_EXT);
  Doxygen::indexList->addImageFile(QCString(fileName)+IMAGE_EXT);
  
  if (generateMap) t << "</map>" << endl;
}


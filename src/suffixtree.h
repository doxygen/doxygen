/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef SUFFIXTREE_H
#define SUFFIXTREE_H

#include "qtbc.h"
#include <qlist.h>
#include <qarray.h>
#include <qfile.h>

class SuffixNodeList;
class IndexTree;

enum WordRefMasks { WORDINNAME_MASK=4, INNAME_MASK=2, FULLWORD_MASK=1 };
enum WordRefBits  { WORDINNAME_BIT=2, INNAME_BIT=1, FULLWORD_BIT=0 };

//  bool  inName;
//  bool  fullWord;
//  bool  wordInName;

struct WordRef
{
  int   id;
  short freq;
  char  flags;
};

class SuffixNode
{
  friend class SuffixTree;
  friend class IndexNode;
  friend class SuffixNodeList;
  public:
    SuffixNode(const char *);
   ~SuffixNode();
    int  insert(const char *word,int refId,int inName,int full);
    void addReference(int refId,int inName,int fullWord);
    void dump(int,const char *);
    void resolveForwardReferences(int &offset);
    int  size(); // return the size of the tree whose root is this node
    bool write(QFile &f);
  private:
    SuffixNodeList *children;
    QArray<WordRef> references;
    QCString label;
    int branchOffset;
    int totalFreq;
};

class SuffixNodeList : public QList<SuffixNode>
{
  public:
    SuffixNodeList() : QList<SuffixNode>() {}
   ~SuffixNodeList() {}
    int compareItems(GCI item1,GCI item2);
};

class SuffixTree
{
  friend class SuffixNode;
  public:
    SuffixTree();
   ~SuffixTree();
    void insertWord(const char *word,int index,bool inName);
    //void buildIndex();
    void resolveForwardReferences();
    void dump();
    int size(); // return the size of the (flat) tree in bytes
    bool write(QFile &f);
    int  numberOfNodes() { return nodes; }
  private:
    int nodes;
    SuffixNode *root;
};

extern bool writeNumber(QFile &f,int);
extern bool writeString(QFile &f,const char *s);

#endif

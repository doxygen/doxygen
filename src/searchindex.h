/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef _SEARCHINDEX_H
#define _SEARCHINDEX_H

#include "qtbc.h"
#include <qintdict.h>
#include <qlist.h>
#include <qdict.h>

#include "suffixtree.h"

//class IndexTree;
class SuffixTree;

class DocRef
{
  public:
    DocRef(int index,const char *name,const char *url) 
          { i=index; n=name; u=url; }
   ~DocRef() {}
    void setOffset(int offset) { o=offset; }
    const char *name() const { return n; }
    const char *url() const { return u; }
    int index() const { return i; }
    int offset() const { return o; }
   
  private:
    QCString n;
    QCString u;
    int i;
    int o;
};

typedef QList<DocRef> DocRefList;
typedef QDict<DocRef> DocRefDict;

class SearchIndex
{
  public:
    SearchIndex();
   ~SearchIndex();
    void addReference(const char *key,const char *ref);
    bool addWord(const char *key,const char *word,bool special);
    bool saveIndex(const char *fileName);
    void dump() { suffixTree->dump(); }
   
  private:
    //IndexTree  *indexTree;
    SuffixTree *suffixTree;
    DocRefList refList;
    DocRefDict refDict;
    QIntDict<DocRef> nameIndex;
    int indexCount;
};

#endif

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "searchindex.h"
#include "suffixtree.h"

SearchIndex::SearchIndex() : refDict(10007), nameIndex(10007)
{
  indexCount=1;
  //indexTree = new IndexTree;
  suffixTree = new SuffixTree;
}

SearchIndex::~SearchIndex()
{
  //delete indexTree;
  delete suffixTree;
}

void SearchIndex::addReference(const char *key,const char *ref)
{
  DocRef *dr=new DocRef(indexCount,key,ref);
  nameIndex.insert(indexCount,dr);
  refList.append(dr);
  refDict.insert(key,dr);
  indexCount++;
}

bool SearchIndex::addWord(const char *key,const char *word,bool special)
{
  DocRef *dr=0;
  if (word && key && strlen(key)>0 && (dr=refDict[key]))
  {
    suffixTree->insertWord(((QCString)word).lower(),dr->index(),special); 
    return TRUE; 
  }
  else if (word)
  {
    printf("SearchIndex::addWord() key `%s' not found!\n",key);
    return FALSE;
  }
  else
  {
    printf("SearchIndex::addWord() trying to insert word with length 0\n");
    return FALSE;
  }
}

bool SearchIndex::saveIndex(const char *fileName)
{
  QFile f(fileName);
  if (!f.open(IO_WriteOnly)) return FALSE;
  
  // write header
  if (f.writeBlock("DOXI",4)!=4) return FALSE; // write header
  
  // compute forward offsets for all children of each node.
  suffixTree->resolveForwardReferences();
  
  // compute offset to the reference index table
  int offset=suffixTree->size()+9;
  
  // write the offset
  if (writeNumber(f,offset)) return FALSE;
  
  // write the suffix tree
  if (!suffixTree->write(f)) return FALSE;
  
  f.putch(0);
  
  // write the index reference table
  DocRef *dr=refList.first();
  offset+=refList.count()*4;
  while (dr)
  {
    writeNumber(f,offset);
    offset+=strlen(dr->name())+strlen(dr->url())+2;
    dr=refList.next(); 
  }
  
  // write the references
  dr=refList.first();
  while (dr)
  {
    writeString(f,dr->name());
    writeString(f,dr->url());
    dr=refList.next();
  }

  //printf("Building index tree\n");
  printf("Size of the suffix tree is %d bytes\n",suffixTree->size());
  printf("Number of tree nodes is %d\n",suffixTree->numberOfNodes());
  printf("Number of links %d\n",indexCount);
  //suffixTree->buildIndex();
  //printf("Computing reference offsets\n");
  //int offset=suffixTree->size()+8;
  //indexTree->setOffsets(&nameIndex);
  //printf("Saving suffix tree\n");
  //printf("Saving index tree\n");
  //result&=indexTree->write(f);
  //printf("Saving reference list\n");
  //dr=refList.first();
  //while (dr)
  //{
  //  result&=!writeString(f,dr->name());
  //  result&=!writeString(f,dr->url());
  //  dr=refList.next();
  //}
  //suffixTree->dump();
  return TRUE;
}

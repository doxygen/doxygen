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

#include <stdio.h>

#include "qtbc.h"
#include "suffixtree.h"

#define MAXWORDLEN 1024

//----------------------------------------------------------------------------

bool writeString(QFile &f,const char *s)
{
  int len=strlen(s)+1;
  return (f.writeBlock(s,len)!=len);
}

bool writeNumber(QFile &f,int num)
{
  return (f.putch((num>>24)&0xff)==-1) ||
         (f.putch((num>>16)&0xff)==-1) ||
         (f.putch((num>>8)&0xff)==-1)  ||
         (f.putch(num&0xff)==-1);
}

static bool writeEncodedNumber(QFile &f,uint number)
{
  bool error=FALSE;
  uint n=number;
  while (n>=128)
  {
    int frac=n&0x7f;
    error = error || (f.putch(frac|0x80)==-1);
    n>>=7;
  }
  error = error || (f.putch(n)==-1); 
  return error; 
}

static int encodedNumberSize(uint number)
{
  uint n=number;
  int size=1;
  while (n>=128) { size++; n>>=7; }
  return size;
}

//----------------------------------------------------------------------------

int SuffixNodeList::compareItems(GCI item1,GCI item2)
{
  SuffixNode *n1=(SuffixNode *)item1;
  SuffixNode *n2=(SuffixNode *)item2;
  return strcmp(n1->label,n2->label);
}

SuffixNode::SuffixNode(const char *lab) : references(0)
{
  children = new SuffixNodeList;
  children->setAutoDelete(TRUE); 
  label=lab;
  totalFreq=0;
  branchOffset=0;
}

SuffixNode::~SuffixNode() 
{
  delete children;
}

void SuffixNode::addReference(int refId,int inName,int fullWord)
{
  totalFreq++;
  uint s=references.size();
  if (s>0 && references.at(s-1).id==refId) // word occured in the same document
  {
    references.at(s-1).freq++;             // increase word's frequency
    references.at(s-1).flags=((references.at(s-1).flags & INNAME_MASK) 
                              | (inName<<INNAME_BIT))
                         +((references.at(s-1).flags & FULLWORD_MASK) 
                              | (fullWord<<FULLWORD_BIT))
                         +((references.at(s-1).flags & WORDINNAME_MASK) 
                              | ((inName & fullWord)<<WORDINNAME_BIT));
  }
  else
  {
    references.resize(s+1);
    references.at(s).id=refId;
    references.at(s).freq=1; 
    references.at(s).flags=(inName<<INNAME_BIT)
                       +(fullWord<<FULLWORD_BIT)
                       +((inName && fullWord)<<WORDINNAME_BIT);
  }
}

int SuffixNode::insert(const char *word,int refId,int inName,int fullWord)
{
  int numNewNodes=0;
  //printf("SuffixNode::insert(%s,%d)\n",word,refId);
  SuffixNode *sn=children->first();
  while (sn)
  {
    const char *lab=sn->label.data();
    char w=word[0],l=lab[0],i=0;
    while (w!=0 && l!=0 && w==l) { i++; w=word[i]; l=lab[i]; }
    if (w==0 && l==0) // match found
    {
      sn->addReference(refId,inName,fullWord);
      return numNewNodes;
    }
    if (i>0) // w and l contain a common prefix of length i
    {
      if (l==0) // w!=0 => follow this branch
      {
        sn->addReference(refId,inName,FALSE);
        numNewNodes+=sn->insert(&word[i],refId,inName,fullWord);
      }
      else // l!=0 => split branch
      {
        char leftlab[MAXWORDLEN];
        memcpy(leftlab,lab,i);
        leftlab[i]='\0';
        SuffixNode *r  = new SuffixNode(leftlab);
        numNewNodes++;
        SuffixNode *n2 = children->take();
        // copy reference info
        r->references  = n2->references.copy();
        int j,refSize  = r->references.size();
        for (j=0;j<refSize;j++) 
        {
          //r->references[j].fullWord=FALSE;
          //r->references[j].wordInName=FALSE;
          r->references[j].flags &= ~(FULLWORD_MASK|WORDINNAME_MASK);
        }
        r->totalFreq   = n2->totalFreq;
        //printf("root branch `%s'\n",leftlab);
        if (w!=0) // two sub branches
        {
          SuffixNode *n1 = new SuffixNode(&word[i]);
          numNewNodes++;
          n1->addReference(refId,inName,fullWord);
          r->addReference(refId,inName,FALSE);
          r->children->append(n1);
          //printf("Right branch `%s'\n",&word[i]);
        }
        else // one sub branch
        {
          r->addReference(refId,inName,fullWord);
        }
        //printf("Left branch `%s'\n",&lab[i]);
        n2->label=&lab[i];
        r->children->append(n2);
        children->append(r);
      }
      return numNewNodes;
    }
    sn=children->next();
  }
  //printf("new branch `%s'\n",word);
  SuffixNode *n=new SuffixNode(word);
  numNewNodes++;
  n->addReference(refId,inName,fullWord);
  children->append(n);
  return numNewNodes;
}

void SuffixNode::dump(int level,const char *prefix)
{
  uint i;
  if (references.size()>0)
  {
    printf("%s (level=%d offset=%d freq=%d) ",
           prefix,level,branchOffset,totalFreq);
    for (i=0;i<references.size();i++) 
      printf("%d->%d ",references.at(i).id,references.at(i).freq);
    printf("\n");
  }
  SuffixNode *sn=children->first();
  while (sn)
  {
    sn->dump(level+1,prefix+("-"+sn->label)); 
    sn=children->next();
  } 
}

void SuffixNode::resolveForwardReferences(int &offset)
{
  if (children->count()>0)
  {
    if (!label.isEmpty()) offset++; // terminator for the previous level
    branchOffset=offset;
  }
  else
    branchOffset=0;
  SuffixNode *sn=children->first();
  while (sn)
  {
    offset+=sn->label.length()+5;
    uint i,refs=sn->references.size();
    if (refs>0)
    {
      offset+=encodedNumberSize(sn->totalFreq);
      offset+=encodedNumberSize((sn->references[0].id<<3)+
                                 sn->references[0].flags);
      offset+=encodedNumberSize(sn->references[0].freq);
      for (i=1;i<refs;i++)
      {
        offset+=encodedNumberSize(
                   ((sn->references.at(i).id - sn->references.at(i-1).id)<<3)+
                     sn->references.at(i).flags);
        offset+=encodedNumberSize(sn->references.at(i).freq);
      }
      offset+=encodedNumberSize(0);
    }
    //printf("Lab=%s offset=%d\n",sn->lab.data(),offset);
    sn=children->next();
  } 
  sn=children->first();
  while (sn)
  {
    //printf("Lab=%s offset=%d\n",sn->lab.data(),offset);
    sn->resolveForwardReferences(offset);
    sn=children->next();
  } 
}

int SuffixNode::size()
{
  int s=0;
  if (!label.isEmpty() && children->count()>0) s++; // for the terminator
  SuffixNode *sn=children->first();
  while (sn)
  {
    uint i,refs=sn->references.size();
    s+=sn->size()+sn->label.length()+5;
    if (refs>0)
    {
      s+=encodedNumberSize(sn->totalFreq);
      s+=encodedNumberSize(
                  (sn->references[0].id<<3)+
                   sn->references[0].flags);
      s+=encodedNumberSize(sn->references[0].freq);
      for (i=1;i<refs;i++)
      {
        s+=encodedNumberSize(
                ((sn->references.at(i).id - sn->references.at(i-1).id)<<3)+
                  sn->references.at(i).flags);
        s+=encodedNumberSize(sn->references.at(i).freq);
      }
      s+=encodedNumberSize(0);
    }
    sn=children->next();
  } 
  return s;
}

bool SuffixNode::write(QFile &f)
{
  bool error=FALSE;
  if (children->count()>0 && !label.isEmpty()) error=error || (f.putch(0)==-1);
  SuffixNode *sn=children->first();
  while (sn)
  {
    //offset+=sn->lab.length()+1+2*sizeof(int);
    int i,refs=sn->references.size();
    error=error || writeString(f,sn->label);
    error=error || writeNumber(f,sn->branchOffset|((refs==0)?0x80000000:0));
    if (refs>0)
    {
      error=error || writeEncodedNumber(f,sn->totalFreq);
      error=error || writeEncodedNumber(f,
                 (sn->references[0].id<<3)+
                  sn->references[0].flags);
      error=error || writeEncodedNumber(f,sn->references[0].freq);
      for (i=1;i<refs;i++)
      {
        error=error || writeEncodedNumber(f,
                ((sn->references[i].id - sn->references[i-1].id)<<3)+
                  sn->references[i].flags);
        error=error || writeEncodedNumber(f,sn->references[i].freq);
      }
      error=error || writeEncodedNumber(f,0);
    }
    //printf("Lab=%s offset=%d\n",sn->lab.data(),offset);
    sn=children->next();
  } 
  sn=children->first();
  while (sn)
  {
    error=error || sn->write(f);
    sn=children->next();
  } 
  return error;
}

//----------------------------------------------------------------------------

SuffixTree::SuffixTree()
{
  root=new SuffixNode("");
  nodes=1;
}

SuffixTree::~SuffixTree()
{
  delete root;
}

void SuffixTree::insertWord(const char *word,int index,bool inName)
{
  QCString suffix=word;
  uint i;
  for (i=2;i<suffix.length();i++) 
  {
    //printf("Inserting suffix %s\n",suffix.right(i).data());
    nodes+=root->insert(suffix.right(i),index,inName?1:0,FALSE);
  }
  nodes+=root->insert(word,index,inName?1:0,TRUE);
}

void SuffixTree::dump()
{
  root->dump(0,"");
}

void SuffixTree::resolveForwardReferences()
{
  int offset=8;
  root->resolveForwardReferences(offset);
}

int SuffixTree::size()
{
  return root->size();
}

bool SuffixTree::write(QFile &f)
{
  if (!f.isOpen()) { printf("File not open\n"); return FALSE; }
  bool error=FALSE;
  error = error || root->write(f);
  return !error;
}

/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

/*! \file
 *  This class represents a list of output generators that work in "parallel".
 *  The class only knows about the abstract base class OutputGenerators.
 *  All output is produced by calling a method of this class, which forwards
 *  the call to all output generators.
 */

#include "outputlist.h"
#include "outputgen.h"
#include "config.h"
#include "message.h"

#include "docparser.h"

OutputList::OutputList(bool)
{
  //printf("OutputList::OutputList()\n");
  outputs = new QList<OutputGenerator>;
  outputs->setAutoDelete(TRUE);
}

OutputList::~OutputList()
{
  //printf("OutputList::~OutputList()\n");
  delete outputs;
}

void OutputList::add(const OutputGenerator *og)
{
  if (og) outputs->append(og);
}

void OutputList::disableAllBut(OutputGenerator::OutputType o)
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->disableIfNot(o);
    og=outputs->next();
  }
}

void OutputList::enableAll()
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->enable();
    og=outputs->next();
  }
}

void OutputList::disableAll()
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->disable();
    og=outputs->next();
  }
}

void OutputList::disable(OutputGenerator::OutputType o)
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->disableIf(o);
    og=outputs->next();
  }
}

void OutputList::enable(OutputGenerator::OutputType o)
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->enableIf(o);
    og=outputs->next();
  }
}

bool OutputList::isEnabled(OutputGenerator::OutputType o)
{
  bool result=FALSE;
  OutputGenerator *og=outputs->first();
  while (og)
  {
    result=result || og->isEnabled(o);
    og=outputs->next();
  }
  return result;
}

void OutputList::pushGeneratorState()
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->pushGeneratorState();
    og=outputs->next();
  }
}

void OutputList::popGeneratorState()
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    og->popGeneratorState();
    og=outputs->next();
  }
}

void OutputList::parseDoc(const char *fileName,int startLine,
                  Definition *ctx,MemberDef * md,
                  const QCString &docStr,bool indexWords,
                  bool isExample,const char *exampleName)
{
  int count=0;
  if (docStr.isEmpty()) return;
  
  OutputGenerator *og=outputs->first();
  while (og)
  {
    if (og->isEnabled()) count++;
    og=outputs->next();
  }
  if (count==0) return; // no output formats enabled.

  DocNode *root=0;
  if (docStr.at(docStr.length()-1)=='\n')
  {
    root = validatingParseDoc(fileName,startLine,
                              ctx,md,docStr,indexWords,isExample,exampleName);
  }
  else
  {
    root = validatingParseDoc(fileName,startLine,
                              ctx,md,docStr+"\n",indexWords,isExample,exampleName);
  }

  og=outputs->first();
  while (og)
  {
    if (og->isEnabled()) og->printDoc(root);
    og=outputs->next();
  }

  delete root;
}

void OutputList::parseText(const QCString &textStr)
{
  int count=0;
  OutputGenerator *og=outputs->first();
  while (og)
  {
    if (og->isEnabled()) count++;
    og=outputs->next();
  }
  if (count==0) return; // no output formats enabled.

  DocNode *root = validatingParseText(textStr);

  og=outputs->first();
  while (og)
  {
    if (og->isEnabled()) og->printDoc(root);
    og=outputs->next();
  }

  delete root;
}


//--------------------------------------------------------------------------
// Create some overloaded definitions of the forall function.
// Using template functions here would have made it a little less
// portable (I guess).

// zero arguments
void OutputList::forall(void (OutputGenerator::*func)())
{
  OutputGenerator *og=outputs->first();
  while (og)
  {
    if (og->isEnabled()) (og->*func)();
    og=outputs->next();
  }
}

// one argument
#define FORALL1(a1,p1)                                        \
void OutputList::forall(void (OutputGenerator::*func)(a1),a1) \
{                                                             \
  OutputGenerator *og=outputs->first();                       \
  while (og)                                                  \
  {                                                           \
    if (og->isEnabled()) (og->*func)(p1);                     \
    og=outputs->next();                                       \
  }                                                           \
}                     

// two arguments
#define FORALL2(a1,a2,p1,p2)                                        \
void OutputList::forall(void (OutputGenerator::*func)(a1,a2),a1,a2) \
{                                                                   \
  OutputGenerator *og=outputs->first();                             \
  while (og)                                                        \
  {                                                                 \
    if (og->isEnabled()) (og->*func)(p1,p2);                        \
    og=outputs->next();                                             \
  }                                                                 \
}                     

// three arguments
#define FORALL3(a1,a2,a3,p1,p2,p3)                                        \
void OutputList::forall(void (OutputGenerator::*func)(a1,a2,a3),a1,a2,a3) \
{                                                                         \
  OutputGenerator *og=outputs->first();                                   \
  while (og)                                                              \
  {                                                                       \
    if (og->isEnabled()) (og->*func)(p1,p2,p3);                           \
    og=outputs->next();                                                   \
  }                                                                       \
}                     

// four arguments
#define FORALL4(a1,a2,a3,a4,p1,p2,p3,p4)                                        \
void OutputList::forall(void (OutputGenerator::*func)(a1,a2,a3,a4),a1,a2,a3,a4) \
{                                                                               \
  OutputGenerator *og=outputs->first();                                         \
  while (og)                                                                    \
  {                                                                             \
    if (og->isEnabled()) (og->*func)(p1,p2,p3,p4);                              \
    og=outputs->next();                                                         \
  }                                                                             \
}                     

// now instantiate only the ones we need.

FORALL1(const char *a1,a1)
FORALL1(char a1,a1)
FORALL1(int a1,a1)
FORALL1(DotClassGraph &a1,a1)
FORALL1(DotInclDepGraph &a1,a1)
FORALL1(DotCallGraph &a1,a1)
FORALL1(DotGfxHierarchyTable &a1,a1)
FORALL1(SectionTypes a1,a1)
#if defined(HAS_BOOL_TYPE) || defined(Q_HAS_BOOL_TYPE)
FORALL1(bool a1,a1)
FORALL2(bool a1,int a2,a1,a2)
FORALL2(bool a1,bool a2,a1,a2)
FORALL4(const char *a1,const char *a2,const char *a3,bool a4,a1,a2,a3,a4)
#endif
FORALL2(int a1,bool a2,a1,a2)
FORALL2(bool a1,HighlightedItem a2,a1,a2)
FORALL2(bool a1,const char *a2,a1,a2)
FORALL2(ParamListTypes a1,const char *a2,a1,a2)
FORALL1(IndexSections a1,a1)
FORALL2(const char *a1,const char *a2,a1,a2)
FORALL2(const char *a1,bool a2,a1,a2)
FORALL2(const char *a1,SectionInfo::SectionType a2,a1,a2)
FORALL3(ClassDiagram &a1,const char *a2,const char *a3,a1,a2,a3)
FORALL3(const char *a1,const char *a2,const char *a3,a1,a2,a3)
FORALL3(const char *a1,const char *a2,bool a3,a1,a2,a3)
FORALL3(const char *a1,const char *a2,SectionInfo::SectionType a3,a1,a2,a3)
FORALL3(uchar a1,uchar a2,uchar a3,a1,a2,a3)
FORALL4(SectionTypes a1,const char *a2,const char *a3,const char *a4,a1,a2,a3,a4)
FORALL4(const char *a1,const char *a2,const char *a3,const char *a4,a1,a2,a3,a4)
FORALL4(const char *a1,const char *a2,const char *a3,int a4,a1,a2,a3,a4)


//--------------------------------------------------------------------------

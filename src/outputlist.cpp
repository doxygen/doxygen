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

OutputList::OutputList(bool)
{
  //printf("OutputList::OutputList()\n");
  outputs = new QList<OutputGenerator>;
  outputs->setAutoDelete(TRUE);
}

OutputList::OutputList(const OutputList *olist)
{
  //printf("OutputList::OutputList() deep copy\n");
  outputs = new QList<OutputGenerator>;
  outputs->setAutoDelete(TRUE);

  QList<OutputGenerator> *ol=olist->outputs;
  OutputGenerator *og=ol->first();
  while (og)
  {
    OutputGenerator *ogc=og->copy();
    outputs->append(ogc); 
    if (og->isEnabled()) ogc->enable(); else ogc->disable();  
    og=ol->next();
  }
  //printf("OutputList::OutputList dst=%d res=%d\n",ol->count(),outputs->count());
}

OutputList::~OutputList()
{
  //printf("OutputList::~OutputList()\n");
  delete outputs;
}

OutputList &OutputList::operator=(const OutputList &olist)
{
  if (this!=&olist)
  {
    QList<OutputGenerator> *ol=olist.outputs;
    OutputGenerator *ogsrc=ol->first();
    OutputGenerator *ogdst=outputs->first();
    //printf("OutputList::operator= src=%d dst=%d\n",outputs->count(),ol->count());
    while (ogdst)
    {
      ogdst=ogsrc->copy();
      ogsrc=ol->next();
      ogdst=outputs->next();
    }
  }
  return *this;
}

void OutputList::add(const OutputGenerator *og)
{
  if (og) outputs->append(og);
}

//HtmlGenerator *OutputList::getHtmlGenerator()
//{
//  OutputGenerator *og=outputs->first();
//  while (og)
//  {
//    if (og->get(OutputGenerator::Html)) return (HtmlGenerator *)og;
//    og=outputs->next();
//  }
//  return 0; // should not happen!
//}

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

OutputList &OutputList::operator+=(const OutputList &outputList)
{
  OutputList *ol=(OutputList *)&outputList;
  OutputGenerator *ogsrc=ol->outputs->first();
  OutputGenerator *ogdst=outputs->first();
  //printf("OutputList::operator+= src=%d dst=%d\n",outputs->count(),ol->outputs->count());
  while (ogdst && ogsrc)
  {
    ogdst->append(ogsrc);
    ogsrc=ol->outputs->next();
    ogdst=outputs->next();
  }
  return *this;
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
FORALL1(DotGfxHierarchyTable &a1,a1)
#if defined(HAS_BOOL_TYPE) || defined(Q_HAS_BOOL_TYPE)
FORALL1(bool a1,a1)
FORALL2(bool a1,int a2,a1,a2)
FORALL2(bool a1,bool a2,a1,a2)
#endif
FORALL2(int a1,bool a2,a1,a2)
FORALL1(IndexSections a1,a1)
FORALL2(const char *a1,const char *a2,a1,a2)
FORALL2(const char *a1,bool a2,a1,a2)
FORALL3(ClassDiagram &a1,const char *a2,const char *a3,a1,a2,a3)
FORALL3(const char *a1,const char *a2,const char *a3,a1,a2,a3)
FORALL3(const char *a1,const char *a2,bool a3,a1,a2,a3)
FORALL3(uchar a1,uchar a2,uchar a3,a1,a2,a3)
FORALL4(const char *a1,const char *a2,const char *a3,const char *a4,a1,a2,a3,a4)
FORALL4(const char *a1,const char *a2,const char *a3,bool a4,a1,a2,a3,a4)


//--------------------------------------------------------------------------

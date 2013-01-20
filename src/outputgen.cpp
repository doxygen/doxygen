/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include <stdlib.h>

#include <qfile.h>

#include "outputgen.h"
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator()
{
  //printf("OutputGenerator::OutputGenerator()\n");
  file=0;
  active=TRUE;
  genStack = new QStack<bool>;
  genStack->setAutoDelete(TRUE);
}

OutputGenerator::~OutputGenerator()
{
  //printf("OutputGenerator::~OutputGenerator()\n");
  delete file;
  delete genStack;
}

void OutputGenerator::startPlainFile(const char *name)
{
  //printf("startPlainFile(%s)\n",name);
  fileName=dir+"/"+name;
  file = new QFile(fileName);
  if (!file)
  {
    err("Could not create file object for %s\n",fileName.data());
    exit(1);
  }
  if (!file->open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",fileName.data());
    exit(1);
  }
  t.setDevice(file);
}

void OutputGenerator::endPlainFile()
{
  t.unsetDevice();
  delete file;
  file=0;
  fileName.resize(0);
}

void OutputGenerator::pushGeneratorState()
{
  genStack->push(new bool(isEnabled()));
  //printf("%p:pushGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
}

void OutputGenerator::popGeneratorState()
{
  //printf("%p:popGeneratorState(%d) enabled=%d\n",this,genStack->count(),isEnabled());
  bool *lb = genStack->pop();
  ASSERT(lb!=0);
  if (lb==0) return; // for some robustness against superfluous \endhtmlonly commands.
  if (*lb) enable(); else disable();
  delete lb;
}


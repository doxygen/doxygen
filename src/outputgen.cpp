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

#include <stdlib.h>
#include "outputgen.h"
#include <qstring.h>
#include "message.h"

OutputGenerator::OutputGenerator()
{
  //printf("OutputGenerator::OutputGenerator()\n");
  file=0;
  b.setBuffer(a);
  b.open( IO_WriteOnly );
  t.setDevice(&b);
  active=TRUE;
}

OutputGenerator::~OutputGenerator()
{
  //printf("OutputGenerator::~OutputGenerator()\n");
  delete file;
}

void OutputGenerator::startPlainFile(const char *name)
{
  //printf("startPlainFile(%s)\n",name);
  QString fileName=dir+"/"+name;
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
  fs.setDevice(file);
}

void OutputGenerator::endPlainFile()
{
  //printf("endPlainFile(%s)\n",file->name());
  fs.writeRawBytes(a.data(),a.size()) ;  // write string buffer to file
  delete file; 
  file=0;
  b.close();
  a.resize(0);
  b.setBuffer(a);
  b.open(IO_WriteOnly);
  t.setDevice(&b);
}

QString OutputGenerator::getContents() const
{
  QString s;
  s.resize(a.size()+1);
  memcpy(s.data(),a.data(),a.size());
  s.at(a.size())='\0';
  return s;
}


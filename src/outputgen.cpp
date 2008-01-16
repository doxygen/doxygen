/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "outputgen.h"
#include "message.h"
#include "portable.h"

OutputGenerator::OutputGenerator()
{
  //printf("OutputGenerator::OutputGenerator()\n");
  file=0;
  b.setBuffer(a);
  b.open( IO_WriteOnly );
  t.setDevice(&b);
#if QT_VERSION >= 200
  t.setEncoding(QTextStream::Latin1);
#endif
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
  QCString fileName=dir+"/"+name;
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
  bool converted=false;
  if (!encoding.isEmpty())
  {
    QByteArray enc(a.size()*4);
    void *cd = portable_iconv_open(encoding,"UTF-8");
    if (cd!=(void *)(-1))
    {
      size_t iLeft=a.size();
      size_t oLeft=enc.size();
      const char *inputPtr = a.data();
      char *outputPtr = enc.data();
      if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
      {
        enc.resize(enc.size()-oLeft);
        postProcess(enc);
        //printf("a.size()=%d enc.size()=%d iLeft=%d oLeft=%d enc2.size()=%d\n",
        //    a.size(),enc.size(),iLeft,oLeft,enc2.size());
        fs.writeRawBytes(enc.data(),enc.size()) ;  // write string buffer to file
        converted=TRUE;
      }
      portable_iconv_close(cd);
    }
  }
  if (!converted)
  {
    //printf("endPlainFile(%s)\n",file->name());
    fs.writeRawBytes(a.data(),a.size()) ;  // write string buffer to file
  }
  b.close();
  delete file; 
  file=0;
  a.resize(0);
  b.setBuffer(a);
  b.open(IO_WriteOnly);
  t.setDevice(&b);
}

QCString OutputGenerator::getContents() const
{
  QCString s;
  s.resize(a.size()+1);
  memcpy(s.data(),a.data(),a.size());
  s.at(a.size())='\0';
  return s;
}

void OutputGenerator::pushGeneratorState()
{
  genStack->push(new bool(isEnabled()));
}

void OutputGenerator::popGeneratorState()
{
  bool *lb = genStack->pop();
  ASSERT(lb!=0);
  if (lb==0) return; // for some robustness against superfluous \endhtmlonly commands.
  if (*lb) enable(); else disable();
  delete lb;
}


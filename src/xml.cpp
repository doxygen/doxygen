/******************************************************************************
 *
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

#include "qtbc.h"
#include "xml.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

const char dtd_data[]=
#include "xml_dtd.h"
;

void generateDTD()
{
  QCString fileName=Config::outputDir+"/xml/doxygen.dtd";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << dtd_data;
}

void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

void writeXMLLink(QTextStream &t,const char *compoundId,const char *memId,
                  const char *text)
{
  if (memId==0)
  {
    t << "<compoundref idref=\"" << compoundId << "\">";
    writeXMLString(t,text);
    t << "</compoundref>";
  }
  else
  {
    t << "<memberref idref=\"" << compoundId << ":" << memId << "\">";
    writeXMLString(t,text);
    t << "</memberref>";
  }
}

void generateXML()
{
  QDir dir(Config::outputDir);
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(Config::outputDir))
    {
      err("Cannot create directory %s\n",Config::outputDir.data());
      return;
    }
  }
  QDir xmlDir(Config::outputDir+"/xml");
  if (!xmlDir.exists() && !xmlDir.mkdir(Config::outputDir+"/xml"))
  {
    err("Could not create xml directory in %s\n",Config::outputDir.data());
    return;
  }
  generateDTD();
  
  QCString fileName=Config::outputDir+"/xml/doxygen.xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << "<?xml version='1.0' encoding='ISO-8859-1' standalone='no'?>" << endl;
  t << "<!DOCTYPE doxygen SYSTEM \"doxygen.dtd\">" << endl;
  t << "<doxygen>" << endl;
  if (classList.count()+inputNameList.count()>0)
  {
    t << "  <compoundlist>" << endl;
    ClassListIterator cli(classList);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      cd->generateXML(t);
    }
    FileNameListIterator fnli(inputNameList);
    FileName *fn;
    for (;(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (;(fd=fni.current());++fni)
      {
        fd->generateXML(t);
      }
    }
    t << "  </compoundlist>" << endl;
  }
  t << "</doxygen>" << endl;
}



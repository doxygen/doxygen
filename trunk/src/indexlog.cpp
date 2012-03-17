/*
 * Copyright (C) 2008 by Sebastian Pipping.
 * Copyright (C) 2008 Dimitri van Heesch.
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
 * Sebastian Pipping <sebastian@pipping.org>
 */

#include "indexlog.h"
#include "message.h"
#include "config.h"

#include <qstring.h>
#include <qfile.h>


IndexLog::IndexLog()
{
}

IndexLog::~IndexLog()
{
}

void IndexLog::initialize()
{
  char const * const attributes[] =
  { "xmlns", 
    "http://doxygen.org/xmlns/indexlog/1/0/",
     NULL 
  };
  m_out.open("log", attributes);

  openMethodCall("initialize");
  closeMethodCall();
}

void IndexLog::finalize()
{
  openMethodCall("finalize");
  closeMethodCall();
  m_out.close("log");
  QCString fileName = Config_getString("HTML_OUTPUT")+"/index.log.xml";
  QFile file(fileName);
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n", fileName.data());
    exit(1);
  }
  m_out.dumpTo(file);
  file.flush();
  file.close();
}

void IndexLog::incContentsDepth()
{
  openMethodCall("incContentsDepth");
  closeMethodCall();
}

void IndexLog::decContentsDepth()
{
  openMethodCall("decContentsDepth");
  closeMethodCall();
}

void IndexLog::addContentsItem(bool isDir, char const * name,
                                  char const * ref, char const * file, 
                                  char const * anchor)
{
  openMethodCall("addContentsItem");
  addBoolParameter("isDir", isDir);
  addStringParameter("name", name);
  addStringParameter("ref", ref);
  addStringParameter("file", file);
  addStringParameter("anchor", anchor);
  closeMethodCall();
}

void IndexLog::addIndexItem(char const * level1, char const * level2,
                               char const * contRef, char const * memRef, 
                               char const * anchor, const MemberDef * md)
{
  openMethodCall("addIndexItem");
  addStringParameter("level1", level1);
  addStringParameter("level2", level2);
  addStringParameter("contRef", contRef);
  addStringParameter("memRef", memRef);
  addStringParameter("anchor", anchor);
  addMemberDefParameter("md", md);
  closeMethodCall();
}

void IndexLog::addIndexFile(char const * name)
{
  openMethodCall("addIndexFile");
  addStringParameter("name", name);
  closeMethodCall();
}

void IndexLog::openMethodCall(char const * methodName)
{
  m_out.setCompressionEnabled(true);
  m_out.open("call");
  m_out.openCloseContent("method", methodName);
}

void IndexLog::addPrimitiveParameter(char const * parameterName,
    char const * value)
{
  m_out.open("param");
  m_out.openCloseContent("name", parameterName);
  if (value != NULL)
  {
    m_out.openCloseContent("value", value);
  }
  m_out.close("param");
}

void IndexLog::addBoolParameter(char const * parameterName, bool value)
{
  addPrimitiveParameter(parameterName, value ? "true" : "false");
}

void IndexLog::addStringParameter(char const * parameterName,
                                     char const * value)
{
  addPrimitiveParameter(parameterName, value);
}

void IndexLog::addMemberDefParameter(char const * parameterName,
                                        const MemberDef * /*value*/)
{
  m_out.open("param");
  m_out.openCloseContent("name", parameterName);
  m_out.close("param");
}

void IndexLog::closeMethodCall()
{
  m_out.setCompressionEnabled(false);
  m_out.close("call");
}


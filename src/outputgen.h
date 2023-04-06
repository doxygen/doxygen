/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef OUTPUTGEN_H
#define OUTPUTGEN_H

#include <memory>
#include <stack>
#include <iostream>
#include <fstream>

#include "types.h"
#include "index.h"
#include "section.h"
#include "textstream.h"
#include "docparser.h"

class ClassDiagram;
class DotClassGraph;
class DotInclDepGraph;
class DotCallGraph;
class DotDirDeps;
class DotGfxHierarchyTable;
class DotGroupCollaboration;
class MemberDef;
class Definition;
class OutputCodeList;

struct DocLinkInfo
{
  QCString name;
  QCString ref;
  QCString url;
  QCString anchor;
};

struct SourceLinkInfo
{
  QCString file;
  int line;
  QCString ref;
  QCString url;
  QCString anchor;
};

enum class OutputType { List, Html, Latex, Man, RTF, Docbook, XML, Null, Extension };

/** Base class for specific output generators.
 *
 *  Subclass this class to add support for a new output format
 */
class OutputGenerator
{
  public:
    OutputGenerator() : m_t(40*1024) {}

    enum class ParamListTypes { Param, RetVal, Exception };
    enum class MemberItemType { Normal, AnonymousStart, AnonymousEnd, Templated };

    OutputGenerator(const QCString &dir);
    OutputGenerator(const OutputGenerator &) = delete;
    OutputGenerator &operator=(const OutputGenerator &) = delete;
    OutputGenerator(OutputGenerator &&) = default;
    OutputGenerator &operator=(OutputGenerator &&) = delete;
   ~OutputGenerator() = default;

    QCString dir() const;
    QCString fileName() const;

    void startPlainFile(const QCString &name);
    void endPlainFile();
  protected:
    TextStream m_t;
    QCString m_dir;
  private:
    QCString m_fileName;
    FILE *m_file = nullptr;
};



#endif

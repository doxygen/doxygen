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
#include "construct.h"

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

enum class OutputType { List, Html, Latex, Man, RTF, Docbook, XML, Null, Extension, Recorder };


/** Base class for code generators
 */
class OutputCodeIntf
{
  public:
    OutputCodeIntf() = default;
    DEFAULT_COPYABLE(OutputCodeIntf)

    virtual OutputType type() const = 0;
    virtual std::unique_ptr<OutputCodeIntf> clone() = 0;
    virtual void codify(const QCString &s) = 0;
    virtual void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) = 0;
    virtual void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) = 0;
    virtual void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) = 0;
    virtual void startCodeLine(int lineNr) = 0;
    virtual void endCodeLine()= 0;
    virtual void startFontClass(const QCString &c) = 0;
    virtual void endFontClass() = 0;
    virtual void writeCodeAnchor(const QCString &name) = 0;
    virtual void startCodeFragment(const QCString &style) = 0;
    virtual void endCodeFragment(const QCString &style) = 0;
    virtual void startFold(int lineNr,const QCString &startMarker,const QCString &endMarker) = 0;
    virtual void endFold() = 0;
};



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
   ~OutputGenerator() = default;
    ONLY_DEFAULT_MOVABLE(OutputGenerator)

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

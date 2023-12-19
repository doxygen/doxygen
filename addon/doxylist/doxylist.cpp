/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

/** @file
 *  @brief Get which symbols are defined on which file/line.
 *
 *  Print output in JSON format.
 */

#include <stdlib.h>
#include <fstream>

#include "dir.h"
#include "doxygen.h"
#include "outputgen.h"
#include "outputlist.h"
#include "parserintf.h"
#include "classdef.h"
#include "namespacedef.h"
#include "filedef.h"
#include "util.h"
#include "classlist.h"
#include "config.h"
#include "filename.h"
#include "version.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class XRefDummyCodeGenerator : public OutputCodeExtension
{
  public:
    XRefDummyCodeGenerator(FileDef *fd) : m_fd(fd) {}
   ~XRefDummyCodeGenerator() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    OutputType type() const override { return OutputType::Extension; }
    void codify(const QCString &) override {}
    void writeCodeLink(CodeSymbolType,const QCString &,const QCString &,const QCString &,const QCString &,const QCString &) override  {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int,bool) override {}
    virtual void writeTooltip(const QCString &,const DocLinkInfo &,
                              const QCString &,const QCString &,const SourceLinkInfo &,
                              const SourceLinkInfo &) override {}
    void startCodeLine(int) override {}
    void endCodeLine() override {}
    void startFontClass(const QCString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const QCString &) override {}
    void startCodeFragment(const QCString &) override {}
    void endCodeFragment(const QCString &) override {}
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}

    // here we are presented with the symbols found by the code parser
    void linkableSymbol(int l, const char *sym,Definition *symDef,Definition *context)
    {
      QCString ctx;
      if (context) // the context of the symbol is known
      {
        if (context->definitionType()==Definition::TypeMember) // it is inside a member
        {
          Definition *parentContext = context->getOuterScope();
          if (parentContext && parentContext->definitionType()==Definition::TypeClass)
             // it is inside a member of a class
          {
            ctx.sprintf("inside %s %s of %s %s",
              (dynamic_cast<MemberDef*>(context))->memberTypeName().data(),
              context->name().data(),
              (dynamic_cast<ClassDef*>(parentContext))->compoundTypeString().data(),
              parentContext->name().data());
          }
          else if (parentContext==Doxygen::globalScope) // it is inside a global member
          {
            ctx.sprintf("inside %s %s",
              (dynamic_cast<MemberDef*>(context))->memberTypeName().data(),
              context->name().data());
          }
        }
        if (ctx.isEmpty()) // it is something else (class, or namespace member, ...)
        {
          ctx.sprintf("in %s",context->name().data());
        }
      }
      printf("Found symbol %s at line %d of %s %s\n",
          sym,l,m_fd->getDefFileName().data(),ctx.data());
      if (symDef && context) // in this case the definition of the symbol is
        // known to doxygen.
      {
        printf("-> defined at line %d of %s\n",
            symDef->getDefLine(),symDef->getDefFileName().data());
      }
    }
  private:
    FileDef *m_fd;
};

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  auto intf=Doxygen::parserManager->getCodeParser(fd->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(fd->name());

  // reset the parsers state
  intf->resetCodeParserState();

  // create a new backend object
  XRefDummyCodeGenerator xrefGen(fd);
  OutputCodeList xrefList;
  xrefList.add(OutputCodeDeferExtension(&xrefGen));

  // parse the source code
  intf->parseCode(xrefList,
                0,
                fileToString(fd->absFilePath()),
                lang,
                FALSE,
                0,
                fd);
}

static void listSymbols(bool details)
{
  json ret;
  for (const auto &kv : *Doxygen::symbolMap)
  {
    for (const auto &def : kv.second)
    {
      if (def->hasDocumentation())
      {
        if (!details)
        {
          ret[def->getDefFileName().data()][def->qualifiedName().data()] = def->getDefLine();
        }
        else
        {
          json item;
          if (def->hasBriefDescription())
          {
            item["briefLine"] = def->briefLine();
          }
          item["docLine"] = def->docLine();
          item["defLine"] = def->getDefLine();
          item["defColumn"] = def->getDefColumn();
          item["startDefLine"] = def->getStartDefLine();
          item["startBodyLine"] = def->getStartBodyLine();
          item["endBodyLine"] = def->getEndBodyLine();
          item["inbodyLine"] = def->inbodyLine();
          ret[def->getDefFileName().data()][def->qualifiedName().data()] = item;
        }
      }
    }
  }
  std::cout << ret.dump(4) << std::endl;
}

int main(int argc,char **argv)
{
  std::string tempdir = "/tmp/doxygen";
  std::string usage = "Usage: %s [--version] [--help] [--details] [--tempdir ARG] path [path...]\n";
  StringVector inputList;
  bool details = false;

  for (size_t i = 0; i < argc; i++)
  {
    if (std::string(argv[i]) == "--version")
    {
      printf("%s version: %s\n",argv[0],getFullVersion().c_str());
      exit(0);
    }
    if (std::string(argv[i]) == "--help")
    {
      printf(usage.c_str(), argv[0]);
      exit(0);
    }
    if (std::string(argv[i]) == "--details")
    {
      details = true;
      continue;
    }
    if (std::string(argv[i]) == "--tempdir")
    {
      if (i+1 >= argc)
      {
        printf(usage.c_str(), argv[0]);
        exit(1);
      }
      tempdir = argv[i+1];
      i++;
      continue;
    }
    inputList.push_back(argv[i]);
  }

  // initialize data structures
  initDoxygen();

  // setup the non-default configuration options

  checkConfiguration();
  adjustConfiguration();
  // we need a place to put intermediate files
  Config_updateString(OUTPUT_DIRECTORY, tempdir.c_str());
  // disable html output
  Config_updateBool(GENERATE_HTML,FALSE);
  // disable latex output
  Config_updateBool(GENERATE_LATEX,FALSE);
  // be quiet
  Config_updateBool(QUIET,TRUE);
  // turn off warnings
  Config_updateBool(WARNINGS,FALSE);
  Config_updateBool(WARN_IF_UNDOCUMENTED,FALSE);
  Config_updateBool(WARN_IF_DOC_ERROR,FALSE);
  Config_updateBool(WARN_IF_UNDOC_ENUM_VAL,FALSE);
  // Extract as much as possible
  Config_updateBool(EXTRACT_ALL,TRUE);
  Config_updateBool(EXTRACT_STATIC,TRUE);
  Config_updateBool(EXTRACT_PRIVATE,TRUE);
  Config_updateBool(EXTRACT_LOCAL_METHODS,TRUE);
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_updateBool(SOURCE_BROWSER,TRUE);
  // In case of a directory take all files on directory and its subdirectories
  Config_updateBool(RECURSIVE,TRUE);

  // set the input
  Config_updateList(INPUT,inputList);

  // parse the files
  parseInput();

  // iterate over the input files
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      // get the references (linked and unlinked) found in this file
      findXRefSymbols(fd.get());
    }
  }

  // clean up after us
  Dir().rmdir(tempdir.c_str());

  listSymbols(details);
}


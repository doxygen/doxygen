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
 *  @brief Example of how to use doxygen as part of another GPL applications
 *
 *  This example shows how to configure and run doxygen programmatically from
 *  within an application without generating the usual output.
 *  The example should work on any Unix like OS (including Linux and Mac OS X).
 *
 *  This example shows how to use to code parser to get cross-references information
 *  and it also shows how to look up symbols in a program parsed by doxygen and
 *  show some information about them.
 */

#include <stdlib.h>
#include "dir.h"
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"
#include "classdef.h"
#include "namespacedef.h"
#include "filedef.h"
#include "util.h"
#include "classlist.h"
#include "config.h"
#include "filename.h"
#include "version.h"

class XRefDummyCodeGenerator : public CodeOutputInterface
{
  public:
    XRefDummyCodeGenerator(FileDef *fd) : m_fd(fd) {}
   ~XRefDummyCodeGenerator() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    OutputType type() const override { return OutputType::Null; }
    void codify(const QCString &) override {}
    void writeCodeLink(CodeSymbolType,const QCString &,const QCString &,const QCString &,const QCString &,const QCString &) override  {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int,bool) override {}
    virtual void writeTooltip(const QCString &,const DocLinkInfo &,
                              const QCString &,const QCString &,const SourceLinkInfo &,
                              const SourceLinkInfo &) override {}
    void startCodeLine(bool) override {}
    void endCodeLine() override {}
    void startFontClass(const QCString &) override {}
    void endFontClass() override {}
    void writeCodeAnchor(const QCString &) override {}
    void setCurrentDoc(const Definition *,const QCString &,bool) override {}
    void addWord(const QCString &,bool) override {}
    void startCodeFragment(const QCString &) override {}
    void endCodeFragment(const QCString &) override {}

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
  XRefDummyCodeGenerator *xrefGen = new XRefDummyCodeGenerator(fd);

  // parse the source code
  intf->parseCode(*xrefGen,
                0,
                fileToString(fd->absFilePath()),
                lang,
                FALSE,
                0,
                fd);

  // dismiss the object.
  delete xrefGen;
}

static void listSymbol(Definition *d)
{
  if (d!=Doxygen::globalScope && // skip the global namespace symbol
      d->name().at(0)!='@'       // skip anonymous stuff
     )
  {
    printf("%s\n",
        d->name().data());
  }
}

static void listSymbols()
{
  for (const auto &kv : *Doxygen::symbolMap)
  {
    for (const auto &def : kv.second)
    {
      listSymbol(def);
    }
  }
}

static void lookupSymbol(const Definition *d)
{
  if (d!=Doxygen::globalScope && // skip the global namespace symbol
      d->name().at(0)!='@'       // skip anonymous stuff
     )
  {
    printf("Symbol info\n");
    printf("-----------\n");
    printf("Name: %s\n",d->name().data());
    printf("File: %s\n",d->getDefFileName().data());
    printf("Line: %d\n",d->getDefLine());
    // depending on the definition type we can case to the appropriate
    // derived to get additional information
    switch (d->definitionType())
    {
      case Definition::TypeClass:
        {
          const ClassDef *cd = dynamic_cast<const ClassDef*>(d);
          printf("Kind: %s\n",cd->compoundTypeString().data());
        }
        break;
      case Definition::TypeFile:
        {
          const FileDef *fd = dynamic_cast<const FileDef*>(d);
          printf("Kind: File: #includes %zu other files\n",
              fd->includeFileList().size());
        }
        break;
      case Definition::TypeNamespace:
        {
          const NamespaceDef *nd = dynamic_cast<const NamespaceDef*>(d);
          printf("Kind: Namespace: contains %zu classes and %zu namespaces\n",
              nd->getClasses().size(),
              nd->getNamespaces().size());
        }
        break;
      case Definition::TypeMember:
        {
          const MemberDef *md = dynamic_cast<const MemberDef*>(d);
          printf("Kind: %s\n",md->memberTypeName().data());
        }
        break;
      default:
        // ignore groups/pages/packages/dirs for now
        break;
    }
  }
}

static void lookupSymbols(const QCString &sym)
{
  if (!sym.isEmpty())
  {
    auto range = Doxygen::symbolMap->find(sym);
    bool found=false;
    for (const Definition *def : range)
    {
      lookupSymbol(def);
      found=true;
    }
    if (!found)
    {
      printf("Unknown symbol\n");
    }
  }
}

int main(int argc,char **argv)
{
  char cmd[256];

  int locArgc = argc;

  if (locArgc == 2)
  {
    if (!strcmp(argv[1],"--help"))
    {
      printf("Usage: %s [source_file | source_dir]\n",argv[0]);
      exit(0);
    }
    else if (!strcmp(argv[1],"--version"))
    {
      printf("%s version: %s\n",argv[0],getFullVersion());
      exit(0);
    }
  }

  if (locArgc!=2)
  {
    printf("Usage: %s [source_file | source_dir]\n",argv[0]);
    exit(1);
  }

  // initialize data structures
  initDoxygen();

  // setup the non-default configuration options

  checkConfiguration();
  adjustConfiguration();
  // we need a place to put intermediate files
  Config_updateString(OUTPUT_DIRECTORY,"/tmp/doxygen");
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
  StringVector inputList;
  inputList.push_back(argv[1]);
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
  Dir().rmdir("/tmp/doxygen");

  while (1)
  {
    printf("> Type a symbol name or\n> .list for a list of symbols or\n> .quit to exit\n> ");
    fgets(cmd,256,stdin);
    QCString s(cmd);
    if (s.at(s.length()-1)=='\n') s=s.left(s.length()-1); // strip trailing \n
    if (s==".list")
      listSymbols();
    else if (s==".quit")
      exit(0);
    else
      lookupSymbols(s);
  }
}


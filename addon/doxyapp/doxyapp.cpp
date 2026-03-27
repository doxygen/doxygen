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
 *
 *  If you are running a non-UNIX-like system you should specify `--tempdir`.
 *
 *  This example shows how to use to code parser to get cross-references information
 *  and it also shows how to look up symbols in a program parsed by doxygen and
 *  show some information about them.
 */

#include <stdlib.h>
#include <map>
#include <string>
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

class XRefDummyCodeGenerator : public OutputCodeIntf
{
  public:
    XRefDummyCodeGenerator(FileDef *fd) : m_fd(fd) {}
   ~XRefDummyCodeGenerator() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    OutputType type() const override { return OutputType::Extension; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<XRefDummyCodeGenerator>(m_fd); }
    void codify(const QCString &) override {}
    void stripCodeComments(bool) override {}
    void startSpecialComment() override {}
    void endSpecialComment() override {}
    void setStripIndentAmount(size_t) override {}
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
  std::unique_ptr<OutputCodeIntf> xrefGen = std::make_unique<XRefDummyCodeGenerator>(fd);
  OutputCodeList xrefList;
  xrefList.add(std::move(xrefGen));

  // parse the source code
  intf->parseCode(xrefList,
                QCString(),
                fileToString(fd->absFilePath()),
                lang,
                FALSE,
                CodeParserOptions()
                .setFileDef(fd));
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

template <typename Iter>
std::string join(Iter begin, Iter end, std::string const& separator)
{
  std::ostringstream result;
  if (begin != end)
    result << *begin++;
  while (begin != end)
    result << separator << *begin++;
  return result.str();
}

static auto symbolInfo(const Definition *def)
{
  std::map<std::string, int> ret;
  if (def->hasDocumentation())
  {
    if (def->hasBriefDescription())
      ret["briefLine"] = def->briefLine();
    ret["docLine"] = def->docLine();
  }
  ret["defLine"] = def->getDefLine();
  ret["defColumn"] = def->getDefColumn();
  ret["startDefLine"] = def->getStartDefLine();
  ret["startBodyLine"] = def->getStartBodyLine();
  ret["endBodyLine"] = def->getEndBodyLine();
  ret["inbodyLine"] = def->inbodyLine();
  return ret;
}

static void locateSymbols()
{
  std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, int>>>> ret;
  for (const auto &kv : *Doxygen::symbolMap)
  {
    for (const auto &def : kv.second)
    {
      if (def == Doxygen::globalScope || def->name().at(0) == '@')
        continue;

      QCString args = "";
      if (def->definitionType() == Definition::TypeMember)
      {
        const auto *md = dynamic_cast<MemberDef*>(def);
        args = md->argsString();
      }
      ret[def->getDefFileName().data()][def->qualifiedName().data()][args.data()] = symbolInfo(def);
    }
  }

  // print as json
  std::vector<std::string> out;
  for (const auto &[fname, qmap] : ret)
  {
    out.push_back(std::string(4, ' ') + "\"" + fname + "\": {\n");
    std::vector<std::string> file;
    for (const auto &[qname, arg_map] : qmap)
    {
      file.push_back(std::string(8, ' ') + "\"" + qname + "\": {\n");
      std::vector<std::string> name;
      for (const auto &[args, imap] : arg_map)
      {
        name.push_back(std::string(12, ' ') + "\"" + args + "\": {\n");
        std::vector<std::string> item;
        for (const auto &[key, value] : imap)
        {
          item.push_back(std::string(16, ' ') + "\"" + key + "\": " + std::to_string(value));
        }
        name.back() += join(item.begin(), item.end(), ",\n");
        name.back() += "\n" + std::string(12, ' ') + "}";
      }
      file.back() += join(name.begin(), name.end(), ",\n");
      file.back() += "\n" + std::string(8, ' ') + "}";
    }
    out.back() += join(file.begin(), file.end(), ",\n");
    out.back() += "\n" + std::string(4, ' ') + "}";
  }
  std::cout << "{\n" << join(out.begin(), out.end(), ",\n") << "\n}\n";
}

int main(int argc,char **argv)
{
  std::string tempdir = "/tmp/doxygen";
  std::string usage = "Usage: %s [--version] [--help] [--list] [--locate] [--tempdir ARG] path [path...]\n";
  StringVector inputList;
  bool list = false;
  bool locate = false;

  for (size_t i = 1; i < argc; i++)
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
    if (std::string(argv[i]) == "--list")
    {
      if (locate)
      {
        printf(usage.c_str(), argv[0]);
        exit(1);
      }
      list = true;
      continue;
    }
    if (std::string(argv[i]) == "--locate")
    {
      if (list)
      {
        printf(usage.c_str(), argv[0]);
        exit(1);
      }
      locate = true;
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

  if (list)
  {
    listSymbols();
    exit(0);
  }

  if (locate)
  {
    locateSymbols();
    exit(0);
  }

  char cmd[256];
  while (1)
  {
    printf("> Type a symbol name or\n> .list for a list of symbols or\n> .quit to exit\n> ");
    (void)fgets(cmd,256,stdin);
    QCString s(cmd);
    if (s.at(s.length()-1)=='\n') s=s.left(s.length()-1); // strip trailing \n
    if (s==".list")
      listSymbols();
    else if (s==".locate")
      locateSymbols();
    else if (s==".quit")
      exit(0);
    else
      lookupSymbols(s);
  }
}


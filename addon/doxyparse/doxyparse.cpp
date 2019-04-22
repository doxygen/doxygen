/******************************************************************************
 *
 * Copyright (C) 2009-2015 by Joenio Costa.
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
 *  @brief Code parse based on doxyapp by Dimitri van Heesch
 *
 */

#include <stdlib.h>
#if !defined(_WIN32) || defined(__CYGWIN__)
  #include <unistd.h>
#else
  #include <windows.h>
#endif
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"
#include "classlist.h"
#include "config.h"
#include "filedef.h"
#include "util.h"
#include "filename.h"
#include "arguments.h"
#include "memberlist.h"
#include "types.h"
#include <string>
#include <cstdlib>
#include <sstream>
#include <map>
#include <qdir.h>

class Doxyparse : public CodeOutputInterface
{
  public:
    Doxyparse(FileDef *fd) : m_fd(fd) {}
   ~Doxyparse() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    void codify(const char *) {}
    void writeCodeLink(const char *,const char *,const char *,const char *,const char *)  {}
    void startCodeLine() {}
    void endCodeLine() {}
    void startCodeAnchor(const char *) {}
    void endCodeAnchor() {}
    void startFontClass(const char *) {}
    void endFontClass() {}
    void writeCodeAnchor(const char *) {}
    void writeLineNumber(const char *,const char *,const char *,int) {}
    virtual void writeTooltip(const char *,const DocLinkInfo &,
                              const char *,const char *,const SourceLinkInfo &,
                              const SourceLinkInfo &) {}
    void startCodeLine(bool) {}
    void setCurrentDoc(const Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}

    void linkableSymbol(int l, const char *sym, Definition *symDef, Definition *context)
    {
      if (!symDef) {
        // in this case we have a local or external symbol

        // TODO record use of external symbols
        // TODO must have a way to differentiate external symbols from local variables
      }
    }

  private:
    FileDef *m_fd;
};

static bool is_c_code = true;
static std::map<std::string, bool> modules;
static std::string current_module;

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  ParserInterface *pIntf=Doxygen::parserManager->getParser(fd->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(fd->name());

  // reset the parsers state
  pIntf->resetCodeParserState();

  // create a new backend object
  Doxyparse *parse = new Doxyparse(fd);

  // parse the source code
  pIntf->parseCode(*parse, 0, fileToString(fd->absFilePath()), lang, FALSE, 0, fd);

  // dismiss the object.
  delete parse;
}

static bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md) {
  if (md->isStatic()) {
    if(md->getFileDef()) {
      if(md->getFileDef()->getOutputFileBase() == context->getFileDef()->getOutputFileBase())
        // TODO ignore prefix of file
        return false;
      else
        return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

static void printFile(std::string file) {
  printf("%s:\n", file.c_str());
}
static void printModule(std::string module) {
  current_module = module;
  printf("  %s:\n", module.c_str());
}
static void printClassInformation(std::string information) {
  printf("    information: %s\n", information.c_str());
}
static void printInheritance(std::string base_class) {
  printf("    inherits: %s\n", base_class.c_str());
}
static void printDefines() {
  if (! modules[current_module]) {
    printf("    defines:\n");
  }
  modules[current_module] = true;
}
static void printDefinition(std::string type, std::string signature, int line) {
  printf("      - \"%s\":\n", signature.c_str());
  printf("          type: %s\n", type.c_str());
  printf("          line: %d\n", line);
}
static void printProtection(std::string protection) {
  printf("          protection: %s\n", protection.c_str());
}
static void printNumberOfLines(int lines) {
  printf("          lines_of_code: %d\n", lines);
}
static void printNumberOfArguments(int arguments) {
  printf("          parameters: %d\n", arguments);
}
static void printUses() {
  printf("          uses:\n");
}
static void printReferenceTo(std::string type, std::string signature, std::string defined_in) {
  printf("            - \"%s\":\n", signature.c_str());
  printf("                type: %s\n", type.c_str());
  printf("                defined_in: %s\n", defined_in.c_str());
}

static int isPartOfCStruct(MemberDef * md) {
  return is_c_code && md->getClassDef() != NULL;
}

std::string removeDoubleQuotes(std::string data) {
  // remove surrounding double quotes
  if (data.front() == '"' && data.back() == '"') {
    data.erase(0, 1);            // first double quote
    data.erase(data.size() - 1); // last double quote
  }
  return data;
}

std::string argumentData(Argument *argument) {
  std::string data = "";
  if (argument->type != NULL)
    data = removeDoubleQuotes(argument->type.data());
  else if (argument->name != NULL)
    data = removeDoubleQuotes(argument->name.data());
  return data;
}

std::string functionSignature(MemberDef* md) {
  std::string signature = md->name().data();
  if(md->isFunction()){
    ArgumentList *argList = md->argumentList();
    ArgumentListIterator iterator(*argList);
    signature += "(";
    Argument * argument = iterator.toFirst();
    if(argument != NULL) {
      signature += argumentData(argument);
      for(++iterator; (argument = iterator.current()); ++iterator){
        signature += std::string(",") + argumentData(argument);
      }
    }
    signature += ")";
  }
  return signature;
}

static void referenceTo(MemberDef* md) {
  std::string type = md->memberTypeName().data();
  std::string defined_in = "";
  std::string signature = "";
  if (isPartOfCStruct(md)) {
    signature = md->getClassDef()->name().data() + std::string("::") + functionSignature(md);
    defined_in = md->getClassDef()->getFileDef()->getOutputFileBase().data();
  }
  else {
    signature = functionSignature(md);
    if (md->getClassDef()) {
      defined_in = md->getClassDef()->name().data();
    }
    else if (md->getFileDef()) {
      defined_in = md->getFileDef()->getOutputFileBase().data();
    }
  }
  printReferenceTo(type, signature, defined_in);
}

void cModule(ClassDef* cd) {
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printDefinition("variable", cd->name().data() + std::string("::") + md->name().data(), md->getDefLine());
      if (md->protection() == Public) {
        printProtection("public");
      }
    }
  }
}

void functionInformation(MemberDef* md) {
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printNumberOfLines(size);
  ArgumentList *argList = md->argumentList();
  printNumberOfArguments(argList->count());
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    printUses();
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        referenceTo(rmd);
      }
    }
  }
}

static void lookupSymbol(Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = dynamic_cast<MemberDef*>(d);
    std::string type = md->memberTypeName().data();
    std::string signature = functionSignature(md);
    printDefinition(type, signature, md->getDefLine());
    if (md->protection() == Public) {
      printProtection("public");
    }
    if (md->isFunction()) {
      functionInformation(md);
    }
  }
}

void listMembers(MemberList *ml) {
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    printDefines();
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      lookupSymbol((Definition*) md);
    }
  }
}

void listAllMembers(ClassDef* cd) {
  // methods
  listMembers(cd->getMemberList(MemberListType_functionMembers));
  // constructors
  listMembers(cd->getMemberList(MemberListType_constructors));
  // attributes
  listMembers(cd->getMemberList(MemberListType_variableMembers));
}

static void classInformation(ClassDef* cd) {
  if (is_c_code) {
    cModule(cd);
  } else {
    printModule(cd->name().data());
    BaseClassList* baseClasses = cd->baseClasses();
    if (baseClasses) {
      BaseClassListIterator bci(*baseClasses);
      BaseClassDef* bcd;
      for (bci.toFirst(); (bcd = bci.current()); ++bci) {
        printInheritance(bcd->classDef->name().data());
      }
    }
    if(cd->isAbstract()) {
      printClassInformation("abstract class");
    }
    listAllMembers(cd);
  }
}

static bool checkLanguage(std::string& filename, std::string extension) {
  if (filename.find(extension, filename.size() - extension.size()) != std::string::npos) {
    return true;
  } else {
    return false;
  }
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
static void detectProgrammingLanguage(FileNameListIterator& fnli) {
  FileName* fn;
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    std::string filename = fn->fileName();
    if (
        checkLanguage(filename, ".cc") ||
        checkLanguage(filename, ".cxx") ||
        checkLanguage(filename, ".cpp") ||
        checkLanguage(filename, ".java") ||
        checkLanguage(filename, ".py") ||
        checkLanguage(filename, ".pyw") ||
        checkLanguage(filename, ".cs")
       ) {
      is_c_code = false;
    }
  }
}

static void listSymbols() {
  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;

  detectProgrammingLanguage(fnli);

  // for each file
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (; (fd=fni.current()); ++fni) {
      printFile(fd->absFilePath().data());
      MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
      if (ml && ml->count() > 0) {
        printModule(fd->getOutputFileBase().data());
        listMembers(ml);
      }

      ClassSDict *classes = fd->getClassSDict();
      if (classes) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          classInformation(cd);
        }
      }
    }
  }
  // TODO print external symbols referenced
}

int main(int argc,char **argv) {
  if (argc < 2) {
    printf("Usage: %s [source_file | source_dir]\n",argv[0]);
    exit(1);
  }

  // initialize data structures
  initDoxygen();

  // check and finalize the configuration
  checkConfiguration();
  adjustConfiguration();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  std::ostringstream tmpdir;
#if !defined(_WIN32) || defined(__CYGWIN__)
  unsigned int pid = (uint)getpid();
#else
  unsigned int pid = (uint)GetCurrentProcessId();
#endif
  tmpdir << "/tmp/doxyparse-" << pid;
  Config_getString(OUTPUT_DIRECTORY)= tmpdir.str().c_str();
  // enable HTML (fake) output to omit warning about missing output format
  Config_getBool(GENERATE_HTML)=TRUE;
  // disable latex output
  Config_getBool(GENERATE_LATEX)=FALSE;
  // be quiet
  Config_getBool(QUIET)=TRUE;
  // turn off warnings
  Config_getBool(WARNINGS)=FALSE;
  Config_getBool(WARN_IF_UNDOCUMENTED)=FALSE;
  Config_getBool(WARN_IF_DOC_ERROR)=FALSE;
  // Extract as much as possible
  Config_getBool(EXTRACT_ALL)=TRUE;
  Config_getBool(EXTRACT_STATIC)=TRUE;
  Config_getBool(EXTRACT_PRIVATE)=TRUE;
  Config_getBool(EXTRACT_LOCAL_METHODS)=TRUE;
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_getBool(SOURCE_BROWSER)=TRUE;
  // find functions call between modules
  Config_getBool(CALL_GRAPH)=TRUE;
  // loop recursive over input files
  Config_getBool(RECURSIVE)=TRUE;
  // set the input
  Config_getList(INPUT).clear();
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      char filename[1024];
      while (1) {
        scanf("%s[^\n]", filename);
        if (feof(stdin)) {
          break;
        }
        Config_getList(INPUT).append(filename);
      }
    } else {
      Config_getList(INPUT).append(argv[i]);
    }
  }
  if (Config_getList(INPUT).isEmpty()) {
    exit(0);
  }

  // parse the files
  parseInput();

  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  // for each file with a certain name
  for (fnli.toFirst();(fn=fnli.current());++fnli) {
    FileNameIterator fni(*fn);
    FileDef *fd;
    // for each file definition
    for (;(fd=fni.current());++fni) {
      // get the references (linked and unlinked) found in this file
      findXRefSymbols(fd);
    }
  }

  QDir thisDir;
  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty())    thisDir.remove(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty())  thisDir.remove(Doxygen::entryDBFileName);
  if (!Doxygen::filterDBFileName.isEmpty()) thisDir.remove(Doxygen::filterDBFileName);

  // clean up after us
  thisDir.rmdir(Config_getString(OUTPUT_DIRECTORY));

  listSymbols();

  std::string cleanup_command = "rm -rf ";
  cleanup_command += tmpdir.str();
  system(cleanup_command.c_str());

  exit(0);
}

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
#include <unistd.h>
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
    void setCurrentDoc(Definition *,const char *,bool) {}
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
      if(md->getFileDef()->getFileBase() == context->getFileDef()->getFileBase())
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

void printArgumentList(MemberDef* md) {
  ArgumentList *argList = md->argumentList();
  ArgumentListIterator iterator(*argList);

  printf("(");
  Argument * argument = iterator.toFirst();
  if(argument != NULL) {
    printf("%s", argument->type.data());
    for(++iterator; (argument = iterator.current()) ;++iterator){
      printf(",%s", argument->type.data());
    }
  }
  printf(")");
}

void printType(MemberDef* md) {
  printf("%s ", md->memberTypeName().data());
}

void printSignature(MemberDef* md) {
  printf("%s", md->name().data());
  if(md->isFunction()){
    printArgumentList(md);
  }
  printf(" ");
}

static void printWhereItWasDefined(MemberDef * md) {
  if (md->getClassDef()) {
    printf("defined in %s\n", md->getClassDef()->name().data());
  }
  else if (md->getFileDef()) {
    printf("defined in %s\n", md->getFileDef()->getFileBase().data());
  }
  else {
    printf("\n");
  }
}

static void printCStructMember(MemberDef * md) {
  printType(md);
  printf("%s::", md->getClassDef()->name().data());
  printSignature(md);
  printf("defined in %s\n", md->getClassDef()->getFileDef()->getFileBase().data());
}

static int isPartOfCStruct(MemberDef * md) {
  return is_c_code && md->getClassDef() != NULL;
}

static void printReferenceTo(MemberDef* md) {
  printf("      uses ");
  if (isPartOfCStruct(md)) {
    printCStructMember(md);
  }
  else {
    printType(md);
    printSignature(md);
    printWhereItWasDefined(md);
  }
}

static void printReferencesMembers(MemberDef *md) {
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        printReferenceTo(rmd);
      }
    }
  }
}

void printDefinitionLine(MemberDef* md) {
  printf("in line %d\n", md->getDefLine());
}

void printDefinition(MemberDef* md) {
  printf("   ");
  printType(md);
  printSignature(md);
  printDefinitionLine(md);
}

static void printProtection(MemberDef* md) {
  if (md->protection() == Public) {
    printf("      protection public\n");
  }
}

void printNumberOfLines(MemberDef* md) {
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printf("      %d lines of code\n", size);
}

void printNumberOfArguments(MemberDef* md) {
  ArgumentList *argList = md->argumentList();
  printf("      %d parameters\n", argList->count());
}

void printFunctionInformation(MemberDef* md) {
  printNumberOfLines(md);
  printNumberOfArguments(md);
  printReferencesMembers(md);
}

static void lookupSymbol(Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = (MemberDef *)d;
    printDefinition(md);
    printProtection(md);
    if (md->isFunction()) {
      printFunctionInformation(md);
    }
  }
}

void listMembers(MemberList *ml) {
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      lookupSymbol((Definition*) md);
    }
  }
}

static void printInheritance(ClassDef* cd) {
  BaseClassList* baseClasses = cd->baseClasses();
  if (baseClasses) {
    BaseClassListIterator bci(*baseClasses);
    BaseClassDef* bcd;
    for (bci.toFirst(); (bcd = bci.current()); ++bci) {
      printf("   inherits from %s\n", bcd->classDef->name().data());
    }
  }
}

void printCModule(ClassDef* cd) {
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printf("   variable %s::%s in line %d\n", cd->name().data(), md->name().data(), md->getDefLine());
      printProtection(md);
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

void printClassInformation(ClassDef* cd) {
  printf("module %s\n", cd->name().data());
  printInheritance(cd);
  if(cd->isAbstract()) {
    printf("   abstract class\n");
  }
  listAllMembers(cd);
}

static void printClass(ClassDef* cd) {
  if (is_c_code) {
    printCModule(cd);
  } else {
    printClassInformation(cd);
  }
}

static void printFile(FileDef* fd) {
  printf("file %s\n", fd->absFilePath().data());
  MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
  if (ml && ml->count() > 0) {
    printf("module %s\n", fd->getFileBase().data());
    listMembers(ml);
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
        checkLanguage(filename, ".java")
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
      printFile(fd);

      ClassSDict *classes = fd->getClassSDict();
      if (classes) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          printClass(cd);
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

  // setup the non-default configuration options

  // we need a place to put intermediate files
  std::ostringstream tmpdir;
  tmpdir << "/tmp/doxyparse-" << getpid();
  Config_getString("OUTPUT_DIRECTORY")= tmpdir.str().c_str();

  // enable HTML (fake) output to omit warning about missing output format
  Config_getBool("GENERATE_HTML")=TRUE;
  // disable latex output
  Config_getBool("GENERATE_LATEX")=FALSE;
  // be quiet
  Config_getBool("QUIET")=TRUE;
  // turn off warnings
  Config_getBool("WARNINGS")=FALSE;
  Config_getBool("WARN_IF_UNDOCUMENTED")=FALSE;
  Config_getBool("WARN_IF_DOC_ERROR")=FALSE;
  // Extract as much as possible
  Config_getBool("EXTRACT_ALL")=TRUE;
  Config_getBool("EXTRACT_STATIC")=TRUE;
  Config_getBool("EXTRACT_PRIVATE")=TRUE;
  Config_getBool("EXTRACT_LOCAL_METHODS")=TRUE;
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_getBool("SOURCE_BROWSER")=TRUE;
  // find functions call between modules
  Config_getBool("CALL_GRAPH")=TRUE;
  // loop recursive over input files
  Config_getBool("RECURSIVE")=TRUE;
  // set the input
  Config_getList("INPUT").clear();
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      char filename[1024];
      while (1) {
        scanf("%s[^\n]", filename);
        if (feof(stdin)) {
          break;
        }
        Config_getList("INPUT").append(filename);
      }
    } else {
      Config_getList("INPUT").append(argv[i]);
    }
  }
  if (Config_getList("INPUT").isEmpty()) {
    exit(0);
  }

  // check and finalize the configuration
  checkConfiguration();
  adjustConfiguration();

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

  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty()) unlink(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty()) unlink(Doxygen::entryDBFileName);
  // clean up after us
  rmdir(Config_getString("OUTPUT_DIRECTORY"));

  listSymbols();

  std::string cleanup_command = "rm -rf ";
  cleanup_command += tmpdir.str();
  system(cleanup_command.c_str());

  exit(0);
}

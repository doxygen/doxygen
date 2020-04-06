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
#include "version.h"
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
#include <qcstring.h>
#include <qregexp.h>
#include "namespacedef.h"
#include "portable.h"

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

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  CodeParserInterface &intf=Doxygen::parserManager->getCodeParser(fd->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(fd->name());

  // reset the parsers state
  intf.resetCodeParserState();

  // create a new backend object
  Doxyparse *parse = new Doxyparse(fd);

  // parse the source code
  intf.parseCode(*parse, 0, fileToString(fd->absFilePath()), lang, FALSE, 0, fd);

  // dismiss the object.
  delete parse;
}

static bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md) {
  if (md->isStatic()) {
    if(md->getFileDef() && context->getFileDef()) {
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

static void startYamlDocument() {
  printf("---\n");
}
static void printFile(std::string file) {
  printf("%s:\n", file.c_str());
}
static void printModule(std::string module) {
  printf("  \"%s\":\n", unescapeCharsInString(module.c_str()).data());
}
static void printClassInformation(std::string information) {
  printf("    information: %s\n", information.c_str());
}
static void printInherits() {
  printf("    inherits:\n");
}
static void printInheritance(std::string base_class) {
  printf("      - %s\n", base_class.c_str());
}
static void printDefines() {
  printf("    defines:\n");
}
static void printDefinition(std::string type, std::string signature, int line) {
  printf("      - \"%s\":\n", signature.substr(0, 1022).c_str());
  printf("          type: %s\n", type.c_str());
  printf("          line: %d\n", line);
}
static void printProtection(std::string protection) {
  printf("          protection: %s\n", protection.c_str());
}
static void printPrototypeYes() {
  printf("          prototype: yes\n");
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
  printf("            - \"%s\":\n", signature.substr(0, 1022).c_str());
  printf("                type: %s\n", type.c_str());
  printf("                defined_in: \"%s\"\n", unescapeCharsInString(defined_in.c_str()).data());
}
static void printNumberOfConditionalPaths(MemberDef* md) {
  printf("          conditional_paths: %d\n", md->numberOfFlowKeyWords());
}

static int isPartOfCStruct(MemberDef * md) {
  return is_c_code && md->getClassDef() != NULL;
}

std::string sanitizeString(std::string data) {
  QCString new_data = QCString(data.c_str());
  new_data.replace(QRegExp("\""), "");
  new_data.replace(QRegExp("\\"), ""); // https://github.com/analizo/analizo/issues/138
  return !new_data.isEmpty() ? new_data.data() : "";
}

std::string argumentData(const Argument &argument) {
  std::string data = "";
  if (argument.type.size() > 1)
    data = sanitizeString(argument.type.data());
  else if (!argument.name.isEmpty())
    data = sanitizeString(argument.name.data());
  return data;
}

std::string functionSignature(MemberDef* md) {
  std::string signature = sanitizeString(md->name().data());
  if(md->isFunction()){
    const ArgumentList &argList = md->argumentList();
    signature += "(";
    auto it = argList.begin();
    if(it!=argList.end()) {
      signature += argumentData(*it);
      for(++it; it!=argList.end(); ++it) {
        signature += std::string(",") + argumentData(*it);
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
    else if (md->getNamespaceDef()) {
      defined_in = md->getNamespaceDef()->name().data();
    }
  }
  printReferenceTo(type, signature, defined_in);
}

void protectionInformation(Protection protection) {
  if (protection == Public) {
    printProtection("public");
  }
  else if (protection == Protected) {
    printProtection("protected");
  }
  else if (protection == Private) {
    printProtection("private");
  }
  else if (protection == Package) {
    printProtection("package");
  }
}

void cModule(ClassDef* cd) {
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    FileDef *fd = cd->getFileDef();
    MemberList *fd_ml = fd->getMemberList(MemberListType_allMembersList);
    if (!fd_ml || fd_ml->count() == 0) {
      printModule(fd->getOutputFileBase().data());
      printDefines();
    }
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printDefinition("variable", cd->name().data() + std::string("::") + md->name().data(), md->getDefLine());
      protectionInformation(md->protection());
    }
  }
}

static bool checkOverrideArg(const ArgumentList &argList, MemberDef *md) {
  if(!md->isFunction() || argList.empty()){
    return false;
  }

  for (const Argument &argument : argList) {
    if(md->name() == argument.name) {
      return true;
    }
  }

  return false;
}

void functionInformation(MemberDef* md) {
  std::string temp = "";
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printNumberOfLines(size);
  const ArgumentList &argList = md->argumentList();
  if (!argList.empty())
  {
    temp = argumentData(argList.front());
// TODO: This is a workaround; better not include "void" in argList, in the first place.
    if (temp!="void")
    {
      printNumberOfArguments(argList.size());
    }
  }

  printNumberOfConditionalPaths(md);
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    printUses();
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd) && !checkOverrideArg(argList, rmd)) {
        referenceTo(rmd);
      }
    }
  }
}

void prototypeInformation(MemberDef* md) {
  printPrototypeYes();
  const ArgumentList &argList = md->argumentList();
  printNumberOfArguments(argList.size());
}

static void lookupSymbol(Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = dynamic_cast<MemberDef*>(d);
    std::string type = md->memberTypeName().data();
    std::string signature = functionSignature(md);
    printDefinition(type, signature, md->getDefLine());
    protectionInformation(md->protection());
    if (md->isFunction() && md->isPrototype()) {
      prototypeInformation(md);
    }
    else if (md->isFunction()) {
      functionInformation(md);
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
      printInherits();
      BaseClassListIterator bci(*baseClasses);
      BaseClassDef* bcd;
      for (bci.toFirst(); (bcd = bci.current()); ++bci) {
        printInheritance(bcd->classDef->name().data());
      }
    }
    if(cd->isAbstract()) {
      printClassInformation("abstract class");
    }
    printDefines();
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
static void detectProgrammingLanguage(FileNameLinkedMap &fnli) {
  for (const auto &fn : fnli) {
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
  detectProgrammingLanguage(*Doxygen::inputNameLinkedMap);

  // iterate over the input files
  for (const auto &fn : *Doxygen::inputNameLinkedMap) {
    for (const auto &fd : *fn) {
      printFile(fd->absFilePath().data());
      MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
      if (ml && ml->count() > 0) {
        printModule(fd->getOutputFileBase().data());
        printDefines();
        listMembers(ml);
      }

      ClassSDict *classes = fd->getClassSDict();
      if (classes) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          if (!cd->isVisited()) {
            classInformation(cd);
            cd->setVisited(TRUE);
          }
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
  if (qstrcmp(&argv[1][2], "version") == 0) {
    QCString versionString = getDoxygenVersion();
    printf("%s\n", versionString.data());
    exit(0);
  }

  // initialize data structures
  initDoxygen();

  // check and finalize the configuration
  checkConfiguration();
  adjustConfiguration();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  std::ostringstream tmpdir;
  unsigned int pid = Portable::pid();
  if (Portable::getenv("TMP"))
    tmpdir << Portable::getenv("TMP") << "/doxyparse-" << pid;
  else if (Portable::getenv("TEMP"))
    tmpdir << Portable::getenv("TEMP") << "/doxyparse-" << pid;
  else
    tmpdir << "doxyparse-" << pid;

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
  Config_getBool(EXTRACT_PACKAGE)=TRUE;
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_getBool(SOURCE_BROWSER)=TRUE;
  // find functions call between modules
  Config_getBool(CALL_GRAPH)=TRUE;
  // loop recursive over input files
  Config_getBool(RECURSIVE)=TRUE;
  // add file extensions
  Config_getList(FILE_PATTERNS).append("*.cc");
  Config_getList(FILE_PATTERNS).append("*.cxx");
  Config_getList(FILE_PATTERNS).append("*.cpp");
  Config_getList(FILE_PATTERNS).append("*.java");
  Config_getList(FILE_PATTERNS).append("*.py");
  Config_getList(FILE_PATTERNS).append("*.pyw");
  Config_getList(FILE_PATTERNS).append("*.cs");
  Config_getList(FILE_PATTERNS).append("*.c");
  Config_getList(FILE_PATTERNS).append("*.h");
  Config_getList(FILE_PATTERNS).append("*.hh");
  Config_getList(FILE_PATTERNS).append("*.hpp");
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
  for (const auto &fn : *Doxygen::inputNameLinkedMap) {
    for (const auto &fd : *fn) {
      // get the references (linked and unlinked) found in this file
      findXRefSymbols(fd.get());
    }
  }

  QDir thisDir;
  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty())    thisDir.remove(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty())  thisDir.remove(Doxygen::entryDBFileName);
  if (!Doxygen::filterDBFileName.isEmpty()) thisDir.remove(Doxygen::filterDBFileName);

  // clean up after us
  thisDir.rmdir(Config_getString(OUTPUT_DIRECTORY));

  startYamlDocument();
  listSymbols();

  std::string cleanup_command = "rm -rf ";
  cleanup_command += tmpdir.str();
  system(cleanup_command.c_str());

  exit(0);
}

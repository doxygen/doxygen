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

#if !defined(_WIN32) || defined(__CYGWIN__)
  #include <unistd.h>
#else
  #include <windows.h>
#endif
#include "version.h"
#include "doxygen.h"
#include "outputgen.h"
#include "outputlist.h"
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
#include <algorithm>
#include <filesystem>
#include "qcstring.h"
#include "namespacedef.h"
#include "portable.h"
#include "dir.h"

class Doxyparse : public OutputCodeIntf
{
  public:
    Doxyparse(const FileDef *fd) : m_fd(fd) {}
   ~Doxyparse() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    OutputType type() const override { return OutputType::Extension; }
    std::unique_ptr<OutputCodeIntf> clone() override { return std::make_unique<Doxyparse>(m_fd); }
    void codify(const QCString &) override {}
    void stripCodeComments(bool) override {}
    void startSpecialComment() override {}
    void endSpecialComment() override {}
    void setStripIndentAmount(size_t) override {}
    void writeCodeLink(CodeSymbolType,const QCString &,const QCString &,const QCString &,const QCString &,const QCString &)  override {}
    void startCodeLine(int) override {}
    void endCodeLine() override {}
    void writeCodeAnchor(const QCString &) override {}
    void startFontClass(const QCString &) override {}
    void endFontClass() override {}
    void writeLineNumber(const QCString &,const QCString &,const QCString &,int,bool) override {}
    virtual void writeTooltip(const QCString &,const DocLinkInfo &,
                              const QCString &,const QCString &,const SourceLinkInfo &,
                              const SourceLinkInfo &) override {}
    void startCodeFragment(const QCString &) override {}
    void endCodeFragment(const QCString &) override {}
    void startFold(int,const QCString &,const QCString &) override {}
    void endFold() override {}

    void linkableSymbol(int l, const char *sym, Definition *symDef, Definition *context)
    {
      if (!symDef) {
        // in this case we have a local or external symbol

        // TODO record use of external symbols
        // TODO must have a way to differentiate external symbols from local variables
      }
    }

  private:
    const FileDef *m_fd;
};

static bool is_c_code = true;

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  auto intf=Doxygen::parserManager->getCodeParser(fd->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(fd->name());

  // reset the parsers state
  intf->resetCodeParserState();

  // create a new backend object
  std::unique_ptr<OutputCodeIntf> parse = std::make_unique<Doxyparse>(fd);
  OutputCodeList parseList;
  parseList.add(std::move(parse));

  // parse the source code
  intf->parseCode(parseList, QCString(), fileToString(fd->absFilePath()), lang,
                  FALSE, CodeParserOptions().setFileDef(fd));
}

static bool ignoreStaticExternalCall(const MemberDef *context, const MemberDef *md) {
  if (!md->isStatic()) return false;
  if (!md->getFileDef() || !context->getFileDef()) return false;

  // TODO ignore prefix of file
  return md->getFileDef()->getOutputFileBase() != context->getFileDef()->getOutputFileBase();
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
  printf("      - \"%s\"\n", base_class.c_str());
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
static void printNumberOfArguments(size_t arguments) {
  printf("          parameters: %zu\n", arguments);
}
static void printUses() {
  printf("          uses:\n");
}
static void printReferenceTo(std::string type, std::string signature, std::string defined_in) {
  printf("            - \"%s\":\n", signature.substr(0, 1022).c_str());
  printf("                type: %s\n", type.c_str());
  printf("                defined_in: \"%s\"\n", unescapeCharsInString(defined_in.c_str()).data());
}
static void printNumberOfConditionalPaths(const MemberDef* md) {
  printf("          conditional_paths: %d\n", md->numberOfFlowKeyWords());
}

static int isPartOfCStruct(const MemberDef * md) {
  return is_c_code && md->getClassDef() != nullptr;
}

[[nodiscard]] std::string sanitizeString(std::string data)
{
  data.erase(std::remove_if(data.begin(), data.end(), [](unsigned char c) {
               return c == '"' || c == '\'';
             }),
             data.end());
  return data;
}

std::string argumentData(const Argument &argument) {
  std::string data = "";
  if (argument.type.size() > 1)
    data = sanitizeString(argument.type.data());
  else if (!argument.name.isEmpty())
    data = sanitizeString(argument.name.data());
  return data;
}

std::string functionSignature(const MemberDef* md) {
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

static void referenceTo(const MemberDef* md) {
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
  if (protection == Protection::Public) {
    printProtection("public");
  }
  else if (protection == Protection::Protected) {
    printProtection("protected");
  }
  else if (protection == Protection::Private) {
    printProtection("private");
  }
  else if (protection == Protection::Package) {
    printProtection("package");
  }
}

void cModule(const ClassDef* cd) {
  const MemberList* ml = cd->getMemberList(MemberListType::VariableMembers());
  if (ml) {
    const FileDef *fd = cd->getFileDef();
    const MemberList *fd_ml = fd->getMemberList(MemberListType::AllMembersList());
    if (!fd_ml || fd_ml->size() == 0) {
      printModule(fd->getOutputFileBase().data());
      printDefines();
    }
    for (const auto &md : *ml) {
      printDefinition("variable", cd->name().data() + std::string("::") + md->name().data(), md->getDefLine());
      protectionInformation(md->protection());
    }
  }
}

static bool checkOverrideArg(const ArgumentList &argList, const MemberDef *md) {
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

void functionInformation(const MemberDef* md) {
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
  auto refList = md->getReferencesMembers();
  if (!refList.empty()) {
    printUses();
    for (const auto &rmd : refList) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd) && !checkOverrideArg(argList, rmd)) {
        referenceTo(rmd);
      }
    }
  }
}

void prototypeInformation(const MemberDef* md) {
  printPrototypeYes();
  const ArgumentList &argList = md->argumentList();
  printNumberOfArguments(argList.size());
}

static void lookupSymbol(const Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    const MemberDef *md = dynamic_cast<const MemberDef*>(d);
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

void listMembers(const MemberList *ml) {
  if (ml) {
    for (const auto &md : *ml) {
      lookupSymbol((Definition*) md);
    }
  }
}

void listAllMembers(const ClassDef* cd) {
  // methods
  listMembers(cd->getMemberList(MemberListType::FunctionMembers()));
  // constructors
  listMembers(cd->getMemberList(MemberListType::Constructors()));
  // attributes
  listMembers(cd->getMemberList(MemberListType::VariableMembers()));
}

static void classInformation(const ClassDef* cd) {
  if (is_c_code) {
    cModule(cd);
  } else {
    printModule(cd->name().data());
    if (!cd->baseClasses().empty()) {
      printInherits();
      for (const auto &bcd : cd->baseClasses()) {
        printInheritance(sanitizeString(bcd.classDef->name().data()));
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
    std::string filename = fn->fileName().str();
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
      MemberList *ml = fd->getMemberList(MemberListType::AllMembersList());
      if (ml && ml->size() > 0) {
        printModule(fd->getOutputFileBase().data());
        printDefines();
        listMembers(ml);
      }

      ClassDefSet visitedClasses;
      for (const auto &cd : fd->getClasses()) {
        if (visitedClasses.find(cd)==visitedClasses.end()) {
          classInformation(cd);
          visitedClasses.insert(cd);
        }
      }
    }
  }
  // TODO print external symbols referenced
}

int main(int argc,char **argv) {
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
      printf("%s version: %s\n",argv[0],getFullVersion().c_str());
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

  // check and finalize the configuration
  checkConfiguration();
  adjustConfiguration();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  std::ostringstream tmpdir;
  unsigned int pid = Portable::pid();
  if (!Portable::getenv("TMP").isEmpty())
    tmpdir << Portable::getenv("TMP") << "/doxyparse-" << pid;
  else if (!Portable::getenv("TEMP").isEmpty())
    tmpdir << Portable::getenv("TEMP") << "/doxyparse-" << pid;
  else
    tmpdir << "doxyparse-" << pid;

  Config_updateString(OUTPUT_DIRECTORY,tmpdir.str().c_str());
  // enable HTML (fake) output to omit warning about missing output format
  Config_updateBool(GENERATE_HTML,TRUE);
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
  Config_updateBool(EXTRACT_PACKAGE,TRUE);
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_updateBool(SOURCE_BROWSER,TRUE);
  // find functions call between modules
  Config_updateBool(CALL_GRAPH,TRUE);
  // loop recursive over input files
  Config_updateBool(RECURSIVE,TRUE);
  // add file extensions
  Config_updateList(FILE_PATTERNS, { "*.cc", "*.cxx", "*.cpp", "*.java",
                                     "*.py", "*.pyw", "*.cs", "*.c", "*.h", "*.hh", "*.hpp"});
  // set the input
  StringVector inputList;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      char filename[1024];
      while (1) {
        (void)scanf("%s[^\n]", filename);
        if (feof(stdin)) {
          break;
        }
        inputList.push_back(filename);
      }
    } else {
      inputList.push_back(argv[i]);
    }
  }
  Config_updateList(INPUT,inputList);
  if (inputList.empty()) {
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

  Dir thisDir;
  // remove temporary files
  if (!Doxygen::filterDBFileName.isEmpty()) thisDir.remove(Doxygen::filterDBFileName.str());

  // clean up after us
  thisDir.rmdir(Config_getString(OUTPUT_DIRECTORY).str());

  startYamlDocument();
  listSymbols();

  std::error_code ec;
  std::filesystem::remove_all(tmpdir.str(), ec);
  return 0;
}

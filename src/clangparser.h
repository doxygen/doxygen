#ifndef CLANGPARSER_H
#define CLANGPARSER_H

#include <qcstring.h>
#include <qstrlist.h>
#include "containers.h"
#include <memory>

class CodeOutputInterface;
class FileDef;
class ClangParser;
class Definition;

namespace clang { namespace tooling {
  class CompilationDatabase;
} }

/** @brief Clang parser object for a single translation unit, which consists of a source file
 *  and the directly or indirectly included headers
 */
class ClangTUParser
{
  public:
    ClangTUParser(const ClangParser &parser,const FileDef *fd);
    virtual ~ClangTUParser();

    /** Parse the file given at construction time as a translation unit
     *  This file should already be preprocessed by doxygen preprocessor at the time of calling.
     */
    void parse();

    /** Switches to another file within the translation unit started with start().
     *  @param[in] fileName The name of the file to switch to.
     */
    void switchToFile(FileDef *fd);

    /** Returns the list of files for this translation unit */
    StringVector filesInSameTU() const;

    /** Looks for \a symbol which should be found at \a line.
     *  returns a clang unique reference to the symbol.
     */
    QCString lookup(uint line,const char *symbol);

    /** writes the syntax highlighted source code for a file
     *  @param[out] ol The output generator list to write to.
     *  @param[in]  fd The file to write sources for.
     */
    void writeSources(CodeOutputInterface &ol,FileDef *fd);

  private:
    void detectFunctionBody(const char *s);
    void writeLineNumber(CodeOutputInterface &ol,FileDef *fd,uint line);
    void codifyLines(CodeOutputInterface &ol,FileDef *fd,const char *text,
                     uint &line,uint &column,const char *fontClass=0);
    void writeMultiLineCodeLink(CodeOutputInterface &ol,
                                FileDef *fd,uint &line,uint &column,
                                Definition *d, const char *text);
    void linkIdentifier(CodeOutputInterface &ol,FileDef *fd,
                        uint &line,uint &column,
                        const char *text,int tokenIndex);
    void linkMacro(CodeOutputInterface &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    void linkInclude(CodeOutputInterface &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    ClangTUParser(const ClangTUParser &) = delete;
    ClangTUParser &operator=(const ClangTUParser &) = delete;
    class Private;
    std::unique_ptr<Private> p;
};

/** @brief Wrapper for to let libclang assisted parsing. */
class ClangParser
{
    friend class ClangTUParser;
  public:
    /** Returns the one and only instance of the class */
    static ClangParser *instance();
    std::unique_ptr<ClangTUParser> createTUParser(const FileDef *fd) const;

  private:
    const clang::tooling::CompilationDatabase *database() const;
    class Private;
    std::unique_ptr<Private> p;
    ClangParser();
    virtual ~ClangParser();
    static ClangParser *s_instance;
};

#endif

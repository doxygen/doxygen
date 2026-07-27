#ifndef CLANGPARSER_H
#define CLANGPARSER_H

#include <memory>
#include <string>
#include <cstdint>

#include "containers.h"
#include "types.h"
#include "construct.h"

class OutputCodeList;
class FileDef;
class ClangParser;
class Definition;

namespace clang::tooling {
  class CompilationDatabase;
}

/** @brief Clang parser object for a single translation unit, which consists of a source file
 *  and the directly or indirectly included headers
 */
class ClangTUParser
{
  public:
    ClangTUParser(const ClangParser &parser,const FileDef *fd);
    NON_COPYABLE(ClangTUParser)
    virtual ~ClangTUParser();

    /** Parse the file given at construction time as a translation unit
     *  This file should already be preprocessed by doxygen preprocessor at the time of calling.
     */
    void parse();

    /** Switches to another file within the translation unit started with start().
     *  @param[in] fd The file definition with the name of the file to switch to.
     */
    void switchToFile(const FileDef *fd);

    /** Returns the list of files for this translation unit */
    StringVector filesInSameTU() const;

    /** Looks for \a symbol which should be found at \a line.
     *  returns a clang unique reference to the symbol.
     */
    std::string lookup(uint32_t line,const char *symbol);

    /** writes the syntax highlighted source code for a file
     *  @param[out] ol The output generator list to write to.
     *  @param[in]  fd The file to write sources for.
     */
    void writeSources(OutputCodeList &ol,const FileDef *fd);

  private:
    void detectFunctionBody(const char *s);
    void writeLineNumber(OutputCodeList &ol,const FileDef *fd,uint32_t line,bool writeLineAnchor);
    void codifyLines(OutputCodeList &ol,const FileDef *fd,const char *text,
                     uint32_t &line,uint32_t &column,const char *fontClass=nullptr);
    void writeMultiLineCodeLink(OutputCodeList &ol,
                                const FileDef *fd,uint32_t &line,uint32_t &column,
                                const Definition *d, const char *text);
    void linkIdentifier(OutputCodeList &ol,const FileDef *fd,
                        uint32_t &line,uint32_t &column,
                        const char *text,int tokenIndex);
    void linkMacro(OutputCodeList &ol,const FileDef *fd,
                   uint32_t &line,uint32_t &column,
                   const char *text);
    void linkInclude(OutputCodeList &ol,const FileDef *fd,
                   uint32_t &line,uint32_t &column,
                   const char *text);
    void codeFolding(OutputCodeList &ol,const Definition *d,uint32_t line);
    void endCodeFold(OutputCodeList &ol,uint32_t line);
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
    NON_COPYABLE(ClangParser)
    virtual ~ClangParser();
    static ClangParser *s_instance;
};

#endif

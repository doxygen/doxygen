#ifndef CLANGPARSER_H
#define CLANGPARSER_H

#include <qcstring.h>
#include <qstrlist.h>

class CodeOutputInterface;
class FileDef;

/** @brief Wrapper for to let libclang assisted parsing. */
class ClangParser
{
  public:
    /** Returns the one and only instance of the class */
    static ClangParser *instance();
    
    /** Start parsing a file.
     *  @param[in] fileName The name of the file to parse.
     *  @param[in,out] filesInTanslationUnit Other files that are
     *                 part of the input and included by the file.
     *                 The function will return a subset of the files,
     *                 only including the onces that were actually found 
     *                 during parsing.
     */
    void start(const char *fileName,QStrList &filesInTranslationUnit);

    /** Switches to another file within the translation unit started
     *  with start().
     *  @param[in] The name of the file to switch to.
     */
    void switchToFile(const char *fileName);

    /** Finishes parsing a translation unit. Free any resources that
     *  were needed for parsing.
     */
    void finish();

    /** Looks for \a symbol which should be found at \a line and
     *  returns a clang unique reference to the symbol.
     */
    QCString lookup(uint line,const char *symbol);

    /** writes the syntax highlighted source code for a file
     *  @param[out] ol The output generator list to write to.
     *  @param[in]  fd The file to write sources for.
     */
    void writeSources(CodeOutputInterface &ol,FileDef *fd);

  private:
    void linkIdentifier(CodeOutputInterface &ol,FileDef *fd,
                        uint &line,uint &column,
                        const char *text,int tokenIndex);
    void linkMacro(CodeOutputInterface &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    void linkInclude(CodeOutputInterface &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    void determineInputFilesInSameTu(QStrList &filesInTranslationUnit);
    class Private;
    Private *p;
    ClangParser();
    virtual ~ClangParser();
    static ClangParser *s_instance;
};

#endif

/******************************************************************************
 *
 *
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

#ifndef PARSERINTF_H
#define PARSERINTF_H

#include <qstrlist.h>

#include <functional>
#include <memory>
#include <map>
#include <string>

#include "types.h"

class Entry;
class FileDef;
class CodeOutputInterface;
class MemberDef;
class Definition;

/** \brief Abstract interface for outline parsers.
 *
 *  By implementing the methods of this interface one can add
 *  a new language parser to doxygen. The parser implementation can make use of the
 *  comment block parser to parse the contents of special comment blocks.
 */
class OutlineParserInterface
{
  public:
    virtual ~OutlineParserInterface() {}

    /** Starts processing a translation unit (source files + headers).
     *  After this call parseInput() is called with sameTranslationUnit
     *  set to FALSE. If parseInput() returns additional include files,
     *  these are also processed using parseInput() with
     *  sameTranslationUnit set to TRUE. After that
     *  finishTranslationUnit() is called.
     */
    virtual void startTranslationUnit(const char *fileName) = 0;

    /** Called after all files in a translation unit have been
     *  processed.
     */
    virtual void finishTranslationUnit() = 0;

    /** Parses a single input file with the goal to build an Entry tree.
     *  @param[in] fileName    The full name of the file.
     *  @param[in] fileBuf     The contents of the file (zero terminated).
     *  @param[in,out] root    The root of the tree of Entry *nodes
     *             representing the information extracted from the file.
     *  @param[in] sameTranslationUnit TRUE if this file was found in the same
     *             translation unit (in the filesInSameTranslationUnit list
     *             returned for another file).
     *  @param[in,out] filesInSameTranslationUnit other files expected to be
     *              found in the same translation unit (used for libclang)
     */
    virtual void parseInput(const char *fileName,
                            const char *fileBuf,
                            const std::shared_ptr<Entry> &root,
                            bool sameTranslationUnit,
                            QStrList &filesInSameTranslationUnit) = 0;

    /** Returns TRUE if the language identified by \a extension needs
     *  the C preprocessor to be run before feed the result to the input
     *  parser.
     *  @see parseInput()
     */
    virtual bool needsPreprocessing(const QCString &extension) const = 0;

    /** Callback function called by the comment block scanner.
     *  It provides a string \a text containing the prototype of a function
     *  or variable. The parser should parse this and store the information
     *  in the Entry node that corresponds with the node for which the
     *  comment block parser was invoked.
     */
    virtual void parsePrototype(const char *text) = 0;

};

/** \brief Abstract interface for code parsers.
 *
 *  By implementing the methods of this interface one can add
 *  a new language parser to doxygen. This interface is used for
 *  syntax highlighting, but also to extract cross references and call graphs.
 */
class CodeParserInterface
{
  public:
    virtual ~CodeParserInterface() {}

    /** Parses a source file or fragment with the goal to produce
     *  highlighted and cross-referenced output.
     *  @param[in] codeOutIntf Abstract interface for writing the result.
     *  @param[in] lang The programming language of the code fragment.
     *  @param[in] scopeName Name of scope to which the code belongs.
     *  @param[in] input Actual code in the form of a string
     *  @param[in] isExampleBlock TRUE iff the code is part of an example.
     *  @param[in] exampleName Name of the example.
     *  @param[in] fileDef File definition to which the code
     *             is associated.
     *  @param[in] startLine Starting line in case of a code fragment.
     *  @param[in] endLine Ending line of the code fragment.
     *  @param[in] inlineFragment Code fragment that is to be shown inline
     *             as part of the documentation.
     *  @param[in] memberDef Member definition to which the code
     *             is associated (non null in case of an inline fragment
     *             for a member).
     *  @param[in] showLineNumbers if set to TRUE and also fileDef is not 0,
     *             line numbers will be added to the source fragment
     *  @param[in] searchCtx context under which search data has to be stored.
     *  @param[in] collectXRefs collect cross-reference relations.
     */
    virtual void parseCode(CodeOutputInterface &codeOutIntf,
                           const char *scopeName,
                           const QCString &input,
                           SrcLangExt lang,
                           bool isExampleBlock,
                           const char *exampleName=0,
                           FileDef *fileDef=0,
                           int startLine=-1,
                           int endLine=-1,
                           bool inlineFragment=FALSE,
                           const MemberDef *memberDef=0,
                           bool showLineNumbers=TRUE,
                           const Definition *searchCtx=0,
                           bool collectXRefs=TRUE
                          ) = 0;

    /** Resets the state of the code parser.
     *  Since multiple code fragments can together form a single example, an
     *  explicit function is used to reset the code parser state.
     *  @see parseCode()
     */
    virtual void resetCodeParserState() = 0;

};

//-----------------------------------------------------------------------------

using OutlineParserFactory = std::function<std::unique_ptr<OutlineParserInterface>()>;

/** \brief Manages programming language parsers.
 *
 *  This class manages the language parsers in the system. One can
 *  register parsers, and obtain a parser given a file extension.
 */
class ParserManager
{
  public:

    struct ParserPair
    {
      ParserPair(OutlineParserFactory opf, std::unique_ptr<CodeParserInterface> cpi)
        : outlineParserFactory(opf), codeParserInterface(std::move(cpi))
      {
      }

      OutlineParserFactory outlineParserFactory;
      std::unique_ptr<CodeParserInterface> codeParserInterface;
    };

    ParserManager(OutlineParserFactory outlineParserFactory,
                  std::unique_ptr<CodeParserInterface> codeParserInterface)
      : m_defaultParsers(outlineParserFactory,std::move(codeParserInterface))
    {
    }

    /** Registers an additional parser.
     *  @param[in] name          A symbolic name of the parser, i.e. "c",
     *                           "python", "fortran", "vhdl", ...
     *  @param[in] outlineParser The language parser (scanner) that is to be used for the
     *                           given name.
     *  @param[in] codeParser    The code parser that is to be used for the
     *                           given name.
     */
    void registerParser(const char *name,OutlineParserFactory outlineParserFactory,
                                         std::unique_ptr<CodeParserInterface> codeParserInterface)
    {
      m_parsers.emplace(std::string(name),
                        ParserPair(outlineParserFactory,std::move(codeParserInterface)));
    }

    /** Registers a file \a extension with a parser with name \a parserName.
     *  Returns TRUE if the extension was successfully registered.
     */
    bool registerExtension(const char *extension, const char *parserName)
    {
      if (parserName==0 || extension==0) return FALSE;

      const auto &parserIt = m_parsers.find(parserName);
      if (parserIt == m_parsers.end()) return FALSE;

      auto extensionIt = m_extensions.find(extension);
      if (extensionIt != m_extensions.end()) // extension already exists
      {
        m_extensions.erase(extensionIt); // remove it (e.g. user specified extension overrules built in one)
      }
      m_extensions.emplace(std::string(extension),parserIt->second); // add new mapping
      return TRUE;
    }

    /** Gets the interface to the parser associated with given \a extension.
     *  If there is no parser explicitly registered for the supplied extension,
     *  the interface to the default parser will be returned.
     */
    std::unique_ptr<OutlineParserInterface> getOutlineParser(const char *extension)
    {
      return getParsers(extension).outlineParserFactory();
    }

    /** Gets the interface to the parser associated with given \a extension.
     *  If there is no parser explicitly registered for the supplied extension,
     *  the interface to the default parser will be returned.
     */
    CodeParserInterface &getCodeParser(const char *extension)
    {
      return *getParsers(extension).codeParserInterface;
    }

  private:
    ParserPair &getParsers(const char *extension)
    {
      QCString ext = QCString(extension).lower().data();
      if (ext.isEmpty()) ext=".no_extension";
      auto it = m_extensions.find(ext.data());
      if (it==m_extensions.end() && ext.length()>4)
      {
        it = m_extensions.find(ext.left(4).data());
      }
      return it!=m_extensions.end() ? it->second : m_defaultParsers;
    }

    std::map<std::string,ParserPair> m_parsers;
    std::map<std::string,ParserPair &> m_extensions;
    ParserPair m_defaultParsers;
};

#endif

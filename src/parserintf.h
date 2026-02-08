/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include <functional>
#include <memory>
#include <map>
#include <string>

#include "types.h"
#include "containers.h"
#include "construct.h"

class Entry;
class FileDef;
class OutputCodeList;
class MemberDef;
class Definition;
class ClangTUParser;

/** \brief Abstract interface for outline parsers.
 *
 *  By implementing the methods of this interface one can add
 *  a new language parser to doxygen. The parser implementation can make use of the
 *  comment block parser to parse the contents of special comment blocks.
 */
class OutlineParserInterface
{
  public:
    ABSTRACT_BASE_CLASS(OutlineParserInterface)

    /** Parses a single input file with the goal to build an Entry tree.
     *  @param[in] fileName    The full name of the file.
     *  @param[in] fileBuf     The contents of the file (zero terminated).
     *  @param[in,out] root    The root of the tree of Entry *nodes
     *             representing the information extracted from the file.
     *  @param[in] clangParser The clang translation unit parser object
     *                         or nullptr if disabled.
     */
    virtual void parseInput(const QCString &fileName,
                            const char *fileBuf,
                            const std::shared_ptr<Entry> &root,
                            ClangTUParser *clangParser) = 0;

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
    virtual void parsePrototype(const QCString &text) = 0;

};

/** Options to configure the code parser */
struct CodeParserOptions
{
  public:
    // === getters for optional params
    bool isExample()              const { return m_isExample;       }
    QCString exampleName()        const { return m_exampleName;     }
    const FileDef * fileDef()     const { return m_fileDef;         }
    int startLine()               const { return m_startLine;       }
    int endLine()                 const { return m_endLine;         }
    bool inlineFragment()         const { return m_inlineFragment;  }
    const MemberDef * memberDef() const { return m_memberDef;       }
    bool showLineNumbers()        const { return m_showLineNumbers; }
    const Definition *searchCtx() const { return m_searchCtx;       }
    bool collectXRefs()           const { return m_collectXRefs;    }

    // === setters for optional params

    /// Associate this comment block with a given example
    CodeParserOptions &setExample(bool isExample,const QCString &name)
    { m_isExample=isExample; m_exampleName = name; return *this; }

    CodeParserOptions &setFileDef(const FileDef *fd)
    { m_fileDef = fd; return *this; }

    CodeParserOptions &setStartLine(int lineNr)
    { m_startLine = lineNr; return *this; }

    CodeParserOptions &setEndLine(int lineNr)
    { m_endLine = lineNr; return *this; }

    CodeParserOptions &setInlineFragment(bool enable)
    { m_inlineFragment = enable; return *this; }

    CodeParserOptions &setMemberDef(const MemberDef * md)
    { m_memberDef = md; return *this; }

    CodeParserOptions &setShowLineNumbers(bool enable)
    { m_showLineNumbers = enable; return *this; }

    CodeParserOptions &setSearchCtx(const Definition *d)
    { m_searchCtx = d; return *this; }

    CodeParserOptions &setCollectXRefs(bool enable)
    { m_collectXRefs = enable; return *this; }

  private:
    bool              m_isExample       = false;
    QCString          m_exampleName;
    const FileDef *   m_fileDef         = nullptr;
    int               m_startLine       = -1;
    int               m_endLine         = -1;
    bool              m_inlineFragment  = false;
    const MemberDef * m_memberDef       = nullptr;
    bool              m_showLineNumbers = true;
    const Definition *m_searchCtx       = nullptr;
    bool              m_collectXRefs    = true;
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
    ABSTRACT_BASE_CLASS(CodeParserInterface)

    /** Parses a source file or fragment with the goal to produce
     *  highlighted and cross-referenced output.
     *  @param[in] codeOutList interface for writing the result.
     *  @param[in] scopeName Name of scope to which the code belongs.
     *  @param[in] input Actual code in the form of a string
     *  @param[in] lang The programming language of the code fragment.
     *  @param[in] stripCodeComments signals whether or not for the code block the doxygen comments should be stripped.
     *  @param[in] options Additional options to configure the parser.
     */
    virtual void parseCode(OutputCodeList &codeOutList,
                           const QCString &scopeName,
                           const QCString &input,
                           SrcLangExt lang,
                           bool stripCodeComments,
                           const CodeParserOptions &options
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
using CodeParserFactory    = std::function<std::unique_ptr<CodeParserInterface>()>;

/** \brief Manages programming language parsers.
 *
 *  This class manages the language parsers in the system. One can
 *  register parsers, and obtain a parser given a file extension.
 */
class ParserManager
{

    struct ParserPair
    {
      ParserPair(OutlineParserFactory opf, const CodeParserFactory &cpf, const QCString &pn)
        : outlineParserFactory(opf), codeParserFactory(cpf), parserName(pn)
      {
      }

      OutlineParserFactory outlineParserFactory;
      CodeParserFactory    codeParserFactory;
      QCString parserName;
    };

  public:
    /** Create the parser manager
     *  @param outlineParserFactory the fallback outline parser factory to use for unknown extensions
     *  @param codeParserFactory    the fallback code parser factory to use for unknown extensions
     */
    ParserManager(const OutlineParserFactory &outlineParserFactory,
                  const CodeParserFactory    &codeParserFactory)
      : m_defaultParsers(outlineParserFactory,codeParserFactory, QCString())
    {
    }

    /** Registers an additional parser.
     *  @param[in] name          A symbolic name of the parser, i.e. "c",
     *                           "python", "fortran", "vhdl", ...
     *  @param[in] outlineParserFactory A factory method to create a language parser (scanner) that
     *                           is to be used for the given name.
     *  @param[in] codeParserFactory    A factory method to create a code parser that is to be used
     *                           for the given name.
     */
    void registerParser(const QCString &name,const OutlineParserFactory &outlineParserFactory,
                                             const CodeParserFactory    &codeParserFactory)
    {
      m_parsers.emplace(name.str(),ParserPair(outlineParserFactory,codeParserFactory,name));
    }

    /** Registers a file \a extension with a parser with name \a parserName.
     *  Returns TRUE if the extension was successfully registered.
     */
    bool registerExtension(const QCString &extension, const QCString &parserName)
    {
      if (parserName.isEmpty() || extension.isEmpty()) return FALSE;

      const auto &parserIt = m_parsers.find(parserName.str());
      if (parserIt == m_parsers.end()) return FALSE;

      auto extensionIt = m_extensions.find(extension.str());
      if (extensionIt != m_extensions.end()) // extension already exists
      {
        m_extensions.erase(extensionIt); // remove it (e.g. user specified extension overrules built in one)
      }
      m_extensions.emplace(extension.str(),parserIt->second); // add new mapping
      return TRUE;
    }

    /** Gets the interface to the parser associated with a given \a extension.
     *  If there is no parser explicitly registered for the supplied extension,
     *  the interface to the default parser will be returned.
     */
    std::unique_ptr<OutlineParserInterface> getOutlineParser(const QCString &extension)
    {
      return getParsers(extension).outlineParserFactory();
    }

    /** Gets the interface to the parser associated with a given \a extension.
     *  If there is no parser explicitly registered for the supplied extension,
     *  the interface to the default parser will be returned.
     */
    std::unique_ptr<CodeParserInterface> getCodeParser(const QCString &extension)
    {
      auto factory = getCodeParserFactory(extension);
      return factory();
    }

    /** Get the factory for create code parser objects with a given \a extension. */
    CodeParserFactory &getCodeParserFactory(const QCString &extension)
    {
      return getParsers(extension).codeParserFactory;
    }

    /** Gets the name of the parser associated with given \a extension.
     *  If there is no parser explicitly registered for the supplied extension,
     *  the empty string will be returned.
     */
    QCString getParserName(const QCString &extension)
    {
      return getParsers(extension).parserName;
    }

  private:
    ParserPair &getParsers(const QCString &extension)
    {
      QCString ext = extension.lower();
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

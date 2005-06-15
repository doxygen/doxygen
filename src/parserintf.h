/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#include <qdict.h>

class Entry;
class FileDef;
class CodeOutputInterface;
class MemberDef;

/** \brief Abstract interface for programming language parsers.
 *
 *  By implementing the methods of this interface one can add
 *  a new language parser to doxygen. The parser can make use of the 
 *  comment block parser to parse the contents of special comment blocks.
 */
class ParserInterface
{
  public:
    /** Parses a single input file with the goal to build an Entry tree. 
     *  @param[in] fileName    The full name of the file.
     *  @param[in] fileBuf     The contents of the file (zero terminated).
     *  @param[in,out] root    The root of the tree of Entry *nodes 
     *             representing the information extracted from the file.
     */
    virtual void parseInput(const char *fileName,
                            const char *fileBuf,
                            Entry *root) = 0;

    /** Returns TRUE if the language identified by \a extension needs
     *  the C preprocessor to be run before feed the result to the input
     *  parser.
     *  @see parseInput()
     */
    virtual bool needsPreprocessing(const QCString &extension) = 0;
    
    /** Parses a source file or fragment with the goal to produce
     *  highlighted and cross-referenced output.
     *  @param[in] codeOutIntf Abstract interface for writing the result.
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
     */
    virtual void parseCode(CodeOutputInterface &codeOutIntf,
                           const char *scopeName,
                           const QCString &input,
                           bool isExampleBlock,
                           const char *exampleName=0,
                           FileDef *fileDef=0,
                           int startLine=-1,
                           int endLine=-1,
                           bool inlineFragment=FALSE,
                           MemberDef *memberDef=0
                          ) = 0;

    /** Resets the state of the code parser.
     *  Since multiple code fragments can together form a single example, an
     *  explicit function is used to reset the code parser state.
     *  @see parseCode()
     */
    virtual void resetCodeParserState() = 0;

    /** Callback function called by the comment block scanner.
     *  It provides a string \a text containing the prototype of a function
     *  or variable. The parser should parse this and store the information
     *  in the Entry node that corresponds with the node for which the
     *  comment block parser was invoked.
     */
    virtual void parsePrototype(const char *text) = 0;

    /** Callback function called by the comment block scanner upon encountering
     *  a group block start command (@@{). If the group has a header 
     *  specified via the @@name command, this will be passed via 
     *  the \a header parameter, if not the \a header parameter will be 0.
     */
    virtual void handleGroupStartCommand(const char *header) = 0;

    /** Callback function called by the comment block scanner upon encountering
     *  a group block end command (@@}). 
     */
    virtual void handleGroupEndCommand() = 0;
};

//-----------------------------------------------------------------------------

/** \brief Manages programming language parsers.
 *
 *  This class manages the language parsers in the system. One can 
 *  register parsers, and obtain a parser given a file extension.
 */
class ParserManager
{
  public:
    /** Creates the parser manager object. 
     *  @param defaultParser The default parser that is used when
     *                       no explicit extension has been registered for
     *                       a given input file.
     */
    ParserManager(ParserInterface *defaultParser)
      : m_defaultParser(defaultParser) {}

    /** Registers an additional parser.
     *  @param[in] extension The file extension that will trigger
     *             the use of this parser (e.g. ".py", or ".bas").
     *  @param[in] parser    The parser that is to be used for the
     *             given extension.
     */
    void registerParser(const char *extension,ParserInterface *parser)
    {
      m_parsers.insert(extension,parser);
    }

    /** Gets the interface to the parser associated with given \a extension.
     *  If there is no parser explicitly registered for the supplied extension, 
     *  the interface to the default parser will be returned.
     */
    ParserInterface *getParser(const char *extension)
    {
      if (extension==0) return m_defaultParser;
      ParserInterface *intf = m_parsers.find(extension);
      return intf ? intf : m_defaultParser;
    }

  private:
    QDict<ParserInterface> m_parsers;
    ParserInterface *m_defaultParser;
};

#endif

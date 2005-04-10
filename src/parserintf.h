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

/** \brief Abstract interface for programming language parsers.
 *
 *  By implementing the methods of this interface one can add
 *  a new language parser to doxygen. The parser can make use of the 
 *  comment block parser to parse the contents of special comment blocks.
 */
class ParserInterface
{
  public:
    /** Parses a single file. 
     *  @param[in] fileName    The full name of the file.
     *  @param[in] fileBuf     The contents of the file (zero terminated).
     *  @param[in,out] root    The root of the tree of Entry *nodes 
     *             representing the information extracted from the file.
     */
    virtual void parse(const char *fileName,const char *fileBuf,Entry *root) = 0;

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
     *                       no explicit extension has been register for
     *                       a given input file.
     */
    ParserManager(ParserInterface *defaultParser)
      : m_defaultParser(defaultParser) {}

    /** Registers a new parser.
     *  @param[in] extension The file extension that will trigger
     *             the use of this parser (e.g. ".py", or ".bas").
     *  @param[in] parser    The parser that is to be used for the
     *             given extension.
     */
    void registerParser(const char *extension,ParserInterface *parser)
    {
      m_parsers.insert(extension,parser);
    }

    /** Gets the interface to the parser associated with given \a extension, 
     *  if there is no parser explicitly registered for the supplied extension, 
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

/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <stdio.h>
#include <memory>

#include "qcstring.h"
#include "growvector.h"

class MemberDef;
class Definition;

//---------------------------------------------------------------------------

//! @brief opaque parser interface
class IDocParser
{
  public:
    virtual ~IDocParser() = default;
};

//! @brief pointer to parser interface
using IDocParserPtr = std::unique_ptr<IDocParser>;

//! @brief factory function to create a parser
IDocParserPtr createDocParser();

//---------------------------------------------------------------------------

//! @brief opaque representation of the abstract syntax tree (AST)
class IDocNodeAST
{
  public:
    virtual ~IDocNodeAST() = default;
    virtual bool isEmpty() const = 0;
};

using IDocNodeASTPtr = std::unique_ptr<IDocNodeAST>;

/*! Main entry point for the comment block parser.
 *  @param parser    The parser object created via createDocParser()
 *  @param fileName  File in which the documentation block is found (or the
 *                   name of the example file in case isExample is TRUE).
 *  @param startLine Line at which the documentation block is found.
 *  @param context   Class or namespace to which this block belongs.
 *  @param md        Member definition to which the documentation belongs.
 *                   Can be 0.
 *  @param input     String representation of the documentation block.
 *  @param indexWords Indicates whether or not words should be put in the
 *                   search index.
 *  @param isExample TRUE if the documentation belongs to an example.
 *  @param exampleName Base name of the example file (0 if isExample is FALSE).
 *  @param singleLine Output should be presented on a single line, so without
 *                   starting a new paragraph at the end.
 *  @param linkFromIndex TRUE if the documentation is generated from an
 *                   index page. In this case context is not used to determine
 *                   the relative path when making a link.
 *  @param markdownSupport TRUE if the input needs to take markdown markup into
 *                   account.
 *  @returns         An object representing the abstract syntax tree. Ownership of the
 *                   pointer is handed over to the caller.
 */
IDocNodeASTPtr validatingParseDoc(IDocParser &parser,const QCString &fileName,int startLine,
                            const Definition *context, const MemberDef *md,
                            const QCString &input,bool indexWords,
                            bool isExample,const QCString &exampleName,
                            bool singleLine,bool linkFromIndex,
                            bool markdownSupport);

/*! Main entry point for parsing simple text fragments. These
 *  fragments are limited to words, whitespace and symbols.
 */
IDocNodeASTPtr validatingParseText(IDocParser &parser,const QCString &input);

IDocNodeASTPtr createRef(IDocParser &parser,const QCString &target,const QCString &context);

//--------------------------------------------------------------------------------

/*! Searches for section and anchor commands in the input
 *  Sections found will be added to the SectionManager.
 */
void docFindSections(const QCString &input,
                     const Definition *d,
                     const QCString &fileName);


#endif

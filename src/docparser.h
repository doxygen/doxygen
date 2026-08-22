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

#include "dstring.h"
#include "construct.h"
#include "types.h"
#include "docoptions.h"

class MemberDef;
class Definition;

//---------------------------------------------------------------------------

//! @brief opaque parser interface
class IDocParser
{
  public:
    ABSTRACT_BASE_CLASS(IDocParser)
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
    ABSTRACT_BASE_CLASS(IDocNodeAST)

    virtual bool empty() const = 0;
};

using IDocNodeASTPtr = std::unique_ptr<IDocNodeAST>;


/*! Main entry point for the comment block parser.
 *  @param parserIntf The parser object created via createDocParser()
 *  @param fileName  File in which the documentation block is found (or the
 *                   name of the example file in case isExample is true).
 *  @param startLine Line at which the documentation block is found.
 *  @param ctx       Class or namespace to which this block belongs.
 *  @param md        Member definition to which the documentation belongs.
 *                   Can be 0.
 *  @param input     String representation of the documentation block.
 *  @param options   Optional parameters.
 *  @returns         An object representing the abstract syntax tree. Ownership of the
 *                   pointer is handed over to the caller.
 */
IDocNodeASTPtr validatingParseDoc(
    IDocParser &parserIntf,
    const DString &fileName,
    int startLine,
    const Definition *ctx,
    const MemberDef *md,
    const DString &input,
    const DocOptions &options);

/*! Main entry point for parsing simple text fragments. These
 *  fragments are limited to words, whitespace and symbols.
 */
IDocNodeASTPtr validatingParseText(IDocParser &parser,const DString &input);


/*! Main entry point for parsing titles. These allow limited markup commands */
IDocNodeASTPtr validatingParseTitle(IDocParser &parserIntf,const DString &fileName,int lineNr,
                                    const DString &input);


IDocNodeASTPtr createRef(IDocParser &parser,const DString &target,const DString &context, const DString &srcFile = "", int srcLine = -1);

//--------------------------------------------------------------------------------

/*! Searches for section and anchor commands in the input
 *  Sections found will be added to the SectionManager.
 */
void docFindSections(const DString &input,
                     const Definition *d,
                     const DString &fileName);

DString parseCommentAsText(const Definition *scope,const MemberDef *md,
                           const DString &doc,const DString &fileName,int lineNr);
DString parseCommentAsHtml(const Definition *scope,const MemberDef *member,
                           const DString &doc,const DString &fileName,int lineNr);

#endif

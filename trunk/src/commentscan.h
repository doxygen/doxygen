/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#ifndef COMMENTSCAN_H
#define COMMENTSCAN_H

#include "qtbc.h"
#include "entry.h"

class ParserInterface;

/** @file
 *  @brief Interface for the comment block parser */

/** Invokes the comment block parser with the request to parse a 
 *  single comment block.
 *  @param[in] parser The language parse that invoked this function.
 *         The comment block parse may invoke 
 *         ParserInterface::parsePrototype() in order to parse
 *         the argument of a @@fn command.
 *  @param[in] curEntry The Entry to which the comment block belongs.
 *         Any information (like documentation) that is found in
 *         the comment block will be stored in this entry.
 *  @param[in] comment A string representing the actual comment block.
 *         Note that leading *'s are already stripped from the comment block.
 *  @param[in] fileName The name of the file in which the comment is found.
 *         Mainly used for producing warnings.
 *  @param[in,out] lineNr The line number at which the comment block was found.
 *         When the function returns it will be set to the last line parsed.
 *  @param[in] isBrief TRUE iff this comment block represents a brief description.
 *  @param[in] isJavaDocStyle TRUE iff this comment block is in "JavaDoc" style.
 *         This means that it starts as a brief description until the end of
 *         the sentences is found and then proceeds as a detailed description.
 *  @param[in] isInbody TRUE iff this comment block is located in the body of
 *         a function.
 *  @param[in,out] prot The protection level in which this comment block was
 *         found. Commands in the comment block may override this.
 *  @param[in,out] position The character position within \a comment where the
 *         comment block starts. Typically used in case the comment block
 *         contains multiple structural commands.
 *  @param[out] newEntryNeeded Boolean that is TRUE if the comment block parser
 *         finds that a the comment block finishes the entry and a new one
 *         needs to be started.
 *  @returns TRUE if the comment requires further processing. The
 *         parameter \a newEntryNeeded will typically be true in this case and
 *         \a position will indicate the offset inside the \a comment string
 *         where to proceed parsing. FALSE indicates no further processing is
 *         needed.
 */
bool parseCommentBlock(ParserInterface *parser,
                       Entry *curEntry,
                       const QCString &comment,
	               const QCString &fileName,
		       int  &lineNr,
		       bool isBrief,
		       bool isJavaDocStyle,
                       bool isInbody,
		       Protection &prot,
                       int &position,
                       bool &newEntryNeeded
		     );

void groupEnterFile(const char *file,int line);
void groupLeaveFile(const char *file,int line);
void groupLeaveCompound(const char *file,int line,const char *name);
void groupEnterCompound(const char *file,int line,const char *name);
void openGroup(Entry *e,const char *file,int line);
void closeGroup(Entry *,const char *file,int line,bool foundInline=FALSE);
void initGroupInfo(Entry *e);


#endif

/*****************************************************************************
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

#ifndef COMMENTCNV_H
#define COMMENTCNV_H

#include <string>

/** @file
 *  @brief First pass comment processing.
 */

/** Converts the comments in a file.
 *  @param inBuf    input buffer holding the file content.
 *  @param outBuf   output buffer to which the results after conversion are written to.
 *  @param fn       the name of the file from which the comments originate.
 *
 *  The following is converted:
 *  - C++ style multiline doxygen comments are converted to C style doxygen comments.
 *  - conditional sections are processed.
 *  - aliases are expanded.
 */
void convertCppComments(const std::string &inBuf,std::string &outBuf,
                        const std::string &fn);

#endif


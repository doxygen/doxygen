/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef UTF8_H
#define UTF8_H

#include <cstdint>
#include <string>

class TextStream;

/** @file
 *  @brief Various UTF8 related helper functions.
 *
 *  See https://en.wikipedia.org/wiki/UTF-8 for details on UTF8 encoding.
 */


/** Converts the input string into a lower case version, also taking into account
 *  non-ASCII characters that has a lower case variant.
 */
std::string convertUTF8ToLower(const std::string &input);

/** Converts the input string into a upper case version, also taking into account
 *  non-ASCII characters that has a upper case variant.
 */
std::string convertUTF8ToUpper(const std::string &input);

/** Returns the UTF8 character found at byte position pos in the input string.
 *  The resulting string can be a multi byte sequence.
 */
std::string getUTF8CharAt(const std::string &input,size_t pos);

/** Returns the 32bit Unicode value matching character at byte position pos in
 *  the UTF8 encoded input.
 */
uint32_t getUnicodeForUTF8CharAt(const std::string &input,size_t pos);

/** Returns the number of bytes making up a single UTF8 character given the first byte
 *  in the sequence.
 */
uint8_t getUTF8CharNumBytes(char firstByte);

/** Writes the UTF8 character pointed to by s to stream t and returns a pointer
 *  to the next character.
 */
const char *writeUTF8Char(TextStream &t,const char *s);

/** Returns true iff the last character in input is a multibyte character. */
bool lastUTF8CharIsMultibyte(const std::string &input);

/** Returns true iff the input string at byte position pos holds an upper case character. */
bool isUTF8CharUpperCase(const std::string &input,size_t pos);

/** Check if the first character pointed at by input is a non-breakable whitespace character.
 *  Returns the byte size of the character if there is match or 0 if not.
 */
int isUTF8NonBreakableSpace(const char *input);

/** Check if the given Unicode character represents a punctuation character */
bool isUTF8PunctuationCharacter(uint32_t unicode);

#endif

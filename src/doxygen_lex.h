/******************************************************************************
 *
 * Copyright (C) 2021 by Dimitri van Heesch.
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

#ifndef DOXYGEN_LEX_H
#define DOXYGEN_LEX_H

#ifndef LEX_NO_REENTRANT
#ifndef LEX_NO_INPUT_FILENAME
#define YY_FATAL_ERROR(msg)                                          \
{                                                                    \
  QCString msg1 = msg;                                               \
  msg1 += "\n    lexical analyzer: ";                                \
  msg1 += getLexerFILE();                                            \
  if (!static_cast<yyguts_t*>(yyscanner)->yyextra_r->fileName.isEmpty()) \
  {                                                                  \
    msg1 += " (for: ";                                               \
    msg1 += static_cast<yyguts_t*>(yyscanner)->yyextra_r->fileName;  \
    msg1 += ")";                                                     \
  }                                                                  \
  msg1 += "\n";                                                      \
  yy_fatal_error( qPrint(msg1) , yyscanner);                         \
}
#else
#define YY_FATAL_ERROR(msg)                                          \
{                                                                    \
  QCString msg1 = msg;                                               \
  msg1 += "\n    lexical analyzer: ";                                \
  msg1 += getLexerFILE();                                            \
  msg1 += "\n";                                                      \
  yy_fatal_error( qPrint(msg1) , yyscanner);                         \
}
#endif
#else
#define YY_FATAL_ERROR(msg)                                          \
{                                                                    \
  QCString msg1 = msg;                                               \
  msg1 += "\n    lexical analyzer: ";                                \
  msg1 += getLexerFILE();                                            \
  msg1 += "\n";                                                      \
  yy_fatal_error( qPrint(msg1));                                     \
}
#endif

#endif

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

#ifndef _CONSTEXP_P_H
#define _CONSTEXP_P_H

#include <string>

//! @file
//! @brief Private interface between Parser (constexp.y) and Lexer (constexp.l)

#include "cppvalue.h"
#define YYSTYPE CPPValue

typedef void* yyscan_t;
struct constexpYY_state
{
  std::string  strToken;
  CPPValue     resultValue;
  int          constExpLineNr;
  std::string  constExpFileName;

  std::string inputString;
  int         inputPosition;
};
constexpYY_state* constexpYYget_extra(yyscan_t yyscanner );

extern int constexpYYlex(YYSTYPE *lvalp, yyscan_t);
extern int constexpYYparse(yyscan_t);


#endif

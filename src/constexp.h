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

#ifndef CONSTEXP_H
#define CONSTEXP_H

#include <string>
#include <memory>

#include "construct.h"

/** @brief constant expression parser used for the C preprocessor */
class ConstExpressionParser
{
  public:
    ConstExpressionParser();
   ~ConstExpressionParser();
    NON_COPYABLE(ConstExpressionParser)
    bool parse(const char *fileName,int line,const std::string &expression,const std::string &orgExpression);
  private:
    struct Private;
    std::unique_ptr<Private> p;
};

#endif

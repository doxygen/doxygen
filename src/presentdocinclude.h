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

#ifndef PRESENTSOURCECODE_H
#define PRESENTSOURCECODE_H

#include "docnode.h"
#include "dstring.h"
#include "godboltoptions.h"

class OutputCodeList;
class TextStream;

using DocIncludeParseFunction = std::function<void(OutputCodeList &ci)>;

/*! @brief generates HTML/XML to present source code requested from \snippet \example and \include commands
 */
void presentDocInclude(
  TextStream                            &t,
  OutputCodeList                        &ci,
  const DString                         &text,
  const std::optional<Godbolt::Options> &godbolt,
  const DocIncludeParseFunction         &parseCode);

#endif

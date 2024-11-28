/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#ifndef CODEFRAGMENT_H
#define CODEFRAGMENT_H

#include <memory>

#include "qcstring.h"
#include "construct.h"

class OutputCodeList;
class Definition;
class MemberDef;
class FileDef;

class CodeFragmentManager
{
  public:
    static CodeFragmentManager &instance();
    void parseCodeFragment(OutputCodeList &codeOutList,
                   const QCString &fileName,  // -> input
                   const QCString &blockId,
                   const QCString &scopeName,
                   bool showLineNumbers,
                   bool trimLeft,
                   bool stripCodeComments
                   );
  private:
    CodeFragmentManager();
    ~CodeFragmentManager();
    NON_COPYABLE(CodeFragmentManager)

    struct Private;
    std::unique_ptr<Private> p;
};

#endif

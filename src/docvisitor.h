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

#ifndef DOCVISITOR_H
#define DOCVISITOR_H

#include <memory>

#include "construct.h"

class CodeParserInterface;
class DString;

/*! @brief Helper base class for functionality shared by all visitors
 */
class DocVisitor
{
  public:
    DocVisitor();
    virtual ~DocVisitor();
    NON_COPYABLE(DocVisitor)
    CodeParserInterface &getCodeParser(const DString &langExt);
    void pushHidden(bool hide);
    bool popHidden();
  private:
    struct Private;
    std::unique_ptr<Private> m_p;
};



#endif

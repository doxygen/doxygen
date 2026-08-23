/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef LINKIFYTEXT_H
#define LINKIFYTEXT_H

#include <string_view>

#include "construct.h"

class ArgumentList;
class Definition;
class DString;
class FileDef;

/** Abstract interface for a hyperlinked text fragment. */
class TextGeneratorIntf
{
  public:
    ABSTRACT_BASE_CLASS(TextGeneratorIntf)

    virtual void writeString(std::string_view,bool) const = 0;
    virtual void writeBreak(int indent) const = 0;
    virtual void writeLink(const DString &extRef,const DString &file,
                           const DString &anchor,std::string_view text
                          ) const = 0;
};

//--------------------------------------------------------------------

struct LinkifyTextOptions
{
  public:
    // === getters for optional params
    const Definition *scope()          const { return m_scope; }
    const FileDef *fileScope()         const { return m_fileScope; }
    const Definition *self()           const { return m_self; }
    const ArgumentList *argumentList() const { return m_al; }
    bool  autoBreak()                  const { return m_autoBreak; }
    bool  external()                   const { return m_external; }
    bool  keepSpaces()                 const { return m_keepSpaces; }
    int   indentLevel()                const { return m_indentLevel; }
    size_t breakThreshold()            const { return m_breakThreshold; }

    // === setters for optional params
    LinkifyTextOptions & setScope(const Definition *scope)
    { m_scope = scope; return *this; }

    LinkifyTextOptions & setFileScope(const FileDef *fileScope)
    { m_fileScope = fileScope; return *this; }

    LinkifyTextOptions & setSelf(const Definition *self)
    { m_self = self;  return *this;}

    LinkifyTextOptions & setArgumentList(const ArgumentList *al)
    { m_al = al; return *this; }

    LinkifyTextOptions & setAutoBreak(bool autoBreak)
    { m_autoBreak = autoBreak; return *this; }

    LinkifyTextOptions & setExternal(bool external)
    { m_external = external; return *this; }

    LinkifyTextOptions & setKeepSpaces(bool keepSpaces)
    { m_keepSpaces = keepSpaces; return *this; }

    LinkifyTextOptions & setIndentLevel(int indentLevel)
    { m_indentLevel = indentLevel; return *this; }

    LinkifyTextOptions & setBreakThreshold(size_t breakThreshold)
    { m_breakThreshold = breakThreshold; return *this; }

  private:
    // optional params with defaults
    const Definition *  m_scope          = nullptr;
    const FileDef *     m_fileScope      = nullptr;
    const Definition *  m_self           = nullptr;
    const ArgumentList *m_al             = nullptr;
    bool                m_autoBreak      = false;
    bool                m_external       = true;
    bool                m_keepSpaces     = false;
    int                 m_indentLevel    = 0;
    size_t              m_breakThreshold = 30;
};

//--------------------------------------------------------------------

void linkifyText(const TextGeneratorIntf &ol,
                 const DString &text,
                 const LinkifyTextOptions &options
                );

#endif // LINKIFYTEXT_H

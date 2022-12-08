/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef DOTATTRIBUTES_H
#define DOTATTRIBUTES_H

#include <map>
#include <string>

#include "qcstring.h"

//! Class representing an attribute list of a dot graph object.
class DotAttributes
{
  public:
    //! Creates an instance of a DotAttribute list given its initial string representation
    DotAttributes(const QCString &input) : m_input(input) {}

    //! Return the string representation of the attribute list
    QCString str() const { return m_input; }

    //! update a given attribute with a new value.
    //! If the attribute is not found a new attribute will be appended.
    void updateValue(const QCString &key,const QCString &value)
    {
      // look for key\s*=
      int index = m_input.find(key);
      if (index!=-1)
      {
        const char *p = m_input.data()+index+key.length(); // make p point just past the key
        while (qisspace(*p)) p++;                          // skip whitespace
        index = (*p=='=') ? p+1-m_input.data() : -1;       // index is position after '=' if found
      }
      if (index!=-1)
      {
        size_t len      = m_input.length();
        size_t startPos = static_cast<size_t>(index);
        size_t pos      = startPos;
        while (pos<len && qisspace(m_input[pos])) pos++;
        if (pos<len && m_input[pos]=='"') // quoted value, search for end quote, ignoring escaped quotes
        {
          char pc=m_input[pos];
          pos++; // skip over start quote
          while (pos<len && (m_input[pos]!='"' || (m_input[pos]=='"' && pc=='\\'))) pc=m_input[pos++];
          if (pos<len) pos++; // skip over end quote
        }
        else // unquoted value, search for attribute separator (space,comma, or semicolon)
        {
          while (pos<len && m_input[pos]!=',' && m_input[pos]!=';' && !qisspace(m_input[pos])) pos++;
        }
        // pos is now the position after the value, so replace the part between [start..pos) with the new value
        m_input=m_input.left(startPos)+value.quoted()+m_input.mid(pos);
      }
      else // append new attribute
      {
        if (!m_input.isEmpty()) m_input+=",";
        m_input+=key+"="+value.quoted();
      }
    }

  private:
    QCString m_input;
};

#endif // DOTATTRIBUTES_H

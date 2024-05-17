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

#include "regex.h"
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
    void updateValue(const QCString &key,const QCString &inpValue)
    {
      // look for key\s*=
      const std::string regStr = key.str()+R"(\s*=)";
      const reg::Ex re { regStr };
      reg::Match match;
      std::string s = m_input.str();
      if (reg::search(s,match,re)) // replace existing attribute
      {
        size_t len      = s.length();
        size_t startPos = match.position()+match.length(); // position after =
        size_t pos      = startPos;
        while (pos<len && qisspace(s[pos])) pos++;
        if (pos<len && s[pos]=='"') // quoted value, search for end quote, ignoring escaped quotes
        {
          char pc=s[pos];
          pos++; // skip over start quote
          while (pos<len && (s[pos]!='"' || (s[pos]=='"' && pc=='\\'))) pc=s[pos++];
          if (pos<len) pos++; // skip over end quote
        }
        else // unquoted value, search for attribute separator (space,comma, or semicolon)
        {
          while (pos<len && s[pos]!=',' && s[pos]!=';' && !qisspace(s[pos])) pos++;
        }
        QCString value;
        if (inpValue.isEmpty())
        {
          value = m_input.mid(startPos,pos-startPos);
        }
        else
        {
          value = inpValue;
        }
        // pos is now the position after the value, so replace the part between [start..pos) with the new value
        m_input=m_input.left(startPos)+value.quoted()+m_input.mid(pos);
      }
      else // append new attribute
      {
        if (!inpValue.isEmpty())
        {
          if (!m_input.isEmpty()) m_input+=",";
          m_input+=key+"="+inpValue.quoted();
        }
      }
    }

  private:
    QCString m_input;
};

#endif // DOTATTRIBUTES_H

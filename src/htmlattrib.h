/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef HTMLATTRIB_H
#define HTMLATTRIB_H

#include <vector>

#include "qcstring.h"
#include "util.h"

/*! \brief Class representing a HTML attribute. */
struct HtmlAttrib
{
  HtmlAttrib(const QCString &n,const QCString &v) : name(n), value(v) {}
  QCString name;
  QCString value;
};

/*! \brief Class representing a list of HTML attributes. */
class HtmlAttribList : public std::vector<HtmlAttrib>
{
  public:
    void mergeAttribute(const QCString &optName,const QCString &optValue)
    {
      auto it = std::find_if(begin(),end(),
                           [&optName](const auto &opt) { return opt.name==optName; });
      if (it!=end()) // attribute name already in the list: append values
      {
        it->value += " " + optValue;
      }
      else // attribute name not yet in the list
      {
        emplace_back(optName,optValue);
      }
    }

    QCString toString(QCString *pAltValue = nullptr) const
    {
      QCString result;
      for (const auto &att : *this)
      {
        if (!att.value.isEmpty())  // ignore attribute without values as they
                                   // are not XHTML compliant, with the exception
                                   // of the alt attribute with the img tag
        {
          if (att.name=="alt" && pAltValue) // optionally return the value of alt separately
                                            // need to convert <img> to <object> for SVG images,
                                            // which do not support the alt attribute
          {
            *pAltValue = att.value;
          }
          else
          {
            result+=" "+att.name+"=\""+convertToXML(att.value)+"\"";
          }
        }
        else if (att.name=="open")
        {
          // The open attribute is a boolean attribute.
          // Specifies that the details should be visible (open) to the user
          // As it is a boolean attribute the initialization value is of no interest
          result+=" "+att.name+"=\"true\"";
        }
        else if (att.name=="nowrap") // In XHTML, attribute minimization is forbidden, and the nowrap attribute must be defined as <td nowrap="nowrap">.
        {
          result+=" "+att.name+"=\"nowrap\"";
        }
      }
      return result;
    }

};

#endif


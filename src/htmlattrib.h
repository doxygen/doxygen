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

#ifndef _HTMLATTRIB_H
#define _HTMLATTRIB_H

#include <qcstring.h>
#include <qlist.h>

/*! A Html option. A name, value pair */
struct HtmlAttrib
{
  QCString name;
  QCString value;
};

/*! @brief A list of Html attributes. 
 *
 * The Html attributes are deeply copied into the list.
 */
class HtmlAttribList : public QList<HtmlAttrib>
{
  public:
    HtmlAttribList() : QList<HtmlAttrib>() { setAutoDelete(TRUE); }
   ~HtmlAttribList() { clear(); }
    HtmlAttribList(const HtmlAttribList &l) : QList<HtmlAttrib>() 
    { operator=(l); }
    HtmlAttribList &operator=(const HtmlAttribList &l)
    { clear(); QList<HtmlAttrib>::operator=(l); return *this; }
    QCString find(const QCString name) const
    {
      QListIterator<HtmlAttrib> it(*this);
      QCString result;
      HtmlAttrib *attr;
      for (;(attr=it.current());++it)
      {
        if (attr->name==name) return attr->value;
      }
      return result;
    }
    QCString toString() const
    {
      QListIterator<HtmlAttrib> it(*this);
      QCString result;
      HtmlAttrib *attr;
      for (;(attr=it.current());++it)
      {
        result+=" "+attr->name+"=\""+attr->value+"\"";
      }
      return result;
    }
  private:
    HtmlAttrib *newValue( HtmlAttrib *v ) const
    { return new HtmlAttrib(*v); }
    void deleteValue(HtmlAttrib *v) const
    { delete v;  }
};

/*! @brief Html attribute list iterator */
class HtmlAttribListIterator : public QListIterator<HtmlAttrib>
{
  public:
    HtmlAttribListIterator(const HtmlAttribList &l) : QListIterator<HtmlAttrib>(l) {}
};

#endif


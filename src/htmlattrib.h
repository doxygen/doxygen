/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

/*! A Html option. A name, value pair */
struct HtmlAttrib
{
  QString name;
  QString value;
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
    QString toString() const
    {
      HtmlAttribList *that = (HtmlAttribList *)this;
      QString result;
      HtmlAttrib *attr=that->first();
      while (attr)
      {
        result+=" "+attr->name+"=\""+attr->value+"\"";
        attr=that->next();
      }
      return result;
    }
  private:
    QCollection::Item newItem( QCollection::Item d ) 
    { return (QCollection::Item)new HtmlAttrib(*(HtmlAttrib *)d); }
    void deleteItem(QCollection::Item d) 
    { delete (HtmlAttrib *)d; }
};

/*! @brief Html attribute list iterator */
class HtmlAttribListIterator : public QListIterator<HtmlAttrib>
{
  public:
    HtmlAttribListIterator(const HtmlAttribList &l) : QListIterator<HtmlAttrib>(l) {}
};

#endif


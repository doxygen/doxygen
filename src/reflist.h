/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#ifndef _REFLIST_H
#define _REFLIST_H

#include "qtbc.h"
#include <qintdict.h>
//#include "outputgen.h"

/*! This struct represents an item in the list of references. */
struct RefItem
{
  RefItem() : written(FALSE) {}
  QCString text;           //!< text of the item.
  QCString listAnchor;     //!< anchor in the list
  bool written;
};

/*! @brief List of cross-referenced items 
 * 
 *  This class represents a list of items that are put
 *  at a certain point in the documentation by some special command 
 *  and are collected in a list. The items cross-reference the 
 *  documentation and the list.
 *
 *  Examples are the todo list, the test list and the bug list,
 *  introduced by the \\todo, \\test, and \\bug commands respectively.
 */
class RefList
{
  public:
    int      addRefItem();
    RefItem *getRefItem(int todoItemId);
    RefItem *getFirstRefItem();
    RefItem *getNextRefItem();
    QCString listName() const;
    QCString pageTitle() const;
    QCString sectionTitle() const;

    RefList(const char *listName,
            const char *pageTitle,const char *secTitle
           );
   ~RefList();

  private:
    int m_id;
    QCString m_listName;
    QCString m_pageTitle;
    QCString m_secTitle;
    QIntDict<RefItem> *m_dict;
    QIntDictIterator<RefItem> *m_dictIterator;
};

#endif
